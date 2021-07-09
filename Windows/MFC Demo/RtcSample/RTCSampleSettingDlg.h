#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <functional>

// CRTCSampleSettingDlg 对话框

#define MAXSIZE 1024

class CRTCSampleSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRTCSampleSettingDlg)

public:
	CRTCSampleSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRTCSampleSettingDlg();

	public:
	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	/*                                        成 员 函 数                                          */
	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	CString ObtainFilePath();                                                                       // 获取工程路径
	void LoadConfigInfo();                                                                          // 加载配置文件
	BOOL IsFileExist(const CString& csFile);
	void AddConfigInfo();
	std::function<void()> doneCallback;
	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	/*                                        成 员 变 量                                          */
	/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
	CString m_strCfgPath;
	CString m_strAutoPublish;
	CString m_strAutoSubscribe;
	CString m_strPushAudio;
	CString m_strPushVideo;
	CString m_strPushSimulcast;
	CString m_strCompatibleH5;
	

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	HICON m_hIcon;
	virtual void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonConfirmChang();
	afx_msg void OnBnClickedButtonCancelChang();
	virtual BOOL OnInitDialog();
	CString m_strServerAddr;
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedRadioPushBig();
	afx_msg void OnBnClickedRadioPushSmall();
	afx_msg void OnBnClickedCheckAutoPushflow();
	afx_msg void OnBnClickedCheckAutoPullflow();
	afx_msg void OnBnClickedCheckPushAudioflow();
	afx_msg void OnBnClickedCheckPushVideoflow();
	afx_msg void OnBnClickedCheckCompatibleH5();
};
