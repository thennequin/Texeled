
#include "ShortKeyManager.h"

#include "ImwWindowManager.h"

#include <algorithm>
#include <vector>

#define ImwVerify(bCondition) { if ((bCondition) == false) { IM_ASSERT(false); } }

ShortKeyManager* ShortKeyManager::s_pInstance = NULL;


ShortKeyManager::ShortKey::ShortKey(
	EasyWindow::EKey eDefaultPrimary
	, EasyWindow::EKey eDefaultModifier1
	, EasyWindow::EKey eDefaultModifier2
	, EasyWindow::EKey eDefaultModifier3
	, EasyWindow::EKey ePrimary
	, EasyWindow::EKey eModifier1
	, EasyWindow::EKey eModifier2
	, EasyWindow::EKey eModifier3
	, EasyWindow::Caller<void>* pCallback
	, EasyWindow::Caller<void>* pCallbackUp
	, bool bSave
	, bool bDeleteCallbacks)
{
	m_eDefaultPrimaryKey = eDefaultPrimary;
	m_eDefaultModifierKeys[0] = eDefaultModifier1;
	m_eDefaultModifierKeys[1] = eDefaultModifier2;
	m_eDefaultModifierKeys[2] = eDefaultModifier3;

	m_ePrimaryKey = ePrimary;
	m_eModifierKeys[0] = eModifier1;
	m_eModifierKeys[1] = eModifier2;
	m_eModifierKeys[2] = eModifier3;

	RefreshKeys();

	m_pCallback = pCallback;
	m_pCallbackUp = pCallbackUp;
	m_bDown = false;
	m_bDownActive = false;
	m_bSave = bSave;
	m_bDeleteCallbacks = bDeleteCallbacks;
}

ShortKeyManager::ShortKey::ShortKey(const ShortKeyManager::ShortKey& oCopy)
{
	m_eDefaultPrimaryKey = oCopy.m_eDefaultPrimaryKey;
	m_eDefaultModifierKeys[0] = oCopy.m_eDefaultModifierKeys[0];
	m_eDefaultModifierKeys[1] = oCopy.m_eDefaultModifierKeys[1];
	m_eDefaultModifierKeys[2] = oCopy.m_eDefaultModifierKeys[2];

	m_ePrimaryKey = oCopy.m_ePrimaryKey;
	m_eModifierKeys[0] = oCopy.m_eModifierKeys[0];
	m_eModifierKeys[1] = oCopy.m_eModifierKeys[1];
	m_eModifierKeys[2] = oCopy.m_eModifierKeys[2];

	RefreshKeys();

	m_pCallback = oCopy.m_pCallback;
	m_pCallbackUp = oCopy.m_pCallbackUp;
	m_bDown = oCopy.m_bDown;
	m_bSave = oCopy.m_bSave;
	m_bDeleteCallbacks = oCopy.m_bDeleteCallbacks;
}

ShortKeyManager::ShortKey::~ShortKey()
{
	if (m_bDeleteCallbacks)
	{
		if (NULL != m_pCallback)
		{
			delete m_pCallback;
		}
		if (NULL != m_pCallbackUp)
		{
			delete m_pCallbackUp;
		}
	}
}

void ShortKeyManager::ShortKey::Call()
{
	if (NULL != m_pCallback)
	{
		m_pCallback->Call();
	}
}

void ShortKeyManager::ShortKey::CallUp()
{
	if (NULL != m_pCallbackUp)
	{
		m_pCallbackUp->Call();
	}
}

void ShortKeyManager::ShortKey::RefreshKeys()
{
	int iKeyCount = 0;
	if (m_eModifierKeys[0] != EasyWindow::KEY_NONE)
		++iKeyCount;
	if (m_eModifierKeys[1] != EasyWindow::KEY_NONE)
		++iKeyCount;
	if (m_eModifierKeys[2] != EasyWindow::KEY_NONE)
		++iKeyCount;
	if (m_ePrimaryKey != EasyWindow::KEY_NONE)
		++iKeyCount;
	m_iKeyCount = iKeyCount;
	ShortKeyManager::ShortkeyToString(&m_sShortKey, m_ePrimaryKey, m_eModifierKeys[0], m_eModifierKeys[1], m_eModifierKeys[2]);
}

