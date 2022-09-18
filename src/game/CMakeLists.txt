add_subdirectory(attachments)

target_sources(${PROJECT_NAME} PRIVATE
  chomp_type.hpp
  lane.hpp
  layout.hpp
  score.hpp
  theme.cpp
  theme.hpp
  world.cpp
  world.hpp
)
