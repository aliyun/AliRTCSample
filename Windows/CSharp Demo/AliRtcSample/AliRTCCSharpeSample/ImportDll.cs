using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.IO;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace AliRtcSample
{
    class ImportDll
    {
        // 回调函数
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void setJoinResultCB(int result, string channel, int elapsed);
        public static setJoinResultCB joinResultCB;

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void setRemoteUserOnLineNotifyCB(string uid, int elapsed, string channel);
        public static setRemoteUserOnLineNotifyCB RemoteUserOnLineNotifyCB;

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void setRemoteUserOffLineNotifyCB(string uid, AliRTCUserOfflineReason reason, string channel);
        public static setRemoteUserOffLineNotifyCB RemoteUserOffLineNotifyCB;

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void setRemoteTrackAvailableNotifyCB(string strUid, AliRTCAudioTrack audioTrack, AliRTCVideoTrack videoTrack, string channel);
        public static setRemoteTrackAvailableNotifyCB RemoteTrackAvailableNotifyCB;

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void setByeCB(int code, string channel);
        public static setByeCB ByeCB;

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void setOccurErrorCB(int error,string msg, string channel);
        public static setOccurErrorCB OccurErrorCB;


        // 接口函数
        [DllImport("AliRTCSdkCWrapper.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)] // 初始化sdk
        public static extern void AliRtcCreateEngine(string extras);

        [DllImport("AliRTCSdkCWrapper.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)] // 释放sdk
        public static extern void AliRtcDestroyEngine();

        [DllImport("AliRTCSdkCWrapper.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)] // 为本地预览设置窗口以及绘制参数
        public static extern int AliRtcSetLocalViewConfig(IntPtr displayView, int backgroundColor, int renderMode, int mirrorMode, int scaleMode, int rotation, long renderId, int width, int height, bool enableGL, AliRTCVideoTrack track);

        [DllImport("AliRTCSdkCWrapper.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        public static extern int AliRtcStartPreview();

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 加入房间回调
        public static extern void AliRtcSetEventOnJoinChannelNotify(setJoinResultCB onJoinChannelNotify);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 加入房间
        public static extern int AliRtcJoinChannel(string channel, string userId, string appId,
                                                    string nonce, string token, string role, string gslbArray,
                                                    string agentArray, long timestamp, string userName);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 设置日志等级
        public static extern void AliRtcSetLogLevel(AliRTClogLevel logLevel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 设置H5兼容模式
        public static extern void AliRtcSetH5CompatibleMode(bool bEnablel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] 
        public static extern int AliRtcPublishLocalVideoStream(bool enabled, string channel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int AliRtcPublishLocalAudioStream(bool enabled, string channel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int AliRtcSubscribeAllRemoteAudioStreams(bool sub, string channel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int AliRtcSubscribeAllRemoteVideoStreams(bool sub, string channel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 是否允许推送次要视频流
        public static extern int AliRtcPublishLocalDualStream(bool enabled, string channel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 远程入会通知回调
        public static extern void AliRtcSetEventOnRemoteUserOnLineNotify(setRemoteUserOnLineNotifyCB onRemoteUserOnLineNotify);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 远程离会通知回调
        public static extern void AliRtcSetEventOnRemoteUserOffLineNotify(setRemoteUserOffLineNotifyCB onRemoteUserOffLineNotify);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 订阅音频流
        public static extern bool AliRtcSubscribeRemoteAudioStream(string pUid, bool bEnable, string channel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 订阅视频流
        public static extern bool AliRtcSubscribeRemoteVideoStream(string pUid, AliRTCVideoTrack track, bool sub, string channel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 远端流改变回调
        public static extern void AliRtcSetEventOnRemoteTrackAvailableNotify(setRemoteTrackAvailableNotifyCB onRemoteTrackAvailableNotify);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int AliRtcMuteLocalCamera(bool mute, AliRTCVideoTrack track, string channel);     //是否将停止本地视频采集

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 订阅视频流
        public static extern int AliRtcSetRemoteViewConfig(IntPtr displayView, int backgroundColor, int renderMode, int mirrorMode, int scaleMode, int rotation, long renderId, int width, int height, bool enableGL, string userId, AliRTCVideoTrack track, string channel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] //被服务器踢出频道的消息
        public static extern void AliRtcSetEventOnByeNotify(setByeCB onByeNotify);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)]  //error信息回调
        public static extern void AliRtcSetEventOnOccurErrorNotify(setOccurErrorCB onOccurErrorNotify);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 开启屏幕共享
        public static extern bool AliRtcStartScreenShareByDesktopId(uint desktopId, bool isShareByRegion, float originX, float originY, float width, float height, string channel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 停止共享
        public static extern bool AliRtcStopScreenShare(string channel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 获取共享配置信息
        public static extern string AliRtcGetScreenShareSourceInfo(AliRTCScreenShareType sourceType, string channel);

        [DllImport("AliRTCSdkCWrapper.dll", CallingConvention = CallingConvention.Cdecl)] // 停止共享
        public static extern bool AliRtcIsScreenSharePublished(string channel);
    }
}
