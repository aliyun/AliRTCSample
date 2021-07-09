#pragma once
#include "afxwin.h"

#include <map>
#include <vector>

#include "Log.h"
#include "engine_interface.h"
using namespace AliRTCSdk;
#pragma comment(lib, "AliRTCSdk.lib")

// CRTCSampleMainInterface 对话框

using namespace std;

#define TIMER_RETRIEVAL_CHANNEL  3001

struct sLoginInfo
{
	CString s_strServerAddr;
	CString s_strRoomID;
	CString s_strUserID;
	CString s_strPasswd;
	bool    s_bH5CompatibleMode;
	bool    s_bAutoSubscribe;
	bool    s_bAutoPublish;
	bool    s_bPushAudio;
	bool    s_bPushVideo;
	bool    s_bPushSimulcast;
	CString s_strRoomTitle;
};

struct sRemoteInfo
{
	String s_strRemoteUserUid;
	String s_strRemoteUserName;
	bool    s_bHsaScreenFlow;
	bool    s_bHsaCameraFlow;
	bool    s_bIsRecvAudio;
	bool    s_bIsRecvCamera;
	bool    s_bIsRecvScreen;
	bool    s_bBigStream;
	int     s_iReflectID;
	int     s_iReflectTips;
	bool    s_bIsReflect;
	int     s_iReflectIndex;
};

struct sChannelInfo
{
	int     s_iReflectID;
	int     s_iReflectTips;
	String s_strRemoteUserUid;
	AliEngineVideoTrack vt;
};

struct sUserInfo
{
	String s_strUserID;
	String s_strUserName;
	bool s_bIsVideoStream;
	bool s_bIsAudioStream;
	bool s_bIsScreenStream;
	bool s_bIsVideoBigStream;
	bool s_bIsVideoSmallStream;
};

struct subChangedParam {
	String uid;
	AliEngineAudioTrack at;
	AliEngineVideoTrack vt;
};

struct userOfflineParam {
	String uid;
};

struct userOnlineParam {
	String uid;
};

typedef enum {
	MoveForward = 1,  ///<前移
	BackwardShift = 2,  ///<后移
} RemoveType;

struct RemoteUserRelease
{
	bool s_bRemoteUserOnLineActioning;
	bool s_bRemoteUserOffLineActioning;
	bool s_bRemoteTrackAvailableActioning;
	bool s_bSubscribeChangedActioning;
};

struct RemoteUserCfg
{
	String s_strSetOccupyUid;
	bool           s_bIsRecVideo;
	bool           s_bHasBigStream;
	bool           s_bHasSmallStream;
	bool           s_bHasScreenStream;
	bool           s_bIsCameraMirror;
	bool           s_bIsScreenMirror;
};

