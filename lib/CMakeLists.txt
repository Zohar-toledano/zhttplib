file(GLOB SOURCES "src/*.cpp")

add_library(
    ZServer_lib STATIC
    ${SOURCES}
)

# target_precompile_headers(ZServer_lib PUBLIC 
# ${CMAKE_CURRENT_SOURCE_DIR}/include/Server.h)  # Replace "my_target" with your target's name
target_link_libraries(ZServer_lib PUBLIC multiproccesing)

target_include_directories(ZServer_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)