#include "repl.hpp"
#include "replxx.hxx"

#include <iostream>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

namespace repl {

namespace {

using Completion = replxx::Replxx::Completion;
using Completions = replxx::Replxx::completions_t;
using Color = replxx::Replxx::Color;
using Hints = replxx::Replxx::hints_t;

} // namespace

REPL::REPL() {
  constexpr auto MaxHintRows = 4;
  constexpr auto MaxHistorySize = 32;

  const auto completion = [this](const auto& text, int& /* length */) {
    return engine.Suggest(text)
        | std::views::transform([](const auto& candidate) {
            return Completion(candidate, Color::BLUE);
          })
        | std::ranges::to<std::vector>();
  };

  const auto hint = [this](const auto& text, int& /* length */, Color& color) {
    color = Color::YELLOW;
    return engine.Suggest(text);
  };

  replxx.set_completion_callback(completion);
  replxx.set_hint_callback(hint);

  replxx.set_max_hint_rows(MaxHintRows);
  replxx.set_max_history_size(MaxHistorySize);
}

auto REPL::input(const std::string& prompt) -> std::optional<std::string> {
  const auto* input = replxx.input(prompt);
  if (input == nullptr) {
    return std::nullopt;
  }

  std::string line = input;
  replxx.history_add(line);
  return std::move(line);
}

auto REPL::out() -> std::ostream& {
  return std::cout;
}

} // namespace repl
