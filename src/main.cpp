#include "repl.hpp"
#include <cstring>

namespace repl {

auto main() -> int {
  REPL repl;
  repl.out() << ";O " << repl.input(":) ") << std::endl;
  return 0;
}

} // namespace repl

auto main(int /*argc*/, char* /*argv*/[]) -> int {
  return repl::main();
}
