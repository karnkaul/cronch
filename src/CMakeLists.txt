add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(util)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
