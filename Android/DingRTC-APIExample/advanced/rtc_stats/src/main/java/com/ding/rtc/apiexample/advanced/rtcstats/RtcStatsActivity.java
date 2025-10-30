package com.ding.rtc.apiexample.advanced.rtcstats;

import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackBoth;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackNo;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackScreen;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

public class RtcStatsActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;

    private FrameLayout mLocalViewHolder;
    private FrameLayout mRemoteViewHolder;
    private TextView mSendAudioBitrateTextView;
    private TextView mSendVideoBitrateTextView;
    private TextView mSendLossrateTextView;
    private TextView mReceiveVideoBitrateTextView;
    private TextView mReceiveAudioBitrateTextView;
    private TextView mReceiveLossrateTextView;

    private TextView mLocalAudioBitrateTextView;
    private TextView mLocalVideoBitrateTextView;
    private TextView mLocalVideoWidthTextView;
    private TextView mLocalVideoHeightTextView;
    private TextView mLocalVideoCaptureFPSTextView;
    private TextView mLocalVideoSendFPSTextView;

    private TextView mRemoteAudioBitrateTextView;
    private TextView mRemoteVideoBitrateTextView;
    private TextView mRemoteVideoWidthTextView;
    private TextView mRemoteVideoHeightTextView;
    private TextView mRemoteVideoDecodeFPSTextView;
    private TextView mRemoteVideoRenderFPSTextView;

    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, RtcStatsActivity.class);
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
        setContentView(R.layout.activity_rtc_stats);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.rtc_stats);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder = findViewById(R.id.remote_view);
        mSendAudioBitrateTextView = findViewById(R.id.tv_send_audio_bitrate);
        mSendVideoBitrateTextView = findViewById(R.id.tv_send_video_bitrate);
        mSendLossrateTextView = findViewById(R.id.tv_send_packet_loss);
        mReceiveAudioBitrateTextView = findViewById(R.id.tv_receive_audio_bitrate);
        mReceiveVideoBitrateTextView = findViewById(R.id.tv_receive_video_bitrate);
        mReceiveLossrateTextView = findViewById(R.id.tv_receive_packet_loss);

        mLocalAudioBitrateTextView = findViewById(R.id.tv_local_audio_bitrate);
        mLocalVideoBitrateTextView = findViewById(R.id.tv_local_video_bitrate);
        mLocalVideoWidthTextView = findViewById(R.id.tv_local_video_width);
        mLocalVideoHeightTextView = findViewById(R.id.tv_local_video_height);
        mLocalVideoCaptureFPSTextView = findViewById(R.id.tv_local_video_capture_fps);
        mLocalVideoSendFPSTextView = findViewById(R.id.tv_local_video_send_fps);

        mRemoteAudioBitrateTextView = findViewById(R.id.tv_remote_audio_bitrate);
        mRemoteVideoBitrateTextView = findViewById(R.id.tv_remote_video_bitrate);
        mRemoteVideoWidthTextView = findViewById(R.id.tv_remote_video_width);
        mRemoteVideoHeightTextView = findViewById(R.id.tv_remote_video_height);
        mRemoteVideoDecodeFPSTextView = findViewById(R.id.tv_remote_video_decode_fps);
        mRemoteVideoRenderFPSTextView = findViewById(R.id.tv_remote_video_render_fps);

        initDingEngine();

        joinChannel();
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
            mRemoteVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
            mLocalViewHolder.addView(localView, 0);
            mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackCamera);
            mRtcEngine.startPreview();

            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {

                @Override
                public void onDingRtcStats(DingRtcEngine.DingRtcStats rtcStats) {
                    super.onDingRtcStats(rtcStats);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mSendAudioBitrateTextView.setText(getString(R.string.rtc_stats_tx_audio_bitrate, rtcStats.txAudioKBitrate));
                            mSendVideoBitrateTextView.setText(getString(R.string.rtc_stats_tx_video_bitrate, rtcStats.txVideoKBitrate));
                            mSendLossrateTextView.setText(getString(R.string.rtc_stats_tx_loss_rate, rtcStats.txPacketLossRate));

                            mReceiveAudioBitrateTextView.setText(getString(R.string.rtc_stats_rx_audio_bitrate, rtcStats.rxAudioKBitrate));
                            mReceiveVideoBitrateTextView.setText(getString(R.string.rtc_stats_rx_video_bitrate, rtcStats.rxVideoKBitrate));
                            mReceiveLossrateTextView.setText(getString(R.string.rtc_stats_rx_loss_rate, rtcStats.rxPacketLossRate));
                        }
                    });
                }

                @Override
                public void onRtcLocalAudioStats(DingRtcEngine.DingRtcLocalAudioStats localAudioStats) {
                    super.onRtcLocalAudioStats(localAudioStats);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mLocalAudioBitrateTextView.setText(getString(R.string.tv_local_audio_bitrate, localAudioStats.sentBitrate/1000));
                        }
                    });
                }

                @Override
                public void onRtcLocalVideoStats(DingRtcEngine.DingRtcLocalVideoStats localVideoStats) {
                    super.onRtcLocalVideoStats(localVideoStats);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mLocalVideoBitrateTextView.setText(getString(R.string.tv_local_video_bitrate, localVideoStats.sentBitrate/1000));
                            mLocalVideoWidthTextView.setText(getString(R.string.tv_local_video_width, localVideoStats.encoderFrameWidth));
                            mLocalVideoHeightTextView.setText(getString(R.string.tv_local_video_height, localVideoStats.encoderFrameHeight));
                            mLocalVideoCaptureFPSTextView.setText(getString(R.string.tv_local_video_capture_fps, localVideoStats.captureFps));
                            mLocalVideoSendFPSTextView.setText(getString(R.string.tv_local_video_send_fps, localVideoStats.sentFps));
                        }
                    });
                }

                @Override
                public void onRtcRemoteAudioStats(DingRtcEngine.DingRtcRemoteAudioStats remoteAudioStats) {
                    super.onRtcRemoteAudioStats(remoteAudioStats);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mRemoteAudioBitrateTextView.setText(getString(R.string.tv_remote_audio_bitrate, remoteAudioStats.recvBitrate/1000));
                        }
                    });
                }

                @Override
                public void onRtcRemoteVideoStats(DingRtcEngine.DingRtcRemoteVideoStats remoteVideoStats) {
                    super.onRtcRemoteVideoStats(remoteVideoStats);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mRemoteVideoBitrateTextView.setText(getString(R.string.tv_remote_video_bitrate, remoteVideoStats.recvBitrate/1000));
                            mRemoteVideoWidthTextView.setText(getString(R.string.tv_remote_video_width, remoteVideoStats.width));
                            mRemoteVideoHeightTextView.setText(getString(R.string.tv_remote_video_height, remoteVideoStats.height));
                            mRemoteVideoDecodeFPSTextView.setText(getString(R.string.tv_remote_video_decode_fps, remoteVideoStats.decoderOutputFrameRate));
                            mRemoteVideoRenderFPSTextView.setText(getString(R.string.tv_remote_video_render_fps, remoteVideoStats.rendererOutputFrameRate));
                        }
                    });
                }

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

                                    SurfaceView localView = mRtcEngine.createRenderSurfaceView(RtcStatsActivity.this);
                                    mRemoteVideoCanvas.view = localView;
                                    mRemoteViewHolder.addView(localView, 0);

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
            mRtcEngine.leaveChannel();
            mRtcEngine.destroy();
            mRtcEngine = null;
        }
    }
}