bool ShortKeyManager::ShortKey::IsDefault() const
{
	return m_eDefaultPrimaryKey == m_ePrimaryKey
		&& m_eDefaultModifierKeys[0] == m_eModifierKeys[0]
		&& m_eDefaultModifierKeys[1] == m_eModifierKeys[1]
		&& m_eDefaultModifierKeys[2] == m_eModifierKeys[2];
}

void ShortKeyManager::ShortKey::Reset()
{
	m_ePrimaryKey = m_eDefaultPrimaryKey;
	m_eModifierKeys[0] = m_eDefaultModifierKeys[0];
	m_eModifierKeys[1] = m_eDefaultModifierKeys[1];
	m_eModifierKeys[2] = m_eDefaultModifierKeys[2];
	RefreshKeys();
}

void ShortKeyManager::ShortKey::SetShortkey(EasyWindow::EKey ePrimary, EasyWindow::EKey eModifier1, EasyWindow::EKey eModifier2, EasyWindow::EKey eModifier3)
{
	m_ePrimaryKey = ePrimary;
	m_eModifierKeys[0] = eModifier1;
	m_eModifierKeys[1] = eModifier2;
	m_eModifierKeys[2] = eModifier3;
	RefreshKeys();
}

//////////////////////////////////////////////////////////////////////////

ShortKeyManager::ShortKeyManager()
{
	s_pInstance = this;
	m_bEnable = true;

	for (int i = 0; i < 512; ++i)
		m_pKeysUsed[i] = false;
}

ShortKeyManager::~ShortKeyManager()
{
	for (ShortKeyManager::ShortKeys::iterator it = m_mShortKeys.begin(), itEnd = m_mShortKeys.end(); it != itEnd; ++it )
	{
		delete it->second;
	}
	s_pInstance = NULL;
}

bool ShortKeyManager::Manage(bool bCaptureKeyboard)
{
	if (m_bEnable)
	{
		if (ImWindow::ImwWindowManager::GetInstance()->HasWantCaptureKeyboard())
			return true;
		ImWindow::ImwPlatformWindow* pFocusedWindow = ImWindow::ImwWindowManager::GetInstance()->GetFocusedPlatformWindow();
		if (pFocusedWindow == NULL)
			return true;

		ImGuiIO& oIO = pFocusedWindow->GetContext()->IO;

		bool bFound = false;
		for (int iKeyCount = 4; iKeyCount > 0 && !bFound; --iKeyCount)
		{
			for (ShortKeys::iterator it = m_mShortKeys.begin(); it != m_mShortKeys.end(); ++it)
			{
				if (it->second->m_iKeyCount == iKeyCount)
				{
					if (IsDown(*it->second, oIO))
					{
						if (it->second->m_bDown == false)
						{
							if (!bCaptureKeyboard)
							{
								it->second->Call();
								it->second->m_bDownActive = true;
							}
							it->second->m_bDown = true;
						}
						bFound = true;
						break;
					}
					else if (it->second->m_bDown == true)
					{
						if (!bCaptureKeyboard)
							it->second->CallUp();
						it->second->m_bDown = false;
						it->second->m_bDownActive = false;
					}
				}
			}
		}
	}

	for (int i = 0; i < 512; ++i)
		m_pKeysUsed[i] = false;

	return true;
}

const ShortKeyManager::ShortKey* ShortKeyManager::RegisterShortKey(const char* pName, EasyWindow::EKey eModifier1, EasyWindow::EKey eModifier2, EasyWindow::EKey eModifier3, EasyWindow::EKey ePrimary, EasyWindow::Caller<void>* pCallback, bool bSave, bool bDeleteCallbacks)
{
	return RegisterShortKeyUp(pName, eModifier1, eModifier2, eModifier3, ePrimary, pCallback, NULL, bSave, bDeleteCallbacks);
}

