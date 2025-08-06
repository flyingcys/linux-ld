/*
 * Linux平台自动初始化机制演示程序
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auto_init.h"

// 声明外部命令执行函数
long execute_shell_command(const char* cmd_name);

/**
 * 简单的Shell命令行解析和执行
 */
static void simple_shell(void) {
    char input[256];
    char *cmd;
    
    printf("\n=== 简单Shell演示 ===\n");
    printf("输入 'help' 查看可用命令，输入 'exit' 退出\n\n");
    
    while (1) {
        printf("shell> ");
        
        // 读取用户输入
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // 移除换行符
        input[strcspn(input, "\n")] = 0;
        
        // 跳过空输入
        if (strlen(input) == 0) {
            continue;
        }
        
        // 解析命令（简单实现，只支持单个命令）
        cmd = strtok(input, " ");
        
        if (cmd == NULL) {
            continue;
        }
        
        // 处理内置命令
        if (strcmp(cmd, "exit") == 0) {
            printf("退出Shell\n");
            break;
        } else if (strcmp(cmd, "help") == 0) {
            show_shell_commands();
        } else if (strcmp(cmd, "stats") == 0) {
            show_init_stats();
        } else {
            // 执行自动注册的命令
            execute_shell_command(cmd);
        }
        
        printf("\n");
    }
}

/**
 * 主函数
 */
int main() {
    printf("============================================\n");
    printf("Linux平台自动初始化机制演示程序\n");
    printf("基于RT-Thread设计实现\n");
    printf("============================================\n\n");
    
    // 显示初始化统计信息
    show_init_stats();
    
    // 执行自动初始化
    int init_result = auto_components_init();
    if (init_result != 0) {
        printf("警告: 某些初始化函数执行失败\n");
    }
    
    // 初始化Shell命令系统
    auto_shell_init();
    
    // 显示所有可用命令
    show_shell_commands();
    
    // 启动简单的Shell
    simple_shell();
    
    printf("\n程序退出\n");
    return 0;
}