#include "ArgParser.h"

ArgumentParser::Argument<int>& ArgumentParser::ArgParse::GetInt(const std::string& val) {
  return *std::get_if<ArgumentParser::Argument<int>>(&ArgumentParser::ArgParse::parameter.find(val)->second);
}
ArgumentParser::Argument<std::string>& ArgumentParser::ArgParse::GetString(const std::string& val) {
  return *std::get_if<ArgumentParser::Argument<std::string>>(&ArgumentParser::ArgParse::parameter.find(val)->second);
}
ArgumentParser::Argument<bool>& ArgumentParser::ArgParse::GetBool(const std::string& val) {
  return *std::get_if<ArgumentParser::Argument<bool>>(&ArgumentParser::ArgParse::parameter.find(val)->second);
}
int ArgumentParser::ArgParse::GetIdxEqual(const std::string& val) {
  for (int i = 0; i < val.size(); ++i) {
    if (val[i] == '=') {
      return i;
    }
  }
  return 0;
}
template<typename T>
void ArgumentParser::ArgParse::GetDescriptionAboutValue(T info, std::string& s) {
  if (info.short_name != '\0') s = s + "-" + info.short_name + ", ";
  else s += "    ";
  s += "--" + info.flag;
  if (!info.description.empty()) s += "    " + info.description;
}
bool ArgumentParser::ArgParse::CheckParams() {
  for (const auto& pair : ArgumentParser::ArgParse::parameter) {
    TypeArgument param = pair.second;
    if (param.index() == 0) {
      Argument<int> info = *get_if<Argument<int>>(&param);
      if (info.value == 0 || (info.storage_single != nullptr && (*info.storage_single) == 0)) return false;
    } else if (param.index() == 1) {
      Argument<std::string> info = *get_if<Argument<std::string>>(&param);
      if (info.value.empty() || (info.storage_single != nullptr && (*info.storage_single).empty())) return false;
    }
  }
  return true;
}
bool ArgumentParser::ArgParse::PreCheckParams() {
  for (const auto& pair : ArgumentParser::ArgParse::parameter) {
    TypeArgument param = pair.second;
    if (param.index() == 0) {
      Argument<int> info = *get_if<Argument<int>>(&param);
      if (info.is_multiple != false && info.storage == nullptr) return false;
    } else if (param.index() == 1) {
      Argument<std::string> info = *get_if<Argument<std::string>>(&param);
      if (info.is_multiple != false && info.storage == nullptr) return false;
    } else {
      Argument<bool> info = *get_if<Argument<bool>>(&param);
      if (info.is_multiple != false && info.storage == nullptr) return false;
    }
  }
  return true;
}
bool ArgumentParser::ArgParse::CheckMinCount() {
  for (const auto& pair : ArgumentParser::ArgParse::parameter) {
    TypeArgument param = pair.second;
    if (param.index() == 0) {
      Argument<int> info = *get_if<Argument<int>>(&param);
      if (((info.storage != nullptr) && (info.multiple_min_count != 0)
          && ((info.storage->size()) < info.multiple_min_count)))
        return false;
    } else if (param.index() == 1) {
      Argument<std::string> info = *get_if<Argument<std::string>>(&param);
      if (((info.storage != nullptr) && (info.multiple_min_count != 0)
          && ((info.storage->size()) < info.multiple_min_count)))
        return false;
    } else {
      Argument<bool> info = *get_if<Argument<bool>>(&param);
      if (((info.storage != nullptr) && (info.multiple_min_count != 0)
          && ((info.storage->size()) < info.multiple_min_count)))
        return false;
    }
  }
  return true;
}
bool ArgumentParser::ArgParse::IsNumeric(const std::string& val) {
  for (char i : val) if (((i - '0') < 0) || ((i - '0') > 9)) return false;
  return true;
}
std::string ArgumentParser::ArgParse::GetFirstPositionFlag(int index) {
  for (const auto& pair : ArgumentParser::ArgParse::parameter) {
    TypeArgument param = pair.second;
    if (param.index() == 0 && param.index() == index) {
      Argument<int> info = *get_if<Argument<int>>(&param);
      if (info.is_positional) {
        if (info.is_multiple) {
          return info.flag;
        } else {
          if (info.storage_single != nullptr && (*info.storage_single == 0)) return info.flag;
          else if (info.value == 0) return info.flag;
        }
      }
    } else if (param.index() == 1 && param.index() == index) {
      Argument<std::string> info = *get_if<Argument<std::string>>(&param);
      if (info.is_positional) {
        if (info.is_multiple) {
          return info.flag;
        } else {
          if (info.storage_single != nullptr && (info.storage_single->empty())) return info.flag;
          else if (info.value.empty()) return info.flag;
        }
      }
    }
  }
  return " ";
}

