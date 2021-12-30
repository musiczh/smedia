package com.example.frameword.framework;

public class ClassLoaderLib {

    static ClassLoader onNativeGetClassLoader() {
        return ClassLoaderLib.class.getClassLoader();
    }
}
