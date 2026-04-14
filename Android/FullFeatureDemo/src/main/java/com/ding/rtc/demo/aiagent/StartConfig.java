package com.ding.rtc.demo.aiagent;

import com.ding.rtc.demo.utils.ConfigManager;

public class StartConfig {
    public String appId;
    public String channelId;
    public String taskId;
    public String templateId;
    public RtcConfig rtcConfig;
    public VoiceChatConfig voiceChatConfig = new VoiceChatConfig();
    
    public StartConfig(String appId, String channelId, String taskId, String templateId, String aiAgentId, String[] uid) {
        this.appId = appId;
        this.channelId = channelId;
        this.taskId = taskId;
        this.templateId = templateId;
        rtcConfig = new RtcConfig();
        rtcConfig.userId = aiAgentId;
        rtcConfig.targetUserIds = uid;
        
        // 从配置文件读取敏感配置
        ConfigManager configManager = ConfigManager.getInstance();
        rtcConfig.serviceName = configManager.getAiAgentRtcServiceName();
        voiceChatConfig.ASRConfig.vendor = configManager.getAiAgentAsrVendor();
        voiceChatConfig.ASRConfig.model = configManager.getAiAgentAsrModel();
        voiceChatConfig.LLMConfig.vendor = configManager.getAiAgentLlmVendor();
        voiceChatConfig.LLMConfig.model = configManager.getAiAgentLlmModel();
        voiceChatConfig.LLMConfig.temperature = configManager.getAiAgentLlmTemperature();
        voiceChatConfig.LLMConfig.topP = configManager.getAiAgentLlmTopP();
        voiceChatConfig.LLMConfig.maxToken = configManager.getAiAgentLlmMaxToken();
        voiceChatConfig.LLMConfig.historyDepth = configManager.getAiAgentLlmHistoryDepth();
        voiceChatConfig.LLMConfig.prompt = configManager.getAiAgentLlmPrompt();
        voiceChatConfig.TTSConfig.vendor = configManager.getAiAgentTtsVendor();
        voiceChatConfig.TTSConfig.model = configManager.getAiAgentTtsModel();
        voiceChatConfig.TTSConfig.voice = configManager.getAiAgentTtsVoice();
    }

    public void setModel(String mode) {
        voiceChatConfig.LLMConfig.model = mode;
    }

    public void setChatModel(int chatMode) {
        voiceChatConfig.chatMode = chatMode;
    }

    public void setInterruptMode(int interruptMode) {
        voiceChatConfig.interruptMode = interruptMode;
    }

    public void setGreeting(String greeting) {
        voiceChatConfig.greeting = greeting;
    }

    public void setVoice(String voice) {
        voiceChatConfig.TTSConfig.voice = voice;
    }

    public static class RtcConfig {
        public String serviceName;
        public String userId;
        public String[] targetUserIds;
    }

    public static class VoiceChatConfig {
        public int chatMode;
        public int interruptMode;
        public String greeting;
        public ASRConfig ASRConfig = new ASRConfig();
        public LLMConfig LLMConfig = new LLMConfig();
        public TTSConfig TTSConfig = new TTSConfig();
    }

    public static class ASRConfig {
        public String vendor;
        public String model;
    }

    public static class LLMConfig {
        public String vendor;
        public String model;
        public float temperature;
        public float topP;
        public int maxToken;
        public int historyDepth;
        public String prompt;
    }

    public static class TTSConfig {
        public String vendor;
        public String model;
        public String voice;
    }
}
