#include <netinet/ip.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>

#include "Socket.h"

const int SERV_PORT = 6000;
const int MAX_LINE = 2048;

int main() {
    TCPSocket socket{Server};
    socket.bindSocket(SERV_PORT);
    socket.listenSocket(3);
    socket.setSocketOption(REUSEADDR | REUSEPORT, 1);

    char msg[MAX_LINE];
    while (true) {
        socket.receiveData(msg, MAX_LINE);
        std::cout << "Received: " << msg << std::endl;
        socket.sendData(msg, strlen(msg));
        std::cout << "Sent: " << msg << std::endl;
    }
}