ArgumentParser::Argument<int>& ArgumentParser::ArgParse::AddIntArgument(const std::string& value) {
  Argument<int> value_Argument;
  value_Argument.flag = value;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetInt(value);
}

ArgumentParser::Argument<int>& ArgumentParser::ArgParse::AddIntArgument(char symbols, const std::string& value) {
  Argument<int> value_Argument = AddIntArgument(value);
  value_Argument.short_name = symbols;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetInt(value);
}

ArgumentParser::Argument<int>& ArgumentParser::ArgParse::AddIntArgument(const std::string& value,
                                                                         const std::string& description) {
  Argument<int> value_Argument = AddIntArgument(value);
  value_Argument.description = description;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetInt(value);
}
ArgumentParser::Argument<int>& ArgumentParser::ArgParse::AddIntArgument(char symbols,
                                                                         const std::string& value,
                                                                         const std::string& description) {
  Argument<int> value_Argument = AddIntArgument(symbols, value);
  value_Argument.description = description;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetInt(value);
}

ArgumentParser::Argument<std::string>& ArgumentParser::ArgParse::AddStringArgument(const std::string& value) {
  Argument<std::string> value_Argument;
  value_Argument.flag = value;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetString(value);
}

ArgumentParser::Argument<std::string>& ArgumentParser::ArgParse::AddStringArgument(char symbols,
                                                                                    const std::string& value) {
  Argument<std::string> value_Argument = AddStringArgument(value);
  value_Argument.short_name = symbols;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetString(value);
}

ArgumentParser::Argument<std::string>& ArgumentParser::ArgParse::AddStringArgument(char symbols,
                                                                                    const std::string& value,
                                                                                    const std::string& description) {
  Argument<std::string> value_Argument = AddStringArgument(symbols, value);
  value_Argument.description = description;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetString(value);
}

ArgumentParser::Argument<std::string>& ArgumentParser::ArgParse::AddStringArgument(const std::string& value,
                                                                                    const std::string& description) {
  Argument<std::string> value_Argument = AddStringArgument(value);
  value_Argument.description = description;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetString(value);
}

ArgumentParser::Argument<bool>& ArgumentParser::ArgParse::AddFlag(char symbols, const std::string& value) {
  Argument<bool> value_Argument;
  value_Argument.flag = value;
  value_Argument.short_name = symbols;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetBool(value);
}

ArgumentParser::Argument<bool>& ArgumentParser::ArgParse::AddFlag(char symbols,
                                                                   const std::string& value,
                                                                   const std::string& description) {
  Argument<bool> value_Argument = AddFlag(symbols, value);
  value_Argument.description = description;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetBool(value);
}

ArgumentParser::Argument<bool>& ArgumentParser::ArgParse::AddFlag(const std::string& value,
                                                                   const std::string& description) {
  Argument<bool> value_Argument;
  value_Argument.flag = value;
  value_Argument.description = description;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetBool(value);
}

ArgumentParser::Argument<std::string>& ArgumentParser::ArgParse::AddHelp(char symbols,
                                                                          const std::string& value,
                                                                          const std::string& description) {
  Argument<std::string> value_Argument = AddStringArgument(symbols, value);
  value_Argument.description = description;
  ArgumentParser::ArgParse::parameter[value] = value_Argument;
  return GetString(value);
}

std::string ArgumentParser::ArgParse::GetStringValue(const std::string& value) {
  ArgumentParser::Argument<std::string> info = ArgumentParser::ArgParse::GetString(value);
  if (info.storage_single != nullptr) {
    return *info.storage_single;
  }
  return info.value;
}

int ArgumentParser::ArgParse::GetIntValue(const std::string& value) {
  ArgumentParser::Argument<int> info = ArgumentParser::ArgParse::GetInt(value);
  if (info.storage_single != nullptr) {
    return *info.storage_single;
  }
  return info.value;
}

int ArgumentParser::ArgParse::GetIntValue(const std::string& value, int i) {
  return (GetInt(value).storage[0][i]);
}

bool ArgumentParser::ArgParse::GetFlag(const std::string& value) {
  ArgumentParser::Argument<bool> info = ArgumentParser::ArgParse::GetBool(value);
  if (info.storage_single != nullptr) {
    return *info.storage_single;
  }
  return info.value;
}

bool ArgumentParser::ArgParse::Help() {
  if (ArgumentParser::ArgParse::parameter.find("help") == ArgumentParser::ArgParse::parameter.end()
      || ArgumentParser::ArgParse::GetFlag('h') == " ")
    return false;
  return true;
}

