package com.ding.rtc.demo;

import static com.ding.rtc.DingRtcEngine.DingRtcAudioDeviceInfo;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioObservePosition.RtcEngineAudioPositionProcessCaptured;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_BlueTooth;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_Earpiece;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_Headset;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_HeadsetNoMic;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_LoudSpeaker;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioRouteType.DingRtcAudioRouteType_Speakerphone;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioTrack.DingRtcAudioTrackMic;
import static com.ding.rtc.DingRtcEngine.DingRtcAudioTrack.DingRtcAudioTrackNo;
import static com.ding.rtc.DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeAuto;
import static com.ding.rtc.DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeCrop;
import static com.ding.rtc.DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeFill;
import static com.ding.rtc.DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeStretch;
import static com.ding.rtc.DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_0;
import static com.ding.rtc.DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_180;
import static com.ding.rtc.DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_270;
import static com.ding.rtc.DingRtcEngine.DingRtcRotationMode.DingRtcRotationMode_90;
import static com.ding.rtc.DingRtcEngine.DingRtcScreenShareMode.DingRtcScreenShareAllMode;
import static com.ding.rtc.DingRtcEngine.DingRtcScreenShareMode.DingRtcScreenShareNoneMode;
import static com.ding.rtc.DingRtcEngine.DingRtcScreenShareMode.DingRtcScreenShareOnlyAudioMode;
import static com.ding.rtc.DingRtcEngine.DingRtcScreenShareMode.DingRtcScreenShareOnlyVideoMode;
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

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.view.inputmethod.EditorInfo;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridLayout;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.graphics.drawable.DrawableCompat;
import androidx.viewpager.widget.ViewPager;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;
import com.ding.rtc.DingRtcEngineSubtitleManager;
import com.ding.rtc.DingRtcEngineSubtitleManagerListener;
import com.ding.rtc.DingRtcEngineSubtitleTypes;
import com.ding.rtc.DingRtcEngineWhiteboardManager;
import com.ding.rtc.DingRtcEngineWhiteboardManagerListener;
import com.ding.rtc.DingRtcErrorCode;
import com.ding.rtc.DingRtcErrorDescription;
import com.ding.rtc.DingRtcRemoteUserInfo;
import com.ding.rtc.DingRtcSubscribeErrorDescription;
import com.ding.rtc.DingRtcWhiteboardView;
import com.ding.rtc.api.DingRtcWhiteBoardTypes;
import com.ding.rtc.demo.annotation.AnnotationInfo;
import com.ding.rtc.demo.annotation.AnnotationToolBar;
import com.ding.rtc.demo.group.GroupListPopup;
import com.ding.rtc.demo.group.GroupMgr;
import com.ding.rtc.demo.rtm.RtmChatPopup;
import com.ding.rtc.demo.rtm.RtmClientHolder;
import com.ding.rtc.demo.rtm.RtmSessionMgr;
import com.ding.rtc.demo.settings.SettingActivity;
import com.ding.rtc.demo.subtitle.SubtitleManager;
import com.ding.rtc.demo.subtitle.SubtitleTranslateAdapter;
import com.ding.rtc.demo.ui.DrawableTextView;
import com.ding.rtc.demo.ui.GestureHandler;
import com.ding.rtc.demo.ui.QualityReportDialog;
import com.ding.rtc.demo.ui.RemoteUserVolumeDialog;
import com.ding.rtc.demo.ui.UserStatesDialog;
import com.ding.rtc.demo.ui.ViewPagerAdapter;
import com.ding.rtc.demo.ui.indicator.CirclePageIndicator;
import com.ding.rtc.demo.utils.DensityUtils;
import com.ding.rtc.demo.utils.FileWriteUtils;
import com.ding.rtc.demo.whiteboard.WhiteboardActivity;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

public class RoomActivity extends BaseActivity implements AnnotationToolBar.AnnotationToolBarCallback {

    private static final String TAG = "RoomActivity->";
    private static final String INTENT_MEETING_USER_ID = "meeting_user_id";
    private static final String INTENT_MEETING_TOKEN = "meeting_token";
    private static final String INTENT_MEETING_USER_NAME = "meeting_user_name";
    private static final String INTENT_MEETING_APP_ID = "meeting_app_id";
    private static final String INTENT_MEETING_CHANNEL_ID = "meeting_channel_id";

    public static void launch(Context context, String userId, String userName, String token,
                              String appId, String channelName) {
        Intent intent = new Intent();
        intent.setClass(context, RoomActivity.class);
        intent.putExtra(INTENT_MEETING_USER_ID, userId);
        intent.putExtra(INTENT_MEETING_USER_NAME, userName);
        intent.putExtra(INTENT_MEETING_TOKEN, token);
        intent.putExtra(INTENT_MEETING_APP_ID, appId);
        intent.putExtra(INTENT_MEETING_CHANNEL_ID, channelName);
        context.startActivity(intent);
    }

    private PopupWindow mPopupWindow;
    private ProgressDialog mProgressDialog;
    private boolean mIsPortrait = true;
    private int mScreenWidth;
    private int mScreenHeight;
    private final int mGridItemCount = 4;
    private final int mGridColumnCount = 2;
    private int mViewPagerCurrentPosition = 0;
    private ViewPager mViewPager;
    private ViewPagerAdapter mViewPagerAdapter;
    private CirclePageIndicator mViewPageIndicator;
    private final List<GridLayout> mGridLayoutList = new ArrayList<>();
    private View mTitleView;
    private View mBottomView;
    private TextView mTitleText;
    private TextView mConnectStateText;

    private TextView mRoomInfo;
    private boolean mSettingSubAllVideo;
    private boolean mSettingSubAllAudio;
    private boolean mLastSubAllAudio = false;
    private String mVirtualBackground = "0";
    private DingRtcEngine.DingRtcVideoStreamType mDefaultSubStreamType;
    private DingRtcEngine.DingRtcRenderMode mDefaultRenderMode;

    //rtc
    private final DingRtcAuthInfo mAuthInfo = new DingRtcAuthInfo();
    private DingRtcEngine mRtcEngine;
    private final RoomUser mUserSelf = new RoomUser();
    private final LinkedHashMap<String,RoomUser> mRemoteUserMap = new LinkedHashMap<>();
    private int mPublishResolution = 16;
    private int mPublishFps = 30;
    private int mScreenSharePublishResolution = 16;
    private int mScreenSharePublishFps = 30;
    private boolean mIsJoin = false;
    private boolean zoom = false;
    private boolean flash = false;
    private boolean mIsFront = true;
    private int videoObservePosition = -1;
    private CountDownTimer countDownTimer;
    private int cameraIdx = 0;
    private boolean useSwitchCameraId = false;

    private int cameraMaxBitrate;
    private int screenMaxBitrate;

    private boolean mEnableAudioVoiceChange;
    private ImageView mSpeakerView;
    private AlertDialog mAudioRouteTypeDialog;

    private DingRtcEngine.DingRtcVideoEncoderConfiguration mVideoEncoderConfiguration;

    // AudioGroup
    private GroupListPopup mAudioGroupPopup;

    // RtmChat
    private  RtmChatPopup mRtmChatPopup;

    private AnnotationToolBar mAnnotationToolBar;
    private String mAnnoSnapshotDir;
    List<AnnotationInfo> mAnnotationList = new ArrayList<>();

    private boolean mIsAudioDumpStart = false;
    private boolean mStartAudioCapture = true;
    private boolean mStartAudioPlay = true;
    private boolean mMuteAudioRecordingSignal = false;
    private int mPlayoutVolume = 100;
    private int mRecordingVolume = 100;

    // ScreenShare
    private AlertDialog mScreenShareDialog;


    // QualityReport
    private QualityReportDialog mQualityReportDialog;

    private boolean mMuteAll = false;

    // Subtitle
    private DingRtcEngineSubtitleManager mSubtitleManager;
    private boolean mSubtitleOpen = false;
    private View mSubtitleView;
    private View mSubTitleServiceSettingsView;
    private int mSubtitleServiceType = 1; // 1 仅开启实时字幕 2 仅开启会后纪要 3 同时开启实时字幕和会后纪要
    private TextView mSubtitleTaskIdView;
    private Spinner mSubtitleServiceTypeSpinner;
    private Button mSubtitleServiceStartBtn;
    private TextView mSubtitleServiceStateTv;
    private Spinner mSubtitleSpeakLanguageSpinner;
    private Button mSubtitleTranslateLanguageBtn;
    private ListView mSubtitleTranslateLanguageListView;
    private SubtitleTranslateAdapter mSubtitleTranslateAdapter;
    private String mCurrentSpeakLanguage = "multilingual";
    private boolean[] mSelectedTranslateLanguages;

