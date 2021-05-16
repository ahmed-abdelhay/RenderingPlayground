add_subdirectory(glfw)
add_subdirectory(imgui)
add_subdirectory(robin_hood)
add_subdirectory(gl3w)

set_target_properties(glfw    PROPERTIES FOLDER 3pty)
set_target_properties(imgui   PROPERTIES FOLDER 3pty)
set_target_properties(gl3w    PROPERTIES FOLDER 3pty)
