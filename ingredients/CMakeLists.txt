set(target ingredients)

set(ingredients_SOURCES
        glad/src/glad.c
	  stb/stb_c_lexer.h
	  stb/stb_connected_components.h
	  stb/stb_divide.h
	  stb/stb_ds.h
	  stb/stb_dxt.h
	  stb/stb_easy_font.h
	  stb/stb_herringbone_wang_tile.h
	  stb/stb_hexwave.h
	  stb/stb_image.h
	  stb/stb_image_resize.h
	  stb/stb_image_write.h
	  stb/stb_include.h
	  stb/stb_leakcheck.h
	  stb/stb_perlin.h
	  stb/stb_rect_pack.h
	  stb/stb_sprintf.h
	  stb/stb_textedit.h
	  stb/stb_tilemap_editor.h
	  stb/stb_truetype.h
	  stb/stb_vorbis.c
	  stb/stb_voxel_render.h)

add_library(${target} STATIC ${ingredients_SOURCES})

target_include_directories(${target} PUBLIC glad/include)
target_include_directories(${target} PUBLIC learnopengl)
target_include_directories(${target} PUBLIC stb)


if( UNIX AND NOT APPLE )
    target_link_libraries(${target} PUBLIC ${CMAKE_DL_LIBS})
endif()
