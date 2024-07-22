#include "suggest.hpp"
#include "sql.hpp"

#include "antlr/c3/CodeCompletionCore.hpp"

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

  antlr4::ParserRuleContext* context = nullptr;
  size_t timeoutMS = 200;
  std::atomic<bool>* cancel = nullptr;
  auto candidates
      = engine.collectCandidates(caretTokenIndex, context, timeoutMS, cancel);
  assert(!candidates.cancelled);

  return {};
}

} // namespace repl
