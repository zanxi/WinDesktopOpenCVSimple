
// Tempo.h : main header file for the Tempo application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

#define WMU_SETFPS							(WM_APP + 1)
#define WMU_SETFPS_START					(WM_APP + 2)
#define WMU_SETFPS_STOP 					(WM_APP + 3)

// CTempoApp:
// See Tempo.cpp for the implementation of this class
//

class CTempoApp : public CWinAppEx
{
public:
	CTempoApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTempoApp theApp;
