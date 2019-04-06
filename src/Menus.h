#ifndef _MENUS_H_
#define _MENUS_H_

#include "ImWindow/ImwMenu.h"

class Menus : ImWindow::ImwMenu
{
public:
								Menus();

	virtual void				OnMenu();
protected:
	bool						m_bResizeKeepRatio;
	int							m_iResizeNewWidth;
	int							m_iResizeNewHeight;
	double						m_fResizeRatio;
};

#endif //_MENUS_H_