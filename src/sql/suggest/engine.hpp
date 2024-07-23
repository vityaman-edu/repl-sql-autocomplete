#pragma once

#include "antlr/c3/CodeCompletionCore.hpp"
#include "sql/antlr/YQLLexer.h"
#include "sql/antlr/YQLParser.h"
#include "sql/suggest/candidate.hpp"
#include <ANTLRInputStream.h>
#include <cstddef>

namespace repl::sql::suggest {

class SuggestionEngine final {
public:
  explicit SuggestionEngine();

  auto Suggest(const std::string& prefix) -> Candidates;

private:
  auto Reset(const std::string& prefix) -> void;
  auto C3Suggest(std::size_t caretTokenIndex) -> c3::CandidatesCollection;
  auto PostProcessed(std::size_t token) -> std::string;

  auto DebugTokens() -> std::vector<std::string>;

  antlr4::ANTLRInputStream chars;
  YQLLexer lexer;
  antlr4::BufferedTokenStream tokens;
  YQLParser parser;
  c3::CodeCompletionCore c3;

  const std::size_t timeoutMs = 200;
};

} // namespace repl::sql::suggest
