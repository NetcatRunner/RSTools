# Contributing

Thanks for helping to improve RSTools! Bug reports, ideas and pull requests are all welcome.

## Report a bug or suggest a feature

Open an [issue](https://github.com/NetcatRunner/RSTools/issues) with a short description, the steps to reproduce the problem, and your compiler and operating system.

## Build and test

```bash
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Tests are plain `.cpp` files in `tests/<module>/`, picked up automatically:

```cpp
#include <RST/RST.hpp>

TEST_CASE(test_trim_removes_spaces) {
    CHECK(RST::String::trimCopy("  hi  ") == "hi");
}
```

To build the documentation (requires [Doxygen](https://www.doxygen.nl)), then open `build/docs/html/index.html`:

```bash
cmake -S . -B build -DRST_BUILD_DOCS=ON
cmake --build build --target docs
```

## Project layout

| Path | Contents |
|---|---|
| `include/RST/` | Public headers, one folder per module |
| `src/` | Implementation |
| `tests/` | Unit tests, one folder per module |
| `examples/` | Small programs, also shown in the documentation |
| `docs/` | Documentation build |

## Code style

- C++20 without warnings: the library builds with `-Wall -Wextra -Wpedantic -Werror`.
- Functions and variables in `lowerCamelCase`, types and constants in `PascalCase`, private members start with `_`.
- Report expected failures with `std::optional`, `bool` or a result type instead of exceptions, and use `assert` for programming errors.
- Prefer simple, readable code over clever optimizations.
- Document public functions and classes with a short `///` comment, like the existing headers.
- Keep the line endings of the files you edit.

## Pull requests

1. Fork the repository and create a branch: `git checkout -b fix/trim-empty-string`.
2. Write focused commits named `[TYPE] - Short description`, where `TYPE` is `ADD`, `UPDATE`, `FIX`, `REFACTOR`, `DOCS`, `TEST` or `BUILD`.
3. Check that the build and the tests pass, and note your change at the top of [CHANGELOG.md](CHANGELOG.md), under an `Unreleased` heading.
4. Open a pull request that explains what changes and why.

Releases follow [Semantic Versioning](https://semver.org/): a breaking change bumps the major version.
