cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0)

project("bureaucracy"
        LANGUAGES CXX
        VERSION 0.1.1)

include(CTest)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
include(CPack)

option(BUILD_TESTS "Build optional tests" ON)
option(BUILD_DOCS  "Build documenttaion" ON)

enable_testing()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)

include_directories(include)

if(BUILD_TESTS)
    find_library(GTEST gtest)
    find_library(GTEST_MAIN gtest_main)
    message(STATUS "gtest location: ${GTEST}")
    message(STATUS "gtest_main location: ${GTEST_MAIN}")
    if(NOT GTEST OR NOT GTEST_MAIN)
        message(WARNING "Could not find gtest libraries; unit tests disabled")
        set(BUILD_TESTS OFF)
    endif()
endif()

if(BUILD_TESTS)
    function(create_test name)
        add_executable(${name} ${ARGN})
        target_link_libraries(${name}
                                ${GTEST}
                                ${GTEST_MAIN}
                                bureaucracy
                             )
        add_test(${name} ${name} "--gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/${name}_results.xml")
    endfunction()
else()
    function(create_test name)
        #do nothing
    endfunction()
endif()

add_subdirectory(timer)
add_subdirectory(worker)

set(bureaucracy_srcs
        ${timer_srcs}
        ${worker_srcs}
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

if(BUILD_DOCS)
    find_program(DOXYGEN "doxygen")
    if(DOXYGEN)
        message(STATUS "doxygen location: ${DOXYGEN}")
        find_program(DOT "dot")
        if(DOT)
            message(STATUS "dot location: ${DOT}")
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
    else()
        message(WARNING "Couldn't find doxygen, disabling documentation")
    endif()
endif()

set(runtimeComponent "Runtime")
set(devComponent "Development")

install(TARGETS
            bureaucracy
            bureaucracy-static
            ARCHIVE DESTINATION lib COMPONENT ${devComponent}
            LIBRARY DESTINATION lib COMPONENT ${runtimeComponent}
            RUNTIME DESTINATION bin COMPONENT ${runtimeComponent}

       )

install(FILES
            include/bureaucracy/diligentworker.hpp
            include/bureaucracy/expandingthreadpool.hpp
            include/bureaucracy/priorityworker.hpp
            include/bureaucracy/serialworker.hpp
            include/bureaucracy/threadpool.hpp
            include/bureaucracy/threadpoolbase.hpp
            include/bureaucracy/timer.hpp
            include/bureaucracy/worker.hpp
            include/bureaucracy/workercommon.hpp
        DESTINATION include/bureaucracy
        COMPONENT ${devComponent}
       )
