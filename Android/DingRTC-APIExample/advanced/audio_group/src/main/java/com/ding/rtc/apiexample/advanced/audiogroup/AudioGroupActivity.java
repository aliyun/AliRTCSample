package com.ding.rtc.apiexample.advanced.audiogroup;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

import  com.ding.rtc.apiexample.advanced.audiogroup.R;

public class AudioGroupActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;
    private Button mOperation;
    private EditText mEditTextGroupId;
    private RecyclerView mRecyclerView;
    private MessageAdapter mMessageAdapter;
    private List<Message> mMessageList;

    private boolean isJoinGroup = true;

    private Handler handler = new Handler(Looper.getMainLooper());

    public static void launch(Context context, String appId, String channelId, String userId,
                              String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, AudioGroupActivity.class);
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
        setContentView(R.layout.activity_audiogroup);
        mEditTextGroupId = (EditText) findViewById(R.id.input_box);
        mRecyclerView = findViewById(R.id.recyclerView);
        mOperation = findViewById(R.id.operation_button);
        mMessageList = new ArrayList<>();
        mMessageAdapter = new MessageAdapter(mMessageList);
        mRecyclerView.setLayoutManager(new LinearLayoutManager(this));
        mRecyclerView.setAdapter(mMessageAdapter);

        initDingEngine();
        joinChannel();
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        if (id == R.id.operation_button) {
            String groupId = mEditTextGroupId.getText().toString().trim();
            if (!groupId.isEmpty()) {
                if(isJoinGroup) {
                    addMessage(new Message(mUserId, "start join audio group : " + groupId, true));
                    mOperation.setText("Leave Group");
                    mEditTextGroupId.setEnabled(false);
                    mRtcEngine.joinGroup(groupId, "");
                    isJoinGroup = !isJoinGroup;
                } else {
                    addMessage(new Message(mUserId, "start leave audio group : " + groupId, true));
                    mOperation.setText("Join Group");
                    mEditTextGroupId.setEnabled(true);
                    mRtcEngine.leaveGroup(groupId);
                    isJoinGroup = !isJoinGroup;
                }
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

                @Override
                public void onAudioGroupJoinResult(int result, String errMsg, String group, List<DingRtcEngine.DingRtcAudioGroupMember> members) {
                    super.onAudioGroupJoinResult(result, errMsg, group, members);
                    addMessage(new Message(mUserId, "join audio group result : " + result, true));
                }

                @Override
                public void onAudioGroupLeaveResult(int result, String errMsg, String group) {
                    super.onAudioGroupLeaveResult(result, errMsg, group);
                    addMessage(new Message(mUserId, "leave audio group result : " + result, true));
                }

                @Override
                public void onAudioGroupMemberUpdate(int updateOpt, String group, List<DingRtcEngine.DingRtcAudioGroupMember> members) {
                    super.onAudioGroupMemberUpdate(updateOpt, group, members);
                    for(DingRtcEngine.DingRtcAudioGroupMember member: members) {
                        if (updateOpt == 1 && !mUserId.equals(member.uid)) {
                            addMessage(new Message(member.uid, "on join audio group : " + group, false));
                        } else if (updateOpt == -1 && !mUserId.equals(member.uid)) {
                            addMessage(new Message(member.uid, "on leave audio group : " + group, false));
                        }
                    }
                }
            });
            mRtcEngine.publishLocalAudioStream(true);
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
                mRecyclerView.scrollToPosition(mMessageList.size() - 1);
            }
        });
    }
}