
// FontFactory.h : main header file for the FontFactory application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CFontFactoryApp:
// See FontFactory.cpp for the implementation of this class
//

class CFontFactoryApp : public CWinAppEx
{
public:
	CFontFactoryApp();

    // GDI
    ULONG_PTR m_gdiplusToken;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CFontFactoryApp theApp;
