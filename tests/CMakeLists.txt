file(GLOB_RECURSE TEST_SOURCES 
    "maths/*.cpp"
    "log/*.cpp"
    "main.cpp"
)

add_executable(run_tests ${TEST_SOURCES})

target_link_libraries(run_tests PRIVATE RST)

add_test(NAME TestBasique_RST COMMAND run_tests)
