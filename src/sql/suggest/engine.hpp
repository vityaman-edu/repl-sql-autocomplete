#pragma once

#include "antlr/c3/CodeCompletionCore.hpp"
#include "sql/antlr/YQLLexer.h"
#include "sql/antlr/YQLParser.h"
#include "sql/suggest/candidate.hpp"
#include <ANTLRInputStream.h>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

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

  std::unordered_map<std::size_t, std::vector<std::string>> names;

  const std::size_t timeoutMs = 200;
};

} // namespace repl::sql::suggest
