#include "sql.hpp"
#include "sql/antlr/SQLiteLexer.h"
#include "sql/antlr/SQLiteParser.h"

#include <ANTLRInputStream.h>
#include <BufferedTokenStream.h>
#include <CharStream.h>
#include <UnbufferedCharStream.h>

namespace repl {

auto ParseToAST(const std::string& text) -> std::string {
  auto chars = antlr4::ANTLRInputStream(text);
  auto lexer = SQLiteLexer(&chars);
  auto tokens = antlr4::BufferedTokenStream(&lexer);
  auto parser = SQLiteParser(&tokens);
  auto* tree = parser.parse();
  return tree->toStringTree(&parser);
}

} // namespace repl
