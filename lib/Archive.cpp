#include <fstream>
#include <bitset>
#include <cmath>
#include <filesystem>
#include "Archive.h"
#include <algorithm>

std::string Extract(std::string str) {
  std::string new_name;
  while (str[str.size() - 1] != '/') {
    new_name = str[str.size() - 1] + new_name;
    str.erase(str.size() - 1, 1);
  }
  return new_name;
}
int GetCountControlBites(int bites) {
  int k = 0;
  while (bites > (std::pow(2, k) - k - 1)) ++k;
  return k;
}
std::vector<int> Factorization(int number) {
  std::vector<int> answer;
  std::bitset<10> bites(number);
  for (int i = 0; i < 10; ++i) if (bites[i] == 1) answer.push_back(i);
  return answer;
}
void EncodeBlock(std::string& block) {
  int k = GetCountControlBites(block.length());
  int position = 1;
  auto* control_bites = new char[k];
  for (int i = 0; i < k; ++i) control_bites[i] = '0';
  for (int i = 0; i < block.length(); ++i) {
    while (float(log2(position)) == log2(position)) ++position;
    if (block[i] == '1') {
      for (int need_control_bite : Factorization(position)) {
        if (control_bites[need_control_bite] == '1') control_bites[need_control_bite] = '0';
        else if (control_bites[need_control_bite] == '0') control_bites[need_control_bite] = '1';
      }
    }
    ++position;
  }
  for (int i = 0; i < k; ++i) block += control_bites[i];
}
std::string DecodeBlock(const std::string& block, int length_block) {
  std::string trans_block;
  int position = 1;
  int mistake = 0;
  int k = block.size() - length_block;
  int* control_bites = new int[k]{0};
  for (int i = 0; i < k; ++i) {
    if (block[length_block + i] == '1') {
      control_bites[i] = 1;
    }
  }
  for (int j = 0; j < length_block; ++j) {
    trans_block += block[j];
  }
  for (int i = 0; i < length_block; ++i) {
    while (static_cast<int>(log2(position)) == log2(position)) ++position;
    if (block[i] == '1') {
      for (int need_control_bit : Factorization(position)) {
        if (control_bites[need_control_bit] == 1) control_bites[need_control_bit] = 0;
        else control_bites[need_control_bit] = 1;
      }
    }
    ++position;
  }
  for (int j = 0; j < k; ++j) if (control_bites[j] != 0) mistake += pow(2, j);
  if (mistake != 0 && (static_cast<int>(log2(mistake)) != log2(mistake))) {
    mistake = static_cast<int>(mistake - log2(mistake) - 1);
    if (trans_block[mistake] == '1') trans_block[mistake] = '0';
    else trans_block[mistake] = '1';
  }
  return trans_block;
}

void Hamming::Encode(std::string& archive, std::vector<std::string>& name_files, int length_block) {
  std::ofstream arch(archive + ".haf", std::ios::binary);
  arch << length_block << "\t";
  for (std::string& name : name_files) {
    std::ifstream file(name, std::ios::binary);
    if (!file) {
      std::cout << "Не удалось открыть файл." << std::endl;
      return;
    }
    arch << Extract(name) << "\t";
    file.seekg(0, std::ios::end);
    int FileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    int counter = 0;
    std::string bites;
    std::string block;
    std::string extra;
    while (counter < FileSize) {
      bites = std::bitset<8>(file.get()).to_string();
      block += bites;
      ++counter;
    }
    int i = 0;
    while (i < block.size()) {
      if (extra.length() == length_block) {
        EncodeBlock(extra);
        for (char j : extra) arch << j;
        extra.clear();
        extra = block[i];
      } else {
        extra += block[i];
      }
      ++i;
    }
    if (extra.length() != 0) {
      EncodeBlock(extra);
      for (char j : extra) arch << j;
    }
    extra.clear();
    block.clear();
    arch << "\t";
  }
}

