package com.ding.rtc.demo.aiagent;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.TextView;

import androidx.annotation.NonNull;

import com.ding.rtc.demo.R;

public class ChatSettingsDialog extends Dialog {

    private RadioButton defaultTalkModeBtn;
    private RadioButton speakToTalkModeBtn;
    private CheckBox interruptCb;
    private TextView interruptDes;
    private int talkMode = 0;
    private boolean interrupt = true;

    public ChatSettingsDialog(@NonNull Context context) {
        super(context);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.chat_settings_dialog);

        defaultTalkModeBtn = findViewById(R.id.default_talk_mode);
        speakToTalkModeBtn = findViewById(R.id.speak_to_talk_mode);
        interruptCb = findViewById(R.id.interrupt_cb);
        interruptDes = findViewById(R.id.interrupt_des);

        defaultTalkModeBtn.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    talkMode = 1;
                    speakToTalkModeBtn.setChecked(false);
                    interruptCb.setVisibility(View.VISIBLE);
                    interruptDes.setVisibility(View.VISIBLE);
                }
            }
        });

        speakToTalkModeBtn.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    talkMode = 2;
                    defaultTalkModeBtn.setChecked(false);
                    interruptCb.setVisibility(View.GONE);
                    interruptDes.setVisibility(View.GONE);
                }
            }
        });

        interruptCb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                interrupt = isChecked;
            }
        });
    }

    public int getTalkMode() {
        return talkMode;
    }

    public int getInterruptMode() { // 1 enable, 2 disable
        return interrupt ? 1 : 2;
    }
}