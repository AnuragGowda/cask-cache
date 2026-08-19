#include "store.h"

void Store::setValue(const std::string& key, const std::string& value) {
  data_[key] = value;
}

std::optional<std::string> Store::getValue(const std::string& key) const{
  auto it = data_.find(key);

  if (it == data_.end()) {
    return std::nullopt;
  }

  return it->second;
}

bool Store::deleteKey(const std::string& key){
  return data_.erase(key) > 0;
}

