#pragma once

#include <string>
#include <vector>

namespace repl {

auto Suggest(const std::string& prefix) -> std::vector<std::string>;

} // namespace repl
