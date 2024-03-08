
// DingRTCSample.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDingRTCSampleApp:
// See DingRTCSample.cpp for the implementation of this class
//

class CDingRTCSampleApp : public CWinApp
{
public:
	CDingRTCSampleApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDingRTCSampleApp theApp;
