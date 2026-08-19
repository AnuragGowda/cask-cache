#include "parser.h"

#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::vector<std::string> splitWhitespace(const std::string &input) {
  std::istringstream stream(input);
  std::vector<std::string> tokens;
  std::string token;

  while (stream >> token) {
    tokens.push_back(token);
  }

  return tokens;
}

std::optional<CommandType> commandTypeFromToken(const std::string &token) {
  if (token == "SET")
    return CommandType::Set;
  if (token == "GET")
    return CommandType::Get;
  if (token == "DEL")
    return CommandType::Del;

  return std::nullopt;
}

ParseResult invalidResult() {
  return ParseResult{.status = ParseStatus::Invalid, .command = std::nullopt};
}

ParseResult validResult(Command command) {
  return ParseResult{.status = ParseStatus::Valid, .command = command};
}
} // namespace

ParseResult parseCommand(const std::string &input) {
  const std::vector<std::string> tokens = splitWhitespace(input);

  if (tokens.empty()) {
    return invalidResult();
  }

  const std::optional<CommandType> type = commandTypeFromToken(tokens[0]);

  if (!type.has_value()) {
    return invalidResult();
  }

  switch (type.value()) {
  case CommandType::Set:
    if (tokens.size() != 3) {
      return invalidResult();
    }

    return validResult(Command{
        .type = CommandType::Set, .key = tokens[1], .value = tokens[2]});

  case CommandType::Get:
  case CommandType::Del:
    if (tokens.size() != 2) {
      return invalidResult();
    }

    return validResult(
        Command{.type = type.value(), .key = tokens[1], .value = std::nullopt});
  }

  return invalidResult();
}
