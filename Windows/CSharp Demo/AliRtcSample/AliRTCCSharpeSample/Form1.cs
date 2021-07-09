using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.IO;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Collections;
using System.Diagnostics;

struct AuthInfo
{
    public string strAppId;
    public string strUserId;
    public string strNonce;
    public string strToken;
    public string strRole;
    public long   iTimesTamp;
    public string strGlsb;
    public string strAgent;
    public string strUserName;
    public string strChannel;
}

public struct ParamInfo
{
    public string strAppSer;
    public string strChannel;
    public string strUserName;
    public bool bAutoPush;
    public bool bAutoSub;
    public bool bH5Mode;
    public bool bPushAudio;
    public bool bPushCamera;
    public bool bPushScreen;
}

public struct viewInfo
{
    public string strUid;
    public AliRTCVideoTrack videoTrack;
    public Panel showPanel;
}

/**
* @brief 日志级别
*/
public enum AliRTClogLevel
{
    LOG_LEVEL_DUMP = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_NONE,
}

/**
* @brief 视频track的类型
*
* 其中AliRtcVideoTrackBoth = AliRtcVideoTrackCamera | AliRtcVideoTrackScreen
*/
public enum AliRTCVideoTrack
{
    VIDEO_TRACK_NONE = 0,
    VIDEO_TRACK_CAMERA,
    VIDEO_TRACK_SCREEN,
    VIDEO_TRACK_BOTH,
}

/**
* @brief 用户离线原因
*/
public enum AliRTCUserOfflineReason
{
    AliRTCUserOfflineQuit = 0,              /* 用户主动离开 */
    AliRTCUserOfflineDropped = 1,           /* 因过长时间收不到对方数据包，超时掉线 */
    AliRTCUserOfflineBecomeAudience = 2,    /* 用户身份从主播切换为观众时触发 */
}

/**
* @brief 音频track的类型
*/
public enum AliRTCAudioTrack
{
    AUDIO_TRACK_NONE = 0,
    AUDIO_TRACK_MIC,
}

public enum AliRTCOnByeType
{
    AliRTCOnByeBeKickOut = 1,
    AliRTCOnByeChannelTerminated = 2,
    AliRTCOnByeUserReplaced = 3
}

/**
* @brief 桌面分享类型
*/
public enum AliRTCScreenShareType
{
    AliRTCScreenShareDesktop = 0,      /* 桌面分享 */
    AliRTCScreenShareWindow,       /* 窗口分享 */
}

namespace AliRtcSample
{
    public partial class AliRtcSample : Form
    {
        public static AliRtcSample mainFrm = null;
        public static string m_strUid = "";
        public static ParamInfo m_ParamInfo;
        public static Hashtable m_camaraUid2panelHash;
        public static Hashtable m_screenUid2panelHash;
        public static LinkedList<string> m_uidList;
        public static bool m_bInitialItem = false;
        public static int m_nsmallViewSize = 4;
        public delegate void RemoteTrackAvailableNotify(string strUid, AliRTCAudioTrack audioTrack, AliRTCVideoTrack videoTrack, string channel);
        public static LinkedList<viewInfo> m_viewInfoList;

        public AliRtcSample()
        {
            InitializeComponent();
            m_camaraUid2panelHash = new Hashtable();
            m_screenUid2panelHash = new Hashtable();
            m_uidList = new LinkedList<string>();
            m_viewInfoList = new LinkedList<viewInfo>();
            initialItem();
            updateParamInfo();
            Control.CheckForIllegalCrossThreadCalls = false;
            mainFrm = this;
            m_bInitialItem = true;
            Layout_ParamItem();
            
        }

        private void initialItem()
        {
            this.richTextBox1.Clear();
            this.richTextBox1.AppendText("127.0.0.1");
            this.check_autoPush.Checked = true;
            this.checkAutoSub.Checked = true;
            this.checkAudio.Checked = true;
            this.checkCamera.Checked = true;
            this.checkH5.Checked = false;
            this.checkScreen.Checked = false;
        }

        private void updateParamInfo()
        {
            m_ParamInfo.bPushAudio = this.checkAudio.Checked;
            m_ParamInfo.bPushCamera = this.checkCamera.Checked;
            m_ParamInfo.bPushScreen = this.checkScreen.Checked;
            m_ParamInfo.bAutoPush = this.check_autoPush.Checked;
            m_ParamInfo.bAutoSub = this.checkAutoSub.Checked;
            m_ParamInfo.bH5Mode = this.checkH5.Checked;
            m_ParamInfo.strAppSer = this.richTextBox1.Text;
            m_ParamInfo.strChannel = this.textBox_ChannelNum.Text;
            m_ParamInfo.strUserName = this.textBox_UserName.Text;
        }

