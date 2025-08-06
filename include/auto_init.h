/*
 * Linux平台自动初始化机制
 * 基于RT-Thread的设计实现Linux版本
 */
#ifndef __AUTO_INIT_H__
#define __AUTO_INIT_H__

#ifdef __cplusplus
extern "C" {
#endif

// 初始化函数类型定义
typedef int (*init_fn_t)(void);

// Shell命令函数类型定义
typedef long (*shell_func_t)(void);

// 简化的初始化描述结构体 - 只存储函数指针和调试信息
struct init_desc {
    init_fn_t fn;
    const char* fn_name;  // 用于调试
};

// 与RT-Thread完全兼容的条件编译
#define FINSH_USING_DESCRIPTION
#define FINSH_USING_SYMTAB
#define FINSH_USING_OPTION_COMPLETION

// RT-Thread兼容的辅助宏
#define rt_used __attribute__((used))
#define rt_section(x) __attribute__((section(x)))

#ifdef FINSH_USING_DESCRIPTION
#define FINSH_DESC(cmd, desc) __fsym_##cmd##_desc,
#else
#define FINSH_DESC(cmd, desc)
#endif

#ifdef FINSH_USING_OPTION_COMPLETION
#define FINSH_COND(opt) opt,
#else
#define FINSH_COND(opt)
#endif

// RT-Thread兼容的选项完成结构体（先声明）
struct msh_cmd_opt;

// RT-Thread兼容的Shell命令结构体（使用finsh_syscall名称）
struct finsh_syscall {
    const char *name;        // 命令名称指针
#if defined(FINSH_USING_DESCRIPTION) && defined(FINSH_USING_SYMTAB)
    const char *desc;        // 命令描述指针  
#endif
#ifdef FINSH_USING_OPTION_COMPLETION
    struct msh_cmd_opt *opt; // 选项完成指针
#endif
    shell_func_t func;       // 函数指针
};

// 为兼容性保留旧名称
typedef struct finsh_syscall shell_syscall;

// 使用__attribute__((section()))将函数放入指定段
#define __used __attribute__((used))
#define __section(x) __attribute__((section(x)))

// 自动初始化导出宏 - 简化版本
#define AUTO_INIT_EXPORT(fn, level)                                        \
    __used const char __init_name_##fn[] = #fn;                            \
    __used const struct init_desc __init_desc_##fn __section("init_fn_" level) = { \
        fn, __init_name_##fn                                               \
    }

// Shell命令导出宏 - 完全按照RT-Thread的MSH_FUNCTION_EXPORT_CMD实现
#define SHELL_EXPORT_CMD(func, cmd, desc)                                 \
    const char __fsym_##cmd##_name[] rt_section(".rodata.name") = #cmd;    \
    const char __fsym_##cmd##_desc[] rt_section(".rodata.name") = #desc;   \
    rt_used const struct finsh_syscall __fsym_##cmd rt_section("FSymTab") = { \
        __fsym_##cmd##_name,    \
        FINSH_DESC(cmd, desc)   \
        FINSH_COND(0)           \
        (shell_func_t)&func     \
    }

// 不同等级的初始化宏
#define INIT_BOARD_EXPORT(fn)    AUTO_INIT_EXPORT(fn, "1")
#define INIT_DEVICE_EXPORT(fn)   AUTO_INIT_EXPORT(fn, "2")
#define INIT_COMPONENT_EXPORT(fn) AUTO_INIT_EXPORT(fn, "3")
#define INIT_FS_EXPORT(fn)       AUTO_INIT_EXPORT(fn, "4")
#define INIT_ENV_EXPORT(fn)      AUTO_INIT_EXPORT(fn, "5")
#define INIT_APP_EXPORT(fn)      AUTO_INIT_EXPORT(fn, "6")

// 运行时函数声明
int auto_components_init(void);
int auto_shell_init(void);
void show_shell_commands(void);
void show_init_stats(void);
long execute_shell_command(const char* cmd_name);

#ifdef __cplusplus
}
#endif

#endif /* __AUTO_INIT_H__ */