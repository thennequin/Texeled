#ifndef __WINDOWS_ABOUT_WINDOW_H__
#define __WINDOWS_ABOUT_WINDOW_H__

#include "ImWindow/ImwWindow.h"

namespace GraphicResources
{
	class Texture2D;
}

namespace Windows
{
	class AboutWindow : public ImWindow::ImwWindow
	{
	public:
		AboutWindow();
		virtual							~AboutWindow();

		virtual void					OnGui();
	protected:
		GraphicResources::Texture2D*	m_pIcon;
	};
}

#endif //__WINDOWS_ABOUT_WINDOW_H__