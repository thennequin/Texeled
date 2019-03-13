#ifndef _STATUS_BARS_H_
#define _STATUS_BARS_H_

#include "ImWindow/ImwStatusBar.h"
class StatusBars : ImWindow::ImwStatusBar
{
public:
	StatusBars();
	virtual ~StatusBars();
	virtual void				OnStatusBar();
};

#endif //_STATUS_BARS_H_