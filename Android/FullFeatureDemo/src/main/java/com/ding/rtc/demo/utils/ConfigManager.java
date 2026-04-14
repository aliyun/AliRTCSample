package com.ding.rtc.demo.utils;

import android.content.Context;
import android.util.Log;

import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
/**
 * 配置管理器，用于读取和管理配置信息
 * 配置文件位于 assets/config.json
 */
public class ConfigManager {
    private static final String TAG = "ConfigManager";
    private static final String CONFIG_FILE = "config.json";
    
    private static ConfigManager instance;
    private JSONObject config;
    
    private ConfigManager() {}
    
    public static synchronized ConfigManager getInstance() {
        if (instance == null) {
            instance = new ConfigManager();
        }
        return instance;
    }
    
    /**
     * 初始化配置管理器，从 assets 目录读取配置文件
     */
    public void init(Context context) {
        if (config != null) {
            return;
        }
        
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(
                    context.getAssets().open(CONFIG_FILE)));
            StringBuilder sb = new StringBuilder();
            String line;
            while ((line = reader.readLine()) != null) {
                sb.append(line);
            }
            reader.close();
            config = new JSONObject(sb.toString());
            Log.d(TAG, "Config loaded successfully");
        } catch (IOException e) {
            Log.e(TAG, "Failed to load config file: " + e.getMessage());
            config = new JSONObject();
        } catch (Exception e) {
            Log.e(TAG, "Failed to parse config file: " + e.getMessage());
            config = new JSONObject();
        }
    }
    
    /**
     * 获取在线环境的 AppServer 地址
     */
    public String getOnlineAppServerUrl() {
        try {
            return config.getJSONObject("appServerConfig").getString("online");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get online app server url: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 AI Agent 默认模板ID（在线环境）
     */
    public String getAiAgentDefaultTemplateId() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("defaultTemplateId");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent default template id online: " + e.getMessage());
            return "";
        }
    }
    
    // AI Agent 配置相关方法
    
    /**
     * 获取 RtcServiceName
     */
    public String getAiAgentRtcServiceName() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("rtcServiceName");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent rtc service name: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 ASR Vendor
     */
    public String getAiAgentAsrVendor() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("asrVendor");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent asr vendor: " + e.getMessage());
            return "aliyun";
        }
    }
    
    /**
     * 获取 ASR Model
     */
    public String getAiAgentAsrModel() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("asrModel");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent asr model: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 LLM Vendor
     */
    public String getAiAgentLlmVendor() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("llmVendor");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent llm vendor: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 LLM Model
     */
    public String getAiAgentLlmModel() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("llmModel");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent llm model: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 LLM Temperature
     */
    public float getAiAgentLlmTemperature() {
        try {
            return (float) config.getJSONObject("aiAgentConfig").getDouble("llmTemperature");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent llm temperature: " + e.getMessage());
            return 0f;
        }
    }
    
    /**
     * 获取 LLM TopP
     */
    public float getAiAgentLlmTopP() {
        try {
            return (float) config.getJSONObject("aiAgentConfig").getDouble("llmTopP");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent llm topP: " + e.getMessage());
            return 0f;
        }
    }
    
    /**
     * 获取 LLM MaxToken
     */
    public int getAiAgentLlmMaxToken() {
        try {
            return config.getJSONObject("aiAgentConfig").getInt("llmMaxToken");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent llm maxToken: " + e.getMessage());
            return 0;
        }
    }
    
    /**
     * 获取 LLM HistoryDepth
     */
    public int getAiAgentLlmHistoryDepth() {
        try {
            return config.getJSONObject("aiAgentConfig").getInt("llmHistoryDepth");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent llm historyDepth: " + e.getMessage());
            return 10;
        }
    }
    
    /**
     * 获取 LLM Prompt
     */
    public String getAiAgentLlmPrompt() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("llmPrompt");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent llm prompt: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 TTS Vendor
     */
    public String getAiAgentTtsVendor() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("ttsVendor");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent tts vendor: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 TTS Model
     */
    public String getAiAgentTtsModel() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("ttsModel");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent tts model: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 TTS Voice
     */
    public String getAiAgentTtsVoice() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("ttsVoice");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent tts voice: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 Agent Start API 路径
     */
    public String getAiAgentApiStart() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("apiStart");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent api start: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 Agent Update API 路径
     */
    public String getAiAgentApiUpdate() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("apiUpdate");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent api update: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 Agent Stop API 路径
     */
    public String getAiAgentApiStop() {
        try {
            return config.getJSONObject("aiAgentConfig").getString("apiStop");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get ai agent api stop: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 Subtitle Start API 路径
     */
    public String getSubtitleApiStart() {
        try {
            return config.getJSONObject("subtitleConfig").getString("apiStart");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get subtitle api start: " + e.getMessage());
            return "";
        }
    }
    
    /**
     * 获取 Subtitle Stop API 路径
     */
    public String getSubtitleApiStop() {
        try {
            return config.getJSONObject("subtitleConfig").getString("apiStop");
        } catch (Exception e) {
            Log.e(TAG, "Failed to get subtitle api stop: " + e.getMessage());
            return "";
        }
    }
}
