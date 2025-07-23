package com.ding.rtc.apiexample.advanced.switchscreen;

import android.Manifest;
import android.app.AlertDialog;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.preference.PreferenceManager;

import android.text.TextUtils;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import com.ding.rtc.api.example.token_generator.TokenGenerator;

public class SwitchScreenLoginActivity extends AppCompatActivity {
    public static final String kChannelID = "channel_id";
    public static final String kUserID = "user_id";
    public static final String kUserName = "user_name";
    private static final int PERMISSION_REQUEST_CODE = 10;

    private EditText mChannelId;
    private EditText mUserId;
    private EditText mUserName;
    private RadioGroup mRadioGroup;

    private int mSwitchScreenType = 0; // 0: systemui auto; 1: systemui portrait; 2: systemui landscape;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login_switch_screen);
        if (getSupportActionBar() != null) {
            getSupportActionBar().show();
            getSupportActionBar().setTitle(R.string.switch_screen);
        }

        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        String channelId = preferences.getString(kChannelID, "");
        mChannelId = findViewById(R.id.channelIdEditText);
        mChannelId.setText(channelId);
        mChannelId.setRawInputType(Configuration.KEYBOARD_QWERTY);
        mUserId = findViewById(R.id.userIdEditText);
        long userId = (long) 10000 + new Random().nextInt(1000);
        mUserId.setText(preferences.getString(kUserID, Long.toString(userId)));
        String userName = preferences.getString(kUserName, "");
        mUserName = findViewById(R.id.usernameEditText);
        mUserName.setText(userName);
        findViewById(R.id.joinChannelButton).setOnClickListener(v -> joinChannel());
        mRadioGroup = findViewById(R.id.radioGroup);
        mRadioGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup radioGroup, int checkedId) {
                if (checkedId == R.id.radioAuto) {
                    mSwitchScreenType = 0;
                } else if (checkedId == R.id.radioPortrait) {
                    mSwitchScreenType = 1;
                } else if (checkedId == R.id.radioLandscape) {
                    mSwitchScreenType = 2;
                }
            }
        });

        mChannelId.requestFocus();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode) {
            case PERMISSION_REQUEST_CODE:
                if (getMissedPermission().size() == 0) {
                    startCall(mChannelId.getText().toString(),
                            mUserId.getText().toString(),
                            mUserName.getText().toString());
                } else {
                    Toast.makeText(SwitchScreenLoginActivity.this, "Some permissions are denied", Toast.LENGTH_LONG).show();
                }
                break;
            default:
                super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    private void joinChannel() {
        String userId = mUserId.getText().toString();
        String channelId = mChannelId.getText().toString();
        String userName = mUserName.getText().toString();
        if (TextUtils.isEmpty(userId)) {
            mUserId.setError("Required");
            return;
        }
        if (TextUtils.isEmpty(channelId)) {
            mChannelId.setError("Required");
            return;
        }

        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        SharedPreferences.Editor editor = preferences.edit();
        editor.putString(kChannelID, channelId);
        editor.putString(kUserID, userId);
        editor.putString(kUserName, userName);
        editor.apply();

        final List<String> missed = getMissedPermission();
        if (missed.size() != 0) {

            List<String> showRationale = new ArrayList<>();
            for (String permission : missed) {
                if (ActivityCompat.shouldShowRequestPermissionRationale(this, permission)) {
                    showRationale.add(permission);
                }
            }

            if (showRationale.size() > 0) {
                new AlertDialog.Builder(SwitchScreenLoginActivity.this)
                        .setMessage("Please allow the permissions")
                        .setPositiveButton("OK", (dialog, which) ->
                                ActivityCompat.requestPermissions(SwitchScreenLoginActivity.this,
                                        missed.toArray(new String[0]),
                                        PERMISSION_REQUEST_CODE))
                        .setNegativeButton("Cancel", null)
                        .create()
                        .show();
            } else {
                ActivityCompat.requestPermissions(this, missed.toArray(new String[0]), PERMISSION_REQUEST_CODE);
            }

            return;
        }

        startCall(channelId, userId, userName);
    }

    private List<String> getMissedPermission() {
        // Important: Add the required authorization here
        ArrayList<String> needPermissions = new ArrayList<>(16);
        needPermissions.add(Manifest.permission.RECORD_AUDIO);
        needPermissions.add(Manifest.permission.CAMERA);
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

    private void startCall(String channelId, String userId, String userName) {
        if (mSwitchScreenType == 0) {
            SwitchScreenWithSystemUIAutoActivity.launch(SwitchScreenLoginActivity.this, TokenGenerator.APP_ID, channelId,
                    userId, userName, TokenGenerator.generateAppToken(channelId, userId));
        } else if (mSwitchScreenType == 1) {
            SwitchScreenWithSystemUIPortraitActivity.launch(SwitchScreenLoginActivity.this, TokenGenerator.APP_ID, channelId,
                    userId, userName, TokenGenerator.generateAppToken(channelId, userId));
        } else if (mSwitchScreenType == 2) {
            SwitchScreenWithSystemUILandscapeActivity.launch(SwitchScreenLoginActivity.this, TokenGenerator.APP_ID, channelId,
                    userId, userName, TokenGenerator.generateAppToken(channelId, userId));
        }
        finish();
    }
}
