#include "suggest.hpp"
#include "sql.hpp"

#include "antlr-c3/CodeCompletionCore.hpp"

#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace repl {

auto Suggest(const std::string& prefix) -> std::vector<std::string> {
  auto pipeline = SyntaxAnalysisPipeline::SetupFor(prefix);

  auto engine = c3::CodeCompletionCore(pipeline.parser.get());
  engine.showResult = true;
  engine.showDebugOutput = true;

  pipeline.tokens->fill();

  if (pipeline.tokens->size() < 2) {
    std::cerr << "Oops!" << std::endl;
    return {};
  }

  size_t caretTokenIndex = pipeline.tokens->size() - 2;
  {
    std::cerr << "tokens: " << std::endl;
    for (std::size_t i = 0; i < pipeline.tokens->size(); ++i) {
      std::cerr << " " << i << ": " << pipeline.tokens->get(i)->toString()
                << std::endl;
    }
    std::cerr << "caretTokenIndex:" << caretTokenIndex << std::endl;
  }

  antlr4::ParserRuleContext* context = nullptr;
  size_t timeoutMS = 200;
  std::atomic<bool>* cancel = nullptr;
  auto candidates
      = engine.collectCandidates(caretTokenIndex, context, timeoutMS, cancel);
  assert(!candidates.cancelled);

  std::vector<std::string> result;
  for (const auto& [token, following] : candidates.tokens) {
    const auto& vocabulary = pipeline.lexer->getVocabulary();
    std::cerr << "token: " << vocabulary.getDisplayName(token) << ""
              << std::endl;
    for (const auto next : following) {
      std::cerr << "  next -> symbolic: '" << vocabulary.getSymbolicName(next) << "'"
                << ", literal: '" << vocabulary.getLiteralName(next) << "'"
                << std::endl;
    }
  }
  for (const auto& [index, candidate] : candidates.rules) {
    for (const auto& rule : candidate.ruleList) {
      const auto& parser = *pipeline.parser;
      std::cerr << "index: " << index //
                << ", startTokenIndex: " << candidate.startTokenIndex //
                << ", rule: " << parser.getRuleNames()[rule] //
                << std::endl;
    }
  }
  return result;
}

} // namespace repl
