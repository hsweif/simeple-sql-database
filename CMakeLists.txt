#Use for google testing
configure_file(./CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()



#1.cmake verson，指定cmake版本
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
PROJECT(DBProject113b)

#3.head file path，头文件目录
INCLUDE_DIRECTORIES(
    /usr/local/include
    include
    sql-parser/src
)

#4.source directory，源文件目录
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
ADD_SUBDIRECTORY( src/RecordModule )
ADD_SUBDIRECTORY( src/IndexModule )
ADD_SUBDIRECTORY( sql-parser/src)
ADD_SUBDIRECTORY( sql-parser/src/util )
ADD_SUBDIRECTORY( sql-parser/src/parser )
ADD_SUBDIRECTORY( sql-parser/src/sql )

#5.set environment variable，设置环境变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行的时候会出现各种问题，比如"symbol lookup error xxxxx , undefined symbol"
SET(SRC
${DIR_SRCS}
        include/IndexModule/IndexHandle.h)

# 添加要编译的可执行文件
ADD_EXECUTABLE(main src/main.cpp)
# ADD_EXECUTABLE(create src/create.cpp)
# ADD_EXECUTABLE(drop src/drop.cpp)
# ADD_EXECUTABLE(use src/use.cpp)
# ADD_EXECUTABLE(show src/show.cpp)
# 添加可执行文件所需要的库，比如用到了RecordModule,在src对应的文件夹下定义子cmakelist，就添加该库的名称
TARGET_LINK_LIBRARIES(main RecordModule)
TARGET_LINK_LIBRARIES(main IndexModule)
TARGET_LINK_LIBRARIES(main SQLParserModule)
TARGET_LINK_LIBRARIES(main SQLParser_Util)
TARGET_LINK_LIBRARIES(main SQLParser_Parser)
TARGET_LINK_LIBRARIES(main SQLParser_SQL)
# TARGET_LINK_LIBRARIES(use RecordModule)
# TARGET_LINK_LIBRARIES(use IndexModule)
link_directories("./sql-parser")

# 使用规范的单元测试,采用Google Test
ENABLE_TESTING()
ADD_EXECUTABLE(testfile src/test.cpp)
TARGET_LINK_LIBRARIES(testfile RecordModule)
TARGET_LINK_LIBRARIES(testfile IndexModule)
TARGET_LINK_LIBRARIES(testfile SQLParserModule)
TARGET_LINK_LIBRARIES(testfile SQLParser_Util)
TARGET_LINK_LIBRARIES(testfile SQLParser_Parser)
TARGET_LINK_LIBRARIES(testfile SQLParser_SQL)
target_link_libraries(testfile gtest_main)
ADD_TEST(mytest testfile)
