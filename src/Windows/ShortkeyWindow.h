#ifndef __SHORT_KEY_WINDOW_H__
#define __SHORT_KEY_WINDOW_H__

#include "ShortKeyManager.h"
#include "ImwWindow.h"

class ShortKeyWindow : public ImWindow::ImwWindow
{
public:
								ShortKeyWindow();
	virtual						~ShortKeyWindow();

	virtual void				OnGui();
protected:
	void						ResetKeys();

	static int					InputTextCallback(ImGuiTextEditCallbackData *data);

	int							m_iCurrentRecord;

	bool						m_bHasSpecialPressed;
	EasyWindow::EKey			m_iPressedPrimaryKey;
	EasyWindow::EKey			m_iPressedModifierKeys[3];
	bool						m_bDoublePressedModifierKeys[3];
};

#endif //__SHORT_KEY_WINDOW_H__