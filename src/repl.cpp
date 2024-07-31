#include "repl.hpp"
#include "replxx.hxx"
#include "sql/antlr/YQLLexer.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
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

  const auto highlight = [](const auto& text, auto& colors) mutable {
    antlr4::ANTLRInputStream chars(text);
    YQLLexer lexer(&chars);
    antlr4::BufferedTokenStream tokens(&lexer);
    tokens.fill();
    for (std::size_t i = 0; i < tokens.size(); ++i) {
      const auto* token = tokens.get(i);
      const auto color = [&] {
        switch (token->getType()) {
        case YQLLexer::STRING_VALUE:
          return Color::GREEN;
        case YQLLexer::ID_PLAIN:
        case YQLLexer::ID_QUOTED:
          return Color::RED;
        case YQLLexer::INTEGER_VALUE:
        case YQLLexer::REAL:
        case YQLLexer::BLOB:
        case YQLLexer::DIGITS:
          return Color::CYAN;
        case YQLLexer::EQUALS:
        case YQLLexer::EQUALS2:
        case YQLLexer::NOT_EQUALS:
        case YQLLexer::NOT_EQUALS2:
        case YQLLexer::LESS:
        case YQLLexer::LESS_OR_EQ:
        case YQLLexer::GREATER:
        case YQLLexer::GREATER_OR_EQ:
        case YQLLexer::SHIFT_LEFT:
        case YQLLexer::ROT_LEFT:
        case YQLLexer::AMPERSAND:
        case YQLLexer::PIPE:
        case YQLLexer::DOUBLE_PIPE:
        case YQLLexer::STRUCT_OPEN:
        case YQLLexer::STRUCT_CLOSE:
        case YQLLexer::PLUS:
        case YQLLexer::MINUS:
        case YQLLexer::TILDA:
        case YQLLexer::SLASH:
        case YQLLexer::ASTERISK:
        case YQLLexer::BACKSLASH:
        case YQLLexer::PERCENT:
        case YQLLexer::SEMICOLON:
        case YQLLexer::DOT:
        case YQLLexer::COMMA:
        case YQLLexer::LPAREN:
        case YQLLexer::RPAREN:
        case YQLLexer::QUESTION:
        case YQLLexer::COLON:
        case YQLLexer::AT:
        case YQLLexer::DOUBLE_AT:
        case YQLLexer::DOLLAR:
        case YQLLexer::QUOTE_DOUBLE:
        case YQLLexer::QUOTE_SINGLE:
        case YQLLexer::BACKTICK:
        case YQLLexer::LBRACE_CURLY:
        case YQLLexer::RBRACE_CURLY:
        case YQLLexer::CARET:
        case YQLLexer::NAMESPACE:
        case YQLLexer::ARROW:
        case YQLLexer::RBRACE_SQUARE:
        case YQLLexer::LBRACE_SQUARE:
          return Color::YELLOW;
        default:
          return Color::BLUE;
        };
      }();
      std::fill( //
          std::next(std::begin(colors),
              static_cast<std::ptrdiff_t>(token->getStartIndex())),
          std::next(std::begin(colors),
              static_cast<std::ptrdiff_t>(token->getStopIndex()) + 1),
          color);
    }
  };

  replxx.set_completion_callback(completion);
  replxx.set_hint_callback(hint);

  replxx.set_highlighter_callback(highlight);

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
