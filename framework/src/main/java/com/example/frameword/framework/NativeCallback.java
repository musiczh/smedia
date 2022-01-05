package com.example.frameword.framework;

public abstract class NativeCallback {

    public NativeCallback(){}

    /**
     * 底层回调此方法，如果需要处理耗时逻辑，需要把处理放到单独线程中。
     * 外部可以定义一个callBack对象供给不同的functor、同个functor不同的回调
     * @param nodeName 回调的callbackFunctor名称，
     * @param keyName 设置callback时候的key
     * @param value 回调的数据
     * @return 布尔值，保留字段，暂无含义
     */
    public abstract boolean onNativeCallback(String nodeName,String keyName,Object value);


}
