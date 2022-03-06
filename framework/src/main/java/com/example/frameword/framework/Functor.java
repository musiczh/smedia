package com.example.frameword.framework;

public abstract class Functor extends FunctorContext{

    public abstract boolean onInitialize();
    public abstract void onUnInitialize();
    public abstract boolean onExecute();
    public abstract void onSetOption(String key,Object value);

    boolean onNativeInitialize(long nativeFunctorContextHandle,String name) {
        onInit(nativeFunctorContextHandle,name);
        onInitialize();
        return true;
    }

    void onNativeUnInitialize(long nativeFunctorContextHandle) {
        onUnInitialize();
    }

    void onNativeSetOption(long nativeFunctorContextHandle,String key,Object value) {
        onSetOption(key, value);
    }

    boolean onNativeExecute(long nativeFunctorContextHandle) {
        return onExecute();
    }
}