    @Override
    public void onBackPressed() {
        if(mIsJoin) {
            leaveChannel();
        } else {
            super.onBackPressed();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mUserSelf.isSelf = true;
        configFromPreference(PreferenceManager.getDefaultSharedPreferences(this),true);

        initRtcEngine();
        setContentView(R.layout.activity_room);
        configFromIntent(getIntent());
        initTitleView();
        initBottomView();
        initContentView();
        addMySelf();
        initAudioGroup();
        initSwitchOrientation();
        initRtmChat();
        initSubTitle();
        initAnnotation();

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

        String faceBeautyPos = preferences.getString(getString(R.string.title_face_beauty_key),"0");
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

        setVirtualBackground();

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

        boolean videoDenoise = preferences.getBoolean(getString(R.string.video_denoise_key),false);
        DingRtcEngine.DingRtcEngineVideoDenoiseOptions videoDenoiseOptions = new DingRtcEngine.DingRtcEngineVideoDenoiseOptions();
        videoDenoiseOptions.mode = videoDenoise ? DingRtcEngine.DingRtcEngineVideoDenoiseMode.DingRtcEngineVideoDenoiseAuto
                : DingRtcEngine.DingRtcEngineVideoDenoiseMode.DingRtcEngineVideoDenoiseOff;
        mRtcEngine.setVideoDenoise(videoDenoiseOptions);

        boolean videoEnhance = preferences.getBoolean(getString(R.string.video_enhance_key),false);
        DingRtcEngine.DingRtcEngineVideoEnhanceOptions videoEnhanceOptions = new DingRtcEngine.DingRtcEngineVideoEnhanceOptions();
        videoEnhanceOptions.mode = videoEnhance ? DingRtcEngine.DingRtcEngineVideoEnhanceMode.DingRtcEngineVideoEnhanceAuto
                : DingRtcEngine.DingRtcEngineVideoEnhanceMode.DingRtcEngineVideoEnhanceOff;
        mRtcEngine.setVideoEnhance(videoEnhanceOptions);

        boolean audioIndication = preferences.getBoolean(getString(R.string.audio_volume_indication_key),false);
        if (audioIndication) {
            mRtcEngine.enableAudioVolumeIndication(300, 3, 1);
        } else {
            mRtcEngine.enableAudioVolumeIndication(0, 3, 0);
        }

        boolean isCameraAutoFocusFaceModeSupported = mRtcEngine.isCameraAutoFocusFaceModeSupported();
        boolean enableFocusFace = preferences.getBoolean(getString(R.string.video_focus_face_key),false);
        if (enableFocusFace) {
            if (isCameraAutoFocusFaceModeSupported) {
                mRtcEngine.setCameraAutoFocusFaceModeEnabled(true);
            } else {
                showToast("不支持人脸聚焦");
            }
        } else if (isCameraAutoFocusFaceModeSupported) {
            mRtcEngine.setCameraAutoFocusFaceModeEnabled(false);
        }

        boolean videoStabilization = preferences.getBoolean(getString(R.string.video_stabilization_key),false);
        DingRtcEngine.DingRtcCameraStabilizationMode cameraStabilizationMode = videoStabilization ? DingRtcEngine.DingRtcCameraStabilizationMode.AUTO : DingRtcEngine.DingRtcCameraStabilizationMode.OFF;
        mRtcEngine.setCameraStabilizationMode(cameraStabilizationMode);

        useSwitchCameraId = preferences.getBoolean(getString(R.string.camera_switch_id_key), false);

        String observePosition = preferences.getString(getString(R.string.video_observer_key),"-1");
        String formatPreference = preferences.getString(getString(R.string.video_observer_format_key),"-1");
        int observePos;
        int formatPos;
        try {
            observePos = Integer.parseInt(observePosition);
            formatPos = Integer.parseInt(formatPreference);
        } catch (NumberFormatException e) {
            observePos = -1;
            formatPos = -1;
        }
        if (observePos == -1) {
            mRtcEngine.unRegisterVideoSampleObserver();
            mUserSelf.statsVideoCallbackView.setText("");
            mUserSelf.statsVideoCallbackView.setVisibility(View.GONE);
            for (RoomUser user : mRemoteUserMap.values()) {
                user.statsVideoCallbackView.setText("");
                user.statsVideoCallbackView.setVisibility(View.GONE);
            }
            closeVideoDataFiles();
        } else {
            DingRtcEngine.DingRtcVideoFormat format = DingRtcEngine.DingRtcVideoFormat.fromValue(formatPos);
            boolean showRemote = observePos == DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPreRender;
            mUserSelf.statsVideoCallbackView.setText("");
            mUserSelf.statsVideoCallbackView.setVisibility(showRemote ? View.GONE : View.VISIBLE);
            for (RoomUser user : mRemoteUserMap.values()) {
                user.statsVideoCallbackView.setText("");
                user.statsVideoCallbackView.setVisibility(showRemote ? View.VISIBLE : View.GONE);
            }
            registerVideoSampleObserver(observePos, format);
        }

        String audioObserveKey = preferences.getString(getString(R.string.audio_observer_key),"-1");
        int audioObservePosition;
        try {
            audioObservePosition = Integer.parseInt(audioObserveKey);
        } catch (NumberFormatException e) {
            audioObservePosition = -1;
        }

         if (audioObservePosition == -1) {
             mUserSelf.statsAudioCallbackView.setText("");
             mUserSelf.statsAudioCallbackView.setVisibility(View.GONE);
         } else {
             mUserSelf.statsAudioCallbackView.setVisibility(View.VISIBLE);
             mRtcEngine.enableAudioFrameObserver(true, audioObservePosition);
             mRtcEngine.registerAudioFrameObserver(new DingRtcEngine.DingRtcAudioFrameObserver() {
                 @Override
                 public void onCapturedAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
                     runOnUiThread(new Runnable() {
                         @Override
                         public void run() {
                             String des = "Audio Observer Captured: "
                                     + " numSamples: " + frame.numSamples
                                     + ", bytesPerSample: " + frame.bytesPerSample
                                     + ", numChannels: " + frame.numChannels
                                     + ", samplesPerSec: " + frame.samplesPerSec;
                             mUserSelf.statsAudioCallbackView.setText(des);
                         }
                     });
                 }

                 @Override
                 public void onProcessCapturedAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
                     runOnUiThread(new Runnable() {
                         @Override
                         public void run() {
                             String des = "Audio Observer 3A: "
                                     + " numSamples: " + frame.numSamples
                                     + ", bytesPerSample: " + frame.bytesPerSample
                                     + ", numChannels: " + frame.numChannels
                                     + ", samplesPerSec: " + frame.samplesPerSec;
                             mUserSelf.statsAudioCallbackView.setText(des);
                         }
                     });
                 }

                 @Override
                 public void onPublishAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
                     runOnUiThread(new Runnable() {
                         @Override
                         public void run() {
                             String des = "Audio Observer Pub: "
                                     + " numSamples: " + frame.numSamples
                                     + ", bytesPerSample: " + frame.bytesPerSample
                                     + ", numChannels: " + frame.numChannels
                                     + ", samplesPerSec: " + frame.samplesPerSec;
                             mUserSelf.statsAudioCallbackView.setText(des);
                         }
                     });
                 }

                 @Override
                 public void onPlaybackAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
                 }

                 @Override
                 public void onRemoteUserAudioFrame(String uid, DingRtcEngine.DingRtcAudioFrame frame) {
                     FileWriteUtils.dumpRemoteUserAudio(RoomActivity.this.getApplicationContext(), uid, frame);
                 }

                 @Override
                 public void onProcessCapturedWithPlaybackAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
                 }
             });
         }

        String cameraRotation = preferences.getString(getString(R.string.title_camera_rotation_key),"0");
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
        String encoderOrientation = preferences.getString(getString(R.string.video_encoder_orientation_key),"0");
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

        String orientationString = preferences.getString(getString(R.string.video_camera_orientation_key), "4");
        DingRtcEngine.DingRtcOrientationMode orientationMode = DingRtcEngine.DingRtcOrientationMode.AUTO;
        if ("0".equals(orientationString)) {
            orientationMode = DingRtcEngine.DingRtcOrientationMode.PORTRAIT;
        } else if ("90".equals(orientationString)) {
            orientationMode = DingRtcEngine.DingRtcOrientationMode.LANDSCAPE_LEFT;
        } else if ("180".equals(orientationString)) {
            orientationMode = DingRtcEngine.DingRtcOrientationMode.PORTRAIT_UPSIDE_DOWN;
        } else if ("270".equals(orientationString)) {
            orientationMode = DingRtcEngine.DingRtcOrientationMode.LANDSCAPE_RIGHT;
        }
        mRtcEngine.setDeviceOrientationMode(orientationMode);

        mEnableAudioVoiceChange = preferences.getBoolean(getString(R.string.audio_voice_change_key), false);
        if (mEnableAudioVoiceChange) {
            DingRtcEngine.DingRtcAudioFrameObserverConfig config = new DingRtcEngine.DingRtcAudioFrameObserverConfig();
            config.permission = DingRtcEngine.DingRtcAudioFramePermission.DingRtcAudioFramePermissionReadAndWrite;
            mRtcEngine.enableAudioFrameObserver(true, RtcEngineAudioPositionProcessCaptured, config);
            mRtcEngine.registerAudioFrameObserver(new DingRtcEngine.DingRtcAudioFrameObserver() {
                @Override
                public void onCapturedAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
                }

                @Override
                public void onProcessCapturedAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
                    byte[] data = new byte[frame.numChannels * frame.numSamples * frame.bytesPerSample];
                    frame.data.clear();
                    frame.data.put(data);
                }

                @Override
                public void onPublishAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
                }

                @Override
                public void onPlaybackAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
                }

                @Override
                public void onRemoteUserAudioFrame(String uid, DingRtcEngine.DingRtcAudioFrame frame) {
                    
                }

                @Override
                public void onProcessCapturedWithPlaybackAudioFrame(DingRtcEngine.DingRtcAudioFrame frame) {
                }
            });
        }

        boolean enableVideoDump = preferences.getBoolean(getString(R.string.video_dump_id_key), false);
        if(enableVideoDump) {
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
    protected void onPause() {
        super.onPause();

        if (mAnnotationToolBar.isShowing()) {
            mAnnotationToolBar.dismiss();
        }
    }

    @Override
    protected void onDestroy() {
        resetCameraBrightnessPref();
        if(mRtcEngine != null) {
            mRtcEngine.unRegisterVideoSampleObserver();
            mRtcEngine.destroy();
            mRtcEngine = null;
        }
        super.onDestroy();
        if (countDownTimer != null) {
            countDownTimer.cancel();
        }
        GroupMgr.getInstance().clearGroup();
        RtmClientHolder.getInstance().clear();
        RtmSessionMgr.getInstance().clearSession();
        FileWriteUtils.destroy();
        SubtitleManager.getInstance().clear();
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
        if(requestCode == SettingActivity.SETTING_REQUEST_CODE) {
            configFromPreference(PreferenceManager.getDefaultSharedPreferences(this),false);
        }
    }

    private void configFromPreference(SharedPreferences preferences,boolean first) {
        mSettingSubAllVideo = preferences.getBoolean(getString(R.string.title_subscribe_all_video_key),true);
        mSettingSubAllAudio = preferences.getBoolean(getString(R.string.title_subscribe_all_audio_key),true);
        mVirtualBackground = preferences.getString(getString(R.string.title_virtual_background_key),"0");
        mDefaultSubStreamType = DingRtcEngine.DingRtcVideoStreamType.fromValue(Integer.parseInt(preferences.getString(getString(R.string.demo_subscribe_resolution_key),"4")));
        mPublishResolution = Integer.parseInt(preferences.getString(getString(R.string.demo_publish_resolution_key),"16"));
        mPublishFps = Integer.parseInt(preferences.getString(getString(R.string.demo_publish_fps_key),"30"));
        mScreenSharePublishResolution = Integer.parseInt(preferences.getString(getString(R.string.demo_screen_share_publish_resolution_key),"16"));
        mScreenSharePublishFps = Integer.parseInt(preferences.getString(getString(R.string.demo_screen_share_publish_fps_key),"15"));
        int renderMode = Integer.parseInt(preferences.getString(getString(R.string.demo_render_crop_mode_key),"0"));
        switch (renderMode) {
            case 0:
                mDefaultRenderMode = DingRtcRenderModeAuto;
                break;
            case 1:
                mDefaultRenderMode = DingRtcRenderModeStretch;
                break;
            case 2:
                mDefaultRenderMode = DingRtcRenderModeFill;
                break;
            case 3:
                mDefaultRenderMode = DingRtcRenderModeCrop;
                break;
            default:
                break;
        }
        if(first) {
            mUserSelf.isVideoStarted = preferences.getBoolean(getString(R.string.title_auto_start_camera_key),true);
            mUserSelf.isVideoPreview = mUserSelf.isVideoStarted;
            mUserSelf.isAudioStarted = preferences.getBoolean(getString(R.string.title_auto_start_audio_key),true);
            mUserSelf.isVideoEnabled = preferences.getBoolean(getString(R.string.title_auto_start_camera_key),true);
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
    }

    private void initAudioGroup() {
        mAudioGroupPopup = new GroupListPopup(this, mRtcEngine);
        findViewById(R.id.group_float_btn).setOnClickListener(v -> {
            if (!mAudioGroupPopup.isShowing()) {
                mAudioGroupPopup.showAtLocation(v, Gravity.TOP, 0, 0);
            }
        });
    }

    private void initSwitchOrientation() {
        findViewById(R.id.switch_orientation_btn).setOnClickListener(v -> {
            if (mIsPortrait) {
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
            } else {
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
            }
        });
    }

    private void initRtmChat() {
        RtmClientHolder.getInstance().init(mRtcEngine.getRtmClient());

        mRtmChatPopup = new RtmChatPopup(this, mRtcEngine.getRtmClient(), mAuthInfo.userId);
        findViewById(R.id.rtm_chat_btn).setOnClickListener(v -> {
            if (!mRtmChatPopup.isShowing()) {
                mRtmChatPopup.showAtLocation(v, Gravity.TOP, 0, 0);
            }
        });
    }

    private void initSubTitle() {
        mSubtitleManager = mRtcEngine.getSubtitleManager();
        mSubtitleManager.setSubtitleManagerEventListener(new DingRtcEngineSubtitleManagerListener() {
            @Override
            public void onLanguageListsUpdated() {
                showToast("语言列表有更新！");
            }

            @Override
            public void onSubtitleServiceStatusChanged(DingRtcEngineSubtitleTypes.DingRtcSubtitleStatus status) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (status == DingRtcEngineSubtitleTypes.DingRtcSubtitleStatus.OPEN) {
                            mSubtitleOpen = true;
                            mSubtitleServiceStateTv.setText("字幕服务状态：开启");
                            if (SubtitleManager.getInstance().hasStartService()) {
                                mSubtitleServiceStartBtn.setText("停止服务");
                            } else {
                                mSubTitleServiceSettingsView.setVisibility(View.GONE);
                            }
                        } else if (status == DingRtcEngineSubtitleTypes.DingRtcSubtitleStatus.CLOSE) {
                            mSubtitleOpen = false;
                            mSubtitleServiceStateTv.setText("字幕服务状态：关闭");
                            mSubtitleServiceStartBtn.setText("开启服务");
                            mSubTitleServiceSettingsView.setVisibility(View.VISIBLE);
                        } else if (status == DingRtcEngineSubtitleTypes.DingRtcSubtitleStatus.ERROR) {
                            mSubtitleServiceStateTv.setText("字幕服务状态：异常");
                        }
                    }
                });
            }

            @Override
            public void onSubtitleMessage(DingRtcEngineSubtitleTypes.DingRtcSubtitleMessage message) {
                Log.d("onSubtitleMessage", "userId: " + message.userId
                        + ", subtitle: " + message.subtitle
                        + ", sentenceIndex: " + message.sentenceIndex
                        + ", sentenceEnd: " + message.sentenceEnd
                        + ", translate: " + message.translate
                        + ", language: " + message.language);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        SubtitleManager.getInstance().onSubtitleMessage(message);
                        mSubtitleTranslateAdapter.updateMessage(SubtitleManager.getInstance().getSubtitles());
                        mSubtitleTranslateAdapter.notifyDataSetChanged();
                    }
                });
            }
        });

        mSubtitleView = findViewById(R.id.subtitle_view);

        mSubTitleServiceSettingsView = findViewById(R.id.subtitle_service_settings);
        mSubtitleTaskIdView = findViewById(R.id.task_id);
        mSubtitleServiceTypeSpinner = findViewById(R.id.subtitle_service_type_spinner);
        mSubtitleServiceTypeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (position == 0) {
                    mSubtitleServiceType = 1;
                } else if (position ==1) {
                    mSubtitleServiceType = 2;
                } else if (position == 2) {
                    mSubtitleServiceType = 3;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        mSubtitleServiceStartBtn = findViewById(R.id.subtitle_start_btn);
        mSubtitleServiceStartBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mSubtitleOpen) {
                    SubtitleManager.getInstance().stopService(new SubtitleManager.SubtitleStopCallback() {
                        @Override
                        public void onSubtitleServiceStopResult(boolean success, String reason) {
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    if (success) {
                                        mSubtitleServiceStateTv.setText("字幕服务状态：关闭");
                                        mSubtitleServiceStartBtn.setText("开启服务");
                                        mSubtitleTaskIdView.setText("TaskId: ");
                                    } else {
                                        RoomActivity.this.showToast("字幕服务关闭失败，reason: " + reason);
                                    }
                                }
                            });
                        }
                    });
                } else {
                    SubtitleManager.getInstance().startService(mAuthInfo.channelId, mAuthInfo.appId,
                            mSubtitleServiceType, new SubtitleManager.SubtitleStartCallback() {
                                @Override
                                public void onSubtitleServiceStartResult(boolean success, String taskId, String reason) {
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            if (success) {
                                                mSubtitleServiceStateTv.setText("字幕服务状态：开启");
                                                mSubtitleServiceStartBtn.setText("停止服务");
                                                mSubtitleTaskIdView.setText("TaskId: " + taskId);
                                            } else {
                                                RoomActivity.this.showToast("字幕服务开启失败，reason: " + reason);
                                            }
                                        }
                                    });
                                }
                            });
                }
            }
        });

        mSubtitleServiceStateTv = findViewById(R.id.subtitle_service_state);

        mSubtitleSpeakLanguageSpinner = findViewById(R.id.subtitle_speak_language_spinner);

        String[] supportedSpeakLanguages = mSubtitleManager.getSupportedSpeakLanguages();
        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item,
                SubtitleManager.getInstance().getCnSpeakLanguages(supportedSpeakLanguages));
        mSubtitleSpeakLanguageSpinner.setAdapter(adapter);
        mSubtitleSpeakLanguageSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                mCurrentSpeakLanguage = supportedSpeakLanguages[position];
                mSubtitleManager.setCurrentSpeakLanguage(mCurrentSpeakLanguage);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        mSubtitleTranslateLanguageBtn = findViewById(R.id.subtitle_translate_language_btn);
        mSubtitleTranslateLanguageBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AlertDialog.Builder builder = new AlertDialog.Builder(RoomActivity.this);
                builder.setTitle("选择翻译语言（可多选）");
                String[] supportedTranslateLanguages = mSubtitleManager.getSupportedTranslateLanguages();
                if (mSelectedTranslateLanguages == null || mSelectedTranslateLanguages.length != supportedTranslateLanguages.length) {
                    mSelectedTranslateLanguages = new boolean[supportedTranslateLanguages.length];
                }
                builder.setMultiChoiceItems(SubtitleManager.getInstance().getCnTranslateLanguages(supportedTranslateLanguages),
                        mSelectedTranslateLanguages, new DialogInterface.OnMultiChoiceClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which, boolean isChecked) {
                        mSelectedTranslateLanguages[which] = isChecked;
                    }
                });

                builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        ArrayList<String> selectedLans = new ArrayList<>();
                        for (int i = 0; i < mSelectedTranslateLanguages.length; i++) {
                            if (mSelectedTranslateLanguages[i]) {
                                selectedLans.add(supportedTranslateLanguages[i]);
                            }
                        }
                        if (selectedLans.size() == 0) {
                            showToast("请至少选择一种翻译语言！");
                        } else {
                            mSubtitleManager.setCurrentTranslateLanguages(selectedLans);
                        }
                    }
                });
                builder.show();
            }
        });

        mSubtitleTranslateLanguageListView = findViewById(R.id.subtitle_translate_language_list);
        mSubtitleTranslateAdapter = new SubtitleTranslateAdapter(this);
        mSubtitleTranslateLanguageListView.setAdapter(mSubtitleTranslateAdapter);
    }

    private void initAnnotation() {
        mAnnotationToolBar = new AnnotationToolBar(this);
        mAnnoSnapshotDir = getExternalFilesDir("annotation").getAbsolutePath()
                + File.separator + "snapshot";
    }

    private void onClickSubtitle() {
        if (isSubtitleViewVisible()) {
            mSubtitleManager.enableSubtitle(false);
            mSubtitleView.setVisibility(View.GONE);
        } else {
            mSubtitleManager.enableSubtitle(true);
            mSubtitleView.setVisibility(View.VISIBLE);
        }
    }

    private boolean isSubtitleViewVisible() {
        return mSubtitleView.getVisibility() == View.VISIBLE;
    }

    private void initTitleView() {
        mTitleView = findViewById(R.id.room_top_layout);
        mTitleText = findViewById(R.id.tv_title);
        mTitleText.setText(R.string.title_join_call);
        mConnectStateText = findViewById(R.id.tv_connect_state);

        mSpeakerView = findViewById(R.id.iv_left_icon);
        mSpeakerView.setImageResource(R.drawable.speaker);
        mSpeakerView.setOnClickListener(view -> {
            final List<DingRtcAudioDeviceInfo> audiDevices = mRtcEngine.getAudioDevices();
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
                        DingRtcAudioDeviceInfo deviceInfo = audiDevices.get(which);
                        showToast("选择了音频设备 : " + items[which]);
                        mRtcEngine.setAudioRouteType(deviceInfo.type);
                        mAudioRouteTypeDialog.dismiss();
                    }
                })
                .setTitle("请选择音频播放设备")
                .create();
            mAudioRouteTypeDialog.show();
        });
        final List<DingRtcAudioDeviceInfo> audiDevices = mRtcEngine.getAudioDevices();
        for (DingRtcAudioDeviceInfo info : audiDevices) {
            if (info.isUsed) {
                int sourceId = getAudioDeviceSource(info.type);
                mSpeakerView.setImageResource(sourceId);
            }
        }

        ImageView switchView = findViewById(R.id.iv_left_icon2);
        switchView.setVisibility(View.VISIBLE);
        switchView.setImageResource(R.drawable.switch_camera);
        switchView.setOnClickListener(view -> {
            resetCameraBrightnessPref();
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
            mUserSelf.flashView.setVisibility(mIsFront ? View.GONE : View.VISIBLE);
        });

        TextView finishView = findViewById(R.id.tv_right);
        finishView.setVisibility(View.VISIBLE);
        finishView.setText(R.string.title_finish);
        finishView.setOnClickListener(view -> {
            if(mIsJoin) {
                leaveChannel();
            } else {
                this.finish();
            }
        });
    }

    private void resetCameraBrightnessPref() {
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        preferences.edit().putInt(getString(R.string.camera_brightness_key), 0).apply();
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

        DrawableTextView pubScreenView = findViewById(R.id.bottom_pub_screen);
        pubScreenView.setOnClickListener(view -> {
            pubScreen(!mUserSelf.isScreenStarted);
            updateScreenUI(pubScreenView);
        });

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
        View popView = LayoutInflater.from(RoomActivity.this).inflate(R.layout.room_pop_window,null);
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
                AlertDialog dialog = new AlertDialog.Builder(RoomActivity.this)
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

        DrawableTextView deviceTest = popView.findViewById(R.id.more_device_test);
        deviceTest.setOnClickListener(view -> {
            updateStatsUI(deviceTest);
        });

        DrawableTextView sendMediaExMs = popView.findViewById(R.id.send_media_ex_msg);
        sendMediaExMs.setOnClickListener(view -> {
            EditText editText = new EditText(RoomActivity.this);
            editText.setHint("请输入扩展信息，不超过8字节");
            editText.setInputType(EditorInfo.TYPE_CLASS_TEXT);
            Dialog dlg = new AlertDialog.Builder(RoomActivity.this)
                    .setTitle("发送媒体扩展信息")
                    .setView(editText)
                    .setPositiveButton("确定", (dialog, which) -> {
                        String exMsg = editText.getText().toString();
                        String[] msgs = exMsg.split(",");
                        if (msgs.length != 8) {
                            showToast("输入非法");
                            return;
                        }
                        byte[] bytes = new byte[8];
                        for (int i = 0; i < 8; i++) {
                            String msg = msgs[i];
                            try {
                                int value = Integer.parseInt(msg);
                                if (value > 127 || value < -128) {
                                    showToast("输入非法");
                                    return;
                                }
                                bytes[i] = (byte)value;
                            } catch (NumberFormatException e) {
                                showToast("解析异常");
                                return;
                            }
                        }
                        int result = mRtcEngine.sendMediaExtensionMsg(bytes, 1);
                        Log.i(TAG, "sendMediaExtensionMsg result: " + result);
                        if (result == 0) {
                            showToast("发送成功");
                        } else {
                            showToast("发送失败，Code : " + result);
                        }
                    })
                    .setNegativeButton("取消", (dialog, which) -> dialog.dismiss())
                    .create();
            dlg.show();
        });

        DrawableTextView subtitle = popView.findViewById(R.id.subtitle);
        subtitle.setOnClickListener(v -> onClickSubtitle());

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
                    mQualityReportDialog = new QualityReportDialog(RoomActivity.this);
                }
                mQualityReportDialog.show();
                mQualityReportDialog.getWindow().setLayout(
                        DensityUtils.getScreenWidth(RoomActivity.this) * 5 / 6,
                        ConstraintLayout.LayoutParams.WRAP_CONTENT);
            }
        });

        DrawableTextView snapshot = popView.findViewById(R.id.more_snapshot);
        snapshot.setOnClickListener(view -> snapshot());

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
        DisplayMetrics dm = resources.getDisplayMetrics();
        mScreenWidth = dm.widthPixels;
        mScreenHeight = dm.heightPixels;
        mViewPager = findViewById(R.id.room_view_pager);
        mViewPagerAdapter = new ViewPagerAdapter();
        mViewPager.setAdapter(mViewPagerAdapter);
        mViewPageIndicator = findViewById(R.id.room_indicator);
        mViewPageIndicator.setViewPager(mViewPager);
        mViewPageIndicator.setOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            @Override
            public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {

            }

            @Override
            public void onPageSelected(int position) {
                if(mRemoteUserMap.size() > 0) {
                    //先删除上一页的画布
                    List<RoomUser> list = getUserListInCurrentPager();
                    mViewPagerCurrentPosition = position;
                    for(RoomUser user : list) {
                        updateRemoteUserUI(user,false);
                    }
                    //再设置当前页的画布
                    list = getUserListInCurrentPager();
                    for(RoomUser user : list) {
                        updateRemoteUserUI(user,false);
                    }
                }
            }

            @Override
            public void onPageScrollStateChanged(int state) {

            }
        });
        mRoomInfo = findViewById(R.id.room_user_count);
        initViewPagerUI();
    }

    private void updateRoomInfo() {
        mRoomInfo.setText("房间名:" + mAuthInfo.channelId +
                ",人数" + (mRemoteUserMap.size() + 1));
    }

    private GridLayout getGridLayout() {
        int rowCount = mGridItemCount / mGridColumnCount;
        GridLayout gridLayout = new GridLayout(this);
        gridLayout.setBackgroundColor(getResources().getColor(R.color.colorF5F7FF));
        gridLayout.setRowCount(rowCount);
        gridLayout.setColumnCount(mGridColumnCount);
        return gridLayout;
    }

    private GridLayout.LayoutParams getGridLayoutParams() {
        GridLayout.LayoutParams layoutParams = new GridLayout.LayoutParams();
        layoutParams.leftMargin = 2;
        layoutParams.topMargin = 2;
        layoutParams.bottomMargin = 2;
        layoutParams.rightMargin = 2;
        layoutParams.width = mScreenWidth / mGridColumnCount - 4;
        if (mIsPortrait) {
            layoutParams.height = layoutParams.width * 4 / 3;
        } else {
            layoutParams.height = (mScreenHeight - 80) / 2;
        }
        return layoutParams;
    }

    private void initViewPagerUI() {
        mGridLayoutList.add(getGridLayout());
        notifyViewPager();
    }

    private void notifyViewPager() {
        mViewPagerAdapter.add(mGridLayoutList); //update viewPager
        mViewPager.setCurrentItem(mViewPagerCurrentPosition);
        if(mGridLayoutList.size() > 1) {
            mViewPageIndicator.setVisibility(View.VISIBLE);
        } else {
            mViewPageIndicator.setVisibility(View.GONE);
        }
    }

    private void updateUIOnConfigurationChanged() {
        DisplayMetrics dm = this.getResources().getDisplayMetrics();
        mScreenWidth = dm.widthPixels;
        mScreenHeight = dm.heightPixels;
        mBottomView.setVisibility(mIsPortrait ? View.VISIBLE:View.GONE);
        mTitleView.setVisibility(mIsPortrait ? View.VISIBLE:View.GONE);
        for(GridLayout gridLayout : mGridLayoutList) {
            int count = gridLayout.getChildCount();
            List<View> tmpList = new ArrayList<>(count);
            for (int i = 0; i < count; i++) {
                tmpList.add(gridLayout.getChildAt(i));
            }
            gridLayout.removeAllViews();
            for (int i = 0; i < count; i++) {
                gridLayout.addView(tmpList.get(i), getGridLayoutParams());
            }
        }
    }

    private void addMySelf() {
        mUserSelf.userId = mAuthInfo.userId;
        createUserUI(mUserSelf);
        if(mUserSelf.isVideoPreview) {
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

    @SuppressLint("ClickableViewAccessibility")
    private void createUserUI(RoomUser user) {
        RelativeLayout renderLayout = (RelativeLayout) LayoutInflater.from(RoomActivity.this).inflate(R.layout.room_render_layout,null);
        renderLayout.setOnClickListener(view -> {
            if(!mIsPortrait) {
                boolean visible = mTitleView.getVisibility() == View.VISIBLE;
                mTitleView.setVisibility(visible ? View.GONE : View.VISIBLE);
                mBottomView.setVisibility(visible ? View.GONE : View.VISIBLE);
            }
        });
        user.rootView = renderLayout;
        user.rootView.setTag(user.userId);
        user.nickNameView = renderLayout.findViewById(R.id.room_render_name);
        if(TextUtils.isEmpty(user.nickName)) {
            user.nickNameView.setText(user.userId);
        } else {
            user.nickNameView.setText(user.userId + " " + user.nickName);
        }
        user.statsAudioView = renderLayout.findViewById(R.id.room_render_stats_audio_view);
        user.statsVideoView = renderLayout.findViewById(R.id.room_render_stats_video_view);
        user.statsScreenView = renderLayout.findViewById(R.id.room_render_stats_screen_view);
        user.statsAudioCallbackView = renderLayout.findViewById(R.id.room_render_audio_callback_view);
        user.statsVideoCallbackView = renderLayout.findViewById(R.id.room_render_video_callback_view);
        user.statsNetworkQualityView = renderLayout.findViewById(R.id.room_render_network_quality);
        user.renderBigGroup = renderLayout.findViewById(R.id.room_render_big_view);
        user.renderSmallGroup = renderLayout.findViewById(R.id.room_render_small_view);
        user.userStatView = renderLayout.findViewById(R.id.user_stat);
        user.annotationView = renderLayout.findViewById(R.id.annotation);
        View statsLayout = renderLayout.findViewById(R.id.room_render_status_view);
        if(mUserSelf == user) {
            statsLayout.setVisibility(View.GONE);

            user.flashView = renderLayout.findViewById(R.id.flash);
            user.flashView.setVisibility(mIsFront ? View.GONE : View.VISIBLE);
            user.flashView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (flash) {
                        flash = false;
                        mRtcEngine.setCameraFlash(false);
                    } else {
                        flash = true;
                        mRtcEngine.setCameraFlash(true);
                    }
                }
            });

            user.zoomView = renderLayout.findViewById(R.id.zoom);
            user.zoomView.setVisibility(View.VISIBLE);
            user.zoomView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (zoom) {
                        zoom = false;
                        user.zoomView.setImageResource(R.drawable.zoom_in);
                        mRtcEngine.setCameraZoom(1.0f);
                    } else {
                        zoom = true;
                        user.zoomView.setImageResource(R.drawable.zoom_out);
                        mRtcEngine.setCameraZoom(1.5f);
                    }
                }
            });

            renderLayout.setOnTouchListener(new View.OnTouchListener() {
                @Override
                public boolean onTouch(View v, MotionEvent event) {
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        float x = event.getX() / renderLayout.getWidth();
                        float y = event.getY() / renderLayout.getHeight();
                        Log.d(TAG, "setCameraExposurePoint : " + x + "@" + y);
                        mRtcEngine.setCameraExposurePoint(x, y);
                        mRtcEngine.setCameraFocusPoint(x, y);
                    }
                    return true;
                }
            });
        } else {
            user.muteRemoteAudioView = renderLayout.findViewById(R.id.mute_remote_audio);
            user.muteRemoteAudioView.setVisibility(View.VISIBLE);
            user.muteRemoteAudioView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (user.muteRemoteAudio) {
                        mRtcEngine.muteRemoteAudio(user.userId, false);
                        user.muteRemoteAudioView.setImageResource(R.drawable.unmute_remote);
                        user.muteRemoteAudio = false;
                    } else {
                        mRtcEngine.muteRemoteAudio(user.userId, true);
                        user.muteRemoteAudioView.setImageResource(R.drawable.mute_remote);
                        user.muteRemoteAudio = true;
                    }
                }
            });
            user.muteRemoteAudioView.setOnLongClickListener(new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View v) {
                    RemoteUserVolumeDialog dialog = new RemoteUserVolumeDialog(RoomActivity.this, mRtcEngine, user);
                    dialog.show();
                    return false;
                }
            });

            user.remoteUserSubView = renderLayout.findViewById(R.id.user_sub);
            user.remoteUserSubView.setVisibility(View.VISIBLE);
            user.remoteUserSubView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    new AlertDialog.Builder(RoomActivity.this)
                            .setTitle("操作")
                            .setSingleChoiceItems(R.array.demo_subscribe_resolution,
                                    user.videoSubscribeType.getValue() - 1, ((dialog, which) -> {
                                        mRtcEngine.setRemoteVideoStreamType(user.userId, DingRtcEngine.DingRtcVideoStreamType.fromValue(which + 1));
                                        dialog.dismiss();
                                    })).create().show();
                }
            });
        }

        user.annotationView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onClickAnnotation(user, DingRtcWhiteBoardTypes.DingRtcAnnoType.CAMERA);
            }
        });

        user.subStatusAudio = renderLayout.findViewById(R.id.room_render_status_audio);
        user.subStatusAudio.setVisibility(View.VISIBLE);
        user.subStatusAudio.setOnClickListener(view -> {
            if(user.isAudioStarted) {
                showToastL("不支持单个audio track订阅操作");
            }
        });
        if(mSettingSubAllAudio) {
            user.subStatusAudio.setBackgroundResource(R.drawable.selector_sub_background);
        }
        user.subStatusVideo = renderLayout.findViewById(R.id.room_render_status_video);
        user.subStatusVideo.setOnClickListener(view -> {
            if(user.isVideoStarted) {
                boolean sub = (user.videoSubscribeState == DingRtcStatsSubscribed);
                mRtcEngine.subscribeRemoteVideoStream(user.userId,DingRtcVideoTrackCamera,!sub);
                if(sub) { //FIXME: sub/unsub 如果不重置画布会崩溃
                    removeVideoCanvas(user.userId);
                } else {
                    addVideoCanvas(user.userId);
                }
            }
        });
        user.subStatusScreen = renderLayout.findViewById(R.id.room_render_status_screen);
        user.subStatusScreen.setVisibility(View.VISIBLE);
        user.subStatusScreen.setOnClickListener(view -> {
            if(user.isScreenStarted) {
                boolean sub = (user.screenSubscribeState == DingRtcStatsSubscribed);
                mRtcEngine.subscribeRemoteVideoStream(user.userId,DingRtcVideoTrackScreen,!sub);
                if(sub) {
                    removeScreenCanvas(user.userId);
                } else {
                    addScreenCanvas(user.userId);
                }
            }
        });
        user.videoSubscribeType = mDefaultSubStreamType;
        if(user == mUserSelf) {
            mGridLayoutList.get(0).addView(renderLayout, getGridLayoutParams());
        } else {
            int userCount = mRemoteUserMap.size();
            int pageIndex = userCount / mGridItemCount;
            if(mGridLayoutList.size() <= pageIndex) {
                mGridLayoutList.add(getGridLayout());
                notifyViewPager();
            }
            mGridLayoutList.get(pageIndex).addView(renderLayout, getGridLayoutParams());
        }

        user.mirrorView = renderLayout.findViewById(R.id.mirror);
        user.mirrorView.setVisibility(View.VISIBLE);
        user.mirrorView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (user.mirrorMode == DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeOnlyFront
                        || user.mirrorMode == DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeAllEnabled) {
                    user.mirrorMode = DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeAllDisable;
                } else {
                    user.mirrorMode = DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeAllEnabled;
                }
                user.cameraCanvas.mirrorMode = user.mirrorMode;
                if (mUserSelf == user) {
                    mRtcEngine.updateViewConfig(user.cameraCanvas, "",  DingRtcVideoTrackCamera);
                } else {
                    mRtcEngine.updateViewConfig(user.cameraCanvas, user.userId, DingRtcVideoTrackCamera);
                }
            }
        });
        user.userStatView.setVisibility(View.VISIBLE);
        user.userStatView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (user.isSelf) {
                    user.isCameraOn = mRtcEngine.isCameraOn();
                }
                UserStatesDialog dialog = new UserStatesDialog(RoomActivity.this, user);
                dialog.show();
                dialog.getWindow().setLayout(
                        DensityUtils.getScreenWidth(RoomActivity.this) * 3 / 4,
                        ConstraintLayout.LayoutParams.WRAP_CONTENT);
            }
        });

        user.rotateView = renderLayout.findViewById(R.id.rotate);
        user.rotateView.setVisibility(View.VISIBLE);
        user.rotateView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (user.rotationMode == DingRtcRotationMode_0) {
                    user.rotationMode = DingRtcRotationMode_270;
                } else if (user.rotationMode == DingRtcRotationMode_90) {
                    user.rotationMode = DingRtcRotationMode_0;
                } else if (user.rotationMode == DingRtcRotationMode_180) {
                    user.rotationMode = DingRtcRotationMode_90;
                } else if (user.rotationMode == DingRtcRotationMode_270) {
                    user.rotationMode = DingRtcRotationMode_180;
                }
                user.cameraCanvas.rotationMode = user.rotationMode;
                if (mUserSelf == user) {
                    mRtcEngine.updateViewConfig(user.cameraCanvas, "",  DingRtcVideoTrackCamera);
                } else {
                    mRtcEngine.updateViewConfig(user.cameraCanvas, user.userId, DingRtcVideoTrackCamera);
                }
            }
        });
    }

    private int getUserInPagerPosition(RoomUser user) {
        List<RoomUser> tmpList = new ArrayList<>(mRemoteUserMap.values());
        int index = tmpList.indexOf(user) + 1;
        return index / mGridItemCount;
    }

    private List<RoomUser> getUserListInCurrentPager() {
        List<RoomUser> tmpList = new ArrayList<>(mRemoteUserMap.values());
        int start = mViewPagerCurrentPosition * mGridItemCount - 1;
        int end = Math.min(start + mGridItemCount,mRemoteUserMap.size());
        if(start < 0) {
            start = 0;
        }
        return tmpList.subList(start,end);
    }

    private void destroyUserUI(RoomUser user) {
        if (user != null) {
            if (user.rootView != null) {
                if(user == mUserSelf) {
                    mGridLayoutList.get(0).removeView(user.rootView);
                } else {
                    int pageIndex = getUserInPagerPosition(user);
                    GridLayout currentLayout = mGridLayoutList.get(pageIndex);
                    currentLayout.removeView(user.rootView);
                    if(currentLayout.getChildCount() == 0) {
                        mGridLayoutList.remove(pageIndex);
                        notifyViewPager();
                    } else {
                        updateNextLayout(pageIndex);
                    }
                }
                user.rootView = null;
            }
        }
    }

    private void updateNextLayout(int index) {
        GridLayout currentLayout = mGridLayoutList.get(index);
        if(mGridLayoutList.size() > (index + 1)) {
            GridLayout nextLayout = mGridLayoutList.get(index + 1);
            if(nextLayout != null) {
                View firstView = nextLayout.getChildAt(0);
                if (firstView != null) {
                    nextLayout.removeView(firstView);
                    currentLayout.addView(firstView);
                    String uid = (String) firstView.getTag();
                    RoomUser user = mRemoteUserMap.get(uid);
                    if(user != null) {
                        updateRemoteUserUI(user,index == 0);
                    }
                }
                if(nextLayout.getChildCount() == 0) {
                    mGridLayoutList.remove(index + 1);
                    notifyViewPager();
                } else {
                    if(mGridLayoutList.size() > index + 1) {
                        updateNextLayout(index + 1);
                    }
                }
            }
        }
    }

    private void addUser(String uid) {
        RoomUser user = new RoomUser();
        user.userId = uid;
        DingRtcRemoteUserInfo remoteUserInfo = mRtcEngine.getUserInfo(uid);
        if(remoteUserInfo != null) {
            user.nickName = remoteUserInfo.getDisplayName();
        }
        mRemoteUserMap.put(user.userId, user);
    }

    private void removeUser(String uid) {
        RoomUser user = mRemoteUserMap.get(uid);
        destroyUserUI(user);
        mRemoteUserMap.remove(uid);

    }
    private void initRtcEngine() {
        DingRtcEngine.setLogDirPath(getExternalFilesDir("log").getAbsolutePath());
        String mcsConfig = "";
        mRtcEngine = DingRtcEngine.create(getApplicationContext(),mcsConfig);
        mRtcEngine.enableAudioLevelObserver(DingRtcEngine.DingRtcDeviceType.RtcEngineTypeAudioRecording, true);
        mRtcEngine.enableAudioLevelObserver(DingRtcEngine.DingRtcDeviceType.RtcEngineTypeAudioPlayout, true);
        mRtcEngine.subscribeAllRemoteVideoStreams(false); //由于demo使用了分页展示，因此需要手动订阅
        mRtcEngine.setRemoteDefaultVideoStreamType(mDefaultSubStreamType);

        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        mRtcEngine.subscribeAllRemoteAudioStreams(mSettingSubAllAudio);
        mLastSubAllAudio = mSettingSubAllAudio;

        String audioProfileStr = preferences.getString(getString(R.string.audio_profile_key), "1");
        DingRtcEngine.DingRtcAudioProfile audioProfile;
        if ("1".equals(audioProfileStr)) {
            audioProfile = DingRtcEngine.DingRtcAudioProfile.DingRtcEngineHighQualityMode;
        } else if ("2".equals(audioProfileStr)) {
            audioProfile = DingRtcEngine.DingRtcAudioProfile.DingRtcEngineSuperHighQualityMode;
        } else if ("3".equals(audioProfileStr))  {
            audioProfile = DingRtcEngine.DingRtcAudioProfile.DingRtcEngineStereoHighQualityMode;
        } else if ("4".equals(audioProfileStr)) {
            audioProfile = DingRtcEngine.DingRtcAudioProfile.DingRtcEngineStereoSuperHighQualityMode;
        } else  {
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

        setVirtualBackground();

        mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
            @Override
            public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {
                super.onJoinChannelResult(result, channel, userId, elapsed);
                runOnUiThread(() -> {
                    if(result == 0) {
                        updateRoomInfo();
                        mIsJoin = true;
                        if(mUserSelf.isAudioStarted) {
                            pubAudio(true);
                        }
                        if(mUserSelf.isVideoStarted) {
                            pubVideo(true);
                        }
                    }
                });
            }

            @Override
            public void onLeaveChannelResult(int result, DingRtcEngine.DingRtcStats stats) {
                super.onLeaveChannelResult(result, stats);
                runOnUiThread(() ->{
                    if(mProgressDialog != null && mProgressDialog.isShowing()) {
                        mProgressDialog.dismiss();
                    }
                    finish();
                });
            }

            @Override
            public void onChannelRemainingTimeNotify(int remainingTimeInSec) {
                runOnUiThread(() ->{
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
            public void onAudioPublishStateChanged(DingRtcEngine.DingRtcPublishState oldState, DingRtcEngine.DingRtcPublishState newState, int elapseSinceLastState, String channel) {
                super.onAudioPublishStateChanged(oldState, newState, elapseSinceLastState, channel);
            }

            @Override
            public void onVideoPublishStateChanged(DingRtcEngine.DingRtcPublishState oldState, DingRtcEngine.DingRtcPublishState newState, int elapseSinceLastState, String channel) {
                super.onVideoPublishStateChanged(oldState, newState, elapseSinceLastState, channel);
            }

            @Override
            public void onDualStreamPublishStateChanged(DingRtcEngine.DingRtcPublishState oldState, DingRtcEngine.DingRtcPublishState newState, int elapseSinceLastState, String channel) {
                super.onDualStreamPublishStateChanged(oldState, newState, elapseSinceLastState, channel);
            }

            @Override
            public void onAudioSubscribeStateChanged(String uid, DingRtcEngine.DingRtcSubscribeState oldState, DingRtcEngine.DingRtcSubscribeState newState, int elapseSinceLastState, String channel) {
                super.onAudioSubscribeStateChanged(uid, oldState, newState, elapseSinceLastState, channel);
                if(newState != DingRtcStatsNoSubscribe &&
                        newState != DingRtcStatsSubscribed) {
                    return;
                }
                runOnUiThread(() -> {
                    RoomUser user = mRemoteUserMap.get(uid);
                    if(user != null) {
                        boolean sub = newState == DingRtcStatsSubscribed;
                        user.isAudioSubscribed = sub;
                        user.subStatusAudio.setBackgroundResource(sub ? R.drawable.selector_sub_background : 0);
                        user.statsAudioView.setText("");
                    }
                });
            }

            @Override
            public void onVideoSubscribeStateChanged(String uid, DingRtcEngine.DingRtcSubscribeState oldState, DingRtcEngine.DingRtcSubscribeState newState, int elapseSinceLastState, String channel) {
                super.onVideoSubscribeStateChanged(uid, oldState, newState, elapseSinceLastState, channel);
                runOnUiThread(() -> {
                    RoomUser user = mRemoteUserMap.get(uid);
                    if(user != null) {
                        boolean sub = newState == DingRtcStatsSubscribed;

                        user.videoSubscribeState = newState;
                        user.subStatusVideo.setBackgroundResource(sub ? R.drawable.selector_sub_background : 0);
                    }
                });
            }

            @Override
            public void onSubscribeStreamTypeChanged(String uid, DingRtcEngine.DingRtcVideoStreamType oldStreamType, DingRtcEngine.DingRtcVideoStreamType newStreamType, int elapseSinceLastState, String channel) {
                super.onSubscribeStreamTypeChanged(uid, oldStreamType, newStreamType, elapseSinceLastState, channel);
                if(newStreamType == DingRtcEngine.DingRtcVideoStreamType.DingRtcVideoStreamTypeNone) {
                    return;
                }
                runOnUiThread(() -> {
                    RoomUser user = mRemoteUserMap.get(uid);
                    if(user != null) {
                        user.videoSubscribeType = newStreamType;
                    }
                });
            }

            @Override
            public void onScreenShareSubscribeStateChanged(String uid, DingRtcEngine.DingRtcSubscribeState oldState, DingRtcEngine.DingRtcSubscribeState newState, int elapseSinceLastState, String channel) {
                super.onScreenShareSubscribeStateChanged(uid, oldState, newState, elapseSinceLastState, channel);
                runOnUiThread(() -> {
                    RoomUser user = mRemoteUserMap.get(uid);
                    if(user != null) {
                        boolean sub = newState == DingRtcStatsSubscribed;
                        user.screenSubscribeState = newState;
                        user.subStatusScreen.setBackgroundResource(sub ? R.drawable.selector_sub_background : 0);
                        if(sub) {
                            addScreenCanvas(user.userId);
                        }
                    }
                });
            }

            @Override
            public void onNetworkQualityChanged(String uid, DingRtcEngine.DingRtcNetworkQuality upQuality, DingRtcEngine.DingRtcNetworkQuality downQuality) {
                Log.i(TAG,"onNetworkQualityChanged, uid: " + uid + " upQuality: " + upQuality + " , downQuality: " + downQuality);
                runOnUiThread(() -> {
                    String upDeps = "UpQuality: " + networkQualityToStr(upQuality);
                    String downDeps = "DownQuality: " + networkQualityToStr(downQuality);
                    if (TextUtils.isEmpty(uid)) {
                        mUserSelf.statsNetworkQualityView.setText(upDeps + "\n" + downDeps);
                    } else {
                        RoomUser roomUser = mRemoteUserMap.get(uid);
                        if (roomUser != null) {
                            roomUser.statsNetworkQualityView.setText(upDeps + "\n" + downDeps);
                        }
                    }
                });
            }

            @Override
            public void onOccurWarning(int warn, String message) {
                super.onOccurWarning(warn, message);
            }

            @Override
            public void onOccurError(int error, String message) {
                super.onOccurError(error, message);
                showToastL("发生异常 code: " + error + ", msg: " + message);
                if (error == DingRtcErrorCode.ERR_SCREENSHARE_NO_PERMISSION || error == DingRtcErrorCode.ERR_SCREENSHARE_STOP_BY_SYSTEM) {
                    runOnUiThread(() -> {
                        pubScreen(!mUserSelf.isScreenStarted);
                        updateScreenUI(findViewById(R.id.bottom_pub_screen));
                    });
                }
            }

            @Override
            public void onOccurErrorDetail(DingRtcErrorDescription message) {
                super.onOccurErrorDetail(message);
                
                if(message instanceof DingRtcSubscribeErrorDescription) {
                    DingRtcSubscribeErrorDescription msg = (DingRtcSubscribeErrorDescription)message;
                    showToastL("发生订阅异常, code:" + msg.errorCode + " subscribeUid: " + msg.subscribeUid);
                } else {
                    showToastL("发生异常 code: " + message.errorCode);
                }
            }

            @Override
            public void onConnectionStatusChanged(DingRtcEngine.DingRtcConnectionStatus status, DingRtcEngine.DingRtcConnectionStatusChangeReason reason) {
                Log.i(TAG,"onConnectionStatusChanged, status: " + status + " , reason: " + reason);
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
                    if (mRemoteUserMap.get(uid) == null) {
                        addUser(uid);
                        RoomUser user = mRemoteUserMap.get(uid);
                        if (user != null) {
                            createUserUI(user);
                        }
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
                    RoomUser user = mRemoteUserMap.get(uid);
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
                    //TODO：确认是否需要这样频繁的更新 remote user UI
                    updateRemoteUserUI(user,false);
                });
            }

            @Override
            public void onFirstRemoteVideoFrameDrawn(String uid, DingRtcEngine.DingRtcVideoTrack videoTrack, int width, int height, int elapsed) {
                super.onFirstRemoteVideoFrameDrawn(uid, videoTrack, width, height, elapsed);
            }

            @Override
            public void onRenderRemoteVideoSizeChanged(String uid, DingRtcEngine.DingRtcVideoTrack videoTrack, int newWidth, int newHeight, int oldWidth, int oldHeight) {
                Log.i("onRenderRemoteVideoSizeChanged",", uid: " + uid + ", videoTrack: " + videoTrack
                        + ", newWidth: " + newWidth + ", newHeight: " + newHeight
                        + ", oldWidth: " + oldWidth + ", oldHeight: " + oldHeight);
                showToastL("RemoteVideoSizeChanged" + ", uid: " + uid + ", videoTrack: " + videoTrack
                        + ", newWidth: " + newWidth + ", newHeight: " + newHeight
                        + ", oldWidth: " + oldWidth + ", oldHeight: " + oldHeight);
            }

            @Override
            public void onRenderLocalVideoSizeChanged(DingRtcEngine.DingRtcVideoTrack videoTrack, int newWidth, int newHeight, int oldWidth, int oldHeight) {
                Log.i("onRenderLocalVideoSizeChanged",", videoTrack: " + videoTrack
                        + ", newWidth: " + newWidth + ", newHeight: " + newHeight
                        + ", oldWidth: " + oldWidth + ", oldHeight: " + oldHeight);
                showToastL("LocalVideoSizeChanged" + ", videoTrack: " + videoTrack
                        + ", newWidth: " + newWidth + ", newHeight: " + newHeight
                        + ", oldWidth: " + oldWidth + ", oldHeight: " + oldHeight);
            }

            @Override
            public void onFirstLocalVideoFrameDrawn(int width, int height, int elapsed) {
                super.onFirstLocalVideoFrameDrawn(width, height, elapsed);
            }

            @Override
            public void onFirstVideoFrameReceived(String uid, DingRtcEngine.DingRtcVideoTrack videoTrack, int timeCost) {
                super.onFirstVideoFrameReceived(uid, videoTrack, timeCost);
            }

            @Override
            public void onFirstVideoPacketSend(DingRtcEngine.DingRtcVideoTrack videoTrack, int timeCost) {
                super.onFirstVideoPacketSend(videoTrack, timeCost);
            }

            @Override
            public void onFirstAudioPacketSent(String uid, int timeCost) {
                super.onFirstAudioPacketSent(uid, timeCost);
            }

            @Override
            public void onFirstVideoPacketReceived(String uid, DingRtcEngine.DingRtcVideoTrack videoTrack, int timeCost) {
                showToast("用户" + uid + "视频首帧回调");
            }

            @Override
            public void onFirstAudioPacketReceived(String uid, int timeCost) {
                showToast("用户" + uid + "音频首帧回调");
            }

            @Override
            public void onBye(int code) {
                super.onBye(code);
                Log.i(TAG,"onBye: " + code);
                runOnUiThread(()->{
                    String text = "当前用户被踢出房间";
                    if(code == 1) {
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
            public void onDingRtcStats(DingRtcEngine.DingRtcStats DingRtcStats) {
                super.onDingRtcStats(DingRtcStats);
                Log.i(TAG,"onDingRtcStats: " + DingRtcStats);
            }

            @Override
            public void onUserAudioMuted(String uid, boolean isMute) {
                super.onUserAudioMuted(uid, isMute);
                runOnUiThread(()-> {
                    RoomUser user = mRemoteUserMap.get(uid);
                    if(user == null) {
                        return;
                    }
                    int res = isMute ? R.drawable.remote_mic_off : R.drawable.remote_mic_on;
                    user.subStatusAudio.setImageResource(res);
                    user.statsAudioView.setText("");
                });
            }

            @Override
            public void onUserVideoMuted(String uid, boolean isMute, DingRtcEngine.DingRtcVideoTrack track) {
                super.onUserVideoMuted(uid, isMute,track);
                runOnUiThread(()-> {
                    RoomUser user = mRemoteUserMap.get(uid);
                    if(user == null) {
                        return;
                    }
                    if(track == DingRtcVideoTrackCamera) {
                        int res = isMute ? R.drawable.remote_video_off : R.drawable.remote_video_on;
                        user.subStatusVideo.setImageResource(res);
                    } else {
                        int res = isMute ? R.drawable.remote_screen_off : R.drawable.remote_screen_on;
                        user.subStatusScreen.setImageResource(res);
                    }
                });
            }

            @Override
            public void onUserVideoEnabled(String uid, boolean enabled) {
                super.onUserVideoEnabled(uid, enabled);
                Log.i(TAG,"onUserVideoEnabled uid : " + uid + ", enabled : " + enabled);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        RoomUser user = mRemoteUserMap.get(uid);
                        if(user == null) {
                            return;
                        }
                        user.isCameraOn = enabled;
                    }
                });
            }

            @Override
            public void onUserWillResignActive(String uid) {
                Log.i(TAG,"onUserWillResignActive uid : " + uid);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        RoomUser user = mRemoteUserMap.get(uid);
                        if(user == null) {
                            return;
                        }
                        user.foreBackState = RoomUser.ForeBackState.Back;
                    }
                });
            }

            @Override
            public void onUserWillBecomeActive(String uid) {
                Log.i(TAG,"onUserWillBecomeActive uid : " + uid);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        RoomUser user = mRemoteUserMap.get(uid);
                        if(user == null) {
                            return;
                        }
                        user.foreBackState = RoomUser.ForeBackState.Fore;
                    }
                });
            }

            @Override
            public void onUserAudioInterruptedBegin(String uid) {
                Log.i(TAG,"onUserAudioInterruptedBegin uid : " + uid);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        showToast("用户: " + uid +"音频被中断");
                    }
                });
            }

            @Override
            public void onUserAudioInterruptedEnded(String uid) {
                Log.i(TAG,"onUserAudioInterruptedEnded uid : " + uid);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        showToast("用户: " + uid +"音频已恢复");
                    }
                });
            }

            @Override
            public void onMediaRecordEvent(int event, String filePath) {
                super.onMediaRecordEvent(event, filePath);
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
                Log.i(TAG,"onRtcLocalVideoStats: " + DingRtcStats);
                runOnUiThread(() -> {
                    boolean isCamera = DingRtcStats.track == DingRtcVideoTrackCamera;
                    String type = isCamera ? "video" : "screen";
                    String text = String.format(Locale.CHINA,type + " cap: %dx%d#%d enc: %dx%d#%d %dKbps",
                            DingRtcStats.captureFrameWidth,DingRtcStats.captureFrameHeight,DingRtcStats.captureFps,
                            DingRtcStats.encoderFrameWidth,DingRtcStats.encoderFrameHeight,DingRtcStats.encodeFps,
                            DingRtcStats.sentBitrate / 1000);
                    if(isCamera) {
                        mUserSelf.statsVideoView.setText(text);
                    } else {
                        mUserSelf.statsScreenView.setText(text);
                    }
                });
            }

            @Override
            public void onRtcRemoteVideoStats(DingRtcEngine.DingRtcRemoteVideoStats DingRtcStats) {
                Log.i(TAG,"onRtcRemoteVideoStats: " + DingRtcStats);
                runOnUiThread(() -> {
                    RoomUser user = mRemoteUserMap.get(DingRtcStats.userId);
                    if(user == null) {
                        return;
                    }
                    boolean isCamera = DingRtcStats.track == DingRtcVideoTrackCamera;
                    String type = isCamera ? "video" : "screen";
                    String text = String.format(Locale.CHINA,type + " : %dx%d#%d %dKbps",
                            DingRtcStats.width,DingRtcStats.height,DingRtcStats.decoderOutputFrameRate,
                            DingRtcStats.recvBitrate / 1000);
                    if(isCamera) {
                        user.statsVideoView.setText(text);
                    } else {
                        user.statsScreenView.setText(text);
                    }
                });
            }

            @Override
            public void onRtcRemoteAudioStats(DingRtcEngine.DingRtcRemoteAudioStats DingRtcStats) {
                Log.i(TAG,"onRtcRemoteAudioStats: " + DingRtcStats);
            }

            @Override
            public void onRtcLocalAudioStats(DingRtcEngine.DingRtcLocalAudioStats DingRtcStats) {
                super.onRtcLocalAudioStats(DingRtcStats);
                Log.i(TAG,"onRtcLocalAudioStats: " + DingRtcStats);
                runOnUiThread(() -> {
                    String text = String.format(Locale.CHINA,"audio: %dKbps vol: %d",DingRtcStats.sentBitrate/1000,DingRtcStats.inputLevel);
                    mUserSelf.statsAudioView.setText(text);
                });
            }

            @Override
            public void onAudioFocusChange(int focusChange) {
                super.onAudioFocusChange(focusChange);
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
            public void onAudioVolumeIndication(List<DingRtcEngine.DingRtcAudioVolumeInfo> speakers) {
                super.onAudioVolumeIndication(speakers);
                if (!speakers.isEmpty()) {
                    for (int i = 0; i < speakers.size(); i ++) {
                        final String uid = speakers.get(i).userId;
                        final int volume = speakers.get(i).volume;
                        final int speechState = speakers.get(i).speechState;
                        Log.i(TAG, "onAudioVolumeIndication userId: " + uid
                                + ", volume: " + volume +", speechState: " + speechState);

                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                RoomUser user;
                                if ("0".equals(uid)) {
                                    user = mUserSelf;
                                } else {
                                    user = mRemoteUserMap.get(uid);
                                }
                                if (user != null && user.isAudioStarted) {
                                    //int color = speechState == 0 ? Color.WHITE : Color.GREEN;
                                    //ImageViewCompat.setImageTintList(user.subStatusAudio, ColorStateList.valueOf(color));
                                    user.statsAudioView.setText("audio: vol : " + volume);
                                    if (speechState == 1) {
                                        user.rootView.setBackgroundResource(R.drawable.green_border);
                                    } else {
                                        user.rootView.setBackground(null);
                                    }
                                }
                            }
                        });
                    }
                }
            }

            @Override
            public void onRecordingDeviceAudioLevel(int level) {
                super.onRecordingDeviceAudioLevel(level);
                Log.i(TAG, "onRecordingDeviceAudioLevel， level: " + level);
            }

            @Override
            public void onPlayoutDeviceAudioLevel(int level) {
                super.onPlayoutDeviceAudioLevel(level);
                Log.i(TAG, "onPlayoutDeviceAudioLevel， level: " + level);
            }

            @Override
            public void onAudioPlayoutEnded() {
                super.onAudioPlayoutEnded();
                Log.i(TAG, "onAudioPlayoutEnded ");
            }

            @Override
            public void onApiCalledExecuted(int error, String api, String result) {
                super.onApiCalledExecuted(error, api, result);
            }

            @Override
            public void onRemoteVideoResolutionChanged(String uId, DingRtcEngine.DingRtcVideoTrack videoTrack, int oldWidth, int oldHeight, int newWidth, int newHeight) {
            }

            @Override
            public void onSnapshotComplete(String uid, DingRtcEngine.DingRtcVideoTrack track, String path, boolean success) {
                super.onSnapshotComplete(uid, track, path, success);
                runOnUiThread(() -> {
                    if (TextUtils.isEmpty(uid)) {
                        showToast("本地截图:" + success + ",track:" + track + ",path:" + path);
                    } else {
                        showToast("远端uid:" + uid + "," + success + ",track:" + track + ",path:" + path);
                    }
                });
            }

            @Override
            public void onAudioMixingStateChanged(DingRtcEngine.DingRtcAudioMixingStatusConfig status) {
                super.onAudioMixingStateChanged(status);
                Log.e(TAG, "onAudioMixingStateChanged， status: " + status.toString());
            }

            @Override
            public void onAudioDeviceStateChanged(String id, int type, int state) {
                Log.i(TAG, "onAudioDeviceStateChanged, id: " + id + ", type: " + type + ", state: " + state);
            }

            @Override
            public void onUserAudioRecordingDeviceMuted(String uid, boolean isMicMuted) {
                Log.i(TAG, "onUserAudioRecordingDeviceMuted, uid: " + uid + ", isMicMuted: " + isMicMuted);
            }

            @Override
            public void onUserSystemAudioMixed(String uid, boolean systemAudioMixed) {
                Log.i(TAG, "onUserSystemAudioMixed, uid: " + uid + ", systemAudioMixed: " + systemAudioMixed);
            }

            @Override
            public void onListAllAudioGroups(List<String> groups) {
                Log.i(TAG, "onListAllAudioGroups: " + Arrays.toString(new List[]{groups}));
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mAudioGroupPopup.onListAllAudioGroups(groups);
                    }
                });
            }

            @Override
            public void onAudioGroupJoinResult(int result, String errMsg, String group, List<DingRtcEngine.DingRtcAudioGroupMember> members) {
                Log.i(TAG, "onAudioGroupJoinResult: " + result + ", errMsg: " + errMsg
                        + ", group: " + group + ", members: " + Arrays.toString(new List[]{members}));
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mAudioGroupPopup.onAudioGroupJoinResult(result, errMsg, group, members);
                    }
                });
            }

            @Override
            public void onAudioGroupLeaveResult(int result, String errMsg, String group) {
                Log.i(TAG, "onAudioGroupLeaveResult: " + result + ", errMsg: " + errMsg + ", group: " + group);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mAudioGroupPopup.onAudioGroupLeaveResult(result, errMsg, group);
                    }
                });
            }

            @Override
            public void onAudioGroupDismissResult(int result, String errMsg, String group) {
                Log.i(TAG, "onAudioGroupDismissResult: " + result + ", errMsg: " + errMsg + ", group: " + group);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mAudioGroupPopup.onAudioGroupDismissResult(result, errMsg, group);
                    }
                });
            }

            @Override
            public void onAudioGroupMixResult(String group, boolean mix, int result, String reason) {
                Log.i(TAG, "onAudioGroupMixResult, group: " + group + ", mix: " + mix
                        + ", result: " + result + ", reason: " + reason);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mAudioGroupPopup.onAudioGroupMixResult(group, mix, result, reason);
                    }
                });
            }

            @Override
            public void onAudioGroupMemberUpdate(int updateOpt, String group, List<DingRtcEngine.DingRtcAudioGroupMember> members) {
                Log.i(TAG, "onAudioGroupMemberUpdate, updateOpt: " + updateOpt + ", group: " + group
                        + "members: " + Arrays.toString(new List[]{members}));
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mAudioGroupPopup.onAudioGroupMemberUpdate(updateOpt, group, members);
                    }
                });
            }

            @Override
            public void onAudioGroupHallMembers(List<DingRtcEngine.DingRtcAudioGroupMember> hallMembers) {
                Log.i(TAG, "onAudioGroupHallMembers, hallMembers: " + Arrays.toString(new List[]{hallMembers}));
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mAudioGroupPopup.onAudioGroupHallMembers(hallMembers);
                    }
                });
            }

            @Override
            public void onAudioGroupListUpdate(int updateOpt, String group) {
                Log.i(TAG, "onAudioGroupListUpdate, updateOpt: " + updateOpt + ", group: " + updateOpt);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mAudioGroupPopup.onAudioGroupListUpdate(updateOpt, group);
                    }
                });
            }

            @Override
            public void onGroupNameChanged(String group, String newName) {
                Log.i(TAG, "onGroupNameChanged, group: " + group + ", newName: " + newName);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mAudioGroupPopup.onGroupNameChanged(group, newName);
                    }
                });
            }

            @Override
            public void onSetGroupNameResult(int result, String errMsg, String group, String groupName) {
                Log.i(TAG, "onSetGroupNameResult, result: " + result + ", errMsg: " + errMsg
                        + ", group: " + group + ", groupName: " + groupName);
                String des = result == 0 ? "设置分组名称成功" : "设置分组名称失败，errMsg: " + errMsg;
                showToast(des);
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

        DingRtcEngineWhiteboardManager whiteboardManager = mRtcEngine.getWhiteBoardManager();
        whiteboardManager.setWhiteboardManagerEventListener(new DingRtcEngineWhiteboardManagerListener() {
            @Override
            public void onWhiteboardServerStateChanged(DingRtcWhiteBoardTypes.DingRtcWBServerState state, int reason) {
                Log.i(TAG, "onWhiteboardServerStateChanged " + state + ", reason:" + reason);
                if (state == DingRtcWhiteBoardTypes.DingRtcWBServerState.Available) {
                    showToast("白板服务可用");
                } else if (state == DingRtcWhiteBoardTypes.DingRtcWBServerState.Unavailable) {
                    showToast("白板服务不可用 reason:" + reason);
                }
            }

            @Override
            public void onWhiteboardStart(String whiteboardId, DingRtcWhiteBoardTypes.DingRtcWBConfig cfg) {
                Log.i(TAG, "onWhiteboardStart " + whiteboardId);
            }

            @Override
            public void onWhiteboardStop(String whiteboardId) {
                Log.i(TAG, "onWhiteboardStop " + whiteboardId);
            }

            @Override
            public void onAnnotationStart(DingRtcWhiteBoardTypes.DingRtcAnnoID annotationId, DingRtcWhiteBoardTypes.DingRtcAnnoConfig cfg) {
                Log.i(TAG, "onAnnotationStart " + annotationId.type + ", uid:" + annotationId.uid);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        startAnnotation(annotationId);
                    }
                });
                showToast("Annotation " + annotationId.uid + " started");
            }

            @Override
            public void onAnnotationStop(DingRtcWhiteBoardTypes.DingRtcAnnoID annotationId) {
                Log.i(TAG, "onAnnotationStop " + annotationId.type + ", uid:" + annotationId.uid);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        stopAnnotation(annotationId, true);
                    }
                });
                showToast("Annotation " + annotationId.uid + " stopped");
            }
        });
    }

    private void registerVideoSampleObserver(int pos, final DingRtcEngine.DingRtcVideoFormat format) {
        DingRtcEngine.DingRtcVideoObserver videoObserver = new DingRtcEngine.DingRtcVideoObserver() {
            @Override
            public boolean onLocalVideoSample(DingRtcEngine.DingRtcVideoSourceType sourceType, DingRtcEngine.DingRtcVideoSample videoSample) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        String type;
                        if (sourceType == DingRtcEngine.DingRtcVideoSourceType.DingRtcSdkVideoSourceScreenShareType) {
                            type = "screen";
                        } else {
                            type = "camera";
                        }
                        String des = "Observer capture: " + type + ", " + videoSample.width + "x" + videoSample.height
                                + ", " + videoSample.format;
                        mUserSelf.statsVideoCallbackView.setText(des);
                        saveVideoData(mAuthInfo.channelId, "self", sourceType.ordinal(),
                                DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPostCapture, videoSample);
                    }
                });
                return super.onLocalVideoSample(sourceType, videoSample);
            }

            @Override
            public boolean onRemoteVideoSample(String callId, DingRtcEngine.DingRtcVideoSourceType sourceType, DingRtcEngine.DingRtcVideoSample videoSample) {
                runOnUiThread(new Runnable() {

                    @Override
                    public void run() {
                        RoomUser user = mRemoteUserMap.get(callId);
                        if (user != null) {
                            String type;
                            if (sourceType == DingRtcEngine.DingRtcVideoSourceType.DingRtcSdkVideoSourceScreenShareType) {
                                type = "screen";
                            } else {
                                type = "camera";
                            }
                            String des = "Observer preRender: " + type + ", " + videoSample.width + "x" + videoSample.height
                                    + ", " + videoSample.format;
                            user.statsVideoCallbackView.setText(des);
                            saveVideoData(mAuthInfo.channelId, callId, sourceType.ordinal(),
                                    DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPreRender, videoSample);
                        }
                    }
                });
                return false;
            }

            @Override
            public boolean onPreEncodeVideoFrame(DingRtcEngine.DingRtcVideoSourceType sourceType, DingRtcEngine.DingRtcVideoSample videoSample) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        String type;
                        if (sourceType == DingRtcEngine.DingRtcVideoSourceType.DingRtcSdkVideoSourceScreenShareType) {
                            type = "screen";
                        } else {
                            type = "camera";
                        }
                        String des = "Observer preEncode: " + type + ", " + videoSample.width + "x" + videoSample.height
                                + ", " + videoSample.format;
                        mUserSelf.statsVideoCallbackView.setText(des);
                        saveVideoData(mAuthInfo.channelId, "self", sourceType.ordinal(),
                                DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPreEncoder, videoSample);
                    }
                });
                return false;
            }

            @Override
            public DingRtcEngine.DingRtcVideoFormat onGetVideoFormatPreference() {
                return format;
            }
        };
        if (videoObservePosition >= 0) {
            mRtcEngine.enableVideoSampleObserver(false, videoObservePosition);
        }
        mRtcEngine.enableVideoSampleObserver(true, pos);
        mRtcEngine.registerVideoSampleObserver(videoObserver);
        videoObservePosition = pos;
    }

    private void joinChannel() {
        //先rtc joinChannel，再joinMeeting
        int ret = mRtcEngine.joinChannel(mAuthInfo,mUserSelf.nickName);
        if(ret != 0) {
            showToastL("加入房间失败");
        }
    }

    private void leaveChannel() {

        mProgressDialog = new ProgressDialog(this);
        mProgressDialog.setMessage("离会中...");
        mProgressDialog.setCancelable(false);
        mProgressDialog.show();
        for (AnnotationInfo annoInfo : mAnnotationList) {
            if (annoInfo.userId.equals(mUserSelf.userId)) {
                annoInfo.stop();
            } else {
                annoInfo.close();
            }
        }
        mRtcEngine.leaveChannel();

        SubtitleManager.getInstance().reset();
    }

    private void updateRemoteUserUI(RoomUser user,boolean forceSub) {
        int position = getUserInPagerPosition(user);
        user.subStatusAudio.setVisibility(user.isAudioStarted ? View.VISIBLE : View.GONE);
        user.statsAudioView.setText("");
        user.subStatusVideo.setVisibility(user.isVideoStarted ? View.VISIBLE : View.GONE);
        user.subStatusScreen.setVisibility(user.isScreenStarted ? View.VISIBLE : View.GONE);
        if(user.isVideoStarted) {
            if(mSettingSubAllVideo) { // 只有在当前页才会订阅视频并设置画布
                if(position == mViewPagerCurrentPosition || forceSub) {
                    if(user.videoSubscribeState == DingRtcStatsNoSubscribe) {
                        user.videoSubscribeState = DingRtcStatsSubscribing;
                        mRtcEngine.subscribeRemoteVideoStream(user.userId,DingRtcVideoTrackCamera,true);
                        addVideoCanvas(user.userId);
                    }
                } else {
                    //如果不在当前显示页，则不设置画布
                    if(user.videoSubscribeState == DingRtcStatsSubscribed) {
                        mRtcEngine.subscribeRemoteVideoStream(user.userId,DingRtcVideoTrackCamera,false);
                        removeVideoCanvas(user.userId);
                    }
                }
            }
        } else {
            if(user.videoSubscribeState == DingRtcStatsSubscribed) {
                mRtcEngine.subscribeRemoteVideoStream(user.userId,DingRtcVideoTrackCamera,false);
                removeVideoCanvas(user.userId);
            }
        }

        if(user.isScreenStarted) {
            if(mSettingSubAllVideo) {
                if(position == mViewPagerCurrentPosition) {
                    if(user.screenSubscribeState == DingRtcStatsNoSubscribe) {
                        user.screenSubscribeState = DingRtcStatsSubscribing;
                        mRtcEngine.subscribeRemoteVideoStream(user.userId,DingRtcVideoTrackScreen,true);
//                        addScreenCanvas(user.userId); //FIXME:快速的设置相机画布和屏幕共享画布会崩溃，因此屏幕共享画布移到订阅成功的消息里设置
                    }
                } else {
                    if(user.screenSubscribeState == DingRtcStatsSubscribed) {
                        mRtcEngine.subscribeRemoteVideoStream(user.userId,DingRtcVideoTrackScreen,false);
                        removeScreenCanvas(user.userId);
                    }
                }
                user.renderSmallGroup.setVisibility(View.VISIBLE);
                user.statsScreenView.setVisibility(View.VISIBLE);
            }
        } else {
            if(user.screenSubscribeState == DingRtcStatsSubscribed) {
                mRtcEngine.subscribeRemoteVideoStream(user.userId,DingRtcVideoTrackScreen,false);
            }
            removeScreenCanvas(user.userId);
            user.statsScreenView.setText("");
            user.renderSmallGroup.setVisibility(View.GONE);
            user.statsScreenView.setVisibility(View.GONE);
        }
    }
    private void addVideoCanvas(String userId) {
        boolean isLocal = mUserSelf.userId.equals(userId);
        RoomUser user;
        if(isLocal) {
            user = mUserSelf;
        } else {
            user = mRemoteUserMap.get(userId);
        }
        if(user != null && user.cameraCanvas == null) {
            user.cameraCanvas = new DingRtcEngine.DingRtcVideoCanvas();
            SurfaceView cameraView = DingRtcEngine.createRenderSurfaceView(this);
            user.cameraCanvas.view = cameraView;
            user.cameraCanvas.renderMode = mDefaultRenderMode;
            if(isLocal) {
                mRtcEngine.setLocalViewConfig(user.cameraCanvas, DingRtcVideoTrackCamera);
            } else {
                mRtcEngine.setRemoteViewConfig(user.cameraCanvas,userId,DingRtcVideoTrackCamera);
            }
            // addView必须放在setLocalViewConfig/setRemoteViewConfig之后
            if(user.screenCanvas == null) {
                cameraView.setZOrderMediaOverlay(true);
            }
            user.renderBigGroup.addView(cameraView);
            GestureHandler gestureHandler = new GestureHandler(RoomActivity.this, new GestureHandler.Callback() {
                @Override
                public void onMove(float x, float y, float dx, float dy, float vx, float vy) {
                    String uid = isLocal ? "" : user.userId;
                    mRtcEngine.updateScreenMoving(uid, user.screenCanvas.view, dx, dy);
//                    mRtcEngine.updateScreenMoving("", user.screenCanvas.view, dx, dy);
                }

                @Override
                public void onScale(float factor, float focusX, float focusY, float spanX, float spanY) {
                    Log.i(TAG, "onScale: " + factor + "," + focusX + "," + focusY);
                    user.screenScale *= factor;
                    String uid = isLocal ? "" : user.userId;
                    //mRtcEngine.updateScreenScaling(uid, user.screenCanvas.view, user.screenScale);
//                     mRtcEngine.updateScreenScaling(user.userId, user.screenCanvas.view, 0.5f, focusX, focusY);
                    mRtcEngine.updateScreenScaling(uid, user.screenCanvas.view, user.screenScale, focusX, focusY);
                }
            });
            user.renderSmallGroup.setOnTouchListener((v, event) -> gestureHandler.handleEvent(event));

            DingRtcWhiteBoardTypes.DingRtcAnnoID annoID = new DingRtcWhiteBoardTypes.DingRtcAnnoID(DingRtcWhiteBoardTypes.DingRtcAnnoType.CAMERA, user.userId);
            AnnotationInfo annotationInfo = getAnnotation(annoID, false);
            if (annotationInfo != null && annotationInfo.isStarted) {
                startAnnotation(annoID);
            }
        }
    }

    private void removeVideoCanvas(String userId) {
        boolean isLocal = mUserSelf.userId.equals(userId);
        RoomUser user;
        if(isLocal) {
            user = mUserSelf;
        } else {
            user = mRemoteUserMap.get(userId);
        }
        if(user != null && user.cameraCanvas != null) {
            if(user.cameraCanvas.view != null) {
                user.renderBigGroup.removeView((SurfaceView)user.cameraCanvas.view);
            }
            //user.cameraCanvas = null;
            user.cameraCanvas.toBeRemoved = true;
            if(isLocal) {
                mRtcEngine.setLocalViewConfig(user.cameraCanvas,DingRtcVideoTrackCamera);
            } else {
                mRtcEngine.setRemoteViewConfig(user.cameraCanvas,userId,DingRtcVideoTrackCamera);
            }
            user.cameraCanvas = null;

            DingRtcWhiteBoardTypes.DingRtcAnnoID annoID = new DingRtcWhiteBoardTypes.DingRtcAnnoID(DingRtcWhiteBoardTypes.DingRtcAnnoType.CAMERA, user.userId);
            AnnotationInfo annotationInfo = getAnnotation(annoID, false);
            if (annotationInfo != null && annotationInfo.isStarted) {
                stopAnnotation(annoID, false);
            }
        }
    }

    private void addScreenCanvas(String userId) {
        boolean isLocal = mUserSelf.userId.equals(userId);
        RoomUser user;
        if(isLocal) {
            user = mUserSelf;
        } else {
            user = mRemoteUserMap.get(userId);
        }
        if(user != null && user.screenCanvas == null) {
            user.screenCanvas = new DingRtcEngine.DingRtcVideoCanvas();
            SurfaceView screenView = DingRtcEngine.createRenderSurfaceView(this);
            user.screenCanvas.view = screenView;
            user.screenCanvas.renderMode = mDefaultRenderMode;
            if(isLocal) {
                mRtcEngine.setLocalViewConfig(user.screenCanvas, DingRtcVideoTrackScreen);
            } else {
                mRtcEngine.setRemoteViewConfig(user.screenCanvas,userId,DingRtcVideoTrackScreen);
            }
            // addView必须放在setLocalViewConfig/setRemoteViewConfig之后
            screenView.setZOrderMediaOverlay(true);
            user.renderSmallGroup.addView(screenView);
            user.renderSmallGroup.setVisibility(View.VISIBLE);
        }
    }

    private void removeScreenCanvas(String userId) {
        boolean isLocal = mUserSelf.userId.equals(userId);
        RoomUser user;
        if(isLocal) {
            user = mUserSelf;
        } else {
            user = mRemoteUserMap.get(userId);
        }
        if(user != null && user.screenCanvas != null) {
            if(user.screenCanvas.view != null) {
                user.renderSmallGroup.removeView((SurfaceView)user.screenCanvas.view);
                user.renderSmallGroup.setVisibility(View.GONE);
            }
            user.screenCanvas = null;
            if(isLocal) {
                mRtcEngine.setLocalViewConfig(null, DingRtcVideoTrackScreen);
            } else {
                mRtcEngine.setRemoteViewConfig(null,userId,DingRtcVideoTrackScreen);
            }
        }
    }

    private void previewVideo(boolean preview) {
        addVideoCanvas(mUserSelf.userId);
        int ret;
        if(preview) {
            ret = mRtcEngine.startPreview();
        } else {
            ret = mRtcEngine.stopPreview();
        }
        if(ret == 0) {
            mUserSelf.isVideoPreview = preview;
        }
    }

    private void updatePreviewUI(DrawableTextView previewView) {
        if(mUserSelf.isVideoPreview) {
            previewView.setDrawableTop(this,R.drawable.preview_on);
        } else {
            previewView.setDrawableTop(this,R.drawable.preview_off);
        }
    }

    private void pubVideo(boolean pub) {
        addVideoCanvas(mUserSelf.userId);
        if (!pub) {
            removeVideoCanvas(mUserSelf.userId);
        }
        int ret = mRtcEngine.publishLocalVideoStream(pub);
        if(ret == 0) {
            mUserSelf.isVideoStarted = pub;
        }
    }

    private void updateVideoUI(DrawableTextView pubVideoView) {
        if(mUserSelf.isVideoStarted) {
            pubVideoView.setText("停止推流");
            pubVideoView.setDrawableTop(this,R.drawable.pub_video);
        } else {
            pubVideoView.setText("开始推流");
            pubVideoView.setDrawableTop(this,R.drawable.unpub_video);
            if(mUserSelf.statsVideoView != null) {
                mUserSelf.statsVideoView.setText("");
            }
        }
    }

    private void pubScreen(boolean pub) {
        if(pub) {
            if (mScreenShareDialog == null) {
                mScreenShareDialog = new AlertDialog.Builder(this)
                        .setItems(R.array.screen_share_mode, (dialog, which) -> {
                            DingRtcEngine.DingRtcScreenShareMode mode;
                            switch (which) {
                                case 0:
                                    mode = DingRtcScreenShareNoneMode;
                                    break;
                                case 1:
                                    mode = DingRtcScreenShareOnlyVideoMode;
                                    break;
                                case 2:
                                    mode = DingRtcScreenShareOnlyAudioMode;
                                    break;
                                case 3:
                                    mode = DingRtcScreenShareAllMode;
                                    break;
                                default:
                                    mode = DingRtcScreenShareAllMode;
                                    break;
                            }
                            addScreenCanvas(mUserSelf.userId);
                            int ret = mRtcEngine.startScreenShare(mode);
                            if(ret == 0) {
                                mUserSelf.isScreenStarted = true;
                            } else {
                                showToast("屏幕共享失败，系统不支持。");
                            }
                        })
                        .create();
            }
            if (!mScreenShareDialog.isShowing()) {
                mScreenShareDialog.show();
            }
        } else {
            int ret = mRtcEngine.stopScreenShare();
            removeScreenCanvas(mUserSelf.userId);
            if(ret == 0) {
                mUserSelf.isScreenStarted = false;
            }
        }
    }

    private void updateScreenUI(DrawableTextView pubVideoView) {
        if(mUserSelf.isScreenStarted) {
            pubVideoView.setDrawableTop(this,R.drawable.pub_screen);
        } else {
            pubVideoView.setDrawableTop(this,R.drawable.unpub_screen);
        }
    }

    private void pubAudio(boolean pub) {
        int ret = mRtcEngine.publishLocalAudioStream(pub);
        if(ret == 0) {
            mUserSelf.isAudioStarted = pub;
        }
    }

    private void updateAudioUI(DrawableTextView pubAudioView) {
        if(mUserSelf.isAudioStarted) {
            pubAudioView.setText("开始推流");
            pubAudioView.setDrawableTop(RoomActivity.this,R.drawable.pub_audio);
        } else {
            pubAudioView.setText("停止推流");
            pubAudioView.setDrawableTop(RoomActivity.this,R.drawable.unpub_audio);
            if (mUserSelf.statsAudioView != null) {
                mUserSelf.statsAudioView.setText("");
            }
        }
    }

    private void enableVideo(boolean enable) {
        int ret = mRtcEngine.enableLocalVideo(enable);
        if(ret == 0) {
            mUserSelf.isVideoEnabled = enable ;
        }
    }

    private void updateEnableVideoUI(DrawableTextView enableVideoView) {
        if(mUserSelf.isVideoEnabled) {
            enableVideoView.setText("停止采集");
            enableVideoView.setDrawableTop(RoomActivity.this,R.drawable.camera_on);
        } else {
            enableVideoView.setText("开启采集");
            enableVideoView.setDrawableTop(RoomActivity.this,R.drawable.camera_off);
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
        if(mUserSelf.isAudioMuted) {
            muteAudioView.setText("Unmute音频");
        } else {
            muteAudioView.setText("Mute音频");
        }
    }

    private void snapshot() {
        if (mRtcEngine != null) {
            // local
            String path = getExternalFilesDir("snapshot").getAbsolutePath() + File.separator
                    + "snapshot_local_camera_" + System.currentTimeMillis() + ".jpg";
            mRtcEngine.snapshotVideo("", DingRtcVideoTrackCamera, path);
            path = getExternalFilesDir("snapshot").getAbsolutePath() + File.separator
                    + "snapshot_local_screen_" + System.currentTimeMillis() + ".jpg";
            mRtcEngine.snapshotVideo("", DingRtcVideoTrackScreen, path);

            // remote
            if (mRemoteUserMap.size() > 0) {
                for(String uid : mRemoteUserMap.keySet()) {
                    path = getExternalFilesDir("snapshot").getAbsolutePath() + File.separator
                            + "snapshot_" + mRemoteUserMap.get(uid).userId + "_camera_" + System.currentTimeMillis() + ".jpg";
                    mRtcEngine.snapshotVideo(uid, DingRtcVideoTrackCamera, path);
                    path = getExternalFilesDir("snapshot").getAbsolutePath() + File.separator
                            + "snapshot_" + mRemoteUserMap.get(uid).userId + "_screen_" + System.currentTimeMillis() + ".jpg";
                    mRtcEngine.snapshotVideo(uid, DingRtcVideoTrackScreen, path);
                }
            }
        }
    }

    private void muteVideo(boolean mute) {
        int ret = mRtcEngine.muteLocalCamera(mute, DingRtcVideoTrackCamera);
        if(ret == 0) {
            mUserSelf.isVideoMuted = mute;
        }
    }

    private void updateVideoMuteUI(DrawableTextView muteVideoView) {
        if(mUserSelf.isVideoMuted) {
            muteVideoView.setText("Unmute视频");
        } else {
            muteVideoView.setText("Mute视频");
        }
    }

    private void muteScreen(boolean mute) {
        int ret = mRtcEngine.muteLocalCamera(mute, DingRtcVideoTrackScreen);
        if(ret == 0) {
            mUserSelf.isScreenMuted = mute;
        }
    }

    private void updateScreenMuteUI(DrawableTextView muteScreenView) {
        if(mUserSelf.isVideoMuted) {
            muteScreenView.setText("Unmute屏幕");
        } else {
            muteScreenView.setText("Mute屏幕");
        }
    }

    private void updateStatsUI(DrawableTextView statsView) {
        if(mUserSelf.statsAudioView.getVisibility() == View.VISIBLE) {
            statsView.setText("显示统计");
            mUserSelf.statsAudioView.setVisibility(View.GONE);
            mUserSelf.statsVideoView.setVisibility(View.GONE);
            mUserSelf.statsScreenView.setVisibility(View.GONE);
            for(RoomUser user : mRemoteUserMap.values()) {
                user.statsAudioView.setVisibility(View.GONE);
                user.statsVideoView.setVisibility(View.GONE);
                user.statsScreenView.setVisibility(View.GONE);
            }
        } else {
            statsView.setText("关闭统计");
            mUserSelf.statsAudioView.setVisibility(View.VISIBLE);
            mUserSelf.statsVideoView.setVisibility(View.VISIBLE);
            mUserSelf.statsScreenView.setVisibility(View.VISIBLE);
            for(RoomUser user : mRemoteUserMap.values()) {
                user.statsAudioView.setVisibility(View.VISIBLE);
                user.statsVideoView.setVisibility(View.VISIBLE);
                user.statsScreenView.setVisibility(View.VISIBLE);
            }
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

    private final Map<String, VideoSaver> mVideoSaverMap = new HashMap<>();

    void saveVideoData(String channel, String uid, int track, int pos, DingRtcEngine.DingRtcVideoSample frame) {
        String tag = channel + uid + track + pos;
        String dir = getExternalFilesDir("dump").getAbsolutePath();
        VideoSaver saver = mVideoSaverMap.get(tag);
        if (saver == null) {
            saver = new VideoSaver(dir,channel, uid, track, pos);
            mVideoSaverMap.put(tag, saver);
        }
        saver.save(frame);
    }

    void closeVideoDataFiles() {

        for(VideoSaver saver : mVideoSaverMap.values()) {
            if (saver != null) {
                saver.close();
            }
        }
        mVideoSaverMap.clear();
    }

    static class VideoSaver {
        private final String channel;
        private final String uid;
        private final int track;
        private final int pos;
        private int width = 0;
        private int height = 0;

        private final String dir;
        private FileOutputStream mFile;


        VideoSaver(String dir, String channel, String uid, int track, int pos) {
            this.dir = dir;
            this.channel = channel;
            this.uid = uid;
            this.track = track;
            this.pos = pos;

        }

        void close() {
            if (mFile != null) {
                try {
                    mFile.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        String genFileName(int width, int height, int rotation) {
            StringBuffer sb = new StringBuffer("video_");
            sb.append(channel).append("_").append(uid).append("_");
            if (track == DingRtcEngine.DingRtcVideoSourceType.DingRtcSdkVideoSourceCameraType.ordinal()) {
                sb.append("camera").append("_");
            } else if (track == DingRtcEngine.DingRtcVideoSourceType.DingRtcSdkVideoSourceScreenShareType.ordinal()) {
                sb.append("screen").append("_");
            }
            sb.append(width).append("x").append(height).append("_");
            if (pos == DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPostCapture) {
                sb.append("Capture").append("_");
            } else if (pos == DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPreEncoder) {
                sb.append("Encoder").append("_");
            } else if (pos == DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPreRender) {
                sb.append("Render").append("_");
            }
            sb.append(rotation).append("_");
            sb.append(System.currentTimeMillis()).append(".yuv");
            return sb.toString();
        }

        void save(DingRtcEngine.DingRtcVideoSample frame) {
            if (frame != null) {
                if (width != frame.width || height != frame.height) {
                    if (mFile != null) {
                        try {
                            mFile.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                    mFile = null;
                    width = frame.width;
                    height = frame.height;
                }
                if (mFile == null) {
                    try {
                        mFile = new FileOutputStream(new File(dir, genFileName(frame.width, frame.height, frame.rotate)));
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    }
                }
                try {
                    if (mFile != null && frame.data.length > 0) {
                        // convert to I420
                        int half_width = (frame.width) / 2;
                        int half_height = (frame.height) / 2;
                        for (int i = 0; i < frame.height; i++) {
                            mFile.write(frame.data, frame.offsetY + i * frame.strideY, frame.width);
                        }
                        for (int i = 0; i < half_height; i++) {
                            mFile.write(frame.data, frame.offsetU + i * frame.strideU, half_width);
                        }
                        for (int i = 0; i < half_height; i++) {
                            mFile.write(frame.data, frame.offsetV + i * frame.strideV, half_width);
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private String networkQualityToStr(DingRtcEngine.DingRtcNetworkQuality quality) {
        String qualityStr;
        if (quality == DingRtcEngine.DingRtcNetworkQuality.DingRtcNetworkQualityDisconnect) {
            qualityStr = "Disconnect";
        } else if (quality == DingRtcEngine.DingRtcNetworkQuality.DingRtcNetworkQualityGood) {
            qualityStr = "Good";
        } else if (quality == DingRtcEngine.DingRtcNetworkQuality.DingRtcNetworkQualityPoor) {
            qualityStr = "Poor";
        } else {
            qualityStr = "UnKnown";
        }
        return qualityStr;
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

    private void setVirtualBackground() {
        if ("0".equals(mVirtualBackground)) {
            mRtcEngine.enableVirtualBackground(false, null);
        } else if ("1".equals(mVirtualBackground)) {
            DingRtcEngine.DingRtcVirtualBackgroundOptions options = new DingRtcEngine.DingRtcVirtualBackgroundOptions();
            options.mode = DingRtcEngine.DingRtcEngineVirtualBackgroundMode.DingRtcEngineVirtualBackgroundBlur;
            mRtcEngine.enableVirtualBackground(true, options);
        }
    }

    private void startAnnotation(DingRtcWhiteBoardTypes.DingRtcAnnoID annoID) {
        AnnotationInfo annoInfo = getAnnotation(annoID, true);
        if (annoInfo == null) {
            return;
        }
        if (annoID.type != DingRtcWhiteBoardTypes.DingRtcAnnoType.CAMERA) {
            return;
        }
        annoInfo.isStarted = true;
        RoomUser user;
        if (annoID.uid.equals(mUserSelf.userId)) {
            user = mUserSelf;
        } else {
            user = mRemoteUserMap.get(annoID.uid);
        }
        if (user != null) {
            DingRtcWhiteboardView view = annoInfo.getWhiteboardView();
            if (view == null) {
                view = new DingRtcWhiteboardView(this);
                user.renderBigGroup.addView(view);
                Log.i(TAG, "startAnnotation create view " + annoID.uid);
            } else {
                ViewParent parentView = view.getParent();
                if (parentView != user.renderBigGroup) {
                    annoInfo.close();
                    if (parentView instanceof ViewGroup) {
                        ((ViewGroup) parentView).removeView(view);
                    }
                    user.renderBigGroup.addView(view);
                }
            }

            annoInfo.open(view, getWbScalingMode(mDefaultRenderMode), user.mirrorMode);
        }
    }

    private void stopAnnotation(DingRtcWhiteBoardTypes.DingRtcAnnoID annoID, boolean needStop) {
        AnnotationInfo annoInfo = getAnnotation(annoID, true);
        if (annoInfo == null) {
            return;
        }
        if (needStop) {
            annoInfo.isStarted = false;
        }
        RoomUser user;
        if (annoID.uid.equals(mUserSelf.userId)) {
            user = mUserSelf;
        } else {
            user = mRemoteUserMap.get(annoID.uid);
        }
        if (user != null) {
            annoInfo.close();
            DingRtcWhiteboardView view = annoInfo.getWhiteboardView();
            if (view != null) {
                user.renderBigGroup.removeView(view);
            }
        }
    }

    private AnnotationInfo getAnnotation(DingRtcWhiteBoardTypes.DingRtcAnnoID annoID, boolean createIfNotExist) {
        for (AnnotationInfo info : mAnnotationList) {
            if (info.userId.equals(annoID.uid) && info.type == annoID.type) {
                return info;
            }
        }
        if (createIfNotExist) {
            AnnotationInfo info = new AnnotationInfo();
            DingRtcEngineWhiteboardManager manager = mRtcEngine.getWhiteBoardManager();
            if (manager == null) {
                return null;
            }
            info.init(manager, annoID.type, annoID.uid);
            info.createAnnotation();
            mAnnotationList.add(info);
            return info;
        }
        return null;
    }

    private DingRtcWhiteBoardTypes.DingRtcWBScalingMode getWbScalingMode(DingRtcEngine.DingRtcRenderMode mode) {
        if (mode == DingRtcRenderModeAuto) {
            return DingRtcWhiteBoardTypes.DingRtcWBScalingMode.ContentAuto;
        } else if (mode == DingRtcRenderModeCrop) {
            return DingRtcWhiteBoardTypes.DingRtcWBScalingMode.CropFill;
        } else if (mode == DingRtcRenderModeFill) {
            return DingRtcWhiteBoardTypes.DingRtcWBScalingMode.FIT;
        }
        return DingRtcWhiteBoardTypes.DingRtcWBScalingMode.FIT;
    }

    private void onClickAnnotation(RoomUser user, DingRtcWhiteBoardTypes.DingRtcAnnoType type) {
        DingRtcWhiteBoardTypes.DingRtcAnnoID id = new DingRtcWhiteBoardTypes.DingRtcAnnoID(type, user.userId);
        AnnotationInfo annoInfo = getAnnotation(id, true);
        if (annoInfo == null) {
            return;
        }

        String toolbarUid = mAnnotationToolBar.getCurrentUid();

        if (user.userId.equals(toolbarUid)) {
            if (annoInfo.isStarted) {
                stopAnnotation(id, true);
                mAnnotationToolBar.dismiss();
                Drawable srcDrawable = DrawableCompat.wrap(user.annotationView.getDrawable());
                DrawableCompat.setTint(srcDrawable, Color.GRAY);
                user.annotationView.setBackground(srcDrawable);
            } else {
                startAnnotation(id);
                mAnnotationToolBar.show(user.renderBigGroup, user.annotationView);
                mAnnotationToolBar.setCurrentUid(user.userId);
                Drawable srcDrawable = DrawableCompat.wrap(user.annotationView.getDrawable());
                DrawableCompat.setTint(srcDrawable, Color.parseColor("#0899F9"));
                user.annotationView.setBackground(srcDrawable);
            }
        } else {
            startAnnotation(id);
            mAnnotationToolBar.show(user.renderBigGroup, user.annotationView);
            Drawable srcDrawable = DrawableCompat.wrap(user.annotationView.getDrawable());
            DrawableCompat.setTint(srcDrawable, Color.parseColor("#0899F9"));
            user.annotationView.setBackground(srcDrawable);
            mAnnotationToolBar.setCurrentUid(user.userId);

            if (!TextUtils.isEmpty(toolbarUid)) {
                RoomUser lastUser = mRemoteUserMap.get(toolbarUid);
                if (lastUser == null) {
                    if (toolbarUid.equals(mUserSelf.userId)) {
                        lastUser = mUserSelf;
                    }
                }
                if (lastUser == null) {
                    return;
                }

                Drawable lastSrcDrawable = DrawableCompat.wrap(lastUser.annotationView.getDrawable());
                DrawableCompat.setTint(lastSrcDrawable, Color.GRAY);

                DingRtcWhiteBoardTypes.DingRtcAnnoID lastId = new DingRtcWhiteBoardTypes.DingRtcAnnoID(type, toolbarUid);
                stopAnnotation(lastId, true);
            }
        }
    }

    @Override
    public void onToolTypeChanged(DingRtcWhiteBoardTypes.DingRtcWBToolType toolType) {
        for (AnnotationInfo info : mAnnotationList) {
            info.setToolType(toolType);
        }
    }

    @Override
    public void onClickSnapshot(String uid) {
        for (AnnotationInfo info : mAnnotationList) {
            if (uid != null && uid.equals(info.userId)) {
                info.snapshot(mAnnoSnapshotDir);
                break;
            }
        }
    }

    public void onClickClear(String uid) {
        for (AnnotationInfo info : mAnnotationList) {
            if (uid != null && uid.equals(info.userId)) {
                info.clear();
                break;
            }
        }
    }
}