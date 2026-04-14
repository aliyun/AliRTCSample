package com.ding.rtc.demo.ui;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.widget.TextView;

import androidx.annotation.NonNull;

import com.ding.rtc.DingRtcEngine;
import com.ding.rtc.demo.R;

public class QualityReportDialog extends Dialog {


    // Network
    private TextView networkTypeTv;
    private TextView signalLevelTv;
    private TextView lossRateTv;
    private TextView rttMsTv;
    private TextView networkRecvBitrateTv;
    private TextView networkSendBitrateTv;

    // Audio
    private TextView audioRecvBitrateTv;
    private TextView audioSendBitrateTv;
    private TextView audioRecvVolumeTv;
    private TextView audioSendVolumeTv;

    // Video
    private TextView cameraRecvBitrateTv;
    private TextView cameraSendBitrateTv;
    private TextView cameraRecvFpsTv;
    private TextView cameraSendFpsTv;
    private TextView cameraRecvResolutionTv;
    private TextView cameraSendResolutionTv;
    private TextView cameraRecvHwTv;
    private TextView cameraSendHwTv;
    private TextView cameraRenderFpsTv;
    private TextView cameraCaptureFpsTv;
    private TextView cameraSendDropFpsTv;

    // ScreenShare
    private TextView screenRecvBitrateTv;
    private TextView screenSendBitrateTv;
    private TextView screenRecvFpsTv;
    private TextView screenSendFpsTv;
    private TextView screenRecvResolutionTv;
    private TextView screenSendResolutionTv;
    private TextView screenRecvHwTv;
    private TextView screenSendHwTv;
    private TextView screenRenderFpsTv;
    private TextView screenCaptureFpsTv;
    private TextView screenSendDropFpsTv;

    public QualityReportDialog(@NonNull Context context) {
        super(context);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.quality_report_dialog);

        networkTypeTv = findViewById(R.id.network_type);
        signalLevelTv = findViewById(R.id.signal_level);
        lossRateTv = findViewById(R.id.loss_rate);
        rttMsTv = findViewById(R.id.rtt_ms);
        networkRecvBitrateTv = findViewById(R.id.recv_bitrate);
        networkSendBitrateTv = findViewById(R.id.send_bitrate);

        audioRecvBitrateTv = findViewById(R.id.audio_recv_bitrate);
        audioSendBitrateTv = findViewById(R.id.audio_send_bitrate);
        audioRecvVolumeTv = findViewById(R.id.audio_recv_volume);
        audioSendVolumeTv = findViewById(R.id.audio_send_volume);

        cameraRecvBitrateTv = findViewById(R.id.camera_recv_bitrate);
        cameraSendBitrateTv = findViewById(R.id.camera_send_bitrate);
        cameraRecvFpsTv = findViewById(R.id.camera_recv_fps);
        cameraSendFpsTv = findViewById(R.id.camera_send_fps);
        cameraRecvResolutionTv = findViewById(R.id.camera_recv_resolution);
        cameraSendResolutionTv = findViewById(R.id.camera_send_resolution);
        cameraRecvHwTv = findViewById(R.id.camera_recv_hw);
        cameraSendHwTv = findViewById(R.id.camera_send_hw);
        cameraRenderFpsTv = findViewById(R.id.camera_render_fps);
        cameraCaptureFpsTv = findViewById(R.id.camera_capture_fps);
        cameraSendDropFpsTv = findViewById(R.id.camera_send_drop_fps);

