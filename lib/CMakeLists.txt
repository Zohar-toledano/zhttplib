file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(
    ZServer_lib STATIC
    ${SOURCES}
)

target_include_directories(ZServer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})