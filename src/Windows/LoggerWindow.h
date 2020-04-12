#ifndef __WINDOWS_LOGGER_WINDOW_H__
#define __WINDOWS_LOGGER_WINDOW_H__

#include "ImWindow/ImwWindow.h"

namespace Windows
{
	class LoggerWindow : public ImWindow::ImwWindow
	{
	public:
		LoggerWindow();
		virtual							~LoggerWindow();

		virtual void					OnGui();
	};
}

#endif //__WINDOWS_LOGGER_WINDOW_H__