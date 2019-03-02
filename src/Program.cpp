
#include "Program.h"

#include "ShortKeyManager.h"

Program* Program::s_pInstance = NULL;

Program::Program()
	: m_bRun(true)
	, m_oMgr(true)
{
	s_pInstance = this;

	m_oMgr.Init();
	m_oMgr.SetMainTitle("Program");

	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.095f, 0.095f, 0.095f, 1.f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.204f, 0.204f, 0.204f, 1.f);
	style.Colors[ImGuiCol_MenuBarBg] = style.Colors[ImGuiCol_WindowBg];

	m_pShortKeyManager = new ShortKeyManager();
}

Program::~Program()
{
	ImwSafeDelete(m_pShortKeyManager);
}

Program* Program::CreateInstance()
{
	if (NULL == s_pInstance)
		new Program();
	return s_pInstance;
}

void Program::DestroyInstance()
{
	if (NULL != s_pInstance)
	{
		delete s_pInstance;
		s_pInstance = NULL;

		ImGui::Shutdown();
	}
}

bool Program::Run()
{
	m_pShortKeyManager->Manage(false);
	return m_bRun && m_oMgr.Run(false) && m_oMgr.Run(true);
}

void Program::AskExit()
{
	m_bRun = false;
}
