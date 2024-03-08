package com.ding.rtc.demo.basicvideocall;

import android.app.Application;
import android.content.Context;

import androidx.multidex.MultiDex;

import com.ding.rtc.DingRtcEngine;

public class DingApplication extends Application {

    public static final String APP_ID = ; // 应用ID
    public static final String TOKEN = ; // 频道鉴权令牌Token
    public static final String NONCE = ; // 随机码
    public static final long TIMESTAMP  = ; // 时间戳
    public static final String[] GSLB_SERVER = ; // GSLB地址
    private DingRtcEngine mRtcEngine;
    private final DingEngineCallback mRtcCallback = new DingEngineCallback();

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        MultiDex.install(this);
    }

    @Override
    public void onCreate() {
        super.onCreate();

        initDingEngine();
    }

    public DingRtcEngine getDingEngine() {
        if (mRtcEngine == null) {
            initDingEngine();
        }
        return mRtcEngine;
    }

    public void registerEventHandler(EventHandler handler) { mRtcCallback.addHandler(handler); }
    public void removeEventHandler(EventHandler handler) { mRtcCallback.removeHandler(handler); }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.setDefaultSubscribeAllRemoteAudioStreams(true);
            mRtcEngine.setDefaultSubscribeAllRemoteVideoStreams(false);
            mRtcEngine.setRemoteDefaultVideoStreamType(DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeFHD);
            mRtcEngine.setRtcEngineEventListener(mRtcCallback);
        }
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        mRtcEngine.destroy();
    }
}
