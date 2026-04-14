package com.ding.rtc.demo.rtm;

import static android.content.Context.WINDOW_SERVICE;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.PaintDrawable;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.EditText;
import android.widget.ExpandableListView;
import android.widget.PopupWindow;
import android.widget.TextView;

import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.ding.rtc.DingRtmClient;
import com.ding.rtc.DingRtmEventListener;
import com.ding.rtc.demo.BaseActivity;
import com.ding.rtc.demo.R;

import java.nio.charset.StandardCharsets;

public class RtmChatPopup extends PopupWindow implements RtmExpandableAdapter.SessionListListener {
    private static final String TAG = "[RTM]";

    private final TextView sessionsTitle;
    private final RtmExpandableAdapter adapter;
    private final BaseActivity activity;
    private final DingRtmClient client;
    private final String selfUid;

    public RtmChatPopup(BaseActivity context, DingRtmClient client, String selfUid) {
        this.activity = context;
        this.client = client;
        this.selfUid = selfUid;
        setRtmEventListener();
        adapter = new RtmExpandableAdapter(context, selfUid);
        adapter.setSessionListListener(this);
        View rootView = LayoutInflater.from(context).inflate(R.layout.rtm_chat_popup_layout, null);
        rootView.findViewById(R.id.create_session).setOnClickListener(v -> onClickCreateSession());
        sessionsTitle = rootView.findViewById(R.id.current_sessions);

        DisplayMetrics dm = new DisplayMetrics();
        Display display = ((WindowManager) context.getApplication()
                .getSystemService(WINDOW_SERVICE)).getDefaultDisplay();
        display.getRealMetrics(dm);
        int screenHeight = dm.heightPixels;

        ExpandableListView expandableListView = rootView.findViewById(R.id.group_expandable_list);
        ViewGroup.LayoutParams params = expandableListView.getLayoutParams();
        params.height = screenHeight / 3;
        expandableListView.setLayoutParams(params);
        expandableListView.setAdapter(adapter);

        setContentView(rootView);
        setWidth(ViewGroup.LayoutParams.MATCH_PARENT);
        setHeight(ViewGroup.LayoutParams.WRAP_CONTENT);
        setBackgroundDrawable(new PaintDrawable(Color.TRANSPARENT));
        setOutsideTouchable(true);
    }

    private void refreshSessionList() {
        adapter.setData(RtmSessionMgr.getInstance().getSessionList());
        sessionsTitle.setText("现有群组(" + RtmSessionMgr.getInstance().getSessionCount() + ")");
        adapter.notifyDataSetChanged();
    }

