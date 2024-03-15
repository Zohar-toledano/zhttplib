
if(UNIX)
  # Add file textension *.bin to all executables on Unix (Linux, OSX, BSD ...)
  set(CMAKE_EXECUTABLE_SUFFIX ".bin")
endif()

# ------------ config embeded executable ----------------#

add_executable(embeded worker/workerMain.cpp)
target_link_libraries(embeded PRIVATE 
ZServer_lib
)

# ------------ Download CmakeRC Resource compiler ----------------#
file(DOWNLOAD "https://raw.githubusercontent.com/vector-of-bool/cmrc/master/CMakeRC.cmake"
                 "${CMAKE_BINARY_DIR}/CMakeRC.cmake")
include("${CMAKE_BINARY_DIR}/CMakeRC.cmake")

#========== Targets Configurations ============#

cmrc_add_resource_library(
    app1-resources

    ALIAS app1::rc
    NAMESPACE app1
	WHENCE ${CMAKE_BINARY_DIR}/ZServer/Debug
	${CMAKE_BINARY_DIR}/ZServer/Debug/embeded.exe
    )


add_library(application1 STATIC master/master.cpp)
target_include_directories(application1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(application1 PRIVATE 
ZServer_lib
app1::rc
)


