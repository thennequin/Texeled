
#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "ImwWindowManagerDX11Custom.h"

#include "Graphics/Texture.h"

class ShortKeyManager;
class IDXGIFactory;
class ID3D11Device;
class ID3D11DeviceContext;

namespace GraphicResources
{
	class Texture2D;
}

namespace Windows
{
	class WorkAreaWindow;
}

class Program
{
	Program(int iArgCount, char** pArgs);
	~Program();
public:

	static Program*							CreateInstance(int iArgCount, char** pArgs);
	static void								DestroyInstance();
	static Program*							GetInstance() { return s_pInstance; }

	bool									Run();
	void									AskExit();

	ImWindow::ImwWindowManager*				GetWindowManager() { return &m_oImWindowMgrDX11; }

	IDXGIFactory*							GetDXGIFactory() const;
	ID3D11Device*							GetDX11Device() const;
	ID3D11DeviceContext*					GetDX11DeviceContext() const;


	void									LoadFile(const char* pFile);
	Graphics::Texture&						GetTexture() { return m_oTexture; }
	GraphicResources::Texture2D*			GetTexture2DRes() { return m_pTexture2D; }

protected:
	static Program*							s_pInstance;

	bool									m_bRun;

	ImWindow::ImwWindowManagerDX11Custom	m_oImWindowMgrDX11;

	ShortKeyManager*						m_pShortKeyManager;

	Windows::WorkAreaWindow*				m_pWorkAreaWindow;

	Graphics::Texture						m_oTexture;
	GraphicResources::Texture2D*			m_pTexture2D;
};

#endif // __PROGRAM_H__