#pragma once 

#include <string>
#include <optional>
#include <unordered_map>

class Store {
private:
  std::unordered_map<std::string, std::string> data_;

public:
  void setValue(const std::string& key, const std::string& value);
  std::optional<std::string> getValue(const std::string& key) const;
  bool deleteKey(const std::string& key);
};
