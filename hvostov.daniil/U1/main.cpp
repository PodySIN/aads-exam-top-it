#include "types.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <limits>
#include "utility.hpp"

int main(int argc, char* argv[])
{
  std::string inputFile = "";
  std::string outputFile = "";

  if (argc > 3) {
    std::cerr << "To many arguments\n";
    return 0;
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

  size_t capacity = 10;
  size_t count = 0;
  hvostov::Person* persons = new hvostov::Person[capacity];

  size_t idsCapacity = 10;
  size_t idsCount = 0;

  size_t* usedIds = nullptr;
  try {
    usedIds = new size_t[idsCapacity];
  } catch (...) {
    delete[] persons;
  }

  size_t successCount = 0;
  size_t ignoredCount = 0;

  std::ifstream inFile;
  std::istream* in = &std::cin;

  if (!inputFile.empty()) {
    inFile.open(inputFile);
    if (!inFile.is_open()) {
      std::cerr << "Error: cannot open input file\n";
      delete[] persons;
      delete[] usedIds;
      return 2;
    }
    in = &inFile;
  }

  while (in->good() && !in->eof()) {
    hvostov::Person person;

    if (*in >> person) {
      if (!hvostov::idExists(usedIds, idsCount, person.id)) {
        if (idsCount >= idsCapacity) {
          size_t* newIds = new size_t[idsCapacity * 2];
          for (size_t i = 0; i < idsCount; ++i) {
            newIds[i] = usedIds[i];
          }
          delete[] usedIds;
          usedIds = newIds;
          idsCapacity *= 2;
        }
        usedIds[idsCount++] = person.id;

        if (count >= capacity) {
          persons = hvostov::resizePersons(persons, count, capacity * 2);
          capacity *= 2;
        }
        persons[count++] = person;
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
  if (count == 0) {
    std::cout << "\n";
    std::cerr << 0 << ' ' << 0 << "\n";
    delete[] persons;
    delete[] usedIds;
    return 0;
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
      delete[] persons;
      delete[] usedIds;
      return 2;
    }
    out = &outFile;
  }

  for (size_t i = 0; i < count; ++i) {
    *out << persons[i] << '\n';
  }

  if (successCount > 0 || ignoredCount > 0) {
    std::cerr << successCount << ' ' << ignoredCount << '\n';
  }

  delete[] persons;
  delete[] usedIds;

  return 0;
}
