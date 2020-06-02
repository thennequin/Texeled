#ifndef _MENUS_H_
#define _MENUS_H_

#include "ImWindow/ImwMenu.h"

#include "GraphicResources/Texture2D.h"

class Menus : ImWindow::ImwMenu
{
public:
	Menus();
	virtual ~Menus();

	virtual void						OnMenu();
protected:
	bool								m_bResizeKeepRatio;
	int									m_iResizeNewWidth;
	int									m_iResizeNewHeight;
	double								m_fResizeRatio;

	GraphicResources::Texture2D*		m_pIconOpen;
	GraphicResources::Texture2D*		m_pIconSave;
	GraphicResources::Texture2D*		m_pIconSaveAs;
	GraphicResources::Texture2D*		m_pIconExit;
	GraphicResources::Texture2D*		m_pIconPrevious;
	GraphicResources::Texture2D*		m_pIconNext;
	GraphicResources::Texture2D*		m_pIconResize;
	GraphicResources::Texture2D*		m_pIconMipMap;
	GraphicResources::Texture2D*		m_pIconMissingMipMap;
	GraphicResources::Texture2D*		m_pIconCurrentMipMap;
	GraphicResources::Texture2D*		m_pIconCut;
	GraphicResources::Texture2D*		m_pIconLog;
	GraphicResources::Texture2D*		m_pIconHelp;
};

#endif //_MENUS_H_