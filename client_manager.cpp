#include "client_manager.h"
#include "util.hpp"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <iomanip>
#include <iostream>
#include <unordered_map>

enum class session_status
{
    NOT_AUTHED,
    AUTH_PROC,
    AUTHED,
    BINDED,
    REJECTED
};

struct session
{
    int             sock_fd;
    defered_close   _df;
    size_t          rejected;
    session_status  status;
    char            data[client_manager::session_data_start_payload];

    explicit session(int sock_fd)
        : sock_fd(sock_fd),
          _df(sock_fd),
          rejected(0),
          status(session_status::NOT_AUTHED)
    {}
};

static inline int proceed_session(session& session,
                                  char const* buff,
                                  ssize_t sz)
{
    switch (session.status)
    {
    case session_status::NOT_AUTHED:
    {
        if (2 + buff[1] > sz)
        {
            session.status = session_status::AUTH_PROC;
            memcpy(session.data, buff, static_cast<size_t>(sz));
            return 0;
        }

        bool found = false;
        for (ssize_t i = 0; !found && i < buff[1]; ++i)
        {
            if (buff[i + 2] == 0x02) // username/pass
                found = true;
        }

        if (!found)
            return -1;

        session.data[0] = 0x05;
        session.data[1] = 0x02;
        send(session.sock_fd, session.data, 2, MSG_DONTWAIT);
        return 0;
    }
    case session_status::AUTH_PROC:
    case session_status::BINDED:
    case session_status::REJECTED:
    default:
        return -1;
    }
}

void client_manager::worker(size_t const index) noexcept
{
    std::unordered_map<sock_desc, session> sockets;

    if (termination_marker) return;

    char read_buffer[read_buffer_sz];

    struct epoll_event event, events[epoll_max_events];
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        std::cerr << "Failed to create epoll file descriptor: "
                  << strerror(errno) << std::endl;
        return;
    }
    auto _epoll_fd_close = defered_close(epoll_fd);

    while (true)
    {
        if (sockets.empty())
        {
            std::unique_lock<std::mutex> lck(mut);
            cond_var.wait(lck,
                          [this, index] ()
                          {
                            return !new_clients_by_thread[index].empty() || termination_marker;
                          }
            );

            if (termination_marker)
                return;
        }

        {
            std::lock_guard<std::mutex> lck(mut);
            while (!new_clients_by_thread[index].empty())
            {
                auto cl_sock = new_clients_by_thread[index].front();
                new_clients_by_thread[index].pop();

                event.events = EPOLLIN | EPOLLET;
                event.data.fd = cl_sock;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cl_sock, &event) == -1)
                {
                    std::cerr << "Failed to add file descriptor to epoll: "
                              << strerror(errno) << std::endl;
                    continue;
                }

                auto it = sockets.find(cl_sock);
                assert(it == sockets.end());
                sockets.insert(it, {cl_sock, session{cl_sock}});
            }
        }

        if (termination_marker)
            return;

        auto nfds = epoll_wait(epoll_fd,
                               events,
                               epoll_max_events,
                               1000);

        if (termination_marker)
            return;

        if (nfds == -1)
        {
            std::cerr << "Failed to epoll_wait: "
                      << strerror(errno) << std::endl;

            termination_marker = 1;
            return;
        }

        for (ssize_t i = 0; i < nfds; ++i)
        {
            if (termination_marker)
                return;

            int sock_fd = events[i].data.fd;

            ssize_t read_sz = recv(sock_fd, read_buffer, read_buffer_sz, 0);
            if (read_sz == -1)
            {
                switch (errno)
                {
                case EAGAIN:
                    std::cerr << "epoll_wait event gave sock_fd which is empty!" << std::endl;
                    continue;
                case ECONNRESET:
                case ETIMEDOUT:
                {
                    std::cerr << "Connection closed " << sock_fd << std::endl;
                    auto it = sockets.find(sock_fd);
                    assert(it != sockets.end());
                    sockets.erase(it);
                    continue;
                }
                default:
                {
                    std::cerr << "recv failed: " << strerror(errno) << std::endl;
                    auto it = sockets.find(sock_fd);
                    assert(it != sockets.end());
                    sockets.erase(it);
                    continue;
                }
                }
            }

            proceed_session(sockets[sock_fd], read_buffer, read_sz);
        }
    }
}

void client_manager::terminate_workers() noexcept
{
    termination_marker = 1;
    cond_var.notify_all();
    for (auto&& th : workers)
        th.join();
}

void client_manager::close_socks() noexcept
{
    for (auto&& worker_clients : new_clients_by_thread)
    {
        while (worker_clients.empty())
        {
            auto x = worker_clients.front();
            worker_clients.pop();
            close(x);
        }
    }
}

client_manager::client_manager(volatile sig_atomic_t &termination_marker)
    : termination_marker(termination_marker),
      threads(std::thread::hardware_concurrency())
{
    try
    {
        while (workers.size() < threads)
        {
            workers.emplace_back([this, ind = workers.size()] { worker(ind); });
            new_clients_by_thread.emplace_back();
        }
    }
    catch (...)
    {
        terminate_workers();
    }
}

client_manager::~client_manager() noexcept
{
    terminate_workers();
    close_socks();
}

void client_manager::add_client(sock_desc sock)
{
    static size_t counter = 0;

    std::lock_guard<std::mutex> lck(mut);
    new_clients_by_thread[(counter++) % threads].push(sock);
    cond_var.notify_all();
}
