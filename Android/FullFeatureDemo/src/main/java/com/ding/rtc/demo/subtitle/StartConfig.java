package com.ding.rtc.demo.subtitle;

public class StartConfig {
    public final String appId;
    public final String channelId;
    public final String env;
    public final int function; // 1 仅开启实时字幕 2 仅开启会后纪要 3 同时开启实时字幕和会后纪要

    public StartConfig(String appId, String channelId, String env, int function) {
        this.appId = appId;
        this.channelId = channelId;
        this.env = env;
        this.function = function;
    }
}
