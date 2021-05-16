add_library(robin_hood INTERFACE)
target_include_directories(robin_hood INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)
