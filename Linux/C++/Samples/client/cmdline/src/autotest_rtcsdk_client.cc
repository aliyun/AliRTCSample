#include "autotest_rtcsdk_client.h"
#include "autotest_tcp_client.h"
#include "tlog/file_path.h"
#include "tlog/tlog.h"
#include "tlog/utils.h"

using json = nlohmann::json;
using namespace ding::rtc;

#define kJsonKeyId "id"
#define kJsonKeyMethod "method"
#define kJsonKeyParams "params"
#define kJsonKeyResult "result"
#define kJsonKeyDuration "duration"

#define KJsonKeyUserId "userID"
#define KJsonKeyDisplayName "displayName"
#define KJsonKeySessionId "sessionID"
#define KJsonKeyIsOnline "isOnline"
#define KJsonKeyIsCameraMirror "isCameraMirror"
#define KJsonKeyIsScreenMirror "isScreenMirror"
#define KJsonKeyMuteAudioPlaying "muteAudioPlaying"
#define KJsonKeyPreferCameraMaster "preferCameraMaster"
#define KJsonKeyHasCameraView "hasCameraView"
#define KJsonKeyHasScreenView "hasScreenView"
#define KJsonKeyHasAudio "hasAudio"
#define KJsonKeyHasCameraMaster "hasCameraMaster"
#define KJsonKeyHasCameraSlave "hasCameraSlave"
#define KJsonKeyHasScreenSharing "hasScreenSharing"
#define KJsonKeyRequestAudio "requestAudio"
#define KJsonKeyRequestCameraMaster "requestCameraMaster"
#define KJsonKeyRequestCameraSlave "requestCameraSlave"
#define KJsonKeyRequestScreenSharing "requestScreenSharing"
#define KJsonKeySubscribeAudio "subScribedAudio"
#define KJsonKeySubscribeCameraMaster "subScribedCameraMaster"
#define KJsonKeySubscribeCameraSlave "subScribedCamearSlave"
#define KJsonKeySubscribeScreenSharing "subScribedScreenSharing"

AutoTestRtcSdkClient::AutoTestRtcSdkClient(AutoTestTcpClient *tcp_client)
    : tcp_client_(tcp_client) {}
AutoTestRtcSdkClient::~AutoTestRtcSdkClient() {
    DestroyRtcEngine();
}

