package com.ding.rtc.apiexample.advanced.subtitle;

public class SubtitleItem {
    private final String text;
    private final long startTimeMs;
    private final long endTimeMs;

    public SubtitleItem(String text, long startTimeMs, long endTimeMs) {
        this.text = text;
        this.startTimeMs = startTimeMs;
        this.endTimeMs = endTimeMs;
    }

    // Getters
    public String getText() { return text; }
    public long getStartTimeMs() { return startTimeMs; }
    public long getEndTimeMs() { return endTimeMs; }
}
