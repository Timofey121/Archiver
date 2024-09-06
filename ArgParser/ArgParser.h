#pragma once

#include <string>
#include <utility>
#include <map>
#include <vector>
#include <variant>
#include <iostream>

namespace ArgumentParser {
template<typename T>
struct Argument {
  T value{};
  char short_name = '\0';
  std::string description;
  std::string flag;
  bool is_positional = false;
  bool is_multiple = false;
  T* storage_single = nullptr;
  std::vector<T>* storage{};
  int multiple_min_count = 0;

  Argument& Default(T default_value) {
    if (storage_single != nullptr) {
      *storage_single = default_value;
    } else value = default_value;
    return *this;
  }
  Argument& Positional() {
    is_positional = true;
    return *this;
  }
  Argument& MultiValue() {
    is_multiple = true;
    return *this;
  }
  Argument& MultiValue(int min) {
    is_multiple = true;
    multiple_min_count = min;
    return *this;
  }
  Argument& StoreValue(T& val) {
    storage_single = &val;
    return *this;
  }
  Argument& StoreValues(std::vector<T>& values) {
    storage = &values;
    return *this;
  }
};

using TypeArgument = std::variant<Argument<int>, Argument<std::string>, Argument<bool>>;

class ArgParse {
 private:
  std::string name;
  std::map<std::string, TypeArgument> parameter;

  template<typename T>
  void GetDescriptionAboutValue(T info, std::string& s);

  bool CheckParams();

  bool PreCheckParams();

  bool CheckMinCount();

  std::string GetFlag(char val);

  bool IsNumeric(const std::string& val);

  std::string GetFirstPositionFlag(int index);
 public:
  Argument<int>& GetInt(const std::string& val);

  Argument<bool>& GetBool(const std::string& val);

  Argument<std::string>& GetString(const std::string& val);

  int GetIdxEqual(const std::string& val);

  Argument<int>& AddIntArgument(const std::string& value);

  Argument<int>& AddIntArgument(char symbols, const std::string& value);

  Argument<int>& AddIntArgument(const std::string& value, const std::string& description);

  Argument<int>& AddIntArgument(char symbols, const std::string& value, const std::string& description);

  Argument<std::string>& AddStringArgument(const std::string& value);

  Argument<std::string>& AddStringArgument(char symbols, const std::string& value);

  Argument<std::string>& AddStringArgument(char symbols,
                                           const std::string& value,
                                           const std::string& description);

  Argument<std::string>& AddStringArgument(const std::string& value, const std::string& description);

  Argument<bool>& AddFlag(char symbols, const std::string& value);

  Argument<bool>& AddFlag(char symbols, const std::string& value, const std::string& description);

  Argument<bool>& AddFlag(const std::string& value, const std::string& description);

  Argument<std::string>& AddHelp(char symbols, const std::string& value, const std::string& description);

  std::string GetStringValue(const std::string& value);

  int GetIntValue(const std::string& value);

  int GetIntValue(const std::string& value, int i);

  bool GetFlag(const std::string& value);

  bool Help();

  std::string HelpDescription();

  bool Parse(const std::vector<std::string>& values);

  bool Parse(int argc, char** argv);

  explicit ArgParse(std::string name_val) {
    name = std::move(name_val);
  }
  ~ArgParse() {
    name = "";
    parameter.clear();
  }
};
}
