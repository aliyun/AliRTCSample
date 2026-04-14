package com.ding.rtc.demo.annotation;

import static android.content.Context.WINDOW_SERVICE;

import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.drawable.Drawable;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;

import androidx.core.graphics.drawable.DrawableCompat;

import com.ding.rtc.api.DingRtcWhiteBoardTypes;
import com.ding.rtc.demo.BaseActivity;
import com.ding.rtc.demo.R;

public class AnnotationToolBar {

    private final WindowManager windowManager;
    private final WindowManager.LayoutParams toolBarParam;
    private final View toolBarRootView;
    private final ImageView pencilBtn;
    private final ImageView eraserBtn;
    private final ImageView clearBtn;
    private final ImageView snapshotBtn;
    private final Drawable pencilDrawable;
    private final Drawable eraserDrawable;
    private boolean isShowing = false;
    private DingRtcWhiteBoardTypes.DingRtcWBToolType toolType = DingRtcWhiteBoardTypes.DingRtcWBToolType.NONE;
    private final AnnotationToolBarCallback callback;
    private String currentUid;

    public AnnotationToolBar(BaseActivity activity) {
        callback = (AnnotationToolBarCallback) activity;
        windowManager = (WindowManager) activity.getSystemService(WINDOW_SERVICE);

        toolBarRootView = LayoutInflater.from(activity).inflate(R.layout.annotation_tool_bar, null);

        pencilBtn = toolBarRootView.findViewById(R.id.pencil);
        pencilDrawable = DrawableCompat.wrap(pencilBtn.getDrawable());
        pencilBtn.setOnClickListener(v -> {
            if (toolType != DingRtcWhiteBoardTypes.DingRtcWBToolType.PATH) {
                resetOldSelectedBtn(toolType);
                toolType = DingRtcWhiteBoardTypes.DingRtcWBToolType.PATH;
                DrawableCompat.setTint(pencilDrawable, Color.parseColor("#0899F9"));
                callback.onToolTypeChanged(toolType);
            }
        });

        eraserBtn = toolBarRootView.findViewById(R.id.eraser);
        eraserDrawable = DrawableCompat.wrap(eraserBtn.getDrawable());
        eraserBtn.setOnClickListener(v -> {
            if (toolType != DingRtcWhiteBoardTypes.DingRtcWBToolType.ERASER) {
                resetOldSelectedBtn(toolType);
                toolType = DingRtcWhiteBoardTypes.DingRtcWBToolType.ERASER;
                DrawableCompat.setTint(eraserDrawable, Color.parseColor("#0899F9"));
                callback.onToolTypeChanged(toolType);
            }
        });

        clearBtn = toolBarRootView.findViewById(R.id.clear);
        clearBtn.setOnClickListener(v -> callback.onClickClear(currentUid));

        snapshotBtn = toolBarRootView.findViewById(R.id.snapshot);
        snapshotBtn.setOnClickListener(v -> callback.onClickSnapshot(currentUid));

        toolBarParam = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY, // 对于 API 26+
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
                PixelFormat.TRANSLUCENT);
        toolBarParam.flags |= WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL;
        toolBarParam.gravity = Gravity.TOP | Gravity.START;
    }

    public boolean isShowing() {
        return isShowing;
    }

    public void show(View container, View anchor) {
        if (isShowing) {
            windowManager.removeView(toolBarRootView);
        }

        int[] location = new int[2];
        container.getLocationInWindow(location);

        toolBarParam.width = container.getWidth();
        toolBarParam.x = location[0];
        toolBarParam.y = anchor.getTop() - 20;
        windowManager.addView(toolBarRootView, toolBarParam);
        isShowing = true;
    }

    public void dismiss() {
        if (isShowing) {
            windowManager.removeView(toolBarRootView);
            isShowing = false;
        }
        currentUid = "";
    }

    private void resetOldSelectedBtn(DingRtcWhiteBoardTypes.DingRtcWBToolType type) {
        if (type == DingRtcWhiteBoardTypes.DingRtcWBToolType.PATH) {
            DrawableCompat.setTint(pencilDrawable, Color.WHITE);
        } else if (type == DingRtcWhiteBoardTypes.DingRtcWBToolType.ERASER) {
            DrawableCompat.setTint(eraserDrawable, Color.WHITE);
        }
    }

    public void setCurrentUid(String uid) {
        currentUid = uid;
    }

    public String getCurrentUid() {
        return currentUid;
    }

    public interface AnnotationToolBarCallback {

        void onToolTypeChanged(DingRtcWhiteBoardTypes.DingRtcWBToolType toolType);

        void onClickSnapshot(String uid);

        void onClickClear(String uid);
    }

}