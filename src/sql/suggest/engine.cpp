#include "engine.hpp"
#include "sql/antlr/SQLiteLexer.h"
#include "sql/suggest/candidate.hpp"
#include <ParserRuleContext.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <ranges>

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
  case SQLiteLexer::BIND_PARAMETER:
  case SQLiteLexer::BLOB_LITERAL:
  case SQLiteLexer::IDENTIFIER:
  case SQLiteLexer::NUMERIC_LITERAL:
  case SQLiteLexer::STRING_LITERAL:
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
  c3.showResult = true;
  c3.ignoredTokens = {
    SQLiteLexer::EOF,
  };
}

auto SuggestionEngine::Suggest(const std::string& prefix) -> Candidates {
  chars.load(prefix, /* lenient = */ false);
  lexer.reset();
  lexer.setInputStream(&chars);
  tokens.setTokenSource(&lexer);
  parser.reset();

  tokens.fill();

  std::vector<std::string> words;
  for (std::size_t i = 0; i < tokens.size(); ++i) {
    const auto& vocabulary = lexer.getVocabulary();
    const auto* token = tokens.get(i);
    auto word = token->toString();
    word += ", type: ";
    word += vocabulary.getSymbolicName(token->getType());
    words.emplace_back(std::move(word));
  }

  if (tokens.size() < 2) {
    return {};
  }

  const auto lastWord = Lowercase(LastWord(prefix));
  const auto isSuitable = [&](const std::string& candidate) {
    return Lowercase(candidate).starts_with(lastWord);
  };

  auto caretTokenIndex = tokens.size() - 2;
  if (lastWord.size() == 0) {
    caretTokenIndex += 1;
  }

  antlr4::ParserRuleContext* context = parser.parse();

  const auto contextStr
      = (context != nullptr) ? context->toStringTree() : "NULL";

  const auto candidates = c3.collectCandidates(
      caretTokenIndex, context, timeoutMs, /* cancel = */ nullptr);
  assert(!candidates.cancelled);

  std::vector<Candidate> result;

  for (const auto& [token, follow] : candidates.tokens) {
    const auto& vocabulary = lexer.getVocabulary();

    auto display = vocabulary.getDisplayName(token);

    if (IsVariableToken(token)) {
      display.insert(std::begin(display), '<');
      display.insert(std::end(display), '>');
    } else {
      display.erase(std::begin(display));
      display.erase(std::prev(std::end(display)));
    }

    if (isSuitable(display) || IsVariableToken(token)) {
      result.emplace_back(std::move(display));
    }
  }

  return result;
}

} // namespace repl::sql::suggest
