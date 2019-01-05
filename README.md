# DBProject113b
## 运行方法
首先确保电脑里安装了cmake
1. mkdir build
2. cd build
3. cmake ..
4. make
即可在build目录下生成可执行档 main

## 项目架构
cpp 源程序放置于 src 目录下，头文件.h 放置于 include 目录下。

## 格式

### 页首定义

> 长度单位为uint

## 注意事项
### 语句的依赖关系
创建模式时使用的InitIndex，注意其应该要在openFile后才使用，因为在openFile函数里方才对数据库路径赋值。


