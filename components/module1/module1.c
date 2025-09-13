#include <stdio.h>
#include "auto_init.h"

static int module1_auto_init(void) {
    printf("Module1 自动初始化完成\n");
    return 0;
}

void module1_init(void) {
    printf("Module1 手动初始化完成\n");
}

int module1_test(void) {
    printf("Module1 测试函数\n");
    return 0;
}

long module1_cmd(void) {
    printf("执行 Module1 命令\n");
    module1_test();
    return 0;
}

// 自动初始化导出 - Component 级别
INIT_COMPONENT_EXPORT(module1_auto_init);

// Shell 命令导出
SHELL_EXPORT_CMD(module1_cmd, mod1, "Module1 测试命令");