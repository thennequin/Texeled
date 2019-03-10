
#include "Program.h"

#include "ShortKeyManager.h"

#include <D3D11.h>

#include "Graphics/TextureLoader.h"

#include "Graphics/TextureLoaders/TextureLoaderSTBI.h"
#include "Graphics/TextureLoaders/TextureLoaderDDS.h"

#include "Resources/Fonts/Consolas_ttf.h"

#include "Windows/WorkAreaWindow.h"

#include "GraphicResources/Texture2D.h"

Program* Program::s_pInstance = NULL;

Program::Program(int iArgCount, char** pArgs)
	: m_bRun(true)
	, m_oImWindowMgrDX11(true)
	, m_pTexture2D(NULL)
{
	s_pInstance = this;

	Graphics::TextureLoader::RegisterLoaderSTBI();
	Graphics::TextureLoader::RegisterLoaderDDS();

	ImFontConfig oConfig;
	oConfig.MergeMode = false;
	oConfig.FontDataOwnedByAtlas = false;
	//ImGui::GetIO().Fonts->AddFontDefault();
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)Resources::Fonts::Consolas_ttf::Data, Resources::Fonts::Consolas_ttf::Size, 16.f, &oConfig, NULL);
	oConfig.MergeMode = true;

	ImGui::GetIO().Fonts->Build();

	m_oImWindowMgrDX11.Init();
	m_oImWindowMgrDX11.SetMainTitle("Texeled");

	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.204f, 0.204f, 0.204f, 1.f);
	style.Colors[ImGuiCol_MenuBarBg] = style.Colors[ImGuiCol_WindowBg];
	style.Colors[ImGuiCol_Header] = ImVec4(0.16f, 0.34, 0.34f, 1.f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.34, 0.34f, 1.f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.42, 0.42f, 1.f);

	m_pShortKeyManager = new ShortKeyManager();
	m_pShortKeyManager->RegisterShortKey("Close", "ALT+F4", new EasyWindow::InstanceCaller<Program, void>(this, &Program::AskExit), false);

	m_pWorkAreaWindow = new Windows::WorkAreaWindow();

	for (int iArg = 1; iArg < iArgCount; ++iArg)
	{
		LoadFile(pArgs[iArg]);
	}
}

Program::~Program()
{
	ImwSafeDelete(m_pTexture2D);
	ImwSafeDelete(m_pShortKeyManager);
}

Program* Program::CreateInstance(int iArgCount, char** pArgs)
{
	if (NULL == s_pInstance)
		new Program(iArgCount, pArgs);
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

IDXGIFactory* Program::GetDXGIFactory() const
{
	return m_oImWindowMgrDX11.GetDXGIFactory();
}

ID3D11Device* Program::GetDX11Device() const
{
	return m_oImWindowMgrDX11.GetDX11Device();
}

ID3D11DeviceContext* Program::GetDX11DeviceContext() const
{
	return m_oImWindowMgrDX11.GetDX11DeviceContext();
}

bool Program::Run()
{
	m_pShortKeyManager->Manage(false);
	return m_bRun && m_oImWindowMgrDX11.Run(false) && m_oImWindowMgrDX11.Run(true);
}

void Program::AskExit()
{
	m_bRun = false;
}

bool Program::LoadFile(const char* pFile)
{
	if (Graphics::LoadFromFile(&m_oTexture, pFile) == ErrorCode::Ok)
	{
		ImwSafeDelete(m_pTexture2D);
		if (GraphicResources::Texture2D::CreateFromTexture(&m_oTexture, &m_pTexture2D) == ErrorCode::Ok)
		{
			return true;
		}
	}
	return false;
}
