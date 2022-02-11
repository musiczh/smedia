cmake_minimum_required(VERSION 3.10)
project(smedia)

set(CMAKE_CXX_STANDARD 11)

# framework
include_directories(framework/core framework/data framework/execute framework/json
        framework/registration framework/utils)
file(GLOB_RECURSE FILE_CORE framework/core/*.cpp)
file(GLOB_RECURSE FILE_DATA framework/data/*.cpp)
file(GLOB_RECURSE FILE_EXECUTE framework/execute/*.cpp)
file(GLOB_RECURSE FILE_JSON framework/json/*.cpp)
file(GLOB_RECURSE FILE_REGIS framework/registration/*.cpp)
file(GLOB_RECURSE FILE_UTILS framework/utils/*.cpp)
set(SOURCE_FRAMEWORK ${FILE_CORE} ${FILE_DATA} ${FILE_EXECUTE} ${FILE_JSON} ${FILE_REGIS} ${FILE_UTILS})


# functors
include_directories(functors)
file(GLOB_RECURSE SOURCE_FUNCTORS functors/*.cpp)

# gl
include_directories(gl gl/util)
file(GLOB_RECURSE FILE_GL gl/*.cpp)
file(GLOB_RECURSE FILE_UTIL gl/util/*.cpp)
set(SOURCE_GL ${FILE_GL} ${FILE_ES} ${FILE_UTIL} )

# 全局util
include_directories(util)
file(GLOB_RECURSE FILE_GLOBAL_UTIL util/*.cpp)
set(SOURCE_UTIL ${FILE_GLOBAL_UTIL})

# jni
include_directories(jni)
file(GLOB_RECURSE FILE_JNI jni/*.cpp)
set(SOURCE_JNI ${FILE_JNI})

# android log
find_library(log-lib log)




add_library(framework SHARED
        ${SOURCE_FRAMEWORK}
        ${SOURCE_FUNCTORS}
        ${SOURCE_GL}
        ${SOURCE_JNI}
        ${SOURCE_UTIL}
)

target_link_libraries(framework android EGL GLESv3 jnigraphics ${log-lib})
