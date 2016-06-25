set(timer_srcs
        "${CMAKE_CURRENT_SOURCE_DIR}/timer.cpp"
    PARENT_SCOPE)

create_test(timer_tests
                timer_test.cpp
           )
