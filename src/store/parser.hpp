#include <cstddef>
#include <expected>
#include <string_view>
#include <variant>

struct PingCommand {};
struct SetCommand {
    std::string_view key;
    std::string_view value;
};
struct GetCommand {
    std::string_view key;
};
struct DelCommand {
    std::string_view key;
};
struct IncrCommand {
    std::string_view key;
};
struct ExpireCommand {
    std::string_view key;
    unsigned int seconds;
};
struct TTLCommand {
    std::string_view key;
};

using Command = std::variant<PingCommand, SetCommand, GetCommand, DelCommand, IncrCommand, ExpireCommand, TTLCommand>;

struct ParseSuccess {
    Command command;
    size_t bytes_consumed;
};
struct ParseIncomplete {};

struct ProtocolError {
    std::string_view message;
};

struct UnknownCommand {
    std::string_view command;
};

struct WrongArity {
    std::size_t expected;
    std::size_t actual;
};

struct InvalidArgument {
    std::size_t index;
};

using ParseError = std::variant<ParseIncomplete, ProtocolError, UnknownCommand, WrongArity, InvalidArgument>;

std::expected<ParseSuccess, ParseError> parseCommand(std::string_view view);
