package com.ding.rtc.demo.ui;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.NonNull;

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.demo.R;
import com.ding.rtc.demo.RoomUser;

public class RemoteUserVolumeDialog extends Dialog {

    private SeekBar seekbar;
    private TextView volumeText;
    private final DingRtcEngine engine;
    private final RoomUser user;

    public RemoteUserVolumeDialog(@NonNull Context context, DingRtcEngine engine, RoomUser user) {
        super(context);
        this.engine = engine;
        this.user = user;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.remote_user_volume_dialog);
        seekbar = findViewById(R.id.volume_seekbar);
        volumeText = findViewById(R.id.volume_value);

        seekbar.setProgress(user.playoutSignalVolume);
        volumeText.setText(String.valueOf(user.playoutSignalVolume));
        seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                volumeText.setText(String.valueOf(progress));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                user.playoutSignalVolume = seekBar.getProgress();
            }
        });

        findViewById(R.id.cancel).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dismiss();
            }
        });

        findViewById(R.id.ok).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                engine.setRemoteUserPlayoutSignalVolume(user.userId, user.playoutSignalVolume);
                dismiss();
            }
        });


    }
}
