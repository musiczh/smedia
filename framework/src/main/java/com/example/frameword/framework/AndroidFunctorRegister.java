package com.example.frameword.framework;

import com.example.util.Logger;

import java.util.HashMap;
import java.util.Map;

public class AndroidFunctorRegister {
    static Map<String ,String> mFunctorMap;
    static {
        mFunctorMap = new HashMap<>();
        mFunctorMap.put("GLViewFunctor","com.example.frameword.framework.GLViewFunctor");
        mFunctorMap.put("CameraFunctor","com.example.frameword.framework.CameraFunctor");
    }

    static Object onNativeCreateAndroidFunctor(String name) {
        if (!mFunctorMap.containsKey(name)) {
            Logger.E("AndroidFunctorRegister","create android functor fail;no such name.");
            return null;
        }
        try {
            Class cls = Class.forName(mFunctorMap.get(name));
            return cls.newInstance();
        } catch (ClassNotFoundException | IllegalAccessException | InstantiationException e) {
            Logger.D("AndroidFunctorRegister","create android functor: "+name+" fail");
            e.printStackTrace();
        }
        return null;
    }
}
