#include <protocol/serializer.hpp>

template <class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::string serializeResult(const ExecutionResult& res) {
    if (!res) {
        return std::string{"-ERR value is not an integer or out of range\r\n"};
    }

    return std::visit(
        overloaded{
            [](const SimpleString& obj) -> std::string {
                return "+" + obj.value + "\r\n";
            },

            [](const BulkString& obj) -> std::string {
                if (!obj.value) {
                    return std::string{"$-1\r\n"};
                }

                return "$" + std::to_string(obj.value->length()) + "\r\n" +
                       *obj.value + "\r\n";
            },

            [](const Integer& obj) -> std::string {
                return ":" + std::to_string(obj.value) + "\r\n";
            }
        },
        *res
    );
}

std::string serializeResult(const ParseError& err) {
    return std::visit(
        overloaded{
            [](const ProtocolError&) -> std::string {
                return "-ERR syntax error\r\n";
            },
            [](const UnknownCommand& err) -> std::string {
                return "-ERR unknown command'" + std::string{err.command} +
                       "\'\r\n";
            },
            [](const WrongArity& err) -> std::string {
                return "-ERR wrong number of arguments for '" +
                       std::string{err.command} + "\'\r\n";
            },
            [](const InvalidArgument&) -> std::string {
                return "-ERR value is not an integer or out of range\r\n";
            },
            [](const ParseIncomplete&) -> std::string {
                return "";
            }
        },
        err
    );
}
