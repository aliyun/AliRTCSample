package com.ding.rtc.demo.group;

import static android.content.Context.WINDOW_SERVICE;

import static com.ding.rtc.DingRtcEngine.HALL_ID;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.graphics.drawable.PaintDrawable;
import android.text.TextUtils;
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

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngine.DingRtcAudioGroupMember;
import com.ding.rtc.demo.BaseActivity;
import com.ding.rtc.demo.R;

import java.util.List;

public class GroupListPopup extends PopupWindow implements GroupExpandableAdapter.GroupListListener {

    private static final String TAG = "[RtcGroup]";

    private final BaseActivity activity;
    private final TextView groupsTitle;
    private final GroupExpandableAdapter adapter;
    private final DingRtcEngine engine;

    public GroupListPopup(BaseActivity context, DingRtcEngine engine) {
        this.engine = engine;
        activity = context;
        adapter = new GroupExpandableAdapter(context);
        adapter.setGroupListListener(this);

        DisplayMetrics dm = new DisplayMetrics();
        Display display = ((WindowManager) activity.getApplication()
                .getSystemService(WINDOW_SERVICE)).getDefaultDisplay();
        display.getRealMetrics(dm);
        int screenHeight = dm.heightPixels;
        View rootView = LayoutInflater.from(context).inflate(R.layout.group_mrg_pop_layout, null);
        groupsTitle = rootView.findViewById(R.id.group_title);
        ExpandableListView expandableListView = rootView.findViewById(R.id.group_expandable_list);
        expandableListView.setAdapter(adapter);
        rootView.findViewById(R.id.create_group).setOnClickListener(v -> onClickCreateGroup());
        rootView.findViewById(R.id.query_subscribed_group).setOnClickListener(v -> onClickGetSubscribedGroup());
        rootView.findViewById(R.id.unsub_all).setOnClickListener(v -> onClickUnSubscribeAll());

        ViewGroup.LayoutParams params = expandableListView.getLayoutParams();
        params.height = screenHeight / 3;
        expandableListView.setLayoutParams(params);

        setContentView(rootView);
        setWidth(ViewGroup.LayoutParams.MATCH_PARENT);
        setHeight(ViewGroup.LayoutParams.WRAP_CONTENT);
        setBackgroundDrawable(new PaintDrawable(Color.TRANSPARENT));
        setOutsideTouchable(true);
    }

