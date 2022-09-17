add_subdirectory(attachments)

target_sources(${PROJECT_NAME} PRIVATE
  lane.hpp
  layout.hpp
  theme.cpp
  theme.hpp
  world.cpp
  world.hpp
)
