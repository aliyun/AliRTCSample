package com.ding.rtc.demo.aiagent;

public class UpdateConfig {
    public String appId;
    public String channelId;
    public String taskId;
    public StartConfig.VoiceChatConfig voiceChatConfig = new StartConfig.VoiceChatConfig();

    public UpdateConfig(String appId, String channelId, String taskId) {
        this.appId = appId;
        this.channelId = channelId;
        this.taskId = taskId;
    }

    public void setChatMode(int chatMode) {
        voiceChatConfig.chatMode = chatMode;
    }

    public void setInterruptMode(int interruptMode) {
        voiceChatConfig.interruptMode = interruptMode;
    }

    public void setGreeting(String greeting) {
        voiceChatConfig.greeting = greeting;
    }

    public void setModel(String mode) {
        voiceChatConfig.LLMConfig.model = mode;
    }

    public void setPrompt(String prompt) {
        voiceChatConfig.LLMConfig.prompt = prompt;
    }

    public void setVoice(String voice) {
        voiceChatConfig.TTSConfig.voice = voice;
    }
}
