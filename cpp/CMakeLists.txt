cmake_minimum_required(VERSION 3.16)
project(rest_api LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------
# System dependencies
# ------------------------------------

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# ------------------------------------
# Include paths
# ------------------------------------

include_directories(
    include
    lib
    /usr/include/mariadb
)

# ------------------------------------
# Sources
# ------------------------------------

file(GLOB_RECURSE APP_SOURCES
    src/*.cpp
)

file(GLOB_RECURSE SERVED_SOURCES
    lib/served/src/*.cpp
)

# ------------------------------------
# Executable
# ------------------------------------

add_executable(rest_api
    ${APP_SOURCES}
    ${SERVED_SOURCES}
)

# ------------------------------------
# Link libraries
# ------------------------------------

target_link_libraries(rest_api
    Boost::system
    Threads::Threads
    mariadb
)

