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
    TCPSocket socket(Server);
    int value = 1;
    socket.setSocketOption(REUSEADDR | REUSEPORT, value);
    socket.bindSocket(PORT);
    socket.listenSocket(3);
    std::cout << "Server is listening on port " << PORT << '\n';
    int clntId = socket.acceptConnection();
    std::cout << "Client connected with ID: " << clntId << '\n';

    char buffer[BUFFER_SIZE] = {0};
    socket.receiveData(buffer, BUFFER_SIZE);
    std::cout << "Message from client: " << buffer << '\n';

    const char *response = "Hello from server";
    socket.sendData(response, strlen(response));
    return 0;
}