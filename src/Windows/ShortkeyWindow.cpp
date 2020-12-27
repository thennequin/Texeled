#include "Windows/ShortKeyWindow.h"

#include "ImWindow/ImwWindowManager.h"

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