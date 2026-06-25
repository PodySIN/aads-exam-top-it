#include "utility.hpp"

hvostov::Person* hvostov::resizePersons(Person* oldArray, size_t oldSize, size_t newSize)
{
  Person* newArray = new Person[newSize];
  for (size_t i = 0; i < oldSize; ++i) {
    newArray[i] = oldArray[i];
  }
  delete[] oldArray;
  return newArray;
}

bool hvostov::idExists(size_t* ids, size_t count, size_t id)
{
  for (size_t i = 0; i < count; ++i) {
    if (ids[i] == id) {
      return true;
    }
  }
  return false;
}
