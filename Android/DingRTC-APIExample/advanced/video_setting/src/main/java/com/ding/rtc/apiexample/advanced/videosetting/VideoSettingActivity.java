package com.ding.rtc.apiexample.advanced.videosetting;

import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackBoth;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackNo;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackScreen;
import static com.ding.rtc.DingRtcEngine.VD_1280x720;
import static com.ding.rtc.DingRtcEngine.VD_1920x1080;
import static com.ding.rtc.DingRtcEngine.VD_640x360;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.FrameLayout;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class VideoSettingActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";
    private static final String INTENT_IS_PUBLISHER = "is_publisher";
    private static final int MIN_BITRATE = 300;
    private static final int MAX_BITRATE = 3096;

    private DingRtcEngine mRtcEngine;
    private ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private String mChannelId;
    private String mUserId;
    private boolean isPublisher = true;
    private boolean needPublishVideo = true;

    private FrameLayout mLocalViewHolder;
    private FrameLayout mRemoteViewHolder;
    private ViewGroup mLocalViewGroup;
    private ViewGroup mRemoteViewGroup;
    private Spinner mResolutionSpinner;
    private Spinner mFrameRateSpinner;
    private Spinner mOrientationModeSpinner;
    private SeekBar mBitrateSeekBar;
    private TextView mBitrateValue;
    private View mVideoSettingViewGroup;
    private Button mOperationButton;


    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;
    private DingRtcEngine.DingRtcVideoEncoderConfiguration mVideoEncodeEncoderConfiguration = new DingRtcEngine.DingRtcVideoEncoderConfiguration();

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token, boolean publisher) {
        Intent intent = new Intent();
        intent.setClass(context, VideoSettingActivity.class);
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
        setContentView(R.layout.activity_video_setting);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.video_setting);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }
        isPublisher = getIntent().getBooleanExtra(INTENT_IS_PUBLISHER, true);
        mLocalViewGroup = findViewById(R.id.local_view_group);
        mRemoteViewGroup = findViewById(R.id.remote_view_group);
        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder = findViewById(R.id.remote_view);
        mResolutionSpinner = findViewById(R.id.spinner_resolution);
        mFrameRateSpinner = findViewById(R.id.spinner_framerate);
        mOrientationModeSpinner = findViewById(R.id.spinner_orientation_mode);
        mBitrateSeekBar = findViewById(R.id.seekBar_bitrate);
        mBitrateValue = findViewById(R.id.tv_bitrate_value);
        mVideoSettingViewGroup = findViewById(R.id.video_setting_view_group);
        mOperationButton = findViewById(R.id.btn_operation);

        ArrayAdapter<CharSequence> resAdapter = ArrayAdapter.createFromResource(this,
                R.array.resolution_options, android.R.layout.simple_spinner_item);
        resAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mResolutionSpinner.setAdapter(resAdapter);
        mResolutionSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int index, long l) {
                switch (index) {
                    case 0:
                        mVideoEncodeEncoderConfiguration.dimensions = VD_1920x1080;
                        break;
                    case 1:
                        mVideoEncodeEncoderConfiguration.dimensions = VD_1280x720;
                        break;
                    case 2:
                        mVideoEncodeEncoderConfiguration.dimensions = VD_640x360;
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        ArrayAdapter<CharSequence> fpsAdapter = ArrayAdapter.createFromResource(this,
                R.array.framerate_options, android.R.layout.simple_spinner_item);
        mFrameRateSpinner.setAdapter(fpsAdapter);
        mFrameRateSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int index, long l) {
                switch (index) {
                    case 0:
                        mVideoEncodeEncoderConfiguration.frameRate = 30;
                        break;
                    case 1:
                        mVideoEncodeEncoderConfiguration.frameRate = 20;
                        break;
                    case 2:
                        mVideoEncodeEncoderConfiguration.frameRate = 15;
                        break;
                    case 3:
                        mVideoEncodeEncoderConfiguration.frameRate = 10;
                        break;
                    case 4:
                        mVideoEncodeEncoderConfiguration.frameRate = 5;
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        ArrayAdapter<CharSequence> orientationModeAdapter = ArrayAdapter.createFromResource(this,
                R.array.orientation_options, android.R.layout.simple_spinner_item);
        mOrientationModeSpinner.setAdapter(orientationModeAdapter);
        mOrientationModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int index, long l) {
                switch (index) {
                    case 0:
                        mVideoEncodeEncoderConfiguration.orientationMode = DingRtcEngine.DingRtcVideoEncoderOrientationMode.DingRtcVideoEncoderOrientationModeAdaptive;
                        break;
                    case 1:
                        mVideoEncodeEncoderConfiguration.orientationMode = DingRtcEngine.DingRtcVideoEncoderOrientationMode.DingRtcVideoEncoderOrientationModeFixedLandscape;
                        break;
                    case 2:
                        mVideoEncodeEncoderConfiguration.orientationMode = DingRtcEngine.DingRtcVideoEncoderOrientationMode.DingRtcVideoEncoderOrientationModeFixedPortrait;
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        mBitrateSeekBar.setMax(MAX_BITRATE - MIN_BITRATE);
        mBitrateSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                int actualValue = progress + MIN_BITRATE;
                mBitrateValue.setText(actualValue + " kbps");
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
        mBitrateSeekBar.setProgress((MAX_BITRATE - MIN_BITRATE) / 2);

        // in order to reserve a larger View for observing video stream,
        // the publisher end the subscriber are split into two separate terminals.
        if (isPublisher) {
            mRemoteViewGroup.setVisibility(View.GONE);
        } else {
            mLocalViewGroup.setVisibility(View.GONE);
            mVideoSettingViewGroup.setVisibility(View.GONE);
        }

        initDingEngine();
        joinChannel();
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");

            if (isPublisher) {
                mVideoSettingViewGroup.setVisibility(View.VISIBLE);
                mRtcEngine.subscribeAllRemoteAudioStreams(false);
                mRtcEngine.subscribeAllRemoteVideoStreams(false);
            } else {
                mRtcEngine.publishLocalVideoStream(false);
                mRtcEngine.publishLocalAudioStream(false);
                mRtcEngine.subscribeAllRemoteAudioStreams(true);
                mRtcEngine.subscribeAllRemoteVideoStreams(true);
                mRemoteVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
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

                                    SurfaceView localView = mRtcEngine.createRenderSurfaceView(VideoSettingActivity.this);
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

    public int getCurrentBitrate() {
        return mBitrateSeekBar.getProgress() + MIN_BITRATE;
    }

    public void onPublishButtonClick(View view) {
        if (needPublishVideo) {
            mVideoEncodeEncoderConfiguration.bitrate = getCurrentBitrate();
            mRtcEngine.publishLocalVideoStream(true);
            SurfaceView localView = mRtcEngine.createRenderSurfaceView(this);
            mLocalVideoCanvas.view = localView;
            mLocalVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
            mLocalViewHolder.addView(localView);
            mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackCamera);
            mRtcEngine.startPreview();
            needPublishVideo = false;
            mOperationButton.setText(R.string.video_setting_unpublish);
        } else {
            mRtcEngine.stopPreview();
            mRtcEngine.publishLocalVideoStream(false);
            needPublishVideo = true;
            mOperationButton.setText(R.string.video_setting_publish);
        }
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