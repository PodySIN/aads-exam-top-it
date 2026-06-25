#include "utils.hpp"

void hvostov::sortArray(size_t* arr, size_t count)
{
  for (size_t i = 0; i < count; ++i) {
    for (size_t j = i + 1; j < count; ++j) {
      if (arr[i] > arr[j]) {
        size_t tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
      }
    }
  }
}

bool hvostov::arrayContains(const size_t* arr, size_t count, size_t value)
{
  for (size_t i = 0; i < count; ++i) {
    if (arr[i] == value) {
      return true;
    }
  }
  return false;
}

size_t* hvostov::expandArray(size_t* oldArr, size_t oldSize, size_t newSize)
{
  size_t* newArr = new size_t[newSize];
  for (size_t i = 0; i < oldSize; ++i) {
    newArr[i] = oldArr[i];
  }
  delete[] oldArr;
  return newArr;
}

void hvostov::printArray(const size_t* arr, size_t count, std::ostream& out)
{
  if (count == 0) {
    out << '\n';
  } else {
    for (size_t i = 0; i < count; ++i) {
      out << arr[i] << '\n';
    }
  }
}