std::string AutoTestRtcSdkClient::OnRecvCommand(uint64_t invoke_id, const std::string &cmd) {
    TLOGI("OnRecvCommand, invoke_id: %lld, cmd: %s", invoke_id, cmd.c_str());
    std::string res;

    json j_cmd = json::parse(cmd, nullptr, false);
    if (j_cmd.is_discarded()) {
        TLOGE("parse json failed");
        return res;
    }

    if (!j_cmd.contains(kJsonKeyMethod)) {
        TLOGE("invalid json, missing method");
        return res;
    }

    std::string method = j_cmd[kJsonKeyMethod];
    json j_param = j_cmd[kJsonKeyParams];
    json j_result;

    j_result[kJsonKeyMethod] = method;
    j_result[kJsonKeyId] = (uint64_t)rtc_engine_;

    if (!is_static_method(method) && !rtc_engine_) {
        TLOGE("method need engine instance got null, return error result");
        j_result[kJsonKeyResult] = -1;
        return j_result.dump();
    }
    auto start = std::chrono::high_resolution_clock::now();
    if (method == "set_log_dir_path") {
        std::string log_dir_path;
        if (!rtc_log_dir_.empty()) {
            log_dir_path = rtc_log_dir_;
        } else {
            log_dir_path = j_param.value("path", "autotest_log");
        }
        TLOGI("set rtc engine log dir: %s", log_dir_path.c_str());
        int result = RtcEngine::SetLogDirPath(log_dir_path.c_str());
        char path[4096] = {0};
        if (RtcEngine::GetLogDirPath(path, sizeof(path))) {
            j_result["log_dir_path"] = std::string(path);
        }
        j_result[kJsonKeyResult] = result;
    } else if (method == "set_log_level") {
        int level = j_param.value("level", 0);
        int result = RtcEngine::SetLogLevel(RtcEngineLogLevel(level));
        j_result[kJsonKeyResult] = result;
    } else if (method == "get_error_description") {
        int err_code = j_param.value("error_code", 0);
        std::string result = RtcEngine::GetErrorDescription(err_code);
        j_result[kJsonKeyResult] = result;
    } else if (method == "create") {
        std::string extras = j_param.value("extras", "");
        assert(rtc_engine_ == nullptr);
        if (rtc_engine_ == nullptr) {
            rtc_engine_ = RtcEngine::Create(extras.c_str());
            rtc_engine_->SetEngineEventListener(this);
        }
        // update kJsonKeyId
        j_result[kJsonKeyId] = (uint64_t)rtc_engine_;
        j_result[kJsonKeyResult] = 1;
    } else if (method == "destroy") {
        external_source_pusher_.reset();
        DestroyRtcEngine();
        j_result[kJsonKeyResult] = "void";
    } else if (method == "get_sdk_version") {
        j_result[kJsonKeyResult] = std::string(RtcEngine::GetSDKVersion()) + "." + RtcEngine::GetSDKBuild();
    } else if (method == "join_channel") {
        std::string user_name = j_param.value("user_name", "");
        json j_auth_info = j_param["auth_info"];
        if (j_auth_info.is_null()) {
            TLOGF("missing auth_info");
        }

        RtcEngineAuthInfo auth_info;
        auth_info.userId = j_auth_info.value("user_id", "").c_str();
        auth_info.channelId = j_auth_info.value("channel_id", "").c_str();
        auth_info.token = j_auth_info.value("token", "").c_str();
        auth_info.appId = j_auth_info.value("app_id", "").c_str();
        if (j_auth_info.contains("gslb") && j_auth_info["gslb"].is_array()) {
            auth_info.gslbServer = std::string(j_auth_info["gslb"][0]).c_str();
        }
        int result = rtc_engine_->JoinChannel(auth_info, user_name.c_str());
        j_result[kJsonKeyResult] = result;
    } else if (method == "leave_channel") {
        if (external_source_pusher_) {
            external_source_pusher_->StopPushRawAuidoFile();
            external_source_pusher_->StopPushRawVideoFile();
        }
        int result = rtc_engine_->LeaveChannel();
        j_result[kJsonKeyResult] = result;
    } else if (method == "is_in_call") {
        bool result = rtc_engine_->IsInCall();
        j_result[kJsonKeyResult] = result;
    } else if (method == "set_local_view_config") {
        int result = -1;
        if (j_param.contains("video_canvas")) {
            json j_video_canvas = j_param["video_canvas"];
            RtcEngineVideoTrack track = RtcEngineVideoTrack(j_param.value("track", 0));
            RtcEngineVideoCanvas *canvas = nullptr;
            if (track == RtcEngineVideoTrackCamera) {
                canvas = &camera_canvas_;
            } else if (track == RtcEngineVideoTrackScreen) {
                canvas = &screen_canvas_;
            } else {
                TLOGE("Unknown video track: %d", track);
            }
            if (canvas) {
                canvas->displayView = nullptr;  // TODO: GetLocalCameraView();
                canvas->renderMode = RtcEngineRenderMode(j_video_canvas.value("render_mode", 0));
                canvas->mirrorMode = RtcEngineRenderMirrorMode(j_video_canvas.value("mirror_mode", 0));
                canvas->backgroundColor = j_video_canvas.value("background_color", 0);
                canvas->rotation = RtcEngineRotationMode(j_video_canvas.value("rotation", 0));
                canvas->toBeRemoved = j_video_canvas.value("to_be_removed", false);
                result = rtc_engine_->SetLocalViewConfig(*canvas, track);
            }
        }
        j_result[kJsonKeyResult] = result;

    } else if (method == "update_view_config") {
        int result = -1;
        if (j_param.contains("video_canvas")) {
            json j_video_canvas = j_param["video_canvas"];
            std::string user_id = j_param.value("user_id", "");
            RtcEngineVideoTrack track = RtcEngineVideoTrack(j_param.value("track", 0));
            uint64_t display_view = j_video_canvas.value("display_view", 0);  // j_video_canvas.value("render_id", 0);
            RtcEngineVideoCanvas *canvas = nullptr;
            if ((uint64_t)camera_canvas_.displayView == display_view) {
                canvas = &camera_canvas_;
            } else if ((uint64_t)screen_canvas_.displayView == display_view) {
                canvas = &screen_canvas_;
            } else {
                TLOGE("can't find canvas for display view: %llu", display_view);
            }
            if (canvas) {
                canvas->renderMode = RtcEngineRenderMode(j_video_canvas.value("render_mode", 0));
                canvas->mirrorMode = RtcEngineRenderMirrorMode(j_video_canvas.value("mirror_mode", 0));
                canvas->backgroundColor = j_video_canvas.value("background_color", 0);
                canvas->rotation = RtcEngineRotationMode(j_video_canvas.value("rotation", 0));
                canvas->toBeRemoved = j_video_canvas.value("to_be_removed", false);
                result = rtc_engine_->UpdateViewConfig(*canvas, user_id.c_str(), track);
            }
        }
        j_result[kJsonKeyResult] = result;

    } else if (method == "set_remote_view_config") {
        std::string user_id = j_param.value("user_id", "");
        RtcEngineVideoTrack track = RtcEngineVideoTrack(j_param.value("track", 0));
        RtcEngineVideoCanvas canvas;
        canvas.toBeRemoved = j_param.value("to_be_removed", false);
        canvas.displayView = nullptr;  // TODO: GetRemoteView(user_id, canvas.toBeRemoved);
        canvas.renderMode = RtcEngineRenderMode(j_param.value("render_mode", 0));
        canvas.mirrorMode = RtcEngineRenderMirrorMode(j_param.value("mirror_mode", 0));
        canvas.backgroundColor = j_param.value("background_color", 0);
        canvas.rotation = RtcEngineRotationMode(j_param.value("rotation", 0));
        int result = rtc_engine_->SetRemoteViewConfig(canvas, user_id.c_str(), track);
        j_result[kJsonKeyResult] = result;
    } else if (method == "start_preview") {
        int result = rtc_engine_->StartPreview();
        j_result[kJsonKeyResult] = result;
    } else if (method == "stop_preview") {
        int result = rtc_engine_->StopPreview();
        j_result[kJsonKeyResult] = result;
    } else if (method == "publish_local_audio_stream") {
        bool enable = j_param.value("enable", false);
        int result = rtc_engine_->PublishLocalAudioStream(enable);
        j_result[kJsonKeyResult] = result;

        if (!external_source_pusher_) {
            external_source_pusher_.reset(new ExternalSourcePusher(rtc_engine_));
        }
        if (external_source_pusher_) {
            if (enable) {
                RawAudioFileConfig audio_config;
                audio_config.file_path = GetResourceDir() + "/pcm/ChengDu_1ch_48000.pcm";
                audio_config.sample_rate = 48000;
                audio_config.channels = 1;
                audio_config.freq = 40;
                external_source_pusher_->StartPushRawAuidoFile(audio_config);
            } else {
                external_source_pusher_->StopPushRawAuidoFile();
            }
        }

    } else if (method == "is_local_audio_stream_published") {
        int result = rtc_engine_->IsLocalAudioStreamPublished();
        j_result[kJsonKeyResult] = result;
    } else if (method == "publish_local_video_stream") {
        bool enable = j_param.value("enable", false);
        int result = rtc_engine_->PublishLocalVideoStream(enable);
        j_result[kJsonKeyResult] = result;

        if (!external_source_pusher_) {
            external_source_pusher_.reset(new ExternalSourcePusher(rtc_engine_));
        }
        if (external_source_pusher_) {
            if (enable) {
                RawVideoFileConfig video_config;
                video_config.file_path = GetResourceDir() + "/yuv/720_1280.yuv";
                video_config.pixel_format = "I420";
                video_config.width = 720;
                video_config.height = 1280;
                video_config.fps = 25;
                external_source_pusher_->StartPushRawVideoFile(video_config);
            } else {
                external_source_pusher_->StopPushRawVideoFile();
            }
        }
    } else if (method == "is_local_video_stream_published") {
        int result = rtc_engine_->IsLocalVideoStreamPublished();
        j_result[kJsonKeyResult] = result;
    } else if (method == "subscribe_all_remote_audio_streams") {
        bool enable = j_param.value("enable", false);
        int result = rtc_engine_->SubscribeAllRemoteAudioStreams(enable);
        j_result[kJsonKeyResult] = result;
    } else if (method == "subscribe_all_remote_video_streams") {
        bool enable = j_param.value("enable", false);
        int result = rtc_engine_->SubscribeAllRemoteVideoStreams(enable);
        j_result[kJsonKeyResult] = result;
    } else if (method == "subscribe_remote_video_stream") {
        std::string user_id = j_param.value("user_id", "");
        bool enable = j_param.value("enable", false);
        RtcEngineVideoTrack track = RtcEngineVideoTrack(j_param.value("track", 0));
        int result = rtc_engine_->SubscribeRemoteVideoStream(user_id.c_str(), track, enable);
        j_result[kJsonKeyResult] = result;
    } else if (method == "subscribe_remote_audio_stream") {
        // onertc NOT support NOW
        // std::string user_id = j_param.value("user_id", "");
        // bool enable = j_param.value("enable", false);
        // int result = rtc_engine_->SubscribeRemoteAudioStream(user_id.c_str(), enable);
        // j_result[kJsonKeyResult] = result;
    } else if (method == "set_remote_default_video_stream_type") {
        RtcEngineVideoStreamType type = RtcEngineVideoStreamType(j_param.value("type", 0));
        int result = rtc_engine_->SetRemoteDefaultVideoStreamType(type);
        j_result[kJsonKeyResult] = result;
    } else if (method == "set_remote_video_stream_type") {
        std::string user_id = j_param.value("user_id", "");
        RtcEngineVideoStreamType type = RtcEngineVideoStreamType(j_param.value("type", 0));
        int result = rtc_engine_->SetRemoteVideoStreamType(user_id.c_str(), type);
        j_result[kJsonKeyResult] = result;
    } else if (method == "mute_local_mic") {
        bool mute = j_param.value("mute", false);
        int result = rtc_engine_->MuteLocalAudio(mute);
        j_result[kJsonKeyResult] = result;
    } else if (method == "mute_local_camera") {
        bool mute = j_param.value("mute", false);
        RtcEngineVideoTrack track = RtcEngineVideoTrack(j_param.value("track", 0));
        int result = rtc_engine_->MuteLocalVideo(mute, track);
        j_result[kJsonKeyResult] = result;
    } else if (method == "get_user_info") {
        std::string user_id = j_param.value("user_id", "");
        Dictionary dictionary;
        rtc_engine_->GetUserInfo(user_id.c_str(), dictionary);
        json j_user_info;
        j_user_info[KJsonKeyUserId] = dictionary.getStringValue(KJsonKeyUserId).c_str();
        j_user_info[KJsonKeyDisplayName] = dictionary.getStringValue(KJsonKeyDisplayName).c_str();
        j_user_info[KJsonKeySessionId] = dictionary.getStringValue(KJsonKeySessionId).c_str();
        j_user_info[KJsonKeyIsOnline] = dictionary.getBooleanValue(KJsonKeyIsOnline, false);
        j_user_info[KJsonKeyIsCameraMirror] = dictionary.getBooleanValue(KJsonKeyIsCameraMirror, false);
        j_user_info[KJsonKeyIsScreenMirror] = dictionary.getBooleanValue(KJsonKeyIsScreenMirror, false);
        j_user_info[KJsonKeyMuteAudioPlaying] = dictionary.getBooleanValue(KJsonKeyMuteAudioPlaying, false);
        j_user_info[KJsonKeyPreferCameraMaster] = dictionary.getBooleanValue(KJsonKeyPreferCameraMaster, false);
        j_user_info[KJsonKeyHasCameraView] = dictionary.getBooleanValue(KJsonKeyHasCameraView, false);
        j_user_info[KJsonKeyHasScreenView] = dictionary.getBooleanValue(KJsonKeyHasScreenView, false);
        j_user_info[KJsonKeyHasAudio] = dictionary.getBooleanValue(KJsonKeyHasAudio, false);
        j_user_info[KJsonKeyHasCameraMaster] = dictionary.getBooleanValue(KJsonKeyHasCameraMaster, false);
        j_user_info[KJsonKeyHasCameraSlave] = dictionary.getBooleanValue(KJsonKeyHasCameraSlave, false);
        j_user_info[KJsonKeyHasScreenSharing] = dictionary.getBooleanValue(KJsonKeyHasScreenSharing, false);
        j_user_info[KJsonKeyRequestAudio] = dictionary.getBooleanValue(KJsonKeyRequestAudio, false);
        j_user_info[KJsonKeyRequestCameraMaster] = dictionary.getBooleanValue(KJsonKeyRequestCameraMaster, false);
        j_user_info[KJsonKeyRequestCameraSlave] = dictionary.getBooleanValue(KJsonKeyRequestCameraSlave, false);
        j_user_info[KJsonKeyRequestScreenSharing] = dictionary.getBooleanValue(KJsonKeyRequestScreenSharing, false);
        j_user_info[KJsonKeySubscribeAudio] = dictionary.getBooleanValue(KJsonKeySubscribeAudio, false);
        j_user_info[KJsonKeySubscribeCameraMaster] = dictionary.getBooleanValue(KJsonKeySubscribeCameraMaster, false);
        j_user_info[KJsonKeySubscribeCameraSlave] = dictionary.getBooleanValue(KJsonKeySubscribeCameraSlave, false);
        j_user_info[KJsonKeySubscribeScreenSharing] = dictionary.getBooleanValue(KJsonKeySubscribeScreenSharing, false);

        j_result[kJsonKeyResult] = j_user_info;
    } else if (method == "get_online_remote_users") {
        StringArray userArray;
        rtc_engine_->GetOnlineRemoteUsers(userArray);
        json j_user_ids = json::array();
        for (int i = 0; i < userArray.size(); i++) {
            j_user_ids.push_back(userArray.at(i).c_str());
        }
        j_result[kJsonKeyResult] = j_user_ids;
    } else if (method == "is_user_online") {
        std::string user_id = j_param.value("user_id", "");
        bool result = rtc_engine_->IsUserOnline(user_id.c_str());
        j_result[kJsonKeyResult] = result;
    } else if (method == "get_current_connection_status") {
        int result = rtc_engine_->GetCurrentConnectionStatus();
        j_result[kJsonKeyResult] = result;
    } else if (method == "set_video_encoder_configuration") {
        json j_dimensions = j_param["dimensions"];
        RtcEngineVideoEncoderConfiguration configuration;
        configuration.dimensions = RtcEngineVideoDimensions(j_dimensions.value("width", 0), j_dimensions.value("height", 0));
        configuration.frameRate = j_param.value("frame_rate", 0);
        configuration.orientationMode = RtcEngineVideoEncoderOrientationMode(j_param.value("orientation_mode", 0));
        rtc_engine_->SetVideoEncoderConfiguration(configuration);
        j_result[kJsonKeyResult] = "void";
    } else if (method == "enable_local_video") {
        bool enable = j_param.value("enable", false);
        int result = rtc_engine_->EnableLocalVideo(enable);
        j_result[kJsonKeyResult] = result;
    } else if (method == "set_screen_share_encoder_configuration") {
        json j_dimensions = j_param["dimensions"];
        RtcEngineScreenShareEncoderConfiguration configuration;
        configuration.dimensions = RtcEngineVideoDimensions(j_dimensions.value("width", 0), j_dimensions.value("height", 0));
        configuration.frameRate = j_param.value("frame_rate", 0);
        rtc_engine_->SetScreenShareEncoderConfiguration(configuration);
        j_result[kJsonKeyResult] = "void";
    } else if (method == "is_screen_share_published") {
        bool result = rtc_engine_->IsScreenSharePublished();
        j_result[kJsonKeyResult] = result;
    } else if (method == "stop_screen_share") {
        int result = rtc_engine_->StopScreenShare();
        j_result[kJsonKeyResult] = result;
    } else if (method == "get_screen_share_source_info") {
        RtcEngineDeviceInfoList *deviceInfoList =
            rtc_engine_->GetScreenShareSourceInfo(RtcEngineScreenShareType(j_param.value("share_type", 0)));
        json j_device_list;
        if (deviceInfoList) {
            for (int i = 0; i < deviceInfoList->GetCount(); i++) {
                json j_device;
                j_device["source_id"] = deviceInfoList->GetDeviceInfo(i).deviceID.c_str();
                j_device["source_name"] = deviceInfoList->GetDeviceInfo(i).deviceName.c_str();
                j_device_list[i] = j_device;
            }
        }
        j_result[kJsonKeyResult] = j_device_list;
    } else if (method == "get_current_screen_share_source_id") {
        int result = rtc_engine_->GetCurrentScreenShareSourceId();
        j_result[kJsonKeyResult] = result;
    } else if (method == "get_current_screen_share_source_type") {
        int result = rtc_engine_->GetCurrentScreenShareSourceType();
        j_result[kJsonKeyResult] = result;
    } else if (method == "get_desktop_region") {
        // int result = rtc_engine_->GetDesktopRegion();
        // j_result[kJsonKeyResult] = result;
    } else if (method == "start_screen_share_by_desktop_id") {
        std::string desktop_id = j_param.value("desktop_id", "");
        RtcEngineScreenShareConfig config;
        config.isShareByRegion = j_param.value("is_share_by_region", false);
        config.shareRegion = {j_param.value("origin_x", 0.0f),
                              j_param.value("origin_y", 0.0f),
                              j_param.value("width", 0.0f),
                              j_param.value("height", 0.0f)};
        int result = rtc_engine_->StartScreenShareByDesktopId(desktop_id.c_str(), config);
        j_result[kJsonKeyResult] = result;
    } else if (method == "start_screen_share_by_window_id") {
        std::string window_id = j_param.value("window_id", "");
        json j_config = j_param["config"];
        json j_share_region = j_config["share_region"];

        RtcEngineScreenShareConfig config;
        config.isShareByRegion = j_config.value("is_share_by_region", false);
        config.shareRegion = {j_share_region.value("origin_x", 0.0f),
                              j_share_region.value("origin_y", 0.0f),
                              j_share_region.value("width", 0.0f),
                              j_share_region.value("height", 0.0f)};
        int result = rtc_engine_->StartScreenShareByWindowId(window_id.c_str(), config);
        j_result[kJsonKeyResult] = result;
    } else if (method == "get_camera_list") {
        RtcEngineDeviceInfoList *camera_list = rtc_engine_->GetVideoDeviceManager()->GetCameraList();
        json j_camera_list;
        for (int i = 0; i < camera_list->GetCount(); i++) {
            json j_camera;
            j_camera["device_id"] = camera_list->GetDeviceInfo(i).deviceID.c_str();
            j_camera["device_name"] = camera_list->GetDeviceInfo(i).deviceName.c_str();
            j_camera_list[i] = j_camera;
        }
        j_result[kJsonKeyResult] = j_camera_list;
    } else if (method == "get_current_camera_name") {
        ding::rtc::String camera_name = rtc_engine_->GetVideoDeviceManager()->GetCurrentCameraName();
        j_result[kJsonKeyResult] = camera_name.c_str();
    } else if (method == "get_current_camera_id") {
        ding::rtc::String camera_id = rtc_engine_->GetVideoDeviceManager()->GetCurrentCameraID();
        j_result[kJsonKeyResult] = camera_id.c_str();
    } else if (method == "set_current_camera_name") {
        std::string camera_name = j_param.value("camera_name", "");
        rtc_engine_->GetVideoDeviceManager()->SetCurrentCameraName(camera_name.c_str());
        j_result[kJsonKeyResult] = "void";
    } else if (method == "set_current_camera_id") {
        std::string camera_id = j_param.value("camera_id", "");
        rtc_engine_->GetVideoDeviceManager()->SetCurrentCameraID(camera_id.c_str());
        j_result[kJsonKeyResult] = "void";
    } else if (method == "switch_camera") {
        j_result[kJsonKeyResult] = 0;
    } else if (method == "set_device_orientation_mode") {
        j_result[kJsonKeyResult] = "void";
    } else if (method == "enable_speakerphone") {
        j_result[kJsonKeyResult] = 0;
    } else if (method == "is_speakerphone_enabled") {
        j_result[kJsonKeyResult] = false;
    } else if (method == "get_recording_device_list") {
        RtcEngineDeviceInfoList *device_list = rtc_engine_->GetAudioDeviceManager()->GetRecordingDeviceList();
        json j_device_list;
        for (int i = 0; i < device_list->GetCount(); i++) {
            json j_device;
            j_device["device_id"] = device_list->GetDeviceInfo(i).deviceID.c_str();
            j_device["device_name"] = device_list->GetDeviceInfo(i).deviceName.c_str();
            j_device_list[i] = j_device;
        }
        j_result[kJsonKeyResult] = j_device_list;
    } else if (method == "get_playout_device_list") {
        RtcEngineDeviceInfoList *device_list = rtc_engine_->GetAudioDeviceManager()->GetPlayoutDeviceList();
        json j_device_list;
        for (int i = 0; i < device_list->GetCount(); i++) {
            json j_device;
            j_device["device_id"] = device_list->GetDeviceInfo(i).deviceID.c_str();
            j_device["device_name"] = device_list->GetDeviceInfo(i).deviceName.c_str();
            j_device_list[i] = j_device;
        }
        j_result[kJsonKeyResult] = j_device_list;
    } else if (method == "get_current_playout_device_id") {
        ding::rtc::String device_id = rtc_engine_->GetAudioDeviceManager()->GetCurrentPlayoutDeviceId();
        j_result[kJsonKeyResult] = device_id.c_str();
    } else if (method == "set_current_playout_device_id") {
        std::string device_id = j_param.value("device_id", "");
        rtc_engine_->GetAudioDeviceManager()->SetCurrentPlayoutDeviceId(device_id.c_str());
        j_result[kJsonKeyResult] = "void";
    } else if (method == "get_current_recording_device_id") {
        ding::rtc::String device_id = rtc_engine_->GetAudioDeviceManager()->GetCurrentRecordingDeviceId();
        j_result[kJsonKeyResult] = device_id.c_str();
    } else if (method == "set_current_recording_device_id") {
        std::string device_id = j_param.value("device_id", "");
        int result = rtc_engine_->GetAudioDeviceManager()->SetCurrentRecordingDeviceId(device_id.c_str());
        j_result[kJsonKeyResult] = "void";
    } else if (method == "set_recording_device_volume") {
        int result = rtc_engine_->GetAudioDeviceManager()->SetRecordingDeviceVolume(j_param.value("volume", -1));
        j_result[kJsonKeyResult] = result;
    } else if (method == "get_recording_device_volume") {
        int result = rtc_engine_->GetAudioDeviceManager()->GetRecordingDeviceVolume();
        j_result[kJsonKeyResult] = result;
    } else if (method == "set_playout_device_volume") {
        int result = rtc_engine_->GetAudioDeviceManager()->SetPlayoutDeviceVolume(j_param.value("volume", -1));
        j_result[kJsonKeyResult] = result;
    } else if (method == "get_playout_device_volume") {
        int result = rtc_engine_->GetAudioDeviceManager()->GetPlayoutDeviceVolume();
        j_result[kJsonKeyResult] = result;
    } else if (method == "set_recording_device_mute") {
        int result = rtc_engine_->GetAudioDeviceManager()->SetRecordingDeviceMute(j_param.value("mute", false));
        j_result[kJsonKeyResult] = result;
    } else if (method == "get_recording_device_mute") {
        bool result = rtc_engine_->GetAudioDeviceManager()->GetRecordingDeviceMute();
        j_result[kJsonKeyResult] = result;
    } else if (method == "set_playout_device_mute") {
        int result = rtc_engine_->GetAudioDeviceManager()->SetPlayoutDeviceMute(j_param.value("mute", false));
        j_result[kJsonKeyResult] = result;
    } else if (method == "get_playout_device_mute") {
        bool result = rtc_engine_->GetAudioDeviceManager()->GetPlayoutDeviceMute();
        j_result[kJsonKeyResult] = result;
    } else if (method == "start_recording_device_test") {
        int result = rtc_engine_->GetAudioDeviceManager()->StartRecordingDeviceTest();
        j_result[kJsonKeyResult] = result;
    } else if (method == "stop_recording_device_test") {
        int result = rtc_engine_->GetAudioDeviceManager()->StopRecordingDeviceTest();
        j_result[kJsonKeyResult] = result;
    } else if (method == "start_playout_device_test") {
        std::string wave_file_path;
        int channels = 0;
        int samplerate = 0;
        int result =
            rtc_engine_->GetAudioDeviceManager()->StartPlayoutDeviceTest(wave_file_path.c_str());
        j_result[kJsonKeyResult] = result;
    } else if (method == "stop_playout_device_test") {
        int result = rtc_engine_->GetAudioDeviceManager()->StopPlayoutDeviceTest();
        j_result[kJsonKeyResult] = result;
    } else if (method == "enable_audio_lever_observer") {
        int result = rtc_engine_->GetAudioDeviceManager()->SetPlayoutDeviceVolume(j_param.value("volume", 0));
        j_result[kJsonKeyResult] = result;
    } else if (method == "start_send_stats_data") {
        RtcStatsType type = RtcStatsType(j_param.value("type", 0));
        set_flag(send_stats_flag_, (int)type, true);
        j_result[kJsonKeyResult] = "void";
    } else if (method == "stop_send_stats_data") {
        RtcStatsType type = RtcStatsType(j_param.value("type", 0));
        set_flag(send_stats_flag_, (int)type, false);
        j_result[kJsonKeyResult] = "void";
    } else if (method == "client_upload_log") {
        // TODO: to implement upload log
        j_result[kJsonKeyResult] = 0;
    } else {
        TLOGE("unknown method: %s", method.c_str());
        j_result[kJsonKeyResult] = -1;
    }

    auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	j_result[kJsonKeyDuration] = duration.count();

    return j_result.dump();
}