const ShortKeyManager::ShortKey* ShortKeyManager::RegisterShortKeyUp(const char* pName, EasyWindow::EKey eModifier1, EasyWindow::EKey eModifier2, EasyWindow::EKey eModifier3, EasyWindow::EKey ePrimary, EasyWindow::Caller<void>* pCallback, EasyWindow::Caller<void>* pCallbackUp, bool bSave, bool bDeleteCallbacks)
{
	IM_ASSERT(eModifier1 == EasyWindow::KEY_NONE || EasyWindow::IsModifierKey(eModifier1));
	IM_ASSERT(eModifier2 == EasyWindow::KEY_NONE || EasyWindow::IsModifierKey(eModifier2));
	IM_ASSERT(eModifier3 == EasyWindow::KEY_NONE || EasyWindow::IsModifierKey(eModifier3));
	IM_ASSERT(ePrimary != EasyWindow::KEY_NONE && EasyWindow::IsModifierKey(ePrimary) == false);
	IM_ASSERT(pName != NULL);
	if (pName != NULL)
	{
		IM_ASSERT(m_mShortKeys.find(pName) == m_mShortKeys.end()); //ShortKey name already used
		if (m_mShortKeys.find(pName) == m_mShortKeys.end())
		{
			//TODO: save
			ShortKey* pShortkey = new ShortKey(
				ePrimary, eModifier1, eModifier2, eModifier3,
				ePrimary, eModifier1, eModifier2, eModifier3,
				pCallback, pCallbackUp, bSave, bDeleteCallbacks);
			 m_mShortKeys[pName] = pShortkey;
			return pShortkey;
		}
	}

	return NULL;
}

bool ShortKeyManager::UnregisterShortKey(const ShortKey* pShortkey)
{
	IM_ASSERT(pShortkey != NULL);
	if (pShortkey != NULL)
	{
		for (ShortKeyManager::ShortKeys::iterator it = m_mShortKeys.begin(), itEnd = m_mShortKeys.end(); it != itEnd; ++it)
		{
			if (it->second == pShortkey)
			{
				delete it->second;
				m_mShortKeys.erase(it);
				return true;
			}
		}
	}
	return false;
}

bool ShortKeyManager::UnregisterShortKey(const char* pName)
{
	IM_ASSERT(pName != NULL);
	if (pName != NULL)
	{
		ShortKeys::iterator it = m_mShortKeys.find(pName);
		IM_ASSERT(it != m_mShortKeys.end()); //ShortKey name not registered
		if (it != m_mShortKeys.end())
		{
			delete it->second;
			m_mShortKeys.erase(it);
			return true;
		}
	}
	return false;
}

bool* ShortKeyManager::GetDownShortKeyBool(const char* pName)
{

	IM_ASSERT(pName != NULL);
	if (pName != NULL)
	{
		ShortKeys::iterator it = m_mShortKeys.find(pName);
		IM_ASSERT(it != m_mShortKeys.end()); //ShortKey name not registered
		if (it != m_mShortKeys.end())
		{
			return &(it->second->m_bDownActive);
		}
	}
	return NULL;
}

void ShortKeyManager::SetKeyUsed(const EasyWindow::EKey iKeyUsed)
{
	m_pKeysUsed[iKeyUsed] = true;
}

void ShortKeyManager::SetShortkeysEnable(bool bEnable)
{
	m_bEnable = bEnable;
}

const ShortKeyManager::ShortKeys& ShortKeyManager::GetShortKeys() const
{
	return m_mShortKeys;
}

bool ShortKeyManager::IsDown(const ShortKey& oKey, ImGuiIO& oIO)
{
	bool bPrimaryDown = false;
	bool bPrimaryValid = false;

	if (oKey.m_ePrimaryKey != EasyWindow::KEY_NONE)
	{
		bPrimaryValid = true;
		if (oIO.KeysDown[oKey.m_ePrimaryKey] != 0 && !m_pKeysUsed[oKey.m_ePrimaryKey])
			bPrimaryDown = true;
	}

	if (!bPrimaryDown && bPrimaryValid)
		return false;

	for (int i = 0; i < 3; ++i)
	{
		bool bSpecialDown = false;
		bool bSpecialValid = false;
		EasyWindow::EKey iSpecialKey = oKey.m_eModifierKeys[i];
		if (iSpecialKey != EasyWindow::KEY_NONE)
		{
			bSpecialValid = true;
			if (oIO.KeysDown[iSpecialKey] != 0 && !m_pKeysUsed[iSpecialKey])
				bSpecialDown = true;
		}

		if (!bSpecialDown && bSpecialValid)
			return false;
	}

	return true;
}

