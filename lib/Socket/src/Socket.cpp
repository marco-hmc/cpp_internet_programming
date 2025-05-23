#include "Socket.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <cstddef>

int add(int a, int b) { return a - b; }

/* ----------------------------------------------------------------------- */
Socket::Socket(SocketType type) : m_type(type) {}

void Socket::bindSocket(int port) {
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_addr.s_addr = INADDR_ANY;
    m_serverAddr.sin_port = htons(port);
    if (bind(m_sockfd, reinterpret_cast<struct sockaddr*>(&m_serverAddr),
             sizeof(m_serverAddr)) < 0) {
        spdlog::error("Bind failed");
        throw std::runtime_error("Bind failed");
    }
}

void Socket::setSocketOption(SocketOptions option, int value) const {
    int optval = value;

    if (option & REUSEADDR) {
        setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    }
    if (option & REUSEPORT) {
        setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    }
    if (option & KEEPALIVE) {
        setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
    }
    if (option & LINGER) {
        struct linger so_linger {};
        so_linger.l_onoff = 1;
        so_linger.l_linger = value;
        setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER, &so_linger,
                   sizeof(so_linger));
    }
    if (option & RCVBUF) {
        setsockopt(m_sockfd, SOL_SOCKET, SO_RCVBUF, &optval, sizeof(optval));
    }
    if (option & SNDBUF) {
        setsockopt(m_sockfd, SOL_SOCKET, SO_SNDBUF, &optval, sizeof(optval));
    }
}

/* ----------------------------------------------------------------------- */

TCPSocket::TCPSocket(SocketType type) : Socket(type), m_clientSockfd(-1) {
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        spdlog::error("Socket creation failed");
        throw std::runtime_error("Socket creation failed");
    }
}

TCPSocket::~TCPSocket() {
    if (m_sockfd >= 0) {
        close(m_sockfd);
    }
    if (m_clientSockfd >= 0) {
        close(m_clientSockfd);
    }
}

void TCPSocket::connectToServer(const char* ip, int port) {
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &m_serverAddr.sin_addr) <= 0) {
        spdlog::error("Invalid address");
        throw std::runtime_error("Invalid address");
    }
    if (connect(m_sockfd, reinterpret_cast<struct sockaddr*>(&m_serverAddr),
                sizeof(m_serverAddr)) < 0) {
        spdlog::error("Connect failed");
        throw std::runtime_error("Connect failed");
    }
}

void TCPSocket::sendData(const char* data, size_t size) {
    int sockfd = (m_type == Server) ? m_clientSockfd : m_sockfd;
    if (send(sockfd, data, size, 0) < 0) {
        spdlog::error("Send failed");
        throw std::runtime_error("Send failed");
    }
}

void TCPSocket::receiveData(char* buffer, size_t size) {
    int sockfd = (m_type == Server) ? m_clientSockfd : m_sockfd;
    if (recv(sockfd, buffer, size, 0) < 0) {
        spdlog::error("Recv failed");
        throw std::runtime_error("Recv failed");
    }
}

void TCPSocket::listenSocket(int backlog) const {
    if (listen(m_sockfd, backlog) < 0) {
        spdlog::error("Listen failed");
        throw std::runtime_error("Listen failed");
    }
}

int TCPSocket::acceptConnection() {
    m_addrLen = sizeof(m_clientAddr);
    m_clientSockfd =
        accept4(m_sockfd, reinterpret_cast<struct sockaddr*>(&m_clientAddr),
                &m_addrLen, SOCK_CLOEXEC);
    if (m_clientSockfd < 0) {
        spdlog::error("Accept failed");
        throw std::runtime_error("Accept failed");
    }
    return m_clientSockfd;
}

void TCPSocket::closeSocket() {
    if (m_sockfd >= 0) {
        close(m_sockfd);
        m_sockfd = -1;
    }
    if (m_clientSockfd >= 0) {
        close(m_clientSockfd);
        m_clientSockfd = -1;
    }
}

/* ----------------------------------------------------------------------- */

UDPSocket::UDPSocket(SocketType type) : Socket(type) {
    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sockfd < 0) {
        spdlog::error("Socket creation failed");
        throw std::runtime_error("Socket creation failed");
    }
}

UDPSocket::~UDPSocket() {
    if (m_sockfd >= 0) {
        close(m_sockfd);
    }
}

void UDPSocket::sendData(const char* data, size_t size) {
    if (sendto(m_sockfd, data, size, 0,
               reinterpret_cast<struct sockaddr*>(&m_serverAddr),
               sizeof(m_serverAddr)) < 0) {
        spdlog::error("Send failed");
        throw std::runtime_error("Send failed");
    }
}

void UDPSocket::receiveData(char* buffer, size_t size) {
    m_addrLen = sizeof(m_clientAddr);
    if (recvfrom(m_sockfd, buffer, size, 0,
                 reinterpret_cast<struct sockaddr*>(&m_clientAddr),
                 &m_addrLen) < 0) {
        spdlog::error("Recv failed");
        throw std::runtime_error("Recv failed");
    }
}