void Hamming::Decode(std::string& archive, std::string& path, std::vector<std::string>& files) {
  std::ifstream arch(archive + ".haf", std::ios::binary);
  if (!arch) {
    std::cout << "Не удалось открыть файл!." << std::endl;
    return;
  }
  int length_block;
  arch >> length_block;
  arch.seekg(0, std::ios::end);
  int FileSize = arch.tellg();
  arch.seekg(0, std::ios::beg);
  int counter = 0;
  int k = 0;
  int cb = GetCountControlBites(length_block);
  char byte;
  std::string bites;
  std::string name;
  std::string block;
  std::string extra;
  std::string ans;
  while (counter < FileSize) {
    arch.get(byte);
    if (byte == '\t' && k == 0) {
      arch.get(byte);
      ++counter;
      while (byte != '\t') {
        name += byte;
        arch.get(byte);
        ++counter;
      }
      --counter;
      arch.seekg(counter, std::ios::beg);
      ++k;
    } else if (byte == '\t' && k == 1) {
      arch.get(byte);
      ++counter;
      while (byte != '\t') {
        if (block.length() < length_block) {
          block += byte;
        } else if (block.length() == length_block) {
          int g = 0;
          while (g < cb - 1 && counter <= FileSize) {
            block += byte;
            ++g;
            ++counter;
            arch.get(byte);
          }
          block += byte;
          std::string trans_block = DecodeBlock(block, length_block);
          ans += trans_block;
          block.clear();
        }
        arch.get(byte);
        ++counter;
      }
      --counter;
      arch.seekg(counter, std::ios::beg);
      k = 0;
      if (files.empty() || (std::find(files.begin(), files.end(), name) != files.end())) {
        std::filesystem::create_directory(path);
        std::ofstream file((path + "/" + name).c_str(), std::ios::binary);
        name.clear();
        int i = 0;
        while (i != ans.size()) {
          if (extra.length() == 8) {
            std::bitset<8> bits(extra);
            char chr = static_cast<char>(bits.to_ulong());
            file << chr;
            extra.clear();
            extra += ans[i];
          } else {
            extra += ans[i];
          }
          ++i;
        }
        if (extra.length() != 0) {
          for (int ii = 0; ii < (8 - extra.length()); ++ii) extra += '0';
          std::bitset<8> bits(extra);
          char chr = static_cast<char>(bits.to_ulong());
          file << chr;
        }
        extra.clear();
        ans.clear();
        block.clear();
      }
    }
    ++counter;
  }
}

void Hamming::Concatenate(std::string& archive1, std::string& archive2) {
  std::ofstream arch(archive1 + ".haf", std::ios::binary | std::ios::app);
  std::ifstream arch1(archive2 + ".haf", std::ios::binary);
  if (!arch) {
    std::cout << "Не удалось найти АРХИВ под название " + archive1 + ".haf" << std::endl;
    return;
  }
  if (!arch1) {
    std::cout << "Не удалось найти АРХИВ под название " + archive2 + ".haf" << std::endl;
    return;
  }
  std::ifstream f(archive1 + ".haf", std::ios::binary);
  std::ifstream s(archive2 + ".haf", std::ios::binary);
  int l1, l2;
  f >> l1;
  s >> l2;
  if (l1 != l2) {
    std::cout << "Разная длина блоков кодирования в предоставленных файлах!";
    return;
  }
  char byte;
  arch1.get(byte);
  arch1.get(byte);
  while (arch1.get(byte)) {
    arch << byte;
  }
}

