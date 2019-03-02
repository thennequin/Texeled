
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
	, bool bSave)
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

const ShortKeyManager::ShortKey* ShortKeyManager::RegisterShortKey(const char* pName, const char* pDefaultShortKey, EasyWindow::Caller<void>* pCallback, bool bSave)
{
	return RegisterShortKeyUp(pName, pDefaultShortKey, pCallback, NULL, bSave);
}

const ShortKeyManager::ShortKey* ShortKeyManager::RegisterShortKeyUp(const char* pName, const char* pDefaultShortKey, EasyWindow::Caller<void>* pCallback, EasyWindow::Caller<void>* pCallbackUp, bool bSave)
{
	IM_ASSERT(IsValidShortKey(pDefaultShortKey));
	IM_ASSERT(pName != NULL);
	if (pName != NULL)
	{
		IM_ASSERT(m_mShortKeys.find(pName) == m_mShortKeys.end()); //ShortKey name already used
		if (m_mShortKeys.find(pName) == m_mShortKeys.end())
		{
			EasyWindow::EKey eDefaultPrimary;
			EasyWindow::EKey eDefaultModifier[3];
			ExtractShortKey(pDefaultShortKey, eDefaultPrimary, eDefaultModifier[0], eDefaultModifier[1], eDefaultModifier[2]);
			
			EasyWindow::EKey ePrimary = eDefaultPrimary;
			EasyWindow::EKey eModifier[3] = { eDefaultModifier[0], eDefaultModifier[1], eDefaultModifier[2] };
			//TODO: save
			ShortKey* pShortkey = new ShortKey(
				eDefaultPrimary, eDefaultModifier[0], eDefaultModifier[1], eDefaultModifier[1],
				ePrimary, eModifier[0], eModifier[1], eModifier[1],
				pCallback, pCallbackUp, bSave);
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

		iKey1 = EasyWindow::StringToKey(sKey.c_str());
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

//////////////////////////////////////////////////////////////////////////
ShortKeyWindow::ShortKeyWindow()
{
	SetTitle("Shortkeys");
	m_iCurrentRecord = -1;
	ResetKeys();
}

ShortKeyWindow::~ShortKeyWindow()
{
	if (m_iCurrentRecord != -1 && NULL != ShortKeyManager::GetInstance())
		ShortKeyManager::GetInstance()->SetShortkeysEnable(true);
}

int ShortKeyWindow::InputTextCallback(ImGuiTextEditCallbackData* pData)
{
	pData->EventChar = 0;
	return 1;
}

void ShortKeyWindow::OnGui()
{
	ShortKeyManager* pInstance = ShortKeyManager::GetInstance();
	if (NULL != pInstance)
	{
		ImWindow::ImwPlatformWindow* pActivePlatformWindow = ImWindow::ImwWindowManager::GetInstance()->GetCurrentPlatformWindow();
		ImGuiIO& oIO = pActivePlatformWindow->GetContext()->IO;

		ShortKeyManager::ShortKeys::iterator it = pInstance->m_mShortKeys.begin(),
			itEnd = pInstance->m_mShortKeys.end();

		if (m_iCurrentRecord != -1)
		{
			int iCurrentKey = 0;
			EasyWindow::EKey iPressedKeys[5] = { EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE };

			for (int iKey = 0; iKey < 512; ++iKey)
			{
				if (oIO.KeysDown[iKey] && iCurrentKey < 5)
				{
					iPressedKeys[iCurrentKey++] = (EasyWindow::EKey)iKey;
				}
			}

			int iCurrentSpecialKey = 0;
			EasyWindow::EKey iPressedSpecialKeys[3] = { EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE };
			EasyWindow::EKey iPressedPrimaryKey = EasyWindow::KEY_NONE;

			for (int iKey = 0; iKey < 5; ++iKey)
			{
				if (iPressedKeys[iKey] != EasyWindow::KEY_NONE)
				{
					if (EasyWindow::IsModifierKey(iPressedKeys[iKey]))
					{
						EasyWindow::EKey eOrignalKey = EasyWindow::GetOriginalKey(iPressedKeys[iKey]);
						if (eOrignalKey == EasyWindow::KEY_NONE && iCurrentSpecialKey < 3)
						{
							iPressedSpecialKeys[iCurrentSpecialKey++] = iPressedKeys[iKey];
						}
					}
					else
					{
						iPressedPrimaryKey = iPressedKeys[iKey];
					}
				}
			}

			bool bHasSpecialPressed = false;
			bool bIsNew = false;
			for (int iTempKey = 0; iTempKey < 3; ++iTempKey)
			{
				if (iPressedSpecialKeys[iTempKey] != EasyWindow::KEY_NONE)
				{
					bHasSpecialPressed = true;
					bool bFound = false;
					for (int iKey = 0; iKey < 3; ++iKey)
					{
						if (m_iPressedModifierKeys[iKey] == iPressedSpecialKeys[iTempKey])
						{
							bFound = true;
							break;
						}
					}
					if (!bFound)
					{
						bIsNew = true;
						break;
					}
				}
			}

			if ((!m_bHasSpecialPressed && bHasSpecialPressed)
				|| (iPressedPrimaryKey != EasyWindow::KEY_NONE && m_iPressedPrimaryKey != iPressedPrimaryKey)
				|| bIsNew
				)
			{
				for (int iKey = 0; iKey < 3; ++iKey)
				{
					//Suport Left/Right modifiers key with double pressed
					/*if (m_iPressedModifierKeys[iKey] != EasyWindow::KEY_NONE 
						&& ( m_iPressedModifierKeys[iKey] == iPressedSpecialKeys[iKey] || EasyWindow::GetOriginalKey(m_iPressedModifierKeys[iKey]) == iPressedSpecialKeys[iKey]))
					{
						EasyWindow::EKey eExtKey1, eExtKey2;
						if (EasyWindow::GetExtendedKeys(iPressedSpecialKeys[iKey], &eExtKey1, &eExtKey2))
						{
							if (oIO.KeysDown[eExtKey1] && oIO.KeysDownDuration[eExtKey1] == 0.f)
							{
								m_iPressedModifierKeys[iKey] = eExtKey1;
								m_bDoublePressedModifierKeys[iKey] = true;
							}
							else if (oIO.KeysDown[eExtKey2] && oIO.KeysDownDuration[eExtKey2] == 0.f)
							{
								m_iPressedModifierKeys[iKey] = eExtKey2;
								m_bDoublePressedModifierKeys[iKey] = true;
							}
						}
					}
					else*/
					{
						m_iPressedModifierKeys[iKey] = iPressedSpecialKeys[iKey];
					}
				}
				m_iPressedPrimaryKey = iPressedPrimaryKey;
			}

			m_bHasSpecialPressed = bHasSpecialPressed;
		}

		ImGui::Columns(2);
		ImGui::Text("Name");
		ImGui::NextColumn();
		ImGui::Text("Keys");
		ImGui::Separator();

		int iCurrentShortKey = 0;
		for (; it != itEnd; ++it, ++iCurrentShortKey)
		{
			ImGui::NextColumn();
			ImGui::Text(it->first.c_str());
			ImGui::NextColumn();
			//ImGui::Text(it->second.m_sShortKey.c_str());
			std::string sInputName = "##Input" + it->first;
			const std::string& sKeys = it->second->m_sShortKey;
			//ImGui::Text(sKey.c_str());

			ImGui::InputText(sInputName.c_str(), (char*)sKeys.c_str(), sKeys.length(), ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackAlways, &ShortKeyWindow::InputTextCallback);
			if (ImGui::IsItemActive())
			{
				if (m_iCurrentRecord != iCurrentShortKey)
				{
					ImwVerify(ShortKeyManager::ExtractShortKey(sKeys,
						m_iPressedPrimaryKey,
						m_iPressedModifierKeys[0],
						m_iPressedModifierKeys[1],
						m_iPressedModifierKeys[2])
					);

					m_bHasSpecialPressed = false;
					m_iCurrentRecord = iCurrentShortKey;
					pInstance->SetShortkeysEnable(false);
				}

				std::string sInputKeys;
				ShortKeyManager::ShortkeyToString(&sInputKeys, m_iPressedPrimaryKey, m_iPressedModifierKeys[0], m_iPressedModifierKeys[1], m_iPressedModifierKeys[2]);

				it->second->SetShortkey(m_iPressedPrimaryKey, m_iPressedModifierKeys[0], m_iPressedModifierKeys[1], m_iPressedModifierKeys[2]);
			}
			else
			{
				if (m_iCurrentRecord == iCurrentShortKey)
				{
					ResetKeys();
					m_iCurrentRecord = -1;
					pInstance->SetShortkeysEnable(true);
				}
			}

			if (it->second->IsDefault() == false)
			{
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
				{
					it->second->Reset();
				}
			}
		}

		ImGui::Columns(1);
	}
}

void ShortKeyWindow::ResetKeys()
{
	m_bHasSpecialPressed = false;
	m_iPressedModifierKeys[0] = EasyWindow::KEY_NONE;
	m_iPressedModifierKeys[1] = EasyWindow::KEY_NONE;
	m_iPressedModifierKeys[2] = EasyWindow::KEY_NONE;
	m_iPressedPrimaryKey = EasyWindow::KEY_NONE;

	m_bDoublePressedModifierKeys[0] = false;
	m_bDoublePressedModifierKeys[1] = false;
	m_bDoublePressedModifierKeys[2] = false;
}