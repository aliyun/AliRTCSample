package com.ding.rtc.demo;

import android.app.Application;
import android.content.Context;

import androidx.multidex.MultiDex;

import com.ding.rtc.demo.utils.ConfigManager;

public class AliRtcDemoApplication extends Application {

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        MultiDex.install(this);
    }
    
    @Override
    public void onCreate() {
        super.onCreate();
        // 初始化配置管理器，从 assets 目录读取配置文件
        ConfigManager.getInstance().init(this);
    }
}
