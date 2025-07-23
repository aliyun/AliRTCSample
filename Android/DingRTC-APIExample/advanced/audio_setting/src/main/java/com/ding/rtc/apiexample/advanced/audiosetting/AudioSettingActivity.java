package com.ding.rtc.apiexample.advanced.audiosetting;

import static com.ding.rtc.DingRtcEngine.DingRtcAudioProfile.DingRtcEngineBasicQualityMode;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioProfile.DingRtcEngineHighQualityMode;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioProfile.DingRtcEngineStereoHighQualityMode;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioProfile.DingRtcEngineStereoSuperHighQualityMode;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioProfile.DingRtcEngineSuperHighQualityMode;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioScenario.DingRtcSceneDefaultMode;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioScenario.DingRtcSceneMusicMode;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;

public class AudioSettingActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;
    private boolean needPublishAudio = true;

    private Spinner mAudioModeSpinner;
    private Spinner mAudioSceneSpinner;
    private SeekBar mVolumnSeekBar;
    private TextView mVolumnValue;
    private Button mOperationButton;
    // set audio quality mode
    private DingRtcEngine.DingRtcAudioProfile mAudioProfile = DingRtcEngineBasicQualityMode;
    // set audio scenario
    private DingRtcEngine.DingRtcAudioScenario mAudioScenario = DingRtcSceneDefaultMode;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, AudioSettingActivity.class);
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
        setContentView(R.layout.activity_audio_setting);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.audio_setting);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        mAudioModeSpinner = findViewById(R.id.spinner_mode);
        mAudioSceneSpinner = findViewById(R.id.spinner_scene);
        mVolumnSeekBar = findViewById(R.id.seekBar_volumn);
        mVolumnValue = findViewById(R.id.tv_volumn_value);
        mOperationButton = findViewById(R.id.btn_operation);

        ArrayAdapter<CharSequence> resAdapter = ArrayAdapter.createFromResource(this,
                R.array.mode_options, android.R.layout.simple_spinner_item);
        resAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mAudioModeSpinner.setAdapter(resAdapter);
        mAudioModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int index, long l) {
                switch (index) {
                    case 0:
                        mAudioProfile = DingRtcEngineBasicQualityMode;
                        break;
                    case 1:
                        mAudioProfile = DingRtcEngineHighQualityMode;
                        break;
                    case 2:
                        mAudioProfile = DingRtcEngineSuperHighQualityMode;
                        break;
                    case 3:
                        mAudioProfile = DingRtcEngineStereoHighQualityMode;
                        break;
                    case 4:
                        mAudioProfile = DingRtcEngineStereoSuperHighQualityMode;
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        ArrayAdapter<CharSequence> fpsAdapter = ArrayAdapter.createFromResource(this,
                R.array.scene_options, android.R.layout.simple_spinner_item);
        mAudioSceneSpinner.setAdapter(fpsAdapter);
        mAudioSceneSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int index, long l) {
                switch (index) {
                    case 0:
                        mAudioScenario = DingRtcSceneDefaultMode;
                        break;
                    case 1:
                        mAudioScenario = DingRtcSceneMusicMode;
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        mVolumnSeekBar.setMax(100);
        mVolumnSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                mVolumnValue.setText("" + progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
        mVolumnSeekBar.setProgress(100);

        initDingEngine();
        joinChannel();
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");

            mRtcEngine.subscribeAllRemoteAudioStreams(true);
            mRtcEngine.subscribeAllRemoteVideoStreams(false);
            mRtcEngine.publishLocalVideoStream(false);
            mRtcEngine.publishLocalAudioStream(false);
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

    public int getCurrentVolumn() {
        return mVolumnSeekBar.getProgress();
    }

    public void onPublishButtonClick(View view) {
        if (needPublishAudio) {
            // set audio profile before publishing local audio stream.
            mRtcEngine.setAudioProfile(mAudioProfile, mAudioScenario);
            // set recording signal volume during publishing local audio stream.
            mRtcEngine.setRecordingSignalVolume(getCurrentVolumn());
            mRtcEngine.publishLocalAudioStream(true);

            needPublishAudio = false;
            mOperationButton.setText(R.string.audio_setting_unpublish);
        } else {
            mRtcEngine.publishLocalAudioStream(false);
            needPublishAudio = true;
            mOperationButton.setText(R.string.audio_setting_publish);
        }
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
}