#ifndef __IM_WINDOW_MANAGER_CUSTOM_H__
#define __IM_WINDOW_MANAGER_CUSTOM_H__

#include "ImwConfig.h"

#include "ImwWindowManagerDX11.h"

namespace GraphicResources
{
	class Texture2D;
}

namespace ImWindow
{
	class ImwWindowManagerCustom : public ImwWindowManagerDX11
	{
	public:
		ImwWindowManagerCustom(bool bCustomFrame);
		virtual							~ImwWindowManagerCustom();

		virtual bool					InternalInit();
		virtual void					InternalDestroy();

		virtual float					GetTitleBarHeight() const;
		virtual void					PaintTitleBar(ImwPlatformWindow* pPlatformWindow, bool bDrawTitle);

		void							ClosePlatformWindow(ImwPlatformWindow* pPlatformWindow);
	protected:
		GraphicResources::Texture2D*	m_pIcon;
	};
}

#endif //__IM_WINDOW_MANAGER_CUSTOM_H__