package com.example.util;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import androidx.annotation.NonNull;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * 独立线程同步Handler，内部会创建一个线程来处理
 * sendMsg可以传入参数是否同步处理，同步处理会在独立线程处理后再返回
 *
 * 外部使用需要继承此类，重写handleMsg方法
 */
public abstract class SyncThreadHandler {
    private final Lock mLock;
    private final Condition mCondition;
    private final HandlerThread mHandlerThread;
    private final Handler mInnerHandler;

    private final Logger mLogger = Logger.create("SyncThreadHandler");

    public SyncThreadHandler(String tag) {
        mLock = new ReentrantLock();
        mCondition = mLock.newCondition();
        mHandlerThread = new HandlerThread(tag);
        mHandlerThread.start();
        mInnerHandler = new InnerHandler(mHandlerThread.getLooper());
    }

    public void sendSyncEmptyMsg(int what){
        Message message = Message.obtain();
        message.what = what;
        sendMsg(message,true);
    }

    /**
     * 发送一个消息被执行
     * @param msg 消息
     * @param isSync 是否同步
     * @return 是否插入成功
     */
    public boolean sendMsg(Message msg,boolean isSync){
        if (!mHandlerThread.isAlive()) {
            mLogger.e("SyncThreadHandler thread is quit,can not send msg");
            return false;
        }
        boolean ret;
        // 如果looper已经退出，那么需要直接返回
        if (isSync) {
            msg.arg1 = 1;
            try {
                mLock.lock();
                ret = mInnerHandler.sendMessage(msg);
                if (!ret) {
                    mLogger.d("insert msg fail");
                    return false;
                }
                mCondition.await();
            }catch (InterruptedException e){
                e.printStackTrace();
                // 线程被interrupt，表示任务没有被正常执行完成，返回false
                ret = false;
                mInnerHandler.removeMessages(msg.what);
            }finally {
                mLock.unlock();
            }
        } else {
            ret = mInnerHandler.sendMessage(msg);
        }
        return ret;
    }

    public void release() {
        mLock.lock();
        try {
            mCondition.signalAll();
        }finally {
            mLock.unlock();
        }
        mHandlerThread.quitSafely();
        mInnerHandler.removeCallbacksAndMessages(null);
    }

    private class InnerHandler extends Handler{
        public InnerHandler(@NonNull Looper looper) {
            super(looper);
        }
        @Override
        public void handleMessage(@NonNull Message msg) {
            //LogUtil.log("process what = "+msg.what);
            SyncThreadHandler.this.handleMsg(msg);
            if (msg.arg1 == 1) {
                try {
                    mLock.lock();
                    mCondition.signal();
                }finally {
                    mLock.unlock();
                }
            }
        }
    }

    public abstract void handleMsg(Message msg);

}
