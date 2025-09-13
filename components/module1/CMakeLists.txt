# Module1 组件
add_library(module1 STATIC
    module1.c
)

target_include_directories(module1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(module1 auto_init)