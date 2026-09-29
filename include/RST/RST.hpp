#pragma once

#include "RST/crypto/Crypto.hpp"
#include "RST/log/Format/Formatter.hpp"
#include "RST/log/Log.hpp"
#include "RST/log/LogMessage.hpp"
#include "RST/log/LogRegistry.hpp"
#include "RST/log/ScopedTimer.hpp"
#include "RST/log/Sinks/Sinks.hpp"
#include "RST/maths/Maths.hpp"
#include "RST/parser/Parser.hpp"
#include "RST/string/String.hpp"
#include "RST/system/System.hpp"
#include "RST/threads/Threads.hpp"
#include "RST/time/Time.hpp"
#include "RST/color/Color.hpp"
#include "RST/test/RSTest.hpp"

/// @namespace RST
/// Root namespace of RSTools. Each module lives in its own namespace, such as RST::String.

/// @example quickstart.cpp
/// A short tour: command line, logging and strings.

/// @example strings.cpp
/// Trimming, splitting, joining, parsing numbers and switching on strings.

/// @example graphics.cpp
/// Vectors, a transformation matrix, seeded random numbers and colors.

/// @example system.cpp
/// System information, environment variables, files and a timer.

/// @example parser.cpp
/// Subcommands, typed options with defaults and positional arguments.

/// @example log_basic.cpp
/// Default logger, levels, macros and named loggers.

/// @example log_advanced.cpp
/// Sinks with their own level and pattern, and the logger registry.
