package com.ding.rtc.demo.subtitle;

import android.text.TextUtils;
import android.util.Log;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.ding.rtc.DingRtcEngineSubtitleTypes;
import com.ding.rtc.DingRtcEngineSubtitleTypes.DingRtcSubtitleMessage;
import com.ding.rtc.demo.utils.ConfigManager;
import com.ding.rtc.demo.utils.LimitedSizeLinkedList;
import com.ding.rtc.demo.utils.MD5Util;
import com.ding.rtc.http.HttpStack;
import com.ding.rtc.http.HttpStackResponse;

import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;

public class SubtitleManager {
    private static final String TAG = "Subtitle";
    public Map<String, String> speakLanguagesMap = new HashMap<>(); // <en, cn>
    public Map<String, String> translateLanguagesMap = new HashMap<>(); // <en, cn>
    private final Map<String, Map<String, LimitedSizeLinkedList<DingRtcSubtitleMessage>>> userTranslateLanguagesMap = new HashMap<>(); // <uid, Map<lan, List<Subtitle>>>
    private boolean started = false;
    private String taskId;
    private String appId;
    private String channelId;
    private int serviceType;

    private static class Holder {
        private static final SubtitleManager INSTANCE = new SubtitleManager();
    }

    private SubtitleManager() {
        speakLanguagesMap.put("multilingual", "自动识别(多语种)");
        speakLanguagesMap.put("source", "原语言(不翻译)");
        speakLanguagesMap.put("cn", "中文");
        speakLanguagesMap.put("yue", "粤语");
        speakLanguagesMap.put("en", "英文");
        speakLanguagesMap.put("ja", "日语");
        speakLanguagesMap.put("ko", "韩语");
        speakLanguagesMap.put("de", "德语");
        speakLanguagesMap.put("fr", "法语");
        speakLanguagesMap.put("ru", "俄语");
        speakLanguagesMap.put("es", "西班牙语");
        speakLanguagesMap.put("vi", "越南语");
        speakLanguagesMap.put("it", "意大利语");
        speakLanguagesMap.put("sv", "瑞典语");
        speakLanguagesMap.put("cs", "捷克语");
        speakLanguagesMap.put("pl", "波兰语");
        speakLanguagesMap.put("th", "泰语");
        speakLanguagesMap.put("fi", "芬兰语");
        speakLanguagesMap.put("hi", "印地语");
        speakLanguagesMap.put("id", "印尼语(印度尼西亚)");
        speakLanguagesMap.put("pt", "葡萄牙语");
        speakLanguagesMap.put("ar", "阿拉伯语");
        speakLanguagesMap.put("fil", "菲律宾语");
        speakLanguagesMap.put("ms", "马来语");
        speakLanguagesMap.put("tr", "土耳其语");
        speakLanguagesMap.put("hu", "匈牙利语");
        speakLanguagesMap.put("lo", "老挝语");
        speakLanguagesMap.put("pt-br", "葡萄牙语(巴西)");
        speakLanguagesMap.put("es-ar", "西班牙语(阿根廷)");
        speakLanguagesMap.put("es-mx", "西班牙语(墨西哥)");

        translateLanguagesMap.put("source", "不翻译");
        translateLanguagesMap.put("cn", "中文");
        translateLanguagesMap.put("en", "英文");
        translateLanguagesMap.put("ja", "日文");
        translateLanguagesMap.put("ko", "韩文");
        translateLanguagesMap.put("th", "泰语");
        translateLanguagesMap.put("id", "印尼语");
        translateLanguagesMap.put("fr", "法语");
        translateLanguagesMap.put("es", "西班牙语");
        translateLanguagesMap.put("pt", "葡萄牙语");
        translateLanguagesMap.put("vi", "越南语");
        translateLanguagesMap.put("ru", "俄语");
        translateLanguagesMap.put("de", "德语");
        translateLanguagesMap.put("it", "意大利语");
        translateLanguagesMap.put("ar", "阿拉伯语");
        translateLanguagesMap.put("pl", "波兰语");
        translateLanguagesMap.put("fil", "菲律宾语");
        translateLanguagesMap.put("ms", "马来语");
        translateLanguagesMap.put("hi", "印地语");
        translateLanguagesMap.put("sv", "瑞典语");
        translateLanguagesMap.put("fi", "芬兰语");
        translateLanguagesMap.put("cs", "捷克语");
        translateLanguagesMap.put("tr", "土耳其语");
        translateLanguagesMap.put("hu", "匈牙利语");
        translateLanguagesMap.put("lo", "老挝语");
    }

    public static SubtitleManager getInstance() {
        return SubtitleManager.Holder.INSTANCE;
    }

    public String[] getCnSpeakLanguages(String[] enLanguages) {
        String[] cnLanguages = new String[enLanguages.length];
        for (int i = 0; i < enLanguages.length; i++) {
            String enLan = enLanguages[i];
            String cnLan = speakLanguagesMap.get(enLan);
            if (TextUtils.isEmpty(cnLan)) {
                cnLanguages[i] = enLan;
            } else {
                cnLanguages[i] = cnLan;
            }
        }
        return cnLanguages;
    }

    public String getCnTranslateLanguage(String enLanguage) {
        String cnLan = translateLanguagesMap.get(enLanguage);
        if (TextUtils.isEmpty(cnLan)) {
            return enLanguage;
        } else {
            return cnLan;
        }
    }


