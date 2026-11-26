file(MAKE_DIRECTORY "${WORK_DIR}")
set(source "${WORK_DIR}/input.asm")
set(output "${WORK_DIR}/output.bin")
set(expected "aab02ab3ff80c00580c210f455b42ab52ab62ab72a80c40580c50580c60580c7050ab00aeb0190ebfb")
file(WRITE "${source}" [=[db 0xAA
mov al, (6 * 7)
mov bl, 255
add al, 5
add dl, 0x10
hlt
db 0x55
mov ah, 42
mov ch, 42
mov dh, 42
mov bh, 42
add ah, 5
add ch, 5
add dh, 5
add bh, 5
db ah
mov al, 0Ah
start:
alias: jmp done
nop
done: jmp start
]=])

execute_process(COMMAND "${OSASM}" -i "${source}" -o "${output}" "${X86_PLUGIN}"
  RESULT_VARIABLE result ERROR_VARIABLE error)
if(NOT "${result}" STREQUAL "0")
  message(FATAL_ERROR "Valid x86 assembly failed (${result}): ${error}")
endif()
file(READ "${output}" actual HEX)
if(NOT actual STREQUAL expected)
  message(FATAL_ERROR "Expected ${expected}, got ${actual}")
endif()

string(REPEAT "nop\n" 127 forward_padding)
string(REPEAT "nop\n" 126 backward_padding)
string(REPEAT "90" 253 padding_hex)
set(boundary_source "${WORK_DIR}/jump-boundary.asm")
set(boundary_output "${WORK_DIR}/jump-boundary.bin")
file(WRITE "${boundary_source}"
  "jmp ah\n${forward_padding}ah:\nformat:\n${backward_padding}jmp format\n")
execute_process(COMMAND "${OSASM}" -i "${boundary_source}" -o "${boundary_output}" "${X86_PLUGIN}"
  RESULT_VARIABLE result ERROR_VARIABLE error)
if(NOT "${result}" STREQUAL "0")
  message(FATAL_ERROR "Short-jump boundary assembly failed (${result}): ${error}")
endif()
file(READ "${boundary_output}" actual HEX)
if(NOT actual STREQUAL "eb7f${padding_hex}eb80")
  message(FATAL_ERROR "Unexpected bytes at the short-jump displacement limits: ${actual}")
endif()

set(invalid_source "${WORK_DIR}/invalid.asm")
foreach(invalid IN ITEMS "hlt 0" "mov al, bl" "mov eax, 42" "add al, 256" "add al, 1 / 0" "unknown"
    "jmp" "jmp 42" "jmp start, start" "jmp missing" "start:\nstart:"
    "jmp done\n${forward_padding}nop\ndone:" "start:\n${backward_padding}nop\njmp start")
  file(WRITE "${invalid_source}" "mov al, 0\n${invalid}\n")
  execute_process(COMMAND "${OSASM}" -i "${invalid_source}" -o "${output}" "${X86_PLUGIN}"
    RESULT_VARIABLE result ERROR_VARIABLE error)
  if(NOT "${result}" STREQUAL "1")
    message(FATAL_ERROR "Expected rejection of '${invalid}', got ${result}: ${error}")
  endif()
  file(READ "${output}" actual HEX)
  if(NOT actual STREQUAL expected)
    message(FATAL_ERROR "Rejected '${invalid}' changed the existing output: ${actual}")
  endif()
endforeach()
