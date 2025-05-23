#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "Socket.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    TCPSocket socket(Client);
    socket.connectToServer("127.0.0.1", PORT);
    const char *message = "Hello from client";
    socket.sendData(message, strlen(message));
    std::cout << "Message sent to server" << '\n';
    char buffer[BUFFER_SIZE] = {0};
    socket.receiveData(buffer, BUFFER_SIZE);
    std::cout << "Message from server: " << buffer << '\n';

    return 0;
}