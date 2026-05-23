cmake_minimum_required(VERSION 3.10)

project(paint CXX)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glfw)

add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)


file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS src/*.c src/*.cpp imgui/*.cpp)

add_executable(paint)
target_sources(paint PRIVATE ${PROJECT_SOURCES})
target_include_directories(paint PUBLIC src imgui)
target_link_libraries(paint PUBLIC glfw glad)

add_custom_command(
    TARGET paint POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:paint>/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:paint>/resources
)
