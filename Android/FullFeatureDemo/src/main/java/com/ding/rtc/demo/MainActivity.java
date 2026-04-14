package com.ding.rtc.demo;

import android.Manifest;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.text.InputFilter;
import android.text.Spanned;
import android.text.TextUtils;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.demo.aiagent.AiAgentActivity;
import com.ding.rtc.demo.bigconf.BigConfRoomActivity;
import com.ding.rtc.demo.settings.SettingActivity;
import com.ding.rtc.demo.token.TokenGenerator;
import com.ding.rtc.demo.utils.FileWriteUtils;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class MainActivity extends BaseActivity {
    private final static int PERMISSION_REQUEST_JOIN_CODE = 10;

    public final static String STAMP_FILE_NAME = "drawable/star.png";
    public static final String PDF_FILE_NAME = "pdf_sample_doc.pdf";
    public static final String WORD_FILE_NAME = "word_sample_doc.docx";
    private boolean mClickAiAgent = false;
    private EditText mChannelName;
    private EditText mUserID;
    private EditText mUserName;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        initTitleView();
        initUserInfo();

        TextView versionInfo = findViewById(R.id.version_info);
        versionInfo.setText("SDK version : " + DingRtcEngine.getSDKVersion());

        FileWriteUtils.checkAndCopyFileToSD("heibai_res.png", getApplicationContext());
        FileWriteUtils.copyResToInternalStorage(getBaseContext(), R.drawable.star, STAMP_FILE_NAME);
    }

    private void initUserInfo() {
        InputFilter filter = new InputFilter() {
            public CharSequence filter(CharSequence source, int start, int end, Spanned dest, int dstart, int dend) {
                // 字母数字下划线横线
                if (source.toString().matches("[a-zA-Z0-9_-]+")) {
                    return source;
                } else {
                    return "";
                }
            }
        };
        mChannelName = findViewById(R.id.channel_name);
        mChannelName.setFilters(new InputFilter[]{filter});
        long uid = new Random().nextInt(1000000);
        mUserID = findViewById(R.id.user_id);
        mUserID.setText(uid + "");
        mUserID.setFilters(new InputFilter[]{filter});
        mUserName = findViewById(R.id.user_name);
        mUserName.setText("Android_" + uid);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == PERMISSION_REQUEST_JOIN_CODE) {
            List<String> miss = getMissedPermission();
            if (!miss.isEmpty()) {
                showToastL("miss permission");
                return;
            }
            startCall();
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    private void initTitleView() {
        TextView titleView = findViewById(R.id.tv_title);
        titleView.setText(R.string.title_join_call);

        ImageView settingView = findViewById(R.id.iv_left_icon);
        settingView.setOnClickListener(view -> SettingActivity.launch(MainActivity.this));
    }

    public void onClickCloseWarning(View view) {
        findViewById(R.id.warning_info).setVisibility(View.GONE);
    }

    public void onClickJoin(View view) {
        mClickAiAgent = false;
        final List<String> missed = getMissedPermission();
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M || missed.isEmpty()) {
            startCall();
        } else {
            ActivityCompat.requestPermissions(this, missed.toArray(new String[0]), PERMISSION_REQUEST_JOIN_CODE);
        }
    }

    public void onClickAiAgent(View view) {
        mClickAiAgent = true;
        final List<String> missed = getMissedPermission();
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M || missed.isEmpty()) {
            startCall();
        } else {
            ActivityCompat.requestPermissions(this, missed.toArray(new String[0]), PERMISSION_REQUEST_JOIN_CODE);
        }
    }

    private void startCall() {
        String channelName = mChannelName.getText().toString();
        if (TextUtils.isEmpty(channelName)) {
            showToastL("channelName empty!!!");
            return;
        }

        String userId = mUserID.getText().toString();
        if (TextUtils.isEmpty(userId)) {
            showToastL("userId empty!!!");
            return;
        }

        String userName = mUserName.getText().toString();
        if (TextUtils.isEmpty(userName)) {
            userName = "Android_" + userId;
        }
        getAppServerToken(channelName, userId, userName);
    }


    private void getAppServerToken(String channelName, String userId, String userName) {
        String token = TokenGenerator.generateAppToken(channelName, userId);
        if (TextUtils.isEmpty(token)) {
            showToastL("获取token失败");
            return;
        }
        if (mClickAiAgent) {
            AiAgentActivity.launch(MainActivity.this, userId, userName, token,
                    TokenGenerator.APP_ID, channelName);
        } else {
            SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
            int renderColumnCount;
            String gridStr = preferences.getString(getString(R.string.render_grid_key), "0");
            if ("1".equals(gridStr)) {
                renderColumnCount = 3;
            } else if ("2".equals(gridStr)) {
                renderColumnCount = 4;
            } else if ("3".equals(gridStr)) {
                renderColumnCount = 5;
            } else {
                renderColumnCount = 2;
            }
            if (renderColumnCount > 2) {
                BigConfRoomActivity.launch(MainActivity.this, userId, userName, token,
                        TokenGenerator.APP_ID, channelName, renderColumnCount);
            } else {
                RoomActivity.launch(MainActivity.this, userId, userName, token,
                        TokenGenerator.APP_ID, channelName);
            }
        }
    }

    private List<String> getMissedPermission() {
        ArrayList<String> needPermissions = new ArrayList<>(16);
        needPermissions.add(Manifest.permission.CAMERA);
        needPermissions.add(Manifest.permission.RECORD_AUDIO);
        needPermissions.add(Manifest.permission.READ_PHONE_STATE);
        if (Build.VERSION.SDK_INT <= 32) {
            needPermissions.add(Manifest.permission.WRITE_EXTERNAL_STORAGE);
        }
        if (Build.VERSION.SDK_INT >= 31) { //android 12
            needPermissions.add("android.permission.BLUETOOTH_SCAN");
            needPermissions.add("android.permission.BLUETOOTH_ADVERTISE");
            needPermissions.add("android.permission.BLUETOOTH_CONNECT");
        }
        List<String> missedList = new ArrayList<>();
        for (String permission : needPermissions) {
            if (!checkPermission(permission)) {
                missedList.add(permission);
            }
        }
        return missedList;
    }

    private boolean checkPermission(String permission) {
        return PackageManager.PERMISSION_GRANTED ==
                checkPermission(permission,
                        android.os.Process.myPid(),
                        android.os.Process.myUid());
    }
}