bool ShortKeyManager::IsValidShortKey(const std::string& sShortKey)
{
	std::string::const_iterator it = sShortKey.begin();
	std::string::const_iterator itEnd = sShortKey.end();
	do
	{
		std::string::const_iterator itPlus = std::find(it, itEnd, '+');
		std::string sKey(it, itPlus);
		if (EasyWindow::StringToKey(sKey.c_str()) == EasyWindow::KEY_NONE)
			return false;
		it = itPlus;
		if (it != itEnd)
			++it;
	} while (it != itEnd);

	return sShortKey.empty() == false;
}

bool ShortKeyManager::ExtractShortKey(const std::string& sShortKey,
	EasyWindow::EKey& ePrimary,
	EasyWindow::EKey& eModifier1,
	EasyWindow::EKey& eModifier2,
	EasyWindow::EKey& eModifier3)
{
	ePrimary = EasyWindow::KEY_NONE;
	eModifier1 = EasyWindow::KEY_NONE;
	eModifier2 = EasyWindow::KEY_NONE;
	eModifier3 = EasyWindow::KEY_NONE;

	int iCurrentSpecial = 0;

	std::string::const_iterator it = sShortKey.begin();
	std::string::const_iterator itEnd = sShortKey.end();
	do
	{
		std::string::const_iterator itPlus = std::find(it, itEnd, '+');
		std::string sKey(it, itPlus);

		EasyWindow::EKey iKey1 = EasyWindow::StringToKey(sKey.c_str());

		if (iKey1 == EasyWindow::KEY_NONE)
		{
			return false;
		}

		bool bIsModifier = EasyWindow::IsModifierKey(iKey1);

		if (bIsModifier)
		{
			switch (iCurrentSpecial)
			{
			case 0:
				eModifier1 = iKey1;
				break;
			case 1:
				eModifier2 = iKey1;
				break;
			case 2:
				eModifier3 = iKey1;
				break;
			}
			++iCurrentSpecial;
		}
		else
		{
			ePrimary = iKey1;
		}

		it = itPlus;
		if (it != itEnd)
			++it;
	} while (it != itEnd);

	return sShortKey.empty() == false;
}

bool ShortKeyManager::ShortkeyToString(std::string* pOutString, EasyWindow::EKey ePrimary, EasyWindow::EKey& eSpecial1, EasyWindow::EKey& eSpecial2, EasyWindow::EKey& eSpecial3)
{
	if (pOutString != NULL)
	{
		pOutString->clear();

		if (eSpecial1 != EasyWindow::KEY_NONE)
		{
			if (EasyWindow::IsModifierKey(eSpecial1) == false)
				return false;

			if (!pOutString->empty())
				*pOutString += "+";

			*pOutString += EasyWindow::KeyToString(eSpecial1);
		}

		if (eSpecial2 != EasyWindow::KEY_NONE)
		{
			if (EasyWindow::IsModifierKey(eSpecial2) == false)
				return false;

			if (!pOutString->empty())
				*pOutString += "+";

			*pOutString += EasyWindow::KeyToString(eSpecial2);
		}

		if (eSpecial3 != EasyWindow::KEY_NONE)
		{
			if (EasyWindow::IsModifierKey(eSpecial3) == false)
				return false;

			if (!pOutString->empty())
				*pOutString += "+";

			*pOutString += EasyWindow::KeyToString(eSpecial3);
		}

		if (ePrimary != EasyWindow::KEY_NONE)
		{
			if (EasyWindow::IsModifierKey(ePrimary) == true)
				return false;

			if (!pOutString->empty())
				*pOutString += "+";

			*pOutString += EasyWindow::KeyToString(ePrimary);
		}

		return true;
	}

	return false;
}