        public static void OnJoinResult(int result, string channel, int elapsed)
        {
            if (0 == result)
            {
                AliRtcSample.mainFrm.listBox_Tips.Items.Add("加入房间成功...");
            }
            else
            {
                AliRtcSample.mainFrm.listBox_Tips.Items.Add("加入房间失败...");
            }
        }

        public static void onRemoteUserOnLineNotify(string uid, int elapsed, string channel)
        {
            AliRtcSample.mainFrm.listBox_Tips.Items.Add("入会通知：" + uid);
            if (m_strUid == "")
            {
                m_strUid = uid;
            }
        }

        public static void onBye(int code, string channel)
        {
            AliRTCOnByeType OnByeType = (AliRTCOnByeType)code;
            if (OnByeType == AliRTCOnByeType.AliRTCOnByeBeKickOut)
                AliRtcSample.mainFrm.listBox_Tips.Items.Add("当前用户被踢出频道");
            else if(OnByeType == AliRTCOnByeType.AliRTCOnByeChannelTerminated)
                AliRtcSample.mainFrm.listBox_Tips.Items.Add("频道结束");
            else if(OnByeType == AliRTCOnByeType.AliRTCOnByeUserReplaced)
                AliRtcSample.mainFrm.listBox_Tips.Items.Add("相同用户在其他设备加入频道，当前设备被下线");
            else
                AliRtcSample.mainFrm.listBox_Tips.Items.Add("其他原因离开频道");
            foreach (viewInfo viewInfo_ in m_viewInfoList)
                viewInfo_.showPanel.Hide();
            m_viewInfoList.Clear();
        }

        public static void onOccurError(int error, string msg, string channel)
        {
            AliRtcSample.mainFrm.listBox_Tips.Items.Add("错误码：" + error + "错误信息：" + msg);
        }

        public static void onRemoteUserOffLineNotify(string uid, AliRTCUserOfflineReason reason, string channel)
        {
            AliRtcSample.mainFrm.listBox_Tips.Items.Add("离会通知：" + uid);

            if (m_strUid == uid)
            {
                m_strUid = "";
            }
        }

        public static void DeleteViewInfo(string struUid, AliRTCVideoTrack videoTrack)
        {
            Debug.Assert(videoTrack == AliRTCVideoTrack.VIDEO_TRACK_CAMERA ||
                videoTrack == AliRTCVideoTrack.VIDEO_TRACK_SCREEN);
            List<viewInfo> deleteViewList = new List<viewInfo>();
            foreach (viewInfo viewInfo_ in m_viewInfoList)
            {
                if (viewInfo_.strUid == struUid && 
                    viewInfo_.videoTrack == videoTrack)
                    deleteViewList.Add(viewInfo_);
            }
            foreach(viewInfo deleteView in deleteViewList)
            {
                deleteView.showPanel.Hide();
                m_viewInfoList.Remove(deleteView);
            }
               
        }

        public static void findOrCreatPanel_(string strUid, AliRTCVideoTrack videoTrack, out Panel panel_)
        {
            panel_ = null;
            foreach(viewInfo viewInfo_ in m_viewInfoList)
            {
                if (viewInfo_.strUid == strUid &&
                    viewInfo_.videoTrack == videoTrack)
                {
                    panel_ = viewInfo_.showPanel;
                    return;
                }
            }
            viewInfo tempViewInfo;
            tempViewInfo.strUid = strUid;
            tempViewInfo.videoTrack = videoTrack;
            tempViewInfo.showPanel = new System.Windows.Forms.Panel();
            tempViewInfo.showPanel.BackColor = System.Drawing.SystemColors.ControlDark;
            panel_ = tempViewInfo.showPanel;
            m_viewInfoList.AddLast(tempViewInfo);
        }

        public static void onLayout()
        {
            Rectangle clientRect;
            Rectangle smallViewRect = new Rectangle();
            Rectangle bigViewRect = new Rectangle();
            Rectangle ItemRect = new Rectangle();
            Rectangle remoteDlgRect = new Rectangle();
            clientRect = mainFrm.ClientRectangle;
            smallViewRect.X = 0;
            smallViewRect.Y = 0;
            smallViewRect.Width = clientRect.Width * 3 / 4;
            smallViewRect.Height = clientRect.Height / 4;

            bigViewRect.X = 0;
            bigViewRect.Y = clientRect.Height / 4 + 5;
            bigViewRect.Width = clientRect.Width * 3 / 4;
            bigViewRect.Height = clientRect.Height * 2 / 4;

            ItemRect.X = 0;
            ItemRect.Y = clientRect.Height * 3 / 4;
            ItemRect.Width = clientRect.Width * 3 / 4;
            ItemRect.Height = clientRect.Height / 4;

            remoteDlgRect.X = clientRect.Width * 3 / 4 + 5;
            remoteDlgRect.Y = 0;
            remoteDlgRect.Width = clientRect.Width - clientRect.Width * 3 / 4 - 5;
            remoteDlgRect.Height = clientRect.Height;
        }

