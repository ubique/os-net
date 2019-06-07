#include "socks5_session.h"

#include <iostream>
#include <cstring>

int session::proceed(const char *buff,
                     size_t sz,
                     boost::optional<int> &new_bind)
{
    new_bind.reset();

    switch (status)
    {
    case session_status::NOT_AUTHED:
        return not_authed_handler(buff, sz);
    case session_status::AUTH_STATUS_INCOMPLETE:
        return auth_incomplete_handler(buff, sz);
    case session_status::AUTH_STATUS_APPROVED:
        return auth_status_approved_handler(buff, sz);
    case session_status::AUTHED:
        return authed_handler(buff, sz, new_bind);
    case session_status::CONNECTED:
        return send_bind(buff, sz);
    default:
        return -1;
    }

    return 0;
}

int session::forward_to_peer(const char* buff, size_t sz)
{
    return send_peer(buff, sz);
}

int session::convert_domain_to_ip4(char* buff)
{
    struct hostent *he;
    if ((he = gethostbyname(buff + 1)) == nullptr)
        return -1;

    auto addr_list = reinterpret_cast<in_addr **>(he->h_addr_list);
    for (size_t i = 0; addr_list[i] != nullptr; ++i)
    {
        *reinterpret_cast<uint32_t*>(buff) = addr_list[i]->s_addr;
        return 0;
    }

    return -1;
}

void session::send_cmd_reply(char repl,
                             const void* dest_addr,
                             size_t len)
{
    static char mesg[22];
    mesg[0] = 0x05; // socks5
    mesg[1] = repl;
    mesg[2] = 0x00; // rsv
    memcpy(mesg + 3, dest_addr, len);

    send_peer(mesg, len + 3);
}

int session::send_peer(char const* ptr, size_t sz) noexcept
{
    if (peer_buff_sz != 0 && append_peer_buff(ptr, sz))
        return -1;

    if (sz || peer_buff_sz)
    {
        const char* buff;
        size_t buff_sz;
        if (peer_buff_sz)
        {
            buff = peer_send_buff.get();
            buff_sz = peer_buff_sz;
        }
        else
        {
            buff = ptr;
            buff_sz = sz;
        }

        ssize_t _sent = send(sock_fd, buff, buff_sz, MSG_DONTWAIT | MSG_NOSIGNAL);
        if (_sent < 0)
        {
            if (errno == EAGAIN)
                return 0;

            return -1;
        }
        else
        {
            auto sent = static_cast<size_t>(_sent);
            memmove(peer_send_buff.get(), buff + sent, buff_sz - sent);
            peer_buff_sz = buff_sz - sent;
        }
    }

    return 0;
}

int session::send_bind(const char *ptr, size_t sz) noexcept
{
    if (bind_buff_sz != 0 && append_bind_buff(ptr, sz))
        return -1;

    if (sz || bind_buff_sz)
    {
        const char* buff;
        size_t buff_sz;
        if (bind_buff_sz)
        {
            buff = bind_send_buff.get();
            buff_sz = bind_buff_sz;
        }
        else
        {
            buff = ptr;
            buff_sz = sz;
        }

        ssize_t _sent = send(dest_sock, buff, buff_sz, MSG_DONTWAIT | MSG_NOSIGNAL);
        if (_sent < 0)
        {
            if (errno == EAGAIN)
                return 0;

            return -1;
        }
        else
        {
            auto sent = static_cast<size_t>(_sent);
            memmove(bind_send_buff.get(), buff + sent, buff_sz - sent);
            bind_buff_sz = buff_sz - sent;
        }
    }

    return 0;
}

int session::not_authed_handler(const char *buff, size_t sz)
{
    if (sz < 2 || 2 + static_cast<size_t>(buff[1]) > sz)
    {
        status = session_status::AUTH_STATUS_INCOMPLETE;
        memcpy(req_buff + data_sz, buff, sz);
        data_sz += sz;
        return 0;
    }

    if (buff[0] != 0x05)
        return -1;

    bool found = false;
    for (ssize_t i = 0; !found && i < buff[1]; ++i)
    {
        if (buff[i + 2] == 0x02) // username/pass
            found = true;
    }

    if (!found)
        return -1;

    req_buff[0] = 0x05;
    req_buff[1] = 0x02;
    auto sent = send_peer(req_buff, 2);
    if (sent)
        return -1;

    status = session_status::AUTH_STATUS_APPROVED;
    data_sz = 0;
    return 0;
}

int session::auth_incomplete_handler(const char *buff, size_t sz)
{
    memcpy(req_buff + data_sz, buff, sz);
    data_sz += sz;
    if (data_sz < 2
            || 2 + static_cast<size_t>(buff[1]) > data_sz)
        return 0;

    if (buff[0] != 0x05)
        return -1;

    bool found = false;
    for (ssize_t i = 0; !found && i < buff[1]; ++i)
    {
        if (buff[i + 2] == 0x02) // username/pass
            found = true;
    }

    if (!found)
        return -1;

    req_buff[0] = 0x05;
    req_buff[1] = 0x02;
    auto sent = send_peer(req_buff, 2);
    if (sent)
        return -1;
    status = session_status::AUTH_STATUS_APPROVED;
    data_sz = 0;
    return 0;
}

