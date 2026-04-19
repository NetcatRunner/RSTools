add_executable(run_tests main.cpp)

target_link_libraries(run_tests PRIVATE RST)

add_test(NAME TestBasique_RST COMMAND run_tests)
