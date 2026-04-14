package com.ding.rtc.demo.annotation;

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineAnnotation;
import com.ding.rtc.DingRtcEngineWhiteboardManager;
import com.ding.rtc.DingRtcWhiteboardView;
import com.ding.rtc.api.DingRtcWhiteBoardTypes;

public class AnnotationInfo {
    public DingRtcWhiteBoardTypes.DingRtcAnnoType type;
    public String userId;
    public boolean isStarted;
    private boolean isShowed;

    private DingRtcEngineWhiteboardManager mManager;
    private DingRtcEngineAnnotation mAnnotation;
    private DingRtcWhiteboardView mView;

    public void init(DingRtcEngineWhiteboardManager manager, DingRtcWhiteBoardTypes.DingRtcAnnoType type, String uid) {
        this.type = type;
        this.userId = uid;
        this.isStarted = false;

        mManager = manager;
    }

    public void createAnnotation() {
        if (mManager != null) {
            DingRtcWhiteBoardTypes.DingRtcAnnoID annoID = new DingRtcWhiteBoardTypes.DingRtcAnnoID(this.type, this.userId);
            DingRtcWhiteBoardTypes.DingRtcAnnoConfig config = new DingRtcWhiteBoardTypes.DingRtcAnnoConfig(this.type);
            mAnnotation = mManager.createAnnotation(annoID, config);
            if (mAnnotation == null) {
                return;
            }
            mAnnotation.setToolType(DingRtcWhiteBoardTypes.DingRtcWBToolType.PATH);
            mAnnotation.setLineWidth(5);
            mAnnotation.setColor(new DingRtcWhiteBoardTypes.DingRtcWBColor(0.f, 0.0f, 1.0f, 1.0f));
        }
    }

    public void open(DingRtcWhiteboardView view, DingRtcWhiteBoardTypes.DingRtcWBScalingMode scalingMode, DingRtcEngine.DingRtcRenderMirrorMode mirrorMode) {
        if (mAnnotation == null) {
            return;
        }
        if (!isShowed) {
            mAnnotation.open(view);
            mAnnotation.setScalingMode(scalingMode);
            mAnnotation.setWindowMirrorMode(mirrorMode);
            isShowed = true;
        }
        mView = view;
    }

    public void close() {
        if (mAnnotation == null) {
            return;
        }
        if (isShowed) {
            mAnnotation.close();
            isShowed = false;
        }
    }

    public void stop() {
        if (mAnnotation == null) {
            return;
        }
        if (isStarted) {
            mAnnotation.stop();
            isShowed = false;
            isStarted = false;
        }
    }

    public void setToolType(DingRtcWhiteBoardTypes.DingRtcWBToolType type) {
        if (mAnnotation == null) {
            return;
        }
        mAnnotation.setToolType(type);
    }

    public void snapshot(String dir) {
        if (mAnnotation == null) {
            return;
        }
        mAnnotation.snapshot(dir);
    }

    public void clear() {
        if (mAnnotation == null) {
            return;
        }
        mAnnotation.clearContents(new DingRtcWhiteBoardTypes.DingRtcWBClearParam(true,
                DingRtcWhiteBoardTypes.DingRtcWBClearMode.SELF));
    }


    public DingRtcWhiteboardView getWhiteboardView() {
        return mView;
    }
}
