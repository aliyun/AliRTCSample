package com.ding.rtc.apiexample.advanced.externalvideosource;

import static com.ding.rtc.DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeAllDisable;
import static com.ding.rtc.DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeFill;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackBoth;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackNo;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackScreen;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ExternalVideoSourceActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private String mChannelId;
    private String mUserId;
    private boolean isPushExternalVideo = false;

    private FrameLayout mLocalViewHolder;
    private FrameLayout mRemoteViewHolder;

    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;
    private DingRtcEngine.DingRtcVirtualBackgroundOptions mVirtualBackgroundOptions = new DingRtcEngine.DingRtcVirtualBackgroundOptions();

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, ExternalVideoSourceActivity.class);
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
        setContentView(R.layout.activity_external_video_source);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.external_video_source);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder = findViewById(R.id.remote_view);

        initDingEngine();
        joinChannel();
    }

    @Override
    public void onClick(View v) {
        Button btn = (Button) v;
        if (!isPushExternalVideo) {
            btn.setText(R.string.external_video_source_stop);
            SurfaceView localView = mRtcEngine.createRenderSurfaceView(getApplicationContext());
            mLocalVideoCanvas.view = localView;
            mLocalVideoCanvas.renderMode = DingRtcRenderModeFill;
            mLocalVideoCanvas.mirrorMode = DingRtcRenderMirrorModeAllDisable;
            mRemoteVideoCanvas.renderMode = DingRtcRenderModeFill;
            mLocalViewHolder.addView(localView);
            mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackCamera);
            // Important : enable external video source ,
            // This means that video frames will be passed in through the pushExternalVideoFrame interface instead of being captured by the camera..
            mRtcEngine.setExternalVideoSource(true, false, DingRtcVideoTrackCamera, DingRtcRenderModeFill);
            // Important : "publishLocalVideoStream(true)" : It means pushing video data to the media server.
            mRtcEngine.publishLocalVideoStream(true);
            mRtcEngine.startPreview();
            mExecutor.execute(new Runnable() {
                @Override
                public void run() {
                    // Prepare external video source file
                    String fileName = "dingrtc_320x240_i420.yuv";
                    File file = new File(getFilesDir().getAbsolutePath(), fileName);
                    String filePath = file.getPath();
                    if (!file.exists() || !file.isFile()) {
                        copyBigDataToSD(fileName, filePath, getApplicationContext());
                    }
                    startExteralVideoSource(filePath);
                }
            });

        } else {
            btn.setText(R.string.external_video_source_start);
            stopExterVideoSource();
            mRtcEngine.setLocalViewConfig(null, DingRtcVideoTrackCamera);
            // Important : enable external video source ,
            // This means that video frames will be passed in through the pushExternalVideoFrame interface instead of being captured by the camera..
            mRtcEngine.setExternalVideoSource(false, false, DingRtcVideoTrackCamera, DingRtcRenderModeFill);
            // Important : "publishLocalVideoStream(true)" : It means pushing video data to the media server.
            mRtcEngine.publishLocalVideoStream(false);
        }
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.publishLocalAudioStream(true);
            mRtcEngine.publishLocalVideoStream(false); // default : not publish video stream
            mRtcEngine.subscribeAllRemoteAudioStreams(true);

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

                                    SurfaceView localView = mRtcEngine.createRenderSurfaceView(ExternalVideoSourceActivity.this);
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

    private void stopExterVideoSource() {
        isPushExternalVideo = false;
    }

    private void startExteralVideoSource(String yuvPath) {
        if (TextUtils.isEmpty(yuvPath)) {
            // input you file path
            return;
        }
        File yuvDataFile = new File(yuvPath);
        if (!yuvDataFile.exists()) {
            //  file not exist!
            return;
        }
        isPushExternalVideo = true;
        int rotation = 0;
        int width = 320;
        int height = 240;
        int fps = 5;
        //start push yuv
        new Thread() {
            @Override
            public void run() {
                File yuvDataFile = new File(yuvPath);
                RandomAccessFile raf = null;
                try {
                    raf = new RandomAccessFile(yuvDataFile, "r");
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                    return;
                }
                try {
                    byte[] buffer = new byte[width * height * 3 / 2];
                    while (isPushExternalVideo) {
                        long start = System.currentTimeMillis();
                        int len = raf.read(buffer);
                        if (len == -1) {
                            raf.seek(0);
                        }
                        DingRtcEngine.DingRtcRawDataFrame rawDataFrame = new DingRtcEngine.DingRtcRawDataFrame();
                        rawDataFrame.format = DingRtcEngine.DingRtcVideoFormat.DingRtcVideoFormatI420;// 支持I420
                        rawDataFrame.frame = buffer;
                        rawDataFrame.width = width;
                        rawDataFrame.height = height;
                        rawDataFrame.lineSize[0] = width;
                        rawDataFrame.lineSize[1] = width/2;
                        rawDataFrame.lineSize[2] = width/2;
                        rawDataFrame.lineSize[3] = 0;
                        rawDataFrame.rotation = rotation;
                        rawDataFrame.videoFrameLength = buffer.length;
                        rawDataFrame.timestamp = System.nanoTime() / 1000;
                        if (mRtcEngine != null) {
                            mRtcEngine.pushExternalVideoFrame(rawDataFrame, DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera);
                        }
                        // Control the frame rate of sending by the time of sleep.
                        long use = System.currentTimeMillis() - start;
                        long delay = 1000 / fps - use;
                        if (delay < 0) {
                            delay = 0;
                        }
                        Thread.sleep(delay);
                    }
                } catch (IOException | InterruptedException ex) {
                    ex.printStackTrace();
                } finally {
                    try {
                        raf.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }.start();
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