void AutoTestRtcSdkClient::SendEventToServer(const char *method, const json &j_params) {
    json j_event;
    j_event[kJsonKeyId] = (uint64_t)rtc_engine_;
    j_event[kJsonKeyMethod] = method;
    j_event[kJsonKeyParams] = j_params;
    if (tcp_client_) {
        tcp_client_->SendEventMsg(j_event.dump());
    }
}

// --- implements RtcEngineEventListener ---
void AutoTestRtcSdkClient::OnJoinChannelResult(int result, const char *channel, const char *userId, int elapsed) {
    TLOGI("OnJoinChannelResult: result: %d, channel: %s, userId: %s, elapsed: %d", result, channel, userId, elapsed);
    json j_params;
    j_params["result"] = result;
    j_params["channel"] = channel;
    j_params["uid"] = userId;
    j_params["elapsed"] = elapsed;
    SendEventToServer("on_join_result", j_params);
}
void AutoTestRtcSdkClient::OnLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats) {
    TLOGI("OnLeaveChannelResult: result: %d", result);
    json j_params;
    j_params["result"] = result;
    SendEventToServer("on_leave_result", j_params);
}
void AutoTestRtcSdkClient::OnChannelRemainingTimeNotify(int remainingTimeInSec) {
    TLOGI("OnChannelRemainingTimeNotify, remainingTimeInSec: %d", remainingTimeInSec);
    json j_params;
    j_params["remain_time_in_sec"] = remainingTimeInSec;
    SendEventToServer("on_channel_remain_time_notify", j_params);
}
void AutoTestRtcSdkClient::OnRemoteUserOnLineNotify(const char *uid, int elapsed) {
    TLOGI("OnRemoteUserOnLineNotify, uid: %s, elapsed = %d", uid, elapsed);
    json j_params;
	j_params["user_id"] = uid;
	j_params["elapsed"] = elapsed;
	SendEventToServer("on_remote_user_online_notify", j_params);
}
void AutoTestRtcSdkClient::OnRemoteUserOffLineNotify(const char *uid, ding::rtc::RtcEngineUserOfflineReason reason) {
    TLOGI("OnRemoteUserOffLineNotify, uid = %d, reason = %d", uid, reason);
    json j_params;
	j_params["user_id"] = uid;
	j_params["reason"] = reason;
	SendEventToServer("on_remote_user_offline_notify", j_params);
}
void AutoTestRtcSdkClient::OnBye(ding::rtc::RtcEngineOnByeType code) {
    TLOGI("OnBye: code = %d", code);
    json j_params;
	j_params["code"] = code;
	SendEventToServer("on_bye", j_params);
}
void AutoTestRtcSdkClient::OnConnectionStatusChanged(ding::rtc::RtcEngineConnectionStatus status,
                                                  ding::rtc::RtcEngineConnectionStatusChangeReason reason) {
    TLOGI("OnConnectionStatusChange: status = %d, reason = %d", status, reason);
    json j_params;
    j_params["status"] = status;
    SendEventToServer("on_connection_status_change", j_params);
}
void AutoTestRtcSdkClient::OnNetworkQualityChanged(const char *uid,
                                                ding::rtc::RtcEngineNetworkQuality upQuality,
                                                ding::rtc::RtcEngineNetworkQuality downQuality) {
    TLOGI("OnNetworkQualityChanged: uid = %s, up = %d, down = %d", uid, upQuality, downQuality);
}
void AutoTestRtcSdkClient::OnStatisticsLog(ding::rtc::String const &log) {
    TLOGI("OnStatisticsLog: log = %s", log.c_str());
}
void AutoTestRtcSdkClient::OnOccurWarning(int warn, const char *msg) {
    TLOGI("OnOccurWarning: warn = %d, msg = %s", warn, msg);
}
void AutoTestRtcSdkClient::OnOccurError(int error, const char *msg) {
    TLOGI("OnOccurError: error = %d, msg = %s", error, msg);
}
void AutoTestRtcSdkClient::OnStatisticsLevelLog(int level, const ding::rtc::String &log) {
    TLOGI("OnStatisticsLevelLog: level = %d, log = %s", level, log.c_str());
}
void AutoTestRtcSdkClient::OnQualityReport(const ding::rtc::QualityReport &quality) {
    TLOGI("OnQualityReport: quality = %d", quality);
}
void AutoTestRtcSdkClient::OnLocalAudioStats(const ding::rtc::LocalAudioStats &localAudioStats) {
    TLOGI("OnLocalAudioStats");
    if (!get_flag(send_stats_flag_, (int)kLocalAudioStats)) {
        TLOGV("send LocalAudioStats not enabled");
        return;
    }
    json j_params;
    j_params["track"] = localAudioStats.track;
    j_params["num_channel"] = localAudioStats.numChannel;
    j_params["sent_samplerate"] = localAudioStats.sentSamplerate;
    j_params["sent_bitrate"] = localAudioStats.sentBitrate;
    SendEventToServer("on_rtc_local_audio_stats", j_params);
}
void AutoTestRtcSdkClient::OnLocalVideoStats(const ding::rtc::LocalVideoStats &localVideoStats) {
    TLOGI("OnLocalVideoStats");
    if (!get_flag(send_stats_flag_, (int)kLocalVideoStats)) {
        TLOGV("send LocalVideoStats not enabled");
        return;
    }
    json j_params;
    j_params["track"] = localVideoStats.track;
    j_params["target_encode_bitrate"] = localVideoStats.targetEncodeBitrate;
    j_params["actual_encode_bitrate"] = localVideoStats.actualEncodeBitrate;
    j_params["sent_bitrate"] = localVideoStats.sentBitrate;
    j_params["capture_fps"] = localVideoStats.captureFps;
    j_params["sent_fps"] = localVideoStats.sentFps;
    j_params["encode_fps"] = localVideoStats.encodeFps;
    j_params["avg_qp"] = localVideoStats.avgQpPerSec;
    SendEventToServer("on_rtc_local_video_stats", j_params);
}
void AutoTestRtcSdkClient::OnRemoteAudioStats(const ding::rtc::RemoteAudioStats &remoteAudioStats) {
    TLOGI("OnRemoteAudioStats");
    if (!get_flag(send_stats_flag_, (int)kRemoteAudioStats)) {
        TLOGV("send RemoteAudioStats not enabled");
        return;
    }
    json j_params;
    j_params["user_id"] = remoteAudioStats.userId.c_str();
    j_params["packet_loss_rate"] = remoteAudioStats.packetLossRate;
    j_params["recv_bitrate"] = remoteAudioStats.recvBitrate;
    j_params["total_frozen_times"] = remoteAudioStats.totalFrozenTime;
    SendEventToServer("on_rtc_remote_audio_stats", j_params);
}
void AutoTestRtcSdkClient::OnRemoteVideoStats(const ding::rtc::RemoteVideoStats &remoteVideoStats) {
    TLOGI("OnRemoteVideoStats");
    if (!get_flag(send_stats_flag_, (int)kRemoteVideoStats)) {
        TLOGV("send RemoteVideoStats not enabled");
        return;
    }
    json j_params;
    j_params["user_id"] = remoteVideoStats.userId.c_str();
    j_params["track"] = remoteVideoStats.track;
    j_params["width"] = remoteVideoStats.width;
    j_params["height"] = remoteVideoStats.height;
    j_params["decode_output_frame_rate"] = remoteVideoStats.decoderOutputFrameRate;
    j_params["render_output_frame_rate"] = remoteVideoStats.rendererOutputFrameRate;
    SendEventToServer("on_rtc_remote_video_stats", j_params);
}
void AutoTestRtcSdkClient::OnStats(const ding::rtc::RtcEngineStats &stats) {
    TLOGI("OnStats");
    if (!get_flag(send_stats_flag_, (int)kRtcEngineStats)) {
        return;
    }
    json j_params;
    j_params["tx_kbitrate"] = stats.txKBitrate;
    j_params["rx_kbitrate"] = stats.rxKBitrate;
    j_params["tx_bytes"] = (uint64_t)stats.txBytes;
    j_params["rx_bytes"] = (uint64_t)stats.rxBytes;
    j_params["rx_video_kbitrate"] = stats.rxVideoKBitrate;
    j_params["tx_video_kbitrate"] = stats.txVideoKBitrate;
    j_params["rx_audio_kbitrate"] = stats.rxAudioKBitrate;
    j_params["tx_audio_kbitrate"] = stats.txAudioKBitrate;
    j_params["duration"] = (uint64_t)stats.duration;
    j_params["app_cpu_rate"] = stats.appCpuRate;
    j_params["system_cpu_rate"] = stats.systemCpuRate;
    j_params["tx_packet_loss_rate"] = stats.txPacketLossRate;
    // j_params["sent_loss_pkts"] = stats.sent_loss_pkts;
    // j_params["sent_expected_pkts"] = stats.sent_expected_pkts;
    j_params["rx_recv_loss_rate"] = stats.rxRecvLossRate;
    j_params["rx_packet_loss_rate"] = stats.rxPacketLossRate;
    j_params["rx_lost_packets"] = (uint64_t)stats.rxLostPackets;
    j_params["rx_packets"] = (uint64_t)stats.rxPackets;
    j_params["lastmile_delay"] = stats.lastmileDelay;
    SendEventToServer("on_rtc_stats", j_params);
}
void AutoTestRtcSdkClient::OnAudioPublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                                   ding::rtc::RtcEnginePublishState newState,
                                                   int elapseSinceLastState,
                                                   const char *channel) {
    TLOGI("OnAudioPublishStateChanged: oldstate = %d, newstate = %d, channel = %s", oldState, newState, channel);
    json j_params;
    j_params["old_state"] = oldState;
    j_params["new_state"] = newState;
    j_params["elapse_since_last_state"] = elapseSinceLastState;
    SendEventToServer("on_audio_publish_state_changed", j_params);
}
void AutoTestRtcSdkClient::OnVideoPublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                                   ding::rtc::RtcEnginePublishState newState,
                                                   int elapseSinceLastState,
                                                   const char *channel) {
    TLOGI("OnVideoPublishStateChanged: oldstate = %d, newstate = %d, elapsed = %d, channel = %s",
          oldState,
          newState,
          elapseSinceLastState,
          channel);
    json j_params;
    j_params["old_state"] = oldState;
    j_params["new_state"] = newState;
    j_params["elapse_since_last_state"] = elapseSinceLastState;
    SendEventToServer("on_video_publish_state_changed", j_params);
}
void AutoTestRtcSdkClient::OnScreenSharePublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                                         ding::rtc::RtcEnginePublishState newState,
                                                         int elapseSinceLastState,
                                                         const char *channel) {
    TLOGI("OnScreenSharePublishStateChanged: oldstate = %d, newstate = %d, elapsed = %d, channel = %s",
          oldState,
          newState,
          elapseSinceLastState,
          channel);
    json j_params;
    j_params["old_state"] = oldState;
    j_params["new_state"] = newState;
    j_params["elapse_since_last_state"] = elapseSinceLastState;
    SendEventToServer("on_screen_share_publish_state_changed", j_params);
}
void AutoTestRtcSdkClient::OnRemoteTrackAvailableNotify(const char *uid,
                                                     ding::rtc::RtcEngineAudioTrack audioTrack,
                                                     ding::rtc::RtcEngineVideoTrack videoTrack) {
    TLOGI("OnRemoteTrackAvailableNotify: uid = %s, audioTrack = %d, videoTrack = %d", uid, audioTrack, videoTrack);

    json j_params;
	j_params["user_id"] = uid;
	j_params["audio_track"] = audioTrack;
	j_params["audio_track"] = videoTrack;
	SendEventToServer("on_remote_track_available_notify", j_params);
}
void AutoTestRtcSdkClient::OnUserAudioMuted(const char *uid, bool isMute) {
    TLOGI("OnUserAudioMuted: uid = %s, isMute = %d", uid, isMute);
    json j_params;
    j_params["user_id"] = uid;
    j_params["mute"] = isMute;
    SendEventToServer("on_user_audio_muted", j_params);
}
void AutoTestRtcSdkClient::OnUserVideoMuted(const char *uid, bool isMute, ding::rtc::RtcEngineVideoTrack videoTrack) {
    TLOGI("OnUserVideoMuted: uid = %s, isMute = %d, videoTrack = %d", uid, isMute, videoTrack);
    json j_params;
    j_params["user_id"] = uid;
    j_params["mute"] = isMute;
    j_params["track"] = videoTrack;
    SendEventToServer("on_user_video_muted", j_params);
}
void AutoTestRtcSdkClient::OnFirstRemoteVideoFrameDrawn(const char *uid,
                                                     ding::rtc::RtcEngineVideoTrack videoTrack,
                                                     int width,
                                                     int height,
                                                     int elapsed) {
    TLOGI("OnFirstRemoteVideoFrameDrawn: uid = %s, videoTrack = %d, width = %d, height = %d, elapsed = %d",
          uid,
          videoTrack,
          width,
          height,
          elapsed);

    json j_params;
	j_params["user_id"] = uid;
	j_params["video_track"] = videoTrack;
    j_params["width"] = width;
	j_params["height"] = height;
	j_params["elapsed"] = elapsed;
	SendEventToServer("on_first_remote_video_frame_drawn", j_params);
}
void AutoTestRtcSdkClient::OnFirstLocalVideoFrameDrawn(int width, int height, int elapsed) {
    TLOGI("OnFirstLocalVideoFrameDrawn:width = %d, height = %d, elapsed = %d", width, height, elapsed);

 	json j_params;
	j_params["width"] = width;
	j_params["height"] = height;
	j_params["elapsed"] = elapsed;
	SendEventToServer("on_first_local_video_frame_drawn", j_params);   
}
void AutoTestRtcSdkClient::OnFirstVideoPacketSend(ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost) {
    TLOGI("OnFirstVideoPacketSend:videoTrack = %d, time_cost = %d.", videoTrack, timeCost);

    json j_params;
	j_params["video_track"] = videoTrack;
	j_params["time_cost"] = timeCost;
	SendEventToServer("on_first_video_packet_send", j_params);
}
void AutoTestRtcSdkClient::OnFirstVideoPacketReceived(const char *uid,
                                                   ding::rtc::RtcEngineVideoTrack videoTrack,
                                                   int timeCost) {
	TLOGI("OnFirstVideoPacketReceived:uid = %s, videoTrack = %d, time_cost = %d", uid, videoTrack, timeCost);

	json j_params;
	j_params["user_id"] = uid;
	j_params["video_track"] = videoTrack;
	j_params["time_cost"] = timeCost;
	SendEventToServer("on_first_video_packet_received", j_params);
}
void AutoTestRtcSdkClient::OnFirstVideoFrameReceived(const char *uid,
                                                  ding::rtc::RtcEngineVideoTrack videoTrack,
                                                  int timeCost) {
    TLOGI("OnFirstVideoFrameReceived:videoTrack = %d, time_cost = %d.", uid, videoTrack, timeCost);

	json j_params;
	j_params["user_id"] = uid;
	j_params["video_track"] = videoTrack;
	j_params["time_cost"] = timeCost;
	SendEventToServer("on_first_video_frame_received", j_params);
}
void AutoTestRtcSdkClient::OnFirstAudioPacketSend(int timeCost) {
    TLOGI("OnFirstAudioPacketSend:time_cost = %d.", timeCost);

	json j_params;
	j_params["time_cost"] = timeCost;
	SendEventToServer("on_first_audio_packet_send", j_params);
}
void AutoTestRtcSdkClient::OnFirstAudioPacketReceived(const char *uid, int timeCost) {
    TLOGI("OnFirstAudioPacketReceived: user_id = %s, time_cost = %d.", uid, timeCost);

	json j_params;
	j_params["user_id"] = uid;
	j_params["time_cost"] = timeCost;
	SendEventToServer("on_first_audio_packet_received", j_params);
}
void AutoTestRtcSdkClient::OnAudioSubscribeStateChanged(const char *uid,
                                                     ding::rtc::RtcEngineSubscribeState oldState,
                                                     ding::rtc::RtcEngineSubscribeState newState,
                                                     int elapseSinceLastState,
                                                     const char *channel) {
    TLOGI("OnAudioSubscribeStateChanged: uid = %s, oldstate = %d, newstate = %d, elapsed = %d, channel = %s",
          uid,
          oldState,
          newState,
          elapseSinceLastState,
          channel);
    json j_params;
    j_params["user_id"] = uid;
    j_params["old_state"] = oldState;
    j_params["new_state"] = newState;
    j_params["elapse_since_last_state"] = elapseSinceLastState;
    SendEventToServer("on_audio_subscribe_state_changed", j_params);
}
void AutoTestRtcSdkClient::OnVideoSubscribeStateChanged(const char *uid,
                                                     ding::rtc::RtcEngineSubscribeState oldState,
                                                     ding::rtc::RtcEngineSubscribeState newState,
                                                     int elapseSinceLastState,
                                                     const char *channel) {
    TLOGI("OnVideoSubscribeStateChanged: uid = %s, oldstate = %d, newstate = %d, elapsed = %d, channel = %s",
          uid,
          oldState,
          newState,
          elapseSinceLastState,
          channel);
    json j_params;
    j_params["user_id"] = uid;
    j_params["old_state"] = oldState;
    j_params["new_state"] = newState;
    j_params["elapse_since_last_state"] = elapseSinceLastState;
    SendEventToServer("on_video_subscribe_state_changed", j_params);
}
void AutoTestRtcSdkClient::OnScreenShareSubscribeStateChanged(const char *uid,
                                                           ding::rtc::RtcEngineSubscribeState oldState,
                                                           ding::rtc::RtcEngineSubscribeState newState,
                                                           int elapseSinceLastState,
                                                           const char *channel) {
    TLOGI("OnScreenShareSubscribeStateChanged: uid = %s, oldstate = %d, newstate = %d, elapsed = %d, channel = %s",
          uid,
          oldState,
          newState,
          elapseSinceLastState,
          channel);
    json j_params;
    j_params["user_id"] = uid;
    j_params["old_state"] = oldState;
    j_params["new_state"] = newState;
    j_params["elapse_since_last_state"] = elapseSinceLastState;
    SendEventToServer("on_screen_share_subscribe_state_changed", j_params);
}
void AutoTestRtcSdkClient::OnSubscribeStreamTypeChanged(const char *uid,
                                                     ding::rtc::RtcEngineVideoStreamType oldStreamType,
                                                     ding::rtc::RtcEngineVideoStreamType newStreamType,
                                                     int elapseSinceLastState,
                                                     const char *channel) {
    TLOGI("OnSubscribeStreamTypeChanged: uid = %s, old = %d, new = %d, elapsed = %d, channel = %s",
          uid,
          oldStreamType,
          newStreamType,
          elapseSinceLastState,
          channel);
    json j_params;
    j_params["user_id"] = uid;
    j_params["old_stream_type"] = oldStreamType;
    j_params["new_stream_type"] = newStreamType;
    j_params["elapse_since_last_state"] = elapseSinceLastState;
    SendEventToServer("on_subscribe_stream_type_changed", j_params);
}
void AutoTestRtcSdkClient::OnRecordingDeviceAudioLevel(int level) {
    TLOGI("OnRecordingDeviceAudioLevel: level = %d", level);
    json j_params;
    j_params["level"] = level;
    SendEventToServer("on_audio_device_record_level", j_params);
}
void AutoTestRtcSdkClient::OnPlayoutDeviceAudioLevel(int level) {
    TLOGI("OnPlayoutDeviceAudioLevel: level = %d", level);
    json j_params;
    j_params["level"] = level;
    SendEventToServer("on_audio_device_playout_level", j_params);
}
void AutoTestRtcSdkClient::OnAudioVolumeIndication(const ding::rtc::AudioVolumeInfo *speakers,
                                                unsigned int speakerNumber) {
    TLOGI("OnAudioVolumeIndication: speaker number = %d.", speakerNumber);

	json j_params;
	j_params["speaker_number"] = speakerNumber;
    json j_array = json::array();
	for(int i=0;i<speakerNumber;i++) {
		json j_speaker;
		j_speaker["user_id"] = speakers[i].userId.c_str();
		j_speaker["volume"] = speakers[i].volume;
		j_speaker["speech_state"] = speakers[i].speechState;
		j_array.push_back(j_speaker);
	}
	j_params["speaker_list"] = j_array;

	SendEventToServer("on_audio_volume_indication", j_params);
}
void AutoTestRtcSdkClient::OnAudioDeviceStateChanged(const char *deviceId,
                                                  ding::rtc::RtcEngineDeviceType deviceType,
                                                  ding::rtc::RtcEngineDeviceState deviceState) {
    TLOGI("OnAudioDeviceStateChanged: deviceId = %s, deviceType = %d, deviceState = %d",
          deviceId,
          deviceType,
          deviceState);

    json j_params;
	j_params["device_id"] = deviceId;
	j_params["device_type"] = deviceType;
	j_params["device_state"] = deviceState;
	SendEventToServer("on_audio_device_state_changed", j_params);
}
void AutoTestRtcSdkClient::OnApiCalledExecuted(int error, const char *api, const char *result) {
    TLOGI("OnApiCalledExecuted, ");
}
void AutoTestRtcSdkClient::OnSnapshotComplete(const char *userId,
                                           ding::rtc::RtcEngineVideoTrack videoTrack,
                                           const char *filePath,
                                           int width,
                                           int height,
                                           bool success) {
    TLOGI("OnSnapshotComplete: user_id = %s, video_track = %d, file_path = %s, width = %d, height = %d, success = %d.", userId, videoTrack, filePath, width, height, success);
    
    json j_params;
	j_params["user_id"] = userId;
	j_params["video_track"] = videoTrack;
	j_params["file_path"] = filePath;
	j_params["width"] = width;
	j_params["height"] = height;
	j_params["success"] = success;
	SendEventToServer("on_snapshot_complete", j_params);
}

void AutoTestRtcSdkClient::DestroyRtcEngine() {
    if (rtc_engine_) {
        rtc_engine_->SetEngineEventListener(nullptr);
        RtcEngine::Destroy(rtc_engine_);
        rtc_engine_ = nullptr;
    }
}

std::string AutoTestRtcSdkClient::GetResourceDir() {
    tlog::FilePath exe_path(tlog::GetExecutablePath());
    // HARDCODE, resource dir: ${exe_dir}/../../../resources
    return exe_path.DirName().DirName().DirName().DirName().Append("resources").value();
}