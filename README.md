# Linux平台自动初始化机制

基于RT-Thread的自动初始化机制设计，实现了Linux平台下的自动启动功能。

## 功能特性

- **自动初始化**: 通过宏定义自动注册初始化函数
- **分级初始化**: 支持6个初始化级别，按顺序执行
- **Shell命令**: 自动注册Shell命令，支持运行时调用
- **跨平台**: 使用GCC的section属性，兼容Linux环境
- **易于使用**: 简单的宏定义即可实现自动注册

## 目录结构

```
linux-ld/
├── include/
│   └── auto_init.h          # 头文件，包含宏定义
├── src/
│   └── auto_init.c          # 实现文件，运行时函数
├── example/
│   ├── main.c               # 演示主程序
│   └── demo_modules.c       # 演示模块
├── auto_init.ld             # 可选的链接器脚本
├── CMakeLists.txt           # CMake构建文件
└── README.md                # 说明文档
```

## 核心机制

### 1. 自动初始化

使用 `AUTO_INIT_EXPORT(fn, level)` 宏将初始化函数注册到指定级别：

```c
#include "auto_init.h"

static int my_device_init(void) {
    printf("设备初始化\n");
    return 0;
}

// 将函数注册为设备级别初始化
INIT_DEVICE_EXPORT(my_device_init);
```

### 2. 初始化级别

系统支持6个初始化级别，按顺序执行：

1. **INIT_BOARD_EXPORT(fn)** - 板级初始化（级别1）
2. **INIT_DEVICE_EXPORT(fn)** - 设备初始化（级别2）
3. **INIT_COMPONENT_EXPORT(fn)** - 组件初始化（级别3）
4. **INIT_FS_EXPORT(fn)** - 文件系统初始化（级别4）
5. **INIT_ENV_EXPORT(fn)** - 环境初始化（级别5）
6. **INIT_APP_EXPORT(fn)** - 应用初始化（级别6）

### 3. Shell命令注册

使用 `SHELL_EXPORT_CMD(func, cmd, desc)` 宏注册Shell命令：

```c
static long cmd_hello(void) {
    printf("Hello, World!\n");
    return 0;
}

// 注册命令：命令名为"hello"，描述为"打印问候语"
SHELL_EXPORT_CMD(cmd_hello, hello, "打印问候语");
```

## 编译和运行

### 1. 使用CMake构建

```bash
cd linux-ld
mkdir build
cd build
cmake ..
make
```

### 2. 运行演示程序

```bash
./demo
```

### 3. 可用的构建目标

- `make demo` - 构建演示程序
- `make auto_init` - 构建自动初始化库
- `make install` - 安装库和头文件
- `make test` - 运行测试
- `make help` - 显示帮助信息

## 演示程序功能

运行演示程序后，你将看到：

1. **初始化统计信息** - 显示各级别的初始化函数数量
2. **自动初始化过程** - 按级别顺序执行所有初始化函数
3. **Shell命令系统** - 显示所有注册的命令
4. **交互式Shell** - 可以执行注册的命令

### 可用的Shell命令

- `help` - 显示所有命令
- `version` - 显示版本信息
- `uptime` - 显示系统运行时间
- `date` - 显示当前日期时间
- `demo` - 演示命令
- `test` - 运行系统自测试
- `list` - 列出初始化函数信息
- `stats` - 显示统计信息
- `exit` - 退出Shell

## 技术实现

### GCC段属性

使用GCC的 `__attribute__((section()))` 将函数指针放入指定段：

```c
#define AUTO_INIT_EXPORT(fn, level) \
    __used const struct init_desc __init_desc_##fn __section(".init_fn." level) = { \
        #fn, fn, level \
    }
```

### 链接器符号

GCC自动为每个段生成 `__start_` 和 `__stop_` 符号：

```c
extern struct init_desc __start_init_fn_1[];  // 段开始
extern struct init_desc __stop_init_fn_1[];   // 段结束
```

### 运行时遍历

在运行时遍历段中的所有函数并执行：

```c
for (desc = __start_init_fn_1; desc < __stop_init_fn_1; desc++) {
    result = desc->fn();  // 调用初始化函数
}
```

## 与RT-Thread的比较

| 特性 | RT-Thread | Linux版本 |
|------|-----------|-----------|
| 段名称 | `.rti_fn.level` | `.init_fn.level` |
| 符号名称 | `__rt_init_start/end` | `__start/__stop_段名` |
| Shell段 | `FSymTab` | `ShellTab` |
| 链接器脚本 | 必需 | 可选（GCC自动生成符号）|
| 平台支持 | 嵌入式 | Linux |

## 扩展使用

### 在你的项目中使用

1. 复制 `include/auto_init.h` 和 `src/auto_init.c` 到你的项目
2. 在CMakeLists.txt中添加库
3. 在你的模块中使用初始化宏
4. 在main函数中调用 `auto_components_init()`

### 添加新的初始化级别

可以通过修改头文件和源文件来添加新的初始化级别：

```c
// 在auto_init.h中添加
#define INIT_CUSTOM_EXPORT(fn) AUTO_INIT_EXPORT(fn, "7")

// 在auto_init.c中添加对应的段符号声明和级别信息
```

## 注意事项

1. **链接顺序**: 确保包含初始化函数的目标文件被链接到最终程序中
2. **符号可见性**: 使用`__used`属性防止编译器优化掉未直接调用的函数
3. **初始化顺序**: 同一级别内的函数执行顺序由链接器决定
4. **错误处理**: 初始化函数应该返回0表示成功，非0表示失败

## 许可证

本项目基于Apache-2.0许可证开源。