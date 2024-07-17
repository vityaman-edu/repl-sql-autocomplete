#include "repl.hpp"
#include <cstring>
#include <optional>
#include <string>

namespace repl {

auto main() -> int {
  REPL repl;
  while (const auto maybe_input = repl.input(":) ")) {
    const auto& input = maybe_input.value();
    repl.out() << ";O " << input << std::endl;
  }
  return 0;
}

} // namespace repl

auto main(int /*argc*/, char* /*argv*/[]) -> int {
  return repl::main();
}
