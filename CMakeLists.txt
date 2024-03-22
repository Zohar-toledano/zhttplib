cmake_minimum_required(VERSION 3.12)

project(
    ZServer
    VERSION 0.0.1
    LANGUAGES CXX    
    )
set (CMAKE_CXX_STANDARD 23)
include_directories(external)


# Add the server library as a subdirectory
add_subdirectory(lib)
# add_subdirectory(multiproccesing)


# Add the subbinary as a subdirectory
add_subdirectory(ZServer)

add_executable(
    main
    main.cpp
    )
target_link_libraries(
    main
    ZSserverMainProcess
)

