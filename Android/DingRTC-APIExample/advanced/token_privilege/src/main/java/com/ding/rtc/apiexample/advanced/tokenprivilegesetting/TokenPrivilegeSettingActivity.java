package com.ding.rtc.apiexample.advanced.tokenprivilegesetting;

import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackBoth;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackNo;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackScreen;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

public class TokenPrivilegeSettingActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";
    private static final String INTENT_IS_PUBLISHER = "is_publisher";
    private static final String INTENT_IS_PUBLISH_AUDIO = "is_publish_audio";
    private static final String INTENT_IS_PUBLISH_VIDEO = "is_publish_video";

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;
    private String mRemoteUserId;

    private ViewGroup mLocalViewHolder;
    private ViewGroup mRemoteViewHolder;

    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;
    private boolean isPublisher = true;
    private boolean isPublishAudio = true;
    private boolean isPublishVideo = true;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token, boolean publisher, boolean isPublishAudio, boolean isPublishVideo) {
        Intent intent = new Intent();
        intent.setClass(context, TokenPrivilegeSettingActivity.class);
        intent.putExtra(INTENT_APP_ID, appId);
        intent.putExtra(INTENT_CHANNEL_ID, channelId);
        intent.putExtra(INTENT_USER_ID, userId);
        intent.putExtra(INTENT_TOKEN, token);
        intent.putExtra(INTENT_USER_NAME, userName);
        intent.putExtra(INTENT_IS_PUBLISHER, publisher);
        intent.putExtra(INTENT_IS_PUBLISH_AUDIO, isPublishAudio);
        intent.putExtra(INTENT_IS_PUBLISH_VIDEO, isPublishVideo);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_token_privilege_setting);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.token_privilege_setting);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }
        isPublisher = getIntent().getBooleanExtra(INTENT_IS_PUBLISHER, true);
        isPublishAudio = getIntent().getBooleanExtra(INTENT_IS_PUBLISH_AUDIO, true);
        isPublishVideo = getIntent().getBooleanExtra(INTENT_IS_PUBLISH_VIDEO, true);
        mLocalViewHolder = findViewById(R.id.local_view_group);
        mRemoteViewHolder = findViewById(R.id.remote_view_group);

        if (isPublisher) {
            mRemoteViewHolder.setVisibility(View.GONE);
        } else {
            mLocalViewHolder.setVisibility(View.GONE);
        }
        initDingEngine();
        joinChannel();
    }

    @Override
    public void onClick(View v) {
        if (mRtcEngine != null) {
            mRtcEngine.switchCamera();
        }
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");

            if (isPublisher) {
                if (isPublishVideo) {
                    mRtcEngine.publishLocalVideoStream(true);
                }
                if (isPublishAudio) {
                    mRtcEngine.publishLocalAudioStream(true);
                }
                mRtcEngine.subscribeAllRemoteAudioStreams(false);
            } else {
                mRtcEngine.publishLocalAudioStream(false);
                mRtcEngine.publishLocalVideoStream(false);
                mRtcEngine.subscribeAllRemoteAudioStreams(true);
                mRtcEngine.subscribeAllRemoteVideoStreams(true);
            }

            if (isPublisher && isPublishVideo) {
                SurfaceView localView = mRtcEngine.createRenderSurfaceView(this);
                mLocalVideoCanvas.view = localView;
                mLocalVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
                mRemoteVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
                mLocalViewHolder.addView(localView);
                mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackCamera);
                mRtcEngine.startPreview();
            }

            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
                @Override
                public void onRemoteTrackAvailableNotify(String uid, DingRtcEngine.DingRtcAudioTrack audioTrack, DingRtcEngine.DingRtcVideoTrack videoTrack) {
                    super.onRemoteTrackAvailableNotify(uid, audioTrack, videoTrack);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            // Publisher is not allowed to subscribe audio or video here.
                            if (isPublisher) {
                                return;
                            }

                            // As sample code, it only supports subscribing to one video
                            if (hasRemoteVideo) {
                                if (videoTrack == DingRtcVideoTrackNo || videoTrack == DingRtcVideoTrackScreen) {
                                    hasRemoteVideo = false;
                                    mRemoteViewHolder.removeAllViews();

                                    // Important: By setting mVideoCanvas.view to empty, the resources bound to the view can be released
                                    mRemoteVideoCanvas.view = null;
                                    mRemoteUserId = null;
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas, uid, DingRtcVideoTrackCamera);

                                    mRtcEngine.subscribeRemoteVideoStream(uid, DingRtcVideoTrackCamera, false);
                                }
                            } else {
                                if (videoTrack == DingRtcVideoTrackCamera || videoTrack == DingRtcVideoTrackBoth) {
                                    hasRemoteVideo = true;

                                    SurfaceView localView = mRtcEngine.createRenderSurfaceView(TokenPrivilegeSettingActivity.this);
                                    mRemoteVideoCanvas.view = localView;
                                    mRemoteViewHolder.addView(localView);
                                    mRemoteUserId = uid;
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas, uid, DingRtcVideoTrackCamera);
                                    mRtcEngine.subscribeRemoteVideoStream(uid, DingRtcVideoTrackCamera, true);
                                }
                            }
                        }
                    });
                }

            });
        }
    }

    private boolean joinChannel() {
        DingRtcAuthInfo authInfo = new DingRtcAuthInfo();
        Intent intent = getIntent();
        if (intent == null) {
            return false;
        }
        mChannelId = intent.getStringExtra(INTENT_CHANNEL_ID);
        if (mChannelId == null) {
            return false;
        }
        authInfo.channelId = mChannelId;
        mUserId = intent.getStringExtra(INTENT_USER_ID);
        if (mUserId == null) {
            return false;
        }
        authInfo.userId = mUserId;
        authInfo.token = intent.getStringExtra(INTENT_TOKEN);
        authInfo.appId = intent.getStringExtra(INTENT_APP_ID);
        String userName = intent.getStringExtra(INTENT_USER_NAME);
        if (userName == null) {
            return false;
        }
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

    @Override
    protected void onStop() {
        super.onStop();
        if (isFinishing()) {
            mRtcEngine.leaveChannel();
            mRtcEngine.destroy();
            mRtcEngine = null;
        }
    }
}