std::string ArgumentParser::ArgParse::GetFlag(char val) {
  for (const auto& pair : ArgumentParser::ArgParse::parameter) {
    TypeArgument param = pair.second;
    if (param.index() == 0) {
      Argument<int> info = *get_if<Argument<int>>(&param);
      if (info.short_name == val) return info.flag;
    } else if (param.index() == 1) {
      Argument<std::string> info = *get_if<Argument<std::string>>(&param);
      if (info.short_name == val) return info.flag;
    } else {
      Argument<bool> info = *get_if<Argument<bool>>(&param);
      if (info.short_name == val) return info.flag;
    }
  }
  return " ";
}

std::string ArgumentParser::ArgParse::HelpDescription() {
  std::string help_description;
  help_description += name + "\n";
  help_description += "Description about your program\n\n";
  for (const auto& pair : ArgumentParser::ArgParse::parameter) {
    TypeArgument param = pair.second;
    if (param.index() == 0) {
      Argument<int> info = *get_if<ArgumentParser::Argument<int>>(&param);
      GetDescriptionAboutValue(info, help_description);
    } else if (param.index() == 1) {
      Argument<std::string> info = *get_if<Argument<std::string>>(&param);
      GetDescriptionAboutValue(info, help_description);
    } else {
      Argument<bool> info = *get_if<Argument<bool>>(&param);
      GetDescriptionAboutValue(info, help_description);
    }
    help_description += "\n";
  }
  return help_description;
}
bool ArgumentParser::ArgParse::Parse(const std::vector<std::string>& values) {
  if (values.size() == 1 && !CheckParams()) return false;
  if (!PreCheckParams()) return false;
  for (int i = 0; i < values.size(); ++i) {
    if (values[i] == "--help" || values[i] == "-h") continue;
    std::string flag;
    std::string value;
    std::string min_flag;
    int idx = GetIdxEqual(values[i]);
    ArgumentParser::TypeArgument param;
    if (idx == 0) {
      if (values[i][1] == '-') {
        flag = values[i].substr(2, values[i].size());
        if (ArgumentParser::ArgParse::parameter.find(flag) == ArgumentParser::ArgParse::parameter.end()) return false;
        Argument<bool>& info = GetBool(flag);
        if (info.storage_single != nullptr) *info.storage_single = true;
        else info.value = true;
      } else if (values[i][0] == '-') {
        flag = values[i].substr(1, values[i].size() - 1);
        for (char j : flag) {
          min_flag = GetFlag(j);
          if (min_flag == " ") return false;
          Argument<bool>& info = GetBool(min_flag);
          if (info.storage_single != nullptr) *info.storage_single = true;
          else info.value = true;
        }
      } else {
        value = values[i];
        if (!IsNumeric(value)) {
          flag = GetFirstPositionFlag(1);
          if (flag == " ") return false;
          Argument<std::string>& info = GetString(flag);
          if (info.is_multiple) {
            info.storage->push_back(value);
          } else {
            if (info.storage_single != nullptr) *info.storage_single = value;
            else info.value = value;
          }
        } else {
          flag = GetFirstPositionFlag(0);
          if (flag == " ") return false;
          Argument<int>& info = GetInt(flag);
          if (info.is_multiple) {
            info.storage->push_back(std::stoi(value));
          } else {
            if (info.storage_single != nullptr) *info.storage_single = std::stoi(value);
            else info.value = std::stoi(value);
          }
        }
      }
    } else {
      if (values[i][1] == '-') {
        flag = values[i].substr(2, idx - 2);
        value = values[i].substr(idx + 1, values[i].size() - idx - 1);
      } else if (values[i][0] == '-') {
        flag = values[i].substr(1, idx - 1);
        value = values[i].substr(idx + 1, values[i].size() - idx - 1);
      }
      if (ArgumentParser::ArgParse::parameter.find(flag) == ArgumentParser::ArgParse::parameter.end()) {
        flag = GetFlag(flag[0]);
      }
      param = ArgumentParser::ArgParse::parameter[flag];
      if (param.index() == 0) {
        Argument<int>& info = GetInt(flag);
        if (info.is_multiple) {
          info.storage->push_back(std::stoi(value));
        } else {
          if (info.storage_single != nullptr) *info.storage_single = std::stoi(value);
          else info.value = std::stoi(value);
        }
      }
      if (param.index() == 1) {
        Argument<std::string>& info = GetString(flag);
        if (info.is_multiple) {
          info.storage->push_back(value);
        } else {
          if (info.storage_single != nullptr) *info.storage_single = value;
          else info.value = value;
        }
      }
    }
  }
  if (!CheckMinCount()) return false;
  return true;
}
bool ArgumentParser::ArgParse::Parse(int argc, char** argv) {
  std::vector<std::string> result;
  for (int i = 1; i < argc; ++i) {
    result.emplace_back(argv[i]);
  }
  return Parse(result);
}
