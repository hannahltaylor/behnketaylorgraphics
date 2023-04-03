set(target terrain)
set( terrain_SOURCES
	main.cpp )

add_executable( ${target} ${terrain_SOURCES} )

target_compile_definitions(${target}
		PRIVATE
		GLFW_INCLUDE_NONE
		)

target_link_libraries( ${target}
		PRIVATE
		ingredients
		glfw
		${OPENGL_gl_LIBRARY}
		)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/iceland_heightmap.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
