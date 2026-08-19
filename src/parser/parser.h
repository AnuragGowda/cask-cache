#pragma once

#include <optional>
#include <string>

enum class ParseStatus {
  Valid, 
  Invalid
};

enum class CommandType {
  Set,
  Get,
  Del
};

struct Command {
  CommandType type;
  std::string key;
  std::optional<std::string> value;
};

struct ParseResult {
  ParseStatus status;
  std::optional<Command> command;
};

ParseResult parseCommand(const std::string& input);
