
# function(get_all_cpp_files result_var dir)
#     file(GLOB_RECURSE children RELATIVE ${dir} ${dir}/*.cpp)
#     set(${result_var} ${children})
# endfunction()

# function(add_include_directory_recursively subdir)
#     file(GLOB children RELATIVE ${subdir} ${subdir}/*)
#     foreach(child ${children})
#         if(IS_DIRECTORY ${subdir}/${child})
#             # add_subdirectory(${subdir}/${child})
#             target_include_directories(ZServer_lib PUBLIC ${subdir}/${child})
#             add_include_directory_recursively(${subdir}/${child})  # Recursive call
#         endif()
#     endforeach()
# endfunction()

# file(GLOB SOURCES "src/*.cpp")
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# get_all_cpp_files(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
    ZServer_lib STATIC
    ${SOURCES}
)

target_include_directories(ZServer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# add_include_directory_recursively(${CMAKE_CURRENT_SOURCE_DIR})

# target_precompile_headers(ZServer_lib PUBLIC 
# ${CMAKE_CURRENT_SOURCE_DIR}/include/Server.h)  # Replace "my_target" with your target's name
# target_link_libraries(ZServer_lib PUBLIC multiproccesing)

# target_include_directories(ZServer_lib PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
# )