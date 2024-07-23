#include "engine.hpp"
#include "sql/antlr/YQLLexer.h"
#include "sql/suggest/candidate.hpp"
#include <ParserRuleContext.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <sstream>
#include <string>

namespace repl::sql::suggest {

namespace {

auto IsWhitespace(char symbol) -> bool {
  return symbol == ' ';
}

auto LastWordIndex(const std::string& text) -> std::size_t {
  const auto length = static_cast<std::int64_t>(text.size());
  for (std::int64_t i = length - 1; 0 <= i; --i) {
    if (IsWhitespace(text[i])) {
      return i + 1;
    }
  }
  return 0;
}

auto LastWord(const std::string& text) -> std::string {
  return std::string(text.substr(LastWordIndex(text)));
}

auto Lowercase(const std::string& text) -> std::string {
  return text
      | std::views::transform([](auto symbol) { return std::tolower(symbol); })
      | std::ranges::to<std::string>();
}

auto IsVariableToken(std::size_t token) -> bool {
  switch (token) {
  case YQLLexer::STRING_VALUE:
  case YQLLexer::INTEGER_VALUE:
  case YQLLexer::ID_PLAIN:
  case YQLLexer::ID_QUOTED:
    return true;
  default:
    return false;
  }
}

} // namespace

SuggestionEngine::SuggestionEngine()
    : lexer(&chars)
    , tokens(&lexer)
    , parser(&tokens)
    , c3(&parser) {
  lexer.removeErrorListeners();
  parser.removeErrorListeners();

  c3.showResult = false;
  c3.ignoredTokens = {
    YQLLexer::EOF,
  };
}

auto SuggestionEngine::Suggest(const std::string& prefix) -> Candidates {
  Reset(prefix);

  const auto lastWord = Lowercase(LastWord(prefix));
  const auto isSuitable = [&](const std::string& candidate) {
    return Lowercase(candidate).starts_with(lastWord);
  };

  tokens.fill();
  const auto caretTokenIndex
      = (2 <= tokens.size()) ? tokens.size() - 2 : tokens.size() - 1;

  const auto candidates = C3Suggest(caretTokenIndex);

  std::vector<Candidate> result;

  for (const auto& [token, follow] : candidates.tokens) {
    auto candidate = PostProcessed(token);
    if (isSuitable(candidate) || IsVariableToken(token)) {
      result.emplace_back(std::move(candidate));
    }
  }

  for (auto& candidate : result) {
    candidate += ' ';
  }

  return result;
}

auto SuggestionEngine::Reset(const std::string& prefix) -> void {
  chars.load(prefix, /* lenient = */ false);
  lexer.reset();
  lexer.setInputStream(&chars);
  tokens.setTokenSource(&lexer);
  parser.reset();
}

auto SuggestionEngine::DebugTokens() -> std::vector<std::string> {
  const auto& vocabulary = lexer.getVocabulary();
  return tokens.getTokens(0, tokens.size())
      | std::views::transform([&](auto* token) {
          const auto symbolic = vocabulary.getSymbolicName(token->getType());

          std::stringstream word;
          word << token->toString() << ", type: " << symbolic;
          return word.str();
        })
      | std::ranges::to<std::vector>();
}

auto SuggestionEngine::C3Suggest(
    std::size_t caretTokenIndex) -> c3::CandidatesCollection {
  antlr4::ParserRuleContext* context = parser.sql_query();
  auto candidates = c3.collectCandidates(
      caretTokenIndex, context, timeoutMs, /* cancel = */ nullptr);
  assert(!candidates.cancelled);
  return candidates;
}

auto SuggestionEngine::PostProcessed(std::size_t token) -> std::string {
  const auto& vocabulary = lexer.getVocabulary();

  auto display = vocabulary.getDisplayName(token);

  if (IsVariableToken(token)) {
    display.insert(std::begin(display), '<');
    display.insert(std::end(display), '>');
  } else if (display.starts_with('\'')) {
    assert(display.ends_with('\''));
    display.erase(std::begin(display));
    display.erase(std::prev(std::end(display)));
  }

  return display;
}

} // namespace repl::sql::suggest
