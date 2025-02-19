#pragma once


// WhiteboardDlg dialog
namespace ding {
namespace rtc {
class RtcEngineWhiteboard;
class RtcEngineWbManagerListener;
class RtcEngineWhiteboardEventListener;
}
}

#if !defined(COMMAND_MESSAGE_BASE)
#define COMMAND_MESSAGE_BASE 4000 // check the range used in alirtctestDlg.h
#endif
#define WBMENUITEM_GOTO_PAGE_0 (COMMAND_MESSAGE_BASE + 200)
// reserved for WBMENUITEM_GOTO_PAGE_XXX
#define WBMENUITEM_APPEND_PAGE (COMMAND_MESSAGE_BASE + 300)
#define WBMENUITEM_ROLE_ADMIN    (COMMAND_MESSAGE_BASE + 302)
#define WBMENUITEM_ROLE_ATTENDEE (COMMAND_MESSAGE_BASE + 303)
#define WBMENUITEM_ROLE_VIEWER   (COMMAND_MESSAGE_BASE + 304)
#define WM_WHITEBOARD_CLOSED (WM_USER + 4010)

class WhiteboardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(WhiteboardDlg)

public:
	WhiteboardDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~WhiteboardDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WHITEBOARD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	/* 关闭白板对话框并通知主对话框 */
	void OnOK() override;
	void OnCancel() override;

	DECLARE_MESSAGE_MAP()

	/* 开启白板 */
	void Start();
	/* 关闭白板 */
	void Stop();

private:
	/* 2个白板相关的事件listener */
	ding::rtc::RtcEngineWhiteboardEventListener *whiteboard_listener_;
	ding::rtc::RtcEngineWbManagerListener *man_listener_;
};
