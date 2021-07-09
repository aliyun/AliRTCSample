using System;
using System.IO;
using System.Threading;
using System.Runtime.InteropServices;
using System.Net;
using System.Collections;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using UnityEngine;
using UnityEditor;
using ali_unity_rtc;
#if (UNITY_2018_3_OR_NEWER && UNITY_ANDROID)
using UnityEngine.Android;
#endif

public class Home : MonoBehaviour
{

	Button mControlButton = null;
	bool mJoined = false;
	IAliRtcEngine mRtcEngine = null;
    private ArrayList permissionList = new ArrayList();


    void Start ()
	{
#if (UNITY_2018_3_OR_NEWER && UNITY_ANDROID)
        permissionList.Add(Permission.Microphone);
        permissionList.Add(Permission.Camera);
        permissionList.Add(Permission.ExternalStorageRead);
        permissionList.Add(Permission.ExternalStorageWrite);
#endif

#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        Screen.fullScreen = false;
        Application.wantsToQuit += OnApplicationGoingToQuit;
#endif
        mControlButton = GameObject.Find("ControlButton").GetComponent<Button>();
        mControlButton.onClick.AddListener(OnControlButtonClicked);

        GameObject go = GameObject.Find("LocalVideoCube");
        VideoDisplaySurface surface = go.AddComponent<VideoDisplaySurface>();

        go = GameObject.Find("RemoteVideoCube");
        surface = go.AddComponent<VideoDisplaySurface>();

        //create alirtc engine
        string extra = "";
        mRtcEngine = IAliRtcEngine.GetEngine (extra);

        //set callback
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        IAliRtcEngine.OnJoinChannelNotify = onJoinChannelNotify;
        IAliRtcEngine.OnPublishNotify = onPublishNotify;
        IAliRtcEngine.OnSubscribeNotify = onSubscribeNotify;
        IAliRtcEngine.OnRemoteUserOnLineNotify = onRemoteUserOnLineNotify;
        IAliRtcEngine.OnRemoteUserOffLineNotify = onRemoteUserOffLineNotify;
        IAliRtcEngine.OnRemoteTrackAvailableNotify = onRemoteTrackAvailableNotify;
        IAliRtcEngine.OnSubscribeChangedNotify = onSubscribeChangedNotify;
        IAliRtcEngine.OnLeaveChannelResultNotify = onLeaveChannelResultNotify;


#else
        mRtcEngine.OnJoinChannelNotify = onJoinChannelNotify;
		mRtcEngine.OnPublishNotify = onPublishNotify;
		mRtcEngine.OnSubscribeNotify = onSubscribeNotify;
		mRtcEngine.OnRemoteUserOnLineNotify = onRemoteUserOnLineNotify;
		mRtcEngine.OnRemoteUserOffLineNotify = onRemoteUserOffLineNotify;
		mRtcEngine.OnRemoteTrackAvailableNotify = onRemoteTrackAvailableNotify;
		mRtcEngine.OnSubscribeChangedNotify = onSubscribeChangedNotify;
		mRtcEngine.OnLeaveChannelResultNotify = onLeaveChannelResultNotify;
		
#endif
        //set auto publish and subscribe
        mRtcEngine.SetAutoPublish(true, true);

    }

	void Update ()
	{
#if (UNITY_2018_3_OR_NEWER && UNITY_ANDROID)
        CheckPermission();
#endif

    }

    bool OnApplicationGoingToQuit()
    {
        Debug.Log("OnApplicationGoingQuit");


        IAliRtcEngine.Destroy();

        Debug.Log("OnApplicationGoingQuit End");
        return true;
    }

    private void CheckPermission()
    {
#if (UNITY_2018_3_OR_NEWER && UNITY_ANDROID)
        foreach (string permission in permissionList)
        {
            if (Permission.HasUserAuthorizedPermission(permission))
            {

            }
            else
            {
                Permission.RequestUserPermission(permission);
            }
        }
#endif
    }

    private void OnControlButtonClicked()
	{
        if(mJoined)
		{
            //stop local preview
            mRtcEngine.StopPreview();
            GameObject go = GameObject.Find("LocalVideoCube");
            VideoDisplaySurface surface = go.GetComponent<VideoDisplaySurface>();
            surface.SetEnable(false);

            go = GameObject.Find("RemoteVideoCube");
            if (!ReferenceEquals(go, null))
            {
                surface = go.GetComponent<VideoDisplaySurface>();
                surface.SetEnable(false);
            }

            mRtcEngine.LeaveChannel();
        }
		else
		{
            //set local preview surface
            GameObject go = GameObject.Find("LocalVideoCube");
            VideoDisplaySurface surface = go.GetComponent<VideoDisplaySurface>();
            surface.SetUserId("");
            surface.SetVideoTrack(AliRTCVideoTrack.VIDEO_TRACK_CAMERA);
            surface.SetEnable(true);

#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
            mRtcEngine.SetLocalViewConfig(true);
            mRtcEngine.ConfigExternalVideoRendering(true);
#endif
            //start local preview
            mRtcEngine.StartPreview();

            AliRTCAuthInfo authInfo = // authInfo 从业务 appserver 获取
            //join channel
            mRtcEngine.JoinChannel(authInfo, "username");

        }

		mJoined = !mJoined;
		mControlButton.GetComponentInChildren<Text>(true).text = mJoined == false ? "Join" : "Leave";
	}

	private void onJoinChannelNotify (int errorCode)
	{
        if (errorCode == 0)
        {
            Debug.Log("加入频道成功");
        }
        else
        {
            Debug.Log("加入频道失败");
        }
    }

	private void onPublishNotify(int errorCode)
	{
        Debug.Log("AliRTCUnityDemo onPublishNotify errorCode = " + errorCode);
        if (errorCode == 0)
        {
            Debug.Log("推流成功");
        }
        else
        {
            Debug.Log("推流失败");
        }
    }

	private void onSubscribeNotify (string userId, int videoTrack, int audioTrack)
	{
	}

	private void onRemoteUserOnLineNotify (string userId)
	{

	}

	private void onRemoteUserOffLineNotify (string userId)
	{
        GameObject go = GameObject.Find("RemoteVideoCube");
        if (!ReferenceEquals(go, null))
        {
            VideoDisplaySurface surface = go.GetComponent<VideoDisplaySurface>();
            surface.SetEnable(false);
        }
    }

	private void onRemoteTrackAvailableNotify (string userId, int audioTrack, int videoTrack)
	{
	}

	private void onSubscribeChangedNotify (string userId, int audioTrack, int videoTrack)
	{

        if (videoTrack == (int)AliRTCVideoTrack.VIDEO_TRACK_CAMERA)
        {
            GameObject go = GameObject.Find("RemoteVideoCube");
            if (!ReferenceEquals(go, null))
            {
                VideoDisplaySurface surface = go.GetComponent<VideoDisplaySurface>();
                surface.SetUserId(userId);
                surface.SetVideoTrack(AliRTCVideoTrack.VIDEO_TRACK_CAMERA);
                surface.SetEnable(true);
            }

        }
        else if (videoTrack == (int)AliRTCVideoTrack.VIDEO_TRACK_NONE)
        {
            GameObject go = GameObject.Find("RemoteVideoCube");
            if (!ReferenceEquals(go, null))
            {
                VideoDisplaySurface surface = go.GetComponent<VideoDisplaySurface>();
                surface.SetEnable(false);
            }

        }
    }

	private void onLeaveChannelResultNotify(int result)
	{
		Debug.Log("AliRTCUnityDemo onLeaveChannelResultNotify result: " + result);
	}

}
