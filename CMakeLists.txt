cmake_minimum_required(VERSION 3.24)
project(vertex VERSION 0.4.0 LANGUAGES C CXX)


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
include(CTest)

# Expose the project version to CI (read by .github/workflows/release.yml).
file(WRITE ${CMAKE_BINARY_DIR}/vertex-version.txt "${PROJECT_VERSION}")

add_subdirectory(targets)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# Keep the executable and its shared libraries together in portable archives.
if(APPLE)
  set_target_properties(osasm PROPERTIES INSTALL_RPATH "@loader_path")
  set_target_properties(vertex PROPERTIES INSTALL_RPATH "@loader_path")
  set_target_properties(utils PROPERTIES INSTALL_RPATH "@loader_path")
  set_target_properties(kasmlang PROPERTIES INSTALL_RPATH "@loader_path")
elseif(UNIX)
  set_target_properties(osasm PROPERTIES INSTALL_RPATH "$ORIGIN")
  set_target_properties(vertex PROPERTIES INSTALL_RPATH "$ORIGIN")
  set_target_properties(utils PROPERTIES INSTALL_RPATH "$ORIGIN")
  set_target_properties(kasmlang PROPERTIES INSTALL_RPATH "$ORIGIN")
endif()
install(TARGETS osasm vertex utils kasmlang
  RUNTIME DESTINATION .
  LIBRARY DESTINATION .)
install(FILES README.md LICENSE DESTINATION .)

include(CPack)
