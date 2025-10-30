package com.ding.rtc.apiexample.advanced.videorendersetting;

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
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.Spinner;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

public class VideoRenderSettingActivity extends AppCompatActivity implements View.OnClickListener {

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
    private FrameLayout mRemoteViewHolder;
    private Spinner mLocalRenderModeSpinner;
    private Spinner mLocalMirrorModeSpinner;
    private Spinner mLocalRotationModeSpinner;
    private Spinner mRemoteRenderModeSpinner;
    private Spinner mRemoteMirrorModeSpinner;
    private Spinner mRemoteRotationModeSpinner;

    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, VideoRenderSettingActivity.class);
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
        setContentView(R.layout.activity_video_render_setting);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.video_render_setting);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder = findViewById(R.id.remote_view);
        mLocalRenderModeSpinner = findViewById(R.id.spinner_local_render_mode);
        mLocalMirrorModeSpinner = findViewById(R.id.spinner_local_mirror_mode);
        mLocalRotationModeSpinner = findViewById(R.id.spinner_local_rotation_mode);
        mRemoteRenderModeSpinner = findViewById(R.id.spinner_remote_render_mode);
        mRemoteMirrorModeSpinner = findViewById(R.id.spinner_remote_mirror_mode);
        mRemoteRotationModeSpinner = findViewById(R.id.spinner_remote_rotation_mode);

        ArrayAdapter<CharSequence> renderModeAdapter = ArrayAdapter.createFromResource(this,
                R.array.render_mode, android.R.layout.simple_spinner_item);
        renderModeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mLocalRenderModeSpinner.setAdapter(renderModeAdapter);
        mRemoteRenderModeSpinner.setAdapter(renderModeAdapter);
        mLocalRenderModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int index, long l) {
                switch (index) {
                    case 0:
                        mLocalVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeAuto;
                        break;
                    case 1:
                        mLocalVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeStretch;
                        break;
                    case 2:
                        mLocalVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeFill;
                        break;
                    case 3:
                        mLocalVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
                        break;
                }
                mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackCamera);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        mRemoteRenderModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int index, long l) {
                switch (index) {
                    case 0:
                        mRemoteVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeAuto;
                        break;
                    case 1:
                        mRemoteVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeStretch;
                        break;
                    case 2:
                        mRemoteVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeFill;
                        break;
                    case 3:
                        mRemoteVideoCanvas.renderMode = DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
                        break;
                }
                if (mRemoteUserId != null) {
                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas, mRemoteUserId, DingRtcVideoTrackCamera);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        ArrayAdapter<CharSequence> mirrorModeAdapter = ArrayAdapter.createFromResource(this,
                R.array.mirror_mode, android.R.layout.simple_spinner_item);
        mirrorModeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mLocalMirrorModeSpinner.setAdapter(mirrorModeAdapter);
        mRemoteMirrorModeSpinner.setAdapter(mirrorModeAdapter);
        mLocalMirrorModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int index, long l) {
                switch (index) {
                    case 0:
                        mLocalVideoCanvas.mirrorMode = DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeOnlyFront;
                        break;
                    case 1:
                        mLocalVideoCanvas.mirrorMode = DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeAllEnabled;
                        break;
                    case 2:
                        mLocalVideoCanvas.mirrorMode = DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeAllDisable;
                        break;
                }
                mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackCamera);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        mRemoteMirrorModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int index, long l) {
                switch (index) {
                    case 0:
                        mRemoteVideoCanvas.mirrorMode = DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeOnlyFront;
                        break;
                    case 1:
                        mRemoteVideoCanvas.mirrorMode = DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeAllEnabled;
                        break;
                    case 2:
                        mRemoteVideoCanvas.mirrorMode = DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeAllDisable;
                        break;
                }
                if (mRemoteUserId != null) {
                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas, mRemoteUserId, DingRtcVideoTrackCamera);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        ArrayAdapter<CharSequence> rotationModeAdapter = ArrayAdapter.createFromResource(this,
                R.array.rotation_mode, android.R.layout.simple_spinner_item);
        rotationModeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mLocalRotationModeSpinner.setAdapter(rotationModeAdapter);
        mRemoteRotationModeSpinner.setAdapter(rotationModeAdapter);
        mLocalRotationModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int index, long l) {
                switch (index) {
                    case 0:
                        mLocalVideoCanvas.rotationMode = DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_0;
                        break;
                    case 1:
                        mLocalVideoCanvas.rotationMode = DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_90;
                        break;
                    case 2:
                        mLocalVideoCanvas.rotationMode = DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_180;
                        break;
                    case 3:
                        mLocalVideoCanvas.rotationMode = DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_270;
                        break;
                }
                mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackCamera);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        mRemoteRotationModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int index, long l) {
                switch (index) {
                    case 0:
                        mRemoteVideoCanvas.rotationMode = DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_0;
                        break;
                    case 1:
                        mRemoteVideoCanvas.rotationMode = DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_90;
                        break;
                    case 2:
                        mRemoteVideoCanvas.rotationMode = DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_180;
                        break;
                    case 3:
                        mRemoteVideoCanvas.rotationMode = DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_270;
                        break;
                }
                if (mRemoteUserId != null) {
                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas, mRemoteUserId, DingRtcVideoTrackCamera);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

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
                                    mRemoteUserId = null;
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas, uid, DingRtcVideoTrackCamera);

                                    mRtcEngine.subscribeRemoteVideoStream(uid, DingRtcVideoTrackCamera, false);
                                }
                            } else {
                                if (videoTrack == DingRtcVideoTrackCamera || videoTrack == DingRtcVideoTrackBoth) {
                                    hasRemoteVideo = true;

                                    SurfaceView localView = mRtcEngine.createRenderSurfaceView(VideoRenderSettingActivity.this);
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
