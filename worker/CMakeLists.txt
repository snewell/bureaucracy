set(worker_srcs
        "${CMAKE_CURRENT_SOURCE_DIR}/diligentworker.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/expandingthreadpool.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/priorityworker.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/serialworker.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/threadpool.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/threadpoolbase.cpp"
    PARENT_SCOPE)

create_test(worker_tests
                diligentworker_test.cpp
                expandingthreadpool_test.cpp
                priorityworker_test.cpp
                serialworker_test.cpp
                threadpool_test.cpp
           )
   