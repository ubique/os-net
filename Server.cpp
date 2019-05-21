#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <stdio.h>

#include <netinet/in.h> // sockaddr_in -- Internet Socket Address
#include <cstring>
#include <unistd.h>

#include <sstream>
#include <iostream>

#include "Server.h"

struct ClientSocketClosed: std::runtime_error {
    ClientSocketClosed(std::string cause);
};


Server::Server(uint16_t port) : port(port),
        sfd(socket(AF_INET, SOCK_STREAM, 0 /* or IPPROTO_TCP*/)) {
    if (sfd == -1) {
        throw ServerException("Socket creation failed");
    }
    struct sockaddr_in socket_addr;

    memset(&socket_addr, 0, sizeof(struct sockaddr_in));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = INADDR_ANY;
    socket_addr.sin_port = htons(port);

    if (bind(sfd, (struct sockaddr*) &socket_addr, sizeof(sockaddr_in)) == -1) {
        close_fd(sfd);
        throw ServerException("Bind failed");
    }

    if (listen(sfd, LISTEN_BACKLOG) == -1) {
        close_fd(sfd);
        throw ServerException("Listen failed");
    }

    requestType.insert({"QUIT", Token::QUIT});
    requestType.insert({"GROUP", Token::GROUP});
    requestType.insert({"LISTGROUP", Token::LISTGROUP});
    requestType.insert({"POST", Token::POST});
    requestType.insert({"ARTICLE", Token::ARTICLE});
}

Server::~Server() {
    close(sfd);
}

void Server::run() {
    while (true) {
        new_client:
        struct sockaddr_in client_addr;
        socklen_t client_addr_size;
        int cfd = accept(sfd, (struct sockaddr *) &client_addr, &client_addr_size); // podumoi
        if (cfd == -1) {
            continue;
        }

        sendGreeting(cfd);

        while (true) {
            if (!readSingleLineRequest(cfd)) {
                break;
            }
            if (!requestType.count(requestTypeName)) {
                unsupportedOperation(cfd);
                continue;
            }
            std::cout << requestTypeName << std::endl;
            Token token = requestType.find(requestTypeName)->second;
            switch (token) {
                case Token::QUIT:
                    quit(cfd);
                    goto new_client;
                case Token::POST:
                    post(cfd);
                    break;
                case Token::GROUP:
                    group(cfd);
                    break;
                case Token::LISTGROUP:
                    listGroup(cfd);
                    break;
                case Token::ARTICLE:
                    article(cfd);
                    break;
                default:
                    unsupportedOperation(cfd);
            }
        }
    }
}



void Server::close_fd(int fd) {
    if (close(fd) == -1) {
        perror("File descriptor was not closed");
    }
}

void Server::sendMessage(std::string const &msg, int fd) {
    std::cout << "[S] " << msg << std::endl;

    if (send(fd, msg.data(), msg.length(), 0) == -1) {
        perror("Client didn't get the message :'(");
    }
}

void Server::sendGreeting(int fd) {
    sendMessage("200\tNNTP Service Ready, posting permitted\r\n", fd);

}

bool Server::readSingleLineRequest(int cfd) {
    ssize_t read = recv(cfd, textBuffer, BUFFER_SIZE, 0);
    if (read == -1) {
        perror("Reading failed");
        return false;
    }
    size_t len = read;
    if (textBuffer[read - 1] == '\n') {
        len--;
    }
    if (textBuffer[read - 1] == '\r') {
        len--;
    }
    std::cout << "[C] hellARTI" << std::string(textBuffer, len) << std::endl;
    processSingleLineRequest(std::string(textBuffer, len)); // request must ends with \r\n
    return true;
}

void Server::processSingleLineRequest(std::string const &request) {
    std::istringstream stream(request);
    stream >> requestTypeName;
    requestArgument.clear();
    if (stream) {
        stream >> requestArgument;
    }
}


void Server::quit(int cfd) {
    sendMessage("205\tclosing connection\r\n", cfd);
    close_fd(cfd);
}

void Server::group(int fd) {
    if (!groups.count(requestArgument)) {
        sendMessage("411\tNo such newsgroup\r\n", fd);
        selectedGroup.clear();
        return;
    }
    selectedGroup = requestArgument;
    currentArticle = 1;
    sendMessage(getCurrentGroupRepresentation(), fd);
}

void Server::listGroup(int fd) {
    if (!requestArgument.empty()) {
        selectedGroup = requestArgument;
    } else if (selectedGroup.empty()) {
        sendMessage("412\tNo newsgroup selected\r\n", fd);
        return;
    }
     if (!groups.count(requestArgument)) {
        sendMessage("411\tNo such newsgroup\r\n", fd);
        selectedGroup.clear();
        return;
    }
     sendMessage(getCurrentGroupRepresentation() + getCurrentGroupArticlesId(), fd);
}

std::string Server::getCurrentGroupRepresentation() {
    auto& articlesID = groups.find(selectedGroup)->second;
    std::stringstream answer;

    answer << "221 " << articlesID.size() << " ";
    answer << articlesID.front() << " ";
    answer << articlesID.back() << " ";
    answer << requestArgument << "\r\n";
    return answer.str();
}

std::string Server::getCurrentGroupArticlesId() {
    auto& articlesID = groups.find(selectedGroup)->second;
    std::stringstream answer;

    for (auto id : articlesID) {
        answer << id << "\r\n";
    }
    answer << ".\r\n";
    return answer.str();
}

void Server::unsupportedOperation(int fd) {
    std::cout << requestTypeName << std::endl;
    sendMessage("500\tUnknown command\r\n", fd);
}

void Server::post(int cfd) {
    sendMessage("340\tInput article; end with <CR-LF>.<CR-LF>\r\n", cfd);
    ssize_t read = recv(cfd, articleBuffer, ARTICLE_SIZE, 0);
    if (read == -1) {
        sendMessage("441\tPosting failed\r\n", cfd);
        return;
    }
    std::string articleText(articleBuffer, read);
    articles.push_back(articleText);
    const size_t id = articles.size();
    size_t groupsPos = articleText.find("Newsgroups:");
    if (groupsPos != std::string::npos) {
        size_t endGroupsPos = articleText.find("\r\n", groupsPos);
        std::stringstream groupsList(articleText.substr(groupsPos + 11, endGroupsPos - groupsPos - 11));
        std::string group;
        while (groupsList >> group) {
            auto it = groups.find(group);
            if (it == groups.end()) {
                groups.insert({group, {id}});
            } else {
                it->second.push_back(id);
            }
            std::cout << "Article " << id << " added to group " << group << std::endl;
        }
    }
    sendMessage("240\tArticle received OK\r\n", cfd);
}

void Server::article(int fd) {
    size_t id = 0;
    if (requestArgument.empty()) {
        if (!selectedGroup.empty()) {
            id = groups.find(selectedGroup)->second[currentArticle - 1];
        } else {
            sendMessage("412\tNo newsgroup selected\r\n", fd);
            return;
        }
    } else {
        id = atoi(requestArgument.data());
        if (id == 0 || id > articles.size()) {
            sendMessage("430\tNo article with that message-id\r\n", fd);
            return;
        }
    }
    std::stringstream ans;
    ans << "202\t" << id << "\r\n";
    ans << articles[id - 1];
    sendMessage(ans.str(), fd);
}