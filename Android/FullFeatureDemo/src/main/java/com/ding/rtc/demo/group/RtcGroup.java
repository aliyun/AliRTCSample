package com.ding.rtc.demo.group;

import com.ding.rtc.DingRtcEngine.DingRtcAudioGroupMember;

import java.util.ArrayList;
import java.util.List;

public class RtcGroup {

    public String groupId;

    public String groupName;

    private boolean joined;

    public List<DingRtcAudioGroupMember> memberList = new ArrayList<>();

    public RtcGroup(String groupId) {
        this.groupId = groupId;
    }

    public void setJoined(boolean joined) {
        this.joined = joined;
    }

    public boolean isJoined() {
        return joined;
    }

    public void setMembers(List<DingRtcAudioGroupMember> members) {
        memberList.clear();
        memberList.addAll(members);
    }

    public void addMembers(List<DingRtcAudioGroupMember> members) {
        for (DingRtcAudioGroupMember member : members) {
            addMember(member);
        }
    }

    public void removeMembers(List<DingRtcAudioGroupMember> members) {
        for (DingRtcAudioGroupMember member : members) {
            removeMember(member.uid);
        }
    }

    public void addMember(DingRtcAudioGroupMember member) {
        boolean alreadyContained = false;
        for (DingRtcAudioGroupMember dingRtcAudioGroupMember : memberList) {
            if (member.uid.equals(dingRtcAudioGroupMember.uid)) {
                alreadyContained = true;
                break;
            }
        }
        if (!alreadyContained) {
            memberList.add(member);
        }
    }

    public void removeMember(String uid) {
        for (DingRtcAudioGroupMember member : memberList) {
            if (member.uid.equals(uid)) {
                memberList.remove(member);
                break;
            }
        }
    }
}
