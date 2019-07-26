# 指定最低 cmake 版本
cmake_minimum_required(VERSION 3.4.1)

# 需要引入我们头文件,以这个配置的目录为基准
include_directories(src/main/jniLibs/include)
include_directories(src/main/jniLibs/other)

# 添加共享库搜索路径
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi)

# 指定源文件目录
#AUX_SOURCE_DIRECTORY(${CMAKE_SOURCE_DIR}/src/main/cpp SRC_LIST)
FILE(GLOB SRC_LIST "src/main/cpp/*.cpp")

add_library(
        # Sets the name of the library.
        native-lib
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        src/main/cpp/XJNICall.cpp
        src/main/cpp/XFFmpeg.cpp
        src/main/cpp/XFFmpeg.h
        src/main/cpp/XAudio.cpp
        src/main/cpp/XAudio.h
        src/main/cpp/XPacketQueue.cpp
        src/main/cpp/XPacketQueue.h
        src/main/cpp/XPlayerStatus.cpp
        src/main/cpp/XPlayerStatus.h
        ${SRC_LIST}
)

target_link_libraries( # Specifies the target library.
        # 链接额外的 ffmpeg 的编译
        native-lib
        # 编解码(最重要的库)
        avcodec-57
        # 设备信息
        avdevice-57
        # 滤镜特效处理库
        avfilter-6
        # 封装格式处理库
        avformat-57
        # 工具库(大部分库都需要这个库的支持)
        avutil-55
        # 后期处理
        postproc-54
        # 音频采样数据格式转换库
        swresample-2
        # 视频像素数据格式转换
        swscale-4
        # 链接 android ndk 自带的一些库
        android
        OpenSLES
        # Links the target library to the log library
        # included in the NDK.
        log)