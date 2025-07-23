package com.ding.rtc.apiexample.advanced.rawvideodata;

import static com.ding.rtc.DingRtcEngine.DingRtcRenderMirrorMode.DingRtcRenderMirrorModeAllDisable;
import static com.ding.rtc.DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeFill;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPostCapture;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPreEncoder;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoObservePosition.DingRtcPositionPreRender;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackBoth;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackNo;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackScreen;

import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

import java.io.File;
import java.io.OutputStream;

public class RawVideoDataActivity extends AppCompatActivity {

    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    private String mChannelId;
    private String mUserId;
    private int mPosition = DingRtcPositionPostCapture;
    private boolean needSnapShot = false;

    private FrameLayout mLocalViewHolder;
    private FrameLayout mRemoteViewHolder;

    private DingRtcEngine.DingRtcVideoCanvas mLocalVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private DingRtcEngine.DingRtcVideoCanvas mRemoteVideoCanvas = new DingRtcEngine.DingRtcVideoCanvas();
    private boolean hasRemoteVideo = false;

    public static void launch(Context context, String appId, String channelId, String userId, String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, RawVideoDataActivity.class);
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
        setContentView(R.layout.activity_raw_video_data);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.raw_video_data);
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }

        mLocalViewHolder = findViewById(R.id.preview_view);
        mRemoteViewHolder = findViewById(R.id.remote_view);

        initDingEngine();
        joinChannel();
    }

    @Override
    public void onPointerCaptureChanged(boolean hasCapture) {
        super.onPointerCaptureChanged(hasCapture);
    }

    public void onClick(View v) {
        needSnapShot = true;
    }

    public void onPositionClick(View v) {
        if (v.getId() == R.id.radioPostCapture) {
            mPosition = DingRtcPositionPostCapture;
        } else if (v.getId() == R.id.radioPreRender) {
            mPosition = DingRtcPositionPreRender;
        } else if (v.getId() == R.id.radioPreEncoder) {
            mPosition = DingRtcPositionPreEncoder;
        }
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.publishLocalAudioStream(false);
            mRtcEngine.subscribeAllRemoteAudioStreams(false);
            SurfaceView localView = mRtcEngine.createRenderSurfaceView(getApplicationContext());
            mLocalVideoCanvas.view = localView;
            mLocalVideoCanvas.renderMode = DingRtcRenderModeFill;
            mLocalVideoCanvas.mirrorMode = DingRtcRenderMirrorModeAllDisable;
            mRemoteVideoCanvas.renderMode = DingRtcRenderModeFill;
            mLocalViewHolder.addView(localView);
            mRtcEngine.setLocalViewConfig(mLocalVideoCanvas, DingRtcVideoTrackCamera);
            mRtcEngine.publishLocalVideoStream(true);
            mRtcEngine.subscribeAllRemoteVideoStreams(true);
            mRtcEngine.startPreview();
            mRtcEngine.enableVideoSampleObserver(true, mPosition);

            mRtcEngine.registerVideoSampleObserver(new DingRtcEngine.DingRtcVideoObserver() {
                @Override
                public boolean onLocalVideoSample(DingRtcEngine.DingRtcVideoSourceType sourceType, DingRtcEngine.DingRtcVideoSample videoSample) {
                    saveVideoData(videoSample);
                    return false;
                }

                @Override
                public boolean onRemoteVideoSample(String callId, DingRtcEngine.DingRtcVideoSourceType sourceType, DingRtcEngine.DingRtcVideoSample videoSample) {
                    saveVideoData(videoSample);
                    return false;
                }

                @Override
                public boolean onPreEncodeVideoFrame(DingRtcEngine.DingRtcVideoSourceType sourceType, DingRtcEngine.DingRtcVideoSample videoSample) {
                    saveVideoData(videoSample);
                    return false;
                }

                @Override
                public DingRtcEngine.DingRtcVideoFormat onGetVideoFormatPreference() {
                    return DingRtcEngine.DingRtcVideoFormat.DingRtcVideoFormatI420;
                }
            });

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

                                    SurfaceView localView = mRtcEngine.createRenderSurfaceView(RawVideoDataActivity.this);
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

    private void saveVideoData(DingRtcEngine.DingRtcVideoSample videoSample) {
        if (needSnapShot) {
            needSnapShot = false;
            Bitmap bitmap = convertI420ToBitmap(videoSample);
            saveBitmap(bitmap);
        }
    }

    public void saveBitmap(Bitmap bm) {
        long currentTimeMillis = System.currentTimeMillis();

        String imageFileName = "DingRTC_" + currentTimeMillis + ".jpg";
        String imageFilePath;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            imageFilePath = Environment.DIRECTORY_PICTURES + File.separator + "DingRTC" + File.separator;
        } else {
            imageFilePath = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).getAbsolutePath()
                    + File.separator + "DingRTC" + File.separator;
        }

        OutputStream os;
        ContentValues cvs = new ContentValues();
        Uri uri;
        cvs.put(MediaStore.Images.ImageColumns.DATE_ADDED, currentTimeMillis);
        cvs.put(MediaStore.Images.ImageColumns.DISPLAY_NAME, imageFileName);
        cvs.put(MediaStore.Images.ImageColumns.MIME_TYPE, "image/jpg");
        cvs.put(MediaStore.Images.ImageColumns.WIDTH, bm.getWidth());
        cvs.put(MediaStore.Images.ImageColumns.HEIGHT, bm.getHeight());
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                cvs.put(MediaStore.Images.ImageColumns.RELATIVE_PATH, imageFilePath);
            } else {
                File imageFileDirectory = new File(imageFilePath);
                if (!imageFileDirectory.exists()) {
                    boolean mkdir = imageFileDirectory.mkdirs();
                    if (!mkdir) {
                        return;
                    }
                }
                cvs.put(MediaStore.Images.ImageColumns.DATA, imageFilePath + imageFileName);
                cvs.put(MediaStore.Images.ImageColumns.TITLE, imageFileName);
            }

            uri = getContentResolver().insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, cvs);
            os = getContentResolver().openOutputStream(uri);

            bm.compress(Bitmap.CompressFormat.PNG, 80, os);
            os.flush();
            os.close();

            cvs.clear();
            cvs.put(MediaStore.Images.ImageColumns.SIZE, new File(imageFilePath).length());
            getContentResolver().update(uri, cvs, null, null);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static Bitmap convertI420ToBitmap(DingRtcEngine.DingRtcVideoSample videoSample) {
        int width = videoSample.width;
        int height = videoSample.height;
        // create Bitmap
        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        int[] pixels = new int[width * height];

        // visit every pixel
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                // get y
                int y = videoSample.data[videoSample.offsetY + i * videoSample.strideY + j] & 0xFF;

                // get u and v
                int u = videoSample.data[videoSample.offsetU + (i / 2) * videoSample.strideU + j / 2] & 0xFF;
                int v = videoSample.data[videoSample.offsetV + (i / 2) * videoSample.strideV + j / 2] & 0xFF;

                // convert YUV to RGB
                int r = (int) (y + 1.402 * (v - 128));
                int g = (int) (y - 0.344 * (u - 128) - 0.714 * (v - 128));
                int b = (int) (y + 1.772 * (u - 128));

                // limit 0~255
                r = Math.min(255, Math.max(0, r));
                g = Math.min(255, Math.max(0, g));
                b = Math.min(255, Math.max(0, b));

                // 合并为 ARGB（A=255）
                pixels[i * width + j] = 0xFF000000 | (r << 16) | (g << 8) | b;
            }
        }

        // save to Bitmap
        bitmap.setPixels(pixels, 0, width, 0, 0, width, height);
        return bitmap;
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