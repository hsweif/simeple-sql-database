#1.cmake verson，指定cmake版本
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
PROJECT(DBProject113b)

#3.head file path，头文件目录
INCLUDE_DIRECTORIES(
    include
    sql-parser/src
)

#4.source directory，源文件目录
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
ADD_SUBDIRECTORY( src/RecordModule )
ADD_SUBDIRECTORY( src/IndexModule )
ADD_SUBDIRECTORY( sql-parser/src)

#5.set environment variable，设置环境变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行的时候会出现各种问题，比如"symbol lookup error xxxxx , undefined symbol"
SET(SRC
${DIR_SRCS}
        include/IndexModule/IndexHandle.h)

#6.add executable file，添加要编译的可执行文件
ADD_EXECUTABLE(main src/main.cpp)
# ADD_EXECUTABLE(create src/create.cpp)
# ADD_EXECUTABLE(drop src/drop.cpp)
# ADD_EXECUTABLE(use src/use.cpp)
# ADD_EXECUTABLE(show src/show.cpp)
#7.add link library，添加可执行文件所需要的库，比如用到了RecordModule,在src对应的文件夹下定义子cmakelist，就添加该库的名称
TARGET_LINK_LIBRARIES(main RecordModule)
TARGET_LINK_LIBRARIES(main IndexModule)
TARGET_LINK_LIBRARIES(main ParserModule)
# TARGET_LINK_LIBRARIES(use RecordModule)
# TARGET_LINK_LIBRARIES(use IndexModule)
link_directories("./sql-parser")
