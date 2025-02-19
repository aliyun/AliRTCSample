
// DingRTCSampleDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "DingRTCSample.h"
#include "DingRTCSampleDlg.h"
#include "afxdialogex.h"
#include "engine_interface.h"
#include "WhiteboardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// �����demo��֧��1��remote user
std::string remoteUserId;

CDingRTCSampleDlg *main_wnd_ = NULL;
// ����Զ�����Ƶ
bool pub_video_when_join_ = true;
// ����Զ�����Ƶ
bool pub_audio_when_join_ = true;
// �Զ�����
bool auto_sub_audio_ = true;
bool auto_sub_video_ = true;
ding::rtc::RtcEngineVideoStreamType prefer_video_quality_ = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeFHD;

class MyEngineEventListerner : public ding::rtc::RtcEngineEventListener
{
public:
	MyEngineEventListerner()
	{
	}
	~MyEngineEventListerner()
	{
	}

	void OnJoinChannelResult(int result, const char *channel,
		const char *userId, int elapsed) override
	{
		// ��������SDK��Ϣ�ص����ٵ���SDK��API
		UpdateMeetingInfoParam *param = new UpdateMeetingInfoParam;
		param->in_room = true;
		main_wnd_->UpdateUIAsync(param);
	}

	void OnLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats) override
	{
	}
	void OnRemoteUserOnLineNotify(const char *uid, int elapsed) override
	{
	}
	void OnRemoteUserOffLineNotify(const char *uid,
        ding::rtc::RtcEngineUserOfflineReason reason) override
	{
	}
	void OnBye(ding::rtc::RtcEngineOnByeType code) override
	{
	}
	void OnOccurError(int error, const char* msg) override
	{
	}
	void OnAudioPublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
		ding::rtc::RtcEnginePublishState newState,
		int elapseSinceLastState, const char* channel) override
	{
	}
	void OnVideoPublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
		ding::rtc::RtcEnginePublishState newState,
		int elapseSinceLastState, const char* channel) override
	{
	}
	void OnScreenSharePublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
		ding::rtc::RtcEnginePublishState newState,
		int elapseSinceLastState,
		const char* channel) override
	{
	}
	void OnRemoteTrackAvailableNotify(const char *uid,
		ding::rtc::RtcEngineAudioTrack audioTrack,
		ding::rtc::RtcEngineVideoTrack videoTrack) override
	{
		UpdateRemoteTrackStateParam *param = new UpdateRemoteTrackStateParam;
		param->uid = uid;
		param->has_audio_track = audioTrack == ding::rtc::RtcEngineAudioTrack::RtcEngineAudioTrackMic;
		param->has_video_track = videoTrack == ding::rtc::RtcEngineVideoTrack::RtcEngineVideoTrackCamera || videoTrack == ding::rtc::RtcEngineVideoTrack::RtcEngineVideoTrackBoth;
		param->has_screen_track = videoTrack == ding::rtc::RtcEngineVideoTrack::RtcEngineVideoTrackScreen || videoTrack == ding::rtc::RtcEngineVideoTrack::RtcEngineVideoTrackBoth;
		main_wnd_->UpdateUIAsync(param);
	}
};

static ding::rtc::RtcEngine *engine_ = NULL;
static MyEngineEventListerner *listener_ = NULL;

// CAboutDlg dialog used for App About

ding::rtc::RtcEngine *GetEngine()
{
	return engine_;
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CDingRTCSampleDlg dialog



CDingRTCSampleDlg::CDingRTCSampleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DINGRTCSAMPLE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	whiteboard_dlg_ = NULL;
}

CDingRTCSampleDlg::~CDingRTCSampleDlg()
{
	if(whiteboard_dlg_) {
		delete whiteboard_dlg_;
		whiteboard_dlg_ = NULL;
	}
}

void CDingRTCSampleDlg::OnClickedBtnOpenWhiteboard()
{
	// if whiteboard dialog exists, then close it; otherwise, create and show it
	if (whiteboard_dlg_) {
		whiteboard_dlg_->ShowWindow(SW_SHOW);
		return;
	}

	whiteboard_dlg_ = new WhiteboardDlg(this);
	whiteboard_dlg_->Create(IDD_DIALOG_WHITEBOARD);
	whiteboard_dlg_->ShowWindow(SW_SHOW);
}

LRESULT CDingRTCSampleDlg::onWhiteboardClosed(WPARAM wParam, LPARAM lParam)
{
	whiteboard_dlg_ = NULL;
	return 0;
}

void CDingRTCSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDingRTCSampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_UPDATE_UI, &OnUpdateUI)
	/* �򿪰װ�Ի��� */
	ON_BN_CLICKED(ID_BTN_OPEN_WHITEBOARD, OnClickedBtnOpenWhiteboard)
	/* ���װ�Ի���ر�ʱ������WM_WHITEBOARD_CLOSED��Ϣ�����Ի��� */
	ON_MESSAGE(WM_WHITEBOARD_CLOSED, onWhiteboardClosed)
END_MESSAGE_MAP()


