package com.ding.rtc.apiexample.advanced.subtitle;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.View;

import java.util.ArrayList;
import java.util.List;

public class SubtitleView extends View {
    private final List<SubtitleItem> subtitles = new ArrayList<>();
    private long currentPosition = 0;
    private final Paint textPaint = new Paint();

    public SubtitleView(Context context) {
        super(context);
        init();
    }

    public SubtitleView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    private void init() {
        textPaint.setColor(Color.WHITE);
        textPaint.setTextSize(42);
        textPaint.setTextAlign(Paint.Align.CENTER);
        textPaint.setAntiAlias(true);
        setBackgroundColor(Color.parseColor("#80000000")); // 半透明背景
    }

    public void setSubtitles(List<SubtitleItem> subtitles) {
        this.subtitles.clear();
        this.subtitles.addAll(subtitles);
        invalidate();
    }

    public void setCurrentPosition(long positionMs) {
        this.currentPosition = positionMs;
        invalidate(); // 触发重绘
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        int yPos = 100; // 起始Y坐标
        for (SubtitleItem item : subtitles) {
            if (currentPosition >= item.getStartTimeMs() &&
                    currentPosition <= item.getEndTimeMs()) {

                canvas.drawText(
                        item.getText(),
                        getWidth() / 2f, // X居中
                        yPos,
                        textPaint
                );
                yPos += 60; // 行间距
            }
        }
    }
}
