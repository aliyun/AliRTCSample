// RTCSampleMainInterface.cpp : 实现文件
//

#include "stdafx.h"
#include "RtcSample.h"
#include "RTCSampleMainInterface.h"
#include "afxdialogex.h"

#include "CURL/curl.h"
#include "JSON/json.h"

#define MSG_SUBSCRIBE_CHANGED (WM_USER + 0x234)
#define MSG_USER_OFFLINE      (WM_USER + 0x235)
#define MSG_USER_CHANGESIZE   (WM_USER + 0x237)

// CRTCSampleMainInterface 对话框

IMPLEMENT_DYNAMIC(CRTCSampleMainInterface, CDialogEx)

CRTCSampleMainInterface::CRTCSampleMainInterface(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MAIN_INTERFACE, pParent)
	, m_strRtcOnline(_T("RTC通信 [在线：0]"))
	, m_strConsole(_T("控制台 [占用:--]"))
	, m_strVideo1(_T(""))
	, m_strVideo2(_T(""))
	, m_strVideo3(_T(""))
	, m_strVideo4(_T(""))
	, m_strVideo5(_T(""))
	, m_strSetting(_T("控制台配置 [占用:--]"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_LOGIN);
	m_nConsoleOccupancy = IDC_STATIC_MAINROOM;

	sChannelInfo ChannelInfo;
	ChannelInfo.s_iReflectID = IDC_STATIC_ROOM1;
	ChannelInfo.s_iReflectTips = IDC_STATIC_VIDEO1;
	ChannelInfo.s_strRemoteUserUid = "";
	m_vecThrowingScreenInfo.push_back(ChannelInfo);

	ChannelInfo.s_iReflectID = IDC_STATIC_ROOM2;
	ChannelInfo.s_iReflectTips = IDC_STATIC_VIDEO2;
	m_vecThrowingScreenInfo.push_back(ChannelInfo);

	ChannelInfo.s_iReflectID = IDC_STATIC_ROOM3;
	ChannelInfo.s_iReflectTips = IDC_STATIC_VIDEO3;
	m_vecThrowingScreenInfo.push_back(ChannelInfo);

	ChannelInfo.s_iReflectID = IDC_STATIC_ROOM4;
	ChannelInfo.s_iReflectTips = IDC_STATIC_VIDEO4;
	m_vecThrowingScreenInfo.push_back(ChannelInfo);

	ChannelInfo.s_iReflectID = IDC_STATIC_ROOM5;
	ChannelInfo.s_iReflectTips = IDC_STATIC_VIDEO5;
	m_vecThrowingScreenInfo.push_back(ChannelInfo);


	m_iLeftSideIndex = 0;
	m_iRightSideIndex = 0;

	m_bMoving = false;
	m_iChangeChannelIndex = 0;

	m_sRemoteUserRelease.s_bRemoteTrackAvailableActioning = false;
	m_sRemoteUserRelease.s_bRemoteUserOffLineActioning = false;
	m_sRemoteUserRelease.s_bRemoteUserOnLineActioning = false;
	m_sRemoteUserRelease.s_bSubscribeChangedActioning = false;
}

CRTCSampleMainInterface::~CRTCSampleMainInterface()
{
}

void CRTCSampleMainInterface::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_RTC_ONLINE, m_strRtcOnline);
	DDX_Text(pDX, IDC_STATIC_CONSOLE, m_strConsole);
	DDX_Text(pDX, IDC_STATIC_VIDEO1, m_strVideo1);
	DDX_Text(pDX, IDC_STATIC_VIDEO2, m_strVideo2);
	DDX_Text(pDX, IDC_STATIC_VIDEO3, m_strVideo3);
	DDX_Text(pDX, IDC_STATIC_VIDEO4, m_strVideo4);
	DDX_Text(pDX, IDC_STATIC_VIDEO5, m_strVideo5);
	DDX_Text(pDX, IDC_STATIC_SETTING, m_strSetting);
	DDX_Control(pDX, IDC_CHECK_ENABLE_SCREEN, m_IsEnableScreen);
	DDX_Control(pDX, IDC_STATIC_MAINROOM, m_ctlMainRoom);
	DDX_Control(pDX, IDC_STATIC_SETTING, m_ctlSetting);
	DDX_Control(pDX, IDC_STATIC_CONSOLE, m_ctlConsole);
	DDX_Control(pDX, IDC_BUTTON_ONEKEY_MUTE, m_ctlButton);
	DDX_Control(pDX, IDC_BUTTON_ONEKEY_NOVIDEO, m_ctlButton_OnekeyNovide);
	DDX_Control(pDX, IDC_BUTTON_EXIT_ROOM, m_ctlButton_ExitRoom);
	DDX_Control(pDX, IDC_STATIC_FLOW, m_ctlFlow);
	DDX_Control(pDX, IDC_STATIC_MIRROR, m_ctlMirror);
	DDX_Control(pDX, IDC_STATIC_QUALITY, m_ctlQuality);
	DDX_Control(pDX, IDC_STATIC_SCENE, m_ctlScene);
	DDX_Control(pDX, IDC_CHECK_VIDEO, m_ctlCheckVideo);
	DDX_Control(pDX, IDC_CHECK_SCREEN_SHARE, m_ctlCheckScreenShare);
	DDX_Control(pDX, IDC_CHECK_AUDIO, m_ctlCheckAudio);
	DDX_Control(pDX, IDC_CHECK_CAMERA_MIRROR, m_ctlCheckCameraMirror);
	DDX_Control(pDX, IDC_CHECK_SCREEN_MIRROR, m_ctlCheckScreenMirror);
	DDX_Control(pDX, IDC_CHECK_CLOSE_MICROPHONE, m_ctlCloseMicrophone);
	DDX_Control(pDX, IDC_CHECK_CLOSE_VIDEO, m_ctlCloseVideo);
	DDX_Control(pDX, IDC_STATIC_LOCAL_SETTING, m_ctlLocalSetting);
	DDX_Control(pDX, IDC_STATIC_RTC_ONLINE, m_ctlRtconline);
	DDX_Control(pDX, IDC_STATIC_LOCAL_VIDEO, m_ctlLocalVideo);
	DDX_Control(pDX, IDC_STATIC_LOCALVIEW, m_ctlLocalView);
	DDX_Control(pDX, IDC_STATIC_VIDEO1, m_ctlVideo1);
	DDX_Control(pDX, IDC_STATIC_VIDEO2, m_ctlVideo2);
	DDX_Control(pDX, IDC_STATIC_VIDEO3, m_ctlVideo3);
	DDX_Control(pDX, IDC_STATIC_VIDEO4, m_ctlVideo4);
	DDX_Control(pDX, IDC_STATIC_VIDEO5, m_ctlVideo5);
	DDX_Control(pDX, IDC_STATIC_ROOM1, m_ctlRoom1);
	DDX_Control(pDX, IDC_STATIC_ROOM2, m_ctlRoom2);
	DDX_Control(pDX, IDC_STATIC_ROOM3, m_ctlRoom3);
	DDX_Control(pDX, IDC_STATIC_ROOM4, m_ctlRoom4);
	DDX_Control(pDX, IDC_STATIC_ROOM5, m_ctlRoom5);
	DDX_Control(pDX, IDC_BUTTON_LEFT, m_ctlButtonLeft);
	DDX_Control(pDX, IDC_BUTTON_RIGHT, m_ctlButtonRight);
}


BEGIN_MESSAGE_MAP(CRTCSampleMainInterface, CDialogEx)
	ON_STN_DBLCLK(IDC_STATIC_ROOM1, &CRTCSampleMainInterface::OnStnDblclickStaticRoom1)
	ON_WM_CTLCOLOR()
	ON_WM_HSCROLL()
	ON_STN_CLICKED(IDC_STATIC_LOCAL_SETTING, &CRTCSampleMainInterface::OnStnClickedStaticLocalSetting)
	ON_STN_DBLCLK(IDC_STATIC_ROOM2, &CRTCSampleMainInterface::OnStnDblclickStaticRoom2)
	ON_STN_DBLCLK(IDC_STATIC_ROOM3, &CRTCSampleMainInterface::OnStnDblclickStaticRoom3)
	ON_STN_DBLCLK(IDC_STATIC_ROOM4, &CRTCSampleMainInterface::OnStnDblclickStaticRoom4)
	ON_STN_DBLCLK(IDC_STATIC_ROOM5, &CRTCSampleMainInterface::OnStnDblclickStaticRoom5)
	ON_STN_DBLCLK(IDC_STATIC_LOCAL_VIDEO, &CRTCSampleMainInterface::OnStnDblclickStaticLocalVideo)
	ON_BN_CLICKED(IDC_BUTTON_EXIT_ROOM, &CRTCSampleMainInterface::OnBnClickedButtonExitRoom)
	ON_STN_DBLCLK(IDC_STATIC_MAINROOM, &CRTCSampleMainInterface::OnStnDblclickStaticMainroom)
	ON_BN_CLICKED(IDC_BUTTON_ONEKEY_NOVIDEO, &CRTCSampleMainInterface::OnBnClickedButtonOnekeyNovideo)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_ENABLE_SCREEN, &CRTCSampleMainInterface::OnBnClickedCheckEnableScreen)
	ON_BN_CLICKED(IDC_BUTTON_ONEKEY_MUTE, &CRTCSampleMainInterface::OnBnClickedButtonOnekeyMute)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_CLEAR, &CRTCSampleMainInterface::OnBnClickedRadioClear)
	ON_BN_CLICKED(IDC_RADIO_FLUENT, &CRTCSampleMainInterface::OnBnClickedRadioFluent)
	ON_MESSAGE(MSG_SUBSCRIBE_CHANGED, onRemoteTrackAvailableNotifyMT)
	ON_MESSAGE(MSG_USER_OFFLINE, onRemoteUserOffLineNotifyMT)
	ON_MESSAGE(MSG_USER_CHANGESIZE, onFreeMobileControl)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, &CRTCSampleMainInterface::OnBnClickedButtonRight)
	ON_BN_CLICKED(IDC_BUTTON_LEFT, &CRTCSampleMainInterface::OnBnClickedButtonLeft)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_CAMERA_MIRROR, &CRTCSampleMainInterface::OnBnClickedCheckCameraMirror)
	ON_BN_CLICKED(IDC_CHECK_SCREEN_MIRROR, &CRTCSampleMainInterface::OnBnClickedCheckScreenMirror)
END_MESSAGE_MAP()


// CRTCSampleMainInterface 消息处理程序

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                       事  件  函  数                                        */
/*                                       Event Function                                        */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 初始化事件(Initialization events)
BOOL CRTCSampleMainInterface::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化
	m_mapRemoteInfo.clear();
	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);

	if ("" == m_sLoginInfo.s_strRoomID || "" == m_sLoginInfo.s_strServerAddr || "" == m_sLoginInfo.s_strUserID)
	{
		WRILOG(LOGTYPE_ERROR, RTC_DEBUG_LOG, "OnInitDialog", "非法参数...[ServerAddr：%s  RoomID：%s  UserID：%s]", m_sLoginInfo.s_strServerAddr, m_sLoginInfo.s_strRoomID, m_sLoginInfo.s_strUserID);
		goto EXIT;
	}

	SetWindowText(m_sLoginInfo.s_strRoomTitle);

	m_Brush.CreateSolidBrush(RGB(30, 144, 255));
	m_Font_SmallRoom.CreatePointFont(150, "华为楷体");
	m_Font_MainRoom.CreatePointFont(180, "华为楷体");

	if (TRUE == m_sLoginInfo.s_bH5CompatibleMode)
		AliEngine::SetH5CompatibleMode(true);

	m_pEngine = AliEngine::Create("");

	if (m_pEngine == nullptr)
	{
		WRILOG(LOGTYPE_ERROR, RTC_DEBUG_LOG, "OnInitDialog", "[sharedInstance] SDK初始化失败...");
		goto EXIT;
	}

	WRILOG(LOGTYPE_DEBUG, RTC_DEBUG_LOG, "OnInitDialog", "[sharedInstance] SDK初始化成功...");
	m_pEngine->SetEngineEventListener(this);
	bool bRet = JoinChannel();

	if (true != bRet)
	{
		goto EXIT;
	}

	bRet = PreviewLocally(IDC_STATIC_LOCAL_VIDEO);

	if (true != bRet)
	{
		WRILOG(LOGTYPE_ERROR, RTC_DEBUG_LOG, "OnInitDialog", "[BrowseLocal] 本地预览失败...");
	}

	WRILOG(LOGTYPE_DEBUG, RTC_DEBUG_LOG, "OnInitDialog", "[BrowseLocal] 本地预览成功...");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE

