#include "socket.hpp"
#include <unistd.h>

Socket::Socket(int fd) : _fd{fd} {};
Socket::~Socket() {
  if (_fd != -1) {
    ::close(_fd);
  }
}

Socket::Socket(Socket &&other) noexcept : _fd{other._fd} { other._fd = -1; }

Socket &Socket::operator=(Socket &&other) noexcept {
  if (this != &other) {
    if (_fd != -1) {
      ::close(_fd);
    }
    _fd = other._fd;
    other._fd = -1;
  }
  return *this;
}

int Socket::fd() const noexcept { return _fd; }
