/*
 * 自动初始化机制的链接器脚本
 * 确保各个初始化段按正确顺序排列
 */

/* 在默认链接器脚本的基础上添加自定义段 */
SECTIONS
{
    /* 自动初始化段 - 按级别排序 */
    .init_sections :
    {
        /* 级别1: Board初始化 */
        __start_init_fn_1 = .;
        KEEP(*(SORT(.init_fn.1*)))
        __stop_init_fn_1 = .;
        
        /* 级别2: Device初始化 */
        __start_init_fn_2 = .;
        KEEP(*(SORT(.init_fn.2*)))
        __stop_init_fn_2 = .;
        
        /* 级别3: Component初始化 */
        __start_init_fn_3 = .;
        KEEP(*(SORT(.init_fn.3*)))
        __stop_init_fn_3 = .;
        
        /* 级别4: FileSystem初始化 */
        __start_init_fn_4 = .;
        KEEP(*(SORT(.init_fn.4*)))
        __stop_init_fn_4 = .;
        
        /* 级别5: Environment初始化 */
        __start_init_fn_5 = .;
        KEEP(*(SORT(.init_fn.5*)))
        __stop_init_fn_5 = .;
        
        /* 级别6: Application初始化 */
        __start_init_fn_6 = .;
        KEEP(*(SORT(.init_fn.6*)))
        __stop_init_fn_6 = .;
    }
    
    /* Shell命令段 */
    .shell_sections :
    {
        __start_ShellTab = .;
        KEEP(*(ShellTab))
        __stop_ShellTab = .;
    }
}

/* 插入到默认的链接器脚本中 */
INSERT AFTER .rodata;