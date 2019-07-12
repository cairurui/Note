

### 在模块中配置支持的 abi 和指定 CMakeLists 文件
``` 
android {
    compileSdkVersion 28
    buildToolsVersion "29.0.0"

    defaultConfig {
        applicationId "com.xc.note"
        ...
        externalNativeBuild {
            cmake {
                cppFlags ""
                // 只生成 armeabi
                abiFilters "armeabi"
            }
        }
    }
    externalNativeBuild {
        cmake {
            path "CMakeLists.txt"
        }
    }
}
```

### 配置 CMakeLists

```cmake
# 指定最低 cmake 版本
cmake_minimum_required(VERSION 3.4.1)

# 需要引入我们头文件,以这个配置的目录为基准
include_directories(src/main/jniLibs/include)
# include_directories(src/main/jniLibs/other)

# 添加共享库搜索路径
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi)

# 指定源文件目录
AUX_SOURCE_DIRECTORY(${CMAKE_SOURCE_DIR}/src/main/cpp SRC_LIST)
# FILE(GLOB SRC_LIST "src/main/cpp/*.cpp")

add_library(
        # Sets the name of the library.
        native-lib
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
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

```

### 在 cpp 中使用测试
```cpp
#include <jni.h>

#include <android/log.h>

extern "C" {
#include "libavformat/avformat.h"
}

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


extern "C" void JNICALL
Java_com_xc_note_MainActivity_test(JNIEnv *env, jobject instance_) {


    av_register_all();
    LOGE("xiaocai test ");
}
```


### 遇到的问题：
编译不通过
```
CMake Error: CMake was unable to find a build program corresponding to "Ninja". CMAKE_MAKE_PROGRAM is not set. You probably need to select a different build tool
```
通过指定 buildToolsVersion 的版本和 classpath 'com.android.tools.build:gradle:3.4.1' 以及 gradle-5.1.1-all 解决。