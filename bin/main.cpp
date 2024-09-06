#include <functional>
#include "../ArgParser/ArgParser.h"
#include "../lib/Archive.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <filesystem>

using namespace ArgumentParser;
using namespace Hamming;

int main(int argc, char** argv) {
  ArgParse hamarc("archive");

  std::vector<std::string> files;
  std::string archive;
  std::string path;
  int length_block;
  hamarc.AddFlag('c', "create").Default(false);
  hamarc.AddStringArgument('f', "file").StoreValue(archive);
  hamarc.AddFlag('l', "list").Default(false);
  hamarc.AddFlag('x', "extract").Default(false);
  hamarc.AddFlag('a', "append").Default(false);;
  hamarc.AddFlag('d', "delete").Default(false);;
  hamarc.AddFlag('A', "concatenate").Default(false);
  hamarc.AddIntArgument('L', "length_block").StoreValue(length_block).Default(8);
  hamarc.AddStringArgument('p', "path").StoreValue(path).Default("");
  hamarc.AddStringArgument("NamesOfFiles").Positional().MultiValue().StoreValues(files);
  hamarc.Parse(argc, argv);

  if (hamarc.GetFlag("list")) {
    std::vector<std::string> name_of_files = List(archive);
    for (const std::string& i : name_of_files) std::cout << i << "\n";
    if (name_of_files.empty()) std::cout << "Архив  пустой!\n";
  }
  if (hamarc.GetFlag("create")) {
    Encode(archive, files, length_block);
  }
  if (hamarc.GetFlag("append")) {
    Append(archive, files[0], length_block);
  }
  if (hamarc.GetFlag("extract")) {
    Decode(archive, path, files);
  }
  if (hamarc.GetFlag("concatenate")) {
    Concatenate(archive, files[0]);
  }
  if (hamarc.GetFlag("delete")) {
    Delete(archive, files[0]);
  }

}
