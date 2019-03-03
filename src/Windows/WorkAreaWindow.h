#ifndef __WINDOWS_WORK_AREA_WINDOW_H__
#define __WINDOWS_WORK_AREA_WINDOW_H__

#include "ImWindow/ImwWindow.h"

namespace GraphicResources
{
	class Texture2D;
}

namespace Windows
{
	class WorkAreaWindow : ImWindow::ImwWindow
	{
	public:
		WorkAreaWindow();
		virtual ~WorkAreaWindow();

		virtual void			OnGui();
		virtual void			OnDropFiles(int iCount, char** pFiles, const ImVec2& oPos) override;
	protected:
		bool					m_bTiling;
		bool					m_bShowPixelGrid;
		double					m_fZoom;
		ImVec2					m_oOffset;

		double					m_fCurrentZoom;
		ImVec2					m_oCurrentOffset;

		GraphicResources::Texture2D*	m_pCheckboardTexture2DRes;
	};
}
//namespace Windows

#endif //__WINDOWS_WORK_AREA_WINDOW_H__