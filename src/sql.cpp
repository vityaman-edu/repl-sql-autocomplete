#include "sql.hpp"
#include "sql/antlr/SQLiteLexer.h"
#include "sql/antlr/SQLiteParser.h"

#include <ANTLRInputStream.h>
#include <BufferedTokenStream.h>
#include <CharStream.h>
#include <UnbufferedCharStream.h>
#include <memory>

namespace repl {

auto SyntaxAnalysisPipeline::SetupFor(
    const std::string& text) -> SyntaxAnalysisPipeline {
  auto chars = std::make_unique<antlr4::ANTLRInputStream>(text);
  auto lexer = std::make_unique<SQLiteLexer>(chars.get());
  auto tokens = std::make_unique<antlr4::BufferedTokenStream>(lexer.get());
  auto parser = std::make_unique<SQLiteParser>(tokens.get());
  return {
    .chars = std::move(chars),
    .lexer = std::move(lexer),
    .tokens = std::move(tokens),
    .parser = std::move(parser),
  };
}

auto ParseToAST(const std::string& text) -> std::string {
  auto pipeline = SyntaxAnalysisPipeline::SetupFor(text);
  return pipeline.parser->parse()->toStringTree(true);
}

} // namespace repl
