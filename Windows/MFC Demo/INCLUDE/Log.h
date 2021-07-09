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
/* 日志类 版本号：1.2.0.1
/* Copyright (C) 2018 - 2019, XuRan, <xr619154254@163.com>, et al.
/*
/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#define WRILOG	WriteLog
#define DELLOG	DeleteLog

#define MAX_BUFF 10240


typedef enum {
	LOGTYPE_START   = 0,		// 日志开始
	LOGTYPE_END     = 1 << 0,	// 日志结束
	LOGTYPE_DEBUG   = 1 << 1,	// 调试信息
	LOGTYPE_ERROR   = 1 << 2,	// 错误信息
	LOGTYPE_SPECIAL = 1 << 3	// 特殊信息
}LOG_TYPE;	// 日志分类

/*********************************************************************************\ 
* 功能：记录日志																 *
* 入参：LogType    - 见定义LOG_TYPE												 *
*       strName    - 日志文件名称												 *
*		strTitle   - 日志标题													 *
*		lpszFormat - 日志信息													 *
* 出参：无																		 *
\*********************************************************************************/
extern "C" void WriteLog(LOG_TYPE LogType, CString strName, CString strTitle="", LPCTSTR lpszFormat="", ...);

/*********************************************************************************\
* 功能：定期删除日志															 *
* 入参：DeleteDays - 删除当前日期之前N天的日志									 *
* 出参：无																		 *
\*********************************************************************************/
extern "C" void DeleteLog(UINT DeleteDays);

#endif //__LOG_H__