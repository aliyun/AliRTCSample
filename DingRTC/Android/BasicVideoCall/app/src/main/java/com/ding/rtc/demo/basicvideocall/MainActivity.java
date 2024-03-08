package com.ding.rtc.demo.basicvideocall;

import android.Manifest;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;
import androidx.preference.PreferenceManager;

import android.text.TextUtils;
import android.util.Log;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class MainActivity extends AppCompatActivity {
    public static final String kChannelID = "channel_id";
    public static final String kUserID = "user_id";
    public static final String kUserName = "user_name";
    public static final String kAudioLoudspeaker = "audio_loudspeaker";
    private static final int PERMISSION_REQUEST_CODE = 10;

    private EditText mChannelId;
    private EditText mUserId;
    private EditText mUserName;

    private Switch mLoudspeaker;
    private ProgressDialog mProgressDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        String channelId = preferences.getString(kChannelID, "");
        mChannelId = findViewById(R.id.editChannelID);
        mChannelId.setText(channelId);
        mChannelId.setRawInputType(Configuration.KEYBOARD_QWERTY);
        mUserId = findViewById(R.id.editUserID);
        long userId = (long) 10000 + new Random().nextInt(1000);
        userId = preferences.getLong(kUserID, userId);
        mUserId.setText(Long.toString(userId));
        String userName = preferences.getString(kUserName, "");
        mUserName = findViewById(R.id.editUserName);
        mUserName.setText(userName);
        findViewById(R.id.buttonJoinChannel).setOnClickListener(v -> joinChannel());

        boolean enabled = preferences.getBoolean(kAudioLoudspeaker, true);
        mLoudspeaker = findViewById(R.id.switchLoudspeaker);
        mLoudspeaker.setOnClickListener(v -> {
            SharedPreferences.Editor editor = preferences.edit();
            editor.putBoolean(kAudioLoudspeaker, mLoudspeaker.isChecked());
            editor.apply();
        });
        mLoudspeaker.setChecked(enabled);

        mChannelId.requestFocus();

        mProgressDialog = new ProgressDialog(this);
        mProgressDialog.setMessage("正在登录");
        mProgressDialog.setCancelable(false);
    }

    @Override
    protected void onResume() {
        super.onResume();
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        boolean enabled = preferences.getBoolean(kAudioLoudspeaker, true);
        mLoudspeaker.setChecked(enabled);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
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
                    Toast.makeText(MainActivity.this, "Some permissions are denied", Toast.LENGTH_LONG).show();
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
        editor.putLong(kUserID, Long.parseLong(userId));
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
                new AlertDialog.Builder(MainActivity.this)
                        .setMessage("Please allow the permissions")
                        .setPositiveButton("OK", (dialog, which) ->
                                ActivityCompat.requestPermissions(MainActivity.this,
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
        ArrayList<String> needPermissions = new ArrayList<>(16);
        needPermissions.add(Manifest.permission.CAMERA);
        needPermissions.add(Manifest.permission.RECORD_AUDIO);
        needPermissions.add(Manifest.permission.WRITE_EXTERNAL_STORAGE);
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
        mProgressDialog.show();
        CallActivity.launch(MainActivity.this,
                userId, userName, DingApplication.TOKEN, DingApplication.APP_ID,
                channelId, DingApplication.NONCE, DingApplication.TIMESTAMP, DingApplication.GSLB_SERVER);
    }
}
