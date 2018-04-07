add_sources(
    "${CMAKE_CURRENT_LIST_DIR}/diligentworker.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/expandingthreadpool.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/priorityworker.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/serialworker.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/threadpool.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/threadpoolbase.cpp"
)
add_headers(
    diligentworker.hpp
    expandingthreadpool.hpp
    priorityworker.hpp
    serialworker.hpp
    threadpool.hpp
    threadpoolbase.hpp
    worker.hpp
    workercommon.hpp
)

create_test(worker_tests
    "${CMAKE_CURRENT_LIST_DIR}/diligentworker_test.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/expandingthreadpool_test.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/priorityworker_test.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/serialworker_test.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/threadpool_test.cpp"
)
   
