using UnityEngine;
using System;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Collections;
using System.Collections.Generic;
using AOT;
using ali_unity_rtc;


namespace ali_unity_rtc
{
    public class IAliRtcEngine : IAliRtcEngineBase
    {
        protected const string AliRtcEngineCallBackObjectName = "AliRtcCallBackObject";
        protected static GameObject AliRtcEngineCallBackObject = null;

        public delegate void OnJoinChannelNotifyHandler(int errorCode);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnJoinChannelNotifyHandler OnJoinChannelNotify;
#else
        public OnJoinChannelNotifyHandler OnJoinChannelNotify;
#endif

        public delegate void OnPublishNotifyHandler(int errorCode);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnPublishNotifyHandler OnPublishNotify;
#else
        public OnPublishNotifyHandler OnPublishNotify;
#endif

        public delegate void OnSubscribeNotifyHandler(string userId, int videoTrack, int audioTrack);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnSubscribeNotifyHandler OnSubscribeNotify;
#else
        public OnSubscribeNotifyHandler OnSubscribeNotify;
#endif

        public delegate void OnRemoteUserOnLineNotifyHandler(string userId);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnRemoteUserOnLineNotifyHandler OnRemoteUserOnLineNotify;
#else
        public OnRemoteUserOnLineNotifyHandler OnRemoteUserOnLineNotify;
#endif

        public delegate void OnRemoteUserOffLineNotifyHandler(string userId);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnRemoteUserOffLineNotifyHandler OnRemoteUserOffLineNotify;
#else
        public OnRemoteUserOffLineNotifyHandler OnRemoteUserOffLineNotify;
#endif

        public delegate void OnRemoteTrackAvailableNotifyHandler(string userId, int audioTrack, int videoTrack);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnRemoteTrackAvailableNotifyHandler OnRemoteTrackAvailableNotify;
#else
        public OnRemoteTrackAvailableNotifyHandler OnRemoteTrackAvailableNotify;
#endif

        public delegate void OnSubscribeChangedNotifyHandler(string userId, int audioTrack, int videoTrack);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnSubscribeChangedNotifyHandler OnSubscribeChangedNotify;
#else
        public OnSubscribeChangedNotifyHandler OnSubscribeChangedNotify;
#endif

        public delegate void OnNetworkQualityChangedNotifyHandler(string userId, int upQuality, int downQuality);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnNetworkQualityChangedNotifyHandler OnNetworkQualityChangedNotify;
#else
        public OnNetworkQualityChangedNotifyHandler OnNetworkQualityChangedNotify;
#endif

        public delegate void OnConnectionLostNotifyHandler();
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnConnectionLostNotifyHandler OnConnectionLostNotify;
#else
        public OnConnectionLostNotifyHandler OnConnectionLostNotify;
#endif

        public delegate void OnTryToReconnectNotifyHandler();
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnTryToReconnectNotifyHandler OnTryToReconnectNotify;
#else
        public OnTryToReconnectNotifyHandler OnTryToReconnectNotify;
#endif

        public delegate void OnConnectionRecoveryNotifyHandler();
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnConnectionRecoveryNotifyHandler OnConnectionRecoveryNotify;
#else
        public OnConnectionRecoveryNotifyHandler OnConnectionRecoveryNotify;
#endif

        public delegate void OnByeNotifyHandler(int code);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnByeNotifyHandler OnByeNotify;
#else
        public OnByeNotifyHandler OnByeNotify;
#endif

        public delegate void OnOccurWarningNotifyHandler(int code);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnOccurWarningNotifyHandler OnOccurWarningNotify;
#else
        public OnOccurWarningNotifyHandler OnOccurWarningNotify;
#endif

        public delegate void OnOccurErrorNotifyHandler(int error);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnOccurErrorNotifyHandler OnOccurErrorNotify;
#else
        public OnOccurErrorNotifyHandler OnOccurErrorNotify;
#endif

        public delegate void OnLeaveChannelResultHandler(int result);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnLeaveChannelResultHandler OnLeaveChannelResultNotify;
#else
        public OnLeaveChannelResultHandler OnLeaveChannelResultNotify;
#endif

        public delegate void OnUpdateRoleNotifyHandler(int oldRole, int newRole);
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        public static OnUpdateRoleNotifyHandler OnUpdateRoleNotify;
#else
        public OnUpdateRoleNotifyHandler OnUpdateRoleNotify;
#endif

