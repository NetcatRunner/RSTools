<div align="center">
  <h1>🛠️ RSTools (RSLib)</h1>
  <p><em>The versatile, all-in-one C++20 utility toolset of R Super Librairie</em></p>

  <p>
    Strings, maths, colors, system, time, command-line parsing, logging and threads for any C++ project.
    <br />
    <a href="https://netcatrunner.github.io/RSTools/"><strong>Documentation</strong></a>
    ·
    <a href="https://github.com/NetcatRunner/RSTools/tree/main/examples"><strong>Examples</strong></a>
    ·
    <a href="https://github.com/NetcatRunner/RSTools/issues">Report a Bug</a>
  </p>

  <img src="https://img.shields.io/badge/C%2B%2B-20-00599C?style=flat-square&logo=c%2B%2B&logoColor=white" alt="C++20">
  <img src="https://img.shields.io/badge/CMake-3.28%2B-064F8C?style=flat-square&logo=cmake&logoColor=white" alt="CMake 3.28+">
  <img src="https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey?style=flat-square" alt="Supported Platforms">
  <br>
  <img src="https://img.shields.io/github/v/release/NetcatRunner/RSTools?style=flat-square&logo=github&color=blue" alt="Latest Release">
  <img src="https://img.shields.io/github/license/NetcatRunner/RSTools?style=flat-square" alt="License: MIT">
  <a href="https://github.com/NetcatRunner/RSTools/actions/workflows/ci.yaml"><img src="https://github.com/NetcatRunner/RSTools/actions/workflows/ci.yaml/badge.svg" alt="Build Status"></a>
</div>

---

## 📖 Table of Contents

  - [About the Project](#-about)
  - [Available Modules](#-modules)
  - [Getting Started](#-getting-started)
  - [Usage Example](#-usage-example)
  - [Community & Contributing](#-community--contributing)
  - [License](#-license)

---

## 🚀 About

**RSTools** is the foundational utility toolkit of **RSLib** (*R Super Librairie*).

Born from the need to stop rewriting the same boilerplate code across different applications, RSTools provides a versatile, highly reusable set of C++ utilities designed to make everyday development faster, safer, and more efficient. Whether you are building a small personal script or a large-scale system architecture, RSTools has you covered with ready-to-use tools.

## 🧰 Modules

Include `<RST/RST.hpp>` for everything, or only the header of a module, such as `<RST/string/String.hpp>`.

| Module | Namespace | Highlights |
|---|---|---|
| 🔤 String | `RST::String` | Trim, split, join, replace, case, number parsing, compile-time hashing |
| 🧮 Maths | `RST::Maths` | Constants, interpolation, random numbers, 2D/3D vectors, matrices |
| 🎨 Color | `RST::Color` | RGBA colors, hex and HSV conversions, blending |
| 💻 System | `RST::System` | CPU, memory, OS and disk info, environment variables, files, paths |
| ⏱️ Time | `RST::Time` | Stopwatch, frame timer, date formatting |
| ⚙️ Parser | `RST::Parser` | Command-line parser with subcommands, validation and generated help |
| 📝 Log | `RST::Log` | Thread-safe loggers, console and file sinks, patterns, macros |
| 🧵 Threads | `RST::Threads` | Thread pool with task priorities |
| ✅ Test | `RSTest.hpp` | Minimal unit-test macros |
| 🔐 Crypto | `RST::Crypto` | *Coming soon* |

## 🛠 Getting Started

**Requirements:** a C++20 compiler (GCC 13+ or Clang 17+) and CMake 3.28+.

Add RSTools to your `CMakeLists.txt` with FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(RSTools
    GIT_REPOSITORY https://github.com/NetcatRunner/RSTools.git
    GIT_TAG v3.1.0
)
FetchContent_MakeAvailable(RSTools)

target_link_libraries(my_app PRIVATE RST::RST)
```

Or clone it into your project and use `add_subdirectory(lib/RSTools)` instead of FetchContent.

| CMake option | Default | Description |
|---|---|---|
| `RST_BUILD_TESTS` | `ON` for the main project | Build the unit tests |
| `RST_BUILD_EXAMPLES` | `ON` for the main project | Build the examples |
| `RST_BUILD_DOCS` | `OFF` | Build the API documentation (requires Doxygen) |

## 💻 Usage Example

Here is a quick glimpse of how easy it is to use RSTools in your main application:

```cpp
#include <RST/RST.hpp>

int main(int argc, char** argv) {
    RST::Parser::ArgParser parser("greet", "Says hello.");
    parser.addOption({"-n", "--count"}, "How many greetings").integer().range(1, 10).defaultValue(1);

    // --help, --version and invalid arguments end here: nothing throws.
    if (auto result = parser.parse(argc, argv); !result) {
        result.print();
        return result.exitCode();
    }

    auto log = RST::Log::Registry::init("greet");
    const std::string name = RST::String::trimCopy("   RSTools   ");
    for (int i = 0; i < parser.get<int>("--count").value_or(1); ++i)
        log->info("Hello {}!", name);
}
```

More in the [examples](https://github.com/NetcatRunner/RSTools/tree/main/examples) and the [documentation](https://netcatrunner.github.io/RSTools/).

## 🤝 Contributing

**RSTools is a community-driven library.** Whether you want to fix a bug, add a function, improve the logger or fix a typo, your help is welcome: see [CONTRIBUTING.md](CONTRIBUTING.md) to get started, and [CHANGELOG.md](CHANGELOG.md) for what changed in each release.

## 📄 License

Distributed under the MIT License. See [LICENSE](https://github.com/NetcatRunner/RSTools/blob/main/LICENSE) for more information.
