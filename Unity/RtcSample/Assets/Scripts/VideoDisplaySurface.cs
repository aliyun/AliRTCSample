using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Runtime.InteropServices;
using ali_unity_rtc;

public class VideoDisplaySurface : MonoBehaviour
{
    private bool mEnable = false;
    private string mUserId = null;
    private int mTrackId = (int)AliRTCVideoTrack.VIDEO_TRACK_CAMERA;
    private int mTextureWidth = 0;
    private int mTextureHeight = 0;
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
    private Texture2D mNativeTexture = null;
    private System.IntPtr mNativeTextureData = Marshal.AllocHGlobal(4000 * 3000 * 4);
    private float mLastUpdateTime = 0f;
    private const float MaxFPS = 30.0f;
    private float mFrameInterval = 1.0f / MaxFPS;
#elif UNITY_ANDROID || UNITY_IOS || UNITY_IPHONE
    private Texture2D nativeTexture;
#endif

    void Start()
    {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        mLastUpdateTime = Time.realtimeSinceStartup;
#endif
    }

    void Update()
    {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        if (Time.realtimeSinceStartup - mLastUpdateTime <= mFrameInterval)
        {
            return;
        }
        mLastUpdateTime = Time.realtimeSinceStartup;

        if (!mEnable)
        {
            return;
        }

        IAliRtcEngine engine = IAliRtcEngine.QueryEngine();
        if (ReferenceEquals(engine, null))
            return;

        if (engine.GetVideoRenderData(mUserId, (int)mTrackId, mNativeTextureData, ref mTextureWidth, ref mTextureHeight) == true)
        {
            if (ReferenceEquals(mNativeTexture, null) && mTextureHeight > 0 && mTextureWidth > 0)
            {
                mNativeTexture = new Texture2D(mTextureWidth, mTextureHeight, TextureFormat.RGBA32, false);
                Renderer rend = GetComponent<Renderer>();
                rend.material.mainTexture = mNativeTexture;
            }

            if (!ReferenceEquals(mNativeTexture, null) && (mTextureWidth != mNativeTexture.width || mTextureHeight != mNativeTexture.height))
            {
                mNativeTexture = null;
                mNativeTexture = new Texture2D(mTextureWidth, mTextureHeight, TextureFormat.RGBA32, false);
                Renderer rend = GetComponent<Renderer>();
                rend.material.mainTexture = mNativeTexture;
            }

            if (!ReferenceEquals(mNativeTexture, null))
            {
                mNativeTexture.LoadRawTextureData(mNativeTextureData, mTextureWidth * mTextureHeight * 4);
                mNativeTexture.Apply();
            }
        }

#elif UNITY_ANDROID || UNITY_IOS || UNITY_IPHONE
        Renderer rend = GetComponent<Renderer>();
        if(mEnable) 
        {
            if (rend.material.mainTexture == null) 
            {
                    IAliRtcEngine engine = IAliRtcEngine.QueryEngine();
                    if (engine == null)
                        return;
                    IntPtr tex = (IntPtr)engine.GenerateTexture();
                    nativeTexture = Texture2D.CreateExternalTexture(720, 1280, TextureFormat.ARGB32, false, false, tex); 
                    nativeTexture.filterMode = FilterMode.Bilinear;
                    nativeTexture.wrapMode = TextureWrapMode.Clamp;
                    nativeTexture.Apply();
                    rend.material.mainTexture = nativeTexture;

                    if (rend.material.mainTexture != null && rend.material.mainTexture is Texture2D) 
                    {
                        Texture2D tex2D = rend.material.mainTexture as Texture2D;
                        int texId = (int)tex2D.GetNativeTexturePtr();
                        int texWidth = 720;
                        int texHeight = 1280;
                        AliRTCTextureInfo textureInfo;
                        textureInfo.textureId =  texId;
                        textureInfo.flip =  (int)AliRTCRenderMirror.RENDER_MIRROR_FRONT_PREVIEW_ENABLED;
                        engine.SetTexture (textureInfo, mTrackId, mUserId);
                    }
            } 
        }
        
#endif
    }

    void OnDestroy()
    {
#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        Marshal.FreeHGlobal(mNativeTextureData);
#endif
    }

    public void SetUserId(string uid)
    {
        mUserId = uid;
    }

    public void SetVideoTrack(AliRTCVideoTrack track)
    {
        mTrackId = (int)track;
    }

    public void SetEnable(bool enable)
    {

#if UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX
        
        mEnable = enable;

        if(!mEnable)
        {
            Renderer rend = GetComponent<Renderer>();
            rend.material.mainTexture = null;
            mNativeTexture = null;
        }
#elif UNITY_ANDROID || UNITY_IOS || UNITY_IPHONE
        IAliRtcEngine engine = IAliRtcEngine.QueryEngine();
        if (engine == null)
            return;

        Renderer rend = GetComponent<Renderer>();
       
       if(enable) 
       {
           if(mUserId == "")
           {
               if (rend.material.mainTexture == null) 
                {
                        IntPtr tex = (IntPtr)engine.GenerateTexture();
                        nativeTexture = Texture2D.CreateExternalTexture(720, 1280, TextureFormat.ARGB32, false, false, tex); 
                        nativeTexture.filterMode = FilterMode.Bilinear;
                        nativeTexture.wrapMode = TextureWrapMode.Clamp;
                        nativeTexture.Apply();
                        rend.material.mainTexture = nativeTexture;

                        if (rend.material.mainTexture != null && rend.material.mainTexture is Texture2D) 
                        {
                            Texture2D tex2D = rend.material.mainTexture as Texture2D;
                            int texId = (int)tex2D.GetNativeTexturePtr();
                            int texWidth = 720;
                            int texHeight = 1280;
                            AliRTCTextureInfo textureInfo;
                            textureInfo.textureId =  texId;
                            textureInfo.flip =  (int)AliRTCRenderMirror.RENDER_MIRROR_FRONT_PREVIEW_ENABLED;
                            Debug.Log ("AliRTCUnityDemo VideoDisplaySurface SetEnable SetTexture : texId " + texId + " uid : " + mUserId + " mTrackId : " + mTrackId );
                            engine.SetTexture (textureInfo, mTrackId, mUserId);
                        }
                } 

           }
           else
           {
               rend.material.mainTexture = null;
           }

       }
       else
       {
            if (rend.material.mainTexture != null && rend.material.mainTexture is Texture2D) 
            {
                Texture2D tex = rend.material.mainTexture as Texture2D;
                int texId = (int)tex.GetNativeTexturePtr ();
                Debug.Log ("AliRTCUnityDemo VideoDisplaySurface engine.RemoveTexture texId : " + texId);
                engine.RemoveTexture(texId);
                rend.material.mainTexture = null;
            }
        }

         mEnable = enable;

         
#endif
    }
}
