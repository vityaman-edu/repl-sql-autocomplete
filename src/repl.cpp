#include "repl.hpp"
#include "replxx.hxx"
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

namespace repl {

namespace {

using Completion = replxx::Replxx::Completion;
using Completions = replxx::Replxx::completions_t;
using Color = replxx::Replxx::Color;

auto complete(const std::string &text, int &length) -> Completions {
  std::stringstream message;
  message << "text: '" << text << "' "
          << "length: " << length;
  return {
      Completion(message.str(), Color::GREEN),
      Completion(message.str(), Color::RED),
      Completion(message.str(), Color::WHITE),
  };
}

} // namespace

REPL::REPL() {
  replxx.set_completion_count_cutoff(8);
  replxx.set_completion_callback(complete);
}

auto REPL::input(const std::string &prompt) -> std::optional<std::string> {
  const auto *input = replxx.input(prompt);
  return input != nullptr ? std::optional(input) : std::nullopt;
}

auto REPL::out() -> std::ostream & { return std::cout; }

} // namespace repl