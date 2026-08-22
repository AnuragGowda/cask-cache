#include "parser.hpp"
#include <cctype>
#include <charconv>
#include <optional>
#include <span>
#include <utility>
#include <variant>
#include <vector>

namespace {

    std::optional<unsigned int> parseInt(std::string_view view) {
        unsigned int val;
        auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), val);
        if (ec != std::errc{} || ptr != view.data() + view.size()) {
            return std::nullopt;
        }
        return val;
    }

    template <typename T> struct Parsed {
        T value;
        std::size_t bytes_consumed;
    };

    using PartialError = std::variant<ParseIncomplete, ProtocolError>;

    template <typename T> using PartialParseResult = std::expected<Parsed<T>, PartialError>;

    PartialParseResult<std::size_t> parseLength(std::string_view view, char prefix) {
        std::size_t current_byte{0}, value{0};
        if (view.empty()) {
            return std::unexpected(ParseIncomplete{});
        }
        if (view[current_byte] != prefix) {
            return std::unexpected(ProtocolError{});
        }
        current_byte++;
        while (current_byte < view.size() && std::isdigit(static_cast<unsigned char>(view[current_byte]))) {
            value = value * 10 + static_cast<int>(view[current_byte] - '0');
            current_byte++;
        }
        if (current_byte >= view.size()) {
            return std::unexpected(ParseIncomplete{});
        } else if (view[current_byte] != '\r') {
            return std::unexpected(ProtocolError{});
        } else if (current_byte + 1 >= view.size()) {
            return std::unexpected(ParseIncomplete{});
        } else if (view[current_byte + 1] != '\n') {
            return std::unexpected(ProtocolError{});
        }
        current_byte += 2;
        return Parsed<std::size_t>{.value = value, .bytes_consumed = current_byte};
    }

    PartialParseResult<std::string_view> parseBulkString(std::string_view view) {
        PartialParseResult<std::size_t> res = parseLength(view, '$');
        if (!res) {
            return std::unexpected{std::move(res).error()};
        }
        if (res->bytes_consumed + res->value + 2 > view.size()) {
            return std::unexpected(ParseIncomplete{});
        }
        if (view.substr(res->bytes_consumed + res->value, 2) != "\r\n") {
            return std::unexpected(ProtocolError{});
        }
        return Parsed<std::string_view>{.value = view.substr(res->bytes_consumed, res->value),
                                        .bytes_consumed = res->bytes_consumed + res->value + 2};
    }

    PartialParseResult<std::vector<std::string_view>> parseRespArr(std::string_view view) {
        PartialParseResult<std::size_t> res = parseLength(view, '*');
        if (!res) {
            return std::unexpected{std::move(res).error()};
        }
        std::vector<std::string_view> resp_arr;
        resp_arr.reserve(res->value);
        std::size_t current_byte = res->bytes_consumed;
        for (std::size_t i = 0; i < res->value; i++) {
            PartialParseResult<std::string_view> string_res = parseBulkString(view.substr(current_byte));
            if (!string_res) {
                return std::unexpected{std::move(string_res).error()};
            }
            resp_arr.push_back(string_res->value);
            current_byte += string_res->bytes_consumed;
        }
        return Parsed<std::vector<std::string_view>>{.value = std::move(resp_arr), .bytes_consumed = current_byte};
    }

    using CommandError = std::variant<UnknownCommand, WrongArity, InvalidArgument>;

    std::expected<Command, CommandError> decodeCommand(std::span<const std::string_view> bulk_strings) {

        if (bulk_strings[0] == "ping") {
            if (bulk_strings.size() != 1) {
                return std::unexpected{WrongArity{.expected = 1, .actual = bulk_strings.size()}};
            }
            return PingCommand{};

        } else if (bulk_strings[0] == "set") {
            if (bulk_strings.size() != 3) {
                return std::unexpected{WrongArity{.expected = 3, .actual = bulk_strings.size()}};
            }
            return SetCommand{.key = bulk_strings[1], .value = bulk_strings[2]};

        } else if (bulk_strings[0] == "get") {
            if (bulk_strings.size() != 2) {
                return std::unexpected{WrongArity{.expected = 2, .actual = bulk_strings.size()}};
            }
            return GetCommand{.key = bulk_strings[1]};

        } else if (bulk_strings[0] == "del") {
            if (bulk_strings.size() != 2) {
                return std::unexpected{WrongArity{.expected = 2, .actual = bulk_strings.size()}};
            }
            return DelCommand{.key = bulk_strings[1]};
        } else if (bulk_strings[0] == "incr") {
            if (bulk_strings.size() != 2) {
                return std::unexpected{WrongArity{.expected = 2, .actual = bulk_strings.size()}};
            }
            return IncrCommand{.key = bulk_strings[1]};

        } else if (bulk_strings[0] == "expire") {
            if (bulk_strings.size() != 3) {
                return std::unexpected{WrongArity{.expected = 3, .actual = bulk_strings.size()}};
            }
            std::optional<unsigned int> seconds = parseInt(bulk_strings[2]);
            if (seconds) {
                return ExpireCommand{.key = bulk_strings[1], .seconds = seconds.value()};
            }
            return std::unexpected{InvalidArgument{.index = 2}};
        } else if (bulk_strings[0] == "ttl") {
            if (bulk_strings.size() != 2) {
                return std::unexpected{WrongArity{.expected = 2, .actual = bulk_strings.size()}};
            }
            return TTLCommand{.key = bulk_strings[1]};
        } else {
            return std::unexpected{UnknownCommand{bulk_strings[0]}};
        }
    }
} // namespace

std::expected<ParseSuccess, ParseError> parseCommand(std::string_view view) {
    PartialParseResult<std::vector<std::string_view>> resp_result = parseRespArr(view);
    if (!resp_result) {
        ParseError error = std::visit([](const auto& err) -> ParseError { return err; }, resp_result.error());
        return std::unexpected{error};
    }

    std::expected<Command, CommandError> decode_res = decodeCommand(resp_result->value);
    if (!decode_res) {
        ParseError error = std::visit([](const auto& err) -> ParseError { return err; }, decode_res.error());
        return std::unexpected{error};
    }

    return ParseSuccess{.command = *decode_res, .bytes_consumed = resp_result->bytes_consumed};
}
