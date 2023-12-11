cmake_minimum_required(VERSION 3.11)
project(LearnOpenGL VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    set(MY_COMLILE_OPTIONS /W4 /WX)
else ()
    set(MY_COMLILE_OPTIONS -Wall -Wextra -pedantic -Werror -Wshadow -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC)
endif()

find_package(glfw3 3.3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(assimp REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})
find_package(X11 REQUIRED)
# note that the order is important for setting the libs
# use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
set(LIBS glfw X11 Xrandr OpenGL::GL dl pthread ${ASSIMP_LIBRARIES})
set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

# include(CTest)
# enable_testing()

add_library(GLAD "third_party/glad.c")
target_include_directories(GLAD PRIVATE ${CMAKE_SOURCE_DIR}/third_party/include)

set(Common_include ${CMAKE_SOURCE_DIR}/third_party/include ${CMAKE_SOURCE_DIR})

set(LIBS ${LIBS} GLAD)

set(CHAPTERS
    1.getting_started
    2.lighting
    3.model_loading
    4.advanced_opengl
)

set(1.getting_started
    3.3.shaders
    4.1.textures
    5.1.transformations
    6.1.coordinate_systems
    7.1.camera
)

set(2.lighting
    1.colors
    2.1.basic_lighting
    3.1.materials
    4.1.lighting_maps_diffuse_map
    5.1.light_casters_directional
    5.2.light_casters_point
    5.3.light_casters_spot
    6.multiple_lights
)

set(3.model_loading
    1.model_loading
)

set(4.advanced_opengl
    1.depth_testing
    2.stencil_testing
    3.1.blending_discard
    3.2.blending_sort
    4.faceculling
    5.1.framebuffers
    5.1.framebuffers_exercise
    6.1.cubemaps_skybox
    6.2.cubemaps_environment_mapping
    8.advanced_glsl_ubo
    9.1.geometry_shader_houses
    9.2.geometry_shader_exploding
    9.3.geometry_shader_normals
)

function(create_project_from_sources chapter demo)
	file(GLOB SOURCE
            "src/${chapter}/${demo}/*.h"
            "src/${chapter}/${demo}/*.cpp"
    )
	if (demo STREQUAL "")
		SET(replaced "")
		string(REPLACE "/" "_" replaced ${chapter})
		set(NAME ${replaced})
	else()
		set(NAME "${chapter}__${demo}")
	endif()
    add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} ${LIBS})
    target_include_directories(${NAME} PRIVATE ${Common_include})
endfunction()

foreach(CHAPTER ${CHAPTERS})
    foreach(DEMO ${${CHAPTER}})
		create_project_from_sources(${CHAPTER} ${DEMO})
    endforeach(DEMO)
endforeach(CHAPTER)