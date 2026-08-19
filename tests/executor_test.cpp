#include "executor/executor.h"

#include <cassert>
#include <optional>
#include <string>

void test_set() {
  Store store;

  Command command{.type = CommandType::Set, .key = "name", .value = "raghu"};

  std::string result = executeCommand(command, store);

  assert(result == "OK");

  auto stored = store.getValue("name");
  assert(stored.has_value());
  assert(stored.value() == "raghu");
}

void test_existing_get() {
  Store store;
  store.setValue("name", "raghu");

  Command command{
      .type = CommandType::Get, .key = "name", .value = std::nullopt};

  std::string result = executeCommand(command, store);

  assert(result == "raghu");
}

void test_missing_get() {
  Store store;

  Command command{
      .type = CommandType::Get, .key = "missing", .value = std::nullopt};

  std::string result = executeCommand(command, store);

  assert(result == "(nil)");
}

void test_existing_del() {
  Store store;
  store.setValue("name", "raghu");

  Command command{
      .type = CommandType::Del, .key = "name", .value = std::nullopt};

  std::string result = executeCommand(command, store);

  assert(result == "1");
  assert(!store.getValue("name").has_value());
}

void test_missing_del() {
  Store store;

  Command command{
      .type = CommandType::Del, .key = "missing", .value = std::nullopt};

  std::string result = executeCommand(command, store);

  assert(result == "0");
}

void test_overwrite_set() {
  Store store;

  Command first{.type = CommandType::Set, .key = "name", .value = "raghu"};

  Command second{.type = CommandType::Set, .key = "name", .value = "anurag"};

  assert(executeCommand(first, store) == "OK");
  assert(executeCommand(second, store) == "OK");

  auto stored = store.getValue("name");
  assert(stored.has_value());
  assert(stored.value() == "anurag");
}

int main() {
  test_set();
  test_existing_get();
  test_missing_get();
  test_existing_del();
  test_missing_del();
  test_overwrite_set();

  return 0;
}
