package com.ding.rtc.demo.aiagent;

import android.text.TextUtils;
import android.util.Log;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.ding.rtc.DingRtmAgentClient;
import com.ding.rtc.DingRtmAgentEventListener;
import com.ding.rtc.DingRtmClient;
import com.ding.rtc.DingRtmEventListener;
import com.ding.rtc.demo.BaseActivity;
import com.ding.rtc.demo.R;
import com.ding.rtc.demo.utils.ConfigManager;
import com.ding.rtc.http.HttpStack;
import com.ding.rtc.http.HttpStackResponse;

import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

public class AiAgent {
    public static final String AGENT_SESSION_ID = "ai_agent_default";
    private static final String TAG = "AiAgent";
    private static final String TASK_ID = "1001";

    private final Map<String, String> headers;
    private final DingRtmClient rtmClient;
    private final BaseActivity activity;
    private final AgentCallback callback;
    private String appId;
    private String channelId;
    private final String agentId;
    private String agentModel = "qwen-plus";
    private int currentVoiceRange = 0;
    private String currentPrompt;
    private String currentGreeting;
    private int currentChatMode = 1;
    private int currentInterruptMode = 1;
    private boolean started = false;

    public AiAgent(DingRtmClient rtmClient, BaseActivity activity, AgentChatAdapter adapter) {
        this.rtmClient = rtmClient;
        this.activity = activity;
        this.callback = (AgentCallback) activity;
        agentId = String.valueOf(new Random().nextInt(10000));

        headers = new HashMap<>();
        headers.put("Content-Type", "application/json");
        headers.put("env", "onertc");

        rtmClient.getAgentClient().setListener(new DingRtmAgentEventListener() {

            @Override
            public void onAgentUserReady(String sessionId, String agentId) {
                Log.d(TAG, "onAgentUserReady, sessionId: " + sessionId + ", agentId: " + agentId);
                activity.runOnUiThread(() -> {
                    Log.d(TAG, "agentId " + agentId);
                    rtmClient.getAgentClient().sendGreetingReady(sessionId, agentId);
                });
            }

            @Override
            public void onAgentStateChanged(String sessionId, String agentId, DingRtmAgentClient.AgentState state) {
                Log.d(TAG, "onAgentStateChanged, sessionId: " + sessionId + ", agentId: " + agentId + ", state: " + state);
                callback.onAgentStateChanged(agentId, state);
            }

            @Override
            public void onTranscriptionMessage(String sessionId, String agentId, DingRtmAgentClient.TranscriptionData data) {
                Log.d(TAG, "onTranscriptionMessage, sessionId: " + sessionId + ", agentId: " + agentId + ", data: " + data.text
                        + ", reasoningText: " + data.reasoningText + ", speakerType: " + data.speakerType
                        + ", speakerId: " + data.speakerId + ", roundId: " + data.roundId
                        + ", beginTime: " + data.beginTime
                        + ", customAttribute: " + data.customAttribute + ", end: " + data.end);
                activity.runOnUiThread(() -> adapter.addMsg(new AgentMsg(agentId, data)));
            }
        });

        rtmClient.setListener(new DingRtmEventListener() {
            @Override
            public void onJoinSessionResult(String sessionId, int result) {
                Log.d(TAG, "onJoinSessionResult, sessionId: " + sessionId + ", result : " + result);
            }

            @Override
            public void onSessionCreate(String sessionId) {
                Log.d(TAG, "onSessionCreate, sessionId: " + sessionId);
                if (AGENT_SESSION_ID.equals(sessionId)) {
                    activity.runOnUiThread(() -> {
                        int ret = rtmClient.joinSession(AGENT_SESSION_ID);
                        Log.d(TAG, "joinSession ret : " + ret);
                    });
                }
            }

            @Override
            public void onSessionRemoteUserJoin(String sessionId, String uid) {
                Log.d(TAG, "onSessionRemoteUserJoin, sessionId: " + sessionId + " uid : " + uid);
                if (AGENT_SESSION_ID.equals(sessionId) && agentId.equals(uid)) {
                    Log.d(TAG, "agent " + uid + " has already joined");
                }
            }
        });
    }

    public boolean isStarted() {
        return started;
    }

    public void updateAgentModel(int selectedPos) {
        agentModel = activity.getResources().getStringArray(R.array.ai_agent_model_value)[selectedPos];
    }

    public void updateAgentSettings(int voiceRange, String prompt, String greeting) {
        if (currentVoiceRange == voiceRange && prompt.equals(currentPrompt) && greeting.equals(currentGreeting)) {
            return;
        }
        currentVoiceRange = voiceRange;
        currentPrompt = prompt;
        currentGreeting = greeting;
    }

    public void updateChatSettings(int chatMode, int interruptMode) {
        if (currentChatMode == chatMode && currentInterruptMode == interruptMode) {
            return;
        }
        currentChatMode = chatMode;
        currentInterruptMode = interruptMode;

        if (started) {
            update();
        }
    }

