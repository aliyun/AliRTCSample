package com.ding.rtc.apiexample.advanced.whiteboard;

import static com.ding.rtc.api.DingRtcWhiteBoardTypes.DingRtcWBToolType.ERASER;
import static com.ding.rtc.api.DingRtcWhiteBoardTypes.DingRtcWBToolType.LASER;
import static com.ding.rtc.api.DingRtcWhiteBoardTypes.DingRtcWBToolType.PATH;
import static com.ding.rtc.api.DingRtcWhiteBoardTypes.DingRtcWBToolType.SELECT;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;
import android.widget.ImageButton;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;

import com.ding.rtc.DingRtcEngineWhiteboard;
import com.ding.rtc.DingRtcEngineWhiteboardEventListener;
import com.ding.rtc.DingRtcWhiteboardView;
import com.ding.rtc.api.DingRtcWhiteBoardTypes;

public class WhiteboardActivity extends AppCompatActivity {

    public static final String TAG = WhiteboardActivity.class.getSimpleName();
    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";
    private static final String INTENT_USER_ID = "user_id";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";

    private DingRtcEngine mRtcEngine;
    DingRtcEngineWhiteboard mWhiteboard;
    DingRtcWhiteboardView mWbView;
    RadioButton mSelectButton;
    RadioButton mDrawButton;
    RadioButton mEraseButton;
    RadioButton mLaserButton;
    ImageButton mFirstPageButton;
    ImageButton mPrevPageButton;
    ImageButton mNextPageButton;
    ImageButton mLastPageButton;
    TextView mPageInfoTextView;

    private String mChannelId;
    private String mUserId;

    private Handler mHandler;

    public static void launch(Context context, String appId, String channelId, String userId,
                              String userName, String token) {
        Intent intent = new Intent();
        intent.setClass(context, WhiteboardActivity.class);
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
        setContentView(R.layout.activity_whiteboard);
        mWbView = findViewById(R.id.whiteboard_view);
        mSelectButton = findViewById(R.id.buttonSelect);
        mDrawButton = findViewById(R.id.buttonDraw);
        mEraseButton = findViewById(R.id.buttonErase);
        mLaserButton = findViewById(R.id.buttonLaser);
        mFirstPageButton = findViewById(R.id.buttonFirstPage);
        mPrevPageButton = findViewById(R.id.buttonPreviousPage);
        mNextPageButton = findViewById(R.id.buttonNextPage);
        mLastPageButton = findViewById(R.id.buttonLastPage);
        mPageInfoTextView = findViewById(R.id.textPageInfo);
        mHandler = new Handler(Looper.getMainLooper());
        initDingEngine();
        joinChannel();
    }

    public void onTopButtonClick(View v) {
        if (mWhiteboard == null) {
            Toast.makeText(WhiteboardActivity.this, "whiteboard is not available!", Toast.LENGTH_LONG).show();
            return;
        }
        int id = v.getId();
        if (id == R.id.buttonSelect) {
            mWhiteboard.setToolType(SELECT);
        } else if (id == R.id.buttonDraw) {
            mWhiteboard.setToolType(PATH);
        } else if (id == R.id.buttonLaser) {
            mWhiteboard.setToolType(LASER);
        } else if (id == R.id.buttonErase) {
            mWhiteboard.setToolType(ERASER);
        }
    }

    public void onBottomButtonClick(View v) {
        if (mWhiteboard == null) {
            Toast.makeText(WhiteboardActivity.this, "whiteboard is not available!", Toast.LENGTH_LONG).show();
            return;
        }
        int id = v.getId();
        if (id == R.id.buttonFirstPage) {
            mWhiteboard.gotoPage(1);
        } else if (id == R.id.buttonPreviousPage) {
            mWhiteboard.prevPage();
        } else if (id == R.id.buttonNextPage) {
            mWhiteboard.nextPage();
        } else if (id == R.id.buttonLastPage) {
            mWhiteboard.gotoPage(mWhiteboard.getTotalNumberOfPages());
        } else if (id == R.id.buttonAddPage) {
            mWhiteboard.addPage(true);
        } else if (id == R.id.buttonDeletePage) {
            mWhiteboard.removePage(mWhiteboard.getCurrentPageNumber(), true);
        }
    }

    public void initDingEngine() {
        if (mRtcEngine == null) {
            mRtcEngine = DingRtcEngine.create(getApplicationContext(), "");
            mRtcEngine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
                @Override
                public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {
                    super.onJoinChannelResult(result, channel, userId, elapsed);

                    if (result == 0) {
                        mHandler.postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                initWhiteboard();
                            }
                        }, 1000);
                    }
                }
            });
        }
    }

    private void joinChannel() {
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
            Toast.makeText(WhiteboardActivity.this, "rtc join failed !", Toast.LENGTH_LONG).show();
        }
    }

    private void initWhiteboard() {
        mWhiteboard = mRtcEngine.getWhiteBoardManager().createWhiteboard("xxxx", new DingRtcWhiteBoardTypes.DingRtcWBConfig(720, 1080, DingRtcWhiteBoardTypes.DingRtcWBMode.BASIC));
        mWhiteboard.setEventListener(new DingRtcEngineWhiteboardEventListener() {

            @Override
            public void onPageNumberChanged(int curPage, int totalPages) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (mPageInfoTextView != null) {
                            mPageInfoTextView.setText(curPage + "/" + totalPages);
                        }
                    }
                });
            }
        });
        mWhiteboard.open(mWbView);
        mWhiteboard.setToolType(DingRtcWhiteBoardTypes.DingRtcWBToolType.PATH);
        mWhiteboard.setLineWidth(5);
        mWhiteboard.setColor(new DingRtcWhiteBoardTypes.DingRtcWBColor(1f, 0f, 0f, 1f));
    }

    @Override
    public void onBackPressed() {
        if (mWhiteboard != null) {
            mWhiteboard.setEventListener(null);
            mWhiteboard.close();
            mWhiteboard.leave();
            mWhiteboard.stop();
            mWhiteboard = null;
        }
        if (mRtcEngine != null) {
            mRtcEngine.setRtcEngineEventListener(null);
            mRtcEngine.leaveChannel();
            mRtcEngine.destroy();
            mRtcEngine = null;
        }
        super.onBackPressed();
    }
}