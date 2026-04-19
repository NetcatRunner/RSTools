#pragma once

#include <iostream>
#include <string_view>
#include <source_location>
#include <vector>
#include <cassert>
#include <exception>
#include <chrono>

namespace RST::RSTester {

    constexpr const char* COLOR_GREEN   = "\033[32m";
    constexpr const char* COLOR_RED     = "\033[31m";
    constexpr const char* COLOR_YELLOW  = "\033[33m";
    constexpr const char* COLOR_RESET   = "\033[0m";


    struct TestAbortException : public std::exception {};

    template <typename T>
    constexpr std::string try_tostr(const T& t){
    if constexpr(requires{ (std::ostringstream() << t).str(); }){
        return (std::ostringstream() << std::boolalpha << t).str();
    } else {
        return "<obj>";
    }
    }

    struct info_t {
        std::string_view expr = "";
        std::source_location sl = std::source_location::current();
        bool result = false;
        std::string opt_lhs = "";
        std::string opt_rhs = "";
    };

    template <class A>
    struct check_t {
        A a;
        info_t& i;
    #define CHECK_T_COMP_OP(OP)             \
        template <class B>                  \
        info_t operator OP(B&& b) && {      \
            i.opt_lhs = try_tostr(a);       \
            i.opt_rhs = try_tostr(b);       \
            i.result = (a OP b);            \
            return std::move(i);            \
        } static_assert(true)
        CHECK_T_COMP_OP(==);
        CHECK_T_COMP_OP(!=);
        CHECK_T_COMP_OP(> );
        CHECK_T_COMP_OP(>=);
        CHECK_T_COMP_OP(< );
        CHECK_T_COMP_OP(<=);
    #undef CHECK_T_COMP_OP
        operator info_t() && {
            i.opt_lhs = try_tostr(a);
            i.result = a;
            return std::move(i);
        }
    };

    struct destruct_t {
        info_t i;
        template <class A>
        check_t<A> operator<(A&& a) && {
            return check_t<A>{std::forward<A>(a), i};
        }
    };

    #define DECOMPOSE_CMP_OP(expr, lit) RST::RSTester::destruct_t{RST::RSTester::info_t{lit ""}} < expr

    inline void check_internal(info_t i, std::string_view additional_info = ""){
        if (!i.result) {
            std::cerr << "  " << i.sl.file_name() << ":" << i.sl.line() << " -> " 
                      << COLOR_RED << "FAILED\n" << COLOR_RESET
                      << "    Expression: " << i.expr << "\n"
                      << "    Values:     " << i.opt_lhs << " and " << i.opt_rhs << "\n";
            throw TestAbortException();
        }
    }

    #define CHECK(e, ...) RST::RSTester::check_internal(DECOMPOSE_CMP_OP(e, #e) __VA_OPT__(,) __VA_ARGS__)

    struct testfunction{
        using function_sig = void();
        function_sig& function;
        std::string_view name;
    };

    class TestRunner {
    public:
        static TestRunner& GetInstance() {
            static TestRunner instance;
            return instance;
        }

        int AddTest(testfunction test) {
            _tests.push_back(test);
            return 0;
        }

        int RunAllTests() {
            auto time_start_global = std::chrono::high_resolution_clock::now();

            std::cout << COLOR_YELLOW << "[==========] " << COLOR_RESET  << "Running " << _tests.size() << " tests.\n";
            int failed_count = 0;

            for (const auto& t : _tests) {
                std::cout << COLOR_YELLOW << "\n[ RUN ] " << COLOR_RESET << t.name << "\n";
                bool passed = true;
                auto time_start_test = std::chrono::high_resolution_clock::now();

                try {
                    t.function();
                } catch (const TestAbortException&) {
                    std::cout << "    -> Test aborted due to fatal failure.\n";
                    failed_count++;
                    passed = false;
                } catch (const std::exception& e) {
                    std::cerr << "    -> Unhandled exception: " << e.what() << "\n";
                    failed_count++;
                    passed = false;
                }

                auto time_end_test = std::chrono::high_resolution_clock::now();
                auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_end_test - time_start_test).count();

                if (passed) {
                    std::cout << COLOR_GREEN << "[ OK ] " << COLOR_RESET << t.name << " (" << duration_ms << " ms)\n";
                } else {
                    std::cout << COLOR_RED << "[ TEST FAILED ] " << COLOR_RESET << t.name << " (" << duration_ms << " ms)\n";
                }
            }
            auto time_end_global = std::chrono::high_resolution_clock::now();
            auto total_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_end_global - time_start_global).count();

            std::cout << COLOR_YELLOW << "\n[==========] " << COLOR_RESET << "Test execution finished. (" << total_duration_ms << " ms total)\n\n";

            if (failed_count > 0) {
                std::cout << COLOR_RED << "[ FAILED ] " << COLOR_RESET  << failed_count << " tests failed.\n";
            } else {
                std::cout << COLOR_GREEN << "[ PASSED ] " << COLOR_RESET  << "All tests passed!\n";
            }
            return failed_count;
        }
    private:
        TestRunner() = default;
        std::vector<testfunction> _tests = {};
    };

    #define CONCAT_IMPL(x, y) x##y
    #define CONCAT(x, y) CONCAT_IMPL(x, y)
    #define TEST_CASE(name) \
        static void name(); \
        static int CONCAT(impl, __LINE__) = RST::RSTester::TestRunner::GetInstance().AddTest({name, #name}); \
        static void name()

    #define RUN_ALL_TESTS() RST::RSTester::TestRunner::GetInstance().RunAllTests()
}
