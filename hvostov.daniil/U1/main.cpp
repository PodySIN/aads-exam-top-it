#include "types.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <cstring>
#include <limits>

int main(int argc, char* argv[])
{
  std::string inputFile = "";
  std::string outputFile = "";

  if (argc > 3) {
    std::cerr << "To many arguments\n";
    return 2;
  }
  for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);

    if (arg.substr(0, 3) == "in:") {
      if (!inputFile.empty()) {
        std::cerr << "Error: duplicate input file argument\n";
        return 1;
      }
      inputFile = arg.substr(3);
    } else if (arg.substr(0, 4) == "out:") {
      if (!outputFile.empty()) {
        std::cerr << "Error: duplicate output file argument\n";
        return 1;
      }
      outputFile = arg.substr(4);
    } else {
      std::cerr << "Error: invalid argument\n";
      return 1;
    }
  }

  std::vector< hvostov::Person > persons;
  std::set< size_t > usedIds;
  size_t successCount = 0;
  size_t ignoredCount = 0;

  std::ifstream inFile;
  std::istream* in = &std::cin;

  if (!inputFile.empty()) {
    inFile.open(inputFile);
    if (!inFile.is_open()) {
      std::cerr << "Error: cannot open input file\n";
      return 2;
    }
    in = &inFile;
  }

  while (in->good() && !in->eof()) {
    hvostov::Person person;

    if (*in >> person) {
      if (usedIds.find(person.id) == usedIds.end()) {
        persons.push_back(person);
        usedIds.insert(person.id);
        successCount++;
      } else {
        ignoredCount++;
      }
    } else {
      if (!in->eof()) {
        ignoredCount++;
        in->clear();
        if (in->fail()) {
          in->ignore(std::numeric_limits< std::streamsize >::max(), '\n');
        }
      }
    }
  }

  if (inFile.is_open()) {
    inFile.close();
  }

  std::ofstream outFile;
  std::ostream* out = &std::cout;

  if (!outputFile.empty()) {
    outFile.open(outputFile);
    if (!outFile.is_open()) {
      std::cerr << "Error: cannot open output file\n";
      return 2;
    }
    out = &outFile;
  }

  for (const auto& person : persons) {
    *out << person << '\n';
  }

  if (successCount > 0 || ignoredCount > 0) {
    std::cerr << successCount << ' ' << ignoredCount << '\n';
  }

  return 0;
}
