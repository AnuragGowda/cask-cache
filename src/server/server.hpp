#include "socket.hpp"
#include <poll.h>
#include <vector>
#include <string>

struct Client {
  Socket socket;
  std::string input;
  std::string output;
  short events = POLLIN;
};

enum class IoResult { remove, ok };

class Server {
private:
  Socket _socket;
  std::vector<Client> _clients;
  IoResult handle_message(Client &client, bool input_ready, bool output_ready);

public:
  explicit Server(int port);
  void run();
};
