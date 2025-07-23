package com.ding.rtc.apiexample.advanced.rawaudiodata;

import static com.ding.rtc.DingRtcEngine.DingRtcAudioObservePosition.RtcEngineAudioPositionCaptured;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class RawAudioDataActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private String mChannelId;
    private String mUserId;
    private int mAudioPosition = RtcEngineAudioPositionCaptured;
    private DingRtcEngine.DingRtcAudioFrameObserverConfig mAudioFrameObserverConfig = new DingRtcEngine.DingRtcAudioFrameObserverConfig();
    private BufferedOutputStream mPcmBos;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, RawAudioDataActivity.class);
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
        setContentView(R.layout.activity_raw_audio_data);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.external_audio_source);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        initDingEngine();
        joinChannel();
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.publishLocalAudioStream(true); // default : not publish audio
            mRtcEngine.publishLocalVideoStream(false);
            mRtcEngine.subscribeAllRemoteVideoStreams(false);
            mRtcEngine.subscribeAllRemoteAudioStreams(true);
            enableDumpAudio();
        }
    }

    //开启音频数据dump
    private int enableDumpAudio() {
        String pcmFileName = "dingrtc_raw_audio_data.pcm";
        File pcmFile = new File(getFilesDir().getAbsolutePath(), pcmFileName);

        try {
            mPcmBos = new BufferedOutputStream(new FileOutputStream(pcmFile, true));
        } catch (FileNotFoundException e) {
            showToast("open yuv file failed" + e.getMessage());
            return -1;
        }
        mAudioFrameObserverConfig.channels = DingRtcEngine.DingRtcAudioNumChannel.DingRtcStereoAudio;
        mAudioFrameObserverConfig.sampleRate = DingRtcEngine.DingRtcAudioSampleRate.DingRtcAudioSampleRate_44100;
        mRtcEngine.enableAudioFrameObserver(true, mAudioPosition, mAudioFrameObserverConfig);
        mRtcEngine.registerAudioFrameObserver(new DingRtcEngine.DingRtcAudioFrameObserver() {
            @Override
            public void onCapturedAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
                writeToPcmFile(frame);
            }

            @Override
            public void onProcessCapturedAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {

            }

            @Override
            public void onPublishAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {

            }

            @Override
            public void onPlaybackAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {

            }

            @Override
            public void onRemoteUserAudioFrame(String uid, DingRtcEngine.DingRtcAudioFrame frame) {

            }
        });
        return 0;
    }

    // disable raw audio data
    private int disableDumpAudio() {
        mRtcEngine.enableAudioFrameObserver(false, RtcEngineAudioPositionCaptured);
        mRtcEngine.registerAudioFrameObserver(null);
        try {
            if (mPcmBos != null) {
                mPcmBos.close();
                mPcmBos = null;
            }
        } catch (IOException e) {
            showToast("close output stream error. " + e.getMessage());
            return -1;
        }
        return 0;
    }

    private int writeToPcmFile(DingRtcEngine.DingRtcAudioFrame audioFrame) {
        int totalBytes = audioFrame.numChannels * audioFrame.numSamples * audioFrame.bytesPerSample;
        byte[] buffer = new byte[totalBytes];
        audioFrame.data.get(buffer);
        try {
            if (mPcmBos != null) {
                mPcmBos.write(buffer, 0, totalBytes);
                mPcmBos.flush();
            }
        } catch (IOException e) {
            showToast("close output stream error. " + e.getMessage());
            return -1;
        }
        return 0;
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
            disableDumpAudio();
            mRtcEngine.leaveChannel();
            mRtcEngine.destroy();
            mRtcEngine = null;
        }
    }

    private void showToast(String message) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(RawAudioDataActivity.this, message, Toast.LENGTH_LONG).show();
            }
        });
    }
}