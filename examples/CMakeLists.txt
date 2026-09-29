foreach(example IN ITEMS quickstart strings graphics system parser log_basic log_advanced)
    add_executable(example_${example} ${example}.cpp)
    target_link_libraries(example_${example} PRIVATE RST::RST)
endforeach()
