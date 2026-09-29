#pragma once

#include "ArgParser.hpp"

/// @namespace RST::Parser
/// Command-line parser with subcommands, typed and validated values and generated help.
///
/// Nothing throws: ArgParser::parse() returns a ParseResult holding the help, version or error text to print.
