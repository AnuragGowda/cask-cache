#pragma once

#include <protocol/parser.hpp>
#include <store/store.hpp>
#include <string>

std::string serializeResult(const ExecutionResult& res);
std::string serializeResult(const ParseError& err);
