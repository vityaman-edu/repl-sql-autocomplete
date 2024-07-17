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

auto candidates(const std::string& word) -> std::vector<std::string> {
  return std::vector<std::string> {
    "select",
    "from",
    "where",
    "group",
    "test",
    "who",
    "finally",
    "session",
    "transform",
    "translate",
  }
  | std::views::filter([&](const std::string& hint) {
      return hint.starts_with(word);
    }) | std::ranges::to<std::vector>();
}

auto complete(const std::string& text, int& length) -> Completions {
  return candidates(text.substr(text.size() - length))
      | std::views::transform([](const auto& candidate) {
          return Completion(candidate, Color::BLUE);
        })
      | std::ranges::to<std::vector>();
}

auto hint(const std::string& text, int& length, Color& color) -> Hints {
  color = Color::BLUE;
  return candidates(text.substr(text.size() - length));
}

} // namespace

REPL::REPL() {
  constexpr auto MaxHintRows = 4;
  constexpr auto MaxHistorySize = 32;

  replxx.set_completion_callback(complete);
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