    public void start(String appId, String channelId, String uid) {
        this.appId = appId;
        this.channelId = channelId;
        new Thread(() -> {
            String config = getStartAiAgentConfig(appId, channelId, uid);
            Log.d(TAG, "start config : " + config);
            ConfigManager configManager = ConfigManager.getInstance();
            String serverUrl = configManager.getOnlineAppServerUrl();
            String url = serverUrl + configManager.getAiAgentApiStart();

            HttpStackResponse response = HttpStack.doPost(url, headers, config.getBytes(StandardCharsets.UTF_8),5000,null);
            if(response != null) {
                Log.d(TAG, "response.code = " + response.code);
                Log.d(TAG, "response.getResultString = " + response.getResultString());
                if (response.code == 200) {
                    JSONObject object = JSON.parseObject(response.getResultString());
                    if (object.containsKey("code")) {
                        int errorCode = object.getInteger("code");
                        if (errorCode == 200) {
                            started = true;
                            activity.showToast("Agent 启动成功");
                        } else {
                            String reason = object.getString("data");
                            activity.showToast("Agent 启动失败: " + reason);
                        }
                    }
                } else {
                    activity.showToast("Agent 启动失败，code: " + response.code);
                }
                callback.onStartResult(started);
            }
        }, "agent-start").start();
    }

    public void stop() {
        new Thread(() -> {
            String config = getStopAiAgentConfig();
            Log.d(TAG, "stop config : " + config);
            ConfigManager configManager = ConfigManager.getInstance();
            String serverUrl = configManager.getOnlineAppServerUrl();
            String url = serverUrl + configManager.getAiAgentApiStop();
            HttpStackResponse response = HttpStack.doPost(url, headers, config.getBytes(StandardCharsets.UTF_8),5000,null);
            if(response != null) {
                Log.d(TAG, "stopAgent response.code = " + response.code);
                Log.d(TAG, "stopAgent response.getResultString = " + response.getResultString());
                if (response.code == 200) {
                    started = false;
                }
                callback.onStopResult(response.code == 200);
            }
        }, "agent-stop").start();
    }
    
    private void update() {
        new Thread(() -> {
            String config = getUpdateAiAgentConfig();
            Log.d(TAG, "update config : " + config);
            ConfigManager configManager = ConfigManager.getInstance();
            String serverUrl = configManager.getOnlineAppServerUrl();
            String url = serverUrl + configManager.getAiAgentApiUpdate();
            HttpStackResponse response = HttpStack.doPost(url, headers, config.getBytes(StandardCharsets.UTF_8),5000,null);
            if(response != null) {
                Log.d(TAG, "updateAgent response.code = " + response.code);
                Log.d(TAG, "updateAgent response.getResultString = " + response.getResultString());
                if (response.code == 200) {
                    activity.showToast("Agent 更新设置成功");
                } else {
                    activity.showToast("Agent 更新设置失败，code: " + response.code);
                }
            }
        }, "agent-update").start();
    }

    private String getStartAiAgentConfig(String appId, String channelId, String uid) {
        String[] uids = new String[1];
        uids[0] = uid;
        ConfigManager configManager = ConfigManager.getInstance();
        String templateId = configManager.getAiAgentDefaultTemplateId();
        StartConfig config = new StartConfig(appId, channelId, TASK_ID, templateId, agentId, uids);
        config.setModel(agentModel);
        config.setChatModel(currentChatMode);
        config.setInterruptMode(currentInterruptMode);
        config.setModel(agentModel);
        String voice = activity.getResources().getStringArray(R.array.agent_voice_range_value)[currentVoiceRange];
        config.setVoice(voice);
        if (!TextUtils.isEmpty(currentGreeting)) {
            config.setGreeting(currentGreeting);
        }
        return JSON.toJSONString(config);
    }

    private String getUpdateAiAgentConfig() {
        UpdateConfig config = new UpdateConfig(appId, channelId, TASK_ID);
        config.setModel(agentModel);
        config.setChatMode(currentChatMode);
        config.setInterruptMode(currentInterruptMode);
        if (!TextUtils.isEmpty(currentGreeting)) {
            config.setGreeting(currentGreeting);
        }
        if (!TextUtils.isEmpty(currentPrompt)) {
            config.setPrompt(currentPrompt);
        }
        String voice = activity.getResources().getStringArray(R.array.agent_voice_range_value)[currentVoiceRange];
        config.setVoice(voice);

        return JSON.toJSONString(config);
    }

    private String getStopAiAgentConfig() {
        StopConfig config = new StopConfig(appId, channelId, TASK_ID);
        return JSON.toJSONString(config);
    }

    public void sendPushToTalk(DingRtmAgentClient.AgentPushToTalkCmd cmd) {
        rtmClient.getAgentClient().sendPushToTalk(AGENT_SESSION_ID, agentId, cmd);
    }

    public interface AgentCallback {
        void onStartResult(boolean success);
        void onStopResult(boolean success);
        void onAgentStateChanged(String agentId, DingRtmAgentClient.AgentState state);
    }
}
