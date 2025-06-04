package com.ding.rtc.apiexample.basic.videocall;

import static com.ding.rtc.DingRtcEngine.*;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.widget.Toolbar;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.basic.videocall.R;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class CallActivity extends CallBaseActivity implements EventHandler {
    private static final String TAG = "VideoCall";

    private static final String INTENT_MEETING_USER_ID = "meeting_user_id";
    private static final String INTENT_MEETING_TOKEN = "meeting_token";
    private static final String INTENT_MEETING_USER_NAME = "meeting_user_name";
    private static final String INTENT_MEETING_APP_ID = "meeting_app_id";
    private static final String INTENT_MEETING_CHANNEL_ID = "meeting_channel_id";
    private static final String INTENT_MEETING_GSLB = "meeting_gslb";
    private static final int SCREEN_CAPTURE_PERMISSION_CODE = 1002;

    private String mSelfUid = "";
    private Set<String> mSubedUids = new HashSet<>();

    private DingRtcEngine mRtcEngine;
    private final DingEngineCallback mRtcCallback = new DingEngineCallback();

    public static void launch(Context context, String appId, String channelId, String userId,
                              String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, CallActivity.class);
        intent.putExtra(INTENT_MEETING_USER_ID, userId);
        intent.putExtra(INTENT_MEETING_USER_NAME, userName);
        intent.putExtra(INTENT_MEETING_TOKEN, token);
        intent.putExtra(INTENT_MEETING_APP_ID, appId);
        intent.putExtra(INTENT_MEETING_CHANNEL_ID, channelId);
        context.startActivity(intent);
    }

    private void registerEventHandler(EventHandler handler) { mRtcCallback.addHandler(handler); }
    private void removeEventHandler(EventHandler handler) { mRtcCallback.removeHandler(handler); }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.subscribeAllRemoteAudioStreams(true);
            mRtcEngine.subscribeAllRemoteVideoStreams(true);
            mRtcEngine.setRemoteDefaultVideoStreamType(DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeFHD);
            mRtcEngine.setRtcEngineEventListener(mRtcCallback);
        }
    }

    @Override
    public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {
        Log.i(TAG, "onJoinChannelResult, result=" + result);
        if (result == 0) {
            mIsChannelJoined = true;
            Toast.makeText(CallActivity.this, "onChannelJoinConfirm success", Toast.LENGTH_LONG).show();
            // 启动本地视频
            if (mVideoMode) {
                mRtcEngine.publishLocalVideoStream(true);
            }
            // 启动本地音频
            mRtcEngine.publishLocalAudioStream(true);
            // 启动屏幕共享
            if (mScreenMode) {
                // 内部授权（推荐）
                mRtcEngine.startScreenShare();
                // 外部授权
                // Intent intent = new Intent(this, ScreenCapturePermissionActivity.class);
                // startActivityForResult(intent, SCREEN_CAPTURE_PERMISSION_CODE);
            }
        } else {
            mIsChannelJoined = false;
            Toast.makeText(CallActivity.this, "onChannelJoinConfirm result=" + result, Toast.LENGTH_LONG).show();
        }
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == SCREEN_CAPTURE_PERMISSION_CODE) {
            if (resultCode == Activity.RESULT_OK && data != null) {
                mRtcEngine.startScreenShare(data, DingRtcEngine.DingRtcScreenShareMode.DingRtcScreenShareAllMode);
            }
        }
    }

    @Override
    public void onLeaveChannelResult(int result, DingRtcEngine.DingRtcStats stats) {
        Log.i(TAG, "onLeaveChannelResult, result=" + result);
        mIsChannelJoined = false;
        Toast.makeText(CallActivity.this, "onChannelLeaveIndication result=" + result, Toast.LENGTH_LONG).show();
        finish();
    }

    @Override
    public void onRemoteUserOnLineNotify(String uid, int elapsed) {
        Log.i(TAG, "onRemoteUserOnLineNotify, uid=" + uid);
        Toast.makeText(CallActivity.this, "onRemoteUserOnLineNotify userId=" + uid, Toast.LENGTH_LONG).show();
    }

    @Override
    public void onRemoteUserOffLineNotify(String uid, DingRtcEngine.DingRtcUserOfflineReason reason) {
        Log.i(TAG, "onRemoteUserOffLineNotify, uid=" + uid);
        Toast.makeText(CallActivity.this, "onUserLeaveIndication uid=" + uid + ", reason=" + reason, Toast.LENGTH_LONG).show();
        // 取消订阅此用户视频
        stopUserVideo(uid);
        stopUserScreen(uid);
    }

    @Override
    public void onRemoteTrackAvailableNotify(String uid, DingRtcEngine.DingRtcAudioTrack audioTrack, DingRtcEngine.DingRtcVideoTrack videoTrack) {
        Log.d(TAG, "onRemoteTrackAvailableNotify, uid :" + uid + " audioTrack : " + audioTrack + ", videoTrack : " + videoTrack);
        boolean isVideoStart = false;
        boolean isScreenStart = false;
        boolean isAudioStart = false;
        if (videoTrack == DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera) {
            isVideoStart = true;
        } else if (videoTrack == DingRtcVideoTrack.DingRtcVideoTrackScreen) {
            isScreenStart = true;
        } else if (videoTrack == DingRtcVideoTrack.DingRtcVideoTrackBoth) {
            isVideoStart = true;
            isScreenStart = true;
        }
        if (audioTrack == DingRtcAudioTrack.DingRtcAudioTrackMic) {
            isAudioStart = true;
        }
        if (isVideoStart) {
            onUserVideoStart(uid, DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeFHD);
        } else {
            onUserVideoStop(uid);
        }
    }

    // 为订阅用户信息，因为总共有5个视图，所以最多能同时订阅5个用户视频
    static class VideoUserInfo {
        String userId;
        DingRtcEngine.DingRtcVideoStreamType maxProfile;
    }

    private Map<String, VideoUserInfo> mUnsubscribedVideoUsers = new HashMap();

    // 用于保存用户视图信息，
    class UserViewInfo {
        String userId;
        DingRtcEngine.DingRtcVideoCanvas canvas;
        ViewGroup renderContainer;
        TextView textView; // 打印用户ID信息
        boolean isFree = true; // 此视图是否空闲
        boolean isScreen = false; // 此视图是否正在显示桌面共享
        DingRtcEngine.DingRtcVideoStreamType maxProfile; // 此用户发布的最大视频能力
        DingRtcEngine.DingRtcVideoStreamType subProfile; // 此用户的当前订阅能力

        void initView(int index, DingRtcEngine.DingRtcVideoCanvas canvas, ViewGroup renderContainer, TextView tv) {
            this.canvas = canvas;
            this.textView = tv;
            this.renderContainer = renderContainer;
            // 设置长按小图时将此小图的用户和大图用户交换视图
            ((View) (this.canvas.view)).setOnLongClickListener(vv -> {
                switchToLargeView(index);
                return true;
            });
        }

        void setVisible(boolean visible) {
            renderContainer.setVisibility(visible ? View.VISIBLE : View.GONE);
            textView.setVisibility(visible ? View.VISIBLE : View.GONE);
        }

        void setUser(String userId) {
            this.userId = userId;
            textView.setText("" + userId);
        }
    }

    // 视图数组，总共5个视频视图，1个大图4个小图，0为大图，其他为小图
    private int mUserViewCount = 5;
    UserViewInfo[] mUserViewArray = new UserViewInfo[mUserViewCount];

    {
        for (int i = 0; i < mUserViewCount; i++) {
            mUserViewArray[i] = new UserViewInfo();
        }
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_call);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        mSelfUid = getIntent().getStringExtra(INTENT_MEETING_USER_ID);
        // 设置点击view时显示或隐藏控制按钮
        View rootView = findViewById(android.R.id.content);
        rootView.setOnClickListener(v -> {
            if (mIsControlPanelShowed) {
                hideControlPanel();
            } else {
                showControlPanel();
            }
        });

        try {
            this.getSupportActionBar().hide();
        } catch (NullPointerException e) {
        }

        registerEventHandler(this);
        initDingEngine();

        DingRtcEngine.DingRtcVideoCanvas cameraCanvas = new DingRtcEngine.DingRtcVideoCanvas();
        SurfaceView cameraView = mRtcEngine.createRenderSurfaceView(this);
        if (cameraView == null) {
            Toast.makeText(getApplicationContext(), "创建画布失败", Toast.LENGTH_SHORT).show();
        } else {
            cameraView.setZOrderMediaOverlay(false);
            cameraCanvas.view = cameraView;
        }

        cameraCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeAuto;
        // 配置大图的视图参数
        mUserViewArray[0].initView(0,
                cameraCanvas,
                findViewById(R.id.large_size_view),
                findViewById(R.id.tv_large_view_user));
        ((View) (mUserViewArray[0].canvas.view)).setOnLongClickListener(null);
        // 设置点击大图时显示或隐藏控制按钮
        ((View) (mUserViewArray[0].canvas.view)).setOnClickListener(v -> {
            if (mIsControlPanelShowed) {
                hideControlPanel();
            } else {
                showControlPanel();
            }
        });

        // 配置左上角小图的视图参数
        DingRtcEngine.DingRtcVideoCanvas cameraCanvas1 = new DingRtcEngine.DingRtcVideoCanvas();
        cameraCanvas1.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeAuto;
        SurfaceView cameraView1 = mRtcEngine.createRenderSurfaceView(this);
        if (cameraView1 == null) {
            Toast.makeText(getApplicationContext(), "创建画布失败", Toast.LENGTH_SHORT).show();
        } else {
            cameraView1.setZOrderMediaOverlay(true);
            cameraCanvas1.view = cameraView1;
        }
        mUserViewArray[1].initView(1,
                cameraCanvas1,
                findViewById(R.id.small_size_view_righttop),
                findViewById(R.id.tv_small_view_righttop_user));

        // 配置右上角小图的视图参数
        DingRtcEngine.DingRtcVideoCanvas cameraCanvas2 = new DingRtcEngine.DingRtcVideoCanvas();
        cameraCanvas2.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeAuto;
        SurfaceView cameraView2 = mRtcEngine.createRenderSurfaceView(this);
        if (cameraView2 == null) {
            Toast.makeText(getApplicationContext(), "创建画布失败", Toast.LENGTH_SHORT).show();
        } else {
            cameraView2.setZOrderMediaOverlay(true);
            cameraCanvas2.view = cameraView2;
        }
        mUserViewArray[2].initView(2,
                cameraCanvas2,
                findViewById(R.id.small_size_view_lefttop),
                findViewById(R.id.tv_small_view_lefttop_user));

        // 配置左下角小图的视图参数
        DingRtcEngine.DingRtcVideoCanvas cameraCanvas3 = new DingRtcEngine.DingRtcVideoCanvas();
        cameraCanvas3.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeAuto;
        SurfaceView cameraView3 = mRtcEngine.createRenderSurfaceView(this);
        if (cameraView3 == null) {
            Toast.makeText(getApplicationContext(), "创建画布失败", Toast.LENGTH_SHORT).show();
        } else {
            cameraView3.setZOrderMediaOverlay(true);
            cameraCanvas3.view = cameraView3;
        }
        mUserViewArray[3].initView(3,
                cameraCanvas3,
                findViewById(R.id.small_size_view_leftbottom),
                findViewById(R.id.tv_small_view_leftbottom_user));

        // 配置右下角小图的视图参数，此视图一般用于显示本地用户视频
        DingRtcEngine.DingRtcVideoCanvas cameraCanvas4 = new DingRtcEngine.DingRtcVideoCanvas();
        cameraCanvas4.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeAuto;
        SurfaceView cameraView4 = mRtcEngine.createRenderSurfaceView(this);
        if (cameraView4 == null) {
            Toast.makeText(getApplicationContext(), "创建画布失败", Toast.LENGTH_SHORT).show();
        } else {
            cameraView4.setZOrderMediaOverlay(true);
            cameraCanvas4.view = cameraView4;
        }
        mUserViewArray[4].initView(4,
                cameraCanvas4,
                findViewById(R.id.small_size_view_rightbottom),
                findViewById(R.id.tv_small_view_rightbottom_user));

       // 启动视频预览，并且显示到大图
       if (mVideoMode) {
           mUserViewArray[0].isFree = false;
           mUserViewArray[0].isScreen = false;
           mUserViewArray[0].setUser(mSelfUid);
           updateLocalVideoRender(mUserViewArray[0].canvas, false);
           int result = mRtcEngine.startPreview();
           mUserViewArray[0].renderContainer.addView((SurfaceView) mUserViewArray[0].canvas.view);
           Log.d(TAG, "startPreview result " + result);
       }

        // 启动屏幕共享预览，并且显示到右下角小图
        if (mScreenMode) {
            mUserViewArray[4].isFree = false;
            mUserViewArray[4].isScreen = true;
            mUserViewArray[4].setUser(mSelfUid);
            updateLocalVideoRender(mUserViewArray[4].canvas, true);
            mUserViewArray[4].renderContainer.addView((SurfaceView) mUserViewArray[4].canvas.view);
            mUserViewArray[4].setVisible(true);
        }

        joinChannel();
    }

    @Override
    public void onBackPressed() {
        leaveChannel();
        int count = getSupportFragmentManager().getBackStackEntryCount();
        for (int i = 0; i < count; i++) {
            getSupportFragmentManager().popBackStack();
        }
        super.onBackPressed();
    }

    @Override
    protected void onDestroy() {
        releaseView();
        removeEventHandler(this);
        mRtcEngine.destroy();
        mRtcEngine = null;
        super.onDestroy();
    }

    // 加入频道
    private boolean joinChannel() {
        DingRtcAuthInfo mAuthInfo = new DingRtcAuthInfo();
        Intent intent = getIntent();
        mAuthInfo.userId = intent.getStringExtra(INTENT_MEETING_USER_ID);
        mAuthInfo.token = intent.getStringExtra(INTENT_MEETING_TOKEN);
        mAuthInfo.appId = intent.getStringExtra(INTENT_MEETING_APP_ID);
        mAuthInfo.channelId = intent.getStringExtra(INTENT_MEETING_CHANNEL_ID);
        String userName = intent.getStringExtra(INTENT_MEETING_USER_NAME);
        int ret = mRtcEngine.joinChannel(mAuthInfo, userName);
        if (ret != 0) {
            Toast.makeText(CallActivity.this, "joinChannel failed, result=" + ret, Toast.LENGTH_LONG).show();
            return false;
        }

        return true;
    }

    // 离开当前频道
    private void leaveChannel() {
        mRtcEngine.stopPreview();
        mRtcEngine.publishLocalAudioStream(false);
        mRtcEngine.publishLocalVideoStream(false);
        mRtcEngine.leaveChannel();
        mUnsubscribedVideoUsers.clear();
        for (int i = 0; i < mUserViewCount; i++) {
            mUserViewArray[i].isFree = true;
        }
    }

    private void releaseView() {
        for (int i = 0; i < mUserViewCount; i++) {
            mUserViewArray[i].canvas = null;
            mUserViewArray[i].isFree = true;
        }
    }

    // -------------------------- RTC Engine Callbacks --------------------------
    public void onUserVideoStart(String userId, DingRtcEngine.DingRtcVideoStreamType maxProfile) {
        Log.i(TAG, "onUserVideoStart, userId=" + userId + ", profile=" + maxProfile);
        if (!mVideoMode || userId.equals(mSelfUid) || mSubedUids.contains(userId)) {
            return;
        }

        int viewIndex = -1;
        boolean localViewIsScreen = false;
        int localViewIndex = -1;
        // 先检查大图是否空闲，如空闲则将此用户显示到大图
        if (mUserViewArray[0].isFree || mSelfUid.equals(mUserViewArray[0].userId)) {
            Log.d(TAG, "onUserVideoStart  userId " + userId);
            // large view is free or used by local user, then make this user to large view
            // 如果大图被本地用户占用，则将本地用户移到小图，如果没有空闲的小图，则不显示本地用户视频
            if (mSelfUid.equals(mUserViewArray[0].userId)) {
                mLocalCanvas = null;
                for (int i = 1; i < mUserViewCount; i++) {
                    if (mUserViewArray[i].isFree) {
                        mLocalCanvas = mUserViewArray[i].canvas;
                        localViewIsScreen = mUserViewArray[0].isScreen;
                        localViewIndex = i;
                        mUserViewArray[i].setVisible(true);
                        mUserViewArray[i].isFree = false;
                        mUserViewArray[i].isScreen = mUserViewArray[0].isScreen;
                        mUserViewArray[i].setUser(mSelfUid);
                        break;
                    }
                }
            }
            mUserViewArray[0].renderContainer.removeAllViews();
            mUserViewArray[0].setUser(userId);
            mUserViewArray[0].isFree = false;
            mUserViewArray[0].isScreen = false;
            mUserViewArray[0].maxProfile = maxProfile;
            viewIndex = 0;
        } else {
            // 如大图不空闲，则尝试找到一个空闲的小图
            // try to find a free small view
            for (int i = 1; i < mUserViewCount; i++) {
                if (mUserViewArray[i].isFree) {
                    mUserViewArray[i].setUser(userId);
                    mUserViewArray[i].isFree = false;
                    mUserViewArray[i].isScreen = false;
                    viewIndex = i;
                    Log.d(TAG, "viewIndex = " + viewIndex);
                    break;
                }
            }
        }
        Log.d(TAG, "onUserVideoStart viewIndex = " + viewIndex);

        if (viewIndex != -1) {
            DingRtcEngine.DingRtcVideoStreamType profile = DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeLD;
            if (viewIndex == 0) {
                profile = ToVideoProfileType(Math.max(mRemoteProfile, maxProfile.getValue()));
            }
            // 订阅此用户视频到指定视图

            if (subscribeUserVideo(userId, mUserViewArray[viewIndex], profile)) {
                mUserViewArray[viewIndex].maxProfile = maxProfile;
                mUserViewArray[viewIndex].subProfile = profile;
            } else {
                mUserViewArray[viewIndex].isFree = true;
            }

            if(localViewIndex != -1) {
                updateLocalVideoRender(mLocalCanvas, localViewIsScreen);
                mUserViewArray[localViewIndex].renderContainer.removeAllViews();
                mUserViewArray[localViewIndex].renderContainer.addView((SurfaceView) mLocalCanvas.view);
                mUserViewArray[localViewIndex].setVisible(true);
            }
        } else {
            // 如果没有空闲视图，将此用户加入未订阅视频用户列表，以等待空闲视图
            // no view available
            VideoUserInfo vui = new VideoUserInfo();
            vui.userId = userId;
            vui.maxProfile = maxProfile;
            mUnsubscribedVideoUsers.put(userId, vui);
        }
    }

    public void onUserVideoStop(String userId) {
        Log.i(TAG, "onUserVideoStop, userId=" + userId);
        // 取消订阅此用户视频
        mSubedUids.remove(userId);
        mRtcEngine.subscribeRemoteVideoStream(userId, DingRtcVideoTrack.DingRtcVideoTrackCamera, false);
        stopUserVideo(userId);

    }

    public void onUserScreenStart(String userId) {
        Log.i(TAG, "onUserScreenStart, userId=" + userId);
        if (!mVideoMode) {
            return;
        }
        // 有用户开启桌面共享，始终显示桌面共享到大图
        if (!mUserViewArray[0].isFree) {
            // 如果大图已经在显示桌面共享，则首先取消订阅之前的共享
            if (mUserViewArray[0].isScreen && !mSelfUid.equals(mUserViewArray[0].userId)) {
                mRtcEngine.subscribeRemoteVideoStream(mUserViewArray[0].userId, DingRtcVideoTrack.DingRtcVideoTrackScreen, false);
            } else if (mSelfUid.equals(mUserViewArray[0].userId)) {
                // 如果大图被本地用户占用，则将本地用户移到小图，如果没有空闲的小图，则不显示本地用户视频
                // large view is used by local user, move local user to last view
                mLocalCanvas = null;
                if (mUserViewArray[mUserViewCount - 1].isFree) {
                    mLocalCanvas = mUserViewArray[mUserViewCount - 1].canvas;
                    mUserViewArray[mUserViewCount - 1].setVisible(true);
                    mUserViewArray[mUserViewCount - 1].isFree = false;
                    mUserViewArray[mUserViewCount - 1].isScreen = mUserViewArray[0].isScreen;
                    mUserViewArray[mUserViewCount - 1].setUser(mSelfUid);
                }
                updateLocalVideoRender(mLocalCanvas, mUserViewArray[0].isScreen);
            } else {
                // 尝试找到一个空闲的小图给当前大图用户
                // try to find a free small view
                int viewIndex = -1;
                for (int i = 1; i < mUserViewCount; i++) {
                    if (mUserViewArray[i].isFree) {
                        mUserViewArray[i].setUser(mUserViewArray[0].userId);
                        mUserViewArray[i].isFree = false;
                        mUserViewArray[i].isScreen = false;
                        viewIndex = i;
                        break;
                    }
                }
                if (viewIndex != -1) {
                    // 找到了一个空闲视图，将大图用户迁移到此空闲视图
                    DingRtcEngine.DingRtcVideoStreamType profile = DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeLD;
                    if (subscribeUserVideo(mUserViewArray[0].userId, mUserViewArray[viewIndex], profile)) {
                        mUserViewArray[viewIndex].maxProfile = mUserViewArray[0].maxProfile;
                        mUserViewArray[viewIndex].subProfile = profile;
                    } else {
                        mRtcEngine.subscribeRemoteVideoStream(mUserViewArray[0].userId, DingRtcVideoTrack.DingRtcVideoTrackCamera, false);
                        mUserViewArray[viewIndex].isFree = true;
                    }
                } else {
                    // 找不到空闲视图，取消订阅此用户，并将此用户加入未订阅视频用户列表，以等待空闲视图
                    // no view available
                    mRtcEngine.subscribeRemoteVideoStream(mUserViewArray[0].userId, DingRtcVideoTrack.DingRtcVideoTrackCamera, false);
                    VideoUserInfo vui = new VideoUserInfo();
                    vui.userId = mUserViewArray[0].userId;
                    vui.maxProfile = mUserViewArray[0].maxProfile;
                    mUnsubscribedVideoUsers.put(vui.userId, vui);
                }
            }
            mUserViewArray[0].isFree = true;
            mUserViewArray[0].isScreen = false;
        }

        // 订阅此用户桌面共享，并显示到大图
        if (subscribeUserScreen(userId, mUserViewArray[0])) {
            //mUserViewArray[0].maxProfile = Constants.VideoProfileType.HD1080P;
            //mUserViewArray[0].subProfile = Constants.VideoProfileType.HD1080P;
        } else {
            mUserViewArray[0].isFree = true;
        }
    }

    public void onUserScreenStop(String userId) {
        Log.i(TAG, "onUserScreenStop, userId=" + userId);
        // 取消订阅此用户桌面共享
        mRtcEngine.subscribeRemoteVideoStream(userId, DingRtcVideoTrack.DingRtcVideoTrackScreen, false);
        stopUserScreen(userId);
    }


    // 订阅用户视频
    private boolean subscribeUserVideo(String userId, UserViewInfo viewInfo, DingRtcEngine.DingRtcVideoStreamType profile) {
        mSubedUids.add(userId);
        viewInfo.setUser(userId);
        viewInfo.setVisible(true);
        viewInfo.isFree = false;
        viewInfo.isScreen = false;

        int ret = mRtcEngine.subscribeRemoteVideoStream(userId, DingRtcVideoTrack.DingRtcVideoTrackCamera, true);
        Log.d(TAG, "subscribeUserVideo = " + userId + ", ret = " + ret);
        mRtcEngine.setRemoteViewConfig(viewInfo.canvas, userId, DingRtcVideoTrack.DingRtcVideoTrackCamera);
        viewInfo.renderContainer.removeAllViews();
        viewInfo.renderContainer.addView((SurfaceView) viewInfo.canvas.view);
        viewInfo.setVisible(true);
        if (ret == 0) {
            return true;
        } else {
            String msg = "subscribeUserVideo failed, userId=" + userId + ", result=" + ret;
            Log.w(TAG, msg);
            Toast.makeText(CallActivity.this, msg, Toast.LENGTH_LONG).show();
            return false;
        }
    }

    // 订阅用户桌面共享
    private boolean subscribeUserScreen(String userId, UserViewInfo viewInfo) {
        viewInfo.setUser(userId);
        viewInfo.setVisible(true);
        viewInfo.isFree = false;
        viewInfo.isScreen = true;


        int ret = mRtcEngine.subscribeRemoteVideoStream(userId, DingRtcVideoTrack.DingRtcVideoTrackScreen, true);
        mRtcEngine.setRemoteViewConfig(viewInfo.canvas, userId, DingRtcVideoTrack.DingRtcVideoTrackScreen);
        viewInfo.renderContainer.addView((SurfaceView) viewInfo.canvas.view);
        viewInfo.setVisible(true);
        if (ret == 0) {
            return true;
        } else {
            String msg = "subscribeUserScreen failed, userId=" + userId + ", result=" + ret;
            Log.w(TAG, msg);
            Toast.makeText(CallActivity.this, msg, Toast.LENGTH_LONG).show();
            return false;
        }
    }

    // 将index指定的视图用户和大图用户交换
    private void switchToLargeView(int index) {
        Log.i(TAG, "switchToLargeView index = " + index);
        if (index < 1 || index > 4) {
            return;
        }
        if (mUserViewArray[index].isFree) {
            return;
        }

        mUserViewArray[0].renderContainer.removeAllViews();
        mUserViewArray[index].renderContainer.removeAllViews();

        String largeViewUid = mUserViewArray[0].userId;
        String smallViewUid = mUserViewArray[index].userId;

        switchUserViewInfo(mUserViewArray[0], mUserViewArray[index]);

        // 本地流大小窗交换
        if (smallViewUid.equals(mSelfUid) && largeViewUid.equals(mSelfUid)) {
            updateLocalVideoRender(mUserViewArray[0].canvas, mUserViewArray[0].isScreen);
            updateLocalVideoRender(mUserViewArray[index].canvas, mUserViewArray[index].isScreen);
           
            mUserViewArray[0].renderContainer.addView((SurfaceView) mUserViewArray[0].canvas.view);
            mUserViewArray[0].setVisible(true);

            mUserViewArray[index].renderContainer.addView((SurfaceView) mUserViewArray[index].canvas.view);
            mUserViewArray[index].setVisible(true);
        }
        else if (smallViewUid.equals(mSelfUid)) {
            // 本地流（小窗）-> 大窗
            updateLocalVideoRender(mUserViewArray[0].canvas, mUserViewArray[0].isScreen);
            mUserViewArray[0].renderContainer.addView((SurfaceView) mUserViewArray[0].canvas.view);
            mUserViewArray[0].setVisible(true);

            if (!TextUtils.isEmpty(largeViewUid)) {
                // 远端流（大窗）-> 小窗
                updateRemoteVideoRender(mUserViewArray[index].canvas, largeViewUid, mUserViewArray[index].isScreen);                
                mUserViewArray[index].renderContainer.addView((SurfaceView) mUserViewArray[index].canvas.view);
                mUserViewArray[index].setVisible(true);
            } else {
                mUserViewArray[index].setVisible(false);
            }

        } else if (largeViewUid.equals(mSelfUid)) {
            // 远端流（小窗）-> 大窗
            updateRemoteVideoRender(mUserViewArray[0].canvas, smallViewUid, mUserViewArray[0].isScreen);
            mUserViewArray[0].renderContainer.addView((SurfaceView) mUserViewArray[0].canvas.view);
            mUserViewArray[0].setVisible(true);

            // 本地流（大窗）-> 小窗
            updateLocalVideoRender(mUserViewArray[index].canvas, mUserViewArray[index].isScreen);
            mUserViewArray[index].renderContainer.addView((SurfaceView) mUserViewArray[index].canvas.view);
            mUserViewArray[index].setVisible(true);
        } else {
            // 远端流（小窗）-> 大窗
            updateRemoteVideoRender(mUserViewArray[0].canvas, smallViewUid, mUserViewArray[0].isScreen);
            mUserViewArray[0].renderContainer.addView((SurfaceView) mUserViewArray[0].canvas.view);
            mUserViewArray[0].setVisible(true);

            if (!TextUtils.isEmpty(largeViewUid)) {
                // 远端流（大窗）-> 小窗
                updateRemoteVideoRender(mUserViewArray[index].canvas, largeViewUid, mUserViewArray[index].isScreen);
                mUserViewArray[index].renderContainer.addView((SurfaceView) mUserViewArray[index].canvas.view);
                mUserViewArray[index].setVisible(true);
            } else {
                mUserViewArray[index].setVisible(false);
            }
        }
    }

    private void switchUserViewInfo(UserViewInfo a, UserViewInfo b) {
        String tmpUid = a.userId;
        boolean tmpIsFree = a.isFree;
        boolean tmpIsScreen = a.isScreen;
        DingRtcEngine.DingRtcVideoStreamType tmpMaxProfile = a.maxProfile;
        DingRtcEngine.DingRtcVideoStreamType tmpSubProfile = a.subProfile;
        a.userId = b.userId;
        a.isFree = b.isFree;
        a.isScreen = b.isScreen;
        a.maxProfile = b.maxProfile;
        a.subProfile = b.subProfile;
        b.userId = tmpUid;
        b.isFree = tmpIsFree;
        b.isScreen = tmpIsScreen;
        b.maxProfile = tmpMaxProfile;
        b.subProfile = tmpSubProfile;
    }


    // 取消订阅用户视频
    private void stopUserVideo(String uid) {
        for (int i = 0; i < mUserViewCount; i++) {
            if (uid.equals(mUserViewArray[i].userId) && !mUserViewArray[i].isScreen) {
                stopUserView(uid, i);
                break;
            }
        }
    }

    // 取消订阅用户桌面共享
    private void stopUserScreen(String uid) {
        if (uid.equals(mUserViewArray[0].userId) && mUserViewArray[0].isScreen) {
            stopUserView(uid, 0);
        }
    }

    private void stopUserView(String userId, int index) {
        mUserViewArray[index].isScreen = false; // reset screen flag
        // check if there is unsubscribed video user, and subscribe it if any
        if (mUnsubscribedVideoUsers.size() > 0) {
            Map.Entry<String, VideoUserInfo> firstEntry = mUnsubscribedVideoUsers.entrySet().iterator().next();
            String firstKey = firstEntry.getKey();
            VideoUserInfo vui = firstEntry.getValue();
            mUnsubscribedVideoUsers.remove(firstKey);

            DingRtcEngine.DingRtcVideoStreamType profile = DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeLD;
            if (index == 0) {
                profile = ToVideoProfileType(Math.max(mRemoteProfile, vui.maxProfile.getValue()));
            }
            if (subscribeUserVideo(vui.userId, mUserViewArray[index], profile)) {
                mUserViewArray[index].maxProfile = vui.maxProfile;
                mUserViewArray[index].subProfile = profile;
                return;
            }
        }

        // if large view user left, then try to move other remote user to large view
        if (index == 0) { // is large view
            for (int j = mUserViewCount - 1; j > 0; j--) {
                if (!mUserViewArray[j].isFree && !mSelfUid.equals(mUserViewArray[j].userId)) {
                    mUserViewArray[0].userId = mUserViewArray[j].userId;
                    mUserViewArray[0].isFree = false;
                    mUserViewArray[0].maxProfile = mUserViewArray[j].maxProfile;
                    mUserViewArray[0].subProfile = mUserViewArray[j].subProfile;
                    mUserViewArray[j].isFree = true;
                    mUserViewArray[j].setVisible(false);

                    DingRtcEngine.DingRtcVideoStreamType profile = ToVideoProfileType(Math.max(mRemoteProfile,
                            mUserViewArray[0].maxProfile.getValue()));
                    // resubscribe the video
                    subscribeUserVideo(mUserViewArray[0].userId, mUserViewArray[0], profile);
                    mUserViewArray[0].subProfile = profile;
                    return;
                }
            }
        }

        mUserViewArray[index].isFree = true;
        mUserViewArray[index].setVisible(false);
        mUserViewArray[index].userId = "";
    }

    // 更新远端用户视频的视图
    private void updateRemoteVideoRender(DingRtcEngine.DingRtcVideoCanvas canvas, String uid, boolean isScreen) {
        if(isScreen) {
            mRtcEngine.setRemoteViewConfig(null, uid, DingRtcVideoTrack.DingRtcVideoTrackScreen);
            mRtcEngine.setRemoteViewConfig(canvas, uid, DingRtcVideoTrack.DingRtcVideoTrackScreen);
            return;
        } else {
            mRtcEngine.setRemoteViewConfig(null, uid, DingRtcVideoTrack.DingRtcVideoTrackCamera);
            mRtcEngine.setRemoteViewConfig(canvas, uid, DingRtcVideoTrack.DingRtcVideoTrackCamera);
        }
    }

    // 更新本地用户视频的视图
    private void updateLocalVideoRender(DingRtcEngine.DingRtcVideoCanvas canvas, boolean isScreen) {
        mLocalCanvas = canvas;
        if(isScreen) {
            mRtcEngine.setLocalViewConfig(null, DingRtcVideoTrack.DingRtcVideoTrackScreen);
            mRtcEngine.setLocalViewConfig(mLocalCanvas, DingRtcVideoTrack.DingRtcVideoTrackScreen);
            return;
        }
        else{
            mRtcEngine.setLocalViewConfig(null, DingRtcVideoTrack.DingRtcVideoTrackCamera);
            mRtcEngine.setLocalViewConfig(mLocalCanvas, DingRtcVideoTrack.DingRtcVideoTrackCamera);
            return;
        }
    }

}
