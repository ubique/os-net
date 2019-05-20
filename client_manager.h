#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <vector>
#include <signal.h>
#include <condition_variable>

struct client_manager
{
    using sock_desc = int;

    constexpr static size_t epoll_max_events = 512;
    constexpr static size_t read_buffer_sz = 4096 * 4;
    constexpr static size_t session_data_start_payload = 256;
    constexpr static size_t max_reject_num = 5;

    client_manager(client_manager const&) = delete;
    client_manager(client_manager&&) = delete;

    client_manager& operator=(client_manager const&) = delete;
    client_manager& operator=(client_manager&&) = delete;

    explicit client_manager(sig_atomic_t volatile& termination_marker);

    void add_client(int sock_desc);

    ~client_manager() noexcept;

private:
    void worker(size_t index) noexcept;
    void terminate_workers() noexcept;
    void close_socks() noexcept;

private:
    sig_atomic_t volatile& termination_marker;
    size_t const threads;
    std::vector<std::thread> workers;

    std::mutex mut;
    std::condition_variable cond_var;
    std::vector<std::queue<sock_desc>> new_clients_by_thread;
};
