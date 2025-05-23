#pragma once

#include <sys/select.h>

#include "Socket.h"

class SelectWrapper {
  public:
    SelectWrapper() {
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);
        max_fd = -1;
    }

    void addReadFd(int fd) {
        FD_SET(fd, &read_fds);
        if (fd > max_fd) {
            max_fd = fd;
        }
    }

    void addWriteFd(int fd) {
        FD_SET(fd, &write_fds);
        if (fd > max_fd) {
            max_fd = fd;
        }
    }

    void addExceptFd(int fd) {
        FD_SET(fd, &except_fds);
        if (fd > max_fd) {
            max_fd = fd;
        }
    }

    void removeReadFd(int fd) { FD_CLR(fd, &read_fds); }

    void removeWriteFd(int fd) { FD_CLR(fd, &write_fds); }

    void removeExceptFd(int fd) { FD_CLR(fd, &except_fds); }

    int select(int timeout_sec = 0, int timeout_usec = 0) {
        fd_set read_fds_copy = read_fds;
        fd_set write_fds_copy = write_fds;
        fd_set except_fds_copy = except_fds;

        timeval timeout;
        timeout.tv_sec = timeout_sec;
        timeout.tv_usec = timeout_usec;

        return ::select(
            max_fd + 1, &read_fds_copy, &write_fds_copy, &except_fds_copy,
            (timeout_sec == 0 && timeout_usec == 0) ? nullptr : &timeout);
    }

    bool isReadable(int fd) const { return FD_ISSET(fd, &read_fds); }

    bool isWritable(int fd) const { return FD_ISSET(fd, &write_fds); }

    bool hasException(int fd) const { return FD_ISSET(fd, &except_fds); }

  private:
    fd_set read_fds;
    fd_set write_fds;
    fd_set except_fds;
    int max_fd;
};