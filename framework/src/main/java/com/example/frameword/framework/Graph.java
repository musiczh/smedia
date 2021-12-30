package com.example.frameword.framework;

import com.example.util.Logger;

import java.util.HashMap;

public class Graph {
    private long mNativeGraphHandle;
    private Logger mLogger = Logger.create("Graph_java");

    public Graph(){
    }

    public void init(String json, HashMap<String,Object> options){
        if (options == null || options.isEmpty()) {
            mNativeGraphHandle = initNativeGraph(json,new String[]{},new Object[]{},0);
            return;
        }
        String[] keys = options.keySet().toArray(new String[0]);
        Object[] values = options.values().toArray(new Object[0]);
        mNativeGraphHandle = initNativeGraph(json,keys,values,options.size());
    }


    public void run() {
        runNativeGraph(mNativeGraphHandle);
    }

    public void setOption(String nodeName,String key,Object value){
        setOptionNative(mNativeGraphHandle,nodeName,key,value);
    }

    public void release() {
        stopNativeGraph(mNativeGraphHandle);
        releaseNativeGraph(mNativeGraphHandle);
    }

    private native long initNativeGraph(String json,String[] keys,Object[] values,int length);
    private native void runNativeGraph(long nativeGraphHandle);
    private native void setOptionNative(long nativeGraphHandle,String nodeName,String key,Object value);
    private native void releaseNativeGraph(long nativeGraphHandle);
    private native void stopNativeGraph(long nativeGraphHandle);

    static {
        System.loadLibrary("framework");
    }
}
