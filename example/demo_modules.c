/*
 * 演示模块 - 展示自动初始化机制的使用
 */
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "auto_init.h"

// ==================== 板级初始化函数 ====================

static int board_led_init(void) {
    printf("  - LED驱动初始化");
    usleep(100000); // 模拟初始化延时
    return 0;
}
INIT_BOARD_EXPORT(board_led_init);

static int board_clock_init(void) {
    printf("  - 时钟系统初始化");
    usleep(50000);
    return 0;
}
INIT_BOARD_EXPORT(board_clock_init);

static int board_gpio_init(void) {
    printf("  - GPIO初始化");
    usleep(30000);
    return 0;
}
INIT_BOARD_EXPORT(board_gpio_init);

// ==================== 设备驱动初始化函数 ====================

static int device_uart_init(void) {
    printf("  - UART设备初始化");
    usleep(80000);
    return 0;
}
INIT_DEVICE_EXPORT(device_uart_init);

static int device_spi_init(void) {
    printf("  - SPI设备初始化");
    usleep(60000);
    return 0;
}
INIT_DEVICE_EXPORT(device_spi_init);

static int device_i2c_init(void) {
    printf("  - I2C设备初始化");
    usleep(40000);
    return 0;
}
INIT_DEVICE_EXPORT(device_i2c_init);

// ==================== 组件初始化函数 ====================

static int component_network_init(void) {
    printf("  - 网络协议栈初始化");
    usleep(200000);
    return 0;
}
INIT_COMPONENT_EXPORT(component_network_init);

static int component_timer_init(void) {
    printf("  - 定时器组件初始化");
    usleep(50000);
    return 0;
}
INIT_COMPONENT_EXPORT(component_timer_init);

// 模拟一个初始化失败的例子
static int component_sensor_init(void) {
    printf("  - 传感器组件初始化");
    usleep(100000);
    // 模拟初始化失败
    return -1;
}
INIT_COMPONENT_EXPORT(component_sensor_init);

// ==================== 文件系统初始化函数 ====================

static int fs_vfs_init(void) {
    printf("  - VFS文件系统初始化");
    usleep(120000);
    return 0;
}
INIT_FS_EXPORT(fs_vfs_init);

static int fs_devfs_init(void) {
    printf("  - DevFS设备文件系统初始化");
    usleep(80000);
    return 0;
}
INIT_FS_EXPORT(fs_devfs_init);

// ==================== 环境初始化函数 ====================

static int env_config_init(void) {
    printf("  - 环境变量系统初始化");
    usleep(60000);
    return 0;
}
INIT_ENV_EXPORT(env_config_init);

static int env_log_init(void) {
    printf("  - 日志系统初始化");
    usleep(40000);
    return 0;
}
INIT_ENV_EXPORT(env_log_init);

// ==================== 应用程序初始化函数 ====================

static int app_main_init(void) {
    printf("  - 主应用程序初始化");
    usleep(100000);
    return 0;
}
INIT_APP_EXPORT(app_main_init);

static int app_service_init(void) {
    printf("  - 后台服务初始化");
    usleep(80000);
    return 0;
}
INIT_APP_EXPORT(app_service_init);

// ==================== Shell命令函数 ====================

static long cmd_version(void) {
    printf("Linux自动初始化机制演示 v1.0\n");
    printf("基于RT-Thread设计 - 适配Linux平台\n");
    return 0;
}
SHELL_EXPORT_CMD(cmd_version, version, "显示版本信息");

static long cmd_uptime(void) {
    static time_t start_time = 0;
    if (start_time == 0) {
        start_time = time(NULL);
    }
    
    time_t current_time = time(NULL);
    double uptime = difftime(current_time, start_time);
    
    printf("系统运行时间: %.0f 秒\n", uptime);
    return 0;
}
SHELL_EXPORT_CMD(cmd_uptime, uptime, "显示系统运行时间");

static long cmd_date(void) {
    time_t now;
    char *time_str;
    
    time(&now);
    time_str = ctime(&now);
    
    printf("当前时间: %s", time_str);
    return 0;
}
SHELL_EXPORT_CMD(cmd_date, date, "显示当前日期和时间");

static long cmd_demo(void) {
    printf("这是一个演示命令\n");
    printf("展示了如何使用SHELL_EXPORT_CMD宏\n");
    printf("自动注册Shell命令\n");
    return 0;
}
SHELL_EXPORT_CMD(cmd_demo, demo, "演示命令");

static long cmd_test(void) {
    printf("执行自测试...\n");
    
    // 模拟一些测试
    const char* tests[] = {
        "内存测试",
        "CPU测试", 
        "IO测试",
        "网络测试"
    };
    
    for (int i = 0; i < 4; i++) {
        printf("  %s...", tests[i]);
        usleep(200000); // 模拟测试耗时
        printf(" 通过\n");
    }
    
    printf("所有测试通过!\n");
    return 0;
}
SHELL_EXPORT_CMD(cmd_test, test, "运行系统自测试");

// ==================== 一些辅助的演示函数 ====================

static long cmd_list_init(void) {
    printf("显示所有初始化函数的详细信息:\n");
    show_init_stats();
    return 0;
}
SHELL_EXPORT_CMD(cmd_list_init, list, "列出所有初始化函数信息");