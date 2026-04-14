package com.ding.rtc.demo.ui;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;

import androidx.annotation.Nullable;
import androidx.appcompat.widget.AppCompatTextView;
import androidx.core.content.res.ResourcesCompat;

import com.ding.rtc.demo.R;

public class DrawableTextView extends AppCompatTextView {
    private int drawableWidth, drawableHeight;
    private Drawable drawableRight = null;
    private Drawable drawableLeft = null;
    private Drawable drawableTop = null;
    private Drawable drawableBottom = null;
    public DrawableTextView(Context context) {
        this(context, null);
    }

    public DrawableTextView(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public DrawableTextView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.DrawableTextView);
        int count = typedArray.getIndexCount();

        for(int i = 0; i < count; i++){
            int attr = typedArray.getIndex(i);
            if (attr == R.styleable.DrawableTextView_drawableRight) {
                drawableRight = typedArray.getDrawable(attr);
            } else if (attr == R.styleable.DrawableTextView_drawableLeft) {
                drawableLeft = typedArray.getDrawable(attr);
            } else if (attr == R.styleable.DrawableTextView_drawableTop) {
                drawableTop = typedArray.getDrawable(attr);
            } else if (attr == R.styleable.DrawableTextView_drawableBottom) {
                drawableBottom = typedArray.getDrawable(attr);
            } else if (attr == R.styleable.DrawableTextView_drawableWidth) {
                drawableWidth = typedArray.getDimensionPixelSize(attr, 0);
            } else if (attr == R.styleable.DrawableTextView_drawableHeight) {
                drawableHeight = typedArray.getDimensionPixelSize(attr,0);
            }
        }
        if(null != drawableLeft){
            drawableLeft.setBounds(0,0, drawableWidth, drawableHeight);
        }
        if(null != drawableRight){
            drawableRight.setBounds(0,0, drawableWidth, drawableHeight);
        }
        if(null != drawableTop){
            drawableTop.setBounds(0,0, drawableWidth, drawableHeight);
        }
        if(null != drawableBottom){
            drawableBottom.setBounds(0,0, drawableWidth, drawableHeight);
        }
        setCompoundDrawables(drawableLeft, drawableTop, drawableRight, drawableBottom);
    }

    public void setDrawableTop(Context context,int res) {
        Resources resources = context.getResources();
        Drawable top =  ResourcesCompat.getDrawable(resources,res,null);
        if(top != null) {
            top.setBounds(0,0, drawableWidth, drawableHeight);
            setCompoundDrawables(drawableLeft, top, drawableRight, drawableBottom);
        }
    }

}