        [MonoPInvokeCallback(typeof(EngineEventOnJoinChannelNotifyHandler))]
        private static void OnJoinChanneNotifyCallback(int errorCode)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnJoinChannelNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnJoinChannelNotify(errorCode);
                });
            }
#else
            if (instance!= null && (instance.OnJoinChannelNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnJoinChannelNotify(errorCode);
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnPublishNotifyHandler))]
        private static void OnPublishNotifyCallback(int errorCode)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnPublishNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnPublishNotify(errorCode);
                });
            }
#else
            if ((instance.OnPublishNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnPublishNotify(errorCode);
                });
            }
#endif

        }

        [MonoPInvokeCallback(typeof(EngineEventOnSubscribeNotifyHandler))]
        private static void OnSubscribeNotifyCallback(string userId, int videoTrack, int audioTrack)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnSubscribeNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnSubscribeNotify(userId, videoTrack, audioTrack);
                });
            }
#else
            if ((instance.OnSubscribeNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnSubscribeNotify(userId, videoTrack, audioTrack);
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnRemoteUserOnLineNotifyHandler))]
        private static void OnRemoteUserOnLineNotifyCallback(string userId)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnRemoteUserOnLineNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnRemoteUserOnLineNotify(userId);
                });
            }
#else
            if ((instance.OnRemoteUserOnLineNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnRemoteUserOnLineNotify(userId);
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnRemoteUserOffLineNotifyHandler))]
        private static void OnRemoteUserOffLineNotifyCallback(string userId)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnRemoteUserOffLineNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnRemoteUserOffLineNotify(userId);
                });
            }
#else
            if ((instance.OnRemoteUserOffLineNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnRemoteUserOffLineNotify(userId);
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnRemoteTrackAvailableNotifyHandler))]
        private static void OnRemoteTrackAvailableNotifyCallback(string userId, int audioTrack, int videoTrack)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnRemoteTrackAvailableNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnRemoteTrackAvailableNotify(userId, audioTrack, videoTrack);
                });
            }
#else
            if ((instance.OnRemoteTrackAvailableNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnRemoteTrackAvailableNotify(userId, audioTrack, videoTrack);
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnSubscribeChangedNotifyHandler))]
        private static void OnSubscribeChangedNotifyCallback(string userId, int audioTrack, int videoTrack)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnSubscribeChangedNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnSubscribeChangedNotify(userId, audioTrack, videoTrack);
                });
            }
#else
            if ((instance.OnSubscribeChangedNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnSubscribeChangedNotify(userId, audioTrack, videoTrack);
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnNetworkQualityChangedNotifyHandler))]
        private static void OnNetworkQualityChangedNotifyCallback(string userId, int upQuality, int downQuality)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnNetworkQualityChangedNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnNetworkQualityChangedNotify(userId, upQuality, downQuality);
                });
            }
#else
            if ((instance.OnNetworkQualityChangedNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnNetworkQualityChangedNotify(userId, upQuality, downQuality);
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnConnectionLostNotifyHandler))]
        private static void OnConnectionLostNotifyCallback()
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnConnectionLostNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnConnectionLostNotify();
                });
            }
#else
            if ((instance.OnConnectionLostNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnConnectionLostNotify();
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnTryToReconnectNotifyHandler))]
        private static void OnTryToReconnectNotifyCallback()
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if (IAliRtcEngine.OnTryToReconnectNotify != null && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnTryToReconnectNotify();
                });
            }
#else
            if (instance.OnTryToReconnectNotify != null && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnTryToReconnectNotify();
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnConnectionRecoveryNotifyHandler))]
        private static void OnConnectionRecoveryNotifyCallback()
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if (IAliRtcEngine.OnConnectionRecoveryNotify != null && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnConnectionRecoveryNotify();
                });
            }
#else
            if (instance.OnConnectionRecoveryNotify != null && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnConnectionRecoveryNotify();
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnByeNotifyHandler))]
        private static void OnByeNotifyCallback(int code)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnByeNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnByeNotify(code);
                });
            }
#else
            if ((instance.OnByeNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnByeNotify(code);
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnOccurWarningNotifyHandler))]
        private static void OnOccurWarningNotifyCallback(int code)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnOccurWarningNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnOccurWarningNotify(code);
                });
            }
#else
            if ((instance.OnOccurWarningNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnOccurWarningNotify(code);
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnOccurErrorNotifyHandler))]
        private static void OnOccurErrorNotifyCallback(int error)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnOccurErrorNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnOccurErrorNotify(error);
                });
            }