        public static void Layout_RemoteView()
        {
            int nsmallViewSize = 0;
            Rectangle clientRect;
            Rectangle smallViewRect = new Rectangle();
            Rectangle bigViewRect = new Rectangle();
            clientRect = mainFrm.ClientRectangle;
            smallViewRect.X = 10 + 25;
            smallViewRect.Y = 10;
            smallViewRect.Width = clientRect.Width * 3 / 4 - 25;
            smallViewRect.Height = clientRect.Height / 6;

            bigViewRect.X = 10;
            bigViewRect.Y = clientRect.Height / 6 + 5;
            bigViewRect.Width = clientRect.Width * 3 / 4;
            bigViewRect.Height = clientRect.Height * 4 / 6;

            if (m_uidList.Count == 0)
                return;
            foreach(string tempUid in m_uidList)
            {
                Debug.Assert(m_camaraUid2panelHash.Contains(tempUid) ||
                    m_screenUid2panelHash.Contains(tempUid));
                if (tempUid == m_uidList.First())
                {
                    //排队1用户
                    if (m_camaraUid2panelHash.Contains(tempUid) &&
                        m_screenUid2panelHash.Contains(tempUid))
                    {
                        Panel camera_panel = (Panel)m_camaraUid2panelHash[tempUid];
                        Panel screen_panel = (Panel)m_screenUid2panelHash[tempUid];
                        Debug.Assert(camera_panel != null);
                        Debug.Assert(screen_panel != null);
                        camera_panel.Location = new System.Drawing.Point(bigViewRect.X, bigViewRect.Y);
                        camera_panel.Size = new System.Drawing.Size(bigViewRect.Width, bigViewRect.Height);
                        mainFrm.Controls.Add(camera_panel);

                        Rectangle subSmallViewRect = new Rectangle();
                        if (nsmallViewSize > m_nsmallViewSize){
                            screen_panel.Hide();
                            continue;
                        }
                        subSmallViewRect.X = 
                            smallViewRect.X + smallViewRect.Width * nsmallViewSize / m_nsmallViewSize;
                        subSmallViewRect.Y = smallViewRect.Y;
                        subSmallViewRect.Width = smallViewRect.Width * (nsmallViewSize + 1) / m_nsmallViewSize;
                        subSmallViewRect.Height = smallViewRect.Height;
                        screen_panel.Location = new System.Drawing.Point(subSmallViewRect.X, subSmallViewRect.Y);
                        screen_panel.Size = new System.Drawing.Size(subSmallViewRect.Width, subSmallViewRect.Height - 5);
                        mainFrm.Controls.Add(screen_panel);
                        ++nsmallViewSize;
                    }
                    else if (m_camaraUid2panelHash.Contains(tempUid) &&
                        !m_screenUid2panelHash.Contains(tempUid))
                    {
                        Panel camera_panel = (Panel)m_camaraUid2panelHash[tempUid];
                        Debug.Assert(camera_panel != null);
                        camera_panel.Location = new System.Drawing.Point(bigViewRect.X, bigViewRect.Y);
                        camera_panel.Size = new System.Drawing.Size(bigViewRect.Width, bigViewRect.Height);
                        mainFrm.Controls.Add(camera_panel);
                    }
                    else if (!m_camaraUid2panelHash.Contains(tempUid) &&
                        m_screenUid2panelHash.Contains(tempUid))
                    {
                        Panel screen_panel = (Panel)m_screenUid2panelHash[tempUid];
                        Debug.Assert(screen_panel != null);
                        screen_panel.Location = new System.Drawing.Point(bigViewRect.X, bigViewRect.Y);
                        screen_panel.Size = new System.Drawing.Size(bigViewRect.Width, bigViewRect.Height);
                        mainFrm.Controls.Add(screen_panel);
                    }

                }
                else
                {   //排队非1用户
                    if (m_camaraUid2panelHash.Contains(tempUid) &&
                        m_screenUid2panelHash.Contains(tempUid))
                    {
                        Panel camera_panel = (Panel)m_camaraUid2panelHash[tempUid];
                        Panel screen_panel = (Panel)m_screenUid2panelHash[tempUid];
                        Debug.Assert(camera_panel != null);
                        Debug.Assert(screen_panel != null);
                        if (nsmallViewSize > m_nsmallViewSize)
                        {
                            camera_panel.Hide();
                            screen_panel.Hide();
                            continue;
                        }
                        Rectangle subSmallViewRect = new Rectangle();
                        subSmallViewRect.X =
                            smallViewRect.X + smallViewRect.Width * nsmallViewSize / m_nsmallViewSize;
                        subSmallViewRect.Y = smallViewRect.Y;
                        subSmallViewRect.Width = smallViewRect.Width * (nsmallViewSize + 1) / m_nsmallViewSize;
                        subSmallViewRect.Height = smallViewRect.Height;

                        camera_panel.Location = new System.Drawing.Point(subSmallViewRect.X, subSmallViewRect.Y);
                        camera_panel.Size = new System.Drawing.Size(subSmallViewRect.Width, subSmallViewRect.Height - 5);
                        mainFrm.Controls.Add(camera_panel);
                        ++nsmallViewSize;

                        if (nsmallViewSize > m_nsmallViewSize)
                        {
                            screen_panel.Hide();
                            continue;
                        }
                        subSmallViewRect.X =
                              smallViewRect.X + smallViewRect.Width * nsmallViewSize / m_nsmallViewSize;
                        subSmallViewRect.Y = smallViewRect.Y;
                        subSmallViewRect.Width = smallViewRect.Width * (nsmallViewSize + 1) / m_nsmallViewSize;
                        subSmallViewRect.Height = smallViewRect.Height;

                        screen_panel.Location = new System.Drawing.Point(subSmallViewRect.X, subSmallViewRect.Y);
                        screen_panel.Size = new System.Drawing.Size(subSmallViewRect.Width, subSmallViewRect.Height - 5);
                        mainFrm.Controls.Add(screen_panel);
                        ++nsmallViewSize;
                    }
                    else if (m_camaraUid2panelHash.Contains(tempUid) &&
                        !m_screenUid2panelHash.Contains(tempUid))
                    {
                        Panel camera_panel = (Panel)m_camaraUid2panelHash[tempUid];
                        Debug.Assert(camera_panel != null);
                        if (nsmallViewSize > m_nsmallViewSize){
                            camera_panel.Hide();
                            continue;
                        }

                        Rectangle subSmallViewRect = new Rectangle();
                        subSmallViewRect.X =
                            smallViewRect.X + smallViewRect.Width * nsmallViewSize / m_nsmallViewSize;
                        subSmallViewRect.Y = smallViewRect.Y;
                        subSmallViewRect.Width = smallViewRect.Width * (nsmallViewSize + 1) / m_nsmallViewSize;
                        subSmallViewRect.Height = smallViewRect.Height;
                        //测试
                        camera_panel.Location = new System.Drawing.Point(subSmallViewRect.X, subSmallViewRect.Y);
                        camera_panel.Size = new System.Drawing.Size(subSmallViewRect.Width, subSmallViewRect.Height - 5);
                        mainFrm.Controls.Add(camera_panel);
                        ++nsmallViewSize;

                    }

                    else if (!m_camaraUid2panelHash.Contains(tempUid) &&
                        m_screenUid2panelHash.Contains(tempUid))
                    {
                        Panel screen_panel = (Panel)m_screenUid2panelHash[tempUid];
                        Debug.Assert(screen_panel != null);
                        if (nsmallViewSize > m_nsmallViewSize)
                        {
                            screen_panel.Hide();
                            continue;
                        }
                        Rectangle subSmallViewRect = new Rectangle();
                        subSmallViewRect.X =
                            smallViewRect.X + smallViewRect.Width * nsmallViewSize / m_nsmallViewSize;
                        subSmallViewRect.Y = smallViewRect.Y;
                        subSmallViewRect.Width = smallViewRect.Width * (nsmallViewSize + 1) / m_nsmallViewSize;
                        subSmallViewRect.Height = smallViewRect.Height;

                        screen_panel.Location = new System.Drawing.Point(subSmallViewRect.X, subSmallViewRect.Y);
                        screen_panel.Size = new System.Drawing.Size(subSmallViewRect.Width, subSmallViewRect.Height - 5);
                        mainFrm.Controls.Add(screen_panel);
                        ++nsmallViewSize;
                    }

                }
                   

            }

        }

