<div align="center">
  <h1>🛠️ RSTools (RSLib)</h1>
  <p><em>The versatile, all-in-one C++ utility toolset of Romain Super Librairie</em></p>

  <p align="center">
    A comprehensive C++17/20 utility library featuring string manipulation, command-line parsing, logging, and math tools for any project.
    <br />
    <a href="#-modules-overview"><strong>Explore the Modules</strong></a>
    ·
    <a href="#-contributing-community-first"><strong>Become a Contributor</strong></a>
    ·
    <a href="https://github.com/your_username/RSTools/issues">Report a Bug</a>
  </p>
</div>

<div align="center">
  <img src="https://img.shields.io/badge/C%2B%2B_17%2F20-00599C?style=flat-square&logo=c%2B%2B&logoColor=white" alt="C++17/20">
  <img src="https://img.shields.io/badge/CMake-064F8C?style=flat-square&logo=cmake&logoColor=white" alt="CMake">
  <img src="https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey?style=flat-square" alt="Supported Platforms">
  <br>
  <img src="https://img.shields.io/github/v/release/NetcatRunner/RSTools?style=flat-square&logo=github&color=blue" alt="Latest Release">
  <img src="https://img.shields.io/github/license/NetcatRunner/RSTools?style=flat-square&color=yellow" alt="License: MIT">
  <br>
  <img src="https://img.shields.io/badge/Contributions-Welcome-4EAA25?style=flat-square" alt="Contributions Welcome">
  <img src="https://img.shields.io/badge/Open_Source-%E2%9D%A4-red?style=flat-square" alt="Open Source Love">
  <br>
  <a href="https://github.com/NetcatRunner/RSTools/actions/workflows/ci.yaml">
    <img src="https://github.com/NetcatRunner/RSTools/actions/workflows/ci.yaml/badge.svg" alt="Build Status">
  </a>
</div>

---

## 📖 Table of Contents

  - [About the Project](https://www.google.com/search?q=%23-about-the-project)
  - [Available Modules](https://www.google.com/search?q=%23-available-modules)
  - [Getting Started](https://www.google.com/search?q=%23-getting-started)
  - [Usage Example](https://www.google.com/search?q=%23-usage-example)
  - [Community & Contributing](https://www.google.com/search?q=%23-community--contributing)
  - [License](https://www.google.com/search?q=%23-license)

-----

## 🚀 About the Project

**RSTools** is the foundational utility toolkit of **RSLib** (*Romain Super Librairie*).

Born from the need to stop rewriting the same boilerplate code across different applications, RSTools provides a versatile, highly reusable set of C++ utilities designed to make everyday development faster, safer, and more efficient. Whether you are building a small personal script or a large-scale system architecture, RSTools has you covered with ready-to-use tools.

## 🧰 Available Modules

Based on a clean and modular architecture, RSTools is divided into several specialized namespaces/folders. You can include the global `<RST/RST.hpp>` or cherry-pick what you need:

  * 🔐 **Crypto:** Lightweight cryptographic utilities and hashing functions.
  * 📝 **Log:** A flexible, thread-safe logging system for easy debugging and monitoring.
  * 🧮 **Maths:** High-performance mathematical helpers and calculations.
  * ⚙️ **Parser:** Robust command-line argument (CLI) parsing and data extraction.
  * 🔤 **String:** Advanced string manipulation (splitting, joining, trimming, formatting).
  * 💻 **System:** OS-level interactions, file system utilities, and environment management.
  * ⏱️ **Time:** Chrono wrappers for easy benchmarking, timers, and date manipulation.

-----

## 🛠 Getting Started

To integrate RSTools into your C++ project, follow these simple steps using CMake.

### Prerequisites

  * A compiler supporting **C++17** or **C++20** (GCC, Clang, or MSVC).
  * **CMake** (3.15 or higher).

### Installation via CMake

1.  Clone the repository into your project's directory (e.g., in a `lib/` or `vendor/` folder):

    ```bash
    git clone https://github.com/NetcatRunner/RSTools.git lib/RSTools
    ```

2.  Add the library to your `CMakeLists.txt`:

    ```cmake
    # Add the RSTools directory
    add_subdirectory(lib/RSTools)

    # Link it to your main executable
    target_link_libraries(your_executable PRIVATE RSTools)
    ```

-----

## 💻 Usage Example

Here is a quick glimpse of how easy it is to use RSTools in your main application:

```cpp
#include <RST/RST.hpp>

#include <memory>
#include <iostream>

int add(int a, int b) {return a + b;}

TEST_CASE(simple_test) {
  CHECK(3 == 3);
  CHECK(add(4, 5) == 9);
};

int main(int argc, const char** argv) {
    // Example: Using the Logger and Parser

    RST::Log::Logger logger("TestLogger");

    logger.addSink(std::make_shared<RST::Log::ConsoleSink>());

    logger.info("Application started.");

    RST::Parser::ArgParser parser;
    parser.addFlag({"--help", "-h"}, "Help message");

    try {
        parser.parse(argc, argv);
    } catch (const RST::Parser::HelpRequested& h) {
        parser.printHelp();
        return 0;
    } catch (const std::exception& e) {
        logger.error(e.what());
        parser.printHelp();
        return 1;
    }

    // Example: Using String utilities
    std::string text = "   Hello RSTools!   ";
    std::cout << RST::String::LtrimCopy(text) << "\n";

    logger.log(RST::Log::LogLevel::Info, "Application stop.");
    return RUN_ALL_TESTS();
}

```

-----

## 🤝 Community & Contributing

**RSTools is a community-driven library.** While it started as "Romain Super Librairie", it has grown into a collaborative space. We strongly believe that the best tools are built together. **Anyone who uses this library is highly encouraged to become an active contributor\!** Whether you want to fix a bug, add a new math function, improve the logger, or simply fix a typo in the documentation, your help is welcome.

### How to contribute:

1.  **Fork** the Project
2.  **Create** your Feature Branch (`git checkout -b feature/AmazingFeature`)
3.  **Commit** your Changes (`git commit -m 'Add some AmazingFeature'`)
4.  **Push** to the Branch (`git push origin feature/AmazingFeature`)
5.  **Open** a Pull Request

Join the community and help us build the ultimate C++ utility toolkit\!

-----

## 📄 License

Distributed under the MIT License. See `LICENSE` for more information.
