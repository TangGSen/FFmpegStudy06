# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)


#FFMpeg配置
#FFmpeg配置目录
set(distribution_DIR ${CMAKE_SOURCE_DIR}/../../../../src/main/jniLibs)

add_library(
            libfaac
            SHARED
            IMPORTED)
set_target_properties(
            libfaac
            PROPERTIES IMPORTED_LOCATION
            ../../../../src/main/jniLibs/armeabi/libfaac.so)


add_library(
            librtmp
            SHARED
            IMPORTED)
set_target_properties(
            librtmp
            PROPERTIES IMPORTED_LOCATION
            ../../../../src/main/jniLibs/armeabi/librtmp.so)

# 封装格式处理库
add_library(
            libx264
            SHARED
            IMPORTED)
set_target_properties(
            libx264
            PROPERTIES IMPORTED_LOCATION
            ../../../../src/main/jniLibs/armeabi/libx264.so)

#配置编译的头文件

include_directories(src/main/jniLibs/include)
include_directories(src/main/jniLibs/include/faac)
include_directories(src/main/jniLibs/include/rtmp)
include_directories(src/main/jniLibs/include/x264)



#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
#判断编译器类型,如果是gcc编译器,则在编译选项中加入c++11支持
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)



# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/push_native.cpp
             src/main/cpp/queue.cpp
            )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib


              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )
 find_library( # Sets the name of the path variable.
              android-lib


              # Specifies the name of the NDK library that
              # you want CMake to locate.
              android)


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib libfaac librtmp libx264
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       ${android-lib})
