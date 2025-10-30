package com.ding.rtc.apiexample.advanced.audiomixing;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.MenuItem;
import android.view.View;
import android.widget.SeekBar;
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

public class AudioMixingActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";
    private static final int MIXING_ID = 1;

    private DingRtcEngine mRtcEngine;
    private ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private String mChannelId;
    private String mUserId;

    private SeekBar mProgressSeekBar;
    private TextView mProgressValue;
    private DingRtcEngine.DingRtcAudioMixingConfig mAudioMixingConfig = new DingRtcEngine.DingRtcAudioMixingConfig();

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, AudioMixingActivity.class);
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
        setContentView(R.layout.activity_audio_mixing);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.audio_mixing);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        mProgressSeekBar = findViewById(R.id.seekBar_progress);
        mProgressValue = findViewById(R.id.tv_progress_value);

        mProgressSeekBar.setMax(100);
        mProgressSeekBar.setProgress(0);
        mProgressSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mProgressValue.setText("" + progress + "%");
                    }
                });
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });


        initDingEngine();

        mExecutor.execute(new Runnable() {
            @Override
            public void run() {
                // Prepare audio mixing file
                String fileName = "test_audio_mixing.mp3";
                File file = new File(getFilesDir().getAbsolutePath(), fileName);
                String filePath = file.getPath();
                if (!file.exists() || !file.isFile()) {
                    copyBigDataToSD(fileName, filePath, getApplicationContext());
                }

                if (file.exists()) {
                    mRtcEngine.createAudioMixing(MIXING_ID, file.getAbsolutePath());
                } else {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(AudioMixingActivity.this, R.string.audio_mixing_file_not_exist, Toast.LENGTH_LONG).show();
                        }
                    });
                }
            }
        });

        Handler handler = new Handler(Looper.getMainLooper());
        Runnable task = new Runnable() {
            @Override
            public void run() {
                if (mRtcEngine != null && mProgressSeekBar != null) {
                    long currentPosition = mRtcEngine.getAudioMixingCurrentPosition(MIXING_ID);
                    long duration = mRtcEngine.getAudioMixingDuration(MIXING_ID);
                    if (currentPosition >= 0 && duration >= 0) {
                        int progress = (int) (100 * currentPosition / duration);
                        mProgressSeekBar.setProgress(progress);
                    }
                }
                handler.postDelayed(this, 1000);
            }
        };
        handler.post(task);

        joinChannel();
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
                @Override
                public void onAudioMixingStateChanged(DingRtcEngine.DingRtcAudioMixingStatusConfig status) {
                    super.onAudioMixingStateChanged(status);
                }


            });
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

    public void onStartButtonClick(View view) {
        if (mRtcEngine != null) {
            mRtcEngine.startAudioMixing(MIXING_ID, mAudioMixingConfig);
        }
    }

    public void onPauseButtonClick(View view) {
        if (mRtcEngine != null) {
            mRtcEngine.pauseAudioMixing(MIXING_ID);
        }
    }

    public void onResumeButtonClick(View view) {
        if (mRtcEngine != null) {
            mRtcEngine.resumeAudioMixing(MIXING_ID);
        }
    }

    public void onStopButtonClick(View view) {
        if (mRtcEngine != null) {
            mRtcEngine.stopAudioMixing(MIXING_ID);
        }
    }
}