# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

# cmake 最低版本
cmake_minimum_required(VERSION 3.10.2)

# 支持gnu++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

# 1. 定义so库和头文件所在目录，方面后面使用
# CMAKE_SOURCE_DIR 为系统变量，指向 CMakeLists.txt 所在目录
# ANDROID_ABI 也是系统变量，指向 so 对应的 CPU 框架目录：armeabi、armeabi-v7a、x86 ...
#【注意：这里有一个坑，那就是不要把so文件放到jniLibs目录下，否则会编译失败，详情见：https://developer.android.com/studio/releases/gradle-plugin#cmake-imported-targets】
set(FFMPEG_LIB_DIR ${CMAKE_SOURCE_DIR}/../nativeLibs/${ANDROID_ABI})

# 2. 添加头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# Declares and names the project.

project("ffmpeg")

# 配置so库编译信息
add_library(avcodec
        SHARED
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_LIB_DIR}/libavcodec.so)

add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_LIB_DIR}/libavfilter.so)

add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_LIB_DIR}/libavformat.so)

add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_LIB_DIR}/libavutil.so)

add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_LIB_DIR}/libswresample.so)

add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_LIB_DIR}/libswscale.so)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# 配置so库编译信息
add_library(
        # 输出so库的名称
        # Sets the name of the library.
        ffmpeg

        # 设置生成库的方式，默认为SHARE动态库
        # Sets the library as a shared library.
        SHARED

        # 列出参与编译的所有源文件
        # Provides a relative path to your source file(s).
        native-lib.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

# 查找代码中使用到的系统库
find_library(
        # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

# 指定编译目标库时，cmake要链接的库
target_link_libraries(
        # 指定目标库，ffmpeg 是在上面 add_library 中配置的目标库
        # Specifies the target library.
        ffmpeg

        # 4. 连接 FFmpeg 相关的库
        avcodec
        avfilter
        avformat
        avutil
        swresample
        swscale

        # Links the target library to the log library
        # included in the NDK.
        # 列出所有需要链接的库
        ${log-lib})