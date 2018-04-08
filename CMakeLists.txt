cmake_minimum_required(VERSION 3.9)
cmake_policy(VERSION 3.9)

project("bureaucracy"
    LANGUAGES
        CXX
    VERSION
        0.2.0
)

include(GNUInstallDirs)
include(CTest)

find_package(GTest)
find_package(PkgConfig)
find_package(Threads REQUIRED)
find_package(Houseguest REQUIRED)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
include(CPack)

option(BUREAUCRAY_BUILD_TESTS "Build optional tests" ON)
option(BUREAUCRAY_BUILD_DOCS  "Build documenttaion" ON)

enable_testing()

if(BUREAUCRAY_BUILD_TESTS)
    find_package(GTest REQUIRED)
    function(create_test name)
        add_executable(${name} ${ARGN})
        target_link_libraries(${name}
            GTest::GTest
            GTest::Main
            bureaucracy
        )
        set_target_properties(${name} PROPERTIES
            CXX_EXTENSIONS OFF
        )
        target_compile_features(${name} PRIVATE
            cxx_std_14
        )
        add_test(${name} ${name} "--gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/${name}_results.xml")
    endfunction()
else()
    function(create_test name)
        #do nothing
    endfunction()
endif()

add_library(bureaucracy SHARED)
set_target_properties(bureaucracy PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)
target_include_directories(bureaucracy PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(bureaucracy
    PUBLIC Threads::Threads
    PUBLIC Houseguest::houseguest
)
target_compile_features(bureaucracy PUBLIC
    cxx_std_14
)

add_library(bureaucracy-static STATIC)
set_target_properties(bureaucracy-static PROPERTIES
    OUTPUT_NAME bureaucracy
)
target_include_directories(bureaucracy-static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(bureaucracy-static
    PUBLIC Houseguest::houseguest
)
target_compile_features(bureaucracy-static PUBLIC
    cxx_std_14
)

set_target_properties(
        bureaucracy
        bureaucracy-static
    PROPERTIES
        CXX_EXTENSIONS OFF
)

function(add_sources)
    foreach(target
                bureaucracy
                bureaucracy-static)
        target_sources(${target} PRIVATE ${ARGN})
    endforeach()
endfunction()
function(add_headers)
    foreach(target
                bureaucracy
                bureaucracy-static)
        foreach(header IN LISTS ARGN)
            set(full_header_path "${CMAKE_CURRENT_SOURCE_DIR}/include/bureaucracy/${header}")
            list(APPEND all_headers ${full_header_path})
            target_sources(${target} PUBLIC
                $<BUILD_INTERFACE:${full_header_path}>
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/bureaucracy/${header}>
            )
        endforeach()
    endforeach()
    install(FILES ${all_headers}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bureaucracy
        COMPONENT ${devComponent}
    )
endfunction()

include(timer/CMakeLists.txt)
include(worker/CMakeLists.txt)

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
    EXPORT BureaucracyConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${devComponent}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${runtimeComponent}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${runtimeComponent}
)

export(TARGETS
        bureaucracy
        bureaucracy-static
    NAMESPACE Bureaucracy::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/BureaucracyConfig.cmake"
)
install(EXPORT
        BureaucracyConfig
    DESTINATION "${CMAKE_INSTALL_DATADIR}/Bureaucracy/cmake"
    NAMESPACE Bureaucracy::
)

if(PKG_CONFIG_FOUND)
    set(pcFile
        "${CMAKE_CURRENT_BINARY_DIR}/bureaucracy-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.pc")
    configure_file(pkg-config.pc.in ${pcFile} @ONLY)
    set(CMAKE_INSTALL_PKGCONFIGDIR "${CMAKE_INSTALL_DATADIR}/pkgconfig")
    install(FILES
            ${pcFile}
        DESTINATION ${CMAKE_INSTALL_PKGCONFIGDIR}
        COMPONENT ${devComponent}
    )
endif()

if(DOXYGEN)
    install(DIRECTORY
            "${CMAKE_CURRENT_BINARY_DIR}/doc/html"
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT ${devComponent}
    )
endif()
install(FILES
        COPYING
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)
