# CMakeList.txt : CMake project for blackjack_strats, include source and define
# project specific logic here.
#
include(FetchContent)

FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG  v5.5.4
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glaze)


set(BLACKJACK_SOURCES
	"main.cpp"
)

# Add source to this project's executable.
add_executable (blackjack_strats ${BLACKJACK_SOURCES})
target_link_libraries(blackjack_strats PRIVATE glaze::glaze)

if (CMAKE_VERSION VERSION_GREATER 3.20)
  set_property(TARGET blackjack_strats PROPERTY CXX_STANDARD 23)
endif()

# TODO: Add tests and install targets if needed.
