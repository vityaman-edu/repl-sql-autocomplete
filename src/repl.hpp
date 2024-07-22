#pragma once

#include <ostream>
#include <replxx.hxx>
#include <string>

namespace repl {

class REPL {
public:
  explicit REPL();

  auto input(const std::string& prompt) -> std::optional<std::string>;
  auto out() -> std::ostream&;

private:
  replxx::Replxx replxx;
};

} // namespace repl