    private void onClickCreateSession() {
        EditText editText = new EditText(activity);
        editText.setHint("请输入sessionId");
        Dialog createDialog = new AlertDialog.Builder(activity)
                .setTitle("创建 Session")
                .setView(editText)
                .setPositiveButton("确定", (dialog, which) -> {
                    String sessionId = editText.getText().toString();
                    if (!sessionId.isEmpty()) {
                        int result = client.joinSession(sessionId);
                        Log.i(TAG, "Create session result " + result);
                        if (result != 0) {
                            activity.showToast("创建Session失败！");
                        }
                    }
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        createDialog.show();
    }

    @Override
    public void onJoinSession(String sessionId) {
        Dialog joinSessionDialog = new AlertDialog.Builder(activity)
                .setTitle("加入Session:" + sessionId + "？")
                .setPositiveButton("确定", (dialog, which) -> {
                    int result = client.joinSession(sessionId);
                    Log.i(TAG, "joinSession result " + result);
                    if (result != 0) {
                        activity.showToast("Join Session失败, result: " + result);
                    }
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        joinSessionDialog.show();
    }

    @Override
    public void onLeaveSession(String sessionId) {
        Dialog leaveSessionDialog = new AlertDialog.Builder(activity)
                .setTitle("离开Session:" + sessionId + "？")
                .setPositiveButton("确定", (dialog, which) -> {
                    int result = client.leaveSession(sessionId);
                    Log.i(TAG, "leaveSession result " + result);
                    if (result != 0) {
                        activity.showToast("Leave Session失败, result: " + result);
                    }
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        leaveSessionDialog.show();
    }

    @Override
    public void onCloseSession(String sessionId) {
        Dialog closeSessionDialog = new AlertDialog.Builder(activity)
                .setTitle("关闭Session:" + sessionId + "？")
                .setPositiveButton("确定", (dialog, which) -> {
                    int result = client.closeSession(sessionId);
                    Log.i(TAG, "closeSession result " + result);
                    if (result != 0) {
                        activity.showToast("Close Session失败, result: " + result);
                    }
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        closeSessionDialog.show();
    }

    @Override
    public void onClickChat(String sessionId, String uid) {
        RtmChatActivity.launch(activity, sessionId, uid, selfUid);
    }

    private void setRtmEventListener() {
        RtmClientHolder.getInstance().addListener(new DingRtmEventListener() {

            @Override
            public void onRtmServerStateChanged(DingRtmClient.DingRtmServerState state, int errorCode) {
                activity.runOnUiThread(() -> {
                    Log.i(TAG, "onRtmServerStateChanged, state: " + state + ", errorCode: " + errorCode);
                    if (state == DingRtmClient.DingRtmServerState.Available) {
                        activity.showToast("RTM服务可用");
                    } else {
                        activity.showToast("RTM服务不可用, errorCode: " + errorCode);
                    }
                });
            }

            @Override
            public void onJoinSessionResult(String sessionId, int result) {
                activity.runOnUiThread(() -> {
                    Log.i(TAG, "onJoinSessionResult, sessionId: " + sessionId + ", result: " + result);
                    if (result == 0) {
                        RtmSessionMgr.getInstance().onJoinSession(sessionId);
                        RtmUser user = new RtmUser();
                        user.uid = selfUid;
                        RtmSessionMgr.getInstance().addSessionUser(sessionId, new RtmUserWrapper(user));
                        refreshSessionList();

//                        DingRtmClient.SessionPropertyData data = new DingRtmClient.SessionPropertyData(
//                                DingRtmClient.SessionPropertyAction.APPEND, "test_ket", "test_value"
//                        );
//
//                        client.setSessionProperty(sessionId, data);
                    } else {
                        activity.showToast("JoinSession " + sessionId + "failed");
                    }
                });

            }

            @Override
            public void onLeaveSessionResult(String sessionId, int result) {
                activity.runOnUiThread(() -> {
                    Log.i(TAG, "onLeaveSessionResult, sessionId: " + sessionId + ", result: " + result);
                    RtmSessionMgr.getInstance().onLeaveSession(sessionId);
                    refreshSessionList();
                });
            }

            @Override
            public void onCloseSessionResult(String sessionId, int result) {
                activity.runOnUiThread(() -> {
                    Log.i(TAG, "onCloseSessionResult, sessionId: " + sessionId + ", result: " + result);
                    if (result == 0) {
                        RtmSessionMgr.getInstance().removeSession(sessionId);
                        refreshSessionList();
                    } else {
                        activity.showToast("closeSession " + sessionId + "failed");
                    }
                });
            }

            @Override
            public void onRemovedFromSession(String sessionId, int reason) {
                activity.runOnUiThread(() -> {
                    Log.i(TAG, "onRemovedFromSession, sessionId: " + sessionId + ", reason: " + reason);
                    if (reason == 0) {
                        RtmSessionMgr.getInstance().removeSession(sessionId);
                        refreshSessionList();
                    }
                });
            }

            @Override
            public void onSessionCreate(String sessionId) {
                activity.runOnUiThread(() -> {
                    Log.i(TAG, "onSessionCreate, sessionId: " + sessionId);
                    RtmSession session = new RtmSession();
                    session.sessionId = sessionId;
                    RtmSessionMgr.getInstance().addSession(new RtmSessionWrapper(session));
                    refreshSessionList();
                });
            }

            @Override
            public void onSessionClose(String sessionId) {
                activity.runOnUiThread(() -> {
                    Log.i(TAG, "onSessionClose, sessionId: " + sessionId);
                    RtmSessionMgr.getInstance().removeSession(sessionId);
                    refreshSessionList();
                });
            }

            @Override
            public void onSessionRemoteUserJoin(String sessionId, String uid) {
                activity.runOnUiThread(() -> {
                    Log.i(TAG, "onSessionRemoteUserJoin, sessionId: " + sessionId + ", uid: " + uid);
                    RtmUser user = new RtmUser();
                    user.uid = uid;
                    RtmSessionMgr.getInstance().addSessionUser(sessionId, new RtmUserWrapper(user));
                    refreshSessionList();
                });
            }

            @Override
            public void onSessionRemoteUserLeave(String sessionId, String uid) {
                activity.runOnUiThread(() -> {
                    Log.i(TAG, "onSessionRemoteUserLeave, sessionId: " + sessionId + ", uid: " + uid);
                    RtmSessionMgr.getInstance().removeSessionUser(sessionId, uid);
                    refreshSessionList();
                });
            }

            @Override
            public void onSessionPropertySettingResult(String sessionId, String key, int result) {
                activity.runOnUiThread(() -> {
                    activity.showToast("Property更新完成, key: " + key + ", result : " + result);
                });
            }

            @Override
            public void onSessionPropertyUpdate(String sessionId, DingRtmClient.SessionPropertyData data) {
                activity.runOnUiThread(() -> {
                    activity.showToast("Property有更新, key: " + data.key
                            + ", value : " + data.value + ", action: " + data.action);
                });
            }

            @Override
            public void onMessage(String sessionId, String fromUid, boolean broadcast, byte[] data) {
                activity.runOnUiThread(() -> {
                    Log.i(TAG, "onMessage, sessionId: " + sessionId
                            + ", fromUid: " + fromUid
                            + ", broadcast: " + broadcast
                            + ", data: " + new String(data, StandardCharsets.UTF_8));
                    RtmSessionWrapper sessionWrapper = RtmSessionMgr.getInstance().getSession(sessionId);
                    if (sessionWrapper == null) {
                        Log.e(TAG, "Cannot find session " + sessionId);
                        return;
                    }
                    RtmMessage message = new RtmMessage(sessionId, fromUid, broadcast, data);

                    if (broadcast) {
                        sessionWrapper.addMsg(message);
                    } else {
                        RtmUserWrapper userWrapper = sessionWrapper.getUser(fromUid);
                        if (userWrapper != null) {
                            userWrapper.addMsg(message);
                        } else {
                            Log.e(TAG, "Cannot find user :" + message.getUid());
                        }
                    }
                    LocalBroadcastManager.getInstance(activity).sendBroadcast(new Intent("refresh"));
                });
            }
        });
    }
}