        public static void hideAllPanel()
        {
            foreach (viewInfo viewInfo_ in m_viewInfoList)
                viewInfo_.showPanel.Hide();
        }

        public static void Layout_RemoteView_()
        {
            int nsmallViewSize = 0;
            Rectangle clientRect;
            Rectangle subSmallViewRect = new Rectangle();
            Rectangle smallViewRect = new Rectangle();
            Rectangle bigViewRect = new Rectangle();
            clientRect = mainFrm.ClientRectangle;
            smallViewRect.X = 10 + 25;
            smallViewRect.Y = 10;
            smallViewRect.Width = clientRect.Width * 3 / 4 - 25;
            smallViewRect.Height = clientRect.Height / 6;

            bigViewRect.X = 10;
            bigViewRect.Y = clientRect.Height / 6 + 5;
            bigViewRect.Width = clientRect.Width * 3 / 4;
            bigViewRect.Height = clientRect.Height * 4 / 6;

            if (m_viewInfoList.Count() == 0)
                return;
            //hideAllPanel();
            viewInfo FirstViewInfo = m_viewInfoList.First();
            foreach (viewInfo viewInfo_ in m_viewInfoList)
            {
                if (viewInfo_.strUid == FirstViewInfo.strUid &&
                    viewInfo_.videoTrack == FirstViewInfo.videoTrack &&
                    viewInfo_.showPanel == FirstViewInfo.showPanel)
                {
                    viewInfo_.showPanel.Location =
                        new System.Drawing.Point(bigViewRect.X, bigViewRect.Y);
                    viewInfo_.showPanel.Size = 
                        new System.Drawing.Size(bigViewRect.Width, bigViewRect.Height);
                    AliRtcSample.mainFrm.Controls.Add(viewInfo_.showPanel);
                }
                else
                {
                    subSmallViewRect.X =
                         smallViewRect.X + smallViewRect.Width * nsmallViewSize / m_nsmallViewSize;
                    subSmallViewRect.Y = smallViewRect.Y;
                    subSmallViewRect.Width = smallViewRect.Width / m_nsmallViewSize;
                    subSmallViewRect.Height = smallViewRect.Height;
                    viewInfo_.showPanel.Location = new System.Drawing.Point(subSmallViewRect.X, subSmallViewRect.Y);
                    viewInfo_.showPanel.Size = new System.Drawing.Size(subSmallViewRect.Width, subSmallViewRect.Height - 5);
                    AliRtcSample.mainFrm.Controls.Add(viewInfo_.showPanel);
                    ++nsmallViewSize;
                }
            }

        }

