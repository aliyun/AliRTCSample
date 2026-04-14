package com.ding.rtc.demo.group;

import static com.ding.rtc.DingRtcEngine.HALL_ID;

import android.text.TextUtils;


import java.util.ArrayList;
import java.util.List;

public class GroupMgr {

    private static final String TAG = "[RtcGroup]";

    private final List<RtcGroup> groupList = new ArrayList<>();
    private long localUserId;

    private static class Holder {
        private static final GroupMgr INSTANCE = new GroupMgr();
    }

    public static GroupMgr getInstance() {
        return Holder.INSTANCE;
    }

    private GroupMgr() {
    }

    public void setLocalUserId(long userId) {
        localUserId = userId;
    }

    public RtcGroup getGroup(String groupId) {
        if (TextUtils.isEmpty(groupId)) {
            return null;
        }
        for (RtcGroup group : groupList) {
            if (groupId.equals(group.groupId)) {
                return group;
            }
        }
        return null;
    }

    public List<RtcGroup> getGroupList() {
        return groupList;
    }

    public int getGroupCount() {
        return groupList.size();
    }

    public void addAllGroup(List<String> groups) {
        for (String groupId : groups) {
            addGroup(groupId);
        }
    }

    public void addGroup(RtcGroup group) {
        boolean alreadyContained = false;
        for (RtcGroup rtcGroup : groupList) {
            if (group.groupId.equals(rtcGroup.groupId)) {
                alreadyContained = true;
                break;
            }
        }
        if (!alreadyContained) {
            groupList.add(group);
        }
    }

    public void addGroup(String groupId) {
        boolean alreadyContained = false;
        for (RtcGroup group : groupList) {
            if (group.groupId.equals(groupId)) {
                alreadyContained = true;
                break;
            }
        }
        if (!alreadyContained) {
            RtcGroup group = new RtcGroup(groupId);
            if (HALL_ID.equals(groupId)) {
                group.setJoined(true);
            }
            groupList.add(group);
        }
    }

    public void removeGroup(String groupId) {
        for (RtcGroup group : groupList) {
            if (group.groupId.equals(groupId)) {
                groupList.remove(group);
                break;
            }
        }
    }

    public void setGroupName(String groupId, String name) {
        RtcGroup group = getGroup(groupId);
        if (group != null) {
            group.groupName = name;
        }
    }

    public void clearGroup() {
        groupList.clear();
    }

}
