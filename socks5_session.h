#pragma once

#include "client_manager.h"
#include "util.hpp"

#include <sys/socket.h>
#include <netdb.h>
#include <boost/optional.hpp>

enum class session_status
{
    NOT_AUTHED,
    AUTH_STATUS_INCOMPLETE,
    AUTH_STATUS_APPROVED,
    AUTHED,
    CONNECTED,
    BINDED,
    UDP_ASSOC
};

/*
 * bnd_ - binded (destination)
 * peer - client
 * host - current machine
 */
struct session
{
    int                     sock_fd;
    int                     dest_sock;
    defered_close           _client_sock_df;
    defered_close           _bnd_sock_df;

    session_status          status;
    size_t                  data_sz;
    size_t                  peer_buff_sz;
    size_t                  bind_buff_sz;
    struct sockaddr_in      bnd_address;
    char                    req_buff[client_manager::session_buff_sz];
    std::unique_ptr<char[]> peer_send_buff;
    std::unique_ptr<char[]> bind_send_buff;

    explicit session(int sock_fd)
        : sock_fd(sock_fd),
          _client_sock_df(sock_fd),
          _bnd_sock_df(-1),
          status(session_status::NOT_AUTHED),
          data_sz(0),
          peer_buff_sz(0),
          bind_buff_sz(0),
          peer_send_buff(std::make_unique<char[]>(client_manager::session_buff_sz)),
          bind_send_buff(std::make_unique<char[]>(client_manager::session_buff_sz))
    {}

    int proceed(char const* buff,
                size_t sz,
                boost::optional<int>& new_bind);

    int forward_to_peer(char const* buff,
                        size_t sz);

    int convert_domain_to_ip4(char* buff);

    void send_cmd_reply(char repl,
                        void const* dest_addr,
                        size_t len);

    int send_peer(char const* ptr, size_t sz) noexcept;
    int send_bind(char const* ptr, size_t sz) noexcept;

    int not_authed_handler(char const* buff,
                           size_t sz);

    int auth_incomplete_handler(char const* buff,
                                size_t sz);

    int auth_status_approved_handler(char const* buff,
                                     size_t sz);

    int authed_handler(char const* buff,
                       size_t sz,
                       boost::optional<int>& new_bind);

private:
    int append_bind_buff(char const* buff, size_t sz) noexcept;
    int append_peer_buff(char const* buff, size_t sz) noexcept;
};
