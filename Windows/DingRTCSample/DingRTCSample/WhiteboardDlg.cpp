// WhiteboardDlg.cpp : implementation file
//

#include "pch.h"
#include "DingRTCSample.h"
#include "WhiteboardDlg.h"
#include "afxdialogex.h"
/* ����ͷ�ļ� */
#include "engine_interface.h"
/* �װ�ͷ�ļ� */
#include "engine_wb_interface.h"

extern ding::rtc::RtcEngine *GetEngine();

/* ʵ��2��listener: Whiteboard manager event, ��Whiteboard event
 */
class WhiteboardListener : public ding::rtc::RtcEngineWhiteboardEventListener
{
public:
	void SetWhiteboardDlg(WhiteboardDlg* d) {
		dlg = d;
	}


private:
	WhiteboardDlg *dlg{ nullptr };
};

class WhiteboardManListener : public ding::rtc::RtcEngineWbManagerListener
{
public:
	void OnWhiteboardStart(const char* whiteboardId, ding::rtc::WBConfig cfg) override
	{
		printf("OnWhiteboardStart\n");
	}

	void OnWhiteboardStop(const char* whiteboardId) override
	{
		printf("OnWhiteboardStop\n");
	}
};

// WhiteboardDlg dialog

IMPLEMENT_DYNAMIC(WhiteboardDlg, CDialogEx)

WhiteboardDlg::WhiteboardDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_WHITEBOARD, pParent)
{
	whiteboard_listener_ = NULL;
	man_listener_ = NULL;
}

WhiteboardDlg::~WhiteboardDlg()
{
	if (whiteboard_listener_)
		delete whiteboard_listener_;
	if (man_listener_)
		delete man_listener_;
}

void WhiteboardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(WhiteboardDlg, CDialogEx)
END_MESSAGE_MAP()


// WhiteboardDlg message handlers
BOOL WhiteboardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	/* ��ʼ�װ壬չʾ���ƾ��� */
	Start();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void WhiteboardDlg::OnOK()
{
	/* �رհװ壬����֪ͨ���Ի��� */
	Stop();
	GetParent()->SendMessage(WM_WHITEBOARD_CLOSED, 0, 0);

	CDialogEx::OnOK();
	delete this;
}

void WhiteboardDlg::OnCancel()
{
	/* �رհװ壬����֪ͨ���Ի��� */
	Stop();
	GetParent()->SendMessage(WM_WHITEBOARD_CLOSED, 0, 0);

	CDialogEx::OnCancel();
	delete this;
}

void WhiteboardDlg::Start()
{
	CWnd *pwnd = GetDlgItem(IDC_STATIC_PAINT_AREA);

	ding::rtc::RtcEngine *engine = GetEngine();
	if (!engine) {
		return;
	}

	/* ��ȡ�װ�����ҽ�������Ϣ�ص� */
	ding::rtc::RtcEngineWbManager *man = engine->GetWbManager();
	man_listener_ = new WhiteboardManListener;
	man->SetEventListener(man_listener_);
	/* �����װ岢��������Ϣ�ص� */
	ding::rtc::WBConfig config;
	config.mode = ding::rtc::WBMode::BASIC;
	config.width = 1280;
	config.height = 720;
	ding::rtc::RtcEngineWhiteboard *wb = man->CreateWhiteboard("default", config);
	if (wb == NULL) {
		MessageBox(_T("Failed to CreateWhiteboard!"), _T("error"), MB_ICONHAND);
		return;
	}

	WhiteboardListener *wb_listener = new WhiteboardListener;
	wb_listener->SetWhiteboardDlg(this);
	whiteboard_listener_ = wb_listener;
	wb->SetEventListener(whiteboard_listener_);

	/* �򿪰װ� */
	wb->Join();
	wb->Open(pwnd->GetSafeHwnd());

	/* ���ó��õĲ��� */
	bool enable = true;
	wb->SetOption(ding::rtc::WBOption::ENABLE_LASER_TRAIL, &enable, sizeof(enable));
	wb->SetOption(ding::rtc::WBOption::ENABLE_CURSORPOS_SYNC, &enable, sizeof(enable));
	wb->SetOption(ding::rtc::WBOption::ENABLE_SHOW_REMOTE_CURSOR, &enable, sizeof(enable));
	wb->SetOption(ding::rtc::WBOption::ENABLE_LOCAL_CURSOR_LABEL, &enable, sizeof(enable));
	//wb->SetBackgroundColor({(float)(240.f/255), (float)(255.0/255), (float)(240.f/255), 1});
	wb->SetRole(ding::rtc::WBRoleType::ATTENDEE);
	wb->SetToolType(ding::rtc::WBToolType::SELECT);
	wb->SetFillType(ding::rtc::WBFillType::COLOR);
	wb->SetFillColor({0.0f, 1.0f, 1.0f, 1.0f}); // brush color
	wb->SetColor({0.f/255, 0, 0.f/255, 1});
	wb->SetScalingMode(ding::rtc::WBScalingMode::Fit);

	/* ѡ�񻭾��ι��� */
	wb->SetToolType(ding::rtc::WBToolType::RECT);
	/* �û������ڰװ����϶���꣬���ƾ��Ρ�
	 * �װ�֧�ַḻ�Ĺ��ߣ��ο��ĵ��� */
}

void WhiteboardDlg::Stop()
{
	ding::rtc::RtcEngine *engine = GetEngine();
	if (!engine) {
		return;
	}

	ding::rtc::RtcEngineWbManager *man = engine->GetWbManager();
	ding::rtc::RtcEngineWhiteboard *wb = man->GetWhiteboard("default");
	if (wb) {
		wb->Close();
	}
}
