
// RtcSampleDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include "Log.h"
#include <memory>

#define TIMER_TIPS_CHANNELID	1001
#define TIMER_TIPS_USERID		1002

// CRtcSampleDlg 对话框
class CRtcSampleDlg : public CDialogEx
{
// 构造
public:
	CRtcSampleDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CRtcSampleDlg();

public:
	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	/*                                          成 员 函 数                                        */
	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	void TransparentPNG(CImage *png);                                                               // 显示logo
	CString ObtainFilePath();                                                                       // 获取工程路径
	void ShowEdition();                                                                             // 展示版本信息
	void RefreshChannelIdTipsText();                                                                // 刷新提示文本
	void RefreshUserIdTipsText();
	void LoadConfig();

	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	/*                                          成 员 变 量                                        */
	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	CImage m_Img;

	CString m_strCfgPath;
	CString m_strServerAddr;
	BOOL    m_bH5CompatibleMode;
	BOOL    m_bAutoSubscribe;
	BOOL    m_bAutoPublish;
	BOOL    m_bPushAudio;
	BOOL    m_bPushVideo;
	BOOL    m_bPushSimulcast;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RTCSAMPLE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnEnSetfocusEditLogin();
	afx_msg void OnEnKillfocusEditLogin();

	CString m_strChannelId;
	afx_msg void OnBnClickedButtonJoinChannel();
	CString m_strUserName;
	afx_msg void OnEnSetfocusEditUsername();
	afx_msg void OnEnKillfocusEditUsername();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonSetting();
	CString m_strEdition;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString m_strChannelText;
	CString m_strUserText;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL DestroyWindow();
};