#pragma once

#include <netinet/in.h>

#include <bitset>
#include <cstddef>

#include "utils.h"

enum SocketType { Server, Client };
enum SocketState { LISTENING, CONNECTED, DISCONNECTED };
enum SocketError {
    NO_ERROR,
    BIND_ERROR,
    LISTEN_ERROR,
    ACCEPT_ERROR,
    CONNECT_ERROR,
    SEND_ERROR,
    RECEIVE_ERROR
};

enum SocketOption {
    REUSEADDR = 1 << 0,
    KEEPALIVE = 1 << 1,
    LINGER = 1 << 2,
    RCVBUF = 1 << 3,
    SNDBUF = 1 << 4,
    REUSEPORT = 1 << 5,
};

using SocketOptions = int;

class Socket {
  public:
    explicit Socket(SocketType type);

    ~Socket() = default;
    DISABLE_COPY_AND_ASSIGN(Socket);

    void bindSocket(int port);
    int getSocketFd() const { return m_sockfd; }
    void setSocketOption(SocketOptions option, int value) const;
    virtual void sendData(const char* data, size_t size) = 0;
    virtual void receiveData(char* buffer, size_t size) = 0;

  protected:
    const SocketType m_type;
    int m_sockfd{-1};
    struct sockaddr_in m_serverAddr {};
    struct sockaddr_in m_clientAddr {};
    socklen_t m_addrLen{};
};

class TCPSocket : public Socket {
  public:
    explicit TCPSocket(SocketType type);
    ~TCPSocket();
    DISABLE_COPY_AND_ASSIGN(TCPSocket);

    void connectToServer(const char* ip, int port);
    void sendData(const char* data, size_t size) override;
    void receiveData(char* buffer, size_t size) override;

    void listenSocket(int backlog) const;
    int acceptConnection();
    void closeSocket();

  private:
    int m_clientSockfd;
    socklen_t m_addrLen{};
};

class UDPSocket : public Socket {
  public:
    explicit UDPSocket(SocketType type);
    ~UDPSocket();
    DISABLE_COPY_AND_ASSIGN(UDPSocket);

    void sendData(const char* data, size_t size) override;
    void receiveData(char* buffer, size_t size) override;

  private:
    int m_sockfd;
};