#else
            if ((instance.OnOccurErrorNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnOccurErrorNotify(error);
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnLeaveChannelResultNotifyHandler))]
        private static void OnLeaveChannelResultNotifyCallback(int result)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnLeaveChannelResultNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnLeaveChannelResultNotify(result);
                });
            }
#else
            if ((instance.OnLeaveChannelResultNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnLeaveChannelResultNotify(result);
                });
            }
#endif
        }

        [MonoPInvokeCallback(typeof(EngineEventOnUpdateRoleNotifyHandler))]
        private static void OnUpdateRoleNotifyCallback(int oldRole, int newRole)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            if ((IAliRtcEngine.OnUpdateRoleNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    IAliRtcEngine.OnUpdateRoleNotify(oldRole, newRole);
                });
            }
#else
            if ((instance.OnUpdateRoleNotify != null) && AliRtcEngineCallBackQueue.Current != null)
            {
                AliRtcEngineCallBackQueue.Current.EnQueue(() =>
                {
                    instance.OnUpdateRoleNotify(oldRole, newRole);
                });
            }
#endif
        }

        private void InitGameObject()
        {
            AliRtcEngineCallBackObject = new GameObject(AliRtcEngineCallBackObjectName);
            AliRtcEngineCallBackObject.AddComponent<AliRtcEngineCallBackQueue>();
            GameObject.DontDestroyOnLoad(AliRtcEngineCallBackObject);
            AliRtcEngineCallBackObject.hideFlags = HideFlags.HideInHierarchy;
        }

        private static void DeInitGameObject()
        {
            GameObject agoraGameObject = GameObject.Find(AliRtcEngineCallBackObjectName);
            if (!ReferenceEquals(agoraGameObject, null))
            {
                AliRtcEngineCallBackQueue.Current.ClearQueue();
                GameObject.Destroy(agoraGameObject);
                agoraGameObject = null;
            }
        }

        private IAliRtcEngine(string extras)
        {
            InitGameObject();
            AliRtcCreateEngine(extras);
            AliRtcSetEventOnJoinChannelNotify(OnJoinChanneNotifyCallback);
            AliRtcSetEventOnPublishNotify(OnPublishNotifyCallback);
            AliRtcSetEventOnSubscribeNotify(OnSubscribeNotifyCallback);
            AliRtcSetEventOnRemoteUserOnLineNotify(OnRemoteUserOnLineNotifyCallback);
            AliRtcSetEventOnRemoteUserOffLineNotify(OnRemoteUserOffLineNotifyCallback);
            AliRtcSetEventOnRemoteTrackAvailableNotify(OnRemoteTrackAvailableNotifyCallback);
            AliRtcSetEventOnSubscribeChangedNotify(OnSubscribeChangedNotifyCallback);
            AliRtcSetEventOnNetworkQualityChangedNotify(OnNetworkQualityChangedNotifyCallback);
            AliRtcSetEventOnConnectionLostNotify(OnConnectionLostNotifyCallback);
            AliRtcSetEventOnTryToReconnectNotify(OnTryToReconnectNotifyCallback);
            AliRtcSetEventOnConnectionRecoveryNotify(OnConnectionRecoveryNotifyCallback);
            AliRtcSetEventOnByeNotify(OnByeNotifyCallback);
            AliRtcSetEventOnOccurWarningNotify(OnOccurWarningNotifyCallback);
            AliRtcSetEventOnOccurErrorNotify(OnOccurErrorNotifyCallback);
            AliRtcSetEventOnLeaveChannelResultNotify(OnLeaveChannelResultNotifyCallback);
            AliRtcSetEventOnUpdateRoleNotify(OnUpdateRoleNotifyCallback);
        }

        private class AliRtcEngineCallBackQueue : MonoBehaviour
        {
            private static Queue<Action> queue = new Queue<Action>();
            private static AliRtcEngineCallBackQueue _current;
            public static AliRtcEngineCallBackQueue Current
            {
                get
                {
                    return _current;
                }
            }

            public void ClearQueue()
            {
                lock (queue)
                {
                    queue.Clear();
                }
            }

            public void EnQueue(Action action)
            {
                lock (queue)
                {
                    if (queue.Count >= 100)
                    {
                        queue.Dequeue();
                    }
                    queue.Enqueue(action);
                }
            }

            private Action DeQueue()
            {
                lock (queue)
                {
                    Action action = queue.Dequeue();
                    return action;
                }
            }

            void Awake()
            {
                _current = this;
            }
            // Update is called once per frame
            void Update()
            {
                if (queue.Count > 0)
                {
                    Action action = DeQueue();
                    action();
                }
            }

            void OnDestroy()
            {
                _current = null;
            }
        }

        public static void SetH5CompatibleMode(bool comp)
        {
            AliRtcSetH5CompatibleMode(comp);
        }

        public static bool GetH5CompatibleMode()
        {
            return AliRtcGetH5CompatibleMode();
        }

        public void SetChannelProfile(int profile)
        {
            AliRtcSetChannelProfile(profile);
        }

        public void SetClientRole(int role)
        {
            AliRtcSetClientRole(role);
        }
        
        public static string GetSdkVersion()
        {
            return Marshal.PtrToStringAnsi(AliRtcGetSdkVersion());
        }

        public void SetAutoPublish(bool autoPub, bool autoSub)
        {
            AliRtcSetAutoPublish(autoPub, autoSub);
        }

        public void SetAudioOnlyMode(bool audioOnly)
        {
            AliRtcSetAudioOnlyMode(audioOnly);
        }

        public void SetLogLevel(int logLevel)
        {
            AliRtcSetLogLevel(logLevel);
        }

        public void JoinChannel(AliRTCAuthInfo authInfo, string userName)
        {
            AliRtcJoinChannel(authInfo, userName);
        }

        public void LeaveChannel()
        {
            AliRtcLeaveChannel();
        }

        public void StartPreview()
        {
            AliRtcStartPreview();
        }

        public void StopPreview()
        {
            AliRtcStopPreview();
        }

        public bool IsInCall()
        {
            return AliRtcIsInCall();
        }

        public bool IsCameraOn()
        {
            return AliRtcIsCameraOn();
        }

        public string GetOnlineRemoteUsers()
        {
            IntPtr userList = AliRtcGetOnlineRemoteUsers();
            return Marshal.PtrToStringAnsi(userList);
        }

        public bool IsAutoPublish()
        {
            return AliRtcIsAutoPublish();
        }

        public bool IsAutoSubscribe()
        {
            return AliRtcIsAutoSubscribe();
        }

        public bool IsLocalAudioPublishEnabled()
        {
            return AliRtcIsLocalAudioPublishEnabled();
        }

        public bool IsLocalCameraPublishEnabled()
        {
            return AliRtcIsLocalCameraPublishEnabled();
        }
        public bool IsLocalSimulcastEnabled()
        {
            return AliRtcIsLocalSimulcastEnabled();
        }

        public bool IsAudioOnly()
        {
            return AliRtcIsAudioOnly();
        }

        public void ConfigLocalAudioPublish(bool enable)
        {
            AliRtcConfigLocalAudioPublish(enable);
        }
        public void ConfigLocalCameraPublish(bool enable)
        {
            AliRtcConfigLocalCameraPublish(enable);
        }

        public void ConfigLocalSimulcast(bool enable, int track)
        {
            AliRtcConfigLocalSimulcast(enable, track);
        }

        public void ConfigRemoteAudio(string userId, bool enable)
        {
            AliRtcConfigRemoteAudio(userId, enable);
        }

        public void ConfigRemoteCameraTrack(string userId, bool master, bool enable)
        {
            AliRtcConfigRemoteCameraTrack(userId, master, enable);
        }

        public void ConfigRemoteScreenTrack(string userId, bool enable)
        {
            AliRtcConfigRemoteScreenTrack(userId, enable);
        }

        public void SetVideoProfile(int profile, int track)
        {
            AliRtcSetVideoProfile(profile, track);
        }

        public void MuteLocalCamera(bool mute, int track)
        {
            AliRtcMuteLocalCamera(mute, track);
        }

        public void MuteLocalMic(bool mute)
        {
            AliRtcMuteLocalMic(mute);
        }

        public int MuteRemoteAudioPlaying(string userId, bool mute)
        {
            return AliRtcMuteRemoteAudioPlaying(userId, mute);
        }

        public void StartAudioPlayer()
        {
            AliRtcStartAudioPlayer();
        }

        public void StopAudioPlayer()
        {
            AliRtcStopAudioPlayer();
        }

        public void Publish()
        {
            AliRtcPublish();
        }

        public void Subscribe(string uid)
        {
            AliRtcSubscribe(uid);
        }

