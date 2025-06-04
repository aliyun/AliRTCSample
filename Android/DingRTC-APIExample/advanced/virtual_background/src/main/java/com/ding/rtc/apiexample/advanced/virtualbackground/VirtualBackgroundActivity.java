package com.ding.rtc.apiexample.advanced.virtualbackground;

import static com.ding.rtc.DingRtcEngine.DingRtcEngineVirtualBackgroundMode.DingRtcEngineVirtualBackgroundBlur;
import static com.ding.rtc.DingRtcEngine.DingRtcEngineVirtualBackgroundMode.DingRtcEngineVirtualBackgroundReplace;
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

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class VirtualBackgroundActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private String mChannelId;
    private String mUserId;

    private FrameLayout mLocalViewHolder;
    private FrameLayout mRemoteViewHolder;

    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;
    private DingRtcEngine.DingRtcVirtualBackgroundOptions mVirtualBackgroundOptions = new DingRtcEngine.DingRtcVirtualBackgroundOptions();

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, VirtualBackgroundActivity.class);
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
        setContentView(R.layout.activity_virtual_background);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.virtual_background);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder = findViewById(R.id.remote_view);

        initDingEngine();


        mExecutor.execute(new Runnable() {
            @Override
            public void run() {
                // Prepare background pictures
                String fileName = "virtual_bg.png";
                File file = new File(getFilesDir().getAbsolutePath(), fileName);
                String filePath = file.getPath();
                if (!file.exists() || !file.isFile()) {
                    copyBigDataToSD(fileName, filePath, getApplicationContext());
                }
                mVirtualBackgroundOptions.bgFilePath = filePath;
                mVirtualBackgroundOptions.mode = DingRtcEngineVirtualBackgroundReplace;
                mRtcEngine.enableVirtualBackground(true,mVirtualBackgroundOptions);
            }
        });

        joinChannel();
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.radioOff) {
            mRtcEngine.enableVirtualBackground(false, mVirtualBackgroundOptions);
        } else if (v.getId() == R.id.radioBlur) {
            mVirtualBackgroundOptions.mode = DingRtcEngineVirtualBackgroundBlur;
            mRtcEngine.enableVirtualBackground(true, mVirtualBackgroundOptions);
        } else if (v.getId() == R.id.radioReplace) {
            mVirtualBackgroundOptions.mode = DingRtcEngineVirtualBackgroundReplace;
            mRtcEngine.enableVirtualBackground(true, mVirtualBackgroundOptions);
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
                                    mRtcEngine.setRemoteViewConfig(mRemoteVideoCanvas, uid, DingRtcVideoTrackCamera);

                                    mRtcEngine.subscribeRemoteVideoStream(uid, DingRtcVideoTrackCamera, false);
                                }
                            } else {
                                if (videoTrack == DingRtcVideoTrackCamera || videoTrack == DingRtcVideoTrackBoth) {
                                    hasRemoteVideo = true;

                                    SurfaceView localView = mRtcEngine.createRenderSurfaceView(VirtualBackgroundActivity.this);
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