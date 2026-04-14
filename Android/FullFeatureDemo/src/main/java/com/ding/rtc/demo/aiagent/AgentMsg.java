package com.ding.rtc.demo.aiagent;

import android.text.TextUtils;

import com.ding.rtc.DingRtmAgentClient;

public class AgentMsg {
    private final String agentId;
    private final DingRtmAgentClient.TranscriptionData data;

    public AgentMsg(String agentId, DingRtmAgentClient.TranscriptionData data) {
        this.agentId = agentId;
        this.data = data;
    }

    public boolean isAgent() {
        return data != null && data.speakerType == DingRtmAgentClient.RtmAgentSpeakerType.Agent;
    }

    public boolean isEnd() {
        if (data != null) {
            return data.end;
        }
        return true;
    }

    public String getMsg() {
        if (data == null) {
            return "消息异常";
        }
        if (!TextUtils.isEmpty(data.text)) {
            return data.text;
        }
        if (!TextUtils.isEmpty(data.reasoningText)) {
            return data.reasoningText;
        }
        return "";
    }
}
