// FastHarkLink.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFastHarkLinkApp:
// �йش����ʵ�֣������ FastHarkLink.cpp
//

class CFastHarkLinkApp : public CWinApp
{
public:
	CFastHarkLinkApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFastHarkLinkApp theApp;