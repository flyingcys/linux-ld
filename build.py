#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
构建脚本 - 类似ESP-IDF的idf.py
提供统一的项目构建、配置和管理接口
"""

import os
import sys
import argparse
import subprocess
import shutil
from pathlib import Path

# 项目配置
PROJECT_NAME = "auto_init_demo"
BUILD_DIR = "build"
KCONFIG_FILE = "Kconfig"
CONFIG_FILE = ".config"
DEFCONFIG_FILE = "defconfig"

class Colors:
    """终端颜色定义"""
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def print_info(message):
    """打印信息"""
    print(f"{Colors.OKBLUE}[INFO]{Colors.ENDC} {message}")

def print_success(message):
    """打印成功信息"""
    print(f"{Colors.OKGREEN}[SUCCESS]{Colors.ENDC} {message}")

def print_warning(message):
    """打印警告"""
    print(f"{Colors.WARNING}[WARNING]{Colors.ENDC} {message}")

def print_error(message):
    """打印错误"""
    print(f"{Colors.FAIL}[ERROR]{Colors.ENDC} {message}")

def print_header(message):
    """打印标题"""
    print(f"\n{Colors.HEADER}{Colors.BOLD}=== {message} ==={Colors.ENDC}")

def run_command(cmd, cwd=None, check=True, capture_output=False):
    """运行命令"""
    if isinstance(cmd, str):
        cmd = cmd.split()

    print_info(f"运行命令: {' '.join(cmd)}")

    try:
        if capture_output:
            result = subprocess.run(cmd, cwd=cwd, check=check,
                                  capture_output=True, text=True)
            return result
        else:
            result = subprocess.run(cmd, cwd=cwd, check=check)
            return result
    except subprocess.CalledProcessError as e:
        print_error(f"命令执行失败: {e}")
        if capture_output and hasattr(e, 'stderr') and e.stderr:
            print_error(f"错误输出: {e.stderr}")
        sys.exit(1)

def check_dependencies():
    """检查依赖"""
    print_header("检查依赖")

    # 检查Python
    if sys.version_info < (3, 6):
        print_error("需要Python 3.6或更高版本")
        sys.exit(1)
    print_success(f"Python版本: {sys.version}")

    # 检查CMake
    try:
        result = run_command("cmake --version", capture_output=True)
        cmake_version = result.stdout.split('\n')[0]
        print_success(f"CMake: {cmake_version}")
    except Exception:
        print_error("CMake未安装或不在PATH中")
        sys.exit(1)

    # 检查kconfiglib
    try:
        import kconfiglib  # pylint: disable=import-outside-toplevel,unused-import
        print_success("kconfiglib: 已安装")
    except ImportError:
        print_warning("kconfiglib未安装，尝试自动安装...")
        try:
            run_command([sys.executable, "-m", "pip", "install", "kconfiglib"])
            print_success("kconfiglib安装成功")
        except Exception:
            print_error("无法安装kconfiglib，请手动安装: pip install kconfiglib")
            sys.exit(1)

def ensure_kconfig():
    """确保Kconfig配置文件存在"""
    if not os.path.exists(KCONFIG_FILE):
        print_error(f"Kconfig文件不存在: {KCONFIG_FILE}")
        sys.exit(1)

def process_kconfig():
    """处理Kconfig配置"""
    print_header("处理Kconfig配置")

    ensure_kconfig()

    import kconfiglib

    # 加载Kconfig
    print_info("加载Kconfig文件...")
    kconf = kconfiglib.Kconfig(KCONFIG_FILE)

    # 如果.config不存在，使用默认配置或生成
    if not os.path.exists(CONFIG_FILE):
        if os.path.exists(DEFCONFIG_FILE):
            print_info(f"使用默认配置: {DEFCONFIG_FILE}")
            shutil.copy(DEFCONFIG_FILE, CONFIG_FILE)
        else:
            print_info("生成默认配置...")
            kconf.write_config(CONFIG_FILE)

    # 加载配置
    print_info("加载配置文件...")
    kconf.load_config(CONFIG_FILE)

    # 生成config.h
    print_info("生成config.h...")
    kconf.write_autoconf("config.h")

    # 生成CMake配置文件
    cmake_config = os.path.join(BUILD_DIR, "config.cmake")
    os.makedirs(BUILD_DIR, exist_ok=True)

    print_info("生成CMake配置...")
    with open(cmake_config, 'w') as f:
        for name, sym in kconf.defined_syms:
            if sym.str_value:
                f.write(f'set(CONFIG_{name} "{sym.str_value}")\n')
            elif sym.tri_value == 2:  # y
                f.write(f'set(CONFIG_{name} TRUE)\n')
            elif sym.tri_value == 1:  # m
                f.write(f'set(CONFIG_{name} MODULE)\n')
            else:  # n
                f.write(f'set(CONFIG_{name} FALSE)\n')

    print_success("Kconfig配置处理完成")

def cmake_configure():
    """CMake配置"""
    print_header("CMake配置")

    os.makedirs(BUILD_DIR, exist_ok=True)

    # 运行CMake配置
    cmd = ["cmake", ".."]
    run_command(cmd, cwd=BUILD_DIR)

    print_success("CMake配置完成")

def build_project():
    """构建项目"""
    print_header("构建项目")

    if not os.path.exists(BUILD_DIR):
        print_error(f"构建目录不存在: {BUILD_DIR}")
        print_info("请先运行配置: python build.py configure")
        sys.exit(1)

    # 运行构建
    cmd = ["make", "-j", str(os.cpu_count() or 4)]
    run_command(cmd, cwd=BUILD_DIR)

    print_success("项目构建完成")

def clean_project():
    """清理项目"""
    print_header("清理项目")

    if os.path.exists(BUILD_DIR):
        print_info(f"删除构建目录: {BUILD_DIR}")
        shutil.rmtree(BUILD_DIR)

    # 清理生成的配置文件
    files_to_clean = ["config.h"]
    for file in files_to_clean:
        if os.path.exists(file):
            print_info(f"删除文件: {file}")
            os.remove(file)

    print_success("清理完成")

def full_clean():
    """完全清理"""
    print_header("完全清理")

    clean_project()

    # 也删除配置文件
    if os.path.exists(CONFIG_FILE):
        print_info(f"删除配置文件: {CONFIG_FILE}")
        os.remove(CONFIG_FILE)

    print_success("完全清理完成")

def menuconfig():
    """运行menuconfig"""
    print_header("运行配置界面")

    ensure_kconfig()

    try:
        import kconfiglib

        # 加载Kconfig
        kconf = kconfiglib.Kconfig(KCONFIG_FILE)

        # 如果配置文件存在，加载它
        if os.path.exists(CONFIG_FILE):
            kconf.load_config(CONFIG_FILE)

        # 运行menuconfig
        kconfiglib.menuconfig(kconf)

        # 保存配置
        kconf.write_config(CONFIG_FILE)

        print_success("配置已保存")

    except ImportError:
        print_error("kconfiglib未安装")
        sys.exit(1)
    except Exception as e:
        print_error(f"menuconfig运行失败: {e}")
        sys.exit(1)

def defconfig():
    """生成默认配置"""
    print_header("生成默认配置")

    ensure_kconfig()

    try:
        import kconfiglib

        kconf = kconfiglib.Kconfig(KCONFIG_FILE)
        kconf.write_config(CONFIG_FILE)

        print_success(f"默认配置已生成: {CONFIG_FILE}")

    except Exception as e:
        print_error(f"生成默认配置失败: {e}")
        sys.exit(1)

def savedefconfig():
    """保存当前配置为defconfig"""
    print_header("保存默认配置")

    if not os.path.exists(CONFIG_FILE):
        print_error(f"配置文件不存在: {CONFIG_FILE}")
        sys.exit(1)

    shutil.copy(CONFIG_FILE, DEFCONFIG_FILE)
    print_success(f"配置已保存为: {DEFCONFIG_FILE}")

def run_tests():
    """运行测试"""
    print_header("运行测试")

    if not os.path.exists(BUILD_DIR):
        print_error("请先构建项目")
        sys.exit(1)

    # 运行CTest
    cmd = ["ctest", "--output-on-failure"]
    run_command(cmd, cwd=BUILD_DIR)

    print_success("测试完成")

def flash_or_run():
    """运行程序"""
    print_header("运行程序")

    demo_path = os.path.join(BUILD_DIR, "demo")
    if not os.path.exists(demo_path):
        print_error("demo程序不存在，请先构建项目")
        sys.exit(1)

    print_info("运行demo程序...")
    run_command([demo_path])

def show_size():
    """显示大小信息"""
    print_header("程序大小信息")

    demo_path = os.path.join(BUILD_DIR, "demo")
    if not os.path.exists(demo_path):
        print_error("demo程序不存在，请先构建项目")
        sys.exit(1)

    # 使用size命令显示大小
    try:
        run_command(["size", demo_path])
    except:
        # 如果size命令不可用，使用ls -lh
        run_command(["ls", "-lh", demo_path])

def show_config():
    """显示当前配置"""
    print_header("当前配置")

    if not os.path.exists(CONFIG_FILE):
        print_warning("配置文件不存在")
        return

    with open(CONFIG_FILE, 'r') as f:
        config_content = f.read()

    # 只显示启用的选项
    enabled_options = []
    for line in config_content.split('\n'):
        line = line.strip()
        if line and not line.startswith('#') and '=' in line:
            enabled_options.append(line)

    if enabled_options:
        print("启用的配置选项:")
        for option in enabled_options:
            print(f"  {option}")
    else:
        print("没有找到启用的配置选项")

def main():
    """主函数"""
    parser = argparse.ArgumentParser(
        description=f"{PROJECT_NAME} 构建工具",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
使用示例:
  python build.py configure         # 配置项目
  python build.py build            # 构建项目
  python build.py menuconfig       # 运行配置界面
  python build.py clean            # 清理构建文件
  python build.py fullclean        # 完全清理
  python build.py run              # 运行程序
  python build.py test             # 运行测试

完整的构建流程:
  python build.py menuconfig       # 配置项目选项
  python build.py build            # 构建项目
  python build.py run              # 运行程序
        """
    )

    parser.add_argument(
        'action',
        choices=[
            'configure', 'build', 'clean', 'fullclean',
            'menuconfig', 'defconfig', 'savedefconfig',
            'test', 'run', 'size', 'config', 'all'
        ],
        help='要执行的操作'
    )

    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='显示详细输出'
    )

    args = parser.parse_args()

    # 设置工作目录为脚本所在目录
    script_dir = Path(__file__).parent.absolute()
    os.chdir(script_dir)

    print_header(f"{PROJECT_NAME} 构建工具")
    print_info(f"工作目录: {os.getcwd()}")

    # 检查依赖
    if args.action != 'clean' and args.action != 'fullclean':
        check_dependencies()

    try:
        if args.action == 'configure':
            process_kconfig()
            cmake_configure()

        elif args.action == 'build':
            if not os.path.exists(os.path.join(BUILD_DIR, "Makefile")):
                print_warning("项目未配置，先运行配置...")
                process_kconfig()
                cmake_configure()
            build_project()

        elif args.action == 'clean':
            clean_project()

        elif args.action == 'fullclean':
            full_clean()

        elif args.action == 'menuconfig':
            menuconfig()

        elif args.action == 'defconfig':
            defconfig()

        elif args.action == 'savedefconfig':
            savedefconfig()

        elif args.action == 'test':
            run_tests()

        elif args.action == 'run':
            flash_or_run()

        elif args.action == 'size':
            show_size()

        elif args.action == 'config':
            show_config()

        elif args.action == 'all':
            process_kconfig()
            cmake_configure()
            build_project()
            print_success("完整构建流程完成！")
            print_info("运行 'python build.py run' 来运行程序")

    except KeyboardInterrupt:
        print_warning("\n用户中断操作")
        sys.exit(1)
    except Exception as e:
        print_error(f"操作失败: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()