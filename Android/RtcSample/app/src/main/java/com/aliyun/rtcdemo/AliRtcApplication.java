package com.aliyun.rtcdemo;

import android.app.Application;

/**
 * 程序入口
 */
public class AliRtcApplication extends Application {

    private static AliRtcApplication sInstance;


    public static AliRtcApplication getInstance(){
        return sInstance;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        sInstance = this;
    }
}
