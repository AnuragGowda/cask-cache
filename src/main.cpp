#include "server/server.hpp"
#include <cstdlib>

int SERVER_PORT = 8080;

int main() {

  Server server{SERVER_PORT};
  server.run();
  return EXIT_SUCCESS;
}
