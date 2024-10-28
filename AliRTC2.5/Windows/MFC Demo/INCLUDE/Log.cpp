#include "stdafx.h"
#include "Log.h"

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// ��ȡ����·��
CString GetProgramPath()
{
	CString strProJectPath;
	TCHAR szPath[MAX_PATH] = { 0 };

	if (GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		(_tcsrchr(szPath, _T('\\')))[1] = 0;
	}

	strProJectPath.Format(_T("%s"), szPath);
	return strProJectPath;
}

void LogType_Start(ofstream &ofWriteLog)
{
	ofWriteLog << setw(20) << "����������������������������������������������������������������������������" << endl;
	ofWriteLog << setw(20) << "��                               S T A R T                                ��" << endl;
	ofWriteLog << setw(20) << "����������������������������������������������������������������������������" << endl;
	ofWriteLog << setw(20) << "����������������������������������������������������������������������������" << endl;
	ofWriteLog << setw(20) << "����������������������������������������������������������������������������" << endl;
}

void LogType_End(ofstream &ofWriteLog)
{
	ofWriteLog << setw(20) << "����������������������������������������������������������������������������" << endl;
	ofWriteLog << setw(20) << "����������������������������������������������������������������������������" << endl;
	ofWriteLog << setw(20) << "��                                 E N D                                  ��" << endl;
	ofWriteLog << setw(20) << "����������������������������������������������������������������������������" << endl;
	ofWriteLog << setw(20) << "" << endl;
}

