package com.ding.rtc.demo.aiagent;

import android.app.Dialog;
import android.content.Context;
import android.widget.EditText;
import android.widget.Spinner;

import androidx.annotation.NonNull;

import com.ding.rtc.demo.R;

public class AgentSettingsDialog extends Dialog {

    private int voiceRange;
    private String prompt;
    private String greeting;

    public AgentSettingsDialog(@NonNull Context context) {
        super(context);

        setContentView(R.layout.agent_settings_dialog);

        Spinner rangeSpinner = findViewById(R.id.range_spinner);
        EditText promptEt = findViewById(R.id.prompt_content);
        EditText greetingEt = findViewById(R.id.greeting_content);
        voiceRange = rangeSpinner.getSelectedItemPosition();
        prompt = promptEt.getText().toString();
        greeting = greetingEt.getText().toString();

        findViewById(R.id.confirm_btn).setOnClickListener(v -> {
            voiceRange = rangeSpinner.getSelectedItemPosition();
            prompt = promptEt.getText().toString();
            greeting = greetingEt.getText().toString();
            dismiss();
        });

        findViewById(R.id.cancel_btn).setOnClickListener(v -> dismiss());
    }

    public int getVoiceRange() {
        return voiceRange;
    }

    public String getPrompt() {
        return prompt;
    }

    public String getGreeting() {
        return greeting;
    }
}
