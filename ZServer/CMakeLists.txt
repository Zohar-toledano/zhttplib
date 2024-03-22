
if(UNIX)
  # Add file textension *.bin to all executables on Unix (Linux, OSX, BSD ...)
  set(CMAKE_EXECUTABLE_SUFFIX ".bin")
endif()

# ------------ config embeded executable ----------------#

# add_executable(embeded worker/workerMain.cpp)
# target_link_libraries(embeded PRIVATE 
# ZServer_lib
# )

# ------------ Download CmakeRC Resource compiler ----------------#
# file(DOWNLOAD "https://raw.githubusercontent.com/vector-of-bool/cmrc/master/CMakeRC.cmake"
#                  "${CMAKE_BINARY_DIR}/CMakeRC.cmake")
# include("${CMAKE_BINARY_DIR}/CMakeRC.cmake")

#========== Targets Configurations ============#

# cmrc_add_resource_library(
#     app1-resources

#     ALIAS app1::rc
#     NAMESPACE app1
# 	WHENCE ${CMAKE_BINARY_DIR}/ZServer/Debug
# 	${CMAKE_BINARY_DIR}/ZServer/Debug/embeded.exe
#     )


add_library(ZSserverMainProcess STATIC master/master.cpp)
target_include_directories(ZSserverMainProcess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ZSserverMainProcess PRIVATE 
ZServer_lib
# app1::rc
)


# add_library(
#     user_code SHARED
#     userMain.cpp
#     )
# # target_include_directories(user_code PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# target_link_libraries(
#     user_code
#     PRIVATE
#   ZServer_lib
# )
