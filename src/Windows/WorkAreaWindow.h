#ifndef __WINDOWS_WORK_AREA_WINDOW_H__
#define __WINDOWS_WORK_AREA_WINDOW_H__

#include "ImWindow/ImwWindow.h"

namespace Windows
{
	class WorkAreaWindow : ImWindow::ImwWindow
	{
	public:
		WorkAreaWindow();
		virtual ~WorkAreaWindow();

		virtual void			OnGui();
	};
}
//namespace Windows

#endif //__WINDOWS_WORK_AREA_WINDOW_H__