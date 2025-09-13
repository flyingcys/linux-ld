#include <stdio.h>
#include "auto_init.h"

static int test_module_auto_init(void) {
    printf("Test Module 自动初始化完成\n");
    return 0;
}

void test_module_init(void) {
    printf("Test Module 手动初始化完成\n");
}

int test_module_test(void) {
    printf("Test Module 测试函数\n");
    return 0;
}

long test_module_cmd(void) {
    printf("执行 Test Module 命令\n");
    test_module_test();
    return 0;
}

// 自动初始化导出 - Component 级别
INIT_COMPONENT_EXPORT(test_module_auto_init);

// Shell 命令导出
SHELL_EXPORT_CMD(test_module_cmd, testmod, "Test Module 测试命令");