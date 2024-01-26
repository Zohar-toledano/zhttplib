# add_executable(ZServer main.cpp)

# # target_link_libraries(ZServer PRIVATE ZServer_lib)

# target_include_directories(ZServer PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

file(GLOB SOURCES "src/*.cpp")

add_library(
    multiproccesing STATIC
    ${SOURCES}
)
target_include_directories(multiproccesing PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
