#pragma once

#include "RST/system/Environment.hpp"
#include "RST/system/File.hpp"
#include "RST/system/Paths.hpp"
#include "RST/system/SystemInfo.hpp"

/// @namespace RST::System
/// Portable access to system information, environment variables, files and paths.
///
/// Nothing throws: failures return `std::nullopt`, `false`, 0 or an empty value.
