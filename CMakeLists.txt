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

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})
find_package(X11 REQUIRED)
# note that the order is important for setting the libs
# use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
set(LIBS glfw X11 Xrandr OpenGL::GL dl pthread ${ASSIMP_LIBRARY})
set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

include(CTest)
enable_testing()

add_library(GLAD "third_party/glad.c")
target_include_directories(GLAD PRIVATE ${CMAKE_SOURCE_DIR}/third_party/include)
set(LIBS ${LIBS} GLAD)

add_executable(1.out 1_shader/main.cpp)
target_link_libraries(1.out ${LIBS})
target_include_directories(1.out PRIVATE ${CMAKE_SOURCE_DIR}/third_party/include ${CMAKE_SOURCE_DIR})

add_executable(2.out 2_texture/main.cpp)
target_link_libraries(2.out ${LIBS})
target_include_directories(2.out PRIVATE ${CMAKE_SOURCE_DIR}/third_party/include ${CMAKE_SOURCE_DIR})

add_executable(3.out 3_transformations/main.cpp)
target_link_libraries(3.out ${LIBS})
target_include_directories(3.out PRIVATE ${CMAKE_SOURCE_DIR}/third_party/include ${CMAKE_SOURCE_DIR})
