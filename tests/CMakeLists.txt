set(EXAMPLES_DIR ${CMAKE_SOURCE_DIR}/examples)
add_test(NAME vertex_smoke_test COMMAND vertex_smoke_test)

add_executable(pluginloader_test pluginloader_test.cpp)
target_link_libraries(pluginloader_test PRIVATE vertex ${CMAKE_DL_LIBS})
add_dependencies(pluginloader_test example_cpu example_language)
add_test(NAME pluginloader_test COMMAND pluginloader_test
  $<TARGET_FILE:example_cpu> $<TARGET_FILE:example_language> $<TARGET_FILE:vertex>)

add_executable(osasm_grammar_test osasm_grammar_test.cpp)
target_link_libraries(osasm_grammar_test osasmcore)
add_test(NAME osasm_grammar_test COMMAND osasm_grammar_test "${EXAMPLES_DIR}/test.asm")

add_executable(osasm_tokens_test osasm_grammar_test.cpp)
target_link_libraries(osasm_tokens_test osasmcore)
add_test(NAME osasm_tokens_test COMMAND osasm_tokens_test "${EXAMPLES_DIR}/identifier.asm")

add_executable(osasm_numbers_test osasm_grammar_test.cpp)
target_link_libraries(osasm_numbers_test osasmcore)
add_test(NAME osasm_numbers_test COMMAND osasm_numbers_test "${EXAMPLES_DIR}/numbers.asm")

add_executable(osasm_comments_test osasm_grammar_test.cpp)
target_link_libraries(osasm_comments_test osasmcore)
add_test(NAME osasm_comments_test COMMAND osasm_comments_test "${EXAMPLES_DIR}/comments.asm")

add_executable(osasm_integer_test osasm_integer_test.cpp)
target_link_libraries(osasm_integer_test osasmcore utils)
add_test(NAME osasm_integer_test COMMAND osasm_integer_test)
