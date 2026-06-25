#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>

namespace hvostov {
  void sortArray(size_t* arr, size_t count);
  bool arrayContains(const size_t* arr, size_t count, size_t value);
  size_t* expandArray(size_t* oldArr, size_t oldSize, size_t newSize);
  void printArray(const size_t* arr, size_t count, std::ostream& out);
}

#endif
