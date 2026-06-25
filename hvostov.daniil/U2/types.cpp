#include "types.hpp"
#include <cctype>

std::istream& hvostov::operator>>(std::istream& in, Person& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry) {
    return in;
  }

  Person input;
  input.hasDescription = false;

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
    in.setstate(std::ios::failbit);
    return in;
  }

  input.hasDescription = true;
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

std::istream& hvostov::operator>>(std::istream& in, Meeting& dest)
{
  std::istream::sentry sentry(in);
  if (!sentry) {
    return in;
  }

  Meeting input;

  if (!(in >> input.id1 >> input.id2 >> input.duration)) {
    return in;
  }

  if (input.id1 == input.id2) {
    in.setstate(std::ios::failbit);
    return in;
  }

  dest = input;
  return in;
}