EXIT:
	EndDialog(FALSE);
	CString strErrorInfo;
	strErrorInfo.Format("加入频道失败!\r\n请查看日志文件[%sLOG\\当前日期\\RTC_DEBUG.LOG]...", ObtainFilePath());
	AfxMessageBox(strErrorInfo);

	return FALSE;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 消息事件(Message events)
BOOL CRTCSampleMainInterface::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 销毁事件(Destruction events)
BOOL CRTCSampleMainInterface::DestroyWindow()
{
	if (true == m_sRemoteUserRelease.s_bRemoteTrackAvailableActioning)
		AfxMessageBox("RemoteTrackAvailable 线程未退出...");

	if(true == m_sRemoteUserRelease.s_bRemoteUserOffLineActioning)
		AfxMessageBox("RemoteUserOffLine 线程未退出...");

	if(true == m_sRemoteUserRelease.s_bRemoteUserOnLineActioning)
		AfxMessageBox("RemoteUserOnLine 线程未退出...");

	if(true == m_sRemoteUserRelease.s_bSubscribeChangedActioning)
		AfxMessageBox("SubscribeChangedNotify 线程未退出...");

	m_pEngine->StopPreview();
	m_pEngine->StopScreenShare();

	m_pEngine->LeaveChannel();
    AliEngine::Destroy();
    m_pEngine = nullptr;

	WRILOG(LOGTYPE_DEBUG, RTC_DEBUG_LOG, "退出房间..........", "[房间：%s][用户名：%s]", m_sLoginInfo.s_strRoomID, m_sLoginInfo.s_strUserID);

	m_mapRemoteInfo.clear(); 

	return CDialogEx::DestroyWindow();
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 窗口颜色控制事件(Window color control events)
HBRUSH CRTCSampleMainInterface::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_ROOM1)
	{
		pDC->SetBkColor(RGB(30, 144, 255));
		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SelectObject(&m_Font_SmallRoom);

		return m_Brush;
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_ROOM2)
	{
		pDC->SetBkColor(RGB(30, 144, 255));
		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SelectObject(&m_Font_SmallRoom);

		return m_Brush;
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_ROOM3)
	{
		pDC->SetBkColor(RGB(30, 144, 255));
		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SelectObject(&m_Font_SmallRoom);

		return m_Brush;
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_ROOM4)
	{
		pDC->SetBkColor(RGB(30, 144, 255));
		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SelectObject(&m_Font_SmallRoom);

		return m_Brush;
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_ROOM5)
	{
		pDC->SetBkColor(RGB(30, 144, 255));
		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SelectObject(&m_Font_SmallRoom);

		return m_Brush;
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_MAINROOM)
	{
		pDC->SetBkColor(RGB(30, 144, 255));
		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SelectObject(&m_Font_MainRoom);

		return m_Brush;
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_LOCAL_VIDEO)
	{
		pDC->SetBkColor(RGB(30, 144, 255));
		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SelectObject(&m_Font_SmallRoom);

		return m_Brush;
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 计时器事件(Timer events)
void CRTCSampleMainInterface::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_RETRIEVAL_CHANNEL:
		RetrievalChannelOrder();
		break;
	}

	__super::OnTimer(nIDEvent);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 绘图事件(Drawing events)
