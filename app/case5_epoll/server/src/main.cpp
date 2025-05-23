#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "select.h"

int main() {
    SelectWrapper selector;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sockfd, reinterpret_cast<sockaddr*>(&server_addr),
                sizeof(server_addr)) == -1) {
        std::cerr << "Failed to connect to server" << std::endl;
        close(sockfd);
        return 1;
    }

    selector.addReadFd(sockfd);

    int result = selector.select(5, 0);
    if (result == -1) {
        std::cerr << "Select error" << std::endl;
    } else if (result > 0) {
        if (selector.isReadable(sockfd)) {
            std::cout << "Socket is readable" << std::endl;

            char buffer[1024];
            ssize_t bytes_read = recv(sockfd, buffer, sizeof(buffer), 0);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                std::cout << "Received: " << buffer << std::endl;
            }
        }
    } else {
        std::cout << "Select timeout" << std::endl;
    }

    close(sockfd);

    return 0;
}