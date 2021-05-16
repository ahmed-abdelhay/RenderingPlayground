set(src_files ${CMAKE_CURRENT_SOURCE_DIR}/include/Playground.h
              ${CMAKE_CURRENT_SOURCE_DIR}/src/Playground.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable (Playground ${src_files})
target_include_directories(Playground
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries (Playground PRIVATE Resha imgui)
