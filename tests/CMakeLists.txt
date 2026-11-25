set(EXAMPLES_DIR ${CMAKE_SOURCE_DIR}/examples)

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

add_executable(osasm_parser_test osasm_parser_test.cpp)
target_link_libraries(osasm_parser_test osasmcore)
add_test(NAME osasm_parser_test COMMAND osasm_parser_test)

add_executable(osasm_newline_test osasm_newline_test.cpp)
target_link_libraries(osasm_newline_test osasmcore)
add_test(NAME osasm_newline_test COMMAND osasm_newline_test)

add_executable(osasm_statements_test osasm_statements_test.cpp)
target_link_libraries(osasm_statements_test osasmcore)
add_test(NAME osasm_statements_test COMMAND osasm_statements_test "${EXAMPLES_DIR}/statements.asm")

add_executable(osasm_semantic_test osasm_semantic_test.cpp)
target_link_libraries(osasm_semantic_test osasmcore)
add_test(NAME osasm_semantic_test COMMAND osasm_semantic_test "${EXAMPLES_DIR}/directives.asm")
