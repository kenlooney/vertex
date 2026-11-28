set(EXAMPLES_DIR ${CMAKE_SOURCE_DIR}/examples)

add_executable(pluginloader_test pluginloader_test.cpp)
target_link_libraries(pluginloader_test PRIVATE vertex ${CMAKE_DL_LIBS})
add_dependencies(pluginloader_test example_cpu example_language)
add_test(NAME pluginloader_test COMMAND pluginloader_test
  $<TARGET_FILE:example_cpu> $<TARGET_FILE:example_language> $<TARGET_FILE:vertex>)

add_executable(osasm_grammar_test osasm_grammar_test.cpp)
target_link_libraries(osasm_grammar_test kasmlang)
add_test(NAME osasm_grammar_test COMMAND osasm_grammar_test "${EXAMPLES_DIR}/test.asm")

add_executable(osasm_tokens_test osasm_grammar_test.cpp)
target_link_libraries(osasm_tokens_test kasmlang)
add_test(NAME osasm_tokens_test COMMAND osasm_tokens_test "${EXAMPLES_DIR}/identifier.asm")

add_executable(osasm_numbers_test osasm_grammar_test.cpp)
target_link_libraries(osasm_numbers_test kasmlang)
add_test(NAME osasm_numbers_test COMMAND osasm_numbers_test "${EXAMPLES_DIR}/numbers.asm")

add_executable(osasm_comments_test osasm_grammar_test.cpp)
target_link_libraries(osasm_comments_test kasmlang)
add_test(NAME osasm_comments_test COMMAND osasm_comments_test "${EXAMPLES_DIR}/comments.asm")

add_executable(osasm_integer_test osasm_integer_test.cpp)
target_link_libraries(osasm_integer_test utils)
add_test(NAME osasm_integer_test COMMAND osasm_integer_test)

add_executable(osasm_parser_test osasm_parser_test.cpp)
target_link_libraries(osasm_parser_test kasmlang)
add_test(NAME osasm_parser_test COMMAND osasm_parser_test)

add_executable(osasm_newline_test osasm_newline_test.cpp)
target_link_libraries(osasm_newline_test kasmlang)
add_test(NAME osasm_newline_test COMMAND osasm_newline_test)

add_executable(osasm_statements_test osasm_statements_test.cpp)
target_link_libraries(osasm_statements_test kasmlang)
add_test(NAME osasm_statements_test COMMAND osasm_statements_test "${EXAMPLES_DIR}/statements.asm")

add_executable(osasm_semantic_test osasm_semantic_test.cpp)
target_link_libraries(osasm_semantic_test kasmlang)
add_test(NAME osasm_semantic_test COMMAND osasm_semantic_test "${EXAMPLES_DIR}/directives.asm")

add_executable(osasm_bytes_test osasm_bytes_test.cpp)
target_link_libraries(osasm_bytes_test kasmlang)
add_test(NAME osasm_bytes_test COMMAND osasm_bytes_test)

add_executable(osasm_output_test osasm_output_test.cpp)
target_link_libraries(osasm_output_test osasmcore kasmlang)
add_test(NAME osasm_output_test COMMAND osasm_output_test)

add_test(NAME osasm_x86_encoding_test COMMAND ${CMAKE_COMMAND}
  "-DOSASM=$<TARGET_FILE:osasm>"
  "-DX86_PLUGIN=$<TARGET_FILE:x86_cpu>"
  "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/x86_encoding/$<CONFIG>"
  -P "${CMAKE_CURRENT_SOURCE_DIR}/osasm_x86_encoding_test.cmake")

add_executable(osasm_x86_decode_test osasm_x86_decode_test.cpp)
target_link_libraries(osasm_x86_decode_test PRIVATE osasmcore kasmlang)
add_dependencies(osasm_x86_decode_test x86_cpu)
add_test(NAME osasm_x86_decode_test COMMAND osasm_x86_decode_test
  "${EXAMPLES_DIR}/decode.asm" "$<TARGET_FILE:x86_cpu>"
  "${CMAKE_CURRENT_BINARY_DIR}/x86_decode_$<CONFIG>.bin")

add_test(NAME osasm_include_test COMMAND ${CMAKE_COMMAND}
  "-DOSASM=$<TARGET_FILE:osasm>"
  "-DX86_PLUGIN=$<TARGET_FILE:x86_cpu>"
  "-DSOURCE=${EXAMPLES_DIR}/include.asm"
  "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/include_$<CONFIG>.bin"
  -P "${CMAKE_CURRENT_SOURCE_DIR}/osasm_include_test.cmake")

add_executable(osasm_include_filename_test osasm_include_filename_test.cpp)
target_link_libraries(osasm_include_filename_test PRIVATE kasmlang)
add_test(NAME osasm_include_filename_test COMMAND osasm_include_filename_test
  "${EXAMPLES_DIR}/include.asm" "${EXAMPLES_DIR}/lib/startup.inc")
