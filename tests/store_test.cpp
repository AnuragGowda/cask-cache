#include "store/store.h"

#include <cassert>
#include <optional>
#include <string>

void test_get_missing_key() {
  Store store;

  auto result = store.getValue("missing");

  assert(!result.has_value());
}

void test_set_then_get() {
  Store store;

  store.setValue("name", "test");

  auto result = store.getValue("name");

  assert(result.has_value());
  assert(result.value() == "test");
}

void test_overwrite_existing_key() {
  Store store;

  store.setValue("name", "raghu");
  store.setValue("name", "anurag");

  auto result = store.getValue("name");

  assert(result.has_value());
  assert(result.value() == "anurag");
}

void test_delete_existing_key() {
  Store store;

  store.setValue("name", "raghu");

  bool deleted = store.deleteKey("name");
  auto result = store.getValue("name");

  assert(deleted);
  assert(!result.has_value());
}

void test_delete_missing_key() {
  Store store;

  bool deleted = store.deleteKey("missing");

  assert(!deleted);
}

int main() {
  test_get_missing_key();
  test_set_then_get();
  test_overwrite_existing_key();
  test_delete_existing_key();
  test_delete_missing_key();

  return 0;
}
