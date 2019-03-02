
#ifndef __SHORT_KEY_MANAGER_H__
#define __SHORT_KEY_MANAGER_H__

#include <map>
#include <string.h>

#include <EasyWindow.h>

#include "ImwWindow.h"

class ShortKeyManager
{
	friend class ShortKeyWindow;
public:
	struct ShortKey
	{
		ShortKey(
			EasyWindow::EKey eDefaultPrimary = EasyWindow::KEY_NONE
			, EasyWindow::EKey eDefaultModifier1 = EasyWindow::KEY_NONE
			, EasyWindow::EKey eDefaultModifier2 = EasyWindow::KEY_NONE
			, EasyWindow::EKey eDefaultModifier3 = EasyWindow::KEY_NONE
			, EasyWindow::EKey ePrimary = EasyWindow::KEY_NONE
			, EasyWindow::EKey eModifier1 = EasyWindow::KEY_NONE
			, EasyWindow::EKey eModifier2 = EasyWindow::KEY_NONE
			, EasyWindow::EKey eModifier3 = EasyWindow::KEY_NONE
			, EasyWindow::Caller<void>* pCallback = NULL
			, EasyWindow::Caller<void>* pCallbackUp = NULL
			, bool bSave = true);
		ShortKey(const ShortKey& oCopy);

		void								Call();
		void								CallUp();
		void								RefreshKeys();
		bool								IsDefault() const;
		void								Reset();
		void								SetShortkey(EasyWindow::EKey ePrimary, EasyWindow::EKey eModifier1, EasyWindow::EKey eModifier2, EasyWindow::EKey eModifier3);

		std::string							m_sShortKey;
		EasyWindow::Caller<void>*			m_pCallback;
		EasyWindow::Caller<void>*			m_pCallbackUp;
		bool								m_bDown;
		bool								m_bDownActive;
		bool								m_bSave;
		int									m_iKeyCount;
		EasyWindow::EKey					m_eDefaultPrimaryKey;
		EasyWindow::EKey					m_eDefaultModifierKeys[3];
		EasyWindow::EKey					m_ePrimaryKey;
		EasyWindow::EKey					m_eModifierKeys[3];
	};

public:
	typedef std::map<std::string, ShortKey*> ShortKeys;

	static ShortKeyManager*				GetInstance() { return s_pInstance; }

	ShortKeyManager();
	~ShortKeyManager();
	bool								Manage(bool bCaptureKeyboard);

	const ShortKey*						RegisterShortKey(const char* pName, const char* pDefaultShortKey, EasyWindow::Caller<void>* pCallback, bool bSave = true);
	const ShortKey*						RegisterShortKeyUp(const char* pName, const char* pDefaultShortKey, EasyWindow::Caller<void>* pCallback, EasyWindow::Caller<void>* pCallbackUp, bool bSave = true);
	bool								UnregisterShortKey(const ShortKey* pShortkey);
	bool								UnregisterShortKey(const char* pName);

	bool*								GetDownShortKeyBool(const char* pName);

	void								SetKeyUsed(const EasyWindow::EKey iKeyUsed);

	void								SetShortkeysEnable(bool bEnable);

	const ShortKeys&					GetShortKeys() const;

	static bool							IsValidShortKey(const std::string& sShortKey);
	static bool							ExtractShortKey(const std::string& sShortKey,
											EasyWindow::EKey& oPrimary,
											EasyWindow::EKey& oSpecial1,
											EasyWindow::EKey& oSpecial2,
											EasyWindow::EKey& oSpecial3);
	static bool							ShortkeyToString(std::string* pOutString, EasyWindow::EKey ePrimary, EasyWindow::EKey& eSpecial1, EasyWindow::EKey& eSpecial2, EasyWindow::EKey& eSpecial3);

protected:
	bool								IsDown(const ShortKey& oKey, ImGuiIO& oIO);
protected:
	ShortKeys							m_mShortKeys;
	bool								m_bEnable;
	bool								m_pKeysUsed[512];

	static ShortKeyManager*				s_pInstance;
};


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

#endif //__SHORT_KEY_MANAGER_H__