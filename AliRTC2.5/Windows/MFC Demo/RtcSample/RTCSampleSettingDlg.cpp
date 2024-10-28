// RTCSampleSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RtcSample.h"
#include "RTCSampleSettingDlg.h"
#include "afxdialogex.h"



// CRTCSampleSettingDlg 对话框

IMPLEMENT_DYNAMIC(CRTCSampleSettingDlg, CDialogEx)

CRTCSampleSettingDlg::CRTCSampleSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SETTING, pParent)
	, m_strServerAddr(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_SETTING);
}

CRTCSampleSettingDlg::~CRTCSampleSettingDlg()
{
	
}

void CRTCSampleSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_strServerAddr);
}


BEGIN_MESSAGE_MAP(CRTCSampleSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CONFIRM_CHANG, &CRTCSampleSettingDlg::OnBnClickedButtonConfirmChang)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_CHANG, &CRTCSampleSettingDlg::OnBnClickedButtonCancelChang)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_PUSH_BIG, &CRTCSampleSettingDlg::OnBnClickedRadioPushBig)
	ON_BN_CLICKED(IDC_RADIO_PUSH_SMALL, &CRTCSampleSettingDlg::OnBnClickedRadioPushSmall)
	ON_BN_CLICKED(IDC_CHECK_AUTO_PUSHFLOW, &CRTCSampleSettingDlg::OnBnClickedCheckAutoPushflow)
	ON_BN_CLICKED(IDC_CHECK_AUTO_PULLFLOW, &CRTCSampleSettingDlg::OnBnClickedCheckAutoPullflow)
	ON_BN_CLICKED(IDC_CHECK_PUSH_AUDIOFLOW, &CRTCSampleSettingDlg::OnBnClickedCheckPushAudioflow)
	ON_BN_CLICKED(IDC_CHECK_PUSH_VIDEOFLOW, &CRTCSampleSettingDlg::OnBnClickedCheckPushVideoflow)
	ON_BN_CLICKED(IDC_CHECK_COMPATIBLE_H5, &CRTCSampleSettingDlg::OnBnClickedCheckCompatibleH5)
END_MESSAGE_MAP()


// CRTCSampleSettingDlg 消息处理程序

BOOL CRTCSampleSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// TODO:  在此添加额外的初始化
	m_strCfgPath = ObtainFilePath() + "CONFIG\\CONFIG.ini";

	BOOL bRet = IsFileExist(m_strCfgPath);

	if (FALSE == bRet)
		AddConfigInfo();

	LoadConfigInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CRTCSampleSettingDlg::OnBnClickedButtonConfirmChang()
{
	UpdateData(TRUE);

	WritePrivateProfileString("CONFIG", "ServerAddress", m_strServerAddr.GetBuffer(0), m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "AutoPublish", m_strAutoPublish.GetBuffer(0), m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "AutoSubscribe", m_strAutoSubscribe.GetBuffer(0), m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "PushAudio", m_strPushAudio.GetBuffer(0), m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "PushVideo", m_strPushVideo.GetBuffer(0), m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "PushSimulcast", m_strPushSimulcast.GetBuffer(0), m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "H5CompatibleMode", m_strCompatibleH5.GetBuffer(0), m_strCfgPath.GetBuffer(0));
	if (doneCallback != NULL) {
		doneCallback();
	}
	CDialog::OnOK();
}


