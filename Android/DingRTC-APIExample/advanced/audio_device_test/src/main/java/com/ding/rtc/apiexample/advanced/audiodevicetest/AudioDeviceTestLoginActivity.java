package com.ding.rtc.apiexample.advanced.audiodevicetest;

import android.Manifest;
import android.app.AlertDialog;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.preference.PreferenceManager;

import android.text.TextUtils;
import android.widget.EditText;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import com.ding.rtc.api.example.token_generator.TokenGenerator;

public class AudioDeviceTestLoginActivity extends AppCompatActivity {
    public static final String kChannelID = "channel_id";
    public static final String kUserID = "user_id";
    public static final String kUserName = "user_name";
    private static final int PERMISSION_REQUEST_CODE = 10;

    private EditText mChannelId;
    private EditText mUserId;
    private EditText mUserName;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login_audio_device_test);
        if (getSupportActionBar() != null) {
            getSupportActionBar().show();
            getSupportActionBar().setTitle(R.string.audio_device_test);
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
        findViewById(R.id.joinChannelButton).setOnClickListener(v -> {
            joinChannel();
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
                    Toast.makeText(AudioDeviceTestLoginActivity.this, "Some permissions are denied", Toast.LENGTH_LONG).show();
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
                new AlertDialog.Builder(AudioDeviceTestLoginActivity.this)
                        .setMessage("Please allow the permissions")
                        .setPositiveButton("OK", (dialog, which) ->
                                ActivityCompat.requestPermissions(AudioDeviceTestLoginActivity.this,
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
        return ContextCompat.checkSelfPermission(this, permission) == PackageManager.PERMISSION_GRANTED;
    }

    private void startCall(String channelId, String userId, String userName) {
        AudioDeviceTestActivity.launch(AudioDeviceTestLoginActivity.this, TokenGenerator.APP_ID, channelId,
                userId, userName, TokenGenerator.generateAppToken(channelId, userId));
        finish();
    }
}
