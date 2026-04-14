package com.ding.rtc.demo.rtm;

import com.ding.rtc.demo.utils.LimitedSizeLinkedList;

import java.util.List;

public class RtmUserWrapper {
    private final RtmUser user;
    private final List<RtmMessage> msgList = new LimitedSizeLinkedList<>(700);

    public RtmUserWrapper(RtmUser user) {
        this.user = user;
    }

    public String uid() {
        return user.uid;
    }

    public void addMsg(RtmMessage msg) {
        msgList.add(msg);
    }

    public List<RtmMessage> getMsgList() {
        return msgList;
    }
}
