/*
 * Linux平台自动初始化机制实现
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "auto_init.h"

// 声明链接器生成的符号 - 使用弱符号，如果段不存在则为NULL
extern struct init_desc __start_init_fn_1[] __attribute__((weak));
extern struct init_desc __stop_init_fn_1[] __attribute__((weak));
extern struct init_desc __start_init_fn_2[] __attribute__((weak));
extern struct init_desc __stop_init_fn_2[] __attribute__((weak));
extern struct init_desc __start_init_fn_3[] __attribute__((weak));
extern struct init_desc __stop_init_fn_3[] __attribute__((weak));
extern struct init_desc __start_init_fn_4[] __attribute__((weak));
extern struct init_desc __stop_init_fn_4[] __attribute__((weak));
extern struct init_desc __start_init_fn_5[] __attribute__((weak));
extern struct init_desc __stop_init_fn_5[] __attribute__((weak));
extern struct init_desc __start_init_fn_6[] __attribute__((weak));
extern struct init_desc __stop_init_fn_6[] __attribute__((weak));

extern struct finsh_syscall __start_FSymTab[] __attribute__((weak));
extern struct finsh_syscall __stop_FSymTab[] __attribute__((weak));

// 定义初始化级别信息
struct init_level_info {
    const char* level_name;
    struct init_desc* start;
    struct init_desc* stop;
};

static struct init_level_info init_levels[] = {
    {"Board", __start_init_fn_1, __stop_init_fn_1},
    {"Device", __start_init_fn_2, __stop_init_fn_2},
    {"Component", __start_init_fn_3, __stop_init_fn_3},
    {"FileSystem", __start_init_fn_4, __stop_init_fn_4},
    {"Environment", __start_init_fn_5, __stop_init_fn_5},
    {"Application", __start_init_fn_6, __stop_init_fn_6},
};

#define INIT_LEVELS_COUNT (sizeof(init_levels) / sizeof(init_levels[0]))

/**
 * 执行指定级别的初始化函数
 */
static int run_init_level(struct init_level_info* level_info) {
    struct init_desc* desc;
    int result;
    int success_count = 0;
    int total_count = 0;
    
    // 检查段是否存在
    if (level_info->start == NULL || level_info->stop == NULL || 
        level_info->start == level_info->stop) {
        printf("=== %s级别：无初始化函数 ===\n\n", level_info->level_name);
        return 0;
    }
    
    printf("=== %s级别初始化开始 ===\n", level_info->level_name);
    
    // 遍历该级别的所有初始化函数
    for (desc = level_info->start; desc < level_info->stop; desc++) {
        total_count++;
        printf("初始化 %s...", desc->fn_name);
        
        result = desc->fn();
        if (result == 0) {
            printf(" 成功\n");
            success_count++;
        } else {
            printf(" 失败 (返回码: %d)\n", result);
        }
    }
    
    printf("=== %s级别初始化完成: %d/%d 成功 ===\n\n", 
           level_info->level_name, success_count, total_count);
           
    return (success_count == total_count) ? 0 : -1;
}

/**
 * 执行所有级别的自动初始化
 */
int auto_components_init(void) {
    printf("开始执行自动初始化流程...\n\n");
    
    int total_success = 0;
    
    // 按级别顺序执行初始化
    for (size_t i = 0; i < INIT_LEVELS_COUNT; i++) {
        if (run_init_level(&init_levels[i]) == 0) {
            total_success++;
        }
    }
    
    printf("自动初始化流程完成: %d/%zu 级别成功\n", total_success, INIT_LEVELS_COUNT);
    return (total_success == INIT_LEVELS_COUNT) ? 0 : -1;
}

/**
 * 初始化Shell命令系统
 */
int auto_shell_init(void) {
    struct finsh_syscall* cmd;
    int cmd_count = 0;
    
    printf("=== 初始化Shell命令系统 ===\n");
    
    // 检查Shell命令段是否存在
    if (__start_FSymTab == NULL || __stop_FSymTab == NULL || 
        __start_FSymTab == __stop_FSymTab) {
        printf("未发现Shell命令\n");
        printf("Shell命令系统初始化完成\n\n");
        return 0;
    }
    
    // 统计命令数量
    for (cmd = __start_FSymTab; cmd < __stop_FSymTab; cmd++) {
        cmd_count++;
    }
    
    printf("发现 %d 个Shell命令\n", cmd_count);
    printf("Shell命令系统初始化完成\n\n");
    
    return 0;
}

/**
 * 显示所有可用的Shell命令
 */
void show_shell_commands(void) {
    struct finsh_syscall* cmd;
    int cmd_count = 0;
    
    printf("=== 可用的Shell命令 ===\n");
    
    // 检查Shell命令段是否存在
    if (__start_FSymTab == NULL || __stop_FSymTab == NULL || 
        __start_FSymTab == __stop_FSymTab) {
        printf("无可用命令\n");
        return;
    }
    
    printf("%-15s %s\n", "命令", "描述");
    printf("%-15s %s\n", "----", "----");
    
    for (cmd = __start_FSymTab; cmd < __stop_FSymTab; cmd++) {
        if (cmd->name != NULL) {
#if defined(FINSH_USING_DESCRIPTION) && defined(FINSH_USING_SYMTAB)
            printf("%-15s %s\n", cmd->name, cmd->desc ? cmd->desc : "无描述");
#else
            printf("%-15s\n", cmd->name);
#endif
        }
        cmd_count++;
    }
    
    printf("\n共 %d 个命令\n", cmd_count);
}

/**
 * 执行Shell命令
 */
long execute_shell_command(const char* cmd_name) {
    struct finsh_syscall* cmd;
    
    // 检查Shell命令段是否存在
    if (__start_FSymTab == NULL || __stop_FSymTab == NULL || 
        __start_FSymTab == __stop_FSymTab) {
        printf("错误: 无可用命令\n");
        return -1;
    }
    
    for (cmd = __start_FSymTab; cmd < __stop_FSymTab; cmd++) {
        if (strcmp(cmd->name, cmd_name) == 0) {
            printf("执行命令: %s\n", cmd_name);
            return cmd->func();
        }
    }
    
    printf("错误: 未找到命令 '%s'\n", cmd_name);
    return -1;
}

// 辅助函数：获取初始化统计信息
void show_init_stats(void) {
    printf("=== 初始化统计信息 ===\n");
    
    for (size_t i = 0; i < INIT_LEVELS_COUNT; i++) {
        struct init_level_info* level = &init_levels[i];
        int count = 0;
        
        if (level->start != NULL && level->stop != NULL) {
            count = level->stop - level->start;
        }
        printf("%-12s级别: %d 个初始化函数\n", level->level_name, count);
    }
    
    int shell_count = 0;
    if (__start_FSymTab != NULL && __stop_FSymTab != NULL) {
        shell_count = __stop_FSymTab - __start_FSymTab;
    }
    printf("Shell命令: %d 个\n", shell_count);
    printf("\n");
}