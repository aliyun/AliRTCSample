package com.ding.rtc.demo.rtm;

import java.nio.charset.StandardCharsets;

public class RtmMessage {
    public String sessionId;
    public String fromUid;
    public byte[] data;
    public boolean broadcast;
    public boolean isByte;

    public RtmMessage(String sessionId, String fromUid, boolean broadcast, byte[] data) {
        this.sessionId = sessionId;
        this.fromUid = fromUid;
        this.broadcast = broadcast;
        this.data = data;
    }

    public void setIsByte(boolean isByte) {
        this.isByte = isByte;
    }

    public String getUid() {
        return fromUid;
    }

    public String getStringData() {
        if (isByte) {
            StringBuilder message = new StringBuilder("[");
            if (isByte && data != null && data.length > 0) {
                for (int i = 0; i < data.length; i++) {
                    if (i != 0) {
                        message.append(",").append(data[i]);
                    } else {
                        message.append(data[i]);
                    }
                }
            }
            message.append("]");
            return message.toString();
        } else {
            return new String(data, StandardCharsets.UTF_8);
        }
    }

}
