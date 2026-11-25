file(GLOB_RECURSE KASMLANG_SOURCES CONFIGURE_DEPENDS "src/*.cpp" "include/*.h")
add_library(kasmlang SHARED ${KASMLANG_SOURCES})
target_link_libraries(kasmlang PUBLIC vertex utils)
target_include_directories(kasmlang PUBLIC "include")