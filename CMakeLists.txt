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

# =============================================================================
# 自动发现components目录下的子目录并添加到构建中
# =============================================================================

# 全局变量：存储所有发现的组件库名称
set(DISCOVERED_COMPONENTS "" CACHE INTERNAL "List of discovered component libraries")

# 自动添加子目录的函数
function(auto_add_subdirectories base_dir)
    # 参数验证
    if(NOT EXISTS ${base_dir})
        message(WARNING "目录不存在: ${base_dir}")
        return()
    endif()

    message(STATUS "=== 自动发现 ${base_dir} 目录下的组件 ===")

    # 获取所有子目录
    file(GLOB subdirs RELATIVE ${base_dir} ${base_dir}/*)

    # 统计变量
    set(total_dirs 0)
    set(added_dirs 0)
    set(skipped_dirs 0)
    set(component_list "")

    foreach(subdir ${subdirs})
        set(subdir_path ${base_dir}/${subdir})

        # 检查是否为目录
        if(IS_DIRECTORY ${subdir_path})
            math(EXPR total_dirs "${total_dirs} + 1")

            # 排除特殊目录
            if(subdir MATCHES "^\\.|build|cmake-build|CMakeFiles")
                message(STATUS "  ⊘ 跳过特殊目录: ${subdir}")
                math(EXPR skipped_dirs "${skipped_dirs} + 1")
                continue()
            endif()

            # 检查是否包含 CMakeLists.txt
            if(EXISTS ${subdir_path}/CMakeLists.txt)
                message(STATUS "  ✅ 自动添加组件: ${subdir}")
                add_subdirectory(${subdir_path})
                list(APPEND component_list ${subdir})
                math(EXPR added_dirs "${added_dirs} + 1")
            else()
                message(STATUS "  ⚠️ 跳过目录 ${subdir}：未找到 CMakeLists.txt")
                math(EXPR skipped_dirs "${skipped_dirs} + 1")
            endif()
        endif()
    endforeach()

    # 更新全局组件列表
    set(DISCOVERED_COMPONENTS ${component_list} CACHE INTERNAL "List of discovered component libraries")

    message(STATUS "=== 组件发现完成 ===")
    message(STATUS "  总目录数: ${total_dirs}")
    message(STATUS "  已添加: ${added_dirs}")
    message(STATUS "  已跳过: ${skipped_dirs}")
    if(component_list)
        message(STATUS "  发现的组件: ${component_list}")
    endif()
    message(STATUS "========================")
endfunction()

# 自动添加 components 目录下的所有组件
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/components)
    auto_add_subdirectories(${CMAKE_CURRENT_SOURCE_DIR}/components)
else()
    message(STATUS "components目录不存在，跳过自动组件发现")
endif()

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

# 自动链接所有发现的组件库到demo程序
# 使用--whole-archive确保包含所有符号，包括初始化函数和Shell命令
foreach(component ${DISCOVERED_COMPONENTS})
    if(TARGET ${component})
        message(STATUS "链接组件库: ${component} (使用--whole-archive)")
        target_link_libraries(demo -Wl,--whole-archive ${component} -Wl,--no-whole-archive)
    endif()
endforeach()

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