
// DingRTCSampleDlg.h : header file
//

#pragma once

#include <string>
#include <mutex>
#include <list>

// base
class UpdateUIParam {
public:
	UpdateUIParam(const char *type) {
		type_ = type;
	}
	virtual ~UpdateUIParam() {}
	std::string type_;
};

// join result
class UpdateMeetingInfoParam : public UpdateUIParam {
public:
	UpdateMeetingInfoParam() : UpdateUIParam(param_type_) {}

	static constexpr char *param_type_ = "UpdateMeetingInfoParam";
	bool in_room;
};

class UpdateRemoteTrackStateParam : public UpdateUIParam {
public:
	UpdateRemoteTrackStateParam() : UpdateUIParam(param_type_) {}

	static constexpr char *param_type_ = "UpdateRemoteTrackStateParam";
	std::string uid;
	bool has_audio_track;
	bool has_video_track;
	bool has_screen_track;
};


#define WM_UPDATE_UI        (WM_USER + 3006)

class WhiteboardDlg;

// CDingRTCSampleDlg dialog
class CDingRTCSampleDlg : public CDialogEx
{
// Construction
public:
	CDingRTCSampleDlg(CWnd* pParent = nullptr);	// standard constructor
	virtual ~CDingRTCSampleDlg();

	// �첽����һ����Ϣ
	void UpdateUIAsync(UpdateUIParam *param) {
		// ����һ��������
		lock_.lock();
		update_ui_list_.push_back(param);
		lock_.unlock();
		// �����߳��д�����Ϣ����
		PostMessage(WM_UPDATE_UI, 0, 0);
	}
private:
	std::list<UpdateUIParam *> update_ui_list_;
	std::mutex lock_;

	// ��appserver�������token
	void GetTokenAsync();

	void DestroyEngine();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DINGRTCSAMPLE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	/* �װ�Ի��� */
	WhiteboardDlg *whiteboard_dlg_;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnUpdateUI(WPARAM wParam, LPARAM lParam);
	/* ��ť��Ϣ�������װ�Ի��� */
	afx_msg void OnClickedBtnOpenWhiteboard();
	/* �����װ�Ի����˳�ʱ��������Ϣ���ͷŶԻ�����Դ */
	afx_msg LRESULT onWhiteboardClosed(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
};
