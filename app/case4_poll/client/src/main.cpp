#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "Socket.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    TCPSocket socket{Client};
    socket.connectToServer("127.0.0.1", PORT);

    char sendLine[BUFFER_SIZE];
    char recvline[BUFFER_SIZE + 1];
    while (fgets(sendLine, BUFFER_SIZE, stdin) != nullptr) {
        socket.sendData(sendLine, strlen(sendLine));
        socket.sendData("\n", 1);
        socket.receiveData(recvline, strlen(recvline));
        recvline[strlen(recvline)] = '\0';
        fputs(recvline, stdout);
    }
    return 0;
}