void CRTCSampleMainInterface::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 图标事件(Icon events)
HCURSOR CRTCSampleMainInterface::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 窗口大小改变事件(Icon events)
void CRTCSampleMainInterface::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	FreeMobileControl();
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                    Interface Button Operation                               */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 投屏1 双击事件(Video1 Dblclick events)
void CRTCSampleMainInterface::OnStnDblclickStaticRoom1()
{
	stnDblclickStaticRoom(IDC_STATIC_ROOM1);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 投屏2 双击事件(Video2 Dblclick events)
void CRTCSampleMainInterface::OnStnDblclickStaticRoom2()
{
	stnDblclickStaticRoom(IDC_STATIC_ROOM2);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 投屏3 双击事件(Video3 Dblclick events)
void CRTCSampleMainInterface::OnStnDblclickStaticRoom3()
{
	stnDblclickStaticRoom(IDC_STATIC_ROOM3);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 投屏4 双击事件(Video4 Dblclick events)
void CRTCSampleMainInterface::OnStnDblclickStaticRoom4()
{
	stnDblclickStaticRoom(IDC_STATIC_ROOM4);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 投屏5 双击事件(Video5 Dblclick events)
void CRTCSampleMainInterface::OnStnDblclickStaticRoom5()
{
	stnDblclickStaticRoom(IDC_STATIC_ROOM5);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 本地预览 双击事件(Browse locally Dblclick events)
void CRTCSampleMainInterface::OnStnDblclickStaticLocalVideo()
{
	GetDlgItem(IDC_STATIC_CONSOLE)->SetWindowText("控制台 [占用:本地预览]");
	UpdateVideoView("", IDC_STATIC_LOCAL_VIDEO, IDC_STATIC_MAINROOM);
	//m_nConsoleOccupancy = IDC_STATIC_LOCAL_VIDEO;

	//GetDlgItem(IDC_STATIC_CONSOLE)->SetWindowTextA("控制台 [占用:本地预览]");
	//
	//PreviewLocally(IDC_STATIC_MAINROOM);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 控制台 双击事件(Console Dblclick events)
void CRTCSampleMainInterface::OnStnDblclickStaticMainroom()
{
	if (IDC_STATIC_MAINROOM == m_nConsoleOccupancy)
	{
		return;
	}

	UpdateVideoView("", IDC_STATIC_MAINROOM, IDC_STATIC_MAINROOM);

	((CButton *)GetDlgItem(IDC_RADIO_CLEAR))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_FLUENT))->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_CONSOLE)->SetWindowText("控制台 [占用:--]");
	GetDlgItem(IDC_STATIC_MAINROOM)->SetWindowText("无信号");
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 本地预览配置 单击事件(Local Browse Configuration Click events)
void CRTCSampleMainInterface::OnStnClickedStaticLocalSetting()
{
	
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 退出频道按钮(Exit channel button)
void CRTCSampleMainInterface::OnBnClickedButtonExitRoom()
{
	m_sLoginInfo.s_bH5CompatibleMode = FALSE;
	m_sLoginInfo.s_strRoomID = "";
	m_sLoginInfo.s_strServerAddr = "";
	m_sLoginInfo.s_strUserID = "";
	
	CDialogEx::OnOK();
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 一键静音按钮(One key mute button)
void CRTCSampleMainInterface::OnBnClickedButtonOnekeyMute()
{

}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 一键闭屏按钮(One key closed screen)
void CRTCSampleMainInterface::OnBnClickedButtonOnekeyNovideo()
{
	
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 切换大流(Switching large flow)
void CRTCSampleMainInterface::OnBnClickedRadioClear()
{
    if (m_pEngine == nullptr)
        return;

	((CButton *)GetDlgItem(IDC_RADIO_FLUENT))->SetCheck(FALSE);
    if (0 == m_mapRemoteInfo.count(m_sRemoteUserCfg.s_strSetOccupyUid.c_str()))
        return;

	m_mapRemoteInfo[m_sRemoteUserCfg.s_strSetOccupyUid.c_str()].s_bBigStream = true;
	m_pEngine->SetRemoteVideoStreamType(m_sRemoteUserCfg.s_strSetOccupyUid.c_str(), AliEngineVideoStreamTypeHigh);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 切换小流(Switching small flow)
void CRTCSampleMainInterface::OnBnClickedRadioFluent()
{
    if (m_pEngine == nullptr)
        return;

	((CButton *)GetDlgItem(IDC_RADIO_CLEAR))->SetCheck(FALSE);

	m_mapRemoteInfo[m_sRemoteUserCfg.s_strSetOccupyUid.c_str()].s_bBigStream = false;
	m_pEngine->SetRemoteVideoStreamType(m_sRemoteUserCfg.s_strSetOccupyUid.c_str(), AliEngineVideoStreamTypeLow);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 切换视频镜像
void CRTCSampleMainInterface::OnBnClickedCheckCameraMirror()
{
    if (m_pEngine == nullptr)
        return;
	if (m_vecRemoteInfoList.empty())
		return;

	bool bIsCheck = ((CButton *)GetDlgItem(IDC_CHECK_CAMERA_MIRROR))->GetCheck() == BST_CHECKED;

	AliEngineVideoCanvas canvas ;
	canvas.displayView = GetDlgItem(m_vecThrowingScreenInfo[m_mapRemoteInfo[m_vecRemoteInfoList.at(0).c_str()].s_iReflectIndex].s_iReflectID)->m_hWnd;
	canvas.mirrorMode = bIsCheck ? AliEngineRenderMirrorModeAllMirror : AliEngineRenderMirrorModeAllNoMirror;
	m_pEngine->SetRemoteViewConfig(canvas, m_sRemoteUserCfg.s_strSetOccupyUid.c_str(), AliEngineVideoTrackCamera);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 切换屏幕共享镜像
void CRTCSampleMainInterface::OnBnClickedCheckScreenMirror()
{
    if (m_pEngine == nullptr)
        return;

	if (m_vecRemoteInfoList.empty() || !m_mapRemoteInfo[m_vecRemoteInfoList.at(0).c_str()].s_bHsaScreenFlow)
		return;
	bool bIsCheck = ((CButton *)GetDlgItem(IDC_CHECK_SCREEN_MIRROR))->GetCheck() == BST_CHECKED;

	AliEngineVideoCanvas canvas;
	canvas.displayView = GetDlgItem(m_vecThrowingScreenInfo[m_mapRemoteInfo[m_vecRemoteInfoList.at(0).c_str()].s_iReflectIndex + 1].s_iReflectID)->m_hWnd;
	canvas.mirrorMode = bIsCheck ? AliEngineRenderMirrorModeAllMirror : AliEngineRenderMirrorModeAllNoMirror;
	m_pEngine->SetRemoteViewConfig(canvas, m_sRemoteUserCfg.s_strSetOccupyUid.c_str(), AliEngineVideoTrackScreen);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 监视器右移
void CRTCSampleMainInterface::OnBnClickedButtonRight()
{
	if (true == m_sRemoteUserRelease.s_bRemoteUserOffLineActioning || true == m_sRemoteUserRelease.s_bRemoteUserOnLineActioning) return;
	if (true == m_bMoving) return;

	m_bMoving = true;

	if (m_vecRemoteInfoList.size() <= 5 || m_vecRemoteInfoList.size() == (m_iRightSideIndex + 1))
	{
		GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(false);
		return;
	}

	RightShift(++m_iLeftSideIndex, ++m_iRightSideIndex);
	SetTimer(TIMER_RETRIEVAL_CHANNEL, 500, NULL);

	if (m_vecRemoteInfoList.size() == (m_iRightSideIndex + 1))
	{
		GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(false);
	}

	GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(true);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 监视器左移
void CRTCSampleMainInterface::OnBnClickedButtonLeft()
{
	if (true == m_sRemoteUserRelease.s_bRemoteUserOffLineActioning || true == m_sRemoteUserRelease.s_bRemoteUserOnLineActioning) return;
	if (true == m_bMoving) return;

	m_bMoving = true;

	if (m_vecRemoteInfoList.size() <= 5 || m_iLeftSideIndex == 0)
	{
		GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(false);
		return;
	}

	LeftShift(--m_iLeftSideIndex, --m_iRightSideIndex);
	SetTimer(TIMER_RETRIEVAL_CHANNEL, 500, NULL);

	if (m_iLeftSideIndex == 0)
	{
		GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(false);
	}

	GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(true);
}


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                         RTC 回调函数                                        */
/*                                         RTC Callback                                        */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 入会通知(Remote user entry notification)
void CRTCSampleMainInterface::OnRemoteUserOnLineNotify(const char *uid, int elapsed)
{
	m_sRemoteUserRelease.s_bRemoteUserOnLineActioning = true;
	WRILOG(LOGTYPE_DEBUG, "DEBUG3.LOG", "onRemoteUserOnLineNotify", "1远端入会开始[uid:%s]", uid);
	Dictionary dict;
	m_pEngine->GetUserInfo(uid, dict);

	sRemoteInfo RemoteInfo;
	m_mapRemoteInfo.insert(pair<CString, sRemoteInfo>(uid, RemoteInfo));
	m_mapRemoteInfo[uid].s_bBigStream = true;
	m_mapRemoteInfo[uid].s_bIsRecvAudio = true;
	m_mapRemoteInfo[uid].s_bIsRecvCamera = true;
	m_mapRemoteInfo[uid].s_bIsRecvScreen = true;
	m_mapRemoteInfo[uid].s_bHsaCameraFlow = false;
	m_mapRemoteInfo[uid].s_bHsaScreenFlow = false;
	m_mapRemoteInfo[uid].s_bIsReflect = false;

	m_mapRemoteInfo[uid].s_strRemoteUserName = dict.getValue("displayName");
	m_mapRemoteInfo[uid].s_strRemoteUserUid = dict.getValue("userID");

	m_vecRemoteInfoList.push_back(uid);
	WRILOG(LOGTYPE_DEBUG, "DEBUG3.LOG", "onRemoteUserOnLineNotify", "2远端入会[uid:%s][在线人数：%d]", uid, m_mapRemoteInfo.size());
	ShowNumOfChannels();
	WRILOG(LOGTYPE_DEBUG, RTC_DEBUG_LOG, "onRemoteUserOnLineNotify", "远端入会[uid：%s][在线人数：%d]", uid, m_mapRemoteInfo.size());

	CString str,strUser;
	for (int i = 0; i < (int)m_vecRemoteInfoList.size(); i++)
	{
		if (strUser != m_vecRemoteInfoList[i].c_str())
		{
			strUser = m_vecRemoteInfoList[i].c_str();
			str.Format("%s\r\n%s", str, strUser);
		}		
	}
	WRILOG(LOGTYPE_DEBUG, "DEBUG2.LOG", "本次人数", "%s", str);

	m_sRemoteUserRelease.s_bRemoteUserOnLineActioning = false;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 离会通知(Remote user departure notification)
void CRTCSampleMainInterface::OnRemoteUserOffLineNotify(const char *uid, AliEngineUserOfflineReason reason)
{
	userOfflineParam *RemoteUserOffLine = new userOfflineParam;
	RemoteUserOffLine->uid = uid;
	PostMessage(MSG_USER_OFFLINE, 0, (LPARAM)RemoteUserOffLine);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*///消息队列里用户离会的相关操作（Message queue for User departure）
LRESULT CRTCSampleMainInterface::onRemoteUserOffLineNotifyMT(WPARAM wParam, LPARAM lParam)
{
	userOfflineParam *RemoteUserOffLine = (userOfflineParam *)lParam;

	if (0 == m_mapRemoteInfo.count(RemoteUserOffLine->uid.c_str())) return -1;

	WRILOG(LOGTYPE_DEBUG, "DEBUG4.LOG", "onRemoteUserOffLineNotifyMT", "1远端离会[uid:%s]", RemoteUserOffLine->uid.c_str());
	m_mapRemoteInfo.erase(RemoteUserOffLine->uid.c_str());

	for (vector<String>::iterator DelVecIter = m_vecRemoteInfoList.begin(); DelVecIter != m_vecRemoteInfoList.end(); DelVecIter++)
	{
		if (*DelVecIter == RemoteUserOffLine->uid)
		{
			m_vecRemoteInfoList.erase(DelVecIter);
			break;
		}
	}
	WRILOG(LOGTYPE_DEBUG, "DEBUG4.LOG", "onRemoteUserOffLineNotifyMT", "2.1远端离会[uid:%s][在线人数：%d]", RemoteUserOffLine->uid.c_str(), m_mapRemoteInfo.size());
	if (m_vecRemoteInfoList.size() > 5)
	{
		if (m_iRightSideIndex == m_vecRemoteInfoList.size())
		{
			m_iLeftSideIndex--;
			m_iRightSideIndex--;
			RemoveRemoteUser(RemoteUserOffLine->uid, MoveForward);

			if (m_iLeftSideIndex < 0)
				m_iLeftSideIndex = 0;
				
			ShowSubscribe(m_vecRemoteInfoList[m_iLeftSideIndex], 0, AliEngineVideoTrackCamera);
			WRILOG(LOGTYPE_DEBUG, "DEBUG4.LOG", "onRemoteUserOffLineNotifyMT", "3.1远端离会  m_iRightSideIndex == m_vecRemoteInfoList.size()[%d == %d]", m_iRightSideIndex, m_vecRemoteInfoList.size());
		}
		else
		{
			RemoveRemoteUser(RemoteUserOffLine->uid, BackwardShift);

			if (m_iRightSideIndex >= m_vecRemoteInfoList.size())
				m_iRightSideIndex = m_vecRemoteInfoList.size() - 1;

			ShowSubscribe(m_vecRemoteInfoList[m_iRightSideIndex], 4, AliEngineVideoTrackCamera);
			WRILOG(LOGTYPE_DEBUG, "DEBUG4.LOG", "onRemoteUserOffLineNotifyMT", "3.2远端离会[%d - %d]", m_iRightSideIndex, m_vecRemoteInfoList.size());

		}
	}
	else if (m_vecRemoteInfoList.size() == 5)
	{
		RemoveRemoteUser(RemoteUserOffLine->uid, BackwardShift);
		ShowSubscribe(m_vecRemoteInfoList[4], 4, AliEngineVideoTrackCamera);

		GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(false);
		WRILOG(LOGTYPE_DEBUG, "DEBUG4.LOG", "onRemoteUserOffLineNotifyMT", "4远端离会 m_vecRemoteInfoList.size() == 5[%d]", m_vecRemoteInfoList.size());
	}
	else
	{
		RemoveRemoteUser(RemoteUserOffLine->uid, BackwardShift);
		SetTimer(TIMER_RETRIEVAL_CHANNEL, 500, NULL);
		GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(false);

		WRILOG(LOGTYPE_DEBUG, "DEBUG4.LOG", "onRemoteUserOffLineNotifyMT", "5远端离会 m_vecRemoteInfoList.size() < 5[%d]", m_vecRemoteInfoList.size());

		WRILOG(LOGTYPE_DEBUG, "DEBUG1.LOG", "", "【%s】-【%s】-【%s】-【%s】-【%s】", m_vecThrowingScreenInfo[0].s_strRemoteUserUid.c_str(),
			m_vecThrowingScreenInfo[1].s_strRemoteUserUid.c_str(),
			m_vecThrowingScreenInfo[2].s_strRemoteUserUid.c_str(),
			m_vecThrowingScreenInfo[3].s_strRemoteUserUid.c_str(),
			m_vecThrowingScreenInfo[4].s_strRemoteUserUid.c_str());
	}

	ShowNumOfChannels();
	WRILOG(LOGTYPE_DEBUG, "DEBUG4.LOG", "onRemoteUserOffLineNotifyMT", "6远端离会[uid:%s]ShowNumOfChannels后", RemoteUserOffLine->uid.c_str());
	WRILOG(LOGTYPE_DEBUG, RTC_DEBUG_LOG, "onRemoteUserOffLineNotify", "远端离会[uid：%s][在线人数：%d]", RemoteUserOffLine->uid.c_str(), m_mapRemoteInfo.size());

	CString str, strUser;
	for (int i = 0; i < (int)m_vecRemoteInfoList.size(); i++)
	{
		if (strUser != m_vecRemoteInfoList[i].c_str())
		{
			strUser = m_vecRemoteInfoList[i].c_str();
			str.Format("%s\r\n%s", str, strUser);
		}
	}

	WRILOG(LOGTYPE_DEBUG, "DEBUG2.LOG", "本次人数", "%s", str);

	delete RemoteUserOffLine;
	RemoteUserOffLine = NULL;
	WRILOG(LOGTYPE_DEBUG, "DEBUG4.LOG", "onRemoteUserOffLineNotifyMT", "7远端离会");
	return 0;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 移除离会用户（Remove Departure Users）
void CRTCSampleMainInterface::RemoveRemoteUser(const String strUid, RemoveType remove)
{
	for (int index = 0; index < 5; index++)
	{
		if (m_vecThrowingScreenInfo[index].s_strRemoteUserUid == strUid)
		{
			if (MoveForward == remove)
			{
				if (0 < index)
				{
					for (int index2 = 0; index2 < index; index2++)
					{
						MoveThrowingScreen(m_vecThrowingScreenInfo[index2].s_iReflectID, 1);
						MoveThrowingScreen(m_vecThrowingScreenInfo[index2].s_iReflectTips, 1);
					}

					MoveThrowingScreen(m_vecThrowingScreenInfo[index].s_iReflectID, -index);
					MoveThrowingScreen(m_vecThrowingScreenInfo[index].s_iReflectTips, -index);
					GetDlgItem(m_vecThrowingScreenInfo[index].s_iReflectID)->SetWindowTextA("无信号");
					GetDlgItem(m_vecThrowingScreenInfo[index].s_iReflectTips)->SetWindowTextA("");
					m_vecThrowingScreenInfo[index].s_strRemoteUserUid = "";

					MoveVecValPos(index, true);
				}
				else
				{
					GetDlgItem(m_vecThrowingScreenInfo[index].s_iReflectID)->SetWindowTextA("无信号");
					GetDlgItem(m_vecThrowingScreenInfo[index].s_iReflectTips)->SetWindowTextA("");
					m_vecThrowingScreenInfo[index].s_strRemoteUserUid = "";
				}

				
			}
			else if (BackwardShift == remove)
			{
				if (4 > index)
				{
					for (int index2 = index + 1; index2 < 5; index2++)
					{
						MoveThrowingScreen(m_vecThrowingScreenInfo[index2].s_iReflectID, -1);
						MoveThrowingScreen(m_vecThrowingScreenInfo[index2].s_iReflectTips, -1);
					}

					MoveThrowingScreen(m_vecThrowingScreenInfo[index].s_iReflectID, 4 - index);
					MoveThrowingScreen(m_vecThrowingScreenInfo[index].s_iReflectTips, 4 - index);
					GetDlgItem(m_vecThrowingScreenInfo[index].s_iReflectID)->SetWindowTextA("无信号");
					GetDlgItem(m_vecThrowingScreenInfo[index].s_iReflectTips)->SetWindowTextA("");
					m_vecThrowingScreenInfo[index].s_strRemoteUserUid = "";

					MoveVecValPos(index, false);
				}
				else
				{
					GetDlgItem(m_vecThrowingScreenInfo[index].s_iReflectID)->SetWindowTextA("无信号");
					GetDlgItem(m_vecThrowingScreenInfo[index].s_iReflectTips)->SetWindowTextA("");
					m_vecThrowingScreenInfo[index].s_strRemoteUserUid = "";
				}	
			}

			break;
		}
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 移动投屏（Move Throwing Screen）
void CRTCSampleMainInterface::MoveThrowingScreen(int nHandle, int index)
{
	CRect rect;
	GetDlgItem(nHandle)->GetWindowRect(&rect);          //获得空间的绝对坐标
	ScreenToClient(&rect);                              //获得相对于主窗体的坐标

	int iMovingDistance = (rect.Width()+3) * index;

	rect.OffsetRect(CSize(iMovingDistance, 0));                   //这里要是要移动的相对位置
	GetDlgItem(nHandle)->MoveWindow(rect);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 移动容器值位置（Moving container value position）
void CRTCSampleMainInterface::MoveVecValPos(int index, bool bFlag)
{
	sChannelInfo sTemp = m_vecThrowingScreenInfo[index];

	m_vecThrowingScreenInfo.erase(m_vecThrowingScreenInfo.begin()+ index);

	if (true == bFlag)
		m_vecThrowingScreenInfo.insert(m_vecThrowingScreenInfo.begin(), sTemp);
	else
		m_vecThrowingScreenInfo.push_back(sTemp);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 远端用户发布或停止发布(Distant user publishes or stops publishing)
void CRTCSampleMainInterface::OnRemoteTrackAvailableNotify(const char *uid,
	AliEngineAudioTrack audioTrack,
	AliEngineVideoTrack videoTrack)
{
	if (0 == m_mapRemoteInfo.count(uid)) return;
	Dictionary dict;
	if (0 != m_pEngine->GetUserInfo(uid, dict)) return;

	subChangedParam *subChange = new subChangedParam;
	subChange->uid = uid;
	subChange->at = audioTrack;
	subChange->vt = videoTrack;

	PostMessage(MSG_SUBSCRIBE_CHANGED, 0, (LPARAM)subChange);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 消息队列里做订阅发布和停止发布的相关操作（Message queue for subscription publishing and stop publishing）
LRESULT CRTCSampleMainInterface::onRemoteTrackAvailableNotifyMT(WPARAM wParam, LPARAM lParam)
{
	if (m_pEngine == NULL) return -1;

	m_sRemoteUserRelease.s_bRemoteTrackAvailableActioning = true;

	subChangedParam *subChange = (subChangedParam *)lParam;

	Dictionary dict;
	bool bIsScreen = false;
	vector<String>::iterator VecIterCamera = m_vecRemoteInfoList.end();

	if (0 != m_pEngine->GetUserInfo(subChange->uid.c_str(), dict))  goto exit;
	if (0 == m_mapRemoteInfo.count(subChange->uid.c_str())) goto exit;
	
	for (vector<String>::iterator VecIter = m_vecRemoteInfoList.begin(); VecIter != m_vecRemoteInfoList.end(); VecIter++)
	{
		if (*VecIter == subChange->uid)
		{
			if (VecIterCamera == m_vecRemoteInfoList.end())
				VecIterCamera = VecIter;
			else
			{
				if (AliEngineVideoTrackBoth != subChange->vt)
				{
					m_vecRemoteInfoList.erase(VecIter);
					break;
				}
				else
					bIsScreen = true;
			}
		}
	}

	if (AliEngineVideoTrackBoth == subChange->vt && !bIsScreen)
	{
		m_vecRemoteInfoList.insert(VecIterCamera, subChange->uid);
	}

	if (5 < m_vecRemoteInfoList.size())
	{
		GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(true);
	}

	UpdateSubscribe(subChange->uid, subChange->at, subChange->vt);

	exit:
	delete subChange;

	m_sRemoteUserRelease.s_bRemoteTrackAvailableActioning = false;
	return 0;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 更新订阅流(Update subscription stream)
void CRTCSampleMainInterface::UpdateSubscribe(const String uid, AliEngineAudioTrack audioTrack, AliEngineVideoTrack videoTrack)
{
	if (0 == m_mapRemoteInfo.count(uid.c_str())) return;
	Dictionary dict;
	if (0 != m_pEngine->GetUserInfo(uid.c_str(), dict))  return;

	m_mapRemoteInfo[uid.c_str()].s_strRemoteUserUid = uid;	

	if (AliEngineAudioTrackMic == audioTrack && true == m_mapRemoteInfo[uid.c_str()].s_bIsRecvAudio)
		m_pEngine->SubscribeRemoteAudioStream(uid.c_str(), true);
	else
		m_pEngine->SubscribeRemoteAudioStream(uid.c_str(), false);

	if ((AliEngineVideoTrackScreen == videoTrack || AliEngineVideoTrackBoth == videoTrack) && true == m_mapRemoteInfo[uid.c_str()].s_bIsRecvScreen)
	{
		m_mapRemoteInfo[uid.c_str()].s_bHsaScreenFlow = true;

		if (AliEngineVideoTrackBoth == videoTrack)
			m_mapRemoteInfo[uid.c_str()].s_bHsaCameraFlow = true;
	}
	else if (AliEngineVideoTrackCamera == videoTrack && true == m_mapRemoteInfo[uid.c_str()].s_bIsRecvCamera)
	{
		m_mapRemoteInfo[uid.c_str()].s_bHsaCameraFlow = true;
		m_mapRemoteInfo[uid.c_str()].s_bHsaScreenFlow = false;
	}
	else
	{
		m_mapRemoteInfo[uid.c_str()].s_bHsaScreenFlow = false;
		m_mapRemoteInfo[uid.c_str()].s_bHsaCameraFlow = false;
	}

	MonitorSortingErrorReset();
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 投映订阅流(Reflect subscription stream)
void CRTCSampleMainInterface::ShowSubscribe(const String uid, int iReflectIndex, AliEngineVideoTrack vt)
{
	if (0 == m_mapRemoteInfo.size()) return;
	if (0 == m_mapRemoteInfo.count(uid.c_str())) return;
	Dictionary dict;
	if (0 != m_pEngine->GetUserInfo(uid.c_str(), dict))  return;

	m_vecThrowingScreenInfo[iReflectIndex].s_strRemoteUserUid = uid;
	m_vecThrowingScreenInfo[iReflectIndex].vt = vt;
	if (AliEngineVideoTrackCamera == vt)
	{
		m_mapRemoteInfo[uid.c_str()].s_iReflectID = m_vecThrowingScreenInfo[iReflectIndex].s_iReflectID;
		m_mapRemoteInfo[uid.c_str()].s_iReflectTips = m_vecThrowingScreenInfo[iReflectIndex].s_iReflectTips;
		m_mapRemoteInfo[uid.c_str()].s_bIsReflect = true;
		m_mapRemoteInfo[uid.c_str()].s_iReflectIndex = iReflectIndex;

		if (true == m_mapRemoteInfo[uid.c_str()].s_bHsaCameraFlow && true == m_mapRemoteInfo[uid.c_str()].s_bIsRecvCamera)
		{
			AliEngineVideoCanvas canvas;

			canvas.displayView = GetDlgItem(m_vecThrowingScreenInfo[m_mapRemoteInfo[uid.c_str()].s_iReflectIndex].s_iReflectID)->m_hWnd;
			m_pEngine->SetRemoteViewConfig(canvas, uid.c_str(), AliEngineVideoTrackCamera);
		}
	}
	else if (AliEngineVideoTrackScreen == vt)
	{
		if (true == m_mapRemoteInfo[uid.c_str()].s_bHsaScreenFlow && true == m_mapRemoteInfo[uid.c_str()].s_bIsRecvScreen)
		{
			AliEngineVideoCanvas canvas;

			canvas.displayView = GetDlgItem(m_vecThrowingScreenInfo[iReflectIndex].s_iReflectID)->m_hWnd;
			m_pEngine->SetRemoteViewConfig(canvas, uid.c_str(), AliEngineVideoTrackScreen);
		}
	}	

	CString strReflectTips;
	strReflectTips.Format("远端用户:%s", m_mapRemoteInfo[uid.c_str()].s_strRemoteUserName.c_str());
	GetDlgItem(m_vecThrowingScreenInfo[iReflectIndex].s_iReflectTips)->SetWindowTextA(strReflectTips);	

	SetTimer(TIMER_RETRIEVAL_CHANNEL, 500, NULL);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 当订阅情况发生变化时(When subscriptions change)
void CRTCSampleMainInterface::OnVideoSubscribeStateChanged(const char *uid,
	AliEngineSubscribeState oldState,
	AliEngineSubscribeState newState,
	int elapseSinceLastState,
	const char *channel)
{
	m_sRemoteUserRelease.s_bSubscribeChangedActioning = true;

	if (newState == AliEngineStatsNoSubscribe)
	{
		for (int i = 0; i < 5; i++)
		{
			if (m_vecThrowingScreenInfo[i].s_strRemoteUserUid == uid)
			{
				GetDlgItem(m_vecThrowingScreenInfo[i].s_iReflectID)->SetWindowTextA("无信号");
				GetDlgItem(m_vecThrowingScreenInfo[i].s_iReflectTips)->SetWindowTextA("");

				break;
			}
		}
	}
	m_sRemoteUserRelease.s_bSubscribeChangedActioning = false;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 被服务器踢出频道的消息(Message kicked out of channel by server)
void CRTCSampleMainInterface::OnBye(int code)
{
    if (AliEngineOnByeChannelTerminated == code)
    {
        AfxMessageBox("房间不存在...");
        CDialogEx::OnOK();
    }
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 外接设备状态变更(State Change of External Equipment)
void CRTCSampleMainInterface::OnAudioDeviceStateChanged(const AliEngineDeviceInfo& deviceInfo, AliEngineExternalDeviceType deviceType, AliEngineExternalDeviceState deviceState)
{

}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 加入频道(Join channel)
bool CRTCSampleMainInterface::JoinChannel()
{
	// 设置自动订阅和自动推流(Setting up automatic push and subscription)
	AliEngine::SetLogLevel(AliEngineLogLevelInfo);
	m_pEngine->PublishLocalAudioStream(m_sLoginInfo.s_bAutoPublish);
	m_pEngine->PublishLocalVideoStream(m_sLoginInfo.s_bAutoPublish);
	m_pEngine->SubscribeAllRemoteAudioStreams(m_sLoginInfo.s_bAutoSubscribe);
	m_pEngine->SubscribeAllRemoteVideoStreams(m_sLoginInfo.s_bAutoSubscribe);
	m_pEngine->PublishLocalDualStream(m_sLoginInfo.s_bPushSimulcast);

	AliEngineAuthInfo authinfo;//备注：需要手动输入鉴权信息 https://help.aliyun.com/document_detail/146833.html?spm=5176.11065259.1996646101.searchclickresult.230e1c95nxlEcb#title-t41-agh-m2i

	//authinfo.appId = "xxxxxxx";
	//authinfo.userId = "xxxxxxxxxxxxxxxxxxxxx";
	//authinfo.channelId = "xxxxxxxxxxxxxx";
	//authinfo.nonce = "AK-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

	//authinfo.gslbCount = 1;
	//authinfo.gslb = new char*[authinfo.gslbCount];
	//for (int i = 0; i < authinfo.gslbCount; i++)
	//{
	//	authinfo.gslb[i] = "https://xxxxxxxxxxxxxxxxxxxxx.com";
	//}

	//authinfo.agentCount = 1;
	//authinfo.agent = new char*[authinfo.agentCount];
	//for (int i = 0; i < authinfo.agentCount; i++)
	//{
	//	authinfo.agent[i] = "xxxxxxxxxxxxxx.com";
	//}

	//authinfo.token = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	//authinfo.timestamp = 1621476611;
	AfxMessageBox(_T("1.可自己搭建appserver获取地址，2. 需手动输入鉴权信息！"));
	return false;

	String appSrv = CStringToAliString(m_sLoginInfo.s_strServerAddr);
	String channel = CStringToAliString(m_sLoginInfo.s_strRoomID);
	String userName = CStringToAliString(m_sLoginInfo.s_strUserID);
	String appID = CStringToAliString(m_sLoginInfo.s_strPasswd);

	m_pEngine->JoinChannel(authinfo, m_sLoginInfo.s_strUserID.GetBuffer());

	delete[] authinfo.agent;
	delete[] authinfo.gslb;
	return true;
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 加入频道回调(Join channel function callback)
void CRTCSampleMainInterface::OnJoinChannelResult(int result, const char *channel, int elapsed)
{
	if(result == 0)
		WRILOG(LOGTYPE_DEBUG, RTC_DEBUG_LOG, "进入房间..........", "[房间：%s][用户名：%s]", m_sLoginInfo.s_strRoomID, m_sLoginInfo.s_strUserID);
	else
	{
		WRILOG(LOGTYPE_ERROR, RTC_DEBUG_LOG, "OnInitDialog", "[JoinRoom-OnJoinChannelResult] 加入房间失败...（错误码：%d）", result);
		AfxMessageBox("加入房间失败...");
		PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_EXIT_ROOM, BN_CLICKED), NULL);
	}	
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 连接服务器回调(Connect servers callback)
size_t httpResponseCallback(void *buffer, size_t size, size_t nmemb, std::string& user_p)
{
	user_p = (char*)buffer;
	WRILOG(LOGTYPE_DEBUG, RTC_DEBUG_LOG, "OnInitDialog", "[JoinRoom-GetPassportFromAppServer] [httpResponseCallback]%s", user_p);
	return size * nmemb;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 连接服务器(Connect servers)
//bool CRTCSampleMainInterface::GetPassportFromAppServer(const char *pServer, const char *pRoomID, const char *pUserID, const char *pPasswd, AliRtcAuthInfo &AuthInfo)
//{
//	// use curl
//	CURLcode return_code = curl_global_init(CURL_GLOBAL_ALL);
//
//	CURL *curl = NULL;
//	CURLcode res;
//	bool curlError = true;
//	int retcode = 0;
//	curl_slist *plist = NULL;
//	std::string url;
//	std::string connectx;
//
//	if (CURLE_OK != return_code)
//	{
//		WRILOG(LOGTYPE_ERROR, RTC_DEBUG_LOG, "OnInitDialog", "[JoinRoom-GetPassportFromAppServer] 连接服务器失败...（curl_global_init 错误码：%d）", return_code);
//		goto ERROR1;
//	}
//
//	// init curl
//	curl = curl_easy_init();
//
//	if (curl == NULL)
//	{
//		WRILOG(LOGTYPE_ERROR, RTC_DEBUG_LOG, "OnInitDialog", "[JoinRoom-GetPassportFromAppServer] 连接服务器失败...（curl_easy_init 初始化失败）");
//		goto ERROR2;
//	}
//
//	res = CURLE_OBSOLETE32;
//	//plist = curl_slist_append(NULL, "Content-Type: application/json;charset=UTF-8");
//	//curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);
//	//curl_easy_setopt(curl, CURLOPT_POST, 1); // post req
//
//	// normal
//	url = pServer;
//	url += "login?passwd=";
//	url += pPasswd;
//	url += "&user=";
//	url += pUserID;
//	url += "&room=";
//	url += pRoomID;
//	//url += "&tokensid=false";
//
//	WRILOG(LOGTYPE_DEBUG, RTC_DEBUG_LOG, "OnInitDialog", "[JoinRoom-GetPassportFromAppServer] [连接地址:%s]", url);
//
//
//	curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url
//
//	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//	// if want to use https
//	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // set peer and host verify false
//	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
//	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
//	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpResponseCallback);
//	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &connectx);
//	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
//	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
//	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
//	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 5000);
//
//	// start req
//	res = curl_easy_perform(curl);
//	if (res != CURLE_OK)
//		WRILOG(LOGTYPE_ERROR, RTC_DEBUG_LOG, "OnInitDialog", "[JoinRoom-GetPassportFromAppServer] 连接服务器失败...（curl_easy_perform 错误码：%d）", res);
//
//	retcode = 0;
//	return_code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
//	if (CURLE_OK != return_code)
//		WRILOG(LOGTYPE_ERROR, RTC_DEBUG_LOG, "OnInitDialog", "[JoinRoom-GetPassportFromAppServer] 连接服务器失败...（curl_easy_getinfo 错误码：%d）", return_code);
//
//	if(res == CURLE_OK && CURLE_OK == return_code && retcode == 200)
//	{
//		Json::Value jsonData;
//		Json::Reader jsonReader;
//
//		if (jsonReader.parse(connectx, jsonData))
//		{
//			AuthInfo.channel = pRoomID;
//			std::string appid(jsonData["data"]["appid"].asString());
//			AuthInfo.appid = appid.c_str();
//			std::string userid(jsonData["data"]["userid"].asString());
//			AuthInfo.user_id = userid.c_str();
//			std::string nonce(jsonData["data"]["nonce"].asString());
//			AuthInfo.nonce = nonce.c_str();
//			std::string ts = jsonData["data"]["timestamp"].asString();
//			std::string::size_type sz = 0;
//			AuthInfo.timestamp = std::stoull(ts, &sz, 10);
//			std::string token(jsonData["data"]["token"].asString());
//			AuthInfo.token = token.c_str();
//			std::string session(jsonData["data"]["session"].asString());
//			Json::Value gslb_list = jsonData["data"]["gslb"];
//			for (int i = 0; i < (int)gslb_list.size(); i++) {
//				AuthInfo.gslb.AddString(gslb_list[i].asString().c_str());
//			}
//
//			curlError = false;
//		}
//	}
//
//	curl_slist_free_all(plist);
//
//ERROR2:
//	if (curl != NULL)
//		curl_easy_cleanup(curl);
//ERROR1:
//	curl_global_cleanup();
//
//	if (curlError) 
//	{
//		WRILOG(LOGTYPE_ERROR, RTC_DEBUG_LOG, "OnInitDialog", "[JoinRoom-GetPassportFromAppServer] 连接服务器失败...（错误码：%d）", retcode);
//		return false;
//	}
//
//	return true;
//}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 本地预览(Preview locally)
bool CRTCSampleMainInterface::PreviewLocally(UINT ControlID)
{
	// Get display window
	AliEngineVideoCanvas canvas;
	canvas.displayView = GetDlgItem(ControlID)->m_hWnd;

	// Config preview window to engine
	m_pEngine->SetLocalViewConfig(canvas, AliEngineVideoTrackCamera);
	int iRet = m_pEngine->StartPreview() >= 0;

	if (!iRet)
	{
		WRILOG(LOGTYPE_ERROR, RTC_DEBUG_LOG, "OnInitDialog", "[BrowseLocal]无法打开本地摄像头...");
		return false;
	}
		
	return true;
}


void CRTCSampleMainInterface::UpdateVideoView(String uid, UINT ViewID, UINT ControlID)
{
	if (m_nConsoleOccupancy == ViewID)
	{
		return;
	}

	//已存在切换的，先切换回去
	AliEngineVideoCanvas canvas;
	if (m_nConsoleOccupancy != ControlID)
	{
		if (m_sRemoteUserCfg.s_strSetOccupyUid.isEmpty() || m_sRemoteUserCfg.s_strSetOccupyUid == "")
		{
			PreviewLocally(m_nConsoleOccupancy);
		}
		else
		{
			//从大屏切回小屏，改为拉取小流
			m_pEngine->SetRemoteVideoStreamType(m_sRemoteUserCfg.s_strSetOccupyUid.c_str(), AliEngineVideoStreamTypeLow);

			canvas.displayView = GetDlgItem(m_nConsoleOccupancy)->m_hWnd;
			m_pEngine->SetRemoteViewConfig(canvas, m_sRemoteUserCfg.s_strSetOccupyUid.c_str(), AliEngineVideoTrackCamera);
		}

		if (ViewID == IDC_STATIC_MAINROOM && uid.isEmpty())
		{
			m_nConsoleOccupancy = ViewID;
			m_sRemoteUserCfg.s_strSetOccupyUid = uid;
			return;//主视图切回原视图，无需再显示主视图
		}
	}

	m_nConsoleOccupancy = ViewID;
	m_sRemoteUserCfg.s_strSetOccupyUid = uid;
	//切换大小图
	if (uid.isEmpty() || uid == "")
	{
		PreviewLocally(ControlID);
	}
	else
	{
		//从小屏切换大屏屏，改为拉取大流
		m_pEngine->SetRemoteVideoStreamType(m_sRemoteUserCfg.s_strSetOccupyUid.c_str(), AliEngineVideoStreamTypeHigh);
		if (IDC_STATIC_MAINROOM == ControlID)
		{
			canvas.displayView = m_ctlMainRoom.GetSafeHwnd();
		}
		else
		{
			canvas.displayView = GetDlgItem(ControlID)->m_hWnd;
		}
		m_pEngine->SetRemoteViewConfig(canvas, uid.c_str(), AliEngineVideoTrackCamera);
	}

	GetDlgItem(ViewID)->Invalidate();
}

void CRTCSampleMainInterface::stnDblclickStaticRoom(UINT ViewID)
{
	int index = RetrievalChannelSortPos(ViewID);
	if (m_vecThrowingScreenInfo[index].s_strRemoteUserUid.isEmpty()) return;

	Dictionary dict;
	if (m_pEngine->GetUserInfo(m_vecThrowingScreenInfo[index].s_strRemoteUserUid.c_str(), dict) != 0) return;

	if (m_vecThrowingScreenInfo[index].vt == AliEngineVideoTrackScreen) return;

	String strUid = m_vecThrowingScreenInfo[index].s_strRemoteUserUid;
	String userName = dict.getValue("displayName");

	// 远程用户大小流切换
	//RemoteUserPreferMasterSwitching(dict);
	// 远程用户镜像切换
	//RemoteUserMirrorSwitching(dict);

	UpdateVideoView(strUid, ViewID, IDC_STATIC_MAINROOM);

	CString strSettingTips;
	strSettingTips.Format("控制台配置 [占用:%s]", AliStringToCString(userName));
	//GetDlgItem(IDC_STATIC_SETTING)->SetWindowText(strSettingTips);
	CString strConsoleTips;
	strConsoleTips.Format("控制台 [占用:%s]", AliStringToCString(userName));
	GetDlgItem(IDC_STATIC_CONSOLE)->SetWindowText(strConsoleTips);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 分割字符串(Split string)
vector<CString> CRTCSampleMainInterface::SplitString(const string &s, const string &seperator)
{
	vector<CString> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i).c_str());
			i = j;
		}
	}
	return result;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 清空投映数据(Clear reflect data)
void CRTCSampleMainInterface::ClearChannel(const String uid, AliEngineVideoTrack videoTrack)
{
	if (0 == m_mapRemoteInfo.count(uid.c_str())) return;

	AliEngineVideoCanvas canvas;
	canvas.displayView = NULL;
	m_pEngine->SetRemoteViewConfig(canvas, uid.c_str(), AliEngineVideoTrackCamera == videoTrack ? AliEngineVideoTrackCamera : AliEngineVideoTrackScreen);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 清空房间视频序列
void CRTCSampleMainInterface::OnBnClickedCheckEnableScreen()
{
	if (m_pEngine == nullptr)
		return;

	int iCheckState = m_IsEnableScreen.GetCheck();

	if (iCheckState)
	{
		RECT rc;
		::GetWindowRect(::GetDesktopWindow(), &rc);

		AliEngineScreenShareRegion screenRegion;
		screenRegion.originX = (float)rc.left;
		screenRegion.originY = (float)rc.top;
		screenRegion.width = (float)(rc.right - rc.left);
		screenRegion.height = (float)(rc.bottom - rc.top);

		AliEngineScreenShareConfig config;
		m_pEngine->StartScreenShareByScreenRegion(screenRegion, config);
	}
	else
	{
		if (m_pEngine->IsScreenSharePublished())
			m_pEngine->StopScreenShare();
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 展示频道在线人数(Number of Display Channels)
void CRTCSampleMainInterface::ShowNumOfChannels()
{
	StringArray NumOfRoom;

	m_pEngine->GetOnlineRemoteUsers(NumOfRoom);
	CString strRtcOnline;
	strRtcOnline.Format("RTC通信 [在线：%d]", NumOfRoom.size());
	GetDlgItem(IDC_STATIC_RTC_ONLINE)->SetWindowTextA(strRtcOnline);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 交换订阅流(Close subscription data)
void CRTCSampleMainInterface::ReplaceSubscribeFlow(const String strOldUid, AliEngineVideoTrack oldvt, const String strNewUid, AliEngineVideoTrack newvt, int index)
{
	m_bMoving = false;
	if (!strOldUid.isEmpty())
	{
		AliEngineVideoCanvas canvas;
		canvas.displayView = NULL;
		if (AliEngineVideoTrackCamera == oldvt)
		{
			m_pEngine->SetRemoteViewConfig(canvas, strOldUid.c_str(), AliEngineVideoTrackCamera);
			m_pEngine->SubscribeRemoteVideoStream(strOldUid.c_str(), AliEngineVideoTrackCamera, false);
		}
		else
		{
			m_pEngine->SetRemoteViewConfig(canvas, strOldUid.c_str(), AliEngineVideoTrackScreen);
			m_pEngine->SubscribeRemoteVideoStream(strOldUid.c_str(), AliEngineVideoTrackScreen, false);
		}	
	}

	m_iChangeChannelIndex = index;

	if (AliEngineVideoTrackCamera == newvt)
	{
		m_pEngine->SubscribeRemoteVideoStream(strNewUid.c_str(), AliEngineVideoTrackCamera, true);
		ShowSubscribe(strNewUid, m_iChangeChannelIndex, AliEngineVideoTrackCamera);
	}
	else if (true == m_mapRemoteInfo[strNewUid.c_str()].s_bHsaScreenFlow && true == m_mapRemoteInfo[strNewUid.c_str()].s_bIsRecvScreen)
	{
		m_pEngine->SubscribeRemoteVideoStream(strNewUid.c_str(), AliEngineVideoTrackScreen, true);
		ShowSubscribe(strNewUid, m_iChangeChannelIndex, AliEngineVideoTrackScreen);
	}
	else
	{
		GetDlgItem(m_vecThrowingScreenInfo[m_iChangeChannelIndex].s_iReflectID)->SetWindowTextA("无信号");
		return;
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 投屏左移（Throwing Screen Right Shift）
void CRTCSampleMainInterface::RightShift(int iLeftSideIndex, int iRightSideIndex)
{
	m_mapRemoteInfo[m_vecThrowingScreenInfo[0].s_strRemoteUserUid.c_str()].s_bIsReflect = false;

	for (int index = 1; index < 5; index++)
	{
		MoveThrowingScreen(m_vecThrowingScreenInfo[index].s_iReflectID, -1);
		MoveThrowingScreen(m_vecThrowingScreenInfo[index].s_iReflectTips, -1);
	}

	MoveThrowingScreen(m_vecThrowingScreenInfo[0].s_iReflectID, 4);
	MoveThrowingScreen(m_vecThrowingScreenInfo[0].s_iReflectTips, 4);

	MoveVecValPos(0, false);

	if (iRightSideIndex >= m_vecRemoteInfoList.size())
		MonitorSortingErrorReset();
	else
	{
		AliEngineVideoTrack vt = AliEngineVideoTrackCamera;
		if (m_vecRemoteInfoList[iRightSideIndex - 1] == m_vecRemoteInfoList[iRightSideIndex])
			vt = AliEngineVideoTrackScreen;
		ReplaceSubscribeFlow(m_vecThrowingScreenInfo[4].s_strRemoteUserUid, m_vecThrowingScreenInfo[4].vt, m_vecRemoteInfoList[iRightSideIndex],vt, 4);
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 投屏右移（Throwing Screen Left Shift）
void CRTCSampleMainInterface::LeftShift(int iLeftSideIndex, int iRightSideIndex)
{
	m_mapRemoteInfo[m_vecThrowingScreenInfo[4].s_strRemoteUserUid.c_str()].s_bIsReflect = false;

	for (int index = 0; index < 4; index++)
	{
		MoveThrowingScreen(m_vecThrowingScreenInfo[index].s_iReflectID, 1);
		MoveThrowingScreen(m_vecThrowingScreenInfo[index].s_iReflectTips, 1);
	}
	int i = iLeftSideIndex;
	MoveThrowingScreen(m_vecThrowingScreenInfo[4].s_iReflectID, -4);
	MoveThrowingScreen(m_vecThrowingScreenInfo[4].s_iReflectTips, -4);

	MoveVecValPos(4, true);

	if(iLeftSideIndex < 0)
		MonitorSortingErrorReset();
	else
	{
		ReplaceSubscribeFlow(m_vecThrowingScreenInfo[0].s_strRemoteUserUid, m_vecThrowingScreenInfo[0].vt, m_vecRemoteInfoList[iLeftSideIndex], AliEngineVideoTrackCamera, 0);
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// CString转AliString（CString To AliString）
String CRTCSampleMainInterface::CStringToAliString(CString &s)
{
	int n = s.GetLength();

	if (n == 0)
		return String();

	USES_CONVERSION;
	LPCWSTR pwcStr = A2CW((LPCSTR)s);

	int len = WideCharToMultiByte(CP_UTF8, 0, pwcStr, n, NULL, 0, NULL, NULL);
	char *pChar = new char[len + 1];
	WideCharToMultiByte(CP_UTF8, 0, pwcStr, n, pChar, len, NULL, NULL);
	pChar[len] = 0;
	String res = pChar;
	delete[] pChar;
	return res;
}

CString CRTCSampleMainInterface::AliStringToCString(String &s)
{
	const char *p = s.c_str();
	int plen = strlen(p);

	int n = MultiByteToWideChar(CP_UTF8, 0, p, plen, NULL, 0);

	if (n == 0)
		return CString();

	wchar_t *buf = (wchar_t *)malloc((n + 1) * sizeof(wchar_t));

	MultiByteToWideChar(CP_UTF8, 0, p, plen, buf, n);
	buf[n] = 0;

	USES_CONVERSION;
	CString ret(W2A(buf));
	free(buf);
	return ret;
}

// a helper function to convert unicode CString to AliRtc::String
std::string CRTCSampleMainInterface::toString(CString &cs)
{
#ifdef _UNICODE
	USES_CONVERSION;
	std::string s(W2A(cs));
	return s;
#else
	std::string s(cs.GetBuffer());
	cs.ReleaseBuffer();
	return s;
#endif
}

CString CRTCSampleMainInterface::toCString(std::string &s)
{
#ifdef _UNICODE
	USES_CONVERSION;
	CString cs(s.c_str());
	return cs;
#else
	CString cs;
	cs.Format("%s", s.c_str());
	return cs;
#endif
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 获取工程路径
CString CRTCSampleMainInterface::ObtainFilePath()
{
	CString strCurrentPath;
	TCHAR szPath[MAX_PATH] = { 0 };

	if (GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		(_tcsrchr(szPath, _T('\\')))[1] = 0;
	}

	strCurrentPath.Format(_T("%s"), szPath);

	return strCurrentPath;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 检验服务连接入参(Inspection Service Connection Reference）
int CRTCSampleMainInterface::CheckServerLegalEntry(CString strServerEntry, int bFlag)
{
	if (1 == bFlag) // 效验合法服务地址(Validation Legal Service Address)
	{
		if (-1 == strServerEntry.Find("http://") && -1 == strServerEntry.Find("https://"))
			return -1;
		if (0 <= strServerEntry.Find(" "))
			return -1;
	}
	else if (2 == bFlag) // 校验合法频道号(Check Legal Channel Number)
	{
		
	}
	else if (3 == bFlag) // 校验合法用户名(Check Legal Username)
	{
		
	}
	else if (4 == bFlag) // 校验合法服务器密码(Check Legal Server Password)
	{

	}
	else
		return -5;

	return 0;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 自由移动控件（Free Mobile Control）
void CRTCSampleMainInterface::FreeMobileControl()
{
	PostMessage(MSG_USER_CHANGESIZE, 0, 0);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 消息队列 - 自由移动控件（Free Mobile Control）
LRESULT CRTCSampleMainInterface::onFreeMobileControl(WPARAM wParam, LPARAM lParam)
{
	RECT rcWin;
	GetClientRect(&rcWin);
	int iWinWidth = rcWin.right;
	int iWinHeight = rcWin.bottom;

	MoveConsoleCfg(iWinWidth, iWinHeight);
	MobeLocalView(iWinWidth, iWinHeight);
	MoveMonitor(iWinWidth, iWinHeight);
	MoveConsole(iWinWidth, iWinHeight);

	return 0;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 控制台配置控件组移动（Configuration control group movement on console）
void CRTCSampleMainInterface::MoveConsoleCfg(int nWinWidth, int nWinHeight)
{
	// 禁用移动
	m_ctlSetting.ShowWindow(FALSE);
	m_ctlFlow.ShowWindow(FALSE);
	m_ctlCheckVideo.ShowWindow(FALSE);
	m_ctlCheckScreenShare.ShowWindow(FALSE);
	m_ctlCheckAudio.ShowWindow(FALSE);
	//m_ctlMirror.ShowWindow(FALSE);
	//m_ctlCheckCameraMirror.ShowWindow(FALSE);
	//m_ctlCheckScreenMirror.ShowWindow(FALSE);
	m_ctlQuality.ShowWindow(FALSE);
	GetDlgItem(IDC_RADIO_CLEAR)->ShowWindow(FALSE);
	GetDlgItem(IDC_RADIO_FLUENT)->ShowWindow(FALSE);
	m_ctlScene.ShowWindow(FALSE);
	GetDlgItem(IDC_RADIO_CAMERA)->ShowWindow(FALSE);
	GetDlgItem(IDC_RADIO_SHARE)->ShowWindow(FALSE);
	m_ctlButton.ShowWindow(FALSE);
	m_ctlButton_OnekeyNovide.ShowWindow(FALSE);
	m_ctlButton_ExitRoom.ShowWindow(FALSE);

	// 控制台配置主窗口
	CRect rcCfg;
	m_ctlSetting.GetWindowRect(&rcCfg);
	// 按钮
	CRect rcButton;
	m_ctlButton.GetWindowRect(&rcButton);
	CRect rcButton_OnekeyNovide;
	m_ctlButton_OnekeyNovide.GetWindowRect(&rcButton_OnekeyNovide);
	CRect rcButton_ExitRoom;
	m_ctlButton_ExitRoom.GetWindowRect(&rcButton_ExitRoom);
	// 
	CRect rcCheckScreenShare;
	m_ctlCheckScreenShare.GetWindowRect(&rcCheckScreenShare);
	// 
	CRect rcCheckScreenMirror;
	m_ctlCheckScreenMirror.GetWindowRect(&rcCheckScreenMirror);
	//
	CRect rcCheckClear;
	GetDlgItem(IDC_RADIO_FLUENT)->GetWindowRect(&rcCheckClear);
	//
	CRect rcCheckShare;
	GetDlgItem(IDC_RADIO_SHARE)->GetWindowRect(&rcCheckShare);

	// 移动
	int iFlowGroupWidth = rcCfg.Width() - 30;
	int iFlowGGroupHeight = (rcCfg.Height() - 30) / 3.5 - 5;
	int iOtherGroupWidth = rcCfg.Width() - 30;
	int iOtherGroupHeight = (rcCfg.Height() - 30 - iFlowGGroupHeight) / 3 - 5;
	// 控制台配置主窗口
	m_ctlSetting.SetWindowPos(NULL, nWinWidth - rcCfg.Width() - 10, nWinHeight - rcCfg.Height() - rcButton.Height() - 15, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	// 音视频流窗口
	m_ctlFlow.SetWindowPos(NULL, nWinWidth - rcCfg.Width() + 5, nWinHeight - rcCfg.Height() - rcButton.Height() + 5, iFlowGroupWidth, iFlowGGroupHeight, SWP_NOZORDER);
	m_ctlCheckVideo.SetWindowPos(NULL, nWinWidth - rcCfg.Width() + 20, nWinHeight - rcCfg.Height() - rcButton.Height() + 20, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_ctlCheckScreenShare.SetWindowPos(NULL, nWinWidth - 35 - rcCheckScreenShare.Width(), nWinHeight - rcCfg.Height() - rcButton.Height() + 20, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_ctlCheckAudio.SetWindowPos(NULL, nWinWidth - rcCfg.Width() + 20, nWinHeight - rcCfg.Height() - rcButton.Height() + 23 + rcCheckScreenShare.Height(), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	// 镜像窗口
	m_ctlMirror.SetWindowPos(NULL, nWinWidth - rcCfg.Width() + 5, nWinHeight - rcCfg.Height() - rcButton.Height() + iFlowGGroupHeight + 8, iOtherGroupWidth, iOtherGroupHeight, SWP_NOZORDER);
	m_ctlCheckCameraMirror.SetWindowPos(NULL, nWinWidth - rcCfg.Width() + 20, nWinHeight - rcCfg.Height() - rcButton.Height() + iFlowGGroupHeight + 25, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_ctlCheckScreenMirror.SetWindowPos(NULL, nWinWidth - 35 - rcCheckScreenMirror.Width(), nWinHeight - rcCfg.Height() - rcButton.Height() + iFlowGGroupHeight + 25, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	// 视频质量窗口
	m_ctlQuality.SetWindowPos(NULL, nWinWidth - rcCfg.Width() + 5, nWinHeight - rcCfg.Height() - rcButton.Height() + iOtherGroupHeight + iFlowGGroupHeight + 11, iOtherGroupWidth, iOtherGroupHeight, SWP_NOZORDER);
	GetDlgItem(IDC_RADIO_CLEAR)->SetWindowPos(NULL, nWinWidth - rcCfg.Width() + 20, nWinHeight - rcCfg.Height() - rcButton.Height() + iOtherGroupHeight + iFlowGGroupHeight + 28, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	GetDlgItem(IDC_RADIO_FLUENT)->SetWindowPos(NULL, nWinWidth - 35 - rcCheckClear.Width(), nWinHeight - rcCfg.Height() - rcButton.Height() + iOtherGroupHeight + iFlowGGroupHeight + 28, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	// 控制台场景窗口
	m_ctlScene.SetWindowPos(NULL, nWinWidth - rcCfg.Width() + 5, nWinHeight - rcCfg.Height() - rcButton.Height() + iOtherGroupHeight * 2 + iFlowGGroupHeight + 14, iOtherGroupWidth, iOtherGroupHeight, SWP_NOZORDER);
	GetDlgItem(IDC_RADIO_CAMERA)->SetWindowPos(NULL, nWinWidth - rcCfg.Width() + 20, nWinHeight - rcCfg.Height() - rcButton.Height() + iOtherGroupHeight * 2 + iFlowGGroupHeight + 31, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	GetDlgItem(IDC_RADIO_SHARE)->SetWindowPos(NULL, nWinWidth - 35 - rcCheckShare.Width(), nWinHeight - rcCfg.Height() - rcButton.Height() + iOtherGroupHeight * 2 + iFlowGGroupHeight + 31, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	// 按钮
	m_ctlButton.SetWindowPos(NULL, nWinWidth - rcCfg.Width() - 10, nWinHeight - rcButton.Height() - 10, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_ctlButton_OnekeyNovide.SetWindowPos(NULL, nWinWidth - rcCfg.Width() - 10 + rcButton.Width() + 2, nWinHeight - rcButton_OnekeyNovide.Height() - 10, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_ctlButton_ExitRoom.SetWindowPos(NULL, nWinWidth - rcCfg.Width() - 10 + rcButton.Width() + rcButton_OnekeyNovide.Width() + 2, nWinHeight - rcButton_ExitRoom.Height() - 10, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	// 启用重绘
	m_ctlSetting.ShowWindow(TRUE);
	//m_ctlFlow.ShowWindow(TRUE);
	//m_ctlCheckVideo.ShowWindow(TRUE);
	//m_ctlCheckScreenShare.ShowWindow(TRUE);
	//m_ctlCheckAudio.ShowWindow(TRUE);
	//m_ctlMirror.ShowWindow(TRUE);
	//m_ctlCheckCameraMirror.ShowWindow(TRUE);
	//m_ctlCheckScreenMirror.ShowWindow(TRUE);
	m_ctlQuality.ShowWindow(TRUE);
	GetDlgItem(IDC_RADIO_CLEAR)->ShowWindow(TRUE);
	GetDlgItem(IDC_RADIO_FLUENT)->ShowWindow(TRUE);
	//m_ctlScene.ShowWindow(TRUE);
	//GetDlgItem(IDC_RADIO_CAMERA)->ShowWindow(TRUE);
	//GetDlgItem(IDC_RADIO_SHARE)->ShowWindow(TRUE);
	//m_ctlButton.ShowWindow(TRUE);
	//m_ctlButton_OnekeyNovide.ShowWindow(TRUE);
	m_ctlButton_ExitRoom.ShowWindow(TRUE);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 本地浏览控件组移动（Local Browse Control Group Mobility）
void CRTCSampleMainInterface::MobeLocalView(int nWinWidth, int nWinHeight)
{
	// 禁用移动
	m_ctlLocalView.ShowWindow(FALSE);
	m_ctlLocalVideo.ShowWindow(FALSE);
	m_ctlCloseMicrophone.ShowWindow(FALSE);
	m_ctlCloseVideo.ShowWindow(FALSE);
	m_IsEnableScreen.ShowWindow(FALSE);
	m_ctlLocalSetting.ShowWindow(FALSE);

	CRect rcLocalView;
	m_ctlLocalView.GetWindowRect(&rcLocalView);
	CRect rcCfg;
	CRect rcLocalVideo;
	m_ctlLocalVideo.GetWindowRect(&rcLocalVideo);
	m_ctlSetting.GetWindowRect(&rcCfg);
	CRect rcButton_ExitRoom;
	m_ctlButton_ExitRoom.GetWindowRect(&rcButton_ExitRoom);
	CRect rcCloseMicrophone;
	m_ctlCloseMicrophone.GetWindowRect(&rcCloseMicrophone);
	CRect rcCloseVideo;
	m_ctlCloseVideo.GetWindowRect(&rcCloseVideo);
	CRect rcLocalSetting;
	m_ctlLocalSetting.GetWindowRect(&rcLocalSetting);

	// 移动
	m_ctlLocalView.SetWindowPos(NULL, nWinWidth - rcLocalView.Width() - 10, nWinHeight - rcCfg.Height() - rcButton_ExitRoom.Height() - rcLocalView.Height() - 25, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_ctlLocalVideo.SetWindowPos(NULL, nWinWidth - rcLocalVideo.Width() - 22, nWinHeight - rcCfg.Height() - rcButton_ExitRoom.Height() - rcLocalView.Height() - 7, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_ctlCloseMicrophone.SetWindowPos(NULL, nWinWidth - rcLocalVideo.Width() - 17, nWinHeight - rcCfg.Height() - rcButton_ExitRoom.Height() - 65, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_ctlCloseVideo.SetWindowPos(NULL, nWinWidth - rcCloseVideo.Width() - 20, nWinHeight - rcCfg.Height() - rcButton_ExitRoom.Height() - 65, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_IsEnableScreen.SetWindowPos(NULL, nWinWidth - rcLocalVideo.Width() - 17, nWinHeight - rcCfg.Height() - rcButton_ExitRoom.Height() - 62 + rcCloseMicrophone.Height(), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_ctlLocalSetting.SetWindowPos(NULL, nWinWidth - rcLocalSetting.Width() - 17, nWinHeight - rcCfg.Height() - rcButton_ExitRoom.Height() - 62 + rcCloseMicrophone.Height(), 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	// 启用重绘
	m_ctlLocalView.ShowWindow(TRUE);
	m_ctlLocalVideo.ShowWindow(TRUE);
	//m_ctlCloseMicrophone.ShowWindow(TRUE);
	//m_ctlCloseVideo.ShowWindow(TRUE);
	m_IsEnableScreen.ShowWindow(TRUE);
	//m_ctlLocalSetting.ShowWindow(TRUE);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 控制台控件组移动（Console Control Group Mobility）
void CRTCSampleMainInterface::MoveConsole(int nWinWidth, int nWinHeight)
{
	CRect rcCfg;
	m_ctlSetting.GetWindowRect(&rcCfg);
	CRect rcRtcOnline;
	m_ctlRtconline.GetWindowRect(&rcRtcOnline);


	m_ctlMainRoom.MoveWindow(20, rcRtcOnline.Height() + 30, nWinWidth - rcCfg.Width() - 50, nWinHeight - rcRtcOnline.Height() - 50);
	m_ctlMainRoom.SetWindowTextA("无信号");
	m_ctlConsole.MoveWindow(10, rcRtcOnline.Height() + 10, nWinWidth - rcCfg.Width() - 30, nWinHeight - rcRtcOnline.Height() - 20);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 监控台控件组移动（Monitor Control Group Mobility）
void CRTCSampleMainInterface::MoveMonitor(int nWinWidth, int nWinHeight)
{
	CRect rcCfg;
	m_ctlSetting.GetWindowRect(&rcCfg);
	CRect rcRtcOnline;
	m_ctlRtconline.GetWindowRect(&rcRtcOnline);
	CRect rcLocalView;
	m_ctlLocalView.GetWindowRect(&rcLocalView);
	CRect rcConsole;
	m_ctlConsole.GetWindowRect(&rcConsole);
	CRect rcButton_ExitRoom;
	m_ctlButton_ExitRoom.GetWindowRect(&rcButton_ExitRoom);

	int iRightHeightCount = rcCfg.Height() + rcLocalView.Height() + rcButton_ExitRoom.Height() + 10;
	int iLeftHeightCount = rcConsole.Height();

	if (iRightHeightCount >= iLeftHeightCount)
	{
		m_ctlRtconline.SetWindowPos(NULL, 10, 10, rcConsole.Width(), nWinHeight / 3.8, SWP_NOZORDER);
		MoveMonitorMarshalling(true);
	}
	else
	{
		m_ctlRtconline.SetWindowPos(NULL, 10, 10, nWinWidth - 20, nWinHeight / 3.8, SWP_NOZORDER);
		MoveMonitorMarshalling(false);
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 监控台编组（Monitoring station grouping）
void CRTCSampleMainInterface::MoveMonitorMarshalling(bool bMonitorState)
{
	// 禁用移动
	m_ctlButtonLeft.ShowWindow(FALSE);
	m_ctlButtonRight.ShowWindow(FALSE);
	m_ctlVideo1.ShowWindow(FALSE);
	m_ctlVideo2.ShowWindow(FALSE);
	m_ctlVideo3.ShowWindow(FALSE);
	m_ctlVideo4.ShowWindow(FALSE);
	m_ctlVideo5.ShowWindow(FALSE);
	m_ctlRoom1.ShowWindow(FALSE);
	m_ctlRoom2.ShowWindow(FALSE);
	m_ctlRoom3.ShowWindow(FALSE);
	m_ctlRoom4.ShowWindow(FALSE);
	m_ctlRoom5.ShowWindow(FALSE);

	CRect rcButton;
	m_ctlButtonLeft.GetWindowRect(&rcButton);
	CRect rcRtcOnline;
	m_ctlRtconline.GetWindowRect(&rcRtcOnline);

	int iButtonWidth = 0;

	if (true == bMonitorState)
	{
		m_ctlButtonLeft.SetWindowPos(NULL, 12, rcRtcOnline.Height() / 2 + 10 - rcButton.Height() / 2, 20, rcButton.Height(), SWP_NOZORDER);
		m_ctlButtonRight.SetWindowPos(NULL, rcRtcOnline.Width() - rcButton.Width() + 8, rcRtcOnline.Height() / 2 + 10 - rcButton.Height() / 2, 20, rcButton.Height(), SWP_NOZORDER);
		iButtonWidth = 20;
	}
	else
	{
		m_ctlButtonLeft.SetWindowPos(NULL, 12, rcRtcOnline.Height() / 2 + 10 - rcButton.Height() / 2, 25, rcButton.Height(), SWP_NOZORDER);
		m_ctlButtonRight.SetWindowPos(NULL, rcRtcOnline.Width() - rcButton.Width() + 8, rcRtcOnline.Height() / 2 + 10 - rcButton.Height() / 2, 25, rcButton.Height(), SWP_NOZORDER);
		iButtonWidth = 25;
	}

	int iVideoWidth = (rcRtcOnline.Width() - 20 - iButtonWidth * 2) / 5;
	int iVideoHeight = rcRtcOnline.Height() * 0.9;
	int iRoomWidth = (rcRtcOnline.Width() - 20 - iButtonWidth * 2) / 5;
	int iRoomHeight = rcRtcOnline.Height() * 0.8;

	GetDlgItem(m_vecThrowingScreenInfo[0].s_iReflectTips)->SetWindowPos(NULL, 14 + iButtonWidth, (rcRtcOnline.Height() - iVideoHeight) / 2 + 10, iVideoWidth, iVideoHeight, SWP_NOZORDER);
	GetDlgItem(m_vecThrowingScreenInfo[0].s_iReflectID)->SetWindowPos(NULL, 14 + iButtonWidth, (rcRtcOnline.Height() - iVideoHeight) / 2 + 10 + iVideoHeight - iRoomHeight, iRoomWidth, iRoomHeight, SWP_NOZORDER);
	GetDlgItem(m_vecThrowingScreenInfo[1].s_iReflectTips)->SetWindowPos(NULL, 17 + iButtonWidth + iVideoWidth, (rcRtcOnline.Height() - iVideoHeight) / 2 + 10, iVideoWidth, iVideoHeight, SWP_NOZORDER);
	GetDlgItem(m_vecThrowingScreenInfo[1].s_iReflectID)->SetWindowPos(NULL, 17 + iButtonWidth + iVideoWidth, (rcRtcOnline.Height() - iVideoHeight) / 2 + 10 + iVideoHeight - iRoomHeight, iRoomWidth, iRoomHeight, SWP_NOZORDER);
	GetDlgItem(m_vecThrowingScreenInfo[2].s_iReflectTips)->SetWindowPos(NULL, 20 + iButtonWidth + iVideoWidth * 2, (rcRtcOnline.Height() - iVideoHeight) / 2 + 10, iVideoWidth, iVideoHeight, SWP_NOZORDER);
	GetDlgItem(m_vecThrowingScreenInfo[2].s_iReflectID)->SetWindowPos(NULL, 20 + iButtonWidth + iVideoWidth * 2, (rcRtcOnline.Height() - iVideoHeight) / 2 + 10 + iVideoHeight - iRoomHeight, iRoomWidth, iRoomHeight, SWP_NOZORDER);
	GetDlgItem(m_vecThrowingScreenInfo[3].s_iReflectTips)->SetWindowPos(NULL, 23 + iButtonWidth + iVideoWidth * 3, (rcRtcOnline.Height() - iVideoHeight) / 2 + 10, iVideoWidth, iVideoHeight, SWP_NOZORDER);
	GetDlgItem(m_vecThrowingScreenInfo[3].s_iReflectID)->SetWindowPos(NULL, 23 + iButtonWidth + iVideoWidth * 3, (rcRtcOnline.Height() - iVideoHeight) / 2 + 10 + iVideoHeight - iRoomHeight, iRoomWidth, iRoomHeight, SWP_NOZORDER);
	GetDlgItem(m_vecThrowingScreenInfo[4].s_iReflectTips)->SetWindowPos(NULL, 26 + iButtonWidth + iVideoWidth * 4, (rcRtcOnline.Height() - iVideoHeight) / 2 + 10, iVideoWidth, iVideoHeight, SWP_NOZORDER);
	GetDlgItem(m_vecThrowingScreenInfo[4].s_iReflectID)->SetWindowPos(NULL, 26 + iButtonWidth + iVideoWidth * 4, (rcRtcOnline.Height() - iVideoHeight) / 2 + 10 + iVideoHeight - iRoomHeight, iRoomWidth, iRoomHeight, SWP_NOZORDER);

	// 启用重绘
	m_ctlButtonLeft.ShowWindow(TRUE);
	m_ctlButtonRight.ShowWindow(TRUE);
	m_ctlVideo1.ShowWindow(TRUE);
	m_ctlVideo2.ShowWindow(TRUE);
	m_ctlVideo3.ShowWindow(TRUE);
	m_ctlVideo4.ShowWindow(TRUE);
	m_ctlVideo5.ShowWindow(TRUE);
	m_ctlRoom1.ShowWindow(TRUE);
	m_ctlRoom2.ShowWindow(TRUE);
	m_ctlRoom3.ShowWindow(TRUE);
	m_ctlRoom4.ShowWindow(TRUE);
	m_ctlRoom5.ShowWindow(TRUE);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 监控台排序错误（Monitor Sorting Error Reset）
void CRTCSampleMainInterface::MonitorSortingErrorReset()
{
	if (m_vecRemoteInfoList.size() >= 5)
	{
		for (int index = 0; index < 5; index++)
		{
			if (m_vecThrowingScreenInfo[index].s_strRemoteUserUid == m_vecRemoteInfoList[index])
				continue;

			AliEngineVideoTrack vt = AliEngineVideoTrackCamera;
			if (0 < index && m_vecRemoteInfoList[index - 1] == m_vecRemoteInfoList[index])
				vt = AliEngineVideoTrackScreen;
			ReplaceSubscribeFlow(m_vecThrowingScreenInfo[index].s_strRemoteUserUid, m_vecThrowingScreenInfo[index].vt,m_vecRemoteInfoList[index],vt, index);
		}
	}
	else if (m_vecRemoteInfoList.size() > 0)
	{
		for (int index = 0; index < (int)m_vecRemoteInfoList.size(); index++)
		{
			if (m_vecThrowingScreenInfo[index].s_strRemoteUserUid == m_vecRemoteInfoList[index])
				continue;

			AliEngineVideoTrack vt = AliEngineVideoTrackCamera;
			if (0 < index && m_vecRemoteInfoList[index - 1] == m_vecRemoteInfoList[index])
				vt = AliEngineVideoTrackScreen;
			ReplaceSubscribeFlow(m_vecThrowingScreenInfo[index].s_strRemoteUserUid, m_vecThrowingScreenInfo[index].vt, m_vecRemoteInfoList[index], vt, index);
		}

		for (int index2 = m_vecRemoteInfoList.size(); index2 < 5; index2++)
		{
			if (m_vecThrowingScreenInfo[index2].s_strRemoteUserUid == "") continue;

			ClearChannel(m_vecThrowingScreenInfo[index2].s_strRemoteUserUid, m_vecThrowingScreenInfo[index2].vt);

			m_vecThrowingScreenInfo[index2].s_strRemoteUserUid = "";
			GetDlgItem(m_vecThrowingScreenInfo[index2].s_iReflectID)->SetWindowTextA("无信号");
			GetDlgItem(m_vecThrowingScreenInfo[index2].s_iReflectTips)->SetWindowTextA("");
		}
	}

}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 检索监视器顺序（Retrieval Channel Order）
void CRTCSampleMainInterface::RetrievalChannelOrder()
{
	KillTimer(TIMER_RETRIEVAL_CHANNEL);

	if ((m_iRightSideIndex - m_iLeftSideIndex) > 4)
	{
		MonitorSortingErrorReset();
		return;
	}

	for (int index = m_iLeftSideIndex; index <= m_iRightSideIndex; index++)
	{
        if (m_vecThrowingScreenInfo.size() <= 0 || m_vecThrowingScreenInfo.size() < (index - m_iLeftSideIndex + 1))
            continue;

		if (m_vecThrowingScreenInfo[index - m_iLeftSideIndex].s_strRemoteUserUid == "") continue;

		if (m_vecThrowingScreenInfo[index - m_iLeftSideIndex].s_strRemoteUserUid == m_vecRemoteInfoList[index]) continue;

		MonitorSortingErrorReset();
	}

	WRILOG(LOGTYPE_DEBUG, "DEBUG1.LOG", "", "【%s】-【%s】-【%s】-【%s】-【%s】", m_vecThrowingScreenInfo[0].s_strRemoteUserUid.c_str(),
		m_vecThrowingScreenInfo[1].s_strRemoteUserUid.c_str(),
		m_vecThrowingScreenInfo[2].s_strRemoteUserUid.c_str(),
		m_vecThrowingScreenInfo[3].s_strRemoteUserUid.c_str(),
		m_vecThrowingScreenInfo[4].s_strRemoteUserUid.c_str());
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 检索监视器顺序位置（Retrieval Channel Sort Location）
int CRTCSampleMainInterface::RetrievalChannelSortPos(int nReflectID)
{
	int index = 0;
	for (; index < 5; index++)
	{
		if (m_vecThrowingScreenInfo[index].s_iReflectID == nReflectID) break;
	}

	return index;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 远程用户大小流切换（Remote User Large and Small Stream Switching）
void CRTCSampleMainInterface::RemoteUserPreferMasterSwitching(Dictionary &dict)
{
	((CButton *)GetDlgItem(IDC_RADIO_FLUENT))->EnableWindow(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO_CLEAR))->EnableWindow(FALSE);

	bool bIsPreferCameraMaster = dict.getBoolValue("preferCameraMaster", false);
	m_sRemoteUserCfg.s_bHasBigStream = dict.getBoolValue("hasCameraMaster", false);
	m_sRemoteUserCfg.s_bHasSmallStream = dict.getBoolValue("hasCameraSlave", false);

	if (false == m_sRemoteUserCfg.s_bHasBigStream && false == m_sRemoteUserCfg.s_bHasSmallStream) return;

	if (true == bIsPreferCameraMaster)
	{
		((CButton *)GetDlgItem(IDC_RADIO_CLEAR))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_FLUENT))->SetCheck(FALSE);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_RADIO_CLEAR))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_FLUENT))->SetCheck(TRUE);
	}

	if (true == m_sRemoteUserCfg.s_bHasSmallStream)
		((CButton *)GetDlgItem(IDC_RADIO_FLUENT))->EnableWindow(TRUE);

	if (true == m_sRemoteUserCfg.s_bHasBigStream)
		((CButton *)GetDlgItem(IDC_RADIO_CLEAR))->EnableWindow(TRUE);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 远程用户镜像切换（Remote User Mirror Switching）
void CRTCSampleMainInterface::RemoteUserMirrorSwitching(Dictionary &dict)
{
	m_sRemoteUserCfg.s_bIsCameraMirror = dict.getBoolValue("isCameraMirror", false);
	m_sRemoteUserCfg.s_bIsScreenMirror = dict.getBoolValue("isScreenMirror", false);
	m_sRemoteUserCfg.s_bHasBigStream = dict.getBoolValue("hasCameraMaster", false);
	m_sRemoteUserCfg.s_bHasSmallStream = dict.getBoolValue("hasCameraSlave", false);
	m_sRemoteUserCfg.s_bHasScreenStream = dict.getBoolValue("hasScreenSharing", false);

	if (true == m_sRemoteUserCfg.s_bIsCameraMirror)
		((CButton *)GetDlgItem(IDC_CHECK_CAMERA_MIRROR))->SetCheck(TRUE);
	else
		((CButton *)GetDlgItem(IDC_CHECK_CAMERA_MIRROR))->SetCheck(FALSE);

	if (true == m_sRemoteUserCfg.s_bIsScreenMirror)
		((CButton *)GetDlgItem(IDC_CHECK_SCREEN_MIRROR))->SetCheck(TRUE);
	else
		((CButton *)GetDlgItem(IDC_CHECK_SCREEN_MIRROR))->SetCheck(FALSE);

	//if(true == m_sRemoteUserCfg.s_bHasBigStream || true == m_sRemoteUserCfg.s_bHasSmallStream)
	//	((CButton *)GetDlgItem(IDC_CHECK_CAMERA_MIRROR))->EnableWindow(TRUE);

	//if(true == m_sRemoteUserCfg.s_bHasScreenStream)
	//	((CButton *)GetDlgItem(IDC_CHECK_SCREEN_MIRROR))->EnableWindow(TRUE);
}
