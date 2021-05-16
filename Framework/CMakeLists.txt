set(public_files  ${CMAKE_CURRENT_SOURCE_DIR}/include/Resha.h)
set(private_files ${CMAKE_CURRENT_SOURCE_DIR}/src/Geometry.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/Maths.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/StringUtils.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/MeshIO.cpp)

add_library(Resha STATIC ${public_files} ${private_files})
target_include_directories(Resha PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Resha PRIVATE robin_hood gl3w)
