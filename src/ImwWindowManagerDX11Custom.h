
#ifndef __IM_WINDOW_MANAGER_DX11_CUSTOM_H__
#define __IM_WINDOW_MANAGER_DX11_CUSTOM_H__

#include "ImwConfig.h"

#include "ImwWindowManagerDX11.h"

namespace GraphicResources
{
	class Texture2D;
}

namespace ImWindow
{
	class ImwWindowManagerDX11Custom : public ImwWindowManagerDX11
	{
	public:
		ImwWindowManagerDX11Custom(bool bCustomFrame);
		virtual							~ImwWindowManagerDX11Custom();

		virtual bool					InternalInit();
		virtual void					InternalDestroy();

		virtual float					GetTitleBarHeight() const;
		virtual void					PaintTitleBar(ImwPlatformWindow* pPlatformWindow);

		void							ClosePlatformWindow(ImwPlatformWindow* pPlatformWindow);
	protected:
		GraphicResources::Texture2D*	m_pIcon;
	};
}

#endif //__IM_WINDOW_MANAGER_DX11_CUSTOM_H__
