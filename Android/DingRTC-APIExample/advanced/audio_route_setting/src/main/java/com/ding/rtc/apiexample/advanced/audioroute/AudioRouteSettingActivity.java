package com.ding.rtc.apiexample.advanced.audioroute;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.MenuItem;
import android.view.View;
import android.widget.RadioGroup;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

import java.util.ArrayList;
import java.util.List;

public class AudioRouteSettingActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;

    private RadioGroup mRadioGroup;
    private List<DeviceItem> mAudioDeviceList = new ArrayList<>();

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, AudioRouteSettingActivity.class);
        intent.putExtra(INTENT_APP_ID, appId);
        intent.putExtra(INTENT_CHANNEL_ID, channelId);
        intent.putExtra(INTENT_USER_ID, userId);
        intent.putExtra(INTENT_TOKEN, token);
        intent.putExtra(INTENT_USER_NAME, userName);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio_route);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.audio_route_setting);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }
        mRadioGroup = findViewById(R.id.radioGroup);


        initDingEngine();
        joinChannel();

        Handler handler = new Handler(Looper.getMainLooper());
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                List<DingRtcEngine.DingRtcAudioDeviceInfo> audioDeviceInfos = mRtcEngine.getAudioDevices();
                for (DingRtcEngine.DingRtcAudioDeviceInfo info : audioDeviceInfos) {
                    mAudioDeviceList.add(new DeviceItem(info.name, info.type, info.isUsed));
                }

                for (int i = 0; i < mAudioDeviceList.size(); i++) {
                    DeviceItem item = mAudioDeviceList.get(i);
                    View itemView = getLayoutInflater().inflate(R.layout.item_device_radio, mRadioGroup, false);
                    RadioButton radioButton = itemView.findViewById(R.id.radioButton);
                    TextView textDeviceName = itemView.findViewById(R.id.textDeviceName);
                    TextView textRouteType = itemView.findViewById(R.id.textRouteType);

                    textDeviceName.setText(item.deviceName);
                    textRouteType.setText(item.routeType.name());
                    radioButton.setChecked(item.isDefault);

                    itemView.setOnClickListener(v -> {
                        for (int j = 0; j < mRadioGroup.getChildCount(); j++) {
                            View child = mRadioGroup.getChildAt(j);
                            RadioButton rb = child.findViewById(R.id.radioButton);
                            if (rb != null) {
                                rb.setChecked(false);
                            }
                        }
                        radioButton.setChecked(true);
                        mRtcEngine.setAudioRouteType(item.routeType);
                    });

                    mRadioGroup.addView(itemView);

                }
            }
        }, 1000);
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
                @Override
                public void onAudioRouteChanged(DingRtcEngine.DingRtcAudioRouteType routing) {
                    super.onAudioRouteChanged(routing);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(AudioRouteSettingActivity.this, getString(R.string.audio_route_setting_change_device_type) + routing.name(), Toast.LENGTH_SHORT).show();
                        }
                    });

                }
            });
            mRtcEngine.subscribeAllRemoteAudioStreams(true);
            mRtcEngine.subscribeAllRemoteVideoStreams(false);
            mRtcEngine.publishLocalVideoStream(false);
            mRtcEngine.publishLocalAudioStream(true);
        }
    }

    private boolean joinChannel() {
        DingRtcAuthInfo authInfo = new DingRtcAuthInfo();
        Intent intent = getIntent();
        mChannelId = intent.getStringExtra(INTENT_CHANNEL_ID);
        authInfo.channelId = mChannelId;
        mUserId = intent.getStringExtra(INTENT_USER_ID);
        authInfo.userId = mUserId;
        authInfo.token = intent.getStringExtra(INTENT_TOKEN);
        authInfo.appId = intent.getStringExtra(INTENT_APP_ID);
        String userName = intent.getStringExtra(INTENT_USER_NAME);
        int ret = mRtcEngine.joinChannel(authInfo, userName);
        if (ret != 0) {
            return false;
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onStop() {
        super.onStop();
        if (isFinishing()) {
            mRtcEngine.leaveChannel();
            mRtcEngine.destroy();
            mRtcEngine = null;
        }
    }

    static class DeviceItem {
        String deviceName;
        DingRtcEngine.DingRtcAudioRouteType routeType;
        boolean isDefault;

        DeviceItem(String deviceName, DingRtcEngine.DingRtcAudioRouteType routeType, boolean isDefault) {
            this.deviceName = deviceName;
            this.routeType = routeType;
            this.isDefault = isDefault;
        }
    }
}