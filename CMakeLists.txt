cmake_minimum_required(VERSION 3.12)
add_compile_definitions(-DUNICODE -D_UNICODE)

set (CMAKE_CXX_STANDARD 23)

project(SERVER VERSION 0.1 LANGUAGES CXX)

file (GLOB_RECURSE CPPFILES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${CPPFILES})
target_link_directories(${PROJECT_NAME} PUBLIC src)
