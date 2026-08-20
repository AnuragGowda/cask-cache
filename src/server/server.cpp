#include "server.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

Server::Server(int port) : _socket{::socket(AF_INET, SOCK_STREAM, 0)} {

  if (_socket.fd() < 0) {
    throw std::runtime_error("Socket Failed");
  }

  // Set opt for early rebind
  int opt = 1;
  if (::setsockopt(_socket.fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
      0) {
    throw std::runtime_error("Sockopt Failed");
  }

  // Bind to IP/Port
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (::bind(_socket.fd(), reinterpret_cast<sockaddr *>(&server_addr),
             sizeof(server_addr)) < 0) {
    throw std::runtime_error("Bind Failed");
  }

  // Listen
  if (::listen(_socket.fd(), SOMAXCONN) < 0) {
    throw std::runtime_error("Listen failed");
  }
  std::cout << "Listening on port " << port << "\n";
}

void Server::run() {
  while (true) {

    // Build pollfds from _client arr
    std::vector<pollfd> pollfds;
    pollfds.reserve(_clients.size() + 1);
    pollfds.push_back({.fd = _socket.fd(), .events = POLLIN, .revents = 0});
    for (const auto &client : _clients) {
      pollfds.push_back(
          {.fd = client.socket.fd(), .events = client.events, .revents = 0});
    }

    int res = ::poll(pollfds.data(), pollfds.size(), -1);
    if (res < 0) {
      if (errno == EINTR) {
        continue;
      }
      throw std::runtime_error("poll failed");
    }

    // Check if existing connections ready
    for (size_t i = _clients.size(); i-- > 0;) {
      const short revents = pollfds[i + 1].revents;
      bool remove_client = false;
      if (revents == 0) {
        continue;
      }
      if (revents & (POLLIN | POLLOUT)) {
        if (IoResult::remove == handle_message(_clients[i], revents & POLLIN, revents & POLLOUT)){
          remove_client = true;
        }
      }
      if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
        remove_client = true;
      }
      if (remove_client){
        _clients.erase(_clients.begin() + i);
      }
    }

    // Check to see if new client connection
    if (pollfds[0].revents & POLLIN) {
      sockaddr_in client_addr{};
      socklen_t client_len = sizeof(client_addr);
      Socket client_fd{accept4(_socket.fd(),
                               reinterpret_cast<sockaddr *>(&client_addr),
                               &client_len, SOCK_NONBLOCK | SOCK_CLOEXEC)};
      if (client_fd.fd() < 0) {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN){
          continue;
        }
        throw std::runtime_error("Accept failed");
      }
      _clients.push_back(
          Client{.socket = std::move(client_fd), .input = "", .output = ""});
    }
  }
}

IoResult Server::handle_message(Client &client, bool input_ready,
                                bool output_ready) {

  if (output_ready) {
    size_t bytes_written = 0;
    while (bytes_written < client.output.size()) {
      ssize_t ret =
          ::send(client.socket.fd(), client.output.data() + bytes_written,
                 client.output.size() - bytes_written, MSG_NOSIGNAL);
      if (ret == -1) {
        if (errno == EINTR) {
          continue;
        }
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
          break;
        }
        return IoResult::remove;
      } else {
        bytes_written += static_cast<size_t>(ret);
      }
    }
    client.output.erase(0, bytes_written);
    if (client.output.empty()) {
      client.events &= static_cast<short>(~POLLOUT);
    }
  }

  if (input_ready) {
    std::vector<char> buffer(1024);

    while (true) {
      std::fill(buffer.begin(), buffer.end(), 0);
      ssize_t ret = ::recv(client.socket.fd(), buffer.data(), buffer.size(), 0);
      if (ret == -1) {
        if (errno == EINTR) {
          continue;
        }
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
          break;
        }
        return IoResult::remove;
      }
      if (ret == 0) {
        return IoResult::remove;
      }
      if (ret > 0) {
        client.input.append(buffer.data(), ret);
      }
    }

    std::string output_data = client.input;
    if (output_data.size() > 0) {
      client.output.append(output_data);
      client.input.clear();
      client.events |= POLLOUT;
    }
  }
  return IoResult::ok;
}
