cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project("bureaucracy"
        LANGUAGES CXX
        VERSION 0.1.0)

enable_testing()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)

include_directories(include)

set(bureaucracy_srcs
        diligentworker.cpp
        priorityworker.cpp
        serialworker.cpp
        threadpool.cpp
        timer.cpp
   )

add_library(bureaucracy SHARED ${bureaucracy_srcs})
set_target_properties(bureaucracy PROPERTIES
                        VERSION ${PROJECT_VERSION}
                        SOVERSION ${PROJECT_VERSION_MAJOR}
                     )

add_library(bureaucracy-static STATIC ${bureaucracy_srcs})
set_target_properties(bureaucracy-static PROPERTIES
                        OUTPUT_NAME bureaucracy
                     )

find_library(GTEST gtest)
find_library(GTEST_MAIN gtest_main)

message(STATUS "gtest location: ${GTEST}")
message(STATUS "gtest_main location: ${GTEST_MAIN}")

if(GTEST AND GTEST_MAIN)
    set(BUILD_TESTS ON)
else()
    message(WARNING "Could not find gtest libraries; unit tests disabled")
endif()

if(BUILD_TESTS)
    function(create_test name)
        add_executable(${name} ${ARGN})
        target_link_libraries(${name}
                                ${GTEST}
                                ${GTEST_MAIN}
                                bureaucracy
                             )
        add_test(${name} ${name})
    endfunction()

    create_test(worker_tests
                    diligentworker_test.cpp
                    priorityworker_test.cpp
                    serialworker_test.cpp
                    threadpool_test.cpp
               )
    create_test(timer_tests
                    timer_test.cpp
               )
endif()

find_program(DOXYGEN "doxygen")
if(DOXYGEN)
    find_program(DOT "dot")
    if(DOT)
        set(HAVE_DOT "YES")
    else()
        set(HAVE_DOT "NO")
    endif()
    configure_file(doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/doxyfile" @ONLY)

    add_custom_target(docs)
    add_custom_command(TARGET docs
                            COMMAND "${DOXYGEN}"
                            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                      )
endif()

install(TARGETS
            bureaucracy
            bureaucracy-static
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin
       )

install(FILES
            include/bureaucracy/diligentworker.hpp
            include/bureaucracy/priorityworker.hpp
            include/bureaucracy/serialworker.hpp
            include/bureaucracy/threadpool.hpp
            include/bureaucracy/timer.hpp
        DESTINATION include/bureaucracy
       )
