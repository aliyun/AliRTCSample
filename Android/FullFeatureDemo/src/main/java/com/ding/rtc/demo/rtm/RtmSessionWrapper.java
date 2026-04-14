package com.ding.rtc.demo.rtm;

import com.ding.rtc.demo.utils.LimitedSizeLinkedList;

import java.util.ArrayList;
import java.util.List;

public class RtmSessionWrapper {

    private final RtmSession session;
    private List<RtmUserWrapper> users = new ArrayList<>();
    private final LimitedSizeLinkedList<RtmMessage> msgList = new LimitedSizeLinkedList<>(700);

    public RtmSessionWrapper(RtmSession session) {
        this.session = session;
    }

    public String getSessionId() {
        return session.sessionId;
    }

    public void setJoined(boolean joined) {
        session.joined = joined;
    }

    public boolean isJoined() {
        return session.joined;
    }

    public List<RtmUserWrapper> getRtmUsers() {
        return users;
    }

    public RtmUserWrapper getUser(String uid) {
        for (RtmUserWrapper user : users) {
            if (user.uid().equals(uid)) {
                return user;
            }
        }
        return null;
    }

    public void addUser(RtmUserWrapper user) {
        boolean exist = false;
        for (RtmUserWrapper rtmUser : users) {
            if (rtmUser.uid().equals(user.uid())) {
                exist = true;
                break;
            }
        }
        if (!exist) {
            users.add(user);
        }
    }

    public void removeUser(String uid) {
        for (RtmUserWrapper rtmUser : users) {
            if (rtmUser.uid().equals(uid)) {
                users.remove(rtmUser);
                break;
            }
        }
    }

    public void removeAllUsers() {
        users.clear();
    }

    public void setUsers(List<RtmUserWrapper> users) {
        this.users = users;
    }

    public void addMsg(RtmMessage msg) {
        msgList.add(msg);
    }

    public List<RtmMessage> getMsgList() {
        return msgList;
    }
}
