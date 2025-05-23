#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "Socket.h"
#include "select.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    TCPSocket socket(Server);
    SelectWrapper selector;
    socket.bindSocket(PORT);

    selector.addReadFd(socket.getSocketFd());

    int result = selector.select(5, 0);
    if (result == -1) {
        std::cerr << "Select error" << std::endl;
    } else if (result > 0) {
        if (selector.isReadable(socket.getSocketFd())) {
            std::cout << "Socket is readable" << std::endl;

            char buffer[1024];
            ssize_t bytes_read =
                recv(socket.getSocketFd(), buffer, sizeof(buffer), 0);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                std::cout << "Received: " << buffer << std::endl;
            }
        }
    } else {
        std::cout << "Select timeout" << std::endl;
    }
    close(socket.getSocketFd());
    return 0;
}