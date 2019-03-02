
#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "ImwWindowManagerDX11Custom.h"

class ShortKeyManager;

class Program
{
	Program();
	~Program();
public:

	static Program*							CreateInstance();
	static void								DestroyInstance();
	static Program*							GetInstance() { return s_pInstance; }

	bool									Run();
	void									AskExit();

	ImWindow::ImwWindowManager*				GetWindowManager() { return &m_oMgr; }

protected:
	static Program*							s_pInstance;

	bool									m_bRun;

	ImWindow::ImwWindowManagerDX11Custom	m_oMgr;

	ShortKeyManager*						m_pShortKeyManager;
};

#endif // __PROGRAM_H__