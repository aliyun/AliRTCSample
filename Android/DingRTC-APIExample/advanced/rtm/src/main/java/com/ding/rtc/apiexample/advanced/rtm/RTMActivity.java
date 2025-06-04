package com.ding.rtc.apiexample.advanced.rtm;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;
import android.widget.EditText;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;
import com.ding.rtc.DingRtmClient;
import com.ding.rtc.DingRtmEventListener;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

public class RTMActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;

    private EditText mEditTextMessage;
    private RecyclerView mRecyclerViewChat;
    private MessageAdapter mMessageAdapter;
    private List<Message> mMessageList;

    private Handler handler = new Handler(Looper.getMainLooper());

    public static void launch(Context context, String appId, String channelId, String userId,
                              String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, RTMActivity.class);
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
        setContentView(R.layout.activity_rtm);
        mEditTextMessage = (EditText) findViewById(R.id.input_box);
        mRecyclerViewChat = findViewById(R.id.recyclerViewChat);

        mMessageList = new ArrayList<>();
        mMessageAdapter = new MessageAdapter(mMessageList);
        mRecyclerViewChat.setLayoutManager(new LinearLayoutManager(this));
        mRecyclerViewChat.setAdapter(mMessageAdapter);

        initDingEngine();
        joinChannel();
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        if (id == R.id.send_button) {
            String messageContent = mEditTextMessage.getText().toString().trim();
            if (!messageContent.isEmpty()) {
                addMessage(new Message(mUserId, messageContent, true));
                mEditTextMessage.setText("");
                mRtcEngine.getRtmClient().broadcastData(mChannelId, messageContent.getBytes(StandardCharsets.UTF_8));
            }
        }
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
            });
            mRtcEngine.getRtmClient().setListener(new DingRtmEventListener() {

                @Override
                public void onRtmServerStateChanged(DingRtmClient.DingRtmServerState state, int errorCode) {
                    super.onRtmServerStateChanged(state, errorCode);
                    addMessage(new Message(mUserId, "rtm server state changed : " + state, true));
                    mRtcEngine.getRtmClient().joinSession(mChannelId);
                }

                @Override
                public void onSessionCreate(String sessionId) {
                    super.onSessionCreate(sessionId);
                    addMessage(new Message(mUserId, "on session create : " + sessionId, true));
                }

                @Override
                public void onMessage(String sessionId, String fromUid, boolean broadcast, byte[] data) {
                    super.onMessage(sessionId, fromUid, broadcast, data);
                    try {
                        String messageContent = new String(data, "UTF-8");
                        addMessage(new Message(fromUid, messageContent, false));
                    } catch (Exception e) {
                        addMessage(new Message(fromUid, "message is invalid !", false));
                        e.printStackTrace();
                    }
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