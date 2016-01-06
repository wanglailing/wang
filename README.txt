1. 代码目录结构：
.
├── bin                             # 可执行文件
│   └── lib
├── build                           # 工程构建目录
│   ├── Debug
│   └── Release
├── cmake_modules                   # 自定义cmake模块
├── doc                             # 文档
├── src                             # 源代码
│   ├── apps                        # 应用模块
│   │   ├── corebusiness            # DMS核心业务数据处理模块
│   │   │   └── test
│   │   ├── dataswitch              # 数据交换模块
│   │   ├── eoaservice              # EOAS扩展模块
│   │   ├── gnetmonitor             # G网监测模块
│   │   │   └── test
│   │   ├── maintainservice         # 维护服务模块
│   │   ├── procmonitor             # 进程监控模块
│   │   ├── storeservice            # 存储服务模块
│   │   ├── tcaservice              # TCAS扩展模块(目前已废弃，相关代码合并至传输控制模块)
│   │   └── transportctrl           # 传输控制模块
│   │       └── test
│   └── helium                      # 程序框架
└── upgrade                         # 制作升级包的临时目录

2. 工程构建方法：
进入build目录的Debug或者Release子目录，执行命令
    cmake -DARM3250=ON -DCMAKE_BUILD_TYPE=Debug(或者Release) ../../
    make
构建完成后，可执行文件将会存放在bin目录。
