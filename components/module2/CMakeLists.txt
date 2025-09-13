# Module2 组件
add_library(module2 STATIC
    module2.c
)

target_include_directories(module2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(module2 auto_init)