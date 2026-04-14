package com.ding.rtc.demo.aiagent;

import static com.ding.rtc.DingRtcEngine.DingRtcRenderMode.DingRtcRenderModeAuto;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackCamera;
import static com.ding.rtc.DingRtcEngine.DingRtcVideoTrack.DingRtcVideoTrackScreen;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.content.ContextCompat;
import androidx.core.graphics.drawable.DrawableCompat;

import com.ding.rtc.DingRtcAuthInfo;
import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.DingRtcEngineEventListener;
import com.ding.rtc.DingRtmAgentClient;
import com.ding.rtc.demo.BaseActivity;
import com.ding.rtc.demo.R;
import com.ding.rtc.demo.utils.DensityUtils;

public class AiAgentActivity extends BaseActivity implements AiAgent.AgentCallback {
    private static final String TAG = "AiAgent";
    private static final String INTENT_USER_ID = "uid";
    private static final String INTENT_TOKEN = "token";
    private static final String INTENT_USER_NAME = "user_name";
    private static final String INTENT_APP_ID = "app_id";
    private static final String INTENT_CHANNEL_ID = "channel_id";

    private final DingRtcAuthInfo authInfo = new DingRtcAuthInfo();
    private String userName;
    private DingRtcEngine engine;
    private AiAgent agent;
    private boolean isScreenMode = false;
    private boolean isVideoOn = true;
    private boolean isAudioOn = true;
    private boolean isAgentAudioOn = true;
    private FrameLayout selfRender;
    private ImageButton selfAudio;
    private ImageButton selfVideo;
    private ImageView selfAvatar;
    private ImageButton agentAudio;
    private TextView agentSettings;
    private TextView agentState;
    private Spinner agentModelSpinner;
    private Spinner selfVideoModeSpinner;
    private Button startBtn;
    private Button speakToTalkBtn;
    private View speakToTalkLayout;
    private AgentChatAdapter msgAdapter;
    private AgentSettingsDialog agentSettingsDialog;
    private ChatSettingsDialog chatSettingsDialog;
    private String agentUid;
    private final Handler handler = new Handler(Looper.getMainLooper());
    private final Runnable speakToTalkLongPressRunnable = new Runnable() {
        @Override
        public void run() {
            agent.sendPushToTalk(DingRtmAgentClient.AgentPushToTalkCmd.Start);
        }
    };

    public static void launch(Context context, String userId, String userName, String token,
                              String appId, String channelName) {
        Intent intent = new Intent();
        intent.setClass(context, AiAgentActivity.class);
        intent.putExtra(INTENT_USER_ID, userId);
        intent.putExtra(INTENT_USER_NAME, userName);
        intent.putExtra(INTENT_TOKEN, token);
        intent.putExtra(INTENT_APP_ID, appId);
        intent.putExtra(INTENT_CHANNEL_ID, channelName);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_agent);

        configFromIntent();

        initViews();
        initDialogs();