void LogType_Debug(ofstream &ofWriteLog, LPCTSTR lpszFormat, CString strTitle, tm t)
{
	CString strDate("");
	strDate.Format("%04d-%02d-%02d %02d:%02d:%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	ofWriteLog << strDate << "\t" << "[DEBUG]" << "[" << strTitle << "]" << endl;
	ofWriteLog << lpszFormat << endl;
	ofWriteLog << setw(20) << "����������������������������������������������������������������������������" << endl;
}

void LogType_Error(ofstream &ofWriteLog, LPCTSTR lpszFormat, CString strTitle, tm t)
{
	CString strDate("");
	strDate.Format("%04d-%02d-%02d %02d:%02d:%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	ofWriteLog << strDate << "\t" << "[ERROR]" << "[" << strTitle << "]" << endl;
	ofWriteLog << lpszFormat << endl;
	ofWriteLog << setw(20) << "����������������������������������������������������������������������������" << endl;
}

void LogType_Special(ofstream &ofWriteLog, LPCTSTR lpszFormat, CString strTitle, tm t)
{
	CString strDate("");
	strDate.Format("%04d-%02d-%02d %02d:%02d:%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	ofWriteLog << strDate << "\t" << "[SPECIAL]" << "[" << strTitle << "]" << endl;
	ofWriteLog << lpszFormat << endl;
	ofWriteLog << setw(20) << "����������������������������������������������������������������������������" << endl;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// ��־��Ӻ���
void WriteLog(LOG_TYPE LogType, CString strName, CString strTitle, LPCTSTR lpszFormat, ...)
{
	char* pBuf = (char*)malloc(MAX_BUFF + 1);
	if (!pBuf) return;
	memset(pBuf, 0x00, MAX_BUFF + 1);

	va_list argList;
	va_start(argList, lpszFormat);
	int iRet = _vsnprintf_s(pBuf, MAX_BUFF + 1, MAX_BUFF + 1, lpszFormat, argList);
	if (iRet == -1)
	{
		pBuf = (char*)realloc(pBuf, 50 * MAX_BUFF + 1);
		memset(pBuf, 0x00, 50 * MAX_BUFF + 1);
		_vsnprintf_s(pBuf, 50 * MAX_BUFF, MAX_BUFF + 1, lpszFormat, argList);
	}
	va_end(argList);

	// ��ȡ��ǰ����
	time_t tt = time(NULL);//��䷵�ص�ֻ��һ��ʱ��cuo
	tm t;
	localtime_s(&t, &tt);

	CString strWriteLogPath;
	strWriteLogPath.Format("%sLOG\\%04d-%02d-%02d\\%s", GetProgramPath(),
		t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, strName);

	CString strWriteLogFilePath;
	strWriteLogFilePath.Format("%sLOG", GetProgramPath());

	if (!PathIsDirectory(strWriteLogFilePath))
	{
		CreateDirectory(strWriteLogFilePath, NULL);
	}

	CString strWriteDateFilePath;
	strWriteDateFilePath.Format("%sLOG\\%04d-%02d-%02d", GetProgramPath(),
		t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

	if (!PathIsDirectory(strWriteDateFilePath))
	{
		CreateDirectory(strWriteDateFilePath, NULL);
	}

	ofstream ofWriteLog;
	ofWriteLog.open(strWriteLogPath, ios::app);		// ���ļ�����д�����ļ������ھʹ�����
	if (!ofWriteLog)return;							// ���ļ�ʧ�����������

	if (LOGTYPE_START == LogType)
	{
		LogType_Start(ofWriteLog);
	}
	else if (LOGTYPE_END == LogType)
	{
		LogType_End(ofWriteLog);	
	}
	else if (LOGTYPE_DEBUG == LogType)
	{
		LogType_Debug(ofWriteLog, pBuf, strTitle, t);
	}
	else if (LOGTYPE_ERROR == LogType)
	{
		LogType_Error(ofWriteLog, pBuf, strTitle, t);
		
	}
	else if (LOGTYPE_SPECIAL == LogType)
	{
		LogType_Special(ofWriteLog, pBuf, strTitle, t);
	}

	free(pBuf);
	pBuf = NULL;
	ofWriteLog.close();
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// ɾ���ļ��к���
bool RemoveDir(const char* szFileDir)

{
	std::string strDir = szFileDir;
	if (strDir.at(strDir.length() - 1) != '\\')
		strDir += '\\';
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile((strDir + "*.*").c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	do
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(wfd.cFileName, ".") != 0 &&
				strcmp(wfd.cFileName, "..") != 0)
				RemoveDir((strDir + wfd.cFileName).c_str());
		}
		else
		{
			DeleteFile((strDir + wfd.cFileName).c_str());
		}
	} while (FindNextFile(hFind, &wfd));
	FindClose(hFind);
	RemoveDirectory(szFileDir);
	return true;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/// ��־ɾ������
void DeleteLog(UINT DeleteDays)
{
	if (DeleteDays > 0)
	{
		WIN32_FIND_DATA FindFileData;

		CString strProjectPath("");
		TCHAR szPath[MAX_PATH] = { 0 };

		if (GetModuleFileName(NULL, szPath, MAX_PATH))
		{
			(_tcsrchr(szPath, _T('\\')))[1] = 0;
		}

		strProjectPath.Format(_T("%s"), szPath);
		CString sAllFile = strProjectPath + "LOG\\" + "*";

		HANDLE hFind = ::FindFirstFile(sAllFile, &FindFileData);

		if (INVALID_HANDLE_VALUE == hFind) return;

		do
		{
			// ����"."��".."������� 
			if (strcmp(FindFileData.cFileName, ".") == 0 || strcmp(FindFileData.cFileName, "..") == 0)
				continue;

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)    // �Ƿ���Ŀ¼ 
			{//Ŀ¼
				CString sFileName;
				sFileName.Format("%s", FindFileData.cFileName);

				if (sFileName.GetLength() > 1)
				{
					CString sFileFullPath = strProjectPath + "LOG\\" + sFileName;// �ļ�ȫ·��

					sFileName.Replace("-", "");

					__int64 nFileName = _atoi64(sFileName);   // ȡ���ļ������ڣ��磺20101030
					CTime tNowTime = CTime::GetCurrentTime();
					tNowTime = tNowTime - CTimeSpan(DeleteDays, 0, 0, 0); // ָ��nDays��ǰ������
					__int64 nNowTime = _atoi64(tNowTime.Format("%Y%m%d"));

					if (20000000 < nFileName && nFileName < nNowTime)
					{
						RemoveDir(sFileFullPath);
					}
				}
			}
		} while (FindNextFile(hFind, &FindFileData));

		FindClose(hFind);
	}
}