#pragma once

#include "parser/parser.h"
#include "store/store.h"

std::string executeCommand(const Command &command, Store &store);
