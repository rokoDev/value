include(FetchContent)

# Setup directory to which dependencies will be downloaded
cmake_path(APPEND FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}" "deps_content" "${CMAKE_GENERATOR_NAME_WITHOUT_SPACES}")

# If dependencies are not available then download it and make available
FetchContent_Declare(
	boost_leaf
	GIT_REPOSITORY https://github.com/boostorg/leaf.git
	GIT_TAG        9d3ffa0f1c859a34da6175b53572fd4f2a61bb18
)

FetchContent_Declare(
	interval
	GIT_REPOSITORY https://github.com/rokoDev/interval.git
	GIT_TAG        94ad0a2ccb71282432ee4b1a761723612434df3f
)

# If necessary it's possible to use Boost.Leaf from different location instead of downloaded from GitHub
# string(TOUPPER boost_leaf UP_boost_leaf)
# set(FETCHCONTENT_SOURCE_DIR_${UP_boost_leaf} ${CMAKE_SOURCE_DIR}/../rokoDev/leaf)

FetchContent_MakeAvailable(boost_leaf interval)
target_link_libraries(value INTERFACE boost_leaf interval)
