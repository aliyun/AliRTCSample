package com.ding.rtc.apiexample.advanced.audiovolumeindication;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.MenuItem;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

import java.util.List;

public class AudioVolumeIndicationActivity extends AppCompatActivity {
    public static final String TAG = AudioVolumeIndicationActivity.class.getSimpleName();
    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;

    private SoundWaveView mSoundWaveView;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, AudioVolumeIndicationActivity.class);
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
        setContentView(R.layout.activity_audio_volume_indication);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.audio_volume_indication);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }
        mSoundWaveView = findViewById(R.id.soundWaveView);
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
            // start audio volume indication by calling enableAudioVolumeIndication
            mRtcEngine.enableAudioVolumeIndication(400,3,1);
            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
                @Override
                /**
                 * receiving audio volume info
                 */
                public void onAudioVolumeIndication(List<DingRtcEngine.DingRtcAudioVolumeInfo> speakers) {
                    super.onAudioVolumeIndication(speakers);
                    if(speakers.size()>0) {
                        int amplitude = speakers.get(0).volume;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mSoundWaveView.addAmplitude(amplitude);
                            }
                        });
                    }
                }

                @Override
                public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {
                    super.onJoinChannelResult(result, channel, userId, elapsed);
                    if(result == 0) {
                        showToast("join success.");
                    } else {
                        showToast("join failure.");
                    }
                }
            });
        }
    }

    private void showToast(String message) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(AudioVolumeIndicationActivity.this, message, Toast.LENGTH_LONG).show();
            }
        });
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
}
