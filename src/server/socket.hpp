class Socket {
private:
  int _fd;

public:
  explicit Socket(int fd);
  ~Socket();

  Socket(const Socket &other) = delete;
  Socket &operator=(const Socket &other) = delete;

  Socket(Socket &&other) noexcept;
  Socket &operator=(Socket &&other) noexcept;

  [[nodiscard]] int fd() const noexcept;
};
