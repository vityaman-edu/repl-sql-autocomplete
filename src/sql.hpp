#pragma once

#include "sql/antlr/SQLiteLexer.h"
#include "sql/antlr/SQLiteParser.h"

#include <ANTLRInputStream.h>

#include <memory>
#include <string>

namespace repl {

struct SyntaxAnalysisPipeline {
  std::unique_ptr<antlr4::ANTLRInputStream> chars;
  std::unique_ptr<SQLiteLexer> lexer;
  std::unique_ptr<antlr4::BufferedTokenStream> tokens;
  std::unique_ptr<SQLiteParser> parser;

  static auto SetupFor(const std::string& text) -> SyntaxAnalysisPipeline;
};

auto ParseToAST(const std::string&) -> std::string;

} // namespace repl
