cmake_minimum_required(VERSION 3.10)

project(
    ZServer
    VERSION 0.0.1
    LANGUAGES CXX    
    )
set (CMAKE_CXX_STANDARD 23)
include_directories(json)

# Add the server library as a subdirectory
add_subdirectory(lib)
add_subdirectory(multiproccesing)


# Add the subbinary as a subdirectory
add_subdirectory(ZServer)