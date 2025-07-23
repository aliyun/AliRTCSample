package com.ding.rtc.apiexample.basic.audiocall;

public class Message {
    private String fromUserId;
    private String content;
    private boolean isSent;

    public Message(String fromUserId, String content, boolean isSent) {
        this.fromUserId = fromUserId;
        this.content = content;
        this.isSent = isSent;
    }

    public String getContent() {
        return content;
    }

    public boolean isSent() {
        return isSent;
    }

    public String getFromUserId() {
        return fromUserId;
    }
}

