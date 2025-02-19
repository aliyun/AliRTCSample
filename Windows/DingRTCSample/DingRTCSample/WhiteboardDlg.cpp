// WhiteboardDlg.cpp : implementation file
//

#include "pch.h"
#include "DingRTCSample.h"
#include "WhiteboardDlg.h"
#include "afxdialogex.h"
/* 引擎头文件 */
#include "engine_interface.h"
/* 白板头文件 */
#include "engine_wb_interface.h"

extern ding::rtc::RtcEngine *GetEngine();

/* 实现2个listener: Whiteboard manager event, 和Whiteboard event
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

	/* 开始白板，展示绘制矩形 */
	Start();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void WhiteboardDlg::OnOK()
{
	/* 关闭白板，并且通知主对话框 */
	Stop();
	GetParent()->SendMessage(WM_WHITEBOARD_CLOSED, 0, 0);

	CDialogEx::OnOK();
	delete this;
}

void WhiteboardDlg::OnCancel()
{
	/* 关闭白板，并且通知主对话框 */
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

	/* 获取白板管理并且接收其消息回调 */
	ding::rtc::RtcEngineWbManager *man = engine->GetWbManager();
	man_listener_ = new WhiteboardManListener;
	man->SetEventListener(man_listener_);
	/* 创建白板并接收其消息回调 */
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

	/* 打开白板 */
	wb->Join();
	wb->Open(pwnd->GetSafeHwnd());

	/* 设置常用的参数 */
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

	/* 选择画矩形工具 */
	wb->SetToolType(ding::rtc::WBToolType::RECT);
	/* 用户可以在白板上拖动鼠标，绘制矩形。
	 * 白板支持丰富的工具，参考文档。 */
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
