package com.ding.rtc.apiexample.advanced.multiviewrender;

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
import android.widget.FrameLayout;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;


public class MultiViewRenderActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;
    private String mRemoteUserId;

    private FrameLayout mLocalViewHolder;
    private FrameLayout mRemoteViewHolder1;
    private FrameLayout mRemoteViewHolder2;
    private FrameLayout mRemoteViewHolder3;
    private FrameLayout mRemoteViewHolder4;

    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas1 = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas2 = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas3 = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas4 = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, MultiViewRenderActivity.class);
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
        setContentView(R.layout.activity_multiview_render);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.multiview_render);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder1 = findViewById(R.id.remote_view1);
        mRemoteViewHolder2 = findViewById(R.id.remote_view2);
        mRemoteViewHolder3 = findViewById(R.id.remote_view3);
        mRemoteViewHolder4 = findViewById(R.id.remote_view4);

        initDingEngine();
        joinChannel();
    }

    @Override
    public void onClick(View v) {
        // Important: By setting the toBeRemoved property, the binding to the video stream is marked and the setLocal/remote View Config is invoked to take effect
        if (v.getId() == R.id.remote_view1) {
            mRemoteVideoCanvas1.toBeRemoved = !mRemoteVideoCanvas1.toBeRemoved;
            mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas1, mRemoteUserId, DingRtcVideoTrackCamera);
        } else if (v.getId() == R.id.remote_view2) {
            mRemoteVideoCanvas2.toBeRemoved = !mRemoteVideoCanvas2.toBeRemoved;
            mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas2, mRemoteUserId, DingRtcVideoTrackCamera);
        } else if (v.getId() == R.id.remote_view3) {
            mRemoteVideoCanvas3.toBeRemoved = !mRemoteVideoCanvas3.toBeRemoved;
            mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas3, mRemoteUserId, DingRtcVideoTrackCamera);
        } else if (v.getId() == R.id.remote_view4) {
            mRemoteVideoCanvas4.toBeRemoved = !mRemoteVideoCanvas4.toBeRemoved;
            mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas4, mRemoteUserId, DingRtcVideoTrackCamera);
        }
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");

            mRtcEngine.publishLocalVideoStream(true);
            mRtcEngine.publishLocalAudioStream(true);
            mRtcEngine.subscribeAllRemoteAudioStreams(true);

            SurfaceView localView = mRtcEngine.createRenderSurfaceView(this);
            mLocalVideoCanvas.view = localView;
            mLocalVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
            mRemoteVideoCanvas1.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
            mRemoteVideoCanvas2.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
            mRemoteVideoCanvas3.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
            mRemoteVideoCanvas4.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
            mLocalViewHolder.addView(localView);
            mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackCamera);
            mRtcEngine.startPreview();

            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
                @Override
                public void onRemoteTrackAvailableNotify(String uid, DingRtcEngine.DingRtcAudioTrack audioTrack, DingRtcEngine.DingRtcVideoTrack videoTrack) {
                    super.onRemoteTrackAvailableNotify(uid, audioTrack, videoTrack);
                    mRemoteUserId = uid;
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            // As sample code, it only supports subscribing to one video
                            if (hasRemoteVideo) {
                                if (videoTrack == DingRtcVideoTrackNo || videoTrack == DingRtcVideoTrackScreen) {
                                    hasRemoteVideo = false;
                                    mRemoteViewHolder1.removeAllViews();
                                    mRemoteViewHolder2.removeAllViews();
                                    mRemoteViewHolder3.removeAllViews();
                                    mRemoteViewHolder4.removeAllViews();

                                    // Important: By setting mVideoCanvas.view to empty, the resources bound to the view can be released
                                    mRemoteVideoCanvas1.view = null;
                                    mRemoteVideoCanvas2.view = null;
                                    mRemoteVideoCanvas3.view = null;
                                    mRemoteVideoCanvas4.view = null;
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas1, uid, DingRtcVideoTrackCamera);
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas2, uid, DingRtcVideoTrackCamera);
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas3, uid, DingRtcVideoTrackCamera);
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas4, uid, DingRtcVideoTrackCamera);

                                    mRtcEngine.subscribeRemoteVideoStream(uid, DingRtcVideoTrackCamera, false);
                                }
                            } else {
                                if (videoTrack == DingRtcVideoTrackCamera || videoTrack == DingRtcVideoTrackBoth) {
                                    hasRemoteVideo = true;

                                    // Important: Multiple render Views can be created and bound to the same video stream.
                                    SurfaceView localView1 = mRtcEngine.createRenderSurfaceView(MultiViewRenderActivity.this);
                                    mRemoteVideoCanvas1.view = localView1;
                                    mRemoteViewHolder1.addView(localView1);
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas1, uid, DingRtcVideoTrackCamera);

                                    SurfaceView localView2 = mRtcEngine.createRenderSurfaceView(MultiViewRenderActivity.this);
                                    mRemoteVideoCanvas2.view = localView2;
                                    mRemoteViewHolder2.addView(localView2);
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas2, uid, DingRtcVideoTrackCamera);

                                    SurfaceView localView3 = mRtcEngine.createRenderSurfaceView(MultiViewRenderActivity.this);
                                    mRemoteVideoCanvas3.view = localView3;
                                    mRemoteViewHolder3.addView(localView3);
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas3, uid, DingRtcVideoTrackCamera);

                                    SurfaceView localView4 = mRtcEngine.createRenderSurfaceView(MultiViewRenderActivity.this);
                                    mRemoteVideoCanvas4.view = localView4;
                                    mRemoteViewHolder4.addView(localView4);
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas4, uid, DingRtcVideoTrackCamera);

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