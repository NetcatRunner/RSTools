# Changelog

All notable changes to RSTools are listed here. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and versions follow [Semantic Versioning](https://semver.org/).

## Unreleased

## 3.1.0 - 2026-09-29

### Added

- API documentation generated with Doxygen (`RST_BUILD_DOCS`) and published on [GitHub Pages](https://netcatrunner.github.io/RSTools/).
- Examples for every module, built with `RST_BUILD_EXAMPLES`.
- `RST::RST` CMake target alias, for FetchContent and `add_subdirectory` users.
- README, contributing guide and this changelog.

### Changed

- Tests and examples are only built by default when RSTools is the main project.

### Fixed

- `<RST/RST.hpp>` now includes the threads module.

## 3.0.0 - 2026-09-29

### Changed

- **Breaking:** functions use lowerCamelCase: `SplitString` becomes `splitString`, `GetCpuCores` becomes `getCpuCores`, and so on.
- **Breaking:** the command-line parser is rewritten. `ArgParser::parse()` no longer throws: it returns a `ParseResult` holding the help, version or error text to print.
- String functions take `std::string_view`, and trimming accepts any set of characters.

### Added

- String: case-insensitive comparisons, replacements, `parseNumber()`, `parseBool()`, compile-time `hash()` and `_hash` literal.
- Maths: `inverseLerp()`, `smoothstep()`, `wrap()`, `Matrix::determinant()` and `Matrix::transformDirection()`.
- Color: hex string, float and HSV conversions, blending, grayscale and more named colors.
- System: environment variables, file helpers, executable and home paths, CPU name, process memory and host name.
- Time: `Timer` can be stopped and resumed; `formatTime()` and `formatNow()` format dates.
- Parser: `--version`, `integer()` and `number()` values, `get<T>()` returning `std::optional`, flag counts (`-vvv`)
  and suggestions for mistyped options.
