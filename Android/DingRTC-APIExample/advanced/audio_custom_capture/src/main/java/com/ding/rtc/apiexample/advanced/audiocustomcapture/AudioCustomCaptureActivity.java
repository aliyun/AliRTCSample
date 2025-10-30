package com.ding.rtc.apiexample.advanced.audiocustomcapture;

import android.content.Context;
import android.content.Intent;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class AudioCustomCaptureActivity extends AppCompatActivity {
    public static final String TAG = AudioCustomCaptureActivity.class.getSimpleName();
    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";
    private static final int AUDIO_FREQUENCY = 48000;
    private static final int AUDIO_CHANNEL_NUM = 1;
    private static final int AUDIO_BYTES_PER_SAMPLE = 2;
    private static final int AUDIO_SAMPLES_NUMBER_IN_10MS = 480;
    private static final int AUDIO_SAMPLES_BYTES_NUMBER_IN_10MS = 960;
    private static final int AUDIO_ENCODING = AudioFormat.ENCODING_PCM_16BIT;
    private static final int RECORD_CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO;
    private static final int RECORD_AUDIO_BUFFER_TIMES = 1;

    private DingRtcEngine mRtcEngine;
    private DingRtcEngine.DingRtcAudioFrame mAudioFrame = new DingRtcEngine.DingRtcAudioFrame();
    private Handler mainHandler = new Handler(Looper.getMainLooper());
    private volatile WindState state = WindState.IDLE;
    private OnState onStateListener;
    private String mChannelId;
    private String mUserId;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, AudioCustomCaptureActivity.class);
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
        setContentView(R.layout.activity_audio_custom_capture);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.custom_audio_capture);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }
        initDingEngine();
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.enableCustomAudioCapture(true);
            mRtcEngine.publishLocalAudioStream(true); // default : not publish audio
            mRtcEngine.publishLocalVideoStream(false);
            mRtcEngine.subscribeAllRemoteVideoStreams(false);
            mRtcEngine.subscribeAllRemoteAudioStreams(true);
        }
    }

    private void showToast(String message) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(AudioCustomCaptureActivity.this, message, Toast.LENGTH_LONG).show();
            }
        });
    }

    private int enableCustomAudioCapture() {
        onStateListener = new OnState() {
            @Override
            public void onStateChanged(WindState currentState) {
                showToast("current state : " + currentState.toString());
            }
        };
        joinChannel();
        startCapturePCM();
        return 0;
    }

    private int disableCustomAudioCapture() {
        stopCapturePCM();
        leaveChannel();
        return 0;
    }

    public synchronized boolean isIdle() {
        return WindState.IDLE.equals(state);
    }

    private synchronized void startCapturePCM() {
        if (!isIdle()) {
            return;
        }
        new AudioRecordThread().start();
    }

    public synchronized void stopCapturePCM() {
        if (!state.equals(WindState.RECORDING)) {
            return;
        }
        state = WindState.STOP_RECORD;
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

    private boolean leaveChannel() {
        if (mRtcEngine != null) {
            int ret = mRtcEngine.leaveChannel();
            return ret == 0;
        }
        return false;
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
            disableCustomAudioCapture();
            mRtcEngine.leaveChannel();
            mRtcEngine.destroy();
            mRtcEngine = null;
        }
    }

    private class AudioRecordThread extends Thread {
        AudioRecord aRecord;
        int bufferSize = 10240;

        AudioRecordThread() {
            bufferSize = AudioRecord.getMinBufferSize(AUDIO_FREQUENCY,
                    RECORD_CHANNEL_CONFIG, AUDIO_ENCODING) * RECORD_AUDIO_BUFFER_TIMES;
            Log.d(TAG, "record buffer size = " + bufferSize);
            aRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, AUDIO_FREQUENCY,
                    RECORD_CHANNEL_CONFIG, AUDIO_ENCODING, bufferSize);

            mAudioFrame.samplesPerSec = AUDIO_FREQUENCY;
            mAudioFrame.numChannels = AUDIO_CHANNEL_NUM;
            mAudioFrame.bytesPerSample = AUDIO_BYTES_PER_SAMPLE;
            mAudioFrame.numSamples = AUDIO_SAMPLES_NUMBER_IN_10MS; // 10 ms audio data samples: (48000 / 1000) * 10 = 480
            mAudioFrame.data = ByteBuffer.allocateDirect(AUDIO_SAMPLES_BYTES_NUMBER_IN_10MS); // 10ms audio data size in bytes: 480 * 2 = 960
            mAudioFrame.data.order(ByteOrder.nativeOrder());
        }

        @RequiresApi(api = Build.VERSION_CODES.M)
        @Override
        public void run() {
            state = WindState.RECORDING;
            notifyState(state);
            Log.d(TAG, "record start.");
            try {
                aRecord.startRecording();
                while (state.equals(WindState.RECORDING) && !isInterrupted()) {
                    int size = aRecord.read(mAudioFrame.data, AUDIO_SAMPLES_BYTES_NUMBER_IN_10MS, AudioRecord.READ_BLOCKING);
                    if (size > 0 && mRtcEngine != null && state.equals(WindState.RECORDING) && !isInterrupted()) {
                        mRtcEngine.sendCustomAudioCaptureFrame(mAudioFrame);
                        mAudioFrame.data.rewind();
                    }
                }
                aRecord.stop();
            } catch (Exception e) {
                Log.e(TAG, "AudioRecordThread:", e);
                notifyState(WindState.ERROR);
            }
            notifyState(state);
            state = WindState.IDLE;
            notifyState(state);
            Log.d(TAG, "record over");
        }
    }


    private synchronized void notifyState(final WindState currentState) {
        if (null != onStateListener) {
            mainHandler.post(new Runnable() {
                @Override
                public void run() {
                    onStateListener.onStateChanged(currentState);
                }
            });
        }
    }

    public interface OnState {
        void onStateChanged(WindState currentState);
    }

    public void onClick(View view) {
        if (isIdle()) {
            ((TextView) view).setText(R.string.custom_audio_capture_stop);
            enableCustomAudioCapture();
        } else {
            ((TextView) view).setText(R.string.custom_audio_capture_start);
            disableCustomAudioCapture();
        }
    }

    /**
     * current status
     */
    public enum WindState {
        ERROR,
        IDLE,
        RECORDING,
        STOP_RECORD,
        PLAYING,
        STOP_PLAY
    }
}
