package com.ding.rtc.apiexample.advanced.screenshare;

import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackBoth;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackNo;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackScreen;
import static com.ding.rtc.DingRtcErrorCode.ERR_SCREENSHARE_NO_PERMISSION;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.FrameLayout;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

public class ScreenshareActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";
    private static final int SCREEN_CAPTURE_PERMISSION_CODE = 1002;

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;

    private CheckBox mCheckBoxSharePreview;
    private CheckBox mCheckBoxShareAudio;
    private CheckBox mCheckBoxSdkRequestPermission;
    private FrameLayout mLocalViewHolder;
    private FrameLayout mRemoteViewHolder;
    private Button mButtonScreenShare;

    private DingRtcEngine.DingRtcScreenShareMode mScreenShareMode = DingRtcEngine.DingRtcScreenShareMode.DingRtcScreenShareOnlyVideoMode;
    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean isStartScreenShare = true;
    private boolean hasRemoteStartScreenShare = false;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, ScreenshareActivity.class);
        intent.putExtra(INTENT_APP_ID, appId);
        intent.putExtra(INTENT_CHANNEL_ID, channelId);
        intent.putExtra(INTENT_USER_ID, userId);
        intent.putExtra(INTENT_TOKEN, token);
        intent.putExtra(INTENT_USER_NAME, userName);
        context.startActivity(intent);
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_screenshare);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.screenshare);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }
        mCheckBoxSharePreview = findViewById(R.id.previewCheckBox);
        mCheckBoxShareAudio = findViewById(R.id.audioCheckBox);
        mCheckBoxSdkRequestPermission = findViewById(R.id.sdkRequestPermissionCheckBox);
        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder = findViewById(R.id.remote_view);
        mButtonScreenShare = findViewById(R.id.shareButton);
        initDingEngine();
        joinChannel();
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == SCREEN_CAPTURE_PERMISSION_CODE) {
            if (resultCode == Activity.RESULT_OK && data != null) {
                mRtcEngine.startScreenShare(data, mScreenShareMode);
            }
        }
    }

    @Override
    public void onClick(View v) {
        if (isStartScreenShare) {
            if (mCheckBoxShareAudio.isChecked()) {
                // Screen sharing contains audio and video
                mScreenShareMode = DingRtcEngine.DingRtcScreenShareMode.DingRtcScreenShareAllMode;
            } else {
                // Screen sharing only contains video
                mScreenShareMode = DingRtcEngine.DingRtcScreenShareMode.DingRtcScreenShareOnlyVideoMode;
            }
            if (mCheckBoxSharePreview.isChecked()) {
                SurfaceView localView = mRtcEngine.createRenderSurfaceView(this);
                mLocalVideoCanvas.view = localView;
                mLocalViewHolder.addView(localView);
            }
            mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackScreen);
            if (mCheckBoxSdkRequestPermission.isChecked()) {
                // If you wish to entrust the RTC SDK to dynamically apply for the relevant permissions of screen sharing,
                // then directly call startScreenShare
                mRtcEngine.startScreenShare(mScreenShareMode);
            } else {
                // If you want to apply for screen sharing permission by yourself,
                // you need to implement the Activity for applying for permission by yourself
                Intent intent = new Intent(this, ScreenCapturePermissionActivity.class);
                startActivityForResult(intent, SCREEN_CAPTURE_PERMISSION_CODE);
            }
        } else {
            // Important: By setting mVideoCanvas.view to empty, the resources bound to the view can be released
            mLocalVideoCanvas.view = null;
            mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackScreen);

            mRtcEngine.stopScreenShare();
            mLocalViewHolder.removeAllViews();
        }

        isStartScreenShare = !isStartScreenShare;
        if (isStartScreenShare) {
            mButtonScreenShare.setText(R.string.screenshare_button_start);
        } else {
            mButtonScreenShare.setText(R.string.screenshare_button_stop);
        }
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.subscribeAllRemoteAudioStreams(true);
            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
                @Override
                public void onRemoteTrackAvailableNotify(String uid, DingRtcEngine.DingRtcAudioTrack audioTrack, DingRtcEngine.DingRtcVideoTrack videoTrack) {
                    super.onRemoteTrackAvailableNotify(uid, audioTrack, videoTrack);
                    // As a sample code for screen sharing, non-screen sharing video streams are not subscribed here,
                    // and at most only one screen sharing stream will be subscribed
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            if (hasRemoteStartScreenShare) {
                                if (videoTrack == DingRtcVideoTrackNo || videoTrack == DingRtcVideoTrackCamera) {
                                    hasRemoteStartScreenShare = false;
                                    mRemoteViewHolder.removeAllViews();

                                    // Important: By setting mVideoCanvas.view to empty, the resources bound to the view can be released
                                    mRemoteVideoCanvas.view = null;
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas, uid, DingRtcVideoTrackScreen);

                                    mRtcEngine.subscribeRemoteVideoStream(uid, DingRtcVideoTrackScreen, false);
                                }
                            } else {
                                if (videoTrack == DingRtcVideoTrackScreen || videoTrack == DingRtcVideoTrackBoth) {
                                    hasRemoteStartScreenShare = true;

                                    SurfaceView localView = mRtcEngine.createRenderSurfaceView(ScreenshareActivity.this);
                                    mRemoteVideoCanvas.view = localView;
                                    mRemoteViewHolder.addView(localView);

                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas, uid, DingRtcVideoTrackScreen);
                                    mRtcEngine.subscribeRemoteVideoStream(uid, DingRtcVideoTrackScreen, true);
                                }
                            }
                        }
                    });
                }

                @Override
                public void onOccurError(int error, String message) {
                    super.onOccurError(error, message);
                    // Important: If the RTC SDK is responsible for applying for screen sharing permissions,
                    // then the error of the permission application being rejected needs to be handled
                    if (error == ERR_SCREENSHARE_NO_PERMISSION) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(ScreenshareActivity.this, R.string.screenshare_no_permission, Toast.LENGTH_LONG).show();
                            }
                        });
                    }
                }
            });
        }
    }

    private boolean joinChannel() {
        DingRtcAuthInfo authInfo = new DingRtcAuthInfo();
        Intent intent = getIntent();
        mChannelId = intent.getStringExtra(INTENT_CHANNEL_ID);
        authInfo.channelId = mChannelId;
        mUserId = intent.getStringExtra(INTENT_USER_ID);
        authInfo.userId = mUserId;
        authInfo.token = intent.getStringExtra(INTENT_TOKEN);
        authInfo.appId = intent.getStringExtra(INTENT_APP_ID);
        String userName = intent.getStringExtra(INTENT_USER_NAME);
        int ret = mRtcEngine.joinChannel(authInfo, userName);
        if (ret != 0) {
            return false;
        }

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}