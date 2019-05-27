#ifndef OS_NET_SERVER_H
#define OS_NET_SERVER_H

#include <errno.h>
#include <string.h>

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

struct ServerException: std::runtime_error {
    ServerException(std::string const& msg) :
                    std::runtime_error(msg+ ", " + strerror(errno)) {}
};


struct Server {
    explicit Server(uint16_t port);
    ~Server();

    void run();

private:
    void closeFileDescriptor(int fd);

    void sendMessage(std::string const& msg, int fd);
    void sendGreeting(int fd);

    bool readSingleLineRequest(int cfd);
    void processSingleLineRequest(std::string const& request);

    void quit(int cfd);
    void group(int fd);
    void listGroup(int fd);
    void unsupportedOperation(int fd);
    void post(int cfd);
    void article(int fd);

    std::string getCurrentGroupRepresentation();
    std::string getCurrentGroupArticlesId();

    const uint16_t port;
    const int sfd;

    bool clientDone;

    static const int LISTEN_BACKLOG = 50;
    static const size_t BUFFER_SIZE = 256;
    static const size_t ARTICLE_SIZE = 4096;

    char textBuffer[BUFFER_SIZE];
    char articleBuffer[ARTICLE_SIZE];

    std::vector<std::string> articles;
    std::unordered_map<std::string, std::vector<size_t>> groups;

    enum class Token {
        QUIT, GROUP, LISTGROUP, POST, ARTICLE
    };

    std::unordered_map<std::string, Token> requestType;

    std::string requestTypeName;
    std::string requestArgument;

    std::string selectedGroup;
    size_t currentArticle;
};


#endif //OS_NET_SERVER_H
