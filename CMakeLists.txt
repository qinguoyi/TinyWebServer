cmake_minimum_required(VERSION 3.5)

project(TinyWebServer)

find_package(PkgConfig)
pkg_check_modules(MySQL REQUIRED mysqlclient>=5.7)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCES
    main.cpp
    ./http/http_conn.cpp
    ./log/log.cpp
    ./CGImysql/sql_connection_pool.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/threadpool
    ${CMAKE_SOURCE_DIR}/lock
    ${CMAKE_SOURCE_DIR}/log
    ${CMAKE_SOURCE_DIR}/CGImysql
)

add_executable(myServer ${SOURCES})

target_include_directories(myServer PUBLIC ${MySQL_INCLUDE_DIRS})
target_link_libraries(myServer PUBLIC ${MySQL_LIBRARIES})