class CRTCSampleMainInterface : public CDialogEx, AliEngineEventListener
{
	DECLARE_DYNAMIC(CRTCSampleMainInterface)

public:
	CRTCSampleMainInterface(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRTCSampleMainInterface();

public:
	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	/*                                        成  员  函  数                                       */
	/*                                       Member Functions                                      */
	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	// [功能函数(Functionality function)]
	bool JoinChannel();                                                                             // 加入频道(Join channel)
	//bool GetPassportFromAppServer(const char *pServer, const char *pRoomID,
	//	const char *pUserID, const char *pPasswd, AliRtcAuthInfo &AuthInfo);                        // 连接服务器(Connect servers)
	bool PreviewLocally(UINT ControlID);                                                            // 本地预览(Preview locally)
	void UpdateVideoView(String uid, UINT ViewID, UINT ControlID);                          //切换大小视图
	void stnDblclickStaticRoom(UINT ViewID);
	// [操作函数(Operability function)]
	void UpdateSubscribe(const String strUid, AliEngineAudioTrack audioTrack,
		AliEngineVideoTrack videoTrack);                                                               // 更新订阅流(Update subscription stream)
	void ShowSubscribe(const String uid, int iReflectIndex, AliEngineVideoTrack vt);           // 投映订阅流(Reflect subscription stream)
	void ClearChannel(const String uid, AliEngineVideoTrack videoTrack);                                                    // 清空频道（Clear reflect）
	void ReplaceSubscribeFlow(const String strOldUid, AliEngineVideoTrack oldvt,
		const String strNewUid, AliEngineVideoTrack newvt, int index);                         // 更换订阅流(Close subscription data)
	void ShowNumOfChannels();                                                                       // 展示频道在线人数(Number of Display Channels)
	void RemoveRemoteUser(const String strUid, RemoveType remove);                          // 移除离会用户（Remove Departure Users）
	void MoveThrowingScreen(int nHandle, int index);                                                // 移动投屏（Move Throwing Screen）
	void RightShift(int iLeftSideIndex, int iRightSideIndex);                                       // 投屏左移（Throwing Screen Right Shift）
	void LeftShift(int iLeftSideIndex, int iRightSideIndex);                                        // 投屏右移（Throwing Screen Left Shift）
	void FreeMobileControl();                                                                       // 自由移动控件（Free Mobile Control）
	void MoveConsoleCfg(int nWinWidth, int nWinHeight);                                             // 控制台配置控件组移动（Configuration control group movement on console）
	void MobeLocalView(int nWinWidth, int nWinHeight);                                              // 本地浏览控件组移动（Local Browse Control Group Mobility）
	void MoveConsole(int nWinWidth, int nWinHeight);                                                // 控制台控件组移动（Console Control Group Mobility）
	void MoveMonitor(int nWinWidth, int nWinHeight);                                                // 监控台控件组移动（Monitor Control Group Mobility）
	void MoveMonitorMarshalling(bool bMonitorState);                                                // 监控台编组（Monitoring station grouping）
	void MonitorSortingErrorReset();                                                                // 监控台排序错误（Monitor Sorting Error Reset）
	void RetrievalChannelOrder();                                                                   // 检索监视器顺序（Retrieval Channel Order）
	int  RetrievalChannelSortPos(int nReflectID);                                                   // 检索监视器顺序位置（Retrieval Channel Sort Location）
	void RemoteUserPreferMasterSwitching(Dictionary &dict);                                 // 远程用户大小流切换（Remote User Large and Small Stream Switching）
	void RemoteUserMirrorSwitching(Dictionary &dict);                                       // 远程用户镜像切换（Remote User Mirror Switching）


	// [工具函数(Instrumentality function)]
	vector<CString> SplitString(const string &s, const string &seperator);                          // 分割字符串(Split string)
	void MoveVecValPos(int index, bool bFlag);                                                      // 移动容器值位置（Moving container value position）
	String CStringToAliString(CString &s);                                                  // CString转AliString（CString To AliString）
	CString AliStringToCString(String &s);
	std::string toString(CString &cs);
	CString toCString(std::string &s);
	CString ObtainFilePath();                                                                       // 获取工程路径（Getting Engineering Path）
	int CheckServerLegalEntry(CString strServerEntry, int bFlag);                                   // 检验服务连接入参(Inspection Service Connection Reference）
	
	// [回调函数(Callback function)]
	void OnJoinChannelResult(int result, const char *channel, int elapsed) override;                                                            // 加入频道回调(Join channel function callback)
	virtual void OnRemoteUserOnLineNotify(const char *uid, int elapsed) override;                      // 入会通知(Remote user entry notification)
	virtual void OnRemoteUserOffLineNotify(const char *uid, AliEngineUserOfflineReason reason) override;                     // 离会通知(Remote user departure notification)
	virtual void OnRemoteTrackAvailableNotify(const char *uid,
		AliEngineAudioTrack audioTrack,
		AliEngineVideoTrack videoTrack) override;                         // 远端用户发布或停止发布(Distant user publishes or stops publishing)
	virtual void OnVideoSubscribeStateChanged(const char *uid,
		AliEngineSubscribeState oldState,
		AliEngineSubscribeState newState,
		int elapseSinceLastState,
		const char *channel) override;                         // 当订阅情况发生变化时(When subscriptions change)
    virtual void OnBye(int code) override;                                                          // 被服务器踢出频道的消息(Message kicked out of channel by server)
    virtual void OnAudioDeviceStateChanged(const AliEngineDeviceInfo& deviceInfo, AliEngineExternalDeviceType deviceType, AliEngineExternalDeviceState deviceState) override;                                                  // 外接设备状态变更(State Change of External Equipment)

	// [消息函数（Message function）]
	afx_msg LRESULT onRemoteTrackAvailableNotifyMT(WPARAM wParam, LPARAM lParam);                   // 消息队列 - 做订阅发布和停止发布的相关操作（Message queue for subscription publishing and stop publishing）
	afx_msg LRESULT onRemoteUserOffLineNotifyMT(WPARAM wParam, LPARAM lParam);                      // 消息队列 - 用户离会的相关操作（Message queue for User departure）
	afx_msg LRESULT onFreeMobileControl(WPARAM wParam, LPARAM lParam);                              // 消息队列 - 自由移动控件（Free Mobile Control）

	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	/*                                          成  员  变  量                                     */
	/*                                         Member variables                                    */
	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	CBrush m_Brush;
	CFont  m_Font_SmallRoom, m_Font_MainRoom;

	sLoginInfo m_sLoginInfo;                                                                        // 登录信息
	AliEngine *m_pEngine;                                                                        // RTC SDK类

	UINT m_nConsoleOccupancy;                                                                       // 控制台占用

	map<CString, sRemoteInfo> m_mapRemoteInfo;                                                      // 远端用户信息
	vector<String> m_vecRemoteInfoList;                                                     // 用户流队列
	vector<sChannelInfo> m_vecThrowingScreenInfo;                                                   // 投屏队列

	int m_iLeftSideIndex;                                                                           // 投屏左索引
	int m_iRightSideIndex;                                                                          // 投屏右索引
	
	bool m_bMoving;                                                                                 // 操作防止频繁
	int m_iChangeChannelIndex;                                                                      // 交换投屏索引

	RemoteUserRelease m_sRemoteUserRelease;

	RemoteUserCfg m_sRemoteUserCfg;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MAIN_INTERFACE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnStnDblclickStaticRoom1();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CString m_strRtcOnline;
	CString m_strConsole;
	CString m_strVideo1;
	CString m_strVideo2;
	CString m_strVideo3;
	CString m_strVideo4;
	CString m_strVideo5;
	afx_msg void OnStnClickedStaticLocalSetting();
	afx_msg void OnStnDblclickStaticRoom2();
	afx_msg void OnStnDblclickStaticRoom3();
	afx_msg void OnStnDblclickStaticRoom4();
	afx_msg void OnStnDblclickStaticRoom5();
	afx_msg void OnStnDblclickStaticLocalVideo();
	CString m_strSetting;
	afx_msg void OnBnClickedButtonExitRoom();

private:
	HICON m_hIcon;

public:
	afx_msg void OnStnDblclickStaticMainroom();
	afx_msg void OnBnClickedButtonOnekeyNovideo();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCheckEnableScreen();
	CButton m_IsEnableScreen;
	afx_msg void OnBnClickedButtonOnekeyMute();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedRadioClear();
	afx_msg void OnBnClickedRadioFluent();
	afx_msg void OnBnClickedButtonRight();
	afx_msg void OnBnClickedButtonLeft();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CStatic m_ctlMainRoom;
	CStatic m_ctlSetting;
	CStatic m_ctlConsole;
	CButton m_ctlButton;
	CButton m_ctlButton_OnekeyNovide;
	CButton m_ctlButton_ExitRoom;
	CStatic m_ctlFlow;
	CStatic m_ctlMirror;
	CStatic m_ctlQuality;
	CStatic m_ctlScene;
	CButton m_ctlCheckVideo;
	CButton m_ctlCheckScreenShare;
	CButton m_ctlCheckAudio;
	CButton m_ctlCheckCameraMirror;
	CButton m_ctlCheckScreenMirror;
	CButton m_ctlCloseMicrophone;
	CButton m_ctlCloseVideo;
	CStatic m_ctlLocalSetting;
	CStatic m_ctlRtconline;
	CStatic m_ctlLocalVideo;
	CStatic m_ctlLocalView;
	CStatic m_ctlVideo1;
	CStatic m_ctlVideo2;
	CStatic m_ctlVideo3;
	CStatic m_ctlVideo4;
	CStatic m_ctlVideo5;
	CStatic m_ctlRoom1;
	CStatic m_ctlRoom2;
	CStatic m_ctlRoom3;
	CStatic m_ctlRoom4;
	CStatic m_ctlRoom5;
	CButton m_ctlButtonLeft;
	CButton m_ctlButtonRight;
	afx_msg void OnBnClickedCheckCameraMirror();
	afx_msg void OnBnClickedCheckScreenMirror();
};
