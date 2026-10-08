#include "store.hpp"

#include <charconv>
#include <chrono>
#include <cstdint>
#include <string>
#include <system_error>

namespace {

    template <class... Ts> struct overloaded : Ts... {
        using Ts::operator()...;
    };

    template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

} // namespace

Store::DataMap::iterator Store::findValue(std::string_view key) {
    auto it = data_.find(key);

    if (it != data_.end() &&
        it->second.exp_time <= std::chrono::steady_clock::now()) {
        data_.erase(it);
        return data_.end();
    }

    return it;
}

ExecutionResult Store::executeCommand(Command cmd) {
    return std::visit(
        overloaded{
            [](PingCommand) -> ExecutionResult {
                return SimpleString{"PONG"};
            },

            [this](SetCommand cmd) -> ExecutionResult {
                std::string key{cmd.key};

                data_[key] = Data{
                    .value = std::string{cmd.value},
                    .exp_time = std::chrono::steady_clock::time_point::max()
                };

                return SimpleString{"OK"};
            },

            [this](GetCommand cmd) -> ExecutionResult {
                auto it = findValue(cmd.key);

                if (it == data_.end()) {
                    return BulkString{std::nullopt};
                }

                return BulkString{it->second.value};
            },

            [this](DelCommand cmd) -> ExecutionResult {
                auto it = findValue(cmd.key);

                if (it == data_.end()) {
                    return Integer{0};
                }

                data_.erase(it);

                return Integer{1};
            },

            [this](IncrCommand cmd) -> ExecutionResult {
                auto it = findValue(cmd.key);

                if (it == data_.end()) {
                    data_[std::string{cmd.key}] = Data{
                        .value = "1",
                        .exp_time = std::chrono::steady_clock::time_point::max()
                    };

                    return Integer{1};
                }

                std::int64_t value;

                const std::string& str = it->second.value;

                auto [ptr, ec] =
                    std::from_chars(str.data(), str.data() + str.size(), value);

                if (ec != std::errc{} || ptr != str.data() + str.size() ||
                    value == INT64_MAX) {
                    return std::unexpected{DataError::NotInteger};
                }

                ++value;
                it->second.value = std::to_string(value);

                return Integer{value};
            },

            [this](ExpireCommand cmd) -> ExecutionResult {
                auto it = findValue(cmd.key);

                if (it == data_.end()) {
                    return Integer{0};
                }

                it->second.exp_time = std::chrono::steady_clock::now() +
                                      std::chrono::seconds{cmd.seconds};

                return Integer{1};
            },

            [this](TTLCommand cmd) -> ExecutionResult {
                auto it = findValue(cmd.key);

                if (it == data_.end()) {
                    return Integer{-2};
                }

                if (it->second.exp_time ==
                    std::chrono::steady_clock::time_point::max()) {
                    return Integer{-1};
                }

                const auto remaining =
                    it->second.exp_time - std::chrono::steady_clock::now();

                const auto seconds =
                    std::chrono::duration_cast<std::chrono::seconds>(remaining)
                        .count();

                return Integer{seconds};
            }
        },
        cmd
    );
}
