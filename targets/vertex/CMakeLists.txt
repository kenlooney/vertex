file(GLOB_RECURSE VERTEX_SOURCES CONFIGURE_DEPENDS "src/*.c" "src/*.cpp" "include/*.h")
add_library(vertex SHARED ${VERTEX_SOURCES})
target_include_directories(vertex PUBLIC "include")
target_link_libraries(vertex PRIVATE utils ${CMAKE_DL_LIBS})