void Hamming::Append(std::string& archive, std::string& name, int length_block) {
  std::ofstream arch(archive + ".haf", std::ios::binary | std::ios::app);
  std::ifstream file(name, std::ios::binary);
  if (!arch) {
    std::cout << "Не удалось найти АРХИВ под название " + archive + ".haf" << std::endl;
    return;
  }
  arch << Extract(name) << "\t";
  file.seekg(0, std::ios::end);
  int FileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  int counter = 0;
  std::string bites;
  std::string block;
  std::string extra;
  while (counter < FileSize) {
    bites = std::bitset<8>(file.get()).to_string();
    block += bites;
    ++counter;
  }
  int i = 0;
  while (i < block.size()) {
    if (extra.length() == length_block) {
      EncodeBlock(extra);
      for (char j : extra) arch << j;
      extra = block[i];
    } else {
      extra += block[i];
    }
    ++i;
  }
  if (extra.length() != 0) {
    EncodeBlock(extra);
    for (char j : extra) arch << j;
  }
  extra.clear();
  block.clear();
  arch << "\t";
}

std::vector<std::string> Hamming::List(std::string& archive) {
  std::vector<std::string> files;
  std::ifstream arch(archive + ".haf", std::ios::binary);
  if (!arch) {
    std::cout << "Не удалось найти АРХИВ под название " + archive + ".haf" << std::endl;
    return {};
  }
  int length_block;
  arch >> length_block;
  arch.seekg(0, std::ios::end);
  int FileSize = arch.tellg();
  arch.seekg(0, std::ios::beg);
  int counter = 0;
  int k = 0;
  char byte;
  std::string bites;
  std::string name;
  std::string block;
  std::string extra;
  std::string ans;
  while (counter < FileSize) {
    arch.get(byte);
    if (byte == '\t' && k == 0) {
      arch.get(byte);
      ++counter;
      while (byte != '\t') {
        name += byte;
        arch.get(byte);
        ++counter;
      }
      --counter;
      arch.seekg(counter, std::ios::beg);
      ++k;
    } else if (byte == '\t' && k == 1) {
      files.push_back(name);
      name.clear();
      arch.get(byte);
      ++counter;
      while (byte != '\t') {
        arch.get(byte);
        ++counter;
      }
      --counter;
      arch.seekg(counter, std::ios::beg);
      k = 0;
    }
    ++counter;
  }
  return files;
}

void Hamming::Delete(std::string& archive, std::string& file) {
  std::ofstream new_arch("COPY" + archive + ".haf", std::ios::binary);
  std::ifstream read_file(archive + ".haf", std::ios::binary);
  if (!read_file) {
    std::cout << "Не удалось найти АРХИВ!" << std::endl;
    return;
  }
  read_file.seekg(0, std::ios::end);
  int FileSize = read_file.tellg();
  read_file.seekg(0, std::ios::beg);
  int counter = 0;
  int k = 0;
  char byte;
  std::string bites;
  std::string name;
  std::string block;
  std::string extra;
  std::string ans;
  int length = 0;
  read_file >> length;
  new_arch << length << '\t';
  while (counter < FileSize) {
    read_file.get(byte);
    if (byte == '\t' && k == 0) {
      read_file.get(byte);
      ++counter;
      while (byte != '\t') {
        name += byte;
        read_file.get(byte);
        ++counter;
      }
      --counter;
      read_file.seekg(counter, std::ios::beg);
      ++k;
    } else if (byte == '\t' && k == 1) {
      if (file != name) {
        new_arch << name << "\t";
      }
      read_file.get(byte);
      ++counter;
      while (byte != '\t') {
        if (file != name) new_arch << byte;
        read_file.get(byte);
        ++counter;
      }
      if (file != name) new_arch << "\t";
      --counter;
      read_file.seekg(counter, std::ios::beg);
      name.clear();
      k = 0;
    }
    ++counter;
  }
  new_arch.close();
  read_file.close();
  remove((archive + ".haf").c_str());
  std::ifstream read_new_arch("COPY" + archive + ".haf", std::ios::binary);
  std::ofstream final_arch(archive + ".haf", std::ios::binary);
  while (read_new_arch.get(byte)) {
    final_arch << byte;
  }
  remove(("COPY" + archive + ".haf").c_str());
}