// CDingRTCSampleDlg message handlers

BOOL CDingRTCSampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	main_wnd_ = this;
	const char *ver = ding::rtc::RtcEngine::GetSDKVersion();
	CStringA s(ver);
	AfxMessageBox(CString(s));

	// create engine and set message handler
	engine_ = ding::rtc::RtcEngine::Create(""); // do not pass NULL pointer!
	listener_ = new MyEngineEventListerner();
	engine_->SetEngineEventListener((ding::rtc::RtcEngineEventListener *)listener_);

	// �򿪱���Ԥ��
	HWND local_view = GetDlgItem(IDC_STATIC_LOCAL)->m_hWnd;
	ding::rtc::RtcEngineVideoCanvas canvas;
	canvas.displayView = local_view;
	canvas.mirrorMode = ding::rtc::RtcEngineRenderMirrorModeAllMirror;
	canvas.renderMode = ding::rtc::RtcEngineRenderMode::RtcEngineRenderModeCrop;
	engine_->SetLocalViewConfig(canvas, ding::rtc::RtcEngineVideoTrackCamera);
	engine_->StartPreview();

	// �����Զ�����
	// SDK��֧��, app��OnJoinChannelResult������Ҫ����Ƶ������Ƶ

	// �����Զ�����
	engine_->SubscribeAllRemoteAudioStreams(auto_sub_audio_);
	engine_->SubscribeAllRemoteVideoStreams(auto_sub_video_);
	engine_->SetRemoteDefaultVideoStreamType(prefer_video_quality_);

	// ���
	// app����ʵ�ַ���appserver�Ĵ��룬��ö�Ӧ������õ�token������DingRTC���
	// Ϊ�˷�ֹ�����������GUI�����鲻Ҫ��token��������token���յ����첽ִ��
	// JoinChannel���
	GetTokenAsync();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDingRTCSampleDlg::DestroyEngine()
{
	if(engine_ != NULL) {
		ding::rtc::RtcEngine::Destroy(engine_);
		engine_ = NULL;
	}

    if(listener_ != NULL) {
        delete listener_;
        listener_ = NULL;
    }
}

void CDingRTCSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDingRTCSampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDingRTCSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDingRTCSampleDlg::GetTokenAsync()
{
	ding::rtc::RtcEngineAuthInfo auth;
	auth.channelId  = (char *)channelName.c_str(); /* Ƶ��ID */
	auth.appId      = (char *)appid.c_str();       /* Ӧ��ID */
	auth.token      = (char *)accessToken.c_str(); /* Ƶ����Ȩ����Token */
	auth.userId     = (char *)uid.c_str();         /* �û�ID */
	// auth.gslbServer = (char *)gslb.c_str(); /* GSLB��ַ�ɲ��� */;
	engine_->JoinChannel(auth, nick.c_str() /* ��ʾ���� */);
}

LRESULT CDingRTCSampleDlg::OnUpdateUI(WPARAM wParam, LPARAM lParam)
{
	while (true) {
		// ȡ��һ����Ϣ
		lock_.lock();
		if(update_ui_list_.size() == 0) {
			lock_.unlock();
			break;
		}
		UpdateUIParam *param = update_ui_list_.front();
		update_ui_list_.pop_front();
		lock_.unlock();

		// JoinChannel�ص�
		if (param->type_ == UpdateMeetingInfoParam::param_type_) {
			bool succeed = ((UpdateMeetingInfoParam *)param)->in_room;
			if (!succeed) {
				AfxMessageBox(_T("failed to join!"));
			}
			else {
				// �����Ҫ�Զ�������������Է�����������
				if (pub_video_when_join_) {
		            engine_->PublishLocalVideoStream(true);
				}
				if (pub_audio_when_join_) {
		            engine_->PublishLocalAudioStream(true);
				}
			}
		}
		else if (param->type_ == UpdateRemoteTrackStateParam::param_type_) {
			UpdateRemoteTrackStateParam *p = (UpdateRemoteTrackStateParam *)param;
			if (!remoteUserId.empty()) {
				AfxMessageBox(_T("Another user joined. This demo ignores more users"));
			}
			if (p->has_video_track) {
				HWND remote_view = GetDlgItem(IDC_STATIC_REMOTE)->m_hWnd;
				ding::rtc::RtcEngineVideoCanvas canvas;
				canvas.displayView = remote_view;
				canvas.mirrorMode = ding::rtc::RtcEngineRenderMirrorModeAllMirror;
				canvas.renderMode = ding::rtc::RtcEngineRenderMode::RtcEngineRenderModeCrop;
				engine_->SetRemoteViewConfig(canvas, p->uid.c_str(), ding::rtc::RtcEngineVideoTrackCamera);
				remoteUserId = p->uid; // û���㹻��Ĵ��ڸ�����remote userʹ����
			}
		}

		delete param;
	}

	return 0;
}


void CDingRTCSampleDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	DestroyEngine();

	CDialogEx::OnOK();
}


void CDingRTCSampleDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	DestroyEngine();

	CDialogEx::OnCancel();
}
