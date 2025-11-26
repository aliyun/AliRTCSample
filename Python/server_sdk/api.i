%module (directors="1",threads="1") dingrtc
%{
#include "../alirtc/api/engine_conf.h"
#include "../alirtc/api/engine_utils.h"
#include "../alirtc/api/engine_types.h"
#include "../alirtc/api/engine_audio_mixing_manager.h"
#include "../alirtc/api/engine_device_manager.h"
#include "../alirtc/api/engine_rtm.h"
#include "../alirtc/api/engine_subtitle_manager.h"
#include "../alirtc/api/engine_wb_types.h"
#include "../alirtc/api/engine_wb_interface.h"
#include "../alirtc/api/engine_interface.h"
%}

%typemap(in) void * {
    $1 = (void *) PyBytes_AsString($input);
}

%typemap(out) void * ding::rtc::RtcEngineAudioFrame::buffer {
    int size = arg1->bytesPerSample * arg1->samples * arg1->channels;
    $result = PyBytes_FromStringAndSize((const char*)$1, size);
}

%typemap(out) void * ding::rtc::RtcEngineVideoFrame::data {
    /* 注意：对应video frame数据在内存中的存储格式，要求为YUV420P，
     * 即先y buffer，然后紧跟着u buffer，然后紧跟着v buffer。
     * 要求stride == width。
     */
    int ysize = arg1->offset[1] - arg1->offset[0];
    int size = ysize * 3 / 2;
    $result = PyBytes_FromStringAndSize((const char*)$1, size);
}

%typemap(out) void * ding::rtc::RtcEngineEncodedVideoFrame::bits {
    int size = arg1->length;
    $result = PyBytes_FromStringAndSize((const char*)$1, size);
}

%typemap(out) void * ding::rtc::RtmData::data {
    int size = arg1->size;
    $result = PyBytes_FromStringAndSize((const char*)$1, size);
}

%typemap(out) void * {
    $result = NULL;
}

%extend ding::rtc::RtcEngineVideoFrame {
    void set_stride(int i, int val) {
        if (i >= 0 && i < 3) $self->stride[i] = val;
    }

    int get_stride(int i) {
        if (i >= 0 && i < 3) return $self->stride[i];
        return 0;
    }

    void set_offset(int i, int val) {
        if (i >= 0 && i < 3) $self->offset[i] = val;
    }

    int get_offset(int i) {
        if (i >= 0 && i < 3) return $self->offset[i];
        return 0;
    }
}

%extend ding::rtc::RtcEngineAuthInfo {
    // Constructor with parameters
    RtcEngineAuthInfo(const char *appId,
                      const char *channelId,
                      const char *userId,
                      const char *token,
                      const char *gslbServer) {
        ding::rtc::RtcEngineAuthInfo* info = new ding::rtc::RtcEngineAuthInfo();
        info->appId = appId;
        info->channelId = channelId;
        info->userId = userId;
        info->token = token;
        info->gslbServer = gslbServer;
        return info;
    }
}

%extend ding::rtc::RtmClient {
    int JoinSessionEx(const char *sessionId) {
        ding::rtc::String sess(sessionId);
        return $self->JoinSession(sess);
    }

    int LeaveSessionEx(const char *sessionId) {
        ding::rtc::String sess(sessionId);
        return $self->LeaveSession(sess);
    }

    int SendDataEx(const char *sessionId, const char *toUid, const void *data, int size) {
        ding::rtc::String sess(sessionId);
        ding::rtc::String usrid(toUid);
        ding::rtc::RtmData rtmData;
        rtmData.data = (void*)data;
        rtmData.size = size;
        return $self->SendData(sess, usrid, rtmData);
    }

    int BroadcastDataEx(const char *sessionId, const void *data, int size) {
        ding::rtc::String sess(sessionId);
        ding::rtc::RtmData rtmData;
        rtmData.data = (void*)data;
        rtmData.size = size;
        return $self->BroadcastData(sess, rtmData);
    }
}

%feature("director") ding::rtc::RtcEngineEventListener;
%feature("director") ding::rtc::RtcEngineAudioFrameObserver;
%feature("director") ding::rtc::RtcEngineVideoFrameObserver;
%feature("director") ding::rtc::RtmEventListener;

%define linux
%define PYTHON_SWIG_CONVERT
%include "../alirtc/api/engine_conf.h"
%include "../alirtc/api/engine_utils.h"
%include "../alirtc/api/engine_types.h"
%include "../alirtc/api/engine_audio_mixing_manager.h"
// engine_device_manager.h causes runtime crash
#%include "../alirtc/api/engine_device_manager.h"
%include "../alirtc/api/engine_rtm.h"
%include "../alirtc/api/engine_subtitle_manager.h"
#%include "../alirtc/api/engine_wb_types.h"
#%include "../alirtc/api/engine_wb_interface.h"
%include "../alirtc/api/engine_interface.h"
%enddef PYTHON_SWIG_CONVERT
%enddef linux