        public static void Layout_ParamItem()
        {
            Rectangle clientRect = mainFrm.ClientRectangle;
            Rectangle ParamItemRect = new Rectangle();
            ParamItemRect.X = 10;
            ParamItemRect.Y = clientRect.Height * 5 / 6;
            ParamItemRect.Width = clientRect.Width * 3 / 4;
            ParamItemRect.Height = clientRect.Height / 6;

            Rectangle smallViewRect = new Rectangle();
            smallViewRect.X = 10;
            smallViewRect.Y = 10;
            smallViewRect.Width = clientRect.Width * 3 / 4;
            smallViewRect.Height = clientRect.Height / 6;

            Rectangle remoteDlgRect = new Rectangle();
            remoteDlgRect.X = clientRect.Width * 3 / 4 + 5;
            remoteDlgRect.Y = 10;
            remoteDlgRect.Width = clientRect.Width / 4 - 5;
            remoteDlgRect.Height = clientRect.Height;


            mainFrm.btn_left.Location = 
                new System.Drawing.Point(smallViewRect.X, smallViewRect.Y);
            mainFrm.btn_left.Size = new System.Drawing.Size(25, smallViewRect.Height - 10);

            mainFrm.btn_right.Location =
                new System.Drawing.Point(clientRect.Width * 3 / 4 - 25, smallViewRect.Y);
            mainFrm.btn_right.Size = new System.Drawing.Size(25, smallViewRect.Height - 10);

            mainFrm.groupBox_LocalView.Location =
                new System.Drawing.Point(remoteDlgRect.X + 10, remoteDlgRect.Y);
            mainFrm.groupBox_LocalView.Size =
                new System.Drawing.Size(remoteDlgRect.Width - 10, (remoteDlgRect.Width - 10) / 2);
            mainFrm.groupBox_LocalView.Controls.Add(mainFrm.panel_LocalView);
            mainFrm.panel_LocalView.Location = new System.Drawing.Point(5, 15);
            mainFrm.panel_LocalView.Size =
                new System.Drawing.Size(remoteDlgRect.Width - 20, (remoteDlgRect.Width - 10) / 2 - 20);

            Rectangle tempRect = new Rectangle();
            tempRect.X = ParamItemRect.X;
            tempRect.Y = ParamItemRect.Y + 20;

            mainFrm.label_ChannelNum.Location = 
                new System.Drawing.Point(tempRect.X, tempRect.Y);
            mainFrm.label_ChannelNum.Size = new System.Drawing.Size(89, 18);

            tempRect.X += 65;
            mainFrm.textBox_ChannelNum.Location = 
                new System.Drawing.Point(tempRect.X, tempRect.Y);
            mainFrm.textBox_ChannelNum.Size = new System.Drawing.Size(ParamItemRect.Width / 6, 28);

            tempRect.X = ParamItemRect.X;
            tempRect.Y = ParamItemRect.Y + 50;
            mainFrm.labelUserName.Location =
                new System.Drawing.Point(tempRect.X, tempRect.Y);
            mainFrm.labelUserName.Size = new System.Drawing.Size(89, 18);

            tempRect.X += 65;
            mainFrm.textBox_UserName.Location =
                new System.Drawing.Point(tempRect.X, tempRect.Y);
            mainFrm.textBox_UserName.Size = new System.Drawing.Size(ParamItemRect.Width / 6, 28);

            tempRect.X += ParamItemRect.Width / 6 + 5;
            tempRect.Y = ParamItemRect.Y + 10;

            mainFrm.groupBox1.Controls.Add(mainFrm.richTextBox1);
            mainFrm.groupBox1.Location = 
                new System.Drawing.Point(tempRect.X, tempRect.Y);
            mainFrm.groupBox1.Size = new System.Drawing.Size(ParamItemRect.Width / 4, 78);

            mainFrm.richTextBox1.Location = new System.Drawing.Point(6, 12);
            mainFrm.richTextBox1.Size = new System.Drawing.Size(ParamItemRect.Width / 4 - 10, 63);


            tempRect.X += ParamItemRect.Width / 4 + 5;
            mainFrm.groupBox2.Controls.Add(mainFrm.check_autoPush);
            mainFrm.groupBox2.Controls.Add(mainFrm.checkAutoSub);
            mainFrm.groupBox2.Controls.Add(mainFrm.checkH5);

            mainFrm.groupBox2.Location = new System.Drawing.Point(tempRect.X, tempRect.Y);
            mainFrm.groupBox2.Size = new System.Drawing.Size(ParamItemRect.Width / 8, 78);

            tempRect.X += ParamItemRect.Width / 8 + 5;
            mainFrm.groupBox3.Controls.Add(mainFrm.checkAudio);
            mainFrm.groupBox3.Controls.Add(mainFrm.checkCamera);
            mainFrm.groupBox3.Controls.Add(mainFrm.checkScreen);
            mainFrm.groupBox3.Location = new System.Drawing.Point(tempRect.X, tempRect.Y);
            mainFrm.groupBox3.Size = new System.Drawing.Size(ParamItemRect.Width / 8, 78);

            tempRect.X += ParamItemRect.Width / 8 + 5;
            tempRect.Y += 10;

            tempRect.Y += 22;

            tempRect.Y += 22;
            tempRect.Y = ParamItemRect.Y + 10;
            mainFrm.button_Join.Location = new System.Drawing.Point(tempRect.X, tempRect.Y);
            mainFrm.button_Join.Size = new System.Drawing.Size(80, 30);

            tempRect.Y += 40;
            mainFrm.button_Leave.Location = new System.Drawing.Point(tempRect.X, tempRect.Y);
            mainFrm.button_Leave.Size = new System.Drawing.Size(80, 30);

            tempRect.X += 85;
            tempRect.Y = ParamItemRect.Y + 10;
            mainFrm.listBox_Tips.Location = new System.Drawing.Point(tempRect.X, tempRect.Y);
            mainFrm.listBox_Tips.Size = new System.Drawing.Size(150, 78);
        }

