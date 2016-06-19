cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project("bureaucracy"
        LANGUAGES CXX
        VERSION 0.1.0)

enable_testing()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_library(bureaucracy SHARED
                threadpool.cpp
           )
set_target_properties(bureaucracy PROPERTIES
                        VERSION ${PROJECT_VERSION}
                        SOVERSION ${PROJECT_VERSION_MAJOR}
                     )

add_executable(bureaucracy_tests
                    threadpool_test.cpp
              )
target_link_libraries(bureaucracy_tests
                            gtest_main
                            gtest
                            bureaucracy
                     )
add_test(bureaucracy_tests bureaucracy_tests)

install(TARGETS
            bureaucracy
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin
       )

install(FILES
            include/bureaucracy/threadpool.hpp
        DESTINATION include/bureaucracy
       )
