package com.ding.rtc.api.example;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import com.ding.rtc.apiexample.advanced.audiogroup.AudioGroupLoginActivity;
import com.ding.rtc.apiexample.advanced.audiosetting.AudioSettingLoginActivity;
import com.ding.rtc.apiexample.advanced.beauty.BeautyLoginActivity;
import com.ding.rtc.apiexample.advanced.externalaudiosource.ExternalAudioSourceLoginActivity;
import com.ding.rtc.apiexample.advanced.externalvideosource.ExternalVideoSourceLoginActivity;
import com.ding.rtc.apiexample.advanced.rawaudiodata.RawAudioDataLoginActivity;
import com.ding.rtc.apiexample.advanced.rawvideodata.RawVideoDataLoginActivity;
import com.ding.rtc.apiexample.advanced.screenshare.ScreenshareLoginActivity;
import com.ding.rtc.apiexample.advanced.speechenhance.SpeechEnhanceLoginActivity;
import com.ding.rtc.apiexample.advanced.subtitle.SubtitleLoginActivity;
import com.ding.rtc.apiexample.advanced.switchscreen.SwitchScreenLoginActivity;
import com.ding.rtc.apiexample.advanced.videosetting.VideoSettingLoginActivity;
import com.ding.rtc.apiexample.advanced.virtualbackground.VirtualBackgroundLoginActivity;
import com.ding.rtc.apiexample.advanced.whiteboard.WhiteboardLoginActivity;
import com.ding.rtc.apiexample.advanced.rtm.RTMLoginActivity;
import com.ding.rtc.apiexample.basic.audiocall.AudioCallLoginActivity;
import com.ding.rtc.apiexample.basic.videocall.VideoCallLoginActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setTitle(R.string.app_name);
        }
        findViewById(R.id.view_audio_call).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, AudioCallLoginActivity.class);
                startActivity(intent);
            }
        });

        findViewById(R.id.view_video_call).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, VideoCallLoginActivity.class);
                startActivity(intent);
            }
        });

        findViewById(R.id.view_rtm).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, RTMLoginActivity.class);
                startActivity(intent);
            }
        });

        findViewById(R.id.view_audiogroup).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, AudioGroupLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_whiteboard).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, WhiteboardLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_speechenhance).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, SpeechEnhanceLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_screenshare).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, ScreenshareLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_virtual_background).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, VirtualBackgroundLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_beauty).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, BeautyLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_external_video_source).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, ExternalVideoSourceLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_external_audio_source).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, ExternalAudioSourceLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_raw_video_data).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, RawVideoDataLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_raw_audio_data).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, RawAudioDataLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_subtitle).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, SubtitleLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_switch_screen).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, SwitchScreenLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_video_setting).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, VideoSettingLoginActivity.class);
                startActivity(intent);
            }
        });
        findViewById(R.id.view_audio_setting).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, AudioSettingLoginActivity.class);
                startActivity(intent);
            }
        });
    }
}
