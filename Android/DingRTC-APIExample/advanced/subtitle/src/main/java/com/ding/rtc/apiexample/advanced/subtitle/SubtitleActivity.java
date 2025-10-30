package com.ding.rtc.apiexample.advanced.subtitle;

import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackBoth;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackNo;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackScreen;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;
import com.ding.rtc.DingRtcEngineSubtitleManager;
import com.ding.rtc.DingRtcEngineSubtitleManagerListener;
import com.ding.rtc.DingRtcEngineSubtitleTypes;

import java.util.ArrayList;


public class SubtitleActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private DingRtcEngineSubtitleManager mSubtitleManager;
    private String mChannelId;
    private String mUserId;
    private FrameLayout mLocalViewHolder;
    private FrameLayout mRemoteViewHolder;
    private SubtitleView mSubtitleView;
    private TextView mSubtitleStatusView;

    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;

    private Handler mHandler = new Handler(Looper.getMainLooper());
    private Runnable mRunnable = new Runnable() {
        @Override
        public void run() {
            // Important : Set the current time for SubtitleView to display real-time subtitles
            mSubtitleView.setCurrentPosition(System.currentTimeMillis());
            mHandler.postDelayed(this, 1000);
        }
    };

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, SubtitleActivity.class);
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
        setContentView(R.layout.activity_subtitle);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.subtitle);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }
        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder = findViewById(R.id.remote_view);
        // Important : Construct and initialize the SubtitleView
        mSubtitleView = findViewById(R.id.subtitle_view);
        mSubtitleStatusView = findViewById(R.id.subtitle_status_textview);

        mHandler.postDelayed(mRunnable, 1000);
        initDingEngine();
        joinChannel();
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mSubtitleManager = mRtcEngine.getSubtitleManager();
            mSubtitleManager.setCurrentSpeakLanguage("zh");
            ArrayList<String> lanList = new ArrayList<>();
            lanList.add("zh");
            mSubtitleManager.setCurrentTranslateLanguages(lanList);
            mSubtitleManager.enableSubtitle(true);
            mSubtitleManager.setSubtitleManagerEventListener(new DingRtcEngineSubtitleManagerListener() {
                @Override
                public void onLanguageListsUpdated() {

                }

                @Override
                public void onSubtitleServiceStatusChanged(DingRtcEngineSubtitleTypes.DingRtcSubtitleStatus status) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mSubtitleStatusView.setText("Status : " + status.toString());
                        }
                    });
                }

                @Override
                public void onSubtitleMessage(DingRtcEngineSubtitleTypes.DingRtcSubtitleMessage message) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            // Important : Add subtitle messages
                            String text = message.userId + " : " + message.subtitle;
                            mSubtitleView.setSubtitle(new SubtitleItem(text, message.beginTime, message.endTime));
                        }
                    });
                }
            });

            mRtcEngine.publishLocalVideoStream(true);
            mRtcEngine.publishLocalAudioStream(true);
            mRtcEngine.subscribeAllRemoteAudioStreams(true);
            SurfaceView localView = mRtcEngine.createRenderSurfaceView(this);
            mLocalVideoCanvas.view = localView;
            mLocalVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
            mRemoteVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
            mLocalViewHolder.addView(localView);
            mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackCamera);
            mRtcEngine.startPreview();

            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {

                @Override
                public void onRemoteTrackAvailableNotify(String uid, DingRtcEngine.DingRtcAudioTrack audioTrack, DingRtcEngine.DingRtcVideoTrack videoTrack) {
                    super.onRemoteTrackAvailableNotify(uid, audioTrack, videoTrack);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
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

                                    SurfaceView localView = mRtcEngine.createRenderSurfaceView(SubtitleActivity.this);
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

    @Override
    protected void onStop() {
        super.onStop();
        if (isFinishing()) {
            mHandler.removeCallbacks(mRunnable);
            mRtcEngine.leaveChannel();
            mRtcEngine.destroy();
            mRtcEngine = null;
        }
    }
}