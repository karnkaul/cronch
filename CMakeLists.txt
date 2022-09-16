cmake_minimum_required(VERSION 3.18)

enable_language(CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_DEBUG_POSTFIX "-d")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(iris)

add_subdirectory(ext)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE tardigrade::tardigrade capo::capo vulkify::vulkify)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  ROOT_NS=cronch
  $<$<CONFIG:Debug>:IRIS_DEBUG>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Wconversion -Werror=return-type -Wunused
  )
endif()

add_subdirectory(src)
