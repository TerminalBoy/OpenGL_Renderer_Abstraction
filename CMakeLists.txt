cmake_minimum_required(VERSION 3.31)

project(OpenGL_Renderer_Abstraction LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(OpenGL_Renderer_Abstraction src/main.cpp)

add_library(
  imgui STATIC 
  dependencies/ImGui/include/imgui_draw.cpp
  dependencies/ImGui/include/imgui_tables.cpp
  dependencies/ImGui/include/imgui_widgets.cpp
  dependencies/ImGui/include/imgui.cpp
  dependencies/ImGui/include/backends/imgui_impl_opengl3.cpp
  dependencies/ImGui/include/backends/imgui_impl_glfw.cpp

)


add_library(
  glad STATIC 
  dependencies/glad/src/glad.c
)
 
add_library(
  stb_image STATIC 
  dependencies/stb_image/src/stb_image.cpp
)


target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/ImGui/include)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/include)
target_include_directories(OpenGL_Renderer_Abstraction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glm/include)


include(FetchContent)


FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
  GIT_TAG 3.5.1
  GIT_SHALLOW TRUE
  
)
 
FetchContent_Declare(
  assimp  
  GIT_REPOSITORY https://github.com/assimp/assimp
  GIT_TAG v5.4.3
  GIT_SHALLOW TRUE
  
)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)


FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(assimp)



target_link_libraries(OpenGL_Renderer_Abstraction PRIVATE
  glad
  glfw
  imgui
  stb_image
  assimp
)
