#pragma once

#include <chrono>
#include <expected>
#include <functional>
#include <protocol/parser.hpp>
#include <string>
#include <unordered_map>
#include <variant>

struct SimpleString {
    std::string value;
};

struct BulkString {
    std::optional<std::string> value;
};

struct Integer {
    std::int64_t value;
};

using ExecuteSuccess = std::variant<SimpleString, BulkString, Integer>;

enum class DataError { NotInteger };

using ExecutionResult = std::expected<ExecuteSuccess, DataError>;

struct Data {
    std::string value;
    std::chrono::steady_clock::time_point exp_time;
};

struct DataHash {
    using is_transparent = void;
    std::size_t operator()(std::string_view value) const noexcept {
        return std::hash<std::string_view>{}(value);
    }
};

class Store {
  private:
    using DataMap =
        std::unordered_map<std::string, Data, DataHash, std::equal_to<>>;
    DataMap data_;
    DataMap::iterator findValue(std::string_view key);

  public:
    Store() = default;
    ExecutionResult executeCommand(Command cmd);
};
