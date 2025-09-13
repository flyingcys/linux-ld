cmake_minimum_required(VERSION 3.10)
project(auto_init_demo)

# 设置C标准
set(CMAKE_C_STANDARD 99)

# 添加编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# 包含目录
include_directories(include)

# 创建自动初始化库
add_library(auto_init STATIC
    src/auto_init.c
)

target_include_directories(auto_init PUBLIC include)

# 设置链接选项 - 使用自定义链接器脚本（可选）
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/auto_init.ld")

# 或者使用GCC内建的段符号生成（推荐）
# GCC会自动为相同前缀的段生成__start_和__stop_符号

# 创建示例程序
add_executable(demo
    example/main.c
    example/demo_modules.c
)

target_link_libraries(demo auto_init)

# 如果需要使用自定义链接器脚本，可以取消下面的注释
# target_link_options(demo PRIVATE -T${CMAKE_CURRENT_SOURCE_DIR}/auto_init.ld)

# 安装规则
install(TARGETS auto_init DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
install(TARGETS demo DESTINATION bin)

# 创建一个简单的测试
enable_testing()
add_test(NAME demo_test COMMAND demo)

# 添加清理目标
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
)

# 打印构建信息
message(STATUS "项目名称: ${PROJECT_NAME}")
message(STATUS "编译器: ${CMAKE_C_COMPILER}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "安装前缀: ${CMAKE_INSTALL_PREFIX}")

# 帮助信息
add_custom_target(show-help
    COMMAND echo "可用目标:"
    COMMAND echo "  demo          - 构建示例程序"
    COMMAND echo "  auto_init     - 构建自动初始化库"
    COMMAND echo "  install       - 安装程序和库"
    COMMAND echo "  test          - 运行测试"
    COMMAND echo "  clean-all     - 清理所有构建文件"
    COMMAND echo "  show-help     - 显示此帮助信息"
)