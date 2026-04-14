package com.ding.rtc.demo.rtm;

import static com.ding.rtc.demo.aiagent.AiAgent.AGENT_SESSION_ID;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.ding.rtc.DingRtmClient;
import com.ding.rtc.demo.BaseActivity;
import com.ding.rtc.demo.R;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

public class RtmChatActivity extends BaseActivity {
    private static final String TAG = "[RTM]";
    private String sessionId;
    private String uid;
    private String selfUid;
    private TextView titleView;
    private String titleStr;
    private EditText msgEditText;
    private CheckBox byteCheckBox;
    private ListView chatListView;
    private RtmChatAdapter adapter;

    private final BroadcastReceiver receiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d(TAG, "BroadcastReceiver receive action: " + action);
            if ("refresh".equals(action)) {
                refreshMsgList();
            }
        }
    };

    public static void launch(Context context, String sessionId, String uid, String selfUid) {
        Intent intent = new Intent();
        intent.setClass(context, RtmChatActivity.class);
        intent.putExtra("sessionId", sessionId);
        intent.putExtra("uid", uid);
        intent.putExtra("selfUid", selfUid);
        context.startActivity(intent);
    }

    private void refreshMsgList() {
        updateData();
        adapter.notifyDataSetChanged();
        chatListView.smoothScrollToPosition(adapter.getCount() - 1);
    }

    private void updateData() {
        RtmSessionWrapper session = RtmSessionMgr.getInstance().getSession(sessionId);
        if (session != null) {
            List<RtmMessage> msgList;
            if (TextUtils.isEmpty(uid)) {
                msgList = new ArrayList<>(session.getMsgList());
            } else {
                RtmUserWrapper user = session.getUser(uid);
                msgList = new ArrayList<>(user.getMsgList());
            }
            adapter.setMsgList(msgList);

            titleView.setText(titleStr + "- " + msgList.size());
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_rtm_chat);

        sessionId = getIntent().getStringExtra("sessionId");
        uid = getIntent().getStringExtra("uid");
        selfUid = getIntent().getStringExtra("selfUid");
        initTitleView();

        chatListView = findViewById(R.id.chat_list);
        adapter = new RtmChatAdapter(this, selfUid);
        chatListView.setAdapter(adapter);
        updateData();

        msgEditText = findViewById(R.id.msg_et);
        byteCheckBox = findViewById(R.id.byte_cb);
        byteCheckBox.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                msgEditText.setHint("");
                msgEditText.setText("0,3,-15,-76,83,-114,64,-44");
            } else {
                msgEditText.setHint("请输入消息内容");
                msgEditText.setText("");
            }
        });

        if (AGENT_SESSION_ID.equals(sessionId)) {
            findViewById(R.id.bottom_layout).setVisibility(View.GONE);
        }

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction("refresh");
        LocalBroadcastManager.getInstance(this).registerReceiver(receiver, intentFilter);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        LocalBroadcastManager.getInstance(this).unregisterReceiver(receiver);
    }


    private void initTitleView() {
        titleView = findViewById(R.id.tv_title);
        titleStr = "Session(" + sessionId + ")";
        if (!TextUtils.isEmpty(uid)) {
            titleStr += " uid(" + uid + ")";
        }
        titleView.setText(titleStr);
        ImageView leftIcon = findViewById(R.id.iv_left_icon);
        leftIcon.setImageResource(R.drawable.svg_icon_back);
        leftIcon.setOnClickListener(v -> onBackPressed());
    }

    public void onClickSendMsg(View view) {
        String text = msgEditText.getText().toString();
        if (TextUtils.isEmpty(text)) {
            showToast("消息内容为空");
            return;
        }
        boolean broadcast = TextUtils.isEmpty(uid);
        boolean sendByte = byteCheckBox.isChecked();
        byte[] bytes;
        if (sendByte) {
            String exMsg = msgEditText.getText().toString();
            String[] msgs = exMsg.split(",");
            bytes = new byte[msgs.length];
            for (int i = 0; i < msgs.length; i++) {
                String msg = msgs[i];
                try {
                    int value = Integer.parseInt(msg);
                    if (value > 127 || value < -128) {
                        showToast("输入非法");
                        return;
                    }
                    bytes[i] = (byte)value;
                } catch (NumberFormatException e) {
                    showToast("解析异常: " + e.getMessage());
                    return;
                }
            }

        } else {
            bytes = text.getBytes(StandardCharsets.UTF_8);
        }
        int ret = -1;
        DingRtmClient client = RtmClientHolder.getInstance().getRtmClient();
        if (client == null) {
            showToast("发送失败，DingRtmClient为空！");
            return;
        }
        if (broadcast) {
            ret = client.broadcastData(sessionId, bytes);
        } else {
            ret = client.sendData(sessionId, uid, bytes);
        }
        if (ret != 0) {
            showToast("发送失败，ret: " + ret);
            return;
        }
        msgEditText.setText("");

        RtmMessage message = new RtmMessage(sessionId, selfUid, broadcast, bytes);
        message.setIsByte(sendByte);
        RtmSessionWrapper session = RtmSessionMgr.getInstance().getSession(sessionId);
        if (broadcast) {
            session.addMsg(message);
        } else {
            RtmUserWrapper userWrapper = session.getUser(uid);
            userWrapper.addMsg(message);
        }
        refreshMsgList();
    }

}
