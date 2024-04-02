# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (Lab09_Normal_maps)	

find_package(OpenGL REQUIRED)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Source code
add_executable(Lab09_Normal_maps 
	source/main.cpp
	source/shader.hpp
	source/shader.cpp
	source/camera.hpp
	source/camera.cpp
	source/model.hpp
	source/model.cpp
	source/stb_image.hpp
	source/vertexShader.vert
	source/fragmentShader.frag
	source/light.hpp
	source/light.cpp
)
target_link_libraries(Lab09_Normal_maps
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(Lab09_Normal_maps PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source/")
create_target_launcher(Lab09_Normal_maps WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/source/")
create_default_target_launcher(Lab09_Normal_maps WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/source/")

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

add_custom_command(
   TARGET Lab09_Normal_maps POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Lab09_Normal_maps${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/source/"
)

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