    public String[] getCnTranslateLanguages(String[] enLanguages) {
        String[] cnLanguages = new String[enLanguages.length];
        for (int i = 0; i < enLanguages.length; i++) {
            String cnLan = translateLanguagesMap.get(enLanguages[i]);
            if (TextUtils.isEmpty(cnLan)) {
                cnLanguages[i] = enLanguages[i];
            } else {
                cnLanguages[i] = cnLan;
            }
        }
        return cnLanguages;
    }

    public void onSubtitleMessage(DingRtcEngineSubtitleTypes.DingRtcSubtitleMessage message) {
        String uid = message.userId;
        String lan = message.language;
        if (TextUtils.isEmpty(uid) || TextUtils.isEmpty(lan)) {
            return;
        }

        Map<String, LimitedSizeLinkedList<DingRtcSubtitleMessage>> lanSubTitleMap = userTranslateLanguagesMap.get(uid);
        if (lanSubTitleMap == null) {
            lanSubTitleMap = new HashMap<>();
            userTranslateLanguagesMap.put(uid, lanSubTitleMap);
        }

        LimitedSizeLinkedList<DingRtcSubtitleMessage> subtitles = lanSubTitleMap.get(lan);
        if (subtitles == null) {
            subtitles = new LimitedSizeLinkedList<>(4);
            subtitles.add(message);
            lanSubTitleMap.put(lan, subtitles);
        } else {
            int sentenceIndex = -1;
            for (int i = 0; i < subtitles.size(); i++) {
                if (subtitles.get(i).sentenceIndex == message.sentenceIndex) {
                    sentenceIndex = i;
                    break;
                }
            }
            if (sentenceIndex != -1) {
                subtitles.set(sentenceIndex, message);
            } else {
                subtitles.add(message);
            }
        }
    }

    public Map<String, Map<String, LimitedSizeLinkedList<DingRtcSubtitleMessage>>> getSubtitles() {
        return userTranslateLanguagesMap;
    }

    public void clear() {
        userTranslateLanguagesMap.clear();
    }

    public boolean hasStartService() {
        return started;
    }

    public void startService(String channelId, String appId, int serviceType, SubtitleStartCallback callback) {
        this.channelId = channelId;
        this.appId = appId;
        this.serviceType = serviceType;
        Map<String, String> headers = new HashMap<>();
        headers.put("Content-Type", "application/json");
        headers.put("DingRTC-Signature", MD5Util.md5(appId));

        started = true;

        ConfigManager configManager = ConfigManager.getInstance();
        String serverUrl = configManager.getOnlineAppServerUrl();
        String url = serverUrl + configManager.getSubtitleApiStart();
        StartConfig config = new StartConfig(appId, channelId, "onertc", serviceType);
        String configJsonStr = JSON.toJSONString(config);
        new Thread(new Runnable() {
            @Override
            public void run() {
                HttpStackResponse response = HttpStack.doPost(url, headers, configJsonStr.getBytes(StandardCharsets.UTF_8),5000,null);
                if(response != null) {
                    Log.d(TAG, "response.code = " + response.code);
                    Log.d(TAG, "response.getResultString = " + response.getResultString());
                    if (response.code == 200) {
                        JSONObject object = JSON.parseObject(response.getResultString());
                        if (object.containsKey("code")) {
                            int errorCode = object.getInteger("code");
                            if (errorCode == 200) {
                                SubtitleResponseData data = object.getObject("data", SubtitleResponseData.class);
                                taskId = data.taskId;
                                callback.onSubtitleServiceStartResult(true, taskId, "");
                            } else {
                                String reason = object.getString("data");
                                callback.onSubtitleServiceStartResult(false, "", reason);
                            }
                        }
                    } else {
                        callback.onSubtitleServiceStartResult(false, "", "errorCode: " + response.code);
                    }
                }
            }
        }, "subtitle-start").start();

    }

    public void stopService(SubtitleStopCallback callback) {
        Map<String, String> headers = new HashMap<>();
        headers.put("Content-Type", "application/json");
        headers.put("DingRTC-Signature", MD5Util.md5(appId));

        started = false;

        ConfigManager configManager = ConfigManager.getInstance();
        String serverUrl = configManager.getOnlineAppServerUrl();
        String url = serverUrl + configManager.getSubtitleApiStop();
        StopConfig config = new StopConfig(appId, channelId, "onertc", serviceType, taskId);
        String configJsonStr = JSON.toJSONString(config);
        new Thread(new Runnable() {
            @Override
            public void run() {
                HttpStackResponse response = HttpStack.doPost(url, headers, configJsonStr.getBytes(StandardCharsets.UTF_8),5000,null);
                if(response != null) {
                    Log.d(TAG, "response.code = " + response.code);
                    Log.d(TAG, "response.getResultString = " + response.getResultString());
                    if (response.code == 200) {
                        JSONObject object = JSON.parseObject(response.getResultString());
                        if (object.containsKey("code")) {
                            int errorCode = object.getInteger("code");
                            if (errorCode == 200) {
                                callback.onSubtitleServiceStopResult(true, "");
                            } else {
                                String reason = object.getString("data");
                                callback.onSubtitleServiceStopResult(false, reason);
                            }
                        }
                    } else {
                        callback.onSubtitleServiceStopResult(false, "errorCode: " + response.code);
                    }
                }
            }
        }, "subtitle-start").start();
    }

    public void reset() {
        started = false;
    }

    public interface SubtitleStartCallback {
        void onSubtitleServiceStartResult(boolean success, String taskId, String reason);

    }

    public interface SubtitleStopCallback {
        void onSubtitleServiceStopResult(boolean success, String reason);
    }
}
