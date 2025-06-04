package com.ding.rtc.apiexample.advanced.speechenhance;

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

public class SpeechEnhanceActivity extends AppCompatActivity {

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

    DingRtcEngine.DingRtcEngineAudioDenoiseOptions mOptions = new DingRtcEngine.DingRtcEngineAudioDenoiseOptions();

    private Handler handler = new Handler(Looper.getMainLooper());

    public static void launch(Context context, String appId, String channelId, String userId,
                              String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, SpeechEnhanceActivity.class);
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
        setContentView(R.layout.activity_speech_enhance);
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
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
                @Override
                public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {
                    super.onJoinChannelResult(result, channel, userId, elapsed);
                    addMessage(new Message(mUserId, "join success.", true));
                }

                @Override
                public void onRemoteUserOnLineNotify(String uid, int elapsed) {
                    super.onRemoteUserOnLineNotify(uid, elapsed);
                    addMessage(new Message(uid, "online.", false));
                }

                @Override
                public void onRemoteUserOffLineNotify(String uid, DingRtcEngine.DingRtcUserOfflineReason reason) {
                    super.onRemoteUserOffLineNotify(uid, reason);
                    addMessage(new Message(mUserId, "offline.", true));
                }
            });
            mRtcEngine.publishLocalAudioStream(true);
            mRtcEngine.subscribeAllRemoteAudioStreams(true);
            mOptions.mode = DingRtcEngine.DingRtcEngineAudioDenoiseMode.DingRtcEngineAudioDenoiseEnhance;
            mRtcEngine.setAudioDenoise(mOptions);
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

    public void onButtonClick(View view) {
        int id = view.getId();
        if (id == R.id.buttonDenoiseEnhance) {
            mOptions.mode = DingRtcEngine.DingRtcEngineAudioDenoiseMode.DingRtcEngineAudioDenoiseEnhance;
            mRtcEngine.setAudioDenoise(mOptions);
        } else if (id == R.id.buttonDenoiseDSP) {
            mOptions.mode = DingRtcEngine.DingRtcEngineAudioDenoiseMode.DingRtcEngineAudioDenoiseEnhance;
            mRtcEngine.setAudioDenoise(mOptions);
        } else if (id == R.id.buttonDenoiseOff) {
            mOptions.mode = DingRtcEngine.DingRtcEngineAudioDenoiseMode.DingRtcEngineAudioDenoiseEnhance;
            mRtcEngine.setAudioDenoise(mOptions);
        }
    }
}