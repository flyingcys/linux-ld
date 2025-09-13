#include <stdio.h>
#include "auto_init.h"

static int module2_init(void) {
    printf("Module2 初始化完成\n");
    return 0;
}

void module2_process(int data) {
    printf("Module2 处理数据: %d\n", data);
}


// 自动初始化导出 - Component 级别
INIT_COMPONENT_EXPORT(module2_init);