        initEngine();
        initAgent();
        joinChannel();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        leaveChannel();
    }

    private void configFromIntent() {
        Intent intent = getIntent();
        authInfo.userId = intent.getStringExtra(INTENT_USER_ID);
        userName = intent.getStringExtra(INTENT_USER_NAME);
        authInfo.token = intent.getStringExtra(INTENT_TOKEN);
        authInfo.appId = intent.getStringExtra(INTENT_APP_ID);
        authInfo.channelId = intent.getStringExtra(INTENT_CHANNEL_ID);
    }

    @SuppressLint("ClickableViewAccessibility")
    private void initViews() {
        selfRender = findViewById(R.id.self_render);
        selfAudio = findViewById(R.id.self_audio);
        selfAvatar = findViewById(R.id.self_avatar);
        startBtn = findViewById(R.id.start_agent_btn);
        selfAudio.setOnClickListener(v -> pubAudio(!isAudioOn));
        selfVideo = findViewById(R.id.self_video);
        selfVideo.setOnClickListener(v -> {
            if (isVideoOn) {
                int ret = engine.muteLocalCamera(true,
                        isScreenMode ? DingRtcVideoTrackScreen : DingRtcVideoTrackCamera);
                if (ret == 0) {
                    isVideoOn = false;
                    selfVideo.setBackground(ContextCompat.getDrawable(AiAgentActivity.this, R.drawable.video_off));
                    showAvatar();
                }
            } else {
                int ret = engine.muteLocalCamera(false,
                        isScreenMode ? DingRtcVideoTrackScreen : DingRtcVideoTrackCamera);
                if (ret == 0) {
                    isVideoOn = true;
                    selfVideo.setBackground(ContextCompat.getDrawable(AiAgentActivity.this, R.drawable.video_on));
                    hideAvatar();
                }
            }
        });

        agentAudio = findViewById(R.id.agent_mute);
        agentAudio.setOnClickListener(v -> {
            if (TextUtils.isEmpty(agentUid)) {
                showToast("Agent还未入会！");
                return;
            }
            if (isAgentAudioOn) {
                int ret = engine.muteRemoteAudio(agentUid, true);
                if (ret == 0) {
                    isAgentAudioOn = false;
                    agentAudio.setBackground(ContextCompat.getDrawable(AiAgentActivity.this, R.drawable.speaker_off));
                }
            } else {
                int ret = engine.muteRemoteAudio(agentUid, false);
                if (ret == 0) {
                    isAgentAudioOn = true;
                    agentAudio.setBackground(ContextCompat.getDrawable(AiAgentActivity.this, R.drawable.speaker_on));
                }
            }
        });

        agentSettings = findViewById(R.id.agent_settings);
        agentModelSpinner = findViewById(R.id.agent_model);
        agentModelSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                agent.updateAgentModel(position);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        agentState = findViewById(R.id.agent_state);

        selfVideoModeSpinner = findViewById(R.id.self_video_mode);
        selfVideoModeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                boolean isScreen = (position == 1);
                if (isScreenMode != isScreen) {
                    if (isScreen) {
                        switchToScreenMode();
                    } else {
                        switchToCameraMode();
                    }
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        speakToTalkLayout = findViewById(R.id.speak_to_talk_layout);
        speakToTalkBtn = findViewById(R.id.speak_to_talk_btn);
        speakToTalkBtn.setOnTouchListener((v, event) -> {
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    handler.postDelayed(speakToTalkLongPressRunnable, 500); // 500秒后触发长按
                    speakToTalkBtn.setText("松开发送");
                    return true;

                case MotionEvent.ACTION_UP:
                    handler.removeCallbacks(speakToTalkLongPressRunnable);
                    agent.sendPushToTalk(DingRtmAgentClient.AgentPushToTalkCmd.Send);
                    speakToTalkBtn.setText("长按说话");
                    return true;

                case MotionEvent.ACTION_CANCEL:
                    handler.removeCallbacks(speakToTalkLongPressRunnable);
                    agent.sendPushToTalk(DingRtmAgentClient.AgentPushToTalkCmd.Cancel);
                    speakToTalkBtn.setText("长按说话");
                    return true;
            }
            return false;
        });

        msgAdapter = new AgentChatAdapter(this);
        ListView msgList = findViewById(R.id.msg_list);
        msgList.setAdapter(msgAdapter);
    }

    private void initDialogs() {
        agentSettingsDialog = new AgentSettingsDialog(this);
        agentSettingsDialog.setOnDismissListener(dialog -> {
            int voiceRange = agentSettingsDialog.getVoiceRange();
            String prompt = agentSettingsDialog.getPrompt();
            String greeting = agentSettingsDialog.getGreeting();
            agent.updateAgentSettings(voiceRange, prompt, greeting);
        });

        chatSettingsDialog = new ChatSettingsDialog(this);
        chatSettingsDialog.setOnDismissListener(dialog -> {
            agent.updateChatSettings(chatSettingsDialog.getTalkMode(),
                    chatSettingsDialog.getInterruptMode());
            if (chatSettingsDialog.getTalkMode() == 2 && agent.isStarted()) { // 对讲机模式
                speakToTalkLayout.setVisibility(View.VISIBLE);
            } else {
                speakToTalkLayout.setVisibility(View.GONE);
            }
        });
    }

    private void initEngine() {
        engine = DingRtcEngine.create(getApplicationContext(), "");
        engine.setRtcEngineEventListener(new DingRtcEngineEventListener() {
            @Override
            public void onJoinChannelResult(int result, String channel, String userId, int elapsed) {
                runOnUiThread(() -> {
                    Log.d(TAG, "onJoinChannelResult " + result);
                    onJoinResult(result);
                });
            }

            @Override
            public void onRemoteUserOnLineNotify(String uid, int elapsed) {
                runOnUiThread(() -> {
                    agentUid = uid;
                    showToast("Agent " + uid + " 已入会！");
                });
            }
        });
    }

    private void initAgent() {
        agent = new AiAgent(engine.getRtmClient(), AiAgentActivity.this, msgAdapter);
    }

    private void joinChannel() {
        int ret = engine.joinChannel(authInfo, userName);
        if(ret != 0) {
            showToastL("加入房间失败");
        }
    }

    private void leaveChannel() {
        agent.stop();
        engine.leaveChannel();
        engine.destroy();
    }

    public void onClickAgentSettings(View view) {
        if (!agentSettingsDialog.isShowing()) {
            agentSettingsDialog.show();
            agentSettingsDialog.getWindow().setLayout(
                    DensityUtils.getScreenWidth(this) * 7 / 8,
                    ConstraintLayout.LayoutParams.WRAP_CONTENT);
        }
    }

    public void onClickChatSettings(View view) {
        if (!chatSettingsDialog.isShowing()) {
            chatSettingsDialog.show();
            chatSettingsDialog.getWindow().setLayout(
                    DensityUtils.getScreenWidth(this) * 4 / 5,
                    ConstraintLayout.LayoutParams.WRAP_CONTENT);
        }
    }

    public void onClickStartAgent(View view) {
        if (agent.isStarted()) {
            agent.stop();
        } else {
            agent.start(authInfo.appId, authInfo.channelId, authInfo.userId);
        }
    }

    private void onJoinResult(int result) {
        if (result != 0) {
            showToast("入会失败，result： " + result);
            onBackPressed();
            return;
        }
        engine.subscribeAllRemoteAudioStreams(true);
        pubAudio(true);
        switchToCameraMode();
    }

    private void pubAudio(boolean pub) {
        if (pub) {
            int ret = engine.publishLocalAudioStream(true);
            if (ret == 0) {
                isAudioOn = true;
                selfAudio.setBackground(ContextCompat.getDrawable(AiAgentActivity.this, R.drawable.mic_on));
            } else {
                isAudioOn = false;
                selfAudio.setBackground(ContextCompat.getDrawable(AiAgentActivity.this, R.drawable.mic_off));
            }
        } else {
            int ret = engine.publishLocalAudioStream(false);
            if (ret == 0) {
                isAudioOn = false;
                selfAudio.setBackground(ContextCompat.getDrawable(AiAgentActivity.this, R.drawable.mic_off));
            }
        }
    }

    private void switchToCameraMode() {
        SurfaceView cameraView = DingRtcEngine.createRenderSurfaceView(this);
        DingRtcEngine.DingRtcVideoCanvas canvas = new DingRtcEngine.DingRtcVideoCanvas();
        canvas.view = cameraView;
        canvas.renderMode = DingRtcRenderModeAuto;
        engine.setLocalViewConfig(canvas, DingRtcVideoTrackCamera);
        selfRender.removeAllViews();
        selfRender.addView(cameraView);

        int ret = engine.startPreview();
        if (ret == 0) {
            isScreenMode = false;
        }
    }

    private void switchToScreenMode() {
        SurfaceView screenView = DingRtcEngine.createRenderSurfaceView(this);
        DingRtcEngine.DingRtcVideoCanvas canvas = new DingRtcEngine.DingRtcVideoCanvas();
        canvas.view = screenView;
        canvas.renderMode = DingRtcRenderModeAuto;
        engine.setLocalViewConfig(canvas, DingRtcVideoTrackScreen);
        selfRender.removeAllViews();
        selfRender.addView(screenView);

        int ret = engine.startScreenShare(DingRtcEngine.DingRtcScreenShareMode.DingRtcScreenShareAllMode);
        if (ret == 0) {
            isScreenMode = true;
        }
    }

    private void showAvatar() {
        selfAvatar.setVisibility(View.VISIBLE);
        selfRender.setVisibility(View.GONE);
        selfVideoModeSpinner.setVisibility(View.GONE);
    }

    private void hideAvatar() {
        selfAvatar.setVisibility(View.GONE);
        selfRender.setVisibility(View.VISIBLE);
        selfVideoModeSpinner.setVisibility(View.VISIBLE);
    }

    @Override
    public void onStartResult(boolean success) {
        runOnUiThread(() -> {
            if (success) {
                startBtn.setText("结束会话");
                startBtn.setBackground(ContextCompat.getDrawable(AiAgentActivity.this,
                        R.drawable.btn_red_background));
                startBtn.setTextColor(Color.RED);

                agentSettings.setEnabled(false);
                Drawable bgDrawable = DrawableCompat.wrap(agentSettings.getBackground());
                DrawableCompat.setTint(bgDrawable, Color.GRAY);
                agentModelSpinner.setEnabled(false);
                if (chatSettingsDialog.getTalkMode() == 2) { // 对讲机模式
                    speakToTalkLayout.setVisibility(View.VISIBLE);
                }
            } else {
                startBtn.setText("开始会话");
                startBtn.setBackground(ContextCompat.getDrawable(AiAgentActivity.this,
                        R.drawable.btn_ok_background));
                startBtn.setTextColor(Color.WHITE);

                agentSettings.setEnabled(true);
                DrawableCompat.setTintList(agentSettings.getBackground(), null);
                agentModelSpinner.setEnabled(true);
            }
        });
    }

    @Override
    public void onStopResult(boolean success) {
        runOnUiThread(() -> {
            if (success) {
                startBtn.setText("开始会话");
                startBtn.setBackground(ContextCompat.getDrawable(AiAgentActivity.this,
                        R.drawable.btn_ok_background));
                startBtn.setTextColor(Color.WHITE);
                msgAdapter.clearAllMsg();
                agentSettings.setEnabled(true);
                DrawableCompat.setTintList(agentSettings.getBackground(), null);
                agentModelSpinner.setEnabled(true);
                agentState.setText("");
                if (chatSettingsDialog.getTalkMode() == 2) { // 对讲机模式
                    speakToTalkLayout.setVisibility(View.GONE);
                }
            } else {
                startBtn.setText("结束会话");
                startBtn.setBackground(ContextCompat.getDrawable(AiAgentActivity.this,
                        R.drawable.btn_red_background));
                startBtn.setTextColor(Color.RED);

                agentSettings.setEnabled(false);
                Drawable bgDrawable = DrawableCompat.wrap(agentSettings.getBackground());
                DrawableCompat.setTint(bgDrawable, Color.GRAY);
                agentModelSpinner.setEnabled(false);
            }
        });
    }

    @Override
    public void onAgentStateChanged(String agentId, DingRtmAgentClient.AgentState state) {
        final String stateStr;
        switch (state) {
            case Listening:
                stateStr = "聆听中...";
                break;
            case Thinking:
                stateStr = "思考中...";
                break;
            case Responding:
                stateStr = "回答中...";
                break;
            default:
                stateStr = "未知状态";
                break;
        }
        runOnUiThread(() -> agentState.setText(stateStr));
    }
}