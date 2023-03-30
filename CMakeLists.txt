cmake_minimum_required (VERSION 3.0)

set (CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

set (glm_DIR C:/GLM/lib/cmake/glm)
set (glfw3_DIR C:/GLFW/lib/cmake/glfw3)

project (terrain)

if(MSVC)
  add_definitions(/DNOMINMAX)
endif()

if(UNIX)
    message(STATUS "Running Linux compiling. Options [GLVND] for modern OpenGL or [LEGACY] for historical OpenGl.")
    message(STATUS "Setting GLVND option.")
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package( glm CONFIG REQUIRED )
find_package( glfw3 CONFIG REQUIRED )
find_package( OpenGL REQUIRED )

add_subdirectory( terrain )
add_subdirectory( terrain/shader )
