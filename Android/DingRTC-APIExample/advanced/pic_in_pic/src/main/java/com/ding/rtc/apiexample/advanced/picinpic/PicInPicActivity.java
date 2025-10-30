package com.ding.rtc.apiexample.advanced.picinpic;

import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackBoth;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackNo;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackScreen;

import android.app.PictureInPictureParams;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.util.Rational;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

public class PicInPicActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";
    private static final String INTENT_IS_PUBLISHER = "is_publisher";

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;
    private boolean isPublisher = true;

    private TextView mLocalLable;
    private TextView mRemoteLable;
    private FrameLayout mLocalViewHolder;
    private FrameLayout mRemoteViewHolder;
    private Button mOperationBtn;

    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token, boolean publisher) {
        Intent intent = new Intent();
        intent.setClass(context, PicInPicActivity.class);
        intent.putExtra(INTENT_APP_ID, appId);
        intent.putExtra(INTENT_CHANNEL_ID, channelId);
        intent.putExtra(INTENT_USER_ID, userId);
        intent.putExtra(INTENT_TOKEN, token);
        intent.putExtra(INTENT_USER_NAME, userName);
        intent.putExtra(INTENT_IS_PUBLISHER, publisher);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_picinpic);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.pic_in_pic);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }
        isPublisher = getIntent().getBooleanExtra(INTENT_IS_PUBLISHER, true);
        mLocalLable = findViewById(R.id.local_textview);
        mRemoteLable = findViewById(R.id.remote_textview);
        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder = findViewById(R.id.remote_view);
        mOperationBtn = findViewById(R.id.btn_operation);

        // in order to reserve a larger View for observing various functions of beauty and filters,
        // the publisher end the subscriber are split into two separate terminals.
        if (isPublisher) {
            mRemoteLable.setVisibility(View.GONE);
            mRemoteViewHolder.setVisibility(View.GONE);
        } else {
            mLocalLable.setVisibility(View.GONE);
            mLocalViewHolder.setVisibility(View.GONE);
        }

        initDingEngine();
        joinChannel();
    }


    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");

            if (isPublisher) {
                mRtcEngine.publishLocalVideoStream(true);
                mRtcEngine.publishLocalAudioStream(true);
                mRtcEngine.subscribeAllRemoteAudioStreams(false);
                mRtcEngine.subscribeAllRemoteVideoStreams(false);
                SurfaceView localView = mRtcEngine.createRenderSurfaceView(this);
                mLocalVideoCanvas.view = localView;
                mLocalVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
                mRemoteVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
                mLocalViewHolder.addView(localView);
                mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackCamera);
                mRtcEngine.startPreview();
            } else {
                mRtcEngine.publishLocalVideoStream(false);
                mRtcEngine.publishLocalAudioStream(false);
                mRtcEngine.subscribeAllRemoteAudioStreams(true);
                mRtcEngine.subscribeAllRemoteVideoStreams(true);
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
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas, uid, DingRtcVideoTrackCamera);

                                    mRtcEngine.subscribeRemoteVideoStream(uid, DingRtcVideoTrackCamera, false);
                                }
                            } else {
                                if (videoTrack == DingRtcVideoTrackCamera || videoTrack == DingRtcVideoTrackBoth) {
                                    hasRemoteVideo = true;

                                    SurfaceView localView = mRtcEngine.createRenderSurfaceView(PicInPicActivity.this);
                                    mRemoteVideoCanvas.view = localView;
                                    mRemoteViewHolder.addView(localView);

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

    private void enterPiPMode() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            PictureInPictureParams params = new PictureInPictureParams.Builder()
                    .setAspectRatio(new Rational(1080, 1920))
                    .build();
            enterPictureInPictureMode(params);
        }
    }

    @Override
    public void onPictureInPictureModeChanged(boolean isInPictureInPictureMode, Configuration newConfig) {
        if (isInPictureInPictureMode) {
            // hide UI
            if (getSupportActionBar() != null) {
                getSupportActionBar().hide();
            }
            mOperationBtn.setVisibility(View.GONE);
            if (isPublisher) {
                mLocalLable.setVisibility(View.GONE);
            } else {
                mRemoteLable.setVisibility(View.GONE);
            }

        } else {
            // show UI
            if (getSupportActionBar() != null) {
                getSupportActionBar().show();
            }
            mOperationBtn.setVisibility(View.VISIBLE);
            if (isPublisher) {
                mLocalLable.setVisibility(View.VISIBLE);
            } else {
                mRemoteLable.setVisibility(View.VISIBLE);
            }
        }
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

    public void onPicInPicButtonClick(View view) {
        enterPiPMode();
    }
}