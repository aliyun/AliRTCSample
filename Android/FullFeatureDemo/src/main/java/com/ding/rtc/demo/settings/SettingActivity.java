package com.ding.rtc.demo.settings;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.TextView;

import com.ding.rtc.demo.BaseActivity;
import com.ding.rtc.demo.R;

public class SettingActivity extends BaseActivity {

    public static int SETTING_REQUEST_CODE = 9999;

    public static void launch(Activity context) {
        Intent intent = new Intent();
        intent.setClass(context, SettingActivity.class);
        context.startActivityForResult(intent, SETTING_REQUEST_CODE);
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings_layout);
        initTitleView();
    }

    private void initTitleView() {
        TextView titleView = findViewById(R.id.tv_title);
        titleView.setText(R.string.title_activity_settings);

        ImageView settingView = findViewById(R.id.iv_left_icon);
        settingView.setImageResource(R.drawable.svg_icon_back);
        settingView.setOnClickListener(view -> finish());
    }

}
