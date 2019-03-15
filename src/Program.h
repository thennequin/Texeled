
#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "ImwWindowManagerDX11Custom.h"

#include "Graphics/Texture.h"

#include "ShortKeyManager.h"

class Menus;
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


struct Shortkeys
{
	const ShortKeyManager::ShortKey* pClose;
	const ShortKeyManager::ShortKey* pOpen;
	const ShortKeyManager::ShortKey* pSave;
	const ShortKeyManager::ShortKey* pSaveAs;
};

struct Fonts
{
	ImFont* pFontConsolasBold;
	ImFont* pFontConsolas;
};

struct ProgramMode
{
	enum Enum
	{
		VIEWER,
		EDITION_COLOR,
		EDITION_NORMAL,

		_COUNT,
		_LAST = _COUNT - 1
	};
};
typedef ProgramMode::Enum ProgramModeEnum;
extern const char* ProgramModeStrings[ProgramMode::_COUNT];

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

	bool									LoadFile(const char* pFile);
	Graphics::Texture&						GetTexture() { return m_oTexture; }
	GraphicResources::Texture2D*			GetTexture2DRes() { return m_pTexture2D; }
	void									UpdateTexture2DRes();

	void									Open();
	void									Save();
	void									SaveAs();

	const Shortkeys&						GetShortkeys() const { return m_oShortkeys; }
	const Fonts&							GetFonts() const { return m_oFonts; }

	ProgramModeEnum							GetMode() const { return m_eMode; }
	void									SetMode(ProgramModeEnum eMode) { m_eMode = eMode; }
protected:
	static Program*							s_pInstance;

	bool									m_bRun;

	ImWindow::ImwWindowManagerDX11Custom	m_oImWindowMgrDX11;

	Fonts									m_oFonts;

	ShortKeyManager*						m_pShortKeyManager;
	Shortkeys								m_oShortkeys;
	Menus*									m_pMenus;

	ProgramModeEnum							m_eMode;

	Windows::WorkAreaWindow*				m_pWorkAreaWindow;

	Graphics::Texture						m_oTexture;
	GraphicResources::Texture2D*			m_pTexture2D;
};

#endif // __PROGRAM_H__