
#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "ImwWindowManagerDX11Custom.h"

class ShortKeyManager;
class IDXGIFactory;
class ID3D11Device;
class ID3D11DeviceContext;

namespace Windows
{
	class WorkAreaWindow;
}

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

	ImWindow::ImwWindowManager*				GetWindowManager() { return &m_oImWindowMgrDX11; }

	IDXGIFactory*							GetDXGIFactory() const;
	ID3D11Device*							GetDX11Device() const;
	ID3D11DeviceContext*					GetDX11DeviceContext() const;

protected:
	static Program*							s_pInstance;

	bool									m_bRun;

	ImWindow::ImwWindowManagerDX11Custom	m_oImWindowMgrDX11;

	ShortKeyManager*						m_pShortKeyManager;

	Windows::WorkAreaWindow*				m_pWorkAreaWindow;
};

#endif // __PROGRAM_H__