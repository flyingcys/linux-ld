# Test Module 组件
add_library(test_module STATIC
    test_module.c
)

target_include_directories(test_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(test_module auto_init)