execute_process(
  COMMAND "${OSASM}" -i "${SOURCE}" -o "${OUTPUT}" "${X86_PLUGIN}"
  RESULT_VARIABLE result
  ERROR_VARIABLE error)
if(NOT "${result}" STREQUAL "0")
  message(FATAL_ERROR "Include example failed (${result}): ${error}")
endif()

file(READ "${OUTPUT}" actual HEX)
string(TOUPPER "${actual}" uppercase_hex)
string(REGEX MATCHALL "[0-9A-F][0-9A-F]" bytes "${uppercase_hex}")
list(JOIN bytes " " formatted_bytes)
message(STATUS "Produced bytes: ${formatted_bytes}")

if(NOT actual STREQUAL "012a02")
  message(FATAL_ERROR "Expected 012a02, got ${actual}")
endif()
