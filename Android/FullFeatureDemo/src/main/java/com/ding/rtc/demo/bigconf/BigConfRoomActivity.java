package com.ding.rtc.demo.bigconf;

import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_BlueTooth;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_Earpiece;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_Headset;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_HeadsetNoMic;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_LoudSpeaker;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_Speakerphone;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioTrack.DingRtcAudioTrackMic;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioTrack.DingRtcAudioTrackNo;
import static com.ding.rtc.DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_0;
import static com.ding.rtc.DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_180;
import static com.ding.rtc.DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_270;
import static com.ding.rtc.DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_90;
import static com.ding.rtc.DingRtcEngine.DingRtcSubscribeState.DingRtcStatsNoSubscribe;
import static com.ding.rtc.DingRtcEngine.DingRtcSubscribeState.DingRtcStatsSubscribed;
import static com.ding.rtc.DingRtcEngine.DingRtcSubscribeState.DingRtcStatsSubscribing;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackBoth;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackNo;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackScreen;
import static com.ding.rtc.DingRtcEngine.VD_120x120;
import static com.ding.rtc.DingRtcEngine.VD_1280x720;
import static com.ding.rtc.DingRtcEngine.VD_160x120;
import static com.ding.rtc.DingRtcEngine.VD_180x180;
import static com.ding.rtc.DingRtcEngine.VD_1920x1080;
import static com.ding.rtc.DingRtcEngine.VD_240x180;
import static com.ding.rtc.DingRtcEngine.VD_240x240;
import static com.ding.rtc.DingRtcEngine.VD_320x180;
import static com.ding.rtc.DingRtcEngine.VD_320x240;
import static com.ding.rtc.DingRtcEngine.VD_360x360;
import static com.ding.rtc.DingRtcEngine.VD_424x240;
import static com.ding.rtc.DingRtcEngine.VD_480x360;
import static com.ding.rtc.DingRtcEngine.VD_480x480;
import static com.ding.rtc.DingRtcEngine.VD_640x360;
import static com.ding.rtc.DingRtcEngine.VD_640x480;
import static com.ding.rtc.DingRtcEngine.VD_840x480;
import static com.ding.rtc.DingRtcEngine.VD_960x540;
import static com.ding.rtc.DingRtcEngine.VD_960x720;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.PopupWindow;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;
import com.ding.rtc.DingRtcErrorDescription;
import com.ding.rtc.DingRtcRemoteUserInfo;
import com.ding.rtc.DingRtcSubscribeErrorDescription;
import com.ding.rtc.demo.BaseActivity;
import com.ding.rtc.demo.ui.QualityReportDialog;
import com.ding.rtc.demo.R;
import com.ding.rtc.demo.settings.SettingActivity;
import com.ding.rtc.demo.ui.DrawableTextView;
import com.ding.rtc.demo.utils.DensityUtils;
import com.ding.rtc.demo.whiteboard.WhiteboardActivity;
import com.google.android.material.floatingactionbutton.FloatingActionButton;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public class BigConfRoomActivity extends BaseActivity {

    private static final String TAG = "BigConfRoomActivity->";
    private static final String INTENT_MEETING_USER_ID = "meeting_user_id";
    private static final String INTENT_MEETING_TOKEN = "meeting_token";
    private static final String INTENT_MEETING_USER_NAME = "meeting_user_name";
    private static final String INTENT_MEETING_APP_ID = "meeting_app_id";
    private static final String INTENT_MEETING_CHANNEL_ID = "meeting_channel_id";
    private static final String INTENT_MEETING_RENDER_COLUMN_COUNT = "meeting_render_column_count";

    public static void launch(Context context, String userId, String userName, String token,
                              String appId, String channelName, int renderGridCount) {
        Intent intent = new Intent();
        intent.setClass(context, BigConfRoomActivity.class);
        intent.putExtra(INTENT_MEETING_USER_ID, userId);
        intent.putExtra(INTENT_MEETING_USER_NAME, userName);
        intent.putExtra(INTENT_MEETING_TOKEN, token);
        intent.putExtra(INTENT_MEETING_APP_ID, appId);
        intent.putExtra(INTENT_MEETING_CHANNEL_ID, channelName);
        intent.putExtra(INTENT_MEETING_RENDER_COLUMN_COUNT, renderGridCount);
        context.startActivity(intent);
    }

    private PopupWindow mPopupWindow;
    private ProgressDialog mProgressDialog;
    private boolean mIsPortrait = true;
    private RecyclerView mRoomUsersView;
    private BigConfUsersAdapter mBigConfUsersAdapter;
    private GridLayoutManager mLayoutManager;
    private View mTitleView;
    private View mBottomView;
    private TextView mTitleText;
    private TextView mConnectStateText;

    private TextView mRoomInfo;
    private boolean mSettingSubAllAudio;
    private boolean mLastSubAllAudio = false;
    private DingRtcEngine.DingRtcVideoStreamType mDefaultSubStreamType;

    //rtc
    private final DingRtcAuthInfo mAuthInfo = new DingRtcAuthInfo();
    private DingRtcEngine mRtcEngine;
    private final BigConfUser mUserSelf = new BigConfUser();
    private int mPublishResolution = 16;
    private int mPublishFps = 30;
    private int mScreenSharePublishResolution = 16;
    private int mScreenSharePublishFps = 30;
    private boolean mIsJoin = false;
    private boolean mIsFront = true;
    private CountDownTimer countDownTimer;
    private int cameraIdx = 0;
    private boolean useSwitchCameraId = false;

    private int cameraMaxBitrate;
    private int screenMaxBitrate;

    ImageView mSpeakerView;
    AlertDialog mAudioRouteTypeDialog;

    DingRtcEngine.DingRtcVideoEncoderConfiguration mVideoEncoderConfiguration;

    private boolean mIsAudioDumpStart = false;
    private boolean mStartAudioCapture = true;
    private boolean mStartAudioPlay = true;
    private boolean mMuteAudioRecordingSignal = false;
    private int mPlayoutVolume = 100;
    private int mRecordingVolume = 100;

    // QualityReport
    private QualityReportDialog mQualityReportDialog;

    private int mRenderColumnCount = 3;
    private int mUserCountPerPage = 9;
    private int mCurrentSelectedPage = 1;
    private final LinkedHashMap<String, BigConfUser> mAllUsers = new LinkedHashMap<>(); // <uid, user>
    private FloatingActionButton mNextPageBtn;
    private FloatingActionButton mPrePageBtn;

    private boolean mMuteAll = false;

    @Override
    public void onBackPressed() {
        if (mIsJoin) {
            leaveChannel();
        } else {
            super.onBackPressed();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mUserSelf.isSelf = true;
        configFromPreference(PreferenceManager.getDefaultSharedPreferences(this), true);
        initRtcEngine();
        setContentView(R.layout.activity_big_conf);
        configFromIntent(getIntent());
        initTitleView();
        initBottomView();
        initContentView();
        addMySelf();

        joinChannel();
    }

    @Override
    protected void onResume() {
        super.onResume();

        mRtcEngine.enableBeautyFace(false, null);

        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);

        float minExposureCompensation = mRtcEngine.getCameraMinExposureCompensation();
        float maxExposureCompensation = mRtcEngine.getCameraMaxExposureCompensation();
        if (minExposureCompensation != 0.0 && maxExposureCompensation != 0.0
                && (-minExposureCompensation == maxExposureCompensation)) {
            int cameraBrightnessKey = preferences.getInt(getString(R.string.camera_brightness_key), 0);
            mRtcEngine.setCameraExposureCompensation(cameraBrightnessKey * maxExposureCompensation / 100);
        }

        String faceBeautyPos = preferences.getString(getString(R.string.title_face_beauty_key), "0");
        if ("1".equals(faceBeautyPos)) {
            DingRtcEngine.DingRtcBeautyFaceOptions options = new DingRtcEngine.DingRtcBeautyFaceOptions();
            options.resourcePath = "n/a";
            options.enableSkinBuffing = true;
            options.skinBuffingFactor = preferences.getFloat("skin_buffing", 0.5f);
            options.skinSharpenFactor = preferences.getFloat("skin_sharpen", 0.5f);
            options.enableSkinWhitening = true;
            options.skinWhitingFactor = preferences.getFloat("skin_whitening", 0.5f);
            options.enableSkinRosy = true;
            options.skinRosyFactor = preferences.getFloat("skin_rosy", 0.0f);
            String filterPath = preferences.getString("filter_path", "");
            options.enableFilter = !TextUtils.isEmpty(filterPath);
            options.filterPath = filterPath;
            options.filterFactor = 1.0f;
            mRtcEngine.enableBeautyFace(true, options);
        }

        if (mLastSubAllAudio != mSettingSubAllAudio) {
            mRtcEngine.subscribeAllRemoteAudioStreams(mSettingSubAllAudio);
            mLastSubAllAudio = mSettingSubAllAudio;
        }

        String audioDenoiseStr = preferences.getString(getString(R.string.audio_denoise_key), "0");
        DingRtcEngine.DingRtcEngineAudioDenoiseMode mode;
        if ("1".equals(audioDenoiseStr)) {
            mode = DingRtcEngine.DingRtcEngineAudioDenoiseMode.DingRtcEngineAudioDenoiseDsp;
        } else if ("2".equals(audioDenoiseStr)) {
            mode = DingRtcEngine.DingRtcEngineAudioDenoiseMode.DingRtcEngineAudioDenoiseEnhance;
        } else {
            mode = DingRtcEngine.DingRtcEngineAudioDenoiseMode.DingRtcEngineAudioDenoiseOff;
        }
        DingRtcEngine.DingRtcEngineAudioDenoiseOptions options = new DingRtcEngine.DingRtcEngineAudioDenoiseOptions();
        options.mode = mode;
        mRtcEngine.setAudioDenoise(options);

        boolean videoDenoise = preferences.getBoolean(getString(R.string.video_denoise_key), false);
        DingRtcEngine.DingRtcEngineVideoDenoiseOptions videoDenoiseOptions = new DingRtcEngine.DingRtcEngineVideoDenoiseOptions();
        videoDenoiseOptions.mode = videoDenoise ? DingRtcEngine.DingRtcEngineVideoDenoiseMode.DingRtcEngineVideoDenoiseAuto
                : DingRtcEngine.DingRtcEngineVideoDenoiseMode.DingRtcEngineVideoDenoiseOff;
        mRtcEngine.setVideoDenoise(videoDenoiseOptions);

        boolean videoEnhance = preferences.getBoolean(getString(R.string.video_enhance_key), false);
        DingRtcEngine.DingRtcEngineVideoEnhanceOptions videoEnhanceOptions = new DingRtcEngine.DingRtcEngineVideoEnhanceOptions();
        videoEnhanceOptions.mode = videoEnhance ? DingRtcEngine.DingRtcEngineVideoEnhanceMode.DingRtcEngineVideoEnhanceAuto
                : DingRtcEngine.DingRtcEngineVideoEnhanceMode.DingRtcEngineVideoEnhanceOff;
        mRtcEngine.setVideoEnhance(videoEnhanceOptions);

        boolean audioIndication = preferences.getBoolean(getString(R.string.audio_volume_indication_key), false);
        if (audioIndication) {
            mRtcEngine.enableAudioVolumeIndication(300, 3, 1);
        } else {
            mRtcEngine.enableAudioVolumeIndication(0, 3, 0);
        }

        boolean isCameraAutoFocusFaceModeSupported = mRtcEngine.isCameraAutoFocusFaceModeSupported();
        boolean enableFocusFace = preferences.getBoolean(getString(R.string.video_focus_face_key), false);
        if (enableFocusFace) {
            if (isCameraAutoFocusFaceModeSupported) {
                mRtcEngine.setCameraAutoFocusFaceModeEnabled(true);
            } else {
                showToast("不支持人脸聚焦");
            }
        } else if (isCameraAutoFocusFaceModeSupported) {
            mRtcEngine.setCameraAutoFocusFaceModeEnabled(false);
        }

        boolean videoStabilization = preferences.getBoolean(getString(R.string.video_stabilization_key), false);
        DingRtcEngine.DingRtcCameraStabilizationMode cameraStabilizationMode = videoStabilization ? DingRtcEngine.DingRtcCameraStabilizationMode.AUTO : DingRtcEngine.DingRtcCameraStabilizationMode.OFF;
        mRtcEngine.setCameraStabilizationMode(cameraStabilizationMode);

        useSwitchCameraId = preferences.getBoolean(getString(R.string.camera_switch_id_key), false);

        String cameraRotation = preferences.getString(getString(R.string.title_camera_rotation_key), "0");
        DingRtcEngine.DingRtcRotationMode rotation = DingRtcRotationMode_0;
        if ("0".equals(cameraRotation)) {
            rotation = DingRtcRotationMode_0;
        } else if ("90".equals(cameraRotation)) {
            rotation = DingRtcRotationMode_90;
        } else if ("180".equals(cameraRotation)) {
            rotation = DingRtcRotationMode_180;
        } else if ("270".equals(cameraRotation)) {
            rotation = DingRtcRotationMode_270;
        }
        if (mVideoEncoderConfiguration == null) {
            mVideoEncoderConfiguration = new DingRtcEngine.DingRtcVideoEncoderConfiguration();
        }
        mVideoEncoderConfiguration.frameRate = mPublishFps;
        mVideoEncoderConfiguration.dimensions = getDingRtcVideoDimensions(mPublishResolution);
        mVideoEncoderConfiguration.rotation = rotation;

        DingRtcEngine.DingRtcVideoEncoderOrientationMode encoderOrientationMode;
        String encoderOrientation = preferences.getString(getString(R.string.video_encoder_orientation_key), "0");
        if ("1".equals(encoderOrientation)) {
            encoderOrientationMode = DingRtcEngine.DingRtcVideoEncoderOrientationMode.DingRtcVideoEncoderOrientationModeFixedLandscape;
        } else if ("2".equals(encoderOrientation)) {
            encoderOrientationMode = DingRtcEngine.DingRtcVideoEncoderOrientationMode.DingRtcVideoEncoderOrientationModeFixedPortrait;
        } else {
            encoderOrientationMode = DingRtcEngine.DingRtcVideoEncoderOrientationMode.DingRtcVideoEncoderOrientationModeAdaptive;
        }
        mVideoEncoderConfiguration.orientationMode = encoderOrientationMode;
        boolean mirror = preferences.getBoolean(getString(R.string.title_pub_video_mirror_key), false);
        mVideoEncoderConfiguration.mirrorMode = mirror ? DingRtcEngine.DingRtcMirrorMode.DingRtcMirrorMode_OnlyPub : DingRtcEngine.DingRtcMirrorMode.DingRtcMirrorMode_Disable;

        String cameraMaxBitrateValue = preferences.getString(getString(R.string.video_max_bitrate_key), "0");
        if (!TextUtils.isEmpty(cameraMaxBitrateValue)) {
            cameraMaxBitrate = parseInt(cameraMaxBitrateValue);
            Log.d(TAG, "cameraMaxBitrate1 = " + cameraMaxBitrate);
            mVideoEncoderConfiguration.bitrate = cameraMaxBitrate;
        }

        mRtcEngine.setVideoEncoderConfiguration(mVideoEncoderConfiguration);

        String screenMaxBitrateValue = preferences.getString(getString(R.string.screen_max_bitrate_key), "0");
        if (!TextUtils.isEmpty(screenMaxBitrateValue)) {
            screenMaxBitrate = parseInt(screenMaxBitrateValue);
            DingRtcEngine.DingRtcScreenShareEncoderConfiguration screenShareEncoderConfiguration = new DingRtcEngine.DingRtcScreenShareEncoderConfiguration();
            screenShareEncoderConfiguration.bitrate = screenMaxBitrate;
            screenShareEncoderConfiguration.frameRate = mScreenSharePublishFps;
            screenShareEncoderConfiguration.dimensions = getDingRtcVideoDimensions(mScreenSharePublishResolution);
            Log.d(TAG, "screenMaxBitrate1 = " + screenMaxBitrate);
            mRtcEngine.setScreenShareEncoderConfiguration(screenShareEncoderConfiguration);
        }

        boolean enableVideoDump = preferences.getBoolean(getString(R.string.video_dump_id_key), false);
        if (enableVideoDump) {
            mRtcEngine.setParameters("{\"video_dump_config\":{\"enable\":true}}");
        } else {
            mRtcEngine.setParameters("{\"video_dump_config\":{\"enable\":false}}");
        }

        int playoutVolume = mRtcEngine.getPlayoutVolume();
        if (playoutVolume != mPlayoutVolume) {
            mRtcEngine.setPlayoutVolume(mPlayoutVolume);
        }
        int recordingVolume = mRtcEngine.getRecordingVolume();
        if (recordingVolume != mRecordingVolume) {
            mRtcEngine.setRecordingVolume(mRecordingVolume);
        }
        showToast("音频播放音量：" + mRtcEngine.getPlayoutVolume() + "\n" + "音频采集音量：" + mRtcEngine.getRecordingVolume());

    }

    @Override
    protected void onDestroy() {
        if (mRtcEngine != null) {
            mRtcEngine.destroy();
            mRtcEngine = null;
        }
        super.onDestroy();
        if (countDownTimer != null) {
            countDownTimer.cancel();
        }
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if (newConfig.orientation == Configuration.ORIENTATION_PORTRAIT) {
            mIsPortrait = true;
        }
        if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            mIsPortrait = false;
        }
        updateUIOnConfigurationChanged();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == SettingActivity.SETTING_REQUEST_CODE) {
            configFromPreference(PreferenceManager.getDefaultSharedPreferences(this), false);
        }
    }

    private void configFromPreference(SharedPreferences preferences, boolean first) {
        mSettingSubAllAudio = preferences.getBoolean(getString(R.string.title_subscribe_all_audio_key), true);
        mDefaultSubStreamType = DingRtcEngine.DingRtcVideoStreamType.fromValue(Integer.parseInt(preferences.getString(getString(R.string.demo_subscribe_resolution_key), "4")));
        mPublishResolution = Integer.parseInt(preferences.getString(getString(R.string.demo_publish_resolution_key), "16"));
        mPublishFps = Integer.parseInt(preferences.getString(getString(R.string.demo_publish_fps_key), "30"));
        mScreenSharePublishResolution = Integer.parseInt(preferences.getString(getString(R.string.demo_screen_share_publish_resolution_key), "16"));
        mScreenSharePublishFps = Integer.parseInt(preferences.getString(getString(R.string.demo_screen_share_publish_fps_key), "15"));

        if (first) {
            mUserSelf.isVideoStarted = preferences.getBoolean(getString(R.string.title_auto_start_camera_key), true);
            mUserSelf.isVideoPreview = mUserSelf.isVideoStarted;
            mUserSelf.isAudioStarted = preferences.getBoolean(getString(R.string.title_auto_start_audio_key), true);
            mUserSelf.isVideoEnabled = preferences.getBoolean(getString(R.string.title_auto_start_camera_key), true);
        }

        if (mRtcEngine != null) {
            boolean startAudioCapture = preferences.getBoolean(getString(R.string.audio_capture_key), true);
            boolean startAudioPlay = preferences.getBoolean(getString(R.string.audio_play_key), true);
            boolean muteAudioRecordingSignal = preferences.getBoolean(getString(R.string.audio_mute_recording_signal_key), true);
            if (startAudioCapture != mStartAudioCapture) {
                if (startAudioCapture) {
                    mRtcEngine.startAudioCapture();
                } else {
                    mRtcEngine.stopAudioCapture();
                }
                mStartAudioCapture = startAudioCapture;
            }

            if (startAudioPlay != mStartAudioPlay) {
                if (startAudioPlay) {
                    mRtcEngine.startAudioPlayer();
                } else {
                    mRtcEngine.stopAudioPlayer();
                }
                mStartAudioPlay = startAudioPlay;
            }
            if (muteAudioRecordingSignal != mMuteAudioRecordingSignal) {
                mRtcEngine.muteRecordingSignal(muteAudioRecordingSignal);
                mMuteAudioRecordingSignal = muteAudioRecordingSignal;
            }
        }
        String playoutVolume = preferences.getString(getString(R.string.audio_playout_volume), "100");
        String recordingVolume = preferences.getString(getString(R.string.audio_recording_volume), "100");
        try {
            mPlayoutVolume = Integer.parseInt(playoutVolume);
            mRecordingVolume = Integer.parseInt(recordingVolume);
        } catch (Exception e) {
            mPlayoutVolume = 100;
            mRecordingVolume = 100;
        }

    }

    private void configFromIntent(Intent intent) {
        mAuthInfo.userId = intent.getStringExtra(INTENT_MEETING_USER_ID);
        mUserSelf.nickName = intent.getStringExtra(INTENT_MEETING_USER_NAME);
        mAuthInfo.token = intent.getStringExtra(INTENT_MEETING_TOKEN);

        mAuthInfo.appId = intent.getStringExtra(INTENT_MEETING_APP_ID);
        mAuthInfo.channelId = intent.getStringExtra(INTENT_MEETING_CHANNEL_ID);

        mRenderColumnCount = intent.getIntExtra(INTENT_MEETING_RENDER_COLUMN_COUNT, 3);
        mUserCountPerPage = mRenderColumnCount * mRenderColumnCount;
    }

    private void initTitleView() {
        mTitleView = findViewById(R.id.room_top_layout);
        mTitleText = findViewById(R.id.tv_title);
        mTitleText.setText(R.string.title_join_call);
        mConnectStateText = findViewById(R.id.tv_connect_state);

        mSpeakerView = findViewById(R.id.iv_left_icon);
        mSpeakerView.setImageResource(R.drawable.speaker);
        mSpeakerView.setOnClickListener(view -> {
            final List<DingRtcEngine.DingRtcAudioDeviceInfo> audiDevices = mRtcEngine.getAudioDevices();
            int selectedIndex = 0;
            final String[] items = new String[audiDevices.size()];
            for (int i = 0; i < audiDevices.size(); i++) {
                String device = getAudioDeviceName(audiDevices.get(i).type);
                items[i] = device;
                if (audiDevices.get(i).isUsed) {
                    selectedIndex = i;
                }
            }
            mAudioRouteTypeDialog = new AlertDialog.Builder(this)
                    .setSingleChoiceItems(items, selectedIndex, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            DingRtcEngine.DingRtcAudioDeviceInfo deviceInfo = audiDevices.get(which);
                            showToast("选择了音频设备 : " + items[which]);
                            mRtcEngine.setAudioRouteType(deviceInfo.type);
                            mAudioRouteTypeDialog.dismiss();
                        }
                    })
                    .setTitle("请选择音频播放设备")
                    .create();
            mAudioRouteTypeDialog.show();
        });
        final List<DingRtcEngine.DingRtcAudioDeviceInfo> audiDevices = mRtcEngine.getAudioDevices();
        for (DingRtcEngine.DingRtcAudioDeviceInfo info : audiDevices) {
            if (info.isUsed) {
                int sourceId = getAudioDeviceSource(info.type);
                mSpeakerView.setImageResource(sourceId);
            }
        }

        ImageView switchView = findViewById(R.id.iv_left_icon2);
        switchView.setVisibility(View.VISIBLE);
        switchView.setImageResource(R.drawable.switch_camera);
        switchView.setOnClickListener(view -> {
            if (!useSwitchCameraId) {
                mIsFront = !mIsFront;
                mRtcEngine.switchCamera();
            } else {
                List<DingRtcEngine.DingRtcVideoDeviceInfo> infos = mRtcEngine.getCameraList();
                if (infos != null && infos.size() > 0) {
                    DingRtcEngine.DingRtcVideoDeviceInfo info = infos.get(cameraIdx);
                    String deviceId = info.deviceId;
                    mRtcEngine.setCurrentCameraID(deviceId);
                    if (++cameraIdx >= infos.size()) {
                        cameraIdx = 0;
                    }
                    mIsFront = info.direction == DingRtcEngine.DingRtcCameraDirection.CAMERA_FRONT;
                }
            }
        });

        TextView finishView = findViewById(R.id.tv_right);
        finishView.setVisibility(View.VISIBLE);
        finishView.setText(R.string.title_finish);
        finishView.setOnClickListener(view -> {
            if (mIsJoin) {
                leaveChannel();
            } else {
                this.finish();
            }
        });
    }

    private void initBottomView() {
        mBottomView = findViewById(R.id.room_bottom_layout);
        DrawableTextView pubAudioView = findViewById(R.id.bottom_pub_audio);
        pubAudioView.setOnClickListener(view -> {
            pubAudio(!mUserSelf.isAudioStarted);
            updateAudioUI(pubAudioView);
        });
        updateAudioUI(pubAudioView);

        DrawableTextView enableVideoView = findViewById(R.id.bottom_enable_video);
        enableVideoView.setOnClickListener(view -> {
            enableVideo(!mUserSelf.isVideoEnabled);
            updateEnableVideoUI(enableVideoView);
        });
        updateEnableVideoUI(enableVideoView);

        DrawableTextView pubVideoView = findViewById(R.id.bottom_pub_video);
        pubVideoView.setOnClickListener(view -> {
            pubVideo(!mUserSelf.isVideoStarted);
            updateVideoUI(pubVideoView);
        });
        updateVideoUI(pubVideoView);

        DrawableTextView previewView = findViewById(R.id.bottom_preview);
        previewView.setOnClickListener(view -> {
            previewVideo(!mUserSelf.isVideoPreview);
            updatePreviewUI(previewView);
        });
        updatePreviewUI(previewView);

        DrawableTextView moreView = findViewById(R.id.bottom_more);
        moreView.setOnClickListener(view -> mPopupWindow.showAtLocation(view, Gravity.BOTTOM | Gravity.END, 0, 0));

        initBottomPopWindow();
    }

    private void initBottomPopWindow() {
        View popView = LayoutInflater.from(BigConfRoomActivity.this).inflate(R.layout.room_pop_window, null);
        DrawableTextView privacyAudio = popView.findViewById(R.id.more_privacy_audio);
        privacyAudio.setOnClickListener(view -> {
            muteAudio(!mUserSelf.isAudioMuted);
            updateAudioMuteUI(privacyAudio);
        });

        DrawableTextView privacyVideo = popView.findViewById(R.id.more_privacy_video);
        privacyVideo.setOnClickListener(view -> {
            muteVideo(!mUserSelf.isVideoMuted);
            updateVideoMuteUI(privacyVideo);
        });

        DrawableTextView privacyScreen = popView.findViewById(R.id.more_privacy_screen);
        privacyScreen.setOnClickListener(view -> {
            muteScreen(!mUserSelf.isScreenMuted);
            updateScreenMuteUI(privacyScreen);
        });

        DrawableTextView setting = popView.findViewById(R.id.more_setting);
        setting.setOnClickListener(view -> SettingActivity.launch(this));

        DrawableTextView whiteBoardView = popView.findViewById(R.id.white_board);
        whiteBoardView.setOnClickListener(view -> WhiteboardActivity.launch(this, mRtcEngine));


        DrawableTextView audioDumpView = popView.findViewById(R.id.audio_dump);
        audioDumpView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mIsAudioDumpStart) {
                    mIsAudioDumpStart = false;
                    audioDumpView.setText("Start Audio Dump");
                    mRtcEngine.setParameters("{\"audio_dump_config\":{\"enable\":false}}");
                } else {
                    mIsAudioDumpStart = true;
                    audioDumpView.setText("Stop Audio Dump");
                    mRtcEngine.setParameters("{\"audio_dump_config\":{\"enable\":true}}");
                }
            }
        });

        DrawableTextView qualityReportView = popView.findViewById(R.id.quality_report);
        qualityReportView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mQualityReportDialog == null) {
                    mQualityReportDialog = new QualityReportDialog(BigConfRoomActivity.this);
                }
                mQualityReportDialog.show();
                mQualityReportDialog.getWindow().setLayout(
                        DensityUtils.getScreenWidth(BigConfRoomActivity.this) * 5 / 6,
                        ConstraintLayout.LayoutParams.WRAP_CONTENT);
            }
        });

        DrawableTextView muteAll = popView.findViewById(R.id.more_mute_all);
        muteAll.setOnClickListener(view -> {
            if (mMuteAll) {
                int ret = mRtcEngine.setAllUserAudioMute(false, null);
                if (ret == 0) {
                    mMuteAll = false;
                    muteAll.setText("全员静音");
                } else {
                    showToast("取消全员静音失败，code: " + ret);
                }
            } else {
                String[] users = mRtcEngine.getOnlineRemoteUsers();
                final boolean[] checkedItems = new boolean[users.length];
                AlertDialog dialog = new AlertDialog.Builder(BigConfRoomActivity.this)
                        .setTitle("请选择静音白名单用户")
                        .setMultiChoiceItems(users, checkedItems, new DialogInterface.OnMultiChoiceClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which, boolean isChecked) {
                                checkedItems[which] = true;
                            }
                        })
                        .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                ArrayList<String> whiteListUsers = new ArrayList<>();
                                for (int i = 0; i < checkedItems.length; i++) {
                                    if (checkedItems[i]) {
                                        whiteListUsers.add(users[i]);
                                    }
                                }
                                int ret = mRtcEngine.setAllUserAudioMute(true, whiteListUsers);
                                if (ret == 0) {
                                    mMuteAll = true;
                                    muteAll.setText("取消全员静音");
                                } else {
                                    showToast("全员静音失败，code: " + ret);
                                }
                            }
                        })
                        .setNegativeButton("取消", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                dialog.dismiss();
                            }
                        })
                        .create();
                dialog.show();
            }
        });

        mPopupWindow = new PopupWindow(popView, ViewGroup.LayoutParams.WRAP_CONTENT,
                ViewGroup.LayoutParams.WRAP_CONTENT);
        mPopupWindow.setFocusable(true);
        mPopupWindow.setBackgroundDrawable(new ColorDrawable(0x00000000));

        View cancel = popView.findViewById(R.id.more_cancel);
        cancel.setOnClickListener(view -> mPopupWindow.dismiss());
    }

    private void initContentView() {
        Resources resources = this.getResources();
        mIsPortrait = resources.getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT;
        mRoomInfo = findViewById(R.id.room_info);
        mRoomUsersView = findViewById(R.id.room_users_view);
        mRoomUsersView.setHasFixedSize(true);

        mLayoutManager = new GridLayoutManager(this, mRenderColumnCount) {
            @Override
            public boolean canScrollVertically() {
                return false;
            }

            @Override
            public boolean canScrollHorizontally() {
                return false;
            }
        };
        mLayoutManager.setInitialPrefetchItemCount(mRenderColumnCount);
        mRoomUsersView.setLayoutManager(mLayoutManager);

        mBigConfUsersAdapter = new BigConfUsersAdapter(this, mRtcEngine, mRenderColumnCount);
        mRoomUsersView.setAdapter(mBigConfUsersAdapter);

        mNextPageBtn = findViewById(R.id.next_page);
        mNextPageBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onClickNextPage();
            }
        });
        mPrePageBtn = findViewById(R.id.pre_page);
        mPrePageBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onClickPrePage();
            }
        });
    }

    private void onClickNextPage() {
        unsubCurrentPageUsers();
        List<BigConfUser> users = getPageUsers(++mCurrentSelectedPage);
        mBigConfUsersAdapter.setUsers(users);
        int totalPages = (int) Math.ceil(mAllUsers.size() * 1.0 / mUserCountPerPage);
        if (mCurrentSelectedPage == totalPages) {
            mNextPageBtn.setVisibility(View.GONE);
        }
        mPrePageBtn.setVisibility(View.VISIBLE);
        updateRoomInfo();
    }

    private void onClickPrePage() {
        unsubCurrentPageUsers();
        List<BigConfUser> users = getPageUsers(--mCurrentSelectedPage);
        mBigConfUsersAdapter.setUsers(users);
        if (mCurrentSelectedPage == 1) {
            mPrePageBtn.setVisibility(View.GONE);
        }
        mNextPageBtn.setVisibility(View.VISIBLE);
        updateRoomInfo();
    }

    private void unsubCurrentPageUsers() {
        List<BigConfUser> users = mBigConfUsersAdapter.getAllUsers();
        for (BigConfUser user : users) {
            if (!user.isSelf) {
                if (user.videoSubscribeState == DingRtcStatsSubscribed || user.screenSubscribeState == DingRtcStatsSubscribed
                        || user.videoSubscribeState == DingRtcStatsSubscribing || user.screenSubscribeState == DingRtcStatsSubscribing) {
                    mRtcEngine.subscribeRemoteVideoStream(user.uid, user.subedStreamType, false);
                    mRtcEngine.setRemoteViewConfig(null, user.uid, user.subedStreamType);
                }
            }
        }
    }

    private List<BigConfUser> getPageUsers(int pageNum) {
        List<BigConfUser> users = new ArrayList<>();
        int startPos = (pageNum - 1) * mUserCountPerPage;
        int endPos = pageNum * mUserCountPerPage - 1;
        int index = 0;
        for (BigConfUser user : mAllUsers.values()) {
            if (index < startPos) {
                index++;
            } else if (index <= endPos) {
                index++;
                users.add(user);
            } else {
                break;
            }
        }
        return users;
    }

    private void updateRoomInfo() {
        int totalPages = (int) Math.ceil(mAllUsers.size() * 1.0 / mUserCountPerPage);
        mRoomInfo.setText("房间名：" + mAuthInfo.channelId +
                "，人数" + mAllUsers.size() + "，第" + mCurrentSelectedPage + "/" + totalPages + "页");
    }

    private void updateUIOnConfigurationChanged() {
        mBottomView.setVisibility(mIsPortrait ? View.VISIBLE : View.GONE);
        mTitleView.setVisibility(mIsPortrait ? View.VISIBLE : View.GONE);
    }

    private void addMySelf() {
        mUserSelf.uid = mAuthInfo.userId;
        mBigConfUsersAdapter.addUser(mUserSelf);
        mAllUsers.put(mAuthInfo.userId, mUserSelf);

        if (mUserSelf.isVideoPreview) {
            mVideoEncoderConfiguration = new DingRtcEngine.DingRtcVideoEncoderConfiguration();
            mVideoEncoderConfiguration.frameRate = mPublishFps;
            mVideoEncoderConfiguration.dimensions = getDingRtcVideoDimensions(mPublishResolution);

            SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
            String cameraMaxBitrateValue = preferences.getString(getString(R.string.video_max_bitrate_key), "0");
            if (!TextUtils.isEmpty(cameraMaxBitrateValue)) {
                cameraMaxBitrate = parseInt(cameraMaxBitrateValue);
                Log.d(TAG, "cameraMaxBitrate = " + cameraMaxBitrate);
                mVideoEncoderConfiguration.bitrate = cameraMaxBitrate;
            }

            mRtcEngine.setVideoEncoderConfiguration(mVideoEncoderConfiguration);
            mRtcEngine.startPreview();

            String screenMaxBitrateValue = preferences.getString(getString(R.string.screen_max_bitrate_key), "0");
            if (!TextUtils.isEmpty(screenMaxBitrateValue)) {
                screenMaxBitrate = parseInt(screenMaxBitrateValue);
                DingRtcEngine.DingRtcScreenShareEncoderConfiguration screenShareEncoderConfiguration = new DingRtcEngine.DingRtcScreenShareEncoderConfiguration();
                screenShareEncoderConfiguration.frameRate = mScreenSharePublishFps;
                screenShareEncoderConfiguration.dimensions = getDingRtcVideoDimensions(mScreenSharePublishResolution);
                screenShareEncoderConfiguration.bitrate = screenMaxBitrate;
                Log.d(TAG, "screenMaxBitrate = " + screenMaxBitrate);
                mRtcEngine.setScreenShareEncoderConfiguration(screenShareEncoderConfiguration);
            }
        }
    }

    private DingRtcEngine.DingRtcVideoDimensions getDingRtcVideoDimensions(int resolution) {
        DingRtcEngine.DingRtcVideoDimensions dimensions;
        switch (resolution) {
            case 0:
                dimensions = VD_120x120;
                break;
            case 1:
                dimensions = VD_160x120;
                break;
            case 2:
                dimensions = VD_180x180;
                break;
            case 3:
                dimensions = VD_240x180;
                break;
            case 4:
                dimensions = VD_320x180;
                break;
            case 5:
                dimensions = VD_240x240;
                break;
            case 6:
                dimensions = VD_320x240;
                break;
            case 7:
                dimensions = VD_424x240;
                break;
            case 8:
                dimensions = VD_360x360;
                break;
            case 9:
                dimensions = VD_480x360;
                break;
            case 10:
                dimensions = VD_640x360;
                break;
            case 11:
                dimensions = VD_480x480;
                break;
            case 12:
                dimensions = VD_640x480;
                break;
            case 13:
                dimensions = VD_840x480;
                break;
            case 14:
                dimensions = VD_960x540;
                break;
            case 15:
                dimensions = VD_960x720;
                break;
            case 16:
                dimensions = VD_1280x720;
                break;
            case 17:
                dimensions = VD_1920x1080;
                break;
            default:
                dimensions = VD_640x360;
                break;
        }
        return dimensions;
    }

    private void addUser(String uid) {
        BigConfUser user = new BigConfUser();
        user.uid = uid;
        DingRtcRemoteUserInfo remoteUserInfo = mRtcEngine.getUserInfo(uid);
        if (remoteUserInfo != null) {
            user.nickName = remoteUserInfo.getDisplayName();
        }
        mAllUsers.put(uid, user);
        if (mBigConfUsersAdapter.getUserCount() < mCurrentSelectedPage * mUserCountPerPage) {
            mBigConfUsersAdapter.addUser(user);
        }

        if (mNextPageBtn.getVisibility() != View.VISIBLE && mAllUsers.size() > mUserCountPerPage * mCurrentSelectedPage) {
            mNextPageBtn.setVisibility(View.VISIBLE);
        }
    }

    private void removeUser(String uid) {
        mAllUsers.remove(uid);

        boolean result = mBigConfUsersAdapter.removeUser(uid);
        if (result) {
            BigConfUser user = mBigConfUsersAdapter.getLastUser();
            if (user != null) {
                BigConfUser nextUser = null;
                boolean findOldUser = false;
                for (Map.Entry<String, BigConfUser> entry : mAllUsers.entrySet()) {
                    if (findOldUser && !uid.equals(entry.getKey())) {
                        nextUser = entry.getValue();
                        break;
                    }
                    if (uid.equals(entry.getKey())) {
                        findOldUser = true;
                    }
                }
                if (nextUser != null) {
                    mBigConfUsersAdapter.addUser(nextUser);
                }

                if (user.videoSubscribeState == DingRtcStatsSubscribed || user.screenSubscribeState == DingRtcStatsSubscribed
                        || user.videoSubscribeState == DingRtcStatsSubscribing || user.screenSubscribeState == DingRtcStatsSubscribing) {
                    mRtcEngine.subscribeRemoteVideoStream(user.uid, user.subedStreamType, false);
                    mRtcEngine.setRemoteViewConfig(null, user.uid, user.subedStreamType);
                }
            }
        }

        int totalPages = (int) Math.ceil(mAllUsers.size() * 1.0 / mUserCountPerPage);
        if (totalPages < mCurrentSelectedPage) {
            onClickPrePage();
        }

        if (mCurrentSelectedPage == totalPages) {
            mNextPageBtn.setVisibility(View.GONE);
        }
        if (mCurrentSelectedPage == 1) {
            mPrePageBtn.setVisibility(View.GONE);
        }
    }

    private void updateUserUI(BigConfUser user) {
        mBigConfUsersAdapter.updateUser(user);
    }

    private void onRemoteUserVideoUnsubed(BigConfUser user) {
        mRtcEngine.subscribeRemoteVideoStream(user.uid, user.subedStreamType, false);
        mRtcEngine.setRemoteViewConfig(null, user.uid, user.subedStreamType);
    }

    private void initRtcEngine() {
        DingRtcEngine.setLogDirPath(getExternalFilesDir("log").getAbsolutePath());
        String mcsConfig = "";
        mRtcEngine = DingRtcEngine.create(getApplicationContext(), mcsConfig);
        mRtcEngine.subscribeAllRemoteVideoStreams(false); //由于demo使用了分页展示，因此需要手动订阅
        mRtcEngine.setRemoteDefaultVideoStreamType(mDefaultSubStreamType);

        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        mRtcEngine.subscribeAllRemoteAudioStreams(mSettingSubAllAudio);
        mLastSubAllAudio = mSettingSubAllAudio;

        String audioProfileStr = preferences.getString(getString(R.string.audio_profile_key), "0");
        DingRtcEngine.DingRtcAudioProfile audioProfile;
        if ("1".equals(audioProfileStr)) {
            audioProfile = DingRtcEngine.DingRtcAudioProfile.DingRtcEngineHighQualityMode;
        } else if ("2".equals(audioProfileStr)) {
            audioProfile = DingRtcEngine.DingRtcAudioProfile.DingRtcEngineSuperHighQualityMode;
        } else if ("3".equals(audioProfileStr)) {
            audioProfile = DingRtcEngine.DingRtcAudioProfile.DingRtcEngineStereoHighQualityMode;
        } else if ("4".equals(audioProfileStr)) {
            audioProfile = DingRtcEngine.DingRtcAudioProfile.DingRtcEngineStereoSuperHighQualityMode;
        } else {
            audioProfile = DingRtcEngine.DingRtcAudioProfile.DingRtcEngineBasicQualityMode;
        }
        String audioScenarioStr = preferences.getString(getString(R.string.audio_scenario_key), "0");
        DingRtcEngine.DingRtcAudioScenario audioScenario;
        if ("1".equals(audioScenarioStr)) {
            audioScenario = DingRtcEngine.DingRtcAudioScenario.DingRtcSceneMusicMode;
        } else {
            audioScenario = DingRtcEngine.DingRtcAudioScenario.DingRtcSceneDefaultMode;
        }
        mRtcEngine.setAudioProfile(audioProfile, audioScenario);

        mStartAudioCapture = preferences.getBoolean(getString(R.string.audio_capture_key), true);
        mStartAudioPlay = preferences.getBoolean(getString(R.string.audio_play_key), true);
        mMuteAudioRecordingSignal = preferences.getBoolean(getString(R.string.audio_mute_recording_signal_key), false);
        if (mStartAudioCapture) {
            mRtcEngine.startAudioCapture();
        }
        if (mStartAudioPlay) {
            mRtcEngine.startAudioPlayer();
        }
        mRtcEngine.muteRecordingSignal(mMuteAudioRecordingSignal);

        mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
            @Override
            public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {
                super.onJoinChannelResult(result, channel, userId, elapsed);
                runOnUiThread(() -> {
                    if (result == 0) {
                        updateRoomInfo();
                        mIsJoin = true;
                        if (mUserSelf.isAudioStarted) {
                            pubAudio(true);
                        }
                        if (mUserSelf.isVideoStarted) {
                            pubVideo(true);
                        }
                    }
                });
            }

            @Override
            public void onLeaveChannelResult(int result, DingRtcEngine.DingRtcStats stats) {
                super.onLeaveChannelResult(result, stats);
                runOnUiThread(() -> {
                    if (mProgressDialog != null && mProgressDialog.isShowing()) {
                        mProgressDialog.dismiss();
                    }
                    finish();
                });
            }

            @Override
            public void onChannelRemainingTimeNotify(int remainingTimeInSec) {
                runOnUiThread(() -> {
                    showToast("频道将在" + remainingTimeInSec + "秒后结束");
                    if (countDownTimer != null) {
                        countDownTimer.cancel();
                    }
                    countDownTimer = new CountDownTimer(remainingTimeInSec * 1000L, 1000) {
                        @Override
                        public void onTick(long millisUntilFinished) {
                            mTitleText.setText(timeConversion((int) (millisUntilFinished / 1000)));
                        }

                        @Override
                        public void onFinish() {
                            showToast("会议已结束！");
                        }
                    };
                    countDownTimer.start();
                });
            }

            @Override
            public void onAudioSubscribeStateChanged(String uid, DingRtcEngine.DingRtcSubscribeState oldState, DingRtcEngine.DingRtcSubscribeState newState, int elapseSinceLastState, String channel) {
                super.onAudioSubscribeStateChanged(uid, oldState, newState, elapseSinceLastState, channel);
                if (newState != DingRtcStatsNoSubscribe && newState != DingRtcStatsSubscribed) {
                    return;
                }
                runOnUiThread(() -> {
                    BigConfUser user = mAllUsers.get(uid);
                    if (user != null) {
                        boolean sub = newState == DingRtcStatsSubscribed;
                        user.isAudioSubscribed = sub;
                    }
                });
            }

            @Override
            public void onVideoSubscribeStateChanged(String uid, DingRtcEngine.DingRtcSubscribeState oldState, DingRtcEngine.DingRtcSubscribeState newState, int elapseSinceLastState, String channel) {
                super.onVideoSubscribeStateChanged(uid, oldState, newState, elapseSinceLastState, channel);
                runOnUiThread(() -> {
                    BigConfUser user = mAllUsers.get(uid);
                    if (user != null) {
                        user.videoSubscribeState = newState;
                        if ((oldState == DingRtcStatsSubscribed || oldState == DingRtcStatsSubscribing)
                                && newState == DingRtcStatsNoSubscribe) {
                            onRemoteUserVideoUnsubed(user);
                        }
                    }
                });
            }

            @Override
            public void onSubscribeStreamTypeChanged(String uid, DingRtcEngine.DingRtcVideoStreamType oldStreamType, DingRtcEngine.DingRtcVideoStreamType newStreamType, int elapseSinceLastState, String channel) {
                super.onSubscribeStreamTypeChanged(uid, oldStreamType, newStreamType, elapseSinceLastState, channel);
                if (newStreamType == DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeNone) {
                    return;
                }
                runOnUiThread(() -> {
                    BigConfUser user = mAllUsers.get(uid);
                    if (user != null) {
                        user.videoSubscribeType = newStreamType;
                    }
                });
            }

            @Override
            public void onScreenShareSubscribeStateChanged(String uid, DingRtcEngine.DingRtcSubscribeState oldState, DingRtcEngine.DingRtcSubscribeState newState, int elapseSinceLastState, String channel) {
                super.onScreenShareSubscribeStateChanged(uid, oldState, newState, elapseSinceLastState, channel);
                runOnUiThread(() -> {
                    BigConfUser user = mAllUsers.get(uid);
                    if (user != null) {
                        boolean sub = newState == DingRtcStatsSubscribed;
                        user.screenSubscribeState = newState;
                    }
                });
            }

            @Override
            public void onOccurError(int error, String message) {
                super.onOccurError(error, message);
                showToastL("发生异常 code: " + error + ", msg: " + message);
            }

            @Override
            public void onOccurErrorDetail(DingRtcErrorDescription message) {
                super.onOccurErrorDetail(message);

                if (message instanceof DingRtcSubscribeErrorDescription) {
                    DingRtcSubscribeErrorDescription msg = (DingRtcSubscribeErrorDescription) message;
                    showToastL("发生订阅异常, code:" + msg.errorCode + " subscribeUid: " + msg.subscribeUid);
                } else {
                    showToastL("发生异常 code: " + message.errorCode);
                }
            }

            @Override
            public void onConnectionStatusChanged(DingRtcEngine.DingRtcConnectionStatus status, DingRtcEngine.DingRtcConnectionStatusChangeReason reason) {
                Log.i(TAG, "onConnectionStatusChanged, status: " + status + " , reason: " + reason);
                runOnUiThread(() -> {
                    String text = "";
                    if (status == DingRtcEngine.DingRtcConnectionStatus.DingRtcConnectionStatusInit) {
                        text = "(Init)";
                    } else if (status == DingRtcEngine.DingRtcConnectionStatus.DingRtcConnectionStatusDisconnected) {
                        text = "(Disconnected)";
                    } else if (status == DingRtcEngine.DingRtcConnectionStatus.DingRtcConnectionStatusConnecting) {
                        text = "(Connecting)";
                    } else if (status == DingRtcEngine.DingRtcConnectionStatus.DingRtcConnectionStatusConnected) {
                        text = "(Connected)";
                    } else if (status == DingRtcEngine.DingRtcConnectionStatus.DingRtcConnectionStatusReconnecting) {
                        text = "(Reconnecting)";
                    } else if (status == DingRtcEngine.DingRtcConnectionStatus.DingRtcConnectionStatusFailed) {
                        text = "(Failed)";
                    }
                    mConnectStateText.setText(text);
                });
            }

            @Override
            public void onRemoteUserOnLineNotify(String uid, int elapsed) {
                super.onRemoteUserOnLineNotify(uid, elapsed);
                Log.i(TAG, "onRemoteUserOnLineNotify uid: " + uid);
                runOnUiThread(() -> {
                    if (!mAllUsers.containsKey(uid)) {
                        addUser(uid);
                    }
                    updateRoomInfo();
                });
            }

            @Override
            public void onRemoteUserOffLineNotify(String uid, DingRtcEngine.DingRtcUserOfflineReason reason) {
                super.onRemoteUserOffLineNotify(uid, reason);
                Log.i(TAG, "onRemoteUserOffLineNotify uid: " + uid);
                runOnUiThread(() -> {
                    removeUser(uid);
                    updateRoomInfo();
                });
            }

            @Override
            public void onRemoteTrackAvailableNotify(String uid, DingRtcEngine.DingRtcAudioTrack audioTrack, DingRtcEngine.DingRtcVideoTrack videoTrack) {
                super.onRemoteTrackAvailableNotify(uid, audioTrack, videoTrack);
                Log.i(TAG, "onRemoteTrackAvailableNotify uid: " + uid + ", audioTrack: " + audioTrack + ", videoTrack: " + videoTrack);
                runOnUiThread(() -> {
                    BigConfUser user = mAllUsers.get(uid);
                    if(user == null) {
                        return;
                    }
                    if(videoTrack == DingRtcVideoTrackCamera) {
                        user.isVideoStarted = true;
                        user.isScreenStarted = false;
                    } else if(videoTrack == DingRtcVideoTrackScreen) {
                        user.isVideoStarted = false;
                        user.isScreenStarted = true;
                    } else if(videoTrack == DingRtcVideoTrackBoth) {
                        user.isVideoStarted = true;
                        user.isScreenStarted = true;
                    } else if(videoTrack == DingRtcVideoTrackNo) {
                        user.isVideoStarted = false;
                        user.isScreenStarted = false;
                    }

                    if(audioTrack == DingRtcAudioTrackNo) {
                        user.isAudioStarted = false;
                    } else if(audioTrack == DingRtcAudioTrackMic) {
                        user.isAudioStarted = true;
                    }
                    updateUserUI(user);
                });
            }

            @Override
            public void onBye(int code) {
                super.onBye(code);
                Log.i(TAG, "onBye: " + code);
                runOnUiThread(() -> {
                    String text = "当前用户被踢出房间";
                    if (code == 1) {
                        text = "当前用户被踢出房间";
                    } else if (code == 2) {
                        text = "当前房间已结束";
                    } else if (code == 3) {
                        text = "当前用户在其他设备登录，设备被下线";
                    } else if (code == 4) {
                        text = "Relogin失败";
                    }
                    showToastL(text);
                    finish();
                });
            }

            @Override
            public void onUserAudioMuted(String uid, boolean isMute) {
                super.onUserAudioMuted(uid, isMute);
                runOnUiThread(() -> {
                    BigConfUser user = mAllUsers.get(uid);
                    if (user == null) {
                        return;
                    }
                    user.isAudioMuted = isMute;
                    //updateUserUI(uid);
                });
            }

            @Override
            public void onUserVideoMuted(String uid, boolean isMute, DingRtcEngine.DingRtcVideoTrack track) {
                super.onUserVideoMuted(uid, isMute, track);
                runOnUiThread(() -> {
                    BigConfUser user = mAllUsers.get(uid);
                    if (user == null) {
                        return;
                    }
                    if (track == DingRtcVideoTrackCamera) {
                        user.isVideoMuted = true;
                    } else {
                        user.isScreenMuted = true;
                    }
                });
            }

            @Override
            public void onUserVideoEnabled(String uid, boolean enabled) {
                super.onUserVideoEnabled(uid, enabled);
                Log.i(TAG, "onUserVideoEnabled uid : " + uid + ", enabled : " + enabled);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        BigConfUser user = mAllUsers.get(uid);
                        if (user == null) {
                            return;
                        }
                        user.isCameraOn = enabled;
                    }
                });
            }

            @Override
            public void onQualityReport(DingRtcEngine.DingRtcQualityReport quality) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (mQualityReportDialog != null) {
                            mQualityReportDialog.updateQualityReport(quality);
                        }
                    }
                });
            }

            @Override
            public void onRtcLocalVideoStats(DingRtcEngine.DingRtcLocalVideoStats DingRtcStats) {
                super.onRtcLocalVideoStats(DingRtcStats);
                Log.i(TAG, "onRtcLocalVideoStats: " + DingRtcStats);
            }

            @Override
            public void onRtcLocalAudioStats(DingRtcEngine.DingRtcLocalAudioStats DingRtcStats) {
                super.onRtcLocalAudioStats(DingRtcStats);
                Log.i(TAG, "onRtcLocalAudioStats: " + DingRtcStats);
            }

            @Override
            public void onAudioRouteChanged(DingRtcEngine.DingRtcAudioRouteType routing) {
                Log.i(TAG, "onAudioRouteChanged routing: " + routing.getValue());
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mSpeakerView.setImageResource(getAudioDeviceSource(routing));
                        mUserSelf.audioDeviceName = getAudioDeviceName(routing);
                    }
                });
            }
            @Override
            public void onVideoDeviceStateChanged(String id, int type, int state) {
                Log.i(TAG, "onVideoDeviceStateChanged, id: " + id + ", type: " + type + ", state: " + state);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (type == 2) {
                            String stateStr;
                            if (state == 1) {
                                stateStr = "被移除";
                            } else {
                                stateStr = "被插入";
                            }
                            showToast("摄像头: " + id + stateStr);
                        }
                    }
                });
            }

            @Override
            public void onMediaExtensionMsgReceived(String uid, byte[] message) {
                String msg = new String(message);
                Log.i(TAG, "onMediaExtensionMsgReceived, uid: " + uid + ", message: " + msg);
                showToast("收到媒体扩展信息:" + msg);
            }
        });
    }

    private void joinChannel() {
        //先rtc joinChannel，再joinMeeting
        int ret = mRtcEngine.joinChannel(mAuthInfo, mUserSelf.nickName);
        if (ret != 0) {
            showToastL("加入房间失败");
        }
    }

    private void leaveChannel() {
        mProgressDialog = new ProgressDialog(this);
        mProgressDialog.setMessage("离会中...");
        mProgressDialog.setCancelable(false);
        mProgressDialog.show();
        mRtcEngine.leaveChannel();
    }

    private void previewVideo(boolean preview) {
        // todo
        int ret;
        if (preview) {
            ret = mRtcEngine.startPreview();
        } else {
            ret = mRtcEngine.stopPreview();
        }
        if (ret == 0) {
            mUserSelf.isVideoPreview = preview;
        }
    }

    private void updatePreviewUI(DrawableTextView previewView) {
        if (mUserSelf.isVideoPreview) {
            previewView.setDrawableTop(this, R.drawable.preview_on);
        } else {
            previewView.setDrawableTop(this, R.drawable.preview_off);
        }
    }

    private void pubVideo(boolean pub) {
        // todo
        int ret = mRtcEngine.publishLocalVideoStream(pub);
        if (ret == 0) {
            mUserSelf.isVideoStarted = pub;
        }
    }

    private void updateVideoUI(DrawableTextView pubVideoView) {
        if (mUserSelf.isVideoStarted) {
            pubVideoView.setText("停止推流");
            pubVideoView.setDrawableTop(this, R.drawable.pub_video);
        } else {
            pubVideoView.setText("开始推流");
            pubVideoView.setDrawableTop(this, R.drawable.unpub_video);
        }
    }

    private void pubAudio(boolean pub) {
        int ret = mRtcEngine.publishLocalAudioStream(pub);
        if (ret == 0) {
            mUserSelf.isAudioStarted = pub;
        }
    }

    private void updateAudioUI(DrawableTextView pubAudioView) {
        if (mUserSelf.isAudioStarted) {
            pubAudioView.setText("开始推流");
            pubAudioView.setDrawableTop(this, R.drawable.pub_audio);
        } else {
            pubAudioView.setText("停止推流");
            pubAudioView.setDrawableTop(this, R.drawable.unpub_audio);
        }
    }

    private void enableVideo(boolean enable) {
        int ret = mRtcEngine.enableLocalVideo(enable);
        if (ret == 0) {
            mUserSelf.isVideoEnabled = enable;
        }
    }

    private void updateEnableVideoUI(DrawableTextView enableVideoView) {
        if (mUserSelf.isVideoEnabled) {
            enableVideoView.setText("停止采集");
            enableVideoView.setDrawableTop(this, R.drawable.camera_on);
        } else {
            enableVideoView.setText("开启采集");
            enableVideoView.setDrawableTop(this, R.drawable.camera_off);
        }
    }

    private void muteAudio(boolean mute) {
        AlertDialog audioMuteDialog = new AlertDialog.Builder(this)
                .setItems(mute ? R.array.audio_mute_mode : R.array.audio_unmute_mode, (dialog, which) -> {
                    int ret = -1;
                    switch (which) {
                        case 0:
                            ret = mRtcEngine.muteRecordingSignal(mute);
                            break;
                        case 1:
                            ret = mRtcEngine.muteLocalAudio(mute);
                            break;
                    }
                    if (ret != 0) {
                        showToast(mute ? "静音失败, code: " + ret : "取消静音失败, code: " + ret);
                    } else {
                        mUserSelf.isAudioMuted = mute;
                    }
                })
                .create();
        audioMuteDialog.show();
    }

    private void updateAudioMuteUI(DrawableTextView muteAudioView) {
        if (mUserSelf.isAudioMuted) {
            muteAudioView.setText("Unmute音频");
        } else {
            muteAudioView.setText("Mute音频");
        }
    }

    private void muteVideo(boolean mute) {
        int ret = mRtcEngine.muteLocalCamera(mute, DingRtcVideoTrackCamera);
        if (ret == 0) {
            mUserSelf.isVideoMuted = mute;
        }
    }

    private void updateVideoMuteUI(DrawableTextView muteVideoView) {
        if (mUserSelf.isVideoMuted) {
            muteVideoView.setText("Unmute视频");
        } else {
            muteVideoView.setText("Mute视频");
        }
    }

    private void muteScreen(boolean mute) {
        int ret = mRtcEngine.muteLocalCamera(mute, DingRtcVideoTrackScreen);
        if (ret == 0) {
            mUserSelf.isScreenMuted = mute;
        }
    }

    private void updateScreenMuteUI(DrawableTextView muteScreenView) {
        if (mUserSelf.isVideoMuted) {
            muteScreenView.setText("Unmute屏幕");
        } else {
            muteScreenView.setText("Mute屏幕");
        }
    }

    private int parseInt(String valStr, String dftValStr, int radix) {
        int retVal;
        int dftVal;

        try {
            dftVal = Integer.parseInt(dftValStr, radix);
        } catch (NumberFormatException e) {
            dftVal = 0;
        }

        try {
            retVal = Integer.parseInt(valStr, radix);
        } catch (NumberFormatException e) {
            retVal = dftVal;
        }

        return retVal;
    }

    private int parseInt(String valStr, String dftValStr) {
        return parseInt(valStr, dftValStr, 10);
    }

    private int parseInt(String valStr) {
        return parseInt(valStr, "0");
    }

    private String timeConversion(int time) {
        int hour = 0;
        int minutes = 0;
        int second = 0;
        int temp = time % 3600;
        if (time > 3600) {
            hour = time / 3600;
            if (temp != 0) {
                if (temp > 60) {
                    minutes = temp / 60;
                    if (temp % 60 != 0) {
                        second = temp % 60;
                    }
                } else {
                    second = temp;
                }
            }
        } else {
            minutes = time / 60;
            if (time % 60 != 0) {
                second = time % 60;
            }
        }
        return (hour < 10 ? ("0" + hour) : hour) + ":" + (minutes < 10 ? ("0" + minutes) : minutes) + ":" + (second < 10 ? ("0" + second) : second);
    }

    private String getAudioDeviceName(DingRtcEngine.DingRtcAudioRouteType type) {
        if (type == DingRtcAudioRouteType_Headset) {
            return "耳机";
        } else if (type == DingRtcAudioRouteType_Earpiece) {
            return "听筒";
        } else if (type == DingRtcAudioRouteType_HeadsetNoMic) {
            return "不带麦克风的耳机";
        } else if (type == DingRtcAudioRouteType_Speakerphone) {
            return "手机扬声器";
        } else if (type == DingRtcAudioRouteType_LoudSpeaker) {
            return "外接扬声器";
        } else if (type == DingRtcAudioRouteType_BlueTooth) {
            return "蓝牙耳机";
        } else {
            return "默认的语音路由";
        }
    }

    private int getAudioDeviceSource(DingRtcEngine.DingRtcAudioRouteType routing) {
        int sourceId;
        if (routing == DingRtcAudioRouteType_Headset) {
            sourceId = R.drawable.headset;
        } else if (routing == DingRtcAudioRouteType_Earpiece) {
            sourceId = R.drawable.ear_speaker;
        } else if (routing == DingRtcAudioRouteType_HeadsetNoMic) {
            sourceId = R.drawable.headset_nomic;
        } else if (routing == DingRtcAudioRouteType_Speakerphone) {
            sourceId = R.drawable.speaker;
        } else if (routing == DingRtcAudioRouteType_LoudSpeaker) {
            sourceId = R.drawable.loud_speaker;
        } else if (routing == DingRtcAudioRouteType_BlueTooth) {
            sourceId = R.drawable.bluetooth;
        } else {
            sourceId = R.drawable.speaker;
        }
        return sourceId;
    }
}