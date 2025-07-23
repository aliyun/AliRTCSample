package com.ding.rtc.apiexample.basic.audiocall;

import static com.ding.rtc.DingRtcEngine.DingRtcAudioProfile.DingRtcEngineBasicQualityMode;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioScenario.DingRtcSceneDefaultMode;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

import java.util.ArrayList;
import java.util.List;

public class AudioCallActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;

    private RecyclerView mRecyclerViewChat;
    private MessageAdapter mMessageAdapter;
    private List<Message> mMessageList;

    private Handler handler = new Handler(Looper.getMainLooper());

    public static void launch(Context context, String appId, String channelId, String userId,
                              String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, AudioCallActivity.class);
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
        if (getSupportActionBar() != null) {
            getSupportActionBar().show();
            getSupportActionBar().setTitle(R.string.audio_call);
        }
        setContentView(R.layout.activity_audiocall);
        mRecyclerViewChat = findViewById(R.id.recyclerViewChat);

        mMessageList = new ArrayList<>();
        mMessageAdapter = new MessageAdapter(mMessageList);
        mRecyclerViewChat.setLayoutManager(new LinearLayoutManager(this));
        mRecyclerViewChat.setAdapter(mMessageAdapter);

        initDingEngine();
        joinChannel();
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            addMessage(new Message("", "DingRtcEngine.create();", true));
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            addMessage(new Message("", "mRtcEngine.setRtcEngineEventListener();", true));
            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
                @Override
                public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {
                    super.onJoinChannelResult(result, channel, userId, elapsed);
                    addMessage(new Message("", "onJoinChannelResult ( result = " + result + " );", true));
                }

                @Override
                public void onRemoteUserOnLineNotify(String uid, int elapsed) {
                    super.onRemoteUserOnLineNotify(uid, elapsed);
                    addMessage(new Message("", "onRemoteUserOnLineNotify ( userId = " + uid + " );", true));
                }

                @Override
                public void onRemoteUserOffLineNotify(String uid, DingRtcEngine.DingRtcUserOfflineReason reason) {
                    super.onRemoteUserOffLineNotify(uid, reason);
                    addMessage(new Message("", "onRemoteUserOffLineNotify ( userId = " + uid + " );", true));
                }

                @Override
                public void onAudioPublishStateChanged(DingRtcEngine.DingRtcPublishState oldState, DingRtcEngine.DingRtcPublishState newState, int elapseSinceLastState, String channel) {
                    super.onAudioPublishStateChanged(oldState, newState, elapseSinceLastState, channel);
                    addMessage(new Message("", "onAudioPublishStateChanged ( newState = " + newState + " );", true));
                }

                @Override
                public void onAudioSubscribeStateChanged(String uid, DingRtcEngine.DingRtcSubscribeState oldState, DingRtcEngine.DingRtcSubscribeState newState, int elapseSinceLastState, String channel) {
                    super.onAudioSubscribeStateChanged(uid, oldState, newState, elapseSinceLastState, channel);
                    addMessage(new Message("", "onAudioSubscribeStateChanged ( newState = " + newState + " );", true));
                }
            });
            addMessage(new Message("", "mRtcEngine.setAudioProfile();", true));
            mRtcEngine.setAudioProfile(DingRtcEngineBasicQualityMode,DingRtcSceneDefaultMode);
            addMessage(new Message("", "mRtcEngine.publishLocalAudioStream();", true));
            mRtcEngine.publishLocalAudioStream(true);
            addMessage(new Message("", "mRtcEngine.subscribeAllRemoteAudioStreams();", true));
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
        addMessage(new Message("", "mRtcEngine.joinChannel();", true));
        int ret = mRtcEngine.joinChannel(authInfo, userName);
        if (ret != 0) {
            addMessage(new Message(mUserId, "joinChannel failed, result=" + ret, false));
            return false;
        }

        return true;
    }

    private void addMessage(Message message) {
        handler.post(new Runnable() {
            @Override
            public void run() {
                mMessageList.add(message);
                mMessageAdapter.notifyItemInserted(mMessageList.size() - 1);
                mRecyclerViewChat.scrollToPosition(mMessageList.size() - 1);
            }
        });
    }
}