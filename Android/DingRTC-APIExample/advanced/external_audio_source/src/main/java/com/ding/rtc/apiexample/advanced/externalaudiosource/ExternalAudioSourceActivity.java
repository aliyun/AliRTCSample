package com.ding.rtc.apiexample.advanced.externalaudiosource;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.text.TextUtils;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ExternalAudioSourceActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private String mChannelId;
    private String mUserId;
    private boolean isPushExternalAudio = false;

    private int pcmSampleRate = 16000;
    private int pcmChannels = 1;
    private int bytesPerSample = 2;
    private int pcmDurationPerRead = 10; // Push 10ms of audio data each time

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, ExternalAudioSourceActivity.class);
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
        setContentView(R.layout.activity_external_audio_source);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.external_audio_source);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        initDingEngine();
        joinChannel();
    }

    @Override
    public void onClick(View v) {
        Button btn = (Button) v;
        if (!isPushExternalAudio) {
            btn.setText(R.string.external_audio_source_stop);
            // Important : enable external video source ,
            // This means that video frames will be passed in through the pushExternalVideoFrame interface instead of being captured by the camera..
            mRtcEngine.setExternalAudioSource(true, pcmSampleRate, pcmChannels);
            // Important : "publishLocalVideoStream(true)" : It means pushing video data to the media server.
            mRtcEngine.publishLocalAudioStream(true);
            mExecutor.execute(new Runnable() {
                @Override
                public void run() {
                    // Prepare external video source file
                    String fileName = "audio_long16.pcm";
                    File file = new File(getFilesDir().getAbsolutePath(), fileName);
                    String filePath = file.getPath();
                    if (!file.exists() || !file.isFile()) {
                        copyBigDataToSD(fileName, filePath, getApplicationContext());
                    }
                    startExteralAudioSource(filePath);
                }
            });

        } else {
            btn.setText(R.string.external_audio_source_start);
            stopExterAudioSource();
            // Important : enable external aduio source ,
            // This means that video frames will be passed in through the pushExternalVideoFrame interface instead of being captured by the camera..
            mRtcEngine.setExternalAudioSource(false, pcmSampleRate, pcmChannels);
            // Important : "publishLocalVideoStream(true)" : It means pushing video data to the media server.
            mRtcEngine.publishLocalAudioStream(false);
        }
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.publishLocalAudioStream(false); // default : not publish audio
            mRtcEngine.publishLocalVideoStream(false);
            mRtcEngine.subscribeAllRemoteVideoStreams(false);
            mRtcEngine.subscribeAllRemoteAudioStreams(true);
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

    private void stopExterAudioSource() {
        isPushExternalAudio = false;
    }

    private void startExteralAudioSource(String pcmPath) {
        if (TextUtils.isEmpty(pcmPath)) {
            // input pcm file path
            return;
        }
        File pcmDataFile = new File(pcmPath);
        if (!pcmDataFile.exists()) {
            // pcm file not exist
            return;
        }
        isPushExternalAudio = true;
        new Thread() {
            @Override
            public void run() {
                File pcmDataFile = new File(pcmPath);
                RandomAccessFile raf = null;
                try {
                    raf = new RandomAccessFile(pcmDataFile, "r");
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                    return;
                }
                try {

                    ByteBuffer byteBuffer = ByteBuffer.allocateDirect(pcmSampleRate * pcmDurationPerRead * pcmChannels * bytesPerSample / 1000);
                    final int sizeInBytes = byteBuffer.capacity();
                    while (isPushExternalAudio) {
                        long start = System.currentTimeMillis();
                        int len = raf.read(byteBuffer.array(), 0, sizeInBytes);
                        if (len == -1) {
                            raf.seek(0);
                        }
                        DingRtcEngine.DingRtcAudioFrame audioFrame = new DingRtcEngine.DingRtcAudioFrame();
                        audioFrame.data = byteBuffer;
                        audioFrame.numSamples = pcmSampleRate * pcmDurationPerRead / 1000;
                        audioFrame.bytesPerSample = bytesPerSample;
                        audioFrame.numChannels = pcmChannels;
                        audioFrame.samplesPerSec = pcmSampleRate;
                        mRtcEngine.pushExternalAudioFrame(audioFrame);
                        byteBuffer.rewind();
                        long end = System.currentTimeMillis();
                        long sleep = pcmDurationPerRead - (end - start);
                        if (sleep > 0) {
                            SystemClock.sleep(sleep);
                        }
                    }
                } catch (IOException ex) {
                    ex.printStackTrace();
                } finally {
                    try {
                        raf.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }.start();
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