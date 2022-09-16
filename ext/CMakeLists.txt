include(FetchContent)

# tardigrade
FetchContent_Declare(
  tardigrade
  GIT_REPOSITORY https://github.com/karnkaul/tardigrade
  GIT_TAG 24e3a2f # change to version tag after all bug-fixes etc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/tardigrade"
)
set(TG_INSTALL OFF)
FetchContent_MakeAvailable(tardigrade)

# capo
FetchContent_Declare(
  capo
  GIT_REPOSITORY https://github.com/capo-devs/capo
  GIT_TAG 259d406 # change to version tag after all bug-fixes etc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/capo"
)
set(CAPO_BUILD_EXAMPLE OFF)
FetchContent_MakeAvailable(capo)

# vulkify
FetchContent_Declare(
  vulkify
  GIT_REPOSITORY https://github.com/vulkify/vulkify
  GIT_TAG 60c5664 # change to version tag after all bug-fixes etc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/vulkify"
)
set(VULKIFY_DEBUG_TRACE OFF)
set(VULKIFY_BUILD_EXAMPLES OFF)
FetchContent_MakeAvailable(vulkify)