    private void onClickCreateGroup() {
        EditText editText = new EditText(activity);
        editText.setHint("请输入分组ID");
        Dialog createDialog = new AlertDialog.Builder(activity)
                .setTitle("创建分组")
                .setView(editText)
                .setPositiveButton("确定", (dialog, which) -> {
                    String groupId = editText.getText().toString();
                    if (!groupId.isEmpty()) {
                        int result = engine.joinGroup(groupId, "Android Test Data1");
                        Log.i(TAG, "create group result " + result);
                        if (result != 0) {
                            activity.showToast("创建分组失败！");
                        }
                    }
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        createDialog.show();
    }

    private void onClickGetSubscribedGroup() {
        String groupId = engine.getCurrentSubscribedAudio();
        Log.i(TAG, "onClickGetSubscribedGroup " + groupId);
        String des;
        if (TextUtils.isEmpty(groupId)) {
            des = "当前无订阅分组";
        } else {
            des = "已订阅分组：" + getGroupDes(groupId);
        }
        activity.showToast(des);
    }

    private void onClickUnSubscribeAll() {
        Dialog unsubDialog = new AlertDialog.Builder(activity)
                .setTitle("取消订阅？")
                .setPositiveButton("确定", (dialog, which) -> {
                    engine.subscribeAllRemoteAudioStreams(false);
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        unsubDialog.show();
    }

    @Override
    public void onLeaveGroup(String groupId) {
        Dialog leaveGroupDialog = new AlertDialog.Builder(activity)
                .setTitle("离开分组")
                .setMessage("确定要离开分组 " + groupId + " 吗？")
                .setPositiveButton("确定", (dialog, which) -> {
                    int result = engine.leaveGroup(groupId);
                    Log.i(TAG, "onLeaveGroup result " + result);
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        leaveGroupDialog.show();
    }

    @Override
    public void onSubscribeGroup(String groupId) {
        Dialog subscribeGroupDialog = new AlertDialog.Builder(activity)
                .setTitle("订阅分组" + getGroupDes(groupId) +  "？")
                .setPositiveButton("确定", (dialog, which) -> {
                    int result = engine.switchSubscriptionToGroup(groupId);
                    Log.i(TAG, "switchSubscriptionToGroup result " + result);
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        subscribeGroupDialog.show();
    }

    @Override
    public void onJoinGroup(String groupId) {
        Dialog joinGroupDialog = new AlertDialog.Builder(activity)
                .setTitle("加入分组" + getGroupDes(groupId) + "？")
                .setPositiveButton("确定", (dialog, which) -> {
                    int result = engine.joinGroup(groupId, "Android Test Data2");
                    Log.i(TAG, "joinGroup result " + result);
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        joinGroupDialog.show();
    }

    @Override
    public void onDismissGroup(String groupId) {
        Dialog dismissGroupDialog = new AlertDialog.Builder(activity)
                .setTitle("解散分组" + groupId + "？")
                .setPositiveButton("确定", (dialog, which) -> {
                    int result = engine.dismissGroup(groupId);
                    Log.i(TAG, "onDismissGroup result " + result);
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        dismissGroupDialog.show();
    }

    boolean isMix = true;
    @Override
    public void onMixGroup(String groupId) {
        String[] itemStrs = new String[] {"合流", "取消合流"};
        Dialog mixGroupDialog = new AlertDialog.Builder(activity)
                .setTitle("合流音频到分组" + getGroupDes(groupId) + "？")
                .setSingleChoiceItems(itemStrs, 0, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        isMix = (which == 0);
                    }
                })
                .setPositiveButton("确定", (dialog, which) -> {
                    int result = engine.mixAudioToGroup(isMix, groupId);
                    Log.i(TAG, "mixAudioToGroup result " + result);
                    String des = isMix ? "合流" : "取消合流";
                    if (result != 0) {
                        activity.showToast(des + "音频到分组" + getGroupDes(groupId) + "失败：" + result);
                    }
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        mixGroupDialog.show();
    }

    @Override
    public void onSetGroupName(String groupId) {
        EditText editText = new EditText(activity);
        editText.setHint("请输入分组名称");
        Dialog setGroupNameDialog = new AlertDialog.Builder(activity)
                .setTitle("修改分组名称")
                .setView(editText)
                .setPositiveButton("确定", (dialog, which) -> {
                    String groupName = editText.getText().toString();
                    if (!groupName.isEmpty()) {
                        int result = engine.setGroupName(groupId, groupName);
                        Log.i(TAG, "set groupName result " + result);
                        if (result != 0) {
                            activity.showToast("设置分组名称失败！");
                        }
                    }
                })
                .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                .create();
        setGroupNameDialog.show();
    }

    public void onListAllAudioGroups(List<String> groups) {
        GroupMgr.getInstance().addAllGroup(groups);
        refreshGroupList();
    }

    public void onAudioGroupJoinResult(int result, String errMsg, String group, List<DingRtcAudioGroupMember> members) {
        String toast;
        if (result == 0) {
            toast = "加入分组: " + group + "成功";
        } else {
            toast = "加入分组: " + group + "失败, errMsg: " + errMsg;
        }
        activity.showToast(toast);
        if (result != 0) {
            GroupMgr.getInstance().removeGroup(group);
            return;
        }
        RtcGroup rtcGroup = GroupMgr.getInstance().getGroup(group);
        if (rtcGroup == null) {
            rtcGroup = new RtcGroup(group);
            rtcGroup.setMembers(members);
            GroupMgr.getInstance().addGroup(rtcGroup);
        } else {
            rtcGroup.setMembers(members);
        }
        rtcGroup.setJoined(true);
        refreshGroupList();
    }

    public void onAudioGroupLeaveResult(int result, String errMsg, String group) {
        String toast;
        if (result == 0) {
            toast = "离开分组: " + group + "成功";
        } else {
            toast = "离开分组: " + group + "失败, errMsg: " + errMsg;
        }
        RtcGroup rtcGroup = GroupMgr.getInstance().getGroup(group);
        if (rtcGroup != null) {
            rtcGroup.setJoined(false);
        }
        activity.showToast(toast);
        refreshGroupList();
    }

    public void onAudioGroupDismissResult(int result, String errMsg, String group) {
        String toast;
        if (result == 0) {
            toast = "解散分组: " + group + "成功";
        } else {
            toast = "解散分组: " + group + "失败, errMsg: " + errMsg;
        }
        activity.showToast(toast);
        if (result == 0) {
            GroupMgr.getInstance().removeGroup(group);
            refreshGroupList();
        }
    }

    public void onAudioGroupMixResult(String group, boolean mix, int result, String reason) {
        String toast;
        String mixGroupDes;
        String groupDes = getGroupDes(group);
        String mixDes = mix ? "合流" : "取消合流";
        if (result == 0) {
            toast = "分组: " + groupDes + mixDes + "成功";
        } else {
            toast = "分组: " + groupDes + mixDes +  "失败, reason: " + reason;
        }
        activity.showToast(toast);
    }

    public void onAudioGroupMemberUpdate(int updateOpt, String group, List<DingRtcEngine.DingRtcAudioGroupMember> members) {
        RtcGroup rtcGroup = GroupMgr.getInstance().getGroup(group);
        if (rtcGroup == null) {
            return;
        }
        if (updateOpt == 1) {
            rtcGroup.addMembers(members);
        } else if (updateOpt == -1) {
            rtcGroup.removeMembers(members);
        }
        refreshGroupList();
    }

    public void onAudioGroupHallMembers(List<DingRtcEngine.DingRtcAudioGroupMember> hallMembers) {
        RtcGroup rtcGroup = GroupMgr.getInstance().getGroup(HALL_ID);
        if (rtcGroup == null) {
            rtcGroup = new RtcGroup(HALL_ID);
            rtcGroup.addMembers(hallMembers);
            GroupMgr.getInstance().addGroup(rtcGroup);
        } else {
            rtcGroup.setMembers(hallMembers);
        }
        refreshGroupList();
    }

    public void onAudioGroupListUpdate(int updateOpt, String group) {
        if (updateOpt == 1) {
            GroupMgr.getInstance().addGroup(group);
        } else if (updateOpt == -1) {
            GroupMgr.getInstance().removeGroup(group);
        }
        refreshGroupList();
    }

    public void onGroupNameChanged(String group, String newName) {
        GroupMgr.getInstance().setGroupName(group, newName);
        refreshGroupList();
    }

    private void refreshGroupList() {
        adapter.setData(GroupMgr.getInstance().getGroupList());
        groupsTitle.setText("音频分组列表(" + GroupMgr.getInstance().getGroupCount() + ")");
        adapter.notifyDataSetChanged();
    }

    private String getGroupDes(String groupId) {
        return HALL_ID.equals(groupId) ? "大厅" : groupId;
    }
}
