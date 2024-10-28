#ifndef __LOG_H__
#define __LOG_H__

#include <afx.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <Shlwapi.h>

using namespace std;

/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*                      _    _ ____  ____ _   _ _    _  ____
/*  Project            \ \  / |  _ \|  __| | | | \  | |/ ___\
/*                      \ \/ /| |_) | |__| | | | |\ | | /
/*                      / /\ \|  _ <|  __| |_| | | \| | \___
/*                     /_/  \_|_| \_|_|   \___/|_|  \_|\____/
/*
/* ��־�� �汾�ţ�1.2.0.1
/* Copyright (C) 2018 - 2019, XuRan, <xr619154254@163.com>, et al.
/*
/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#define WRILOG	WriteLog
#define DELLOG	DeleteLog

#define MAX_BUFF 10240


typedef enum {
	LOGTYPE_START   = 0,		// ��־��ʼ
	LOGTYPE_END     = 1 << 0,	// ��־����
	LOGTYPE_DEBUG   = 1 << 1,	// ������Ϣ
	LOGTYPE_ERROR   = 1 << 2,	// ������Ϣ
	LOGTYPE_SPECIAL = 1 << 3	// ������Ϣ
}LOG_TYPE;	// ��־����

/*********************************************************************************\ 
* ���ܣ���¼��־																 *
* ��Σ�LogType    - ������LOG_TYPE												 *
*       strName    - ��־�ļ�����												 *
*		strTitle   - ��־����													 *
*		lpszFormat - ��־��Ϣ													 *
* ���Σ���																		 *
\*********************************************************************************/
extern "C" void WriteLog(LOG_TYPE LogType, CString strName, CString strTitle="", LPCTSTR lpszFormat="", ...);

/*********************************************************************************\
* ���ܣ�����ɾ����־															 *
* ��Σ�DeleteDays - ɾ����ǰ����֮ǰN�����־									 *
* ���Σ���																		 *
\*********************************************************************************/
extern "C" void DeleteLog(UINT DeleteDays);

#endif //__LOG_H__