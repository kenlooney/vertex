# Builds a plugin library (CPU, language, grammar, ...) for vertex's loaders to load at runtime.
# MODULE libraries are loaded at runtime and never linked against.
function(vertex_add_plugin target)
  add_library(${target} MODULE ${ARGN})
  target_link_libraries(${target} PRIVATE vertex)
  set_target_properties(${target} PROPERTIES PREFIX "")
endfunction()

add_subdirectory(utils)
add_subdirectory(vertex)
add_subdirectory(osasm)
add_subdirectory(osasmcore)
add_subdirectory(cpus)
add_subdirectory(languages)
