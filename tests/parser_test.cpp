#include "parser/parser.h"

#include <cassert>

void test_valid_set() {
  ParseResult res = parseCommand("SET key value");

  assert(res.status == ParseStatus::Valid);
  assert(res.command.has_value());
  assert(res.command->type == CommandType::Set);
  assert(res.command->key == "key");
  assert(res.command->value.has_value());
  assert(res.command->value.value() == "value");
}

void test_valid_get() {
  ParseResult res = parseCommand("GET key");

  assert(res.status == ParseStatus::Valid);
  assert(res.command.has_value());
  assert(res.command->type == CommandType::Get);
  assert(res.command->key == "key");
  assert(!res.command->value.has_value());
}

void test_valid_delete() {
  ParseResult res = parseCommand("DEL key");

  assert(res.status == ParseStatus::Valid);
  assert(res.command.has_value());
  assert(res.command->type == CommandType::Del);
  assert(res.command->key == "key");
  assert(!res.command->value.has_value());
}

void test_invalid_command() {
  ParseResult res = parseCommand("RANDOM key value");

  assert(res.status == ParseStatus::Invalid);
  assert(!res.command.has_value());
}

void test_too_many_args() {
  ParseResult get_res = parseCommand("GET key extra");
  assert(get_res.status == ParseStatus::Invalid);
  assert(!get_res.command.has_value());

  ParseResult del_res = parseCommand("DEL key extra");
  assert(del_res.status == ParseStatus::Invalid);
  assert(!del_res.command.has_value());

  ParseResult set_res = parseCommand("SET key value extra");
  assert(set_res.status == ParseStatus::Invalid);
  assert(!set_res.command.has_value());
}

void test_too_few_args() {
  ParseResult empty_res = parseCommand("");
  assert(empty_res.status == ParseStatus::Invalid);
  assert(!empty_res.command.has_value());

  ParseResult get_res = parseCommand("GET");
  assert(get_res.status == ParseStatus::Invalid);
  assert(!get_res.command.has_value());

  ParseResult del_res = parseCommand("DEL");
  assert(del_res.status == ParseStatus::Invalid);
  assert(!del_res.command.has_value());

  ParseResult set_res = parseCommand("SET key");
  assert(set_res.status == ParseStatus::Invalid);
  assert(!set_res.command.has_value());
}

int main() {
  test_valid_set();
  test_valid_get();
  test_valid_delete();
  test_invalid_command();
  test_too_many_args();
  test_too_few_args();

  return 0;
}
