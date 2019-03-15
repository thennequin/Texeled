
#include "Program.h"

#include "ShortKeyManager.h"
#include "Menus.h"
#include "StatusBars.h"

#include <D3D11.h>

#include "PlatformUtils.h"

#include "Core/StringBuilder.h"
#include "Core/StringUtils.h"

#include "Graphics/TextureLoader.h"
#include "Graphics/TextureWriter.h"

#include "Graphics/TextureLoaders/TextureLoaderSTBI.h"
#include "Graphics/TextureLoaders/TextureLoaderDDS.h"

#include "Graphics/TextureWriters/TextureWriterDDS.h"

#include "Resources/Fonts/Consolas_ttf.h"
#include "Resources/Fonts/Consolas_Bold_ttf.h"

#include "Windows/WorkAreaWindow.h"

#include "GraphicResources/Texture2D.h"

const char* ProgramModeStrings[ProgramMode::_COUNT] =
{
	"Viewer",
	"Edition Color",
	"Edition Normal",
};

Program* Program::s_pInstance = NULL;

Program::Program(int iArgCount, char** pArgs)
	: m_bRun(true)
	, m_eMode(ProgramMode::VIEWER)
	, m_oImWindowMgrDX11(true)
	, m_pTexture2D(NULL)
{
	s_pInstance = this;

	Graphics::TextureLoader::RegisterLoaderSTBI();
	Graphics::TextureLoader::RegisterLoaderDDS();

	Graphics::TextureWriter::RegisterWriterDDS();

	ImFontConfig oConfig;
	oConfig.MergeMode = false;
	oConfig.FontDataOwnedByAtlas = false;
	//ImGui::GetIO().Fonts->AddFontDefault();
	m_oFonts.pFontConsolasBold = ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)Resources::Fonts::Consolas_Bold_ttf::Data, Resources::Fonts::Consolas_Bold_ttf::Size, 16.f, &oConfig, NULL);
	m_oFonts.pFontConsolas = ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)Resources::Fonts::Consolas_ttf::Data, Resources::Fonts::Consolas_ttf::Size, 16.f, &oConfig, NULL);
	oConfig.MergeMode = true;

	ImGui::GetIO().Fonts->Build();

	m_oImWindowMgrDX11.Init();
	m_oImWindowMgrDX11.SetMainTitle("Texeled");

	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.204f, 0.204f, 0.204f, 1.f);
	style.Colors[ImGuiCol_MenuBarBg] = style.Colors[ImGuiCol_WindowBg];
	style.Colors[ImGuiCol_Header] = ImVec4(0.16f, 0.34f, 0.34f, 1.f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.34f, 0.34f, 1.f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.42, 0.42f, 1.f);

	m_pShortKeyManager = new ShortKeyManager();
	m_oShortkeys.pClose = m_pShortKeyManager->RegisterShortKey("Close", "ALT+F4", new EasyWindow::InstanceCaller<Program, void>(this, &Program::AskExit), false);
	m_oShortkeys.pOpen = m_pShortKeyManager->RegisterShortKey("Open", "CTRL+O", new EasyWindow::InstanceCaller<Program, void>(this, &Program::Open), false);
	m_oShortkeys.pSave = m_pShortKeyManager->RegisterShortKey("Save", "CTRL+S", new EasyWindow::InstanceCaller<Program, void>(this, &Program::Save), false);
	m_oShortkeys.pSaveAs = m_pShortKeyManager->RegisterShortKey("Save As", "CTRL+SHIFT+S", new EasyWindow::InstanceCaller<Program, void>(this, &Program::SaveAs), false);

	m_pMenus = new Menus();
	new StatusBars();

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

void Program::UpdateTexture2DRes()
{
	ImwSafeDelete(m_pTexture2D);
	if (m_oTexture.IsValid())
	{
		CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&m_oTexture, &m_pTexture2D));
	}
}

bool Program::LoadFile(const char* pFile)
{
	if (Graphics::LoadFromFile(&m_oTexture, pFile) == ErrorCode::Ok)
	{
		UpdateTexture2DRes();
		return true;
	}
	return false;
}

void Program::Open()
{
	char pBuffer[1024] = { 0 };
	int iIndex;

	Core::StringBuilder oExts;
	static char* s_pExts = NULL;

	const Graphics::TextureLoaderInfo* pLoaders;
	int iLoaderCount;
	GetTextureLoaders(&pLoaders, &iLoaderCount);

	oExts += "All supported formats";
	oExts += (char)0;
	for (int iIndex = 0; iIndex < iLoaderCount; ++iIndex)
	{
		if (iIndex > 0)
			oExts += ';';
		oExts += pLoaders[iIndex].pExts;
	}
	oExts += (char)0;

	for (int iIndex = 0; iIndex < iLoaderCount; ++iIndex)
	{
		oExts += pLoaders[iIndex].pName;
		oExts += " (";
		oExts += pLoaders[iIndex].pExts;
		oExts += ')';
		oExts += (char)0;
		oExts += pLoaders[iIndex].pExts;
		oExts += (char)0;
	}

	char* pExts = oExts.Export();
	if (PlatformUtils::OpenFileDialog("Open file", pExts, pBuffer, sizeof(pBuffer), &iIndex))
	{
		//TODO testing iindex
		LoadFile(pBuffer);
	}
	free(pExts);
}

void Program::Save()
{
}

void Program::SaveAs()
{
	char pBuffer[1024] = { 0 };
	int iIndex;

	Core::StringBuilder oExts;
	static char* s_pExts = NULL;

	const Graphics::TextureWriterInfo* pWriters;
	int iLoaderCount;
	Graphics::GetTextureWriters(&pWriters, &iLoaderCount);

	for (int iIndex = 0; iIndex < iLoaderCount; ++iIndex)
	{
		oExts += pWriters[iIndex].pName;
		oExts += " (";
		oExts += pWriters[iIndex].pExt;
		oExts += ')';
		oExts += (char)0;
		oExts += pWriters[iIndex].pExt;
		oExts += (char)0;
	}

	char* pExts = oExts.Export();
	if (PlatformUtils::SaveFileDialog("Save as", pExts, pBuffer, sizeof(pBuffer), &iIndex))
	{
		if (Core::StringUtils::Wildcard(pBuffer, pExts) == false)
		{
			//Append extension
		}
		//TODO testing iindex
		CORE_VERIFY_OK(Graphics::SaveToFile(&m_oTexture, NULL, pBuffer));
		LoadFile(pBuffer);
	}
	free(pExts);
}