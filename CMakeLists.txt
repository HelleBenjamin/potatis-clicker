cmake_minimum_required(VERSION 3.10)

project(PotatisClicker)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project's headers
include_directories(include)

# Main files
set(SRC_FILES
  src/main.c
)

# Third-party libraries
set(LIBS_FILES
  src/glad/glad.c
)

add_executable(PotatisClicker ${SRC_FILES} ${LIBS_FILES})

if(UNIX)
  message("Linux Detected")
  target_link_directories(PotatisClicker PRIVATE ${CMAKE_SOURCE_DIR}/lib/linux)
  target_link_libraries(PotatisClicker PRIVATE GL GLU m glut glfw3)
elseif(WIN32)
  message("Windows Detected")
  target_link_directories(PotatisClicker PRIVATE ${CMAKE_SOURCE_DIR}/lib/win32)
  target_link_libraries(PotatisClicker PRIVATE gdi32 opengl32 m glfw3 glu32)
endif()

# Set building flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2") # Use -O2 or -O3 for better performance, may increase compile time, default is -O1, for debug use -O0 with -g

set_target_properties(PotatisClicker PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Copies assets to build directory, necessary on Windows
add_custom_command(TARGET PotatisClicker POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:PotatisClicker>/assets
)