#if UNITY_ANDROID || UNITY_IOS || UNITY_IPHONE
        public int GenerateTexture()
        {
            return AliRtcGenerateTexture();
        }

        public int SetTexture(AliRTCTextureInfo textureInfo, int track, string userId)
        {
            return AliRtcSetTexture(textureInfo, track, userId);
        }

        public void RemoveTexture(int textureId)
        {
            AliRtcRemoveTexture(textureId);
        }

        public int EnableSpeakerphone(bool enable)
        {
            return AliRtcEnableSpeakerphone(enable);
        }

        public int SwitchCamera()
        {
            return AliRtcSwitchCamera();
        }

        public int SetCameraZoom(float zoom, bool flash, bool autoFocus)
        {
            return AliRtcSetCameraZoom(zoom, flash, autoFocus);
        }

        public bool IsCameraFocusPointSupported()
        {
            return AliRtcIsCameraFocusPointSupported();
        }

        public bool IsCameraExposurePointSupported()
        {
            return AliRtcIsCameraExposurePointSupported();
        }

        public int SetCameraFocusPoint(float x, float y)
        {
            return AliRtcSetCameraFocusPoint(x, y);
        }

        public int SetCameraExposurePoint(float x, float y)
        {
            return AliRtcSetCameraExposurePoint(x, y);
        }
