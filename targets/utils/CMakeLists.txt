file(GLOB_RECURSE UTILS_SOURCES CONFIGURE_DEPENDS "src/*.c" "src/*.cpp" "include/*.h")
add_library(utils SHARED ${UTILS_SOURCES})
target_include_directories(utils PUBLIC "include")