void CRTCSampleSettingDlg::OnBnClickedButtonCancelChang()
{
	CDialogEx::OnCancel();
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 获取工程路径
CString CRTCSampleSettingDlg::ObtainFilePath()
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

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// 加载配置文件
void CRTCSampleSettingDlg::LoadConfigInfo()
{
	// 服务器地址
	GetPrivateProfileString("CONFIG", "ServerAddress", "", m_strServerAddr.GetBuffer(MAXSIZE), MAXSIZE, m_strCfgPath.GetBuffer(0));
	m_strServerAddr.ReleaseBuffer();

	if (0 == m_strServerAddr.GetLength())
	{
		m_strServerAddr = "http://11.239.175.78/app/v1/";
		WritePrivateProfileString("CONFIG", "ServerAddress", "http://11.239.175.78/app/v1/", m_strCfgPath.GetBuffer(0));
	}

	// 自动推流
	GetPrivateProfileString("CONFIG", "AutoPublish", "123", m_strAutoPublish.GetBuffer(MAXSIZE), MAXSIZE, m_strCfgPath.GetBuffer(0));
	m_strAutoPublish.ReleaseBuffer();

	if ("1" == m_strAutoPublish)
		((CButton*)GetDlgItem(IDC_CHECK_AUTO_PUSHFLOW))->SetCheck(TRUE);
	else if ("0" == m_strAutoPublish)
		((CButton*)GetDlgItem(IDC_CHECK_AUTO_PUSHFLOW))->SetCheck(FALSE);
	else
	{
		((CButton*)GetDlgItem(IDC_CHECK_AUTO_PUSHFLOW))->SetCheck(TRUE);
		m_strAutoPublish = "1";
		WritePrivateProfileString("CONFIG", "AutoPublish", "1", m_strCfgPath.GetBuffer(0));
	}

	// 自动订阅
	GetPrivateProfileString("CONFIG", "AutoSubscribe", "123", m_strAutoSubscribe.GetBuffer(MAXSIZE), MAXSIZE, m_strCfgPath.GetBuffer(0));
	m_strAutoSubscribe.ReleaseBuffer();

	if ("1" == m_strAutoSubscribe)
		((CButton*)GetDlgItem(IDC_CHECK_AUTO_PULLFLOW))->SetCheck(TRUE);
	else if ("0" == m_strAutoSubscribe)
		((CButton*)GetDlgItem(IDC_CHECK_AUTO_PULLFLOW))->SetCheck(FALSE);
	else
	{
		m_strAutoSubscribe = "1";
		((CButton*)GetDlgItem(IDC_CHECK_AUTO_PULLFLOW))->SetCheck(TRUE);
		WritePrivateProfileString("CONFIG", "AutoSubscribe", "1", m_strCfgPath.GetBuffer(0));
	}

	// 推送音频流
	GetPrivateProfileString("CONFIG", "PushAudio", "123", m_strPushAudio.GetBuffer(MAXSIZE), MAXSIZE, m_strCfgPath.GetBuffer(0));
	m_strPushAudio.ReleaseBuffer();

	if ("1" == m_strPushAudio)
		((CButton*)GetDlgItem(IDC_CHECK_PUSH_AUDIOFLOW))->SetCheck(TRUE);
	else if ("0" == m_strPushAudio)
		((CButton*)GetDlgItem(IDC_CHECK_PUSH_AUDIOFLOW))->SetCheck(FALSE);
	else
	{
		m_strPushAudio = "1";
		((CButton*)GetDlgItem(IDC_CHECK_PUSH_AUDIOFLOW))->SetCheck(TRUE);
		WritePrivateProfileString("CONFIG", "PushAudio", "1", m_strCfgPath.GetBuffer(0));
	}

	//推送视频流
	GetPrivateProfileString("CONFIG", "PushVideo", "123", m_strPushVideo.GetBuffer(MAXSIZE), MAXSIZE, m_strCfgPath.GetBuffer(0));
	m_strPushVideo.ReleaseBuffer();

	if ("1" == m_strPushVideo)
		((CButton*)GetDlgItem(IDC_CHECK_PUSH_VIDEOFLOW))->SetCheck(TRUE);
	else if ("0" == m_strPushVideo)
		((CButton*)GetDlgItem(IDC_CHECK_PUSH_VIDEOFLOW))->SetCheck(FALSE);
	else
	{
		m_strPushVideo = "1";
		((CButton*)GetDlgItem(IDC_CHECK_PUSH_VIDEOFLOW))->SetCheck(TRUE);
		WritePrivateProfileString("CONFIG", "PushVideo", "1", m_strCfgPath.GetBuffer(0));
	}

	//推送大小流
	GetPrivateProfileString("CONFIG", "PushSimulcast", "123", m_strPushSimulcast.GetBuffer(MAXSIZE), MAXSIZE, m_strCfgPath.GetBuffer(0));
	m_strPushSimulcast.ReleaseBuffer();

	if ("1" == m_strPushSimulcast)
	{
		((CButton*)GetDlgItem(IDC_RADIO_PUSH_SMALL))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_PUSH_BIG))->SetCheck(FALSE);
	}
	else if ("0" == m_strPushSimulcast)
	{
		((CButton*)GetDlgItem(IDC_RADIO_PUSH_BIG))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_PUSH_SMALL))->SetCheck(FALSE);
	}
	else
	{
		m_strPushSimulcast = "0";
		((CButton*)GetDlgItem(IDC_RADIO_PUSH_SMALL))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_PUSH_BIG))->SetCheck(TRUE);
		WritePrivateProfileString("CONFIG", "PushSimulcast", "0", m_strCfgPath.GetBuffer(0));
	}

	// 兼容H5
	GetPrivateProfileString("CONFIG", "H5CompatibleMode", "123", m_strCompatibleH5.GetBuffer(MAXSIZE), MAXSIZE, m_strCfgPath.GetBuffer(0));
	m_strCompatibleH5.ReleaseBuffer();

	if ("1" == m_strCompatibleH5)
		((CButton*)GetDlgItem(IDC_CHECK_COMPATIBLE_H5))->SetCheck(TRUE);
	else if ("0" == m_strCompatibleH5)
		((CButton*)GetDlgItem(IDC_CHECK_COMPATIBLE_H5))->SetCheck(FALSE);
	else
	{
		m_strCompatibleH5 = "1";
		((CButton *)GetDlgItem(IDC_CHECK_COMPATIBLE_H5))->SetCheck(TRUE);
		WritePrivateProfileString("CONFIG", "H5CompatibleMode", "1", m_strCfgPath.GetBuffer(0));
	}

	UpdateData(FALSE);
}

