#include "executor.h"
#include <cassert>

std::string executeCommand(const Command &command, Store &store) {
  switch (command.type) {
  case CommandType::Set: {
    assert(command.value.has_value());

    store.setValue(command.key, command.value.value());
    return "OK";
  }

  case CommandType::Get: {
    auto res = store.getValue(command.key);

    if (res.has_value()) {
      return res.value();
    }

    return "(nil)";
  }

  case CommandType::Del: {
    return store.deleteKey(command.key) ? "1" : "0";
  }
  }

  return "ERR";
}
