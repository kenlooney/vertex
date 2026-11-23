add_test(NAME vertex_smoke_test COMMAND vertex_smoke_test)

add_executable(pluginloader_test pluginloader_test.cpp)
target_link_libraries(pluginloader_test PRIVATE vertex ${CMAKE_DL_LIBS})
add_dependencies(pluginloader_test example_cpu example_language example_grammar)
add_test(NAME pluginloader_test COMMAND pluginloader_test
  $<TARGET_FILE:example_cpu> $<TARGET_FILE:example_language> $<TARGET_FILE:example_grammar> $<TARGET_FILE:vertex>)