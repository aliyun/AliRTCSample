package com.ding.rtc.apiexample.basic.videocall;

import android.os.Bundle;

import androidx.fragment.app.FragmentTransaction;

import android.view.Window;
import android.view.WindowManager;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.basic.videocall.R;

public abstract class CallBaseActivity extends AppCompatActivity implements ControlPanel.OnControlPanelListener {
    protected int mRemoteProfile = 5; // Max

    protected boolean mVideoMode = true;
    
    protected boolean mScreenMode = false;

    protected DingRtcEngine mRtcEngine = null;
    protected boolean mIsChannelJoined = false;
    protected boolean mFrontCamera = true;
    protected DingRtcEngine.DingRtcVideoCanvas mLocalCanvas;

    private ControlPanel mControlPanel;
    protected boolean mIsControlPanelShowed = false;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setScreenOnFlag();

        mControlPanel = ControlPanel.newInstance();
    }

    protected void showControlPanel() {
        if (!mIsControlPanelShowed) {
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            if (!mControlPanel.isAdded()) {
                ft.replace(R.id.fl_control_panel, mControlPanel)
                        .addToBackStack(null);
            }
            try {
                ft.setCustomAnimations(R.anim.panel_appear, R.anim.panel_disappear)
                        .show(mControlPanel)
                        .commitAllowingStateLoss();
            } catch (IllegalStateException e) {
                return;
            }
            mIsControlPanelShowed = true;
        }
    }

    protected void hideControlPanel() {
        if (mIsControlPanelShowed) {
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            ft.setCustomAnimations(R.anim.panel_appear, R.anim.panel_disappear)
                    .remove(mControlPanel)
                    .commitAllowingStateLoss();
            mIsControlPanelShowed = false;
        }
    }

    DingRtcEngine.DingRtcVideoStreamType ToVideoProfileType(int profile) {
        switch (profile) {
            case 0:
                return DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeNone;
            case 1:
                return DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeFHD;
            case 2:
                return DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeHD;
            case 3:
                return DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeSD;
            case 4:
                return DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeLD;
            default:
                return DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeSD;
        }
    }

    private void setScreenOnFlag() {
        final int keepScreenOnFlag =
                WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD |
                        WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED |
                        WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON |
                        WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON;
        Window w = getWindow();
        w.getAttributes().flags |= keepScreenOnFlag;
        w.addFlags(keepScreenOnFlag);
    }

    // 控制按钮回调处理
    // -------------------------- Control Panel Callbacks --------------------------
    @Override
    public void onMuteAudio()
    {
        mRtcEngine.muteLocalMic(true, DingRtcEngine.DingRtcMuteLocalAudioMode.DingRtcMuteAudioModeDefault);
    }

    @Override
    public void onUnmuteAudio()
    {
        mRtcEngine.muteLocalMic(false, DingRtcEngine.DingRtcMuteLocalAudioMode.DingRtcMuteAudioModeDefault);
    }

    @Override
    public void onSwitchCamera()
    {
        mRtcEngine.switchCamera();
        mFrontCamera = !mFrontCamera;
    }

    @Override
    public void onSettings() {
        hideControlPanel();
    }

    @Override
    public void onEndCall()
    {
        onBackPressed();
    }
}
