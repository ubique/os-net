#include "socks5_session.h"
#include "client_manager.h"
#include "util.hpp"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <iomanip>
#include <arpa/inet.h>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include <boost/optional.hpp>

using all_binded_t = std::unordered_map<int, int>;

using all_sockets_t = std::unordered_map<int, session>;


template<typename Container>
static inline void socket_deleter(int sock,
                                  all_sockets_t& sockets,
                                  all_binded_t& binds,
                                  int epoll_fd,
                                  Container& deleted)
{
    std::cerr << "Deleting sock" << " " << sock << std::endl;
    auto it = sockets.find(sock);
    assert(it != sockets.end());
    if (it->second.status == session_status::CONNECTED)
    {
        auto binds_iter = binds.find(it->second.dest_sock);
        assert(binds_iter != binds.end());

        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, binds_iter->first, nullptr) == -1)
        {
            std::cerr << "Epoll_ctl del bind error: " << strerror(errno) << std::endl;
        }

        deleted.insert(binds_iter->second);
        binds.erase(binds_iter);
    }
    deleted.insert(sock);
    sockets.erase(it);
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock, nullptr) == -1)
    {
        std::cerr << "Epoll_ctl del error: " << strerror(errno) << std::endl;
    }
}

void client_manager::worker(size_t const index) noexcept
{
    if (termination_marker) return;

    char                        read_buffer[read_buff_sz];
    boost::optional<int>        new_bind;
    struct epoll_event          event;
    struct epoll_event          events[epoll_max_events];
    int                         epoll_fd = epoll_create1(0);

    if (epoll_fd == -1)
    {
        std::cerr << "Failed to create epoll file descriptor: "
                  << strerror(errno) << std::endl;
        return;
    }
    auto _epoll_fd_close = defered_close(epoll_fd);

    all_sockets_t sockets;
    all_binded_t binds;

    while (!termination_marker)
    {
        std::unordered_set<int> deleted;

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

                std::cerr << "New client: " << cl_sock << std::endl;

                auto it = sockets.find(cl_sock);
                assert(it == sockets.end());
                sockets.insert(it, {cl_sock, session{cl_sock}});

                auto del_it = deleted.find(cl_sock);
                if (del_it != deleted.end())
                    deleted.erase(del_it);
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

            continue;
        }

        for (ssize_t i = 0; i < nfds; ++i)
        {
            if (termination_marker)
                return;

            int sock_fd = events[i].data.fd;
            if (deleted.find(sock_fd) != deleted.end())
                continue;

            ssize_t read_sz = recv(sock_fd, read_buffer, read_buff_sz, 0);

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
                    assert(deleted.find(sock_fd) == deleted.end() && "WTF1");
                    socket_deleter(sock_fd, sockets, binds, epoll_fd, deleted);
                    continue;
                }
                default:
                {
                    std::cerr << "recv failed: " << strerror(errno) << std::endl;
                    assert(deleted.find(sock_fd) == deleted.end() && "WTF2");
                    socket_deleter(sock_fd, sockets, binds, epoll_fd, deleted);
                    continue;
                }
                }
            }

            auto iter_binds = binds.find(sock_fd);
            if (iter_binds != binds.end())
            {
                auto iter_socks = sockets.find(iter_binds->second);
                if (iter_socks == sockets.end())
                    assert(false && "Socket erased, but bind record is alive");
                else
                {
                    if (iter_socks->second.forward_to_peer(read_buffer, static_cast<size_t>(read_sz)))
                    {
                       std::cerr << "Cannot forward to peer: " << strerror(errno) << std::endl;
                       if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_fd, nullptr) == -1)
                       {
                           std::cerr << "Epoll_ctl del error: " << strerror(errno) << std::endl;
                       }

                       if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, iter_binds->second, nullptr) == -1)
                       {
                           std::cerr << "Epoll_ctl del error: " << strerror(errno) << std::endl;
                       }


                       deleted.insert(sock_fd);
                       deleted.insert(iter_binds->second);

                       sockets.erase(iter_socks);
                       binds.erase(iter_binds);
                    }
                }

                continue;
            }

            auto it = sockets.find(sock_fd);
            assert(it != sockets.end() && "Unrecognized descriptor found");
            auto& session = it->second;
            if (session.peer_buff_sz)
                session.send_peer(session.peer_send_buff.get(), session.peer_buff_sz);

            if (it != sockets.end()
                    && session.proceed(read_buffer,
                                       static_cast<size_t>(read_sz),
                                       new_bind) != 0)
            {
                assert(deleted.find(sock_fd) == deleted.end());
                socket_deleter(sock_fd, sockets, binds, epoll_fd, deleted);
            }
            else if (new_bind)
            {
                binds[*new_bind] = sock_fd;
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = *new_bind;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, *new_bind, &event))
                {
                    std::cerr << "Failed to add file descriptor to epoll: "
                              << strerror(errno) << std::endl;
                    continue;
                }
            }
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
        while (!worker_clients.empty())
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