#elif UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX

        public void SetLocalViewConfig(bool flip)
        {
            AliRtcSetLocalViewConfig(flip);
        }

        public bool IsLocalScreenPublishEnabled()
        {
            return AliRtcIsLocalScreenPublishEnabled();
        }

        public void ConfigLocalScreenPublish(bool enable)
        {
            AliRtcConfigLocalScreenPublish(enable);
        }

        public int GetVideoProfile(int track)
        {
            return AliRtcGetVideoProfile(track);
        }
        
        public string GetAudioCaptures()
        {
            IntPtr audioCaptureList = AliRtcGetAudioCaptures();
            return Marshal.PtrToStringAnsi(audioCaptureList);
        }

        public string GetCurrentAudioCapture()
        {
            IntPtr curAduioCapture = AliRtcGetCurrentAudioCapture();
            return Marshal.PtrToStringAnsi(curAduioCapture);
        }

        public void SetCurrentAudioCapture(string deviceName)
        {
            AliRtcSetCurrentAudioCapture(deviceName);
        }

        public string GetAudioRenderers()
        {
            IntPtr audioRenderList = AliRtcGetAudioRenderers();
            return Marshal.PtrToStringAnsi(audioRenderList);
        }

        public string GetCurrentAudioRenderer()
        {
            IntPtr curAduioRender = AliRtcGetCurrentAudioRenderer();
            return Marshal.PtrToStringAnsi(curAduioRender);
        }

        public void SetCurrentAudioRenderer(string deviceName)
        {
            AliRtcSetCurrentAudioRenderer(deviceName);
        }

        public string GetCameraList()
        {
            IntPtr cameraList = AliRtcGetCameraList();
            return Marshal.PtrToStringAnsi(cameraList);
        }

        public string GetCurrentCamera()
        {
            IntPtr curCamera = AliRtcGetCurrentCamera();
            return Marshal.PtrToStringAnsi(curCamera);
        }

        public void SetCurrentCamera(string deviceName)
        {
            AliRtcSetCurrentCamera(deviceName);
        }
#endif

        public bool GetVideoRenderData(string uid, int track, IntPtr data, ref int width, ref int height)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            int ret = AliRtcGetVideoRenderData(uid, track, data);
            if (ret == -1)
                return false;
            width = (int)ret >> 16;
            height = (int)(ret & 0xffff);
#endif
            return true;
        }

        public void ConfigExternalVideoRendering(bool enable)
        {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            AliRtcConfigExternalVideoRendering(enable);
#endif
        }


        public static IAliRtcEngine GetEngine(string extras)
        {
            if (instance == null)
            {
                instance = new IAliRtcEngine(extras);
            }
            return instance;
        }

        public static void Destroy()
        {
            if (instance == null)
                return;

            AliRtcDestroyEngine();
            instance = null;
            DeInitGameObject();
        }

        // only query, do not create
        public static IAliRtcEngine QueryEngine()
        {
            return instance;
        }

        private static IAliRtcEngine instance = null;
    }
};

