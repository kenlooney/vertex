file(GLOB_RECURSE OSASMCORE_SOURCES CONFIGURE_DEPENDS "src/*.c" "src/*.cpp" "include/*.h")
add_library(osasmcore STATIC ${OSASMCORE_SOURCES})
target_include_directories(osasmcore PUBLIC "include")
target_link_libraries(osasmcore PUBLIC vertex utils)