        public static void findOrCreatPanel(string strUid, AliRTCVideoTrack videoTrack, out Panel panel_)
        {
            if (videoTrack == AliRTCVideoTrack.VIDEO_TRACK_CAMERA)
            {
                if (m_camaraUid2panelHash.Contains(strUid)){
                    panel_ = (Panel)m_camaraUid2panelHash[strUid];
                }
                else{
                    panel_ = new System.Windows.Forms.Panel();
                    panel_.BackColor = System.Drawing.SystemColors.ControlDark;
                    m_camaraUid2panelHash.Add(strUid, panel_);
                }
            }
            else if (videoTrack == AliRTCVideoTrack.VIDEO_TRACK_SCREEN)
            {
                if (m_screenUid2panelHash.Contains(strUid)){
                    panel_ = (Panel)m_screenUid2panelHash[strUid];
                }
                else{
                    panel_ = new System.Windows.Forms.Panel();
                    panel_.BackColor = System.Drawing.SystemColors.ControlDark;
                    m_screenUid2panelHash.Add(strUid, panel_);
                }
            }
            else
            {
                panel_ = null;
            }
        }

        public static void onRemoteTrackAvailableNotify_(string strUid, AliRTCAudioTrack audioTrack, AliRTCVideoTrack videoTrack, string channel)
        {
            if (videoTrack == AliRTCVideoTrack.VIDEO_TRACK_NONE)
            {
                DeleteViewInfo(strUid, AliRTCVideoTrack.VIDEO_TRACK_CAMERA);
                DeleteViewInfo(strUid, AliRTCVideoTrack.VIDEO_TRACK_SCREEN);
            }
            else if (videoTrack == AliRTCVideoTrack.VIDEO_TRACK_CAMERA)
            {
                Panel panel_camera;
                DeleteViewInfo(strUid, AliRTCVideoTrack.VIDEO_TRACK_SCREEN);
                findOrCreatPanel_(strUid, videoTrack, out panel_camera);
                Debug.Assert(panel_camera != null);
                ImportDll.AliRtcSetRemoteViewConfig(panel_camera.Handle, 0, 0, 0, 0, 0, 0, 0, 0, true, strUid, videoTrack, m_ParamInfo.strChannel);
            }
            else if (videoTrack == AliRTCVideoTrack.VIDEO_TRACK_SCREEN)
            {
                Panel panel_screen;
                DeleteViewInfo(strUid, AliRTCVideoTrack.VIDEO_TRACK_CAMERA);
                findOrCreatPanel_(strUid, videoTrack, out panel_screen);
                Debug.Assert(panel_screen != null);
                ImportDll.AliRtcSetRemoteViewConfig(panel_screen.Handle, 0, 0, 0, 0, 0, 0, 0, 0, true, strUid, videoTrack, m_ParamInfo.strChannel);
            }
            else if (videoTrack == AliRTCVideoTrack.VIDEO_TRACK_BOTH)
            {
                Panel panel_camera;
                Panel panel_screen;
                findOrCreatPanel_(strUid, AliRTCVideoTrack.VIDEO_TRACK_CAMERA, out panel_camera);
                findOrCreatPanel_(strUid, AliRTCVideoTrack.VIDEO_TRACK_SCREEN, out panel_screen);
                Debug.Assert(panel_camera != null);
                Debug.Assert(panel_screen != null);

                ImportDll.AliRtcSetRemoteViewConfig(panel_camera.Handle, 0, 0, 0, 0, 0, 0, 0, 0, true, strUid, AliRTCVideoTrack.VIDEO_TRACK_CAMERA, m_ParamInfo.strChannel);

                ImportDll.AliRtcSetRemoteViewConfig(panel_screen.Handle, 0, 0, 0, 0, 0, 0, 0, 0, true, strUid, AliRTCVideoTrack.VIDEO_TRACK_SCREEN, m_ParamInfo.strChannel);
            }
            Layout_RemoteView_();
        }

