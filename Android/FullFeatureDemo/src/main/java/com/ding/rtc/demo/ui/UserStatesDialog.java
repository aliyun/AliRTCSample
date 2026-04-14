package com.ding.rtc.demo.ui;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.ding.rtc.demo.R;
import com.ding.rtc.demo.RoomUser;

public class UserStatesDialog extends Dialog {
    RoomUser user;

    public UserStatesDialog(Context context, RoomUser user) {
        super(context);
        this.user = user;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.user_state_layout);

        TextView cameraState = findViewById(R.id.camera_state);
        cameraState.setText("摄像头：" + (user.isCameraOn ? "On" : "Off"));

        if (user.isSelf) {
            TextView audioDeviceState = findViewById(R.id.audio_device_state);
            audioDeviceState.setVisibility(View.VISIBLE);
            audioDeviceState.setText("音频设备：" + user.audioDeviceName);
        } else {
            TextView foreBackState = findViewById(R.id.fore_back_ground_state);
            String state;
            if (user.foreBackState == RoomUser.ForeBackState.Fore) {
                state = "前台";
            } else if (user.foreBackState == RoomUser.ForeBackState.Back) {
                state = "后台";
            } else {
                state = "未知";
            }
            foreBackState.setText("前后台：" + state);
            foreBackState.setVisibility(View.VISIBLE);
        }
    }
}
