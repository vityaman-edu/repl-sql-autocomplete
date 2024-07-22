#include "engine.hpp"
#include "sql/suggest/candidate.hpp"
#include <cassert>

namespace repl::sql::suggest {

SuggestionEngine::SuggestionEngine()
    : lexer(&chars)
    , tokens(&lexer)
    , parser(&tokens)
    , c3(&parser) {
  c3.showResult = true;
  c3.showDebugOutput = true;
}

auto SuggestionEngine::Suggest(const std::string& prefix) -> Candidates {
  chars.load(prefix, /* lenient = */ false);
  tokens.fill();

  if (tokens.size() < 2) {
    return {};
  }

  const auto caretTokenIndex = tokens.size() - 2;
  const auto candidates = c3.collectCandidates(caretTokenIndex,
      /* context = */ nullptr, timeoutMs, /* cancel = */ nullptr);
  assert(!candidates.cancelled);

  std::vector<Candidate> result;

  for (const auto& [token, follow] : candidates.tokens) {
    const auto& vocabulary = lexer.getVocabulary();

    auto display = vocabulary.getDisplayName(token);
    display = display.substr(1, display.size() - 2);

    result.emplace_back(std::move(display));
  }

  return result;
}

} // namespace repl::sql::suggest
