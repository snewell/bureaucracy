add_sources(
    "${CMAKE_CURRENT_LIST_DIR}/timer.cpp"
)
add_headers(
    timer.hpp
)

create_test(timer_tests
    "${CMAKE_CURRENT_LIST_DIR}/timer_test.cpp"
)
