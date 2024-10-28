
// RtcSample.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "RtcSample.h"
#include "RtcSampleDlg.h"

#include <Dbghelp.h>
#include <signal.h>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void signal_abort_handler(int signal)
{
	time_t  nowtime;
	time(&nowtime);
	struct tm *pTime = NULL;
	localtime_s(pTime, &nowtime);
	char szFile[128] = { 0 };

	sprintf_s(szFile, "RtcSample_abort_%4d_%02d_%02d_%02d_%02d_%02d.dmp", pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
	HANDLE hFile = CreateFileA(szFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_TYPE flags = (MINIDUMP_TYPE)(MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithHandleData | MiniDumpWithUnloadedModules | MiniDumpWithThreadInfo);
		BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, flags, 0, NULL, NULL);
		CloseHandle(hFile);
	}

	return;
}

LONG WINAPI exception_handler(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
	LONG ret = EXCEPTION_CONTINUE_SEARCH;

	time_t  nowtime;
	time(&nowtime);
	struct tm *pTime = NULL;
	localtime_s(pTime, &nowtime);
	char szFile[128] = { 0 };

	sprintf_s(szFile, "RtcSample_exception_%4d_%02d_%02d_%02d_%02d_%02d.dmp", pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
	HANDLE hFile = CreateFileA(szFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;

		MINIDUMP_TYPE flags = (MINIDUMP_TYPE)(MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithHandleData | MiniDumpWithUnloadedModules | MiniDumpWithThreadInfo);
		BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, flags, &ExInfo, NULL, NULL);
		ret = EXCEPTION_EXECUTE_HANDLER;
		CloseHandle(hFile);
	}

	return ret;
}

BOOL DisableSetUnhandledExceptionFilter()
{
	HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
	if (hKernel32 == NULL) return FALSE;
	void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
	if (pOrgEntry == NULL) return FALSE;

#ifdef _M_IX86
	unsigned char szExecute[] = { 0x33, 0xC0, 0xC2, 0x04, 0x00 };
#elif _M_X64
	unsigned char szExecute[] = { 0x33, 0xC0, 0xC3 };
#endif

	SIZE_T bytesWritten = 0;
	DWORD dwOldFlag, dwTempFlag;
	//VirtualProtect(pOrgEntry, sizeof(szExecute), PAGE_READWRITE, &dwOldFlag);
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
		pOrgEntry, szExecute, sizeof(szExecute), &bytesWritten);
	//VirtualProtect(pOrgEntry, sizeof(szExecute), dwOldFlag, &dwTempFlag);
	return bRet;
}

 // CRtcSampleApp

BEGIN_MESSAGE_MAP(CRtcSampleApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CRtcSampleApp 构造

CRtcSampleApp::CRtcSampleApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CRtcSampleApp 对象

CRtcSampleApp theApp;


// CRtcSampleApp 初始化

BOOL CRtcSampleApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	//set crash dump handler
	signal(SIGABRT, signal_abort_handler);
	_set_abort_behavior(0, _WRITE_ABORT_MSG);
	SetUnhandledExceptionFilter(exception_handler);
	DisableSetUnhandledExceptionFilter();

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CRtcSampleDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