        public static void onRemoteTrackAvailableNotify(string strUid, AliRTCAudioTrack audioTrack, AliRTCVideoTrack videoTrack, string channel)
        {
            if (mainFrm.InvokeRequired)
            {
                Object[] parametors = new Object[] { strUid, audioTrack, videoTrack, channel };
                mainFrm.Invoke(new RemoteTrackAvailableNotify(onRemoteTrackAvailableNotify_), parametors);
            }
        }

        private void dllJoinChannel(AuthInfo autoinfo)
        {
            ImportDll.joinResultCB = new ImportDll.setJoinResultCB(OnJoinResult);
            ImportDll.AliRtcSetEventOnJoinChannelNotify(ImportDll.joinResultCB);

            ImportDll.RemoteUserOnLineNotifyCB = new ImportDll.setRemoteUserOnLineNotifyCB(onRemoteUserOnLineNotify);
            ImportDll.AliRtcSetEventOnRemoteUserOnLineNotify(ImportDll.RemoteUserOnLineNotifyCB);

            ImportDll.RemoteUserOffLineNotifyCB = new ImportDll.setRemoteUserOffLineNotifyCB(onRemoteUserOffLineNotify);
            ImportDll.AliRtcSetEventOnRemoteUserOffLineNotify(ImportDll.RemoteUserOffLineNotifyCB);

            ImportDll.RemoteTrackAvailableNotifyCB = new ImportDll.setRemoteTrackAvailableNotifyCB(onRemoteTrackAvailableNotify);
            ImportDll.AliRtcSetEventOnRemoteTrackAvailableNotify(ImportDll.RemoteTrackAvailableNotifyCB);

            ImportDll.ByeCB = new ImportDll.setByeCB(onBye);
            ImportDll.AliRtcSetEventOnByeNotify(ImportDll.ByeCB);

            ImportDll.OccurErrorCB = new ImportDll.setOccurErrorCB(onOccurError);
            ImportDll.AliRtcSetEventOnOccurErrorNotify(ImportDll.OccurErrorCB);


            ImportDll.AliRtcSetLogLevel(AliRTClogLevel.LOG_LEVEL_INFO);
            ImportDll.AliRtcPublishLocalAudioStream(m_ParamInfo.bPushAudio, m_ParamInfo.strChannel);
            ImportDll.AliRtcPublishLocalVideoStream(m_ParamInfo.bPushCamera, m_ParamInfo.strChannel);
            ImportDll.AliRtcSubscribeAllRemoteAudioStreams(m_ParamInfo.bAutoSub, m_ParamInfo.strChannel);
            ImportDll.AliRtcSubscribeAllRemoteVideoStreams(m_ParamInfo.bAutoSub, m_ParamInfo.strChannel);
            ImportDll.AliRtcPublishLocalDualStream(true, m_ParamInfo.strChannel);


            int nRet = ImportDll.AliRtcJoinChannel(autoinfo.strChannel,
                autoinfo.strUserId,
                autoinfo.strAppId,
                autoinfo.strNonce,
                autoinfo.strToken,
                autoinfo.strRole,
                autoinfo.strGlsb,
                autoinfo.strAgent,
                autoinfo.iTimesTamp,
                autoinfo.strUserName);

            if (0 != nRet)
            {
                listBox_Tips.Items.Add("加入房间失败...");
            }
        }

