#include "parser.hpp"
#include <cctype>
#include <optional>
#include <span>
#include <variant>
#include <vector>

namespace {
    template <typename T> struct Parsed {
        T value;
        size_t bytes_consumed;
    };

    template <typename T> using PartialParseResult = std::variant<Parsed<T>, ParseIncomplete, ProtocolError>;

    PartialParseResult<size_t> parseLength(std::string_view view, char prefix) {
        size_t current_byte{0}, value{0};
        if (view.empty()) {
            return ParseIncomplete{};
        }
        if (view.at(current_byte) != prefix) {
            return ProtocolError{};
        }
        current_byte++;
        while (current_byte < view.size() && std::isdigit(static_cast<unsigned int>(view[current_byte]))) {
            value = value * 10 + static_cast<int>(view.at(current_byte) - '0');
            current_byte++;
        }
        if (current_byte >= view.size()) {
            return ParseIncomplete{};
        } else if (view[current_byte] != '\r') {
            return ProtocolError{};
        } else if (current_byte + 1 >= view.size()) {
            return ParseIncomplete{};
        } else if (view[current_byte + 1] != '\n') {
            return ProtocolError{};
        }
        current_byte += 2;
        return Parsed<size_t>{.value = value, .bytes_consumed = current_byte};
    }

    PartialParseResult<std::string_view> parseBulkString(std::string_view view) {
        PartialParseResult<size_t> res = parseLength(view, '$');
        if (std::holds_alternative<ParseIncomplete>(res)) {
            return ParseIncomplete{};
        } else if (std::holds_alternative<ProtocolError>(res)) {
            return ProtocolError{};
        }
        Parsed<size_t> parsed_len = std::get<Parsed<size_t>>(res);
        if (parsed_len.bytes_consumed + parsed_len.value + 2 > view.size()) {
            return ParseIncomplete{};
        }
        if (view.substr(parsed_len.bytes_consumed + parsed_len.value, 2) != "\r\n") {
            return ProtocolError{};
        }
        return Parsed<std::string_view>{.value = view.substr(parsed_len.bytes_consumed, parsed_len.value),
                                        .bytes_consumed = parsed_len.bytes_consumed + parsed_len.value + 2};
    }

    PartialParseResult<std::vector<std::string_view>> parseRespArr(std::string_view view) {
        PartialParseResult<size_t> res = parseLength(view, '*');
        if (std::holds_alternative<ParseIncomplete>(res)) {
            return ParseIncomplete{};
        } else if (std::holds_alternative<ProtocolError>(res)) {
            return ProtocolError{};
        }
        Parsed<size_t> parsed_len = std::get<Parsed<size_t>>(res);
        std::vector<std::string_view> resp_arr;
        resp_arr.reserve(parsed_len.value);
        size_t current_byte = parsed_len.bytes_consumed;
        for (size_t i = 0; i < parsed_len.value; i++) {
            if (current_byte >= view.size()) {
                return ProtocolError{};
            }
            PartialParseResult<std::string_view> string_res = parseBulkString(view.substr(current_byte));
            if (std::holds_alternative<ParseIncomplete>(string_res)) {
                return ParseIncomplete{};
            } else if (std::holds_alternative<ProtocolError>(string_res)) {
                return ProtocolError{};
            }
            Parsed<std::string_view> bulk_string = std::get<Parsed<std::string_view>>(string_res);
            resp_arr.push_back(bulk_string.value);
            current_byte += bulk_string.bytes_consumed;
        }
        return Parsed<std::vector<std::string_view>>{.value = resp_arr, .bytes_consumed = current_byte};
    }

    std::optional<Command> decodeCommand(std::span<const std::string_view> bulk_strings) {

        if (bulk_strings[0] == "ping") {
            if (bulk_strings.size() != 1) {
                return std::nullopt;
            }
            return PingCommand{};

        } else if (bulk_strings[0] == "set") {
            if (bulk_strings.size() != 3) {
                return std::nullopt;
            }
            return SetCommand{.key = bulk_strings[1], .value = bulk_strings[2]};

        } else if (bulk_strings[0] == "get") {
            if (bulk_strings.size() != 2) {
                return std::nullopt;
            }
            return GetCommand{.key = bulk_strings[1]};

        } else if (bulk_strings[0] == "del") {
            if (bulk_strings.size() != 2) {
                return std::nullopt;
            }
            return DelCommand{.key = bulk_strings[1]};
        } else if (bulk_strings[0] == "incr") {
            if (bulk_strings.size() != 2) {
                return std::nullopt;
            }
            return IncrCommand{.key = bulk_strings[1]};

        } else if (bulk_strings[0] == "expire") {
            if (bulk_strings.size() != 3) {
                return std::nullopt;
            }
            unsigned int value = 0;
            auto [ptr, ec] =
                std::from_chars(bulk_strings[2].data(), bulk_strings[2].data() + bulk_strings[2].size(), value);
            if (ec == std::errc()) {
                return ExpireCommand{.key = bulk_strings[1], .seconds = value};
            }
            return std::nullopt;
        } else if (bulk_strings[0] == "ttl") {
            if (bulk_strings.size() != 2) {
                return std::nullopt;
            }
            return TTLCommand{.key = bulk_strings[1]};
        } else {
            return std::nullopt;
        }
    }
} // namespace

ParseResult parseCommand(std::string_view view) {
    PartialParseResult<std::vector<std::string_view>> resp_result{parseRespArr(view)};
    if (std::holds_alternative<ParseIncomplete>(resp_result)) {
        return ParseIncomplete{};
    } else if (std::holds_alternative<ProtocolError>(resp_result)) {
        return ProtocolError{};
    }

    Parsed<std::vector<std::string_view>> resp_arr = std::get<Parsed<std::vector<std::string_view>>>(resp_result);
    std::optional<Command> decode_res{decodeCommand(resp_arr.value)};
    if (!decode_res.has_value()) {
        return CommandError{};
    }

    return ParseSuccess{.command = decode_res.value(), .bytes_consumed = resp_arr.bytes_consumed};
}
