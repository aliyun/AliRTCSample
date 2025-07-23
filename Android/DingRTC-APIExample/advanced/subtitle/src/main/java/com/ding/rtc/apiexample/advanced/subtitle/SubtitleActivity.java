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
import android.view.MenuItem;
import android.view.SurfaceView;
import android.widget.FrameLayout;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;
import com.ding.rtc.DingRtcEngineSubtitleManager;
import com.ding.rtc.DingRtcEngineSubtitleManagerListener;
import com.ding.rtc.DingRtcEngineSubtitleTypes;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;


public class SubtitleActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";
    private static final String INTENT_IS_PUBLISHER = "is_publisher";

    private DingRtcEngine mRtcEngine;
    private DingRtcEngineSubtitleManager mSubtitleManager;
    private String mChannelId;
    private String mUserId;
    private boolean isPublisher = true;
    private FrameLayout mLocalViewHolder;
    private FrameLayout mRemoteViewHolder;
    private SubtitleView mSubtitleView;

    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;
    // Important : List<SubtitleItem> Used to save the subtitle messages displayed.
    private List<SubtitleItem> mSubtitles = new ArrayList<>();

    private Handler mHandler = new Handler(Looper.getMainLooper());
    private Runnable mRunnable = new Runnable() {
        @Override
        public void run() {
            // Important : Set the current time for SubtitleView to display real-time subtitles
            mSubtitleView.setCurrentPosition(System.currentTimeMillis());
            mHandler.postDelayed(this, 1000);
        }
    };

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token, boolean publisher) {
        Intent intent = new Intent();
        intent.setClass(context, SubtitleActivity.class);
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
        setContentView(R.layout.activity_subtitle);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.subtitle);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }
        isPublisher = getIntent().getBooleanExtra(INTENT_IS_PUBLISHER, true);
        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder = findViewById(R.id.remote_view);
        // Important : Construct and initialize the SubtitleView
        mSubtitleView = findViewById(R.id.subtitle_view);
        mSubtitleView.setSubtitles(mSubtitles);

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

                }

                @Override
                public void onSubtitleMessage(DingRtcEngineSubtitleTypes.DingRtcSubtitleMessage message) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            // Important : Add subtitle messages
                            String text = message.userId + " : " + message.subtitle;
                            mSubtitles.add(new SubtitleItem(text, message.beginTime, message.endTime));
                        }
                    });
                }
            });

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