        private void button_Join_Click(object sender, EventArgs e)
        {
            ImportDll.AliRtcSetH5CompatibleMode(m_ParamInfo.bH5Mode);
            ImportDll.AliRtcCreateEngine("");
            updateParamInfo();
            string appServerUrl = m_ParamInfo.strAppSer;
            string channel = m_ParamInfo.strChannel;
            string userName = m_ParamInfo.strUserName;
            AuthInfo authinfo;//备注：需要手动输入鉴权信息 https://help.aliyun.com/document_detail/146833.html?spm=5176.11065259.1996646101.searchclickresult.230e1c95nxlEcb#title-t41-agh-m2i

            authinfo.strAppId = "xxxx";
            authinfo.strUserId = "xxxxxxxxx";
            authinfo.strChannel = "xxx";
            authinfo.strRole = "";
            authinfo.strNonce = "AK-xxxxxxxxxxxxxxxxxxxxxxxx";
            authinfo.strGlsb = "https://xxxxxxxxxxxxxxxxxx.com";
            authinfo.strAgent = "xxxxxxxxxxxxxxxxxxxxxxx.com";
            authinfo.strToken = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
            authinfo.iTimesTamp = 1621740670;
            authinfo.strUserName = m_ParamInfo.strUserName;

            ImportDll.AliRtcSetLocalViewConfig(panel_LocalView.Handle, 0, 0, 0, 0, 0, 0, 0, 0, true, AliRTCVideoTrack.VIDEO_TRACK_CAMERA);
 
            if (0 == ImportDll.AliRtcStartPreview())
                listBox_Tips.Items.Add("加载本地浏览成功...");
            else
                listBox_Tips.Items.Add("加载本地浏览失败...");

            dllJoinChannel(authinfo);
        }

        private void button_Leave_Click(object sender, EventArgs e)
        {
            ImportDll.AliRtcDestroyEngine();

            listBox_Tips.Items.Add("离开房间...");
        }

        private void AliRtcSample_SizeChanged(object sender, EventArgs e)
        {
            if (!m_bInitialItem)
                return;
            Layout_ParamItem();
            Layout_RemoteView_();
        }

        private void AliRtcSample_FormClosing(object sender, EventArgs e)
        {
            mainFrm = null;
        }

        private void btn_left_Click(object sender, EventArgs e)
        {
            if (m_viewInfoList.Count() == 0)
                return;
            viewInfo FirstViewInfo = m_viewInfoList.First();
            m_viewInfoList.RemoveFirst();
            m_viewInfoList.AddLast(FirstViewInfo);
            Layout_RemoteView_();
        }

        private void btn_right_Click(object sender, EventArgs e)
        {
            if (m_viewInfoList.Count() == 0)
                return;
            viewInfo LastViewInfo = m_viewInfoList.Last();
            m_viewInfoList.RemoveLast();
            m_viewInfoList.AddFirst(LastViewInfo);
            Layout_RemoteView_();
        }

        private void checkAudio_CheckedChanged(object sender, EventArgs e)
        {
            if (!m_bInitialItem)
                return;
            m_ParamInfo.bPushAudio = mainFrm.checkAudio.Checked;
            ImportDll.AliRtcPublishLocalAudioStream(m_ParamInfo.bPushAudio, m_ParamInfo.strChannel);
        }

        private void checkCamera_CheckedChanged(object sender, EventArgs e)
        {
            if (!m_bInitialItem)
                return;
            m_ParamInfo.bPushCamera = mainFrm.checkCamera.Checked;
            ImportDll.AliRtcPublishLocalVideoStream(m_ParamInfo.bPushCamera, m_ParamInfo.strChannel);
        }

        private void checkScreen_CheckedChanged(object sender, EventArgs e)
        {
            if (!m_bInitialItem)
                return;
            m_ParamInfo.bPushScreen = mainFrm.checkScreen.Checked;
            if (m_ParamInfo.bPushScreen)
            {
                string strScreenInfo = ImportDll.AliRtcGetScreenShareSourceInfo(AliRTCScreenShareType.AliRTCScreenShareDesktop, m_ParamInfo.strChannel);
                JArray jsonArray = JArray.Parse(strScreenInfo);
                if (jsonArray.Count() > 0)
                {
                    JObject objData = JObject.Parse(jsonArray[0].ToString());
                    string strId = objData["sourceId"].ToString();
                    ImportDll.AliRtcStartScreenShareByDesktopId(uint.Parse(strId), false, 0, 0, 0, 0, m_ParamInfo.strChannel);
                }
            }
            else
            {
                if (ImportDll.AliRtcIsScreenSharePublished(m_ParamInfo.strChannel))
                {
                    ImportDll.AliRtcStopScreenShare(m_ParamInfo.strChannel);
                }
            }
        }
    }
}