        screenRecvBitrateTv = findViewById(R.id.screen_recv_bitrate);
        screenSendBitrateTv = findViewById(R.id.screen_send_bitrate);
        screenRecvFpsTv = findViewById(R.id.screen_recv_fps);
        screenSendFpsTv = findViewById(R.id.screen_send_fps);
        screenRecvResolutionTv = findViewById(R.id.screen_recv_resolution);
        screenSendResolutionTv = findViewById(R.id.screen_send_resolution);
        screenRecvHwTv = findViewById(R.id.screen_recv_hw);
        screenSendHwTv = findViewById(R.id.screen_send_hw);
        screenRenderFpsTv = findViewById(R.id.screen_render_fps);
        screenCaptureFpsTv = findViewById(R.id.screen_capture_fps);
        screenSendDropFpsTv = findViewById(R.id.screen_send_drop_fps);
    }

    public void updateQualityReport(DingRtcEngine.DingRtcQualityReport report) {
        // network
        networkTypeTv.setText("网络类型：" + report.networkType);
        signalLevelTv.setText("信号强度：" + report.signalLevel);
        lossRateTv.setText("LossRate：" + report.lossRate);
        rttMsTv.setText("RttMs：" + report.rttMs);
        networkRecvBitrateTv.setText("RecvBitrate：" + toKbps(report.recvBitrate));
        networkSendBitrateTv.setText("SendBitrate：" + toKbps(report.sendBitrate));

        // Audio
        audioRecvBitrateTv.setText("RecvBitrate：" + toKbps(report.audioRecvBitrate));
        audioSendBitrateTv.setText("SendBitrate：" + toKbps(report.audioSendBitrate));
        audioRecvVolumeTv.setText("RecvVolume：" + report.audioRecvVolume);
        audioSendVolumeTv.setText("SendVolume：" + report.audioSendVolume);

        // Video
        cameraRecvBitrateTv.setText("RecvBitrate：" + toKbps(report.cameraRecvBitrate));
        cameraSendBitrateTv.setText("SendBitrate：" + toKbps(report.cameraSendBitrate));
        cameraRecvFpsTv.setText("RecvFps：" + report.cameraRecvFps);
        cameraSendFpsTv.setText("SendFps：" + report.cameraSendFps);
        cameraRecvResolutionTv.setText("接收分辨率：" + report.cameraRecvWidth + "x" + report.cameraRecvHeight);
        cameraSendResolutionTv.setText("发送分辨率：" + report.cameraSendWidth + "x" + report.cameraSendHeight);
        cameraRecvHwTv.setText("接收端编码：" + ((report.cameraRecvHw == 1) ? "硬解" : "软解"));
        cameraSendHwTv.setText("发送端编码：" + ((report.cameraSendHw == 1) ? "硬编" : "软编"));
        cameraRenderFpsTv.setText("RenderFps：" + report.cameraRecvRenderFps);
        cameraCaptureFpsTv.setText("CaptureFps：" + report.cameraSendCaptureFps);
        cameraSendDropFpsTv.setText("SendDropFps：" + report.cameraSendDropFps);

        // ScreenShare
        screenRecvBitrateTv.setText("RecvBitrate：" + toKbps(report.screenshareRecvBitrate));
        screenSendBitrateTv.setText("SendBitrate：" + toKbps(report.screenshareSendBitrate));
        screenRecvFpsTv.setText("RecvFps：" + report.screenshareRecvFps);
        screenSendFpsTv.setText("SendFps：" + report.screenshareSendFps);
        screenRecvResolutionTv.setText("接收分辨率：" + report.screenshareRecvWidth + "x" + report.screenshareRecvHeight);
        screenSendResolutionTv.setText("发送分辨率：" + report.screenshareSendWidth + "x" + report.screenshareSendHeight);
        screenRecvHwTv.setText("接收端编码：" + ((report.screenshareRecvHw == 1) ? "硬解" : "软解"));
        screenSendHwTv.setText("发送端编码：" + ((report.screenshareSendHw == 1) ? "硬编" : "软编"));
        screenRenderFpsTv.setText("RenderFps：" + report.screenshareRecvRenderFps);
        screenCaptureFpsTv.setText("CaptureFps：" + report.screenshareSendCaptureFps);
        screenSendDropFpsTv.setText("SendDropFps：" + report.screenshareSendDropFps);
    }

    private String toKbps(int bitrate) {
        return bitrate / 1000 + "kbps";
    }
}
