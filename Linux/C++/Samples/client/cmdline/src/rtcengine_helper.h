#ifndef __RTCENGINE_HELPER_H__
#define __RTCENGINE_HELPER_H__

#include <string>

#include "engine_types.h"

using namespace ding::rtc;

bool RequestAuthInfo(const std::string &appServer,
                     const std::string &appId,
                     const std::string &userId,
                     const std::string &userName,
                     const std::string &channelName,
                     const std::string &env,
                     const std::string &passwd,
                     RtcEngineAuthInfo &authInfo);

std::string LocalAudioStatsToString(const LocalAudioStats& localAudioStats);
std::string LocalVideoStatsToString(const LocalVideoStats& localVideoStats);
std::string RemoteAudioStatsToString(const RemoteAudioStats& remoteAudioStats);
std::string RemoteVideoStatsToString(const RemoteVideoStats& remoteVideoStats);
std::string RtcEngineStatsToString(const RtcEngineStats& stats);

const char* to_string(RtcEngineUserOfflineReason reason);
const char* to_string(RtcEngineVideoTrack videoTrack);
const char* to_string(RtcEnginePublishState state);
const char* to_string(RtcEngineSubscribeState state);

#endif // __RTCENGINE_HELPER_H__