void CRTCSampleSettingDlg::AddConfigInfo()
{
	UpdateData(TRUE);

	WritePrivateProfileString("CONFIG", "ServerAddress", "http://11.239.175.78/app/v1/", m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "AutoPublish", "1", m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "AutoSubscribe", "1", m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "PushAudio", "1", m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "PushVideo", "1", m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "PushSimulcast", "0", m_strCfgPath.GetBuffer(0));
	WritePrivateProfileString("CONFIG", "H5CompatibleMode", "1", m_strCfgPath.GetBuffer(0));

	UpdateData(FALSE);
}

void CRTCSampleSettingDlg::OnPaint()
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

HCURSOR CRTCSampleSettingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CRTCSampleSettingDlg::IsFileExist(const CString& csFile)
{
	DWORD dwAttrib = GetFileAttributes(csFile);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void CRTCSampleSettingDlg::OnBnClickedRadioPushBig()
{
	((CButton *)GetDlgItem(IDC_RADIO_PUSH_SMALL))->SetCheck(FALSE);
	m_strPushSimulcast = "0";
}

void CRTCSampleSettingDlg::OnBnClickedRadioPushSmall()
{
	((CButton *)GetDlgItem(IDC_RADIO_PUSH_BIG))->SetCheck(FALSE);
	m_strPushSimulcast = "1";
}


void CRTCSampleSettingDlg::OnBnClickedCheckAutoPushflow()
{
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_AUTO_PUSHFLOW))->GetCheck())
		m_strAutoPublish = "1";
	else
		m_strAutoPublish = "0";
}


void CRTCSampleSettingDlg::OnBnClickedCheckAutoPullflow()
{
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_AUTO_PULLFLOW))->GetCheck())
		m_strAutoSubscribe = "1";
	else
		m_strAutoSubscribe = "0";
}


void CRTCSampleSettingDlg::OnBnClickedCheckPushAudioflow()
{
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_PUSH_AUDIOFLOW))->GetCheck())
		m_strPushAudio = "1";
	else
		m_strPushAudio = "0";
}


void CRTCSampleSettingDlg::OnBnClickedCheckPushVideoflow()
{
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_PUSH_VIDEOFLOW))->GetCheck())
		m_strPushVideo = "1";
	else
		m_strPushVideo = "0";
}


void CRTCSampleSettingDlg::OnBnClickedCheckCompatibleH5()
{
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_COMPATIBLE_H5))->GetCheck())
		m_strCompatibleH5 = "1";
	else
		m_strCompatibleH5 = "0";
}

void CRTCSampleSettingDlg::OnClose()
{
	
}