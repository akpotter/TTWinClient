#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"
#include "updateDlg.h"

class CTTUpdateApp : public CWinApp
{
public:
	CTTUpdateApp();

public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	BOOL DetectStartMark();
	BOOL GetIntallFilePath(CString& path);

	DECLARE_MESSAGE_MAP()

private:
	CUpdateDlg*         m_pUpdateDlg;
};

extern CTTUpdateApp theApp;