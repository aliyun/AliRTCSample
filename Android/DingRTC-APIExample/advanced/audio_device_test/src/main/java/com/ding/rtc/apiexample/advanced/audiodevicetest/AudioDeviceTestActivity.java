package com.ding.rtc.apiexample.advanced.audiodevicetest;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class AudioDeviceTestActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";
    private static final String AUDIO_PLAYOUT_TEST_FILE_NAME = "test_audio_playout.wav";

    private DingRtcEngine mRtcEngine;
    private ExecutorService mExecutor = Executors.newSingleThreadExecutor();

    private String mChannelId;
    private String mUserId;
    private File mAudioPlayoutTestFile;

    private Button mRecordTestButton;
    private Button mPlayoutTestButton;
    private TextView mRecordVolumeTextView;
    private TextView mPlayoutVolumeTextView;

    private boolean isAudioRecording = false;
    private boolean isAudioPlayout = false;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, AudioDeviceTestActivity.class);
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
        setContentView(R.layout.activity_audio_device_test);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.audio_device_test);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        mRecordTestButton = findViewById(R.id.btn_audio_record_test_operation);
        mPlayoutTestButton = findViewById(R.id.btn_audio_playout_test_operation);
        mRecordVolumeTextView = findViewById(R.id.tv_record_volume_value);
        mPlayoutVolumeTextView = findViewById(R.id.tv_playout_volume_value);

        initDingEngine();

        mExecutor.execute(new Runnable() {
            @Override
            public void run() {
                // Prepare audio playout test file
                mAudioPlayoutTestFile = new File(getFilesDir().getAbsolutePath(), AUDIO_PLAYOUT_TEST_FILE_NAME);
                String filePath = mAudioPlayoutTestFile.getPath();
                if (!mAudioPlayoutTestFile.exists() || !mAudioPlayoutTestFile.isFile()) {
                    copyBigDataToSD(AUDIO_PLAYOUT_TEST_FILE_NAME, filePath, getApplicationContext());
                }

                if (!mAudioPlayoutTestFile.exists()) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(AudioDeviceTestActivity.this, R.string.audio_playout_test_file_not_exist, Toast.LENGTH_LONG).show();
                        }
                    });
                }
            }
        });

        joinChannel();
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");

            mRtcEngine.subscribeAllRemoteAudioStreams(true);
            mRtcEngine.subscribeAllRemoteVideoStreams(false);
            mRtcEngine.publishLocalVideoStream(false);
            mRtcEngine.publishLocalAudioStream(true);

            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
                @Override
                public void onRecordingDeviceAudioLevel(int level) {
                    super.onRecordingDeviceAudioLevel(level);
                    mRecordVolumeTextView.setText("" + level);
                }

                @Override
                public void onPlayoutDeviceAudioLevel(int level) {
                    super.onPlayoutDeviceAudioLevel(level);
                    mPlayoutVolumeTextView.setText("" + level);
                }
            });
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

    public void onRecordTestButtonClick(View view) {
        if (!isAudioRecording) {
            // Important: start audio recording device test
            mRtcEngine.startRecordingDeviceTest();
            isAudioRecording = true;
            mRecordTestButton.setText(R.string.audio_device_stop_record_test);
        } else {
            mRtcEngine.stopRecordingDeviceTest();
            isAudioRecording = false;
            mRecordTestButton.setText(R.string.audio_device_start_record_test);
        }
    }

    public void onPlayoutTestButtonClick(View view) {
        if (!isAudioPlayout) {
            // Important: start audio playout device test
            if (mAudioPlayoutTestFile != null && mAudioPlayoutTestFile.exists())
                mRtcEngine.startPlayoutDeviceTest(mAudioPlayoutTestFile.getAbsolutePath());
            isAudioPlayout = true;
            mPlayoutTestButton.setText(R.string.audio_device_stop_playout_test);
        } else {
            mRtcEngine.stopPlayoutDeviceTest();
            isAudioPlayout = false;
            mPlayoutTestButton.setText(R.string.audio_device_start_playout_test);
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

    private static void copyBigDataToSD(String fileName, String strOutFileName, Context context) {
        try {
            InputStream myInput;
            OutputStream myOutput = new FileOutputStream(strOutFileName);
            myInput = context.getAssets().open(fileName);
            byte[] buffer = new byte[1024];
            int length = myInput.read(buffer);
            while (length > 0) {
                myOutput.write(buffer, 0, length);
                length = myInput.read(buffer);
            }

            myOutput.flush();
            myInput.close();
            myOutput.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}