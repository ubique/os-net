#include "utils.h"

fd_wrapper::fd_wrapper(int fd) : fd(fd) {}

bool fd_wrapper::bad() { return fd == -1; }

int const& fd_wrapper::value() { return fd; }

fd_wrapper::~fd_wrapper() { close(fd); }

void accumulator::insert(std::string const& n) { data += n; }

std::pair<int, std::string> accumulator::read_fully(const int& fd,
                                                    const size_t BUFF_SIZE) {
    int result = 0;
    std::string res;
    while (true) {
        std::string cur;
        res = next();
        if (!res.empty()) {
            break;
        }
        std::vector<char> buffer(BUFF_SIZE);
        ssize_t was_read = recv(fd, buffer.data(), BUFF_SIZE, 0);
        if (was_read == -1) {
            return {-1, ""};
        }
        result += was_read;
        buffer.resize(static_cast<size_t>(was_read));
        buffer.push_back('\0');
        cur = std::string(buffer.data());
        if (cur.empty()) {
            break;
        }
        insert(cur);
    }
    return {result, res};
}

std::string accumulator::next() {
    size_t end = data.find("\r\n");
    if (end == std::string::npos) {
        return "";
    }
    std::string res = data.substr(0, end);
    data = data.substr(end + 2);
    return res;
}
