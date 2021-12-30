cmake_minimum_required(VERSION 3.10.2)
project("eglDemo")

include_directories(gles jni util)

file(GLOB_RECURSE  GLES_SOURCE gles/*.cpp)
file(GLOB_RECURSE  JNI_SOURCE jni/*.cpp)
file(GLOB_RECURSE  UTIL_SOURCE util/*.cpp)

add_library(eglDemo SHARED  ${GLES_SOURCE} ${JNI_SOURCE} ${UTIL_SOURCE})


find_library(log-lib log)

target_link_libraries(eglDemo ${log-lib} android EGL GLESv3 jnigraphics)
