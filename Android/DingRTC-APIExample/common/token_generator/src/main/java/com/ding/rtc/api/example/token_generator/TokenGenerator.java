package com.ding.rtc.api.example.token_generator;

import com.ding.rtc.api.example.token_generator.model.AppToken;
import com.ding.rtc.api.example.token_generator.model.Service;

public class TokenGenerator {
    public static final String APP_ID = #APP_ID#; // 应用ID
    public static final String APP_KEY = #APP_KEY#; // 应用KEY

    public static String generateAppToken(String channelId, String userId) {
        int expiredTs = (int) (System.currentTimeMillis() / 1000) + 12 * 60 * 60;
        AppToken appToken = new AppToken(APP_ID, APP_KEY, expiredTs);
        final Service service = new Service(channelId, userId);
        appToken.addService(service);
        String appTokenStr = "";
        try {
            appTokenStr = appToken.buildTokenString();
        } catch (Throwable t) {
            t.printStackTrace();
        }

        return appTokenStr;
    }
}
