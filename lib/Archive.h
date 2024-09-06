#pragma once

#include <string>
#include <utility>
#include <map>
#include <vector>
#include <variant>
#include <iostream>

namespace Hamming {
void Encode(std::string& archive, std::vector<std::string>& name_files, int length_block);
void Decode(std::string& archive, std::string& path, std::vector<std::string>& files);
void Append(std::string& archive, std::string& name, int length_block);
std::vector<std::string> List(std::string& archive);
void Concatenate(std::string& archive1, std::string& archive2);
void Delete(std::string& archive, std::string& name_files);
};


