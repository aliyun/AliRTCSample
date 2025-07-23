package com.ding.rtc.apiexample.advanced.picinpic;

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
import android.widget.CheckBox;
import android.widget.FrameLayout;
import android.widget.RadioGroup;

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

public class PicInPicActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";
    private static final String INTENT_IS_PUBLISHER = "is_publisher";

    private static final String[] FILTER_ARRAY = {"", "jiaopian_res.png", "gangfeng_res.png", "hanfeng_res.png",
            "heibai_res.png", "naixia_res.png", "qingxin_res.png", "rixi_res.png",
            "senxi_res.png", "yuanqi_res.png", "ziran_res.png", "landiao_res.png"};
    private DingRtcEngine mRtcEngine;
    private ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private String mChannelId;
    private String mUserId;
    private boolean isPublisher = true;

    private FrameLayout mLocalViewHolder;
    private FrameLayout mRemoteViewHolder;
    private CheckBox mWhiteCheckBox;
    private CheckBox mSharpCheckBox;
    private CheckBox mRosyCheckBox;
    private CheckBox mBuffCheckBox;
    private ViewGroup mLocalViewGroup;
    private ViewGroup mRemoteViewGroup;
    private ViewGroup mBeautyViewGroup;
    private RadioGroup[] mFilterGroup = new RadioGroup[3];

    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;
    private DingRtcEngine.DingRtcBeautyFaceOptions mBeautyFaceOptions = new DingRtcEngine.DingRtcBeautyFaceOptions();
    private RadioGroup.OnCheckedChangeListener mCheckedChangeListener = new RadioGroup.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(RadioGroup radioGroup, int index) {
            String tag = (String) radioGroup.getTag();
            int tagValue = 0;
            try {
                tagValue = Integer.parseInt(tag);
            } catch (Throwable t) {
                t.printStackTrace();
            }
            for (int i = 0; i < 3; i++) {
                if (i != tagValue) {
                    // only clearCheck will cause a loop callback of onCheckedChanged.
                    mFilterGroup[i].setOnCheckedChangeListener(null);
                    mFilterGroup[i].clearCheck();
                    mFilterGroup[i].setOnCheckedChangeListener(mCheckedChangeListener);
                }
            }
        }
    };

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
            getSupportActionBar().setTitle(R.string.beauty);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }
        isPublisher = getIntent().getBooleanExtra(INTENT_IS_PUBLISHER, true);
        mLocalViewGroup = findViewById(R.id.local_view_group);
        mRemoteViewGroup = findViewById(R.id.remote_view_group);
        mBeautyViewGroup = findViewById(R.id.beauty_view_group);
        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder = findViewById(R.id.remote_view);
        mWhiteCheckBox = findViewById(R.id.beautyWhitening);
        mBuffCheckBox = findViewById(R.id.beautyBuffing);
        mRosyCheckBox = findViewById(R.id.beautyRosy);
        mSharpCheckBox = findViewById(R.id.beautySharpening);
        mFilterGroup[0] = findViewById(R.id.filterGroup1);
        mFilterGroup[1] = findViewById(R.id.filterGroup2);
        mFilterGroup[2] = findViewById(R.id.filterGroup3);
        for (int i = 0; i < 3; i++) {
            mFilterGroup[i].setOnCheckedChangeListener(mCheckedChangeListener);
        }

        // n order to reserve a larger View for observing various functions of beauty and filters,
        // the publisher end the subscriber are split into two separate terminals.
        if (isPublisher) {
            mRemoteViewGroup.setVisibility(View.GONE);
        } else {
            mLocalViewGroup.setVisibility(View.GONE);
            mBeautyViewGroup.setVisibility(View.GONE);
        }

        initDingEngine();
        joinChannel();
    }

    public void onFilterClick(View v) {
        String tag = (String) v.getTag();
        int tagValue = 0;
        try {
            tagValue = Integer.parseInt(tag);
        } catch (Throwable t) {
            t.printStackTrace();
        }
        if (tagValue == 0 && tagValue < 12) {
            mBeautyFaceOptions.enableFilter = false;
        } else {
            final int filterIndex = tagValue;
            mExecutor.execute(new Runnable() {
                @Override
                public void run() {
                    // Prepare filter resource
                    String fileName = FILTER_ARRAY[filterIndex];
                    File file = new File(getFilesDir().getAbsolutePath(), fileName);
                    String filePath = file.getPath();
                    if (!file.exists() || !file.isFile()) {
                        copyBigDataToSD(fileName, filePath, getApplicationContext());
                    }
                    mBeautyFaceOptions.filterPath = filePath;
                    mBeautyFaceOptions.enableFilter = true;
                    mRtcEngine.enableBeautyFace(true, mBeautyFaceOptions);
                }
            });
        }
    }

    public void onBeautyClick(View v) {
        if (v.getId() == R.id.beautyWhitening) {
            mBeautyFaceOptions.enableSkinWhitening = mWhiteCheckBox.isChecked();
        } else if (v.getId() == R.id.beautyBuffing) {
            mBeautyFaceOptions.enableSkinBuffing = mBuffCheckBox.isChecked();
        } else if (v.getId() == R.id.beautyRosy) {
            mBeautyFaceOptions.enableSkinRosy = mRosyCheckBox.isChecked();
        } else if (v.getId() == R.id.beautySharpening) {
            if (mSharpCheckBox.isChecked()) {
                mBeautyFaceOptions.skinSharpenFactor = 0.8f;
            } else {
                mBeautyFaceOptions.skinSharpenFactor = 0.0f;
            }
        }
        mRtcEngine.enableBeautyFace(true, mBeautyFaceOptions);
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

    @Override
    protected void onStop() {
        super.onStop();
        if (isFinishing()) {
            mRtcEngine.leaveChannel();
            mRtcEngine.destroy();
            mRtcEngine = null;
        }
    }

    private static void copyBigDataToSD(String fileName, String strOutFileName, Context context) {
        try {
            InputStream myInput;
            OutputStream myOutput = new FileOutputStream(strOutFileName);
            myInput = context.getAssets().open(fileName);
            byte[] buffer = new byte[1024];
            int length = myInput.read(buffer);
            while (length > 0) {
                myOutput.write(buffer, 0, length);
                length = myInput.read(buffer);
            }

            myOutput.flush();
            myInput.close();
            myOutput.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}