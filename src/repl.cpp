#include "repl.hpp"
#include <iostream>
#include <string>

namespace repl {

auto REPL::input(const std::string& prompt) -> std::string {
  return replxx.input(prompt);
}

auto REPL::out() -> std::ostream& {
  return std::cout;
}

} // namespace repl