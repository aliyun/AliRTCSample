
// RtcSampleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RtcSample.h"
#include "RtcSampleDlg.h"
#include "afxdialogex.h"
#include "RTCSampleSettingDlg.h"
#include "RTCSampleMainInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char* pVerNo = "Ver.1.3.1.2";
const char* pVersion =
"==================================================================================\n"
"版 本 号：1.3.1.2\n"
"发布日期：2019-04-08(编译时间：" __DATE__ " " __TIME__ ")\n"
"发 布 者：XuRan(徐冉)\n"
"=================================================================================="
"发布说明：\n"
"\t2019-03-20 发布版本1.0.0.1：创建基本功能：界面搭建、环境搭建、RTC SDK导入（XuRan(徐冉)）\n"
"\t2019-03-20 发布版本1.1.0.1：创建功能：初始化SDK、本地浏览、连接服务器、加入频道（XuRan(徐冉)）\n"
"\t2019-03-20 发布版本1.2.0.1：创建功能：1对多通信、屏幕共享（XuRan(徐冉)）\n"
"\t2019-04-08 发布版本1.3.0.1：创建功能：初始配置（加入自动推流订阅、推送视频流、音频流、大小流）（XuRan(徐冉)）\n"
"\t2019-04-08 发布版本1.3.1.1：修复 BUG：不同服务器返回uid格式不同，需要灵活处理（XuRan(徐冉)）\n"
"\t2019-04-08 发布版本1.3.1.2：创建功能：切换订阅大小流（XuRan(徐冉)）\n"
"\t2019-04-15 发布版本1.3.2.1：修复 BUG：首次修改地址失败、Mac订阅视频流卡住问题（XuRan(徐冉)）\n"
"\t2019-04-16 发布版本1.3.3.1：修复 BUG：左右移动Crash优化（XuRan(徐冉)）\n"
"\t2019-04-17 发布版本1.4.0.1：创建功能：添加控件随界面大小自由改变功能（XuRan(徐冉)）\n"
"\t2019-04-18 发布版本1.5.0.1：创建功能：大小流切换、镜像切换（XuRan(徐冉)）\n"
;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRtcSampleDlg 对话框



CRtcSampleDlg::CRtcSampleDlg(CWnd* pParent /*= NULL*/)
	: CDialogEx(IDD_RTCSAMPLE_DIALOG)
	, m_strChannelId(_T("请输入频道ID[3-12位数字]"))
	, m_strUserName(_T("请输入用户ID"))
	, m_strEdition(_T(""))
	, m_strChannelText(_T(""))
	, m_strUserText(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_ALILOGO);
}

CRtcSampleDlg::~CRtcSampleDlg()
{
	
}

void CRtcSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOGIN, m_strChannelId);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_STATIC_EDITION, m_strEdition);
	DDX_Text(pDX, IDC_STATIC_CHANNEL_TEXT, m_strChannelText);
	DDX_Text(pDX, IDC_STATIC_USER_TEXT, m_strUserText);
}

BEGIN_MESSAGE_MAP(CRtcSampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_SETFOCUS(IDC_EDIT_LOGIN, &CRtcSampleDlg::OnEnSetfocusEditLogin)
	ON_EN_KILLFOCUS(IDC_EDIT_LOGIN, &CRtcSampleDlg::OnEnKillfocusEditLogin)
	ON_BN_CLICKED(IDC_BUTTON_JOIN_CHANNEL, &CRtcSampleDlg::OnBnClickedButtonJoinChannel)
	ON_EN_SETFOCUS(IDC_EDIT_USERNAME, &CRtcSampleDlg::OnEnSetfocusEditUsername)
	ON_EN_KILLFOCUS(IDC_EDIT_USERNAME, &CRtcSampleDlg::OnEnKillfocusEditUsername)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_SETTING, &CRtcSampleDlg::OnBnClickedButtonSetting)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CRtcSampleDlg 消息处理程序

BOOL CRtcSampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	DELLOG(30);

	WRILOG(LOGTYPE_START, RTC_DEBUG_LOG, "");

	// EDIT 控件文字居中
	RECT rect;
	GetDlgItem(IDC_EDIT_LOGIN)->GetClientRect(&rect);
	OffsetRect(&rect, 0, 4);
	GetDlgItem(IDC_EDIT_LOGIN)->SendMessage(EM_SETRECT, 0, (LPARAM)&rect);
	GetDlgItem(IDC_EDIT_USERNAME)->GetClientRect(&rect);
	OffsetRect(&rect, 0, 4);
	GetDlgItem(IDC_EDIT_USERNAME)->SendMessage(EM_SETRECT, 0, (LPARAM)&rect);

	((CButton *)GetDlgItem(IDC_BUTTON_JOIN_CHANNEL))->SetFocus();

	m_strCfgPath = ObtainFilePath() + "CONFIG\\CONFIG.ini";

	ShowEdition();
	LoadConfig();
	OnBnClickedButtonSetting();

	return FALSE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRtcSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRtcSampleDlg::OnPaint()
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
		CPaintDC dc(this);

		if (!m_Img.IsNull()) 
			m_Img.Destroy();

		CString strPicPath = ObtainFilePath() + "CONFIG\\LOGO.png";
		m_Img.Load(strPicPath.GetBuffer(0));                           //load载入，否则不显示  
		TransparentPNG(&m_Img);										 //调用透明化处理函数  

		CRect rect;
		GetClientRect(&rect);

		int iHeight = rect.Width() / 2 - m_Img.GetWidth() / 2;

		if (!m_Img.IsNull()) m_Img.Draw(dc.m_hDC, iHeight, 28);       //以左上角为原点，按原始大小，在(80,20)处贴图  

		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRtcSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 绘图事件
