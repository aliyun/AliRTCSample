package com.ding.rtc.api.example;

import android.app.Application;
import android.content.Context;

import androidx.multidex.MultiDex;

public class DingRTCApplication extends Application {

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        MultiDex.install(this);
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    public void onTerminate() {
        super.onTerminate();

    }
}
