package com.ding.rtc.apiexample.advanced.audiovolumeindication;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.View;

import androidx.annotation.Nullable;

import java.util.LinkedList;
import java.util.List;

public class SoundWaveView extends View {

    private Paint paint;
    private List<Integer> amplitudes;
    private RectF rectF = new RectF();

    private int waveColor = Color.WHITE;
    private float barWidth = 10f;
    private float barSpacing = 6f;
    private float barCornerRadius = 4f;

    private int maxBars;
    private float viewHeight;

    public SoundWaveView(Context context) {
        super(context);
        init(null);
    }

    public SoundWaveView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        init(attrs);
    }

    public SoundWaveView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(attrs);
    }

    private void init(@Nullable AttributeSet attrs) {
        paint = new Paint();
        paint.setAntiAlias(true); // 抗锯齿

        amplitudes = new LinkedList<>();

        if (attrs != null) {
            TypedArray typedArray = getContext().obtainStyledAttributes(attrs, R.styleable.SoundWaveView);
            waveColor = typedArray.getColor(R.styleable.SoundWaveView_waveColor, Color.WHITE);
            barWidth = typedArray.getDimension(R.styleable.SoundWaveView_barWidth, 10f);
            barSpacing = typedArray.getDimension(R.styleable.SoundWaveView_barSpacing, 6f);
            barCornerRadius = typedArray.getDimension(R.styleable.SoundWaveView_barCornerRadius, 4f);
            typedArray.recycle();
        }

        paint.setColor(waveColor);
    }

    /**
     * 公共方法，用于从外部传入新的音量大小
     * @param amplitude 建议传入一个归一化后的值 (例如 0-100)
     */
    public void addAmplitude(int amplitude) {
        amplitudes.add(amplitude);

        // 如果声波条数量超过了视图所能容纳的最大数量，则移除最旧的一个
        if (amplitudes.size() > maxBars) {
            amplitudes.remove(0);
        }

        // 请求重绘视图
        invalidate();
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        viewHeight = h;
        // 计算视图宽度最多能容纳多少个声波条
        maxBars = (int) (w / (barWidth + barSpacing));
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (amplitudes.isEmpty()) {
            return;
        }

        // 从右向左绘制，实现新声波从右侧进入的效果
        float currentX = getWidth() - barWidth;

        for (int i = amplitudes.size() - 1; i >= 0; i--) {
            int amplitude = amplitudes.get(i);

            // 将音量大小（假设为0-100）映射到视图高度
            // 我们让声波从中心向上下扩展
            float barHeight = ((float) amplitude / 100f) * viewHeight;
            // 保证音量为0时也能看到一条细线
            barHeight = Math.max(barHeight, 2.0f);

            float top = (viewHeight - barHeight) / 2f;
            float bottom = top + barHeight;

            // 设置矩形位置并绘制
            rectF.set(currentX, top, currentX + barWidth, bottom);
            canvas.drawRoundRect(rectF, barCornerRadius, barCornerRadius, paint);

            // 移动到下一个声波条的绘制位置
            currentX -= (barWidth + barSpacing);

            // 如果已经超出视图左边界，则停止绘制
            if (currentX < 0) {
                break;
            }
        }
    }

    // --- 公共的 setter 方法，方便在代码中动态修改样式 ---
    public void setWaveColor(int waveColor) {
        this.waveColor = waveColor;
        paint.setColor(waveColor);
        invalidate();
    }

    public void setBarWidth(float barWidth) {
        this.barWidth = barWidth;
        invalidate();
    }

    public void setBarSpacing(float barSpacing) {
        this.barSpacing = barSpacing;
        invalidate();
    }

    public void setBarCornerRadius(float barCornerRadius) {
        this.barCornerRadius = barCornerRadius;
        invalidate();
    }
}


