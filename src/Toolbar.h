#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

#include "ImWindow/ImwToolBar.h"

class Toolbar : ImWindow::ImwToolBar
{
public:
	Toolbar();
	~Toolbar();
	virtual void				OnToolBar();
};

#endif //_TOOLBAR_H_