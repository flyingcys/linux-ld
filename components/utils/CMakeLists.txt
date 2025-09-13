# Utils 工具组件
add_library(utils STATIC
    string_utils.c
    math_utils.c
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})