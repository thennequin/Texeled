
#ifndef __IM_WINDOW_MANAGER_DX11_CUSTOM_H__
#define __IM_WINDOW_MANAGER_DX11_CUSTOM_H__

#include "ImwConfig.h"

#include "ImwWindowManagerDX11.h"

namespace ImWindow
{
	class ImwWindowManagerDX11Custom : public ImwWindowManagerDX11
	{
	public:
		ImwWindowManagerDX11Custom(bool bCustomFrame);
		virtual							~ImwWindowManagerDX11Custom();

		virtual float					GetTitleBarHeight() const;
		virtual void					PaintTitleBar(ImwPlatformWindow* pPlatformWindow);
	};
}

#endif //__IM_WINDOW_MANAGER_DX11_CUSTOM_H__
