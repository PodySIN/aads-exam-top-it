#include "types.hpp"
#include <cctype>
#include <string>
#include <limits>

std::istream& hvostov::operator>>(std::istream& in, Person& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry) {
    return in;
  }

  Person input;

  if (!(in >> input.id)) {
    in.clear();
    in.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    return in;
  }

  while (in.good() && std::isspace(in.peek()) && in.peek() != '\n') {
    in.get();
  }

  std::getline(in, input.info);

  size_t start = 0;
  while (start < input.info.length() && std::isspace(input.info[start])) {
    start++;
  }

  size_t end = input.info.length();
  while (end > start && std::isspace(input.info[end - 1])) {
    end--;
  }

  input.info = input.info.substr(start, end - start);
  if (input.info.empty()) {
    return in;
  }

  dest = input;
  return in;
}

std::ostream& hvostov::operator<<(std::ostream& out, const Person& dest)
{
  std::ostream::sentry sentry(out);
  if (!sentry) {
    return out;
  }

  out << dest.id << ' ' << dest.info;
  return out;
}