HBRUSH CRtcSampleDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_LOGIN)
	{
		if (m_strChannelId == "请输入频道ID[3-12位数字]")
		{
			pDC->SetTextColor(RGB(180, 180, 180));
		}
		else
		{
			pDC->SetTextColor(RGB(0, 0, 0));
		}
	}
	else if (pWnd->GetDlgCtrlID() == IDC_EDIT_USERNAME)
	{
		if (m_strUserName == "请输入用户ID")
		{
			pDC->SetTextColor(RGB(180, 180, 180));
		}
		else
		{
			pDC->SetTextColor(RGB(0, 0, 0));
		}
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_CHANNEL_TEXT)
	{
		pDC->SetTextColor(RGB(255, 69, 0));
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_USER_TEXT)
	{
		pDC->SetTextColor(RGB(255, 69, 0));
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 响应事件
BOOL CRtcSampleDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 计时器事件
void CRtcSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);

	switch (nIDEvent)
	{
	case TIMER_TIPS_CHANNELID:
		RefreshChannelIdTipsText();
		break;
	case TIMER_TIPS_USERID:
		RefreshUserIdTipsText();
		break;
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                       加 入 频 道 按 钮                                     */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void CRtcSampleDlg::OnBnClickedButtonJoinChannel()
{
	UpdateData(TRUE);

	bool bRet = false;

	if (m_strChannelId == "请输入频道ID[3-12位数字]" || 0 == m_strChannelId.GetLength())
	{
		m_strChannelText = "请先输入频道ID...";
		bRet = true;

		UpdateData(FALSE);
		SetTimer(TIMER_TIPS_CHANNELID, 5000, NULL);
	}

	if (m_strUserName == "请输入用户ID" || 0 == m_strUserName.GetLength())
	{
		m_strUserText = "请先输入用户ID...";
		bRet = true;

		UpdateData(FALSE);
		SetTimer(TIMER_TIPS_USERID, 5000, NULL);
	}

	if (true == bRet) return;
		
	CRTCSampleMainInterface dlg;
	dlg.m_sLoginInfo.s_strServerAddr = m_strServerAddr;
	dlg.m_sLoginInfo.s_strRoomID = m_strChannelId;
	dlg.m_sLoginInfo.s_strUserID = m_strUserName;
	dlg.m_sLoginInfo.s_strPasswd = "hello1234";

	if(TRUE == m_bH5CompatibleMode)
		dlg.m_sLoginInfo.s_bH5CompatibleMode = true;
	else
		dlg.m_sLoginInfo.s_bH5CompatibleMode = false;

	if(TRUE == m_bAutoPublish)
		dlg.m_sLoginInfo.s_bAutoPublish = true;
	else
		dlg.m_sLoginInfo.s_bAutoPublish = false;

	if(TRUE == m_bAutoSubscribe)
		dlg.m_sLoginInfo.s_bAutoSubscribe = true;
	else
		dlg.m_sLoginInfo.s_bAutoSubscribe = false;

	if(TRUE == m_bPushAudio)
		dlg.m_sLoginInfo.s_bPushAudio = true;
	else
		dlg.m_sLoginInfo.s_bPushAudio = false;

	if (TRUE == m_bPushVideo)
		dlg.m_sLoginInfo.s_bPushVideo = true;
	else
		dlg.m_sLoginInfo.s_bPushVideo = false;

	if (TRUE == m_bPushSimulcast)
		dlg.m_sLoginInfo.s_bPushSimulcast = true;
	else
		dlg.m_sLoginInfo.s_bPushSimulcast = false;

	dlg.m_sLoginInfo.s_strRoomTitle.Format("AliRTCSample[当前频道：%s  本地用户名：%s]", m_strChannelId, m_strUserName);

	ShowWindow(SW_HIDE);

	int iStatus = dlg.DoModal();

	ShowWindow(SW_SHOW);

	if (iStatus != IDOK)
	{
		if (IDOK == MessageBox("是否关闭？", "AliTRCSample", MB_OKCANCEL))
		{
			::SendMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);
		}
			
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                       设 置 界 面 按 钮                                     */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void CRtcSampleDlg::OnBnClickedButtonSetting()
{
	CRTCSampleSettingDlg dlg;
	dlg.doneCallback = [this, &dlg] {

	};
	int iStatus = dlg.DoModal();
	if (iStatus == IDOK)
	{
		LoadConfig();
		ShowEdition();
	}
}


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                       控 件 事 件 类                                        */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 频道ID EDIT 控件 获取焦点事件
void CRtcSampleDlg::OnEnSetfocusEditLogin()
{
	UpdateData(TRUE);

	if (m_strChannelId == "请输入频道ID[3-12位数字]")
	{
		m_strChannelId = "";
		UpdateData(FALSE);
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 频道ID EDIT 控件 失去焦点事件
void CRtcSampleDlg::OnEnKillfocusEditLogin()
{
	UpdateData(TRUE);

	if (m_strChannelId == "请输入频道ID[3-12位数字]") return;

	if (0 == m_strChannelId.GetLength())
	{
		m_strChannelId = "请输入频道ID[3-12位数字]";
		UpdateData(FALSE);
	}

	
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 用户名 EDIT 控件 获取焦点事件
void CRtcSampleDlg::OnEnSetfocusEditUsername()
{
	UpdateData(TRUE);

	if (m_strUserName == "请输入用户ID")
	{
		m_strUserName = "";
		UpdateData(FALSE);
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 用户名 EDIT 控件 失去焦点事件
void CRtcSampleDlg::OnEnKillfocusEditUsername()
{
	UpdateData(TRUE);

	if (m_strUserName == "请输入用户ID") return;

	if (0 == m_strUserName.GetLength())
	{
		m_strUserName = "请输入用户ID";
		UpdateData(FALSE);
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 显示logo
void CRtcSampleDlg::TransparentPNG(CImage *png)
{
	for (int i = 0; i <png->GetWidth(); i++)
	{
		for (int j = 0; j <png->GetHeight(); j++)
		{
			unsigned char* pucColor = reinterpret_cast<unsigned char*>(png->GetPixelAddress(i, j));
			pucColor[0] = pucColor[0] * pucColor[3] / 255;
			pucColor[1] = pucColor[1] * pucColor[3] / 255;
			pucColor[2] = pucColor[2] * pucColor[3] / 255;
		}
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 获取工程路径
CString CRtcSampleDlg::ObtainFilePath()
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

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 展示版本信息
void CRtcSampleDlg::ShowEdition()
{
	UpdateData(TRUE);

	CString strEdition;
	GetPrivateProfileString("CONFIG", "Edition", "", strEdition.GetBuffer(20), 20, m_strCfgPath.GetBuffer());
	strEdition.ReleaseBuffer();
	m_strEdition.Format("Ver %s", strEdition);

	UpdateData(FALSE);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 刷新提示文字
void CRtcSampleDlg::RefreshChannelIdTipsText()
{
	UpdateData(TRUE);

	m_strChannelText = "";

	UpdateData(FALSE);
}

void CRtcSampleDlg::RefreshUserIdTipsText()
{
	UpdateData(TRUE);

	m_strUserText = "";

	UpdateData(FALSE);
}


BOOL CRtcSampleDlg::DestroyWindow()
{
	WRILOG(LOGTYPE_END, RTC_DEBUG_LOG, "");
	//WinExec("cmd /c taskkill /F /IM RtcSample.exe", SW_HIDE);

	return CDialogEx::DestroyWindow();
}

void CRtcSampleDlg::LoadConfig()
{ 
	GetPrivateProfileString("CONFIG", "ServerAddress", "http://127.0.0.1:8080/app/v1/", m_strServerAddr.GetBuffer(1024), 1024, m_strCfgPath.GetBuffer());
	m_strServerAddr.ReleaseBuffer();

	m_bH5CompatibleMode = GetPrivateProfileInt("CONFIG", "H5CompatibleMode", 1, m_strCfgPath.GetBuffer());
	m_bAutoSubscribe = GetPrivateProfileInt("CONFIG", "AutoSubscribe", 1, m_strCfgPath.GetBuffer());
	m_bAutoPublish = GetPrivateProfileInt("CONFIG", "AutoPublish", 1, m_strCfgPath.GetBuffer());
	m_bPushAudio = GetPrivateProfileInt("CONFIG", "PushAudio", 1, m_strCfgPath.GetBuffer());
	m_bPushVideo = GetPrivateProfileInt("CONFIG", "PushVideo", 1, m_strCfgPath.GetBuffer());
	m_bPushSimulcast = GetPrivateProfileInt("CONFIG", "PushSimulcast", 1, m_strCfgPath.GetBuffer());
}





