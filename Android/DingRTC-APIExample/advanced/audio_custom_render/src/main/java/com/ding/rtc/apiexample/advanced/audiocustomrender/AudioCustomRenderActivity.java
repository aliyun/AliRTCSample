package com.ding.rtc.apiexample.advanced.audiocustomrender;

import android.content.Context;
import android.content.Intent;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class AudioCustomRenderActivity extends AppCompatActivity {
    public static final String TAG = AudioCustomRenderActivity.class.getSimpleName();
    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";
    private static final int AUDIO_FREQUENCY = 48000;
    private static final int PLAY_CHANNEL_CONFIG = AudioFormat.CHANNEL_OUT_MONO;
    private static final int AUDIO_ENCODING = AudioFormat.ENCODING_PCM_16BIT;
    private static final int PLAY_AUDIO_BUFFER_TIMES = 1;

    private DingRtcEngine mRtcEngine;
    private DingRtcEngine.DingRtcAudioFrame mAudioFrame = new DingRtcEngine.DingRtcAudioFrame();
    private Handler mainHandler = new Handler(Looper.getMainLooper());
    private volatile WindState state = WindState.IDLE;
    private OnState onStateListener;
    private String mChannelId;
    private String mUserId;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, AudioCustomRenderActivity.class);
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
        setContentView(R.layout.activity_audio_custom_render);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.custom_audio_render);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }
        initDingEngine();
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.enableCustomAudioRender(true);
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
                Toast.makeText(AudioCustomRenderActivity.this, message, Toast.LENGTH_LONG).show();
            }
        });
    }

    private int enableCustomAudioRender() {
        onStateListener = new OnState() {
            @Override
            public void onStateChanged(WindState currentState) {
                showToast("current state : " + currentState.toString());
            }
        };
        joinChannel();
        startPlayPCM();
        return 0;
    }

    private int disableCustomAudioRender() {
        stopPlayPCM();
        leaveChannel();
        return 0;
    }

    public synchronized boolean isIdle() {
        return WindState.IDLE.equals(state);
    }

    private synchronized void startPlayPCM() {
        if (!isIdle()) {
            return;
        }
        new AudioTrackPlayThread().start();
    }

    public synchronized void stopPlayPCM() {
        if (!state.equals(WindState.PLAYING)) {
            return;
        }
        state = WindState.STOP_PLAY;
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
            disableCustomAudioRender();
            mRtcEngine.leaveChannel();
            mRtcEngine.destroy();
            mRtcEngine = null;
        }
    }

    /**
     * AudioTrack Play Thread
     */
    private class AudioTrackPlayThread extends Thread {
        AudioTrack track;

        AudioTrackPlayThread() {
            setPriority(Thread.MAX_PRIORITY);
            int bufferSize = AudioTrack.getMinBufferSize(AUDIO_FREQUENCY,
                    PLAY_CHANNEL_CONFIG, AUDIO_ENCODING) * PLAY_AUDIO_BUFFER_TIMES;
            track = new AudioTrack(AudioManager.STREAM_MUSIC,
                    AUDIO_FREQUENCY,
                    PLAY_CHANNEL_CONFIG, AUDIO_ENCODING, bufferSize,
                    AudioTrack.MODE_STREAM);
            mAudioFrame.samplesPerSec = 48000;
            mAudioFrame.numChannels = 1;
            mAudioFrame.numSamples = 480;
            mAudioFrame.bytesPerSample = 2;
            mAudioFrame.data = ByteBuffer.allocateDirect(bufferSize);
            mAudioFrame.data.order(ByteOrder.nativeOrder());
        }

        @Override
        public void run() {
            super.run();
            state = WindState.PLAYING;
            notifyState(state);
            try {
                track.play();
                while (state.equals(WindState.PLAYING) && mRtcEngine != null) {
                    int result = mRtcEngine.getCustomAudioRenderFrame(mAudioFrame);
                    if (result == 0) {
                        track.write(mAudioFrame.data, 480*2, AudioTrack.WRITE_BLOCKING);
                        mAudioFrame.data.clear();
                    }
                }
                track.stop();
                track.release();
            } catch (Exception e) {
                Log.e(TAG, "AudioTrackPlayThread:", e);
                notifyState(WindState.ERROR);
            }
            state = WindState.STOP_PLAY;
            notifyState(state);
            state = WindState.IDLE;
            notifyState(state);
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

    public void onClick(View view){
        if(isIdle()) {
            ((TextView)view).setText(R.string.custom_audio_render_stop);
            enableCustomAudioRender();
        } else {
            ((TextView)view).setText(R.string.custom_audio_render_start);
            disableCustomAudioRender();
        }
    }

    /**
     * 表示当前状态
     */
    public enum WindState {
        ERROR,
        IDLE,
        PLAYING,
        STOP_PLAY
    }
}