int session::auth_status_approved_handler(const char *buff, size_t sz)
{
    memcpy(req_buff + data_sz, buff, sz);
    data_sz += sz;

    if (data_sz < 3) return 0;
    if (buff[0] != 0x01)
        return -1;

    size_t ulen = static_cast<size_t>(buff[1]);
    if (data_sz < 2 + ulen) return 0;

    size_t plen = static_cast<size_t>(buff[ulen + 2]);
    if (data_sz < 3 + ulen + plen) return 0;

    std::string username(buff, 2, ulen);
    std::string passwd(buff, 3 + ulen, plen);

    if (username != client_manager::AUTH_CREDENTIALS.first
            || passwd != client_manager::AUTH_CREDENTIALS.second)
    {
        req_buff[0] = 0x01;
        req_buff[1] = 0x01; // error

        auto _sent = send_peer(req_buff, 2);
        return -1;
    }

    req_buff[0] = 0x01;
    req_buff[1] = 0x00; // error
    auto _sent = send_peer(req_buff, 2);
    if (_sent)
        return -1;

    data_sz = 0;
    status = session_status::AUTHED;

    return 0;
}

int session::authed_handler(const char *buff,
                            size_t sz,
                            boost::optional<int>& new_bind)
{
    memcpy(req_buff + data_sz, buff, sz);
    data_sz += sz;

    if (data_sz < 6) return 0;

    size_t msg_len = 6;
    switch (buff[3])
    {
    case 0x01:          // IPv4
        msg_len += 4;
        break;
    case 0x03:          // Domain
        msg_len += static_cast<size_t>(buff[4]);
        break;
    case 0x04:
        msg_len += 16;
        break;
    default:
        std::cerr << "Unexpected addressing" << std::endl;
        return -1;
    }

    if (data_sz < msg_len)
        return 0;

    /*
     *
     *  MESSAGE FULLY RECEIVED
     *
     */

    if (buff[0] != 0x05)
        return -1;

    bnd_address.sin_port = *reinterpret_cast<uint16_t*>(req_buff + (msg_len - 2));
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len = sizeof peer_addr;
    uint16_t peer_port = 0;

    if (getpeername(sock_fd,
                    reinterpret_cast<struct sockaddr*>(&peer_addr),
                    &peer_addr_len) != 0)
    {
        std::cerr << "getpeername error: " << strerror(errno) << std::endl;
        return -1;
    } else
    {
        switch (peer_addr.ss_family)
        {
        case AF_INET:
            peer_port = reinterpret_cast<struct sockaddr_in*>(&peer_addr)->sin_port;
            break;
        case AF_INET6:
            peer_port = reinterpret_cast<struct sockaddr_in6*>(&peer_addr)->sin6_port;
            break;
        default:
            std::cerr << "Unexpected AF: " << peer_addr.ss_family << std::endl;
            return -1;
        }
    }

    req_buff[msg_len - 2] = 0;

    if (buff[3] == 0x03)
    {
        if (convert_domain_to_ip4(req_buff + 4))
        {
            return -1;
        }

        req_buff[3] = 0x01;
    }

    assert(peer_port != 0);
    *reinterpret_cast<uint16_t*>(req_buff + (msg_len - 2)) = peer_port;

    switch (buff[3])
    {
    case 0x01:
        //                AF_INET
        bnd_address.sin_family = AF_INET;
        break;
    case 0x04:
        //              AF_INET6;
        send_cmd_reply(0x08, buff + 3, msg_len - 3);
        return -1;
    case 0x03:
        //                Domain
        assert(false && "Impossible state");
        return -1;
    default:
        std::cerr << "Unexpected addressing" << std::endl;
        return -1;
    }

    switch (buff[1]) // CMD
    {
    case 0x01:               // Connect
    {
        bnd_address.sin_addr.s_addr = *reinterpret_cast<uint32_t*>(req_buff + 4);

        dest_sock = socket(bnd_address.sin_family, SOCK_STREAM, 0);
        if (dest_sock == -1)
        {
            std::cerr << "Error creating destination socket: "
                      << strerror(errno) << std::endl;
            return -1;
        }
        _bnd_sock_df.set_fd(dest_sock);

        if (connect(dest_sock,
                    reinterpret_cast<struct sockaddr*>(&bnd_address),
                    sizeof(bnd_address)))
        {
            std::cerr << "Error connecting via tcp: " << strerror(errno) << std::endl;
            send_cmd_reply(0x05, buff + 3, msg_len - 3);
            return -1;
        }
        else
        {
            send_cmd_reply(0x00, buff + 3, msg_len - 3);
            status = session_status::CONNECTED;
            new_bind = dest_sock;
            return 0;
        }
    }
    case 0x02:               // Bind
        send_cmd_reply(0x07, buff + 3, msg_len - 3);
        return -1;
//         ...
//        break;
    case 0x03:               // UDP_ASSOC
        send_cmd_reply(0x07, buff + 3, msg_len - 3);
        std::cerr << "UDP not implemented" << std::endl;
        //                add_udp_assoc();
        return -1;
    default:
        std::cerr << "Unexpected command: " << buff[1] << std::endl;
        return -1;
    }

    return 0;
}

int session::append_bind_buff(const char *buff, size_t sz) noexcept
{
    if (bind_buff_sz + sz > client_manager::session_buff_sz)
        return -1;
    memcpy(bind_send_buff.get() + bind_buff_sz, buff, sz);
    bind_buff_sz += sz;
    return 0;
}

int session::append_peer_buff(const char *buff, size_t sz) noexcept
{
    if (peer_buff_sz + sz > client_manager::session_buff_sz)
        return -1;
    memcpy(peer_send_buff.get() + peer_buff_sz, buff, sz);
    peer_buff_sz += sz;
    return 0;
}
