
#include "Program.h"

#include "ShortKeyManager.h"
#include "Menus.h"
#include "Toolbar.h"

#include <D3D11.h>

#include "PlatformUtils.h"

#include "Core/StringBuilder.h"
#include "Core/StringUtils.h"

#include "IO/FileSystem.h"
#include "IO/Clipboard.h"

#include "Graphics/TextureUtils.h"

#include "Texture/TextureLoader.h"
#include "Texture/TextureWriter.h"

#include "Texture/TextureLoaders/TextureLoaderSTBI.h"
#include "Texture/TextureLoaders/TextureLoaderPNG.h"
#include "Texture/TextureLoaders/TextureLoaderDDS.h"
#include "Texture/TextureLoaders/TextureLoaderEXR.h"
#include "Texture/TextureLoaders/TextureLoaderKTX.h"

#include "Texture/TextureWriters/TextureWriterDDS.h"
#include "Texture/TextureWriters/TextureWriterPNG.h"
#include "Texture/TextureWriters/TextureWriterTGA.h"
#include "Texture/TextureWriters/TextureWriterEXR.h"

#include "Resources/Fonts/Consolas_ttf.h"
#include "Resources/Fonts/Consolas_Bold_ttf.h"

#include "Windows/WorkAreaWindow.h"

#include "GraphicResources/Texture2D.h"

const char* ProgramModeEnumStrings[ProgramModeEnum::_COUNT] =
{
	"Viewer",
	"Edit color",
	"Edit normal",
};

Program* Program::s_pInstance = NULL;

Program::Program(int iArgCount, char** pArgs)
	: m_bRun(true)
	, m_eMode(ProgramModeEnum::VIEWER)
	, m_oImWindowMgr(true)
	, m_pTexture2D(NULL)
{
	s_pInstance = this;
	Core::Logger::RegisterLoggerOutputer(this);

	// Checking PixelFormats
	for (int i = 0; i < Graphics::PixelFormatEnum::_COUNT; ++i)
	{
		CORE_ASSERT(Graphics::PixelFormatEnumInfos[i].eFormat == i);
	}

	Texture::TextureLoader::RegisterLoaderPNG();
	Texture::TextureLoader::RegisterLoaderSTBI();
	Texture::TextureLoader::RegisterLoaderDDS();
	Texture::TextureLoader::RegisterLoaderEXR();
	Texture::TextureLoader::RegisterLoaderKTX();

	Texture::TextureWriter::RegisterWriterDDS();
	Texture::TextureWriter::RegisterWriterPNG();
	Texture::TextureWriter::RegisterWriterTGA();
	Texture::TextureWriter::RegisterWriterEXR();

	ImFontConfig oConfig;
	oConfig.MergeMode = false;
	oConfig.FontDataOwnedByAtlas = false;
	//ImGui::GetIO().Fonts->AddFontDefault();
	m_oFonts.pFontConsolasBold = ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)Resources::Fonts::Consolas_Bold_ttf::Data, Resources::Fonts::Consolas_Bold_ttf::Size, 16.f, &oConfig, NULL);
	m_oFonts.pFontConsolas = ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)Resources::Fonts::Consolas_ttf::Data, Resources::Fonts::Consolas_ttf::Size, 16.f, &oConfig, NULL);
	oConfig.MergeMode = true;

	m_oDisplayOptions.bShowPixelGrid = false;
	m_oDisplayOptions.bTiling = false;
	m_oDisplayOptions.eShowChannels = ChannelFlag::RED | ChannelFlag::GREEN | ChannelFlag::BLUE | ChannelFlag::ALPHA;
	m_oDisplayOptions.iMip = 0;
	m_oDisplayOptions.iSlice = 0;
	m_oDisplayOptions.fRange[0] = 0.f;
	m_oDisplayOptions.fRange[1] = 1.f;
	m_oDisplayOptions.fGamma = 1.f;

	ImGui::GetIO().Fonts->Build();

	m_oImWindowMgr.Init();
	m_oImWindowMgr.SetMainTitle("Texeled");

	ImWindow::ImwWindowManager::Config& oImwConfig = m_oImWindowMgr.GetConfig();
	oImwConfig.m_bShowTabShadows = false;
	oImwConfig.m_fTabOverlap = 3.f;
	oImwConfig.m_fTabSlopWidth = 0.f;
	oImwConfig.m_fTabSlopP1Ratio = 0.f;
	oImwConfig.m_fTabSlopP2Ratio = 0.f;
	oImwConfig.m_fTabSlopHRatio = 0.f;
	oImwConfig.m_fTabShadowDropSize = 7.5f;
	oImwConfig.m_fTabShadowSlopRatio = 1.f;
	oImwConfig.m_fTabShadowAlpha = 1.f;

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
	style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.48f, 0.47f, 0.47f, 0.91f);
	style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.56f, 0.55f, 0.55f, 0.62f);
	style.Colors[ImGuiCol_Button]                = ImVec4(0.50f, 0.50f, 0.50f, 0.63f);
	style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.67f, 0.67f, 0.68f, 0.63f);
	style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.26f, 0.26f, 0.26f, 0.63f);
	style.Colors[ImGuiCol_Header]                = ImVec4(0.54f, 0.54f, 0.54f, 0.58f);
	style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.64f, 0.65f, 0.65f, 0.50f);
	style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
	style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.12f, 0.12f, 0.12f, 0.85f);
	style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

	style.WindowRounding = 0.f;

	m_pShortKeyManager = new ShortKeyManager();
	m_oShortkeys.pClose = m_pShortKeyManager->RegisterShortKey("Close", EasyWindow::KEY_ALT, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_F4, new Callback::InstanceCaller<Program, void>(this, &Program::CloseCurrentWindow), false);
	m_oShortkeys.pClose = m_pShortKeyManager->RegisterShortKey("Escape", EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_ESC, new Callback::InstanceCaller<Program, void>( this, &Program::AskExit), false);
	m_oShortkeys.pOpen = m_pShortKeyManager->RegisterShortKey("Open", EasyWindow::KEY_CTRL, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_O, new Callback::InstanceCaller<Program, void>(this, &Program::Open), false);
	m_oShortkeys.pSave = m_pShortKeyManager->RegisterShortKey("Save", EasyWindow::KEY_CTRL, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_S, new Callback::InstanceCaller<Program, void>(this, &Program::Save), false);
	m_oShortkeys.pSaveAs = m_pShortKeyManager->RegisterShortKey("Save As", EasyWindow::KEY_CTRL, EasyWindow::KEY_SHIFT, EasyWindow::KEY_NONE, EasyWindow::KEY_S, new Callback::InstanceCaller<Program, void>(this, &Program::SaveAs), false);

	m_oShortkeys.pReloadFile = m_pShortKeyManager->RegisterShortKey("Reload file", EasyWindow::KEY_CTRL, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_R, new Callback::InstanceCaller<Program, void>(this, &Program::ReloadFile), false);
	m_oShortkeys.pOpenPreviousFile = m_pShortKeyManager->RegisterShortKey("Open previous file", EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_PAGEUP, new Callback::InstanceCaller<Program, void>(this, &Program::OpenPreviousFileCallback), false);
	m_oShortkeys.pOpenNextFile = m_pShortKeyManager->RegisterShortKey("Open next", EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_PAGEDOWN, new Callback::InstanceCaller<Program, void>(this, &Program::OpenNextFileCallback), false);

	m_oShortkeys.pCopyCurrentSlice = m_pShortKeyManager->RegisterShortKey("Copy current slice", EasyWindow::KEY_CTRL, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_C, new Callback::InstanceCaller<Program, void>(this, &Program::CopyCurrentSliceCallback), false);
	m_oShortkeys.pCopyComponent[0] = m_pShortKeyManager->RegisterShortKey("Copy first component", EasyWindow::KEY_CTRL, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_1, new Callback::InstanceCaller<Program, void>(this, &Program::CopyComponentCallback<0>), false);
	m_oShortkeys.pCopyComponent[1] = m_pShortKeyManager->RegisterShortKey("Copy second component", EasyWindow::KEY_CTRL, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_2, new Callback::InstanceCaller<Program, void>(this, &Program::CopyComponentCallback<1>), false);
	m_oShortkeys.pCopyComponent[2] = m_pShortKeyManager->RegisterShortKey("Copy third component", EasyWindow::KEY_CTRL, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_3, new Callback::InstanceCaller<Program, void>(this, &Program::CopyComponentCallback<2>), false);
	m_oShortkeys.pCopyComponent[3] = m_pShortKeyManager->RegisterShortKey("Copy fourth component", EasyWindow::KEY_CTRL, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_4, new Callback::InstanceCaller<Program, void>(this, &Program::CopyComponentCallback<3>), false);

	m_oShortkeys.pPasteToNewTexture = m_pShortKeyManager->RegisterShortKey("Paste to new texture", EasyWindow::KEY_CTRL, EasyWindow::KEY_SHIFT, EasyWindow::KEY_NONE, EasyWindow::KEY_V, new Callback::InstanceCaller<Program, void>(this, &Program::PasteToNewTextureCallback), false);
	m_oShortkeys.pPasteToCurrentSlice = m_pShortKeyManager->RegisterShortKey("Paste to current slice", EasyWindow::KEY_CTRL, EasyWindow::KEY_NONE, EasyWindow::KEY_NONE, EasyWindow::KEY_V, new Callback::InstanceCaller<Program, void>(this, &Program::PasteToCurrentSliceCallback), false);
	m_oShortkeys.pPasteToComponent[0] = m_pShortKeyManager->RegisterShortKey("Paste to first component", EasyWindow::KEY_CTRL, EasyWindow::KEY_SHIFT, EasyWindow::KEY_NONE, EasyWindow::KEY_1, new Callback::InstanceCaller<Program, void>(this, &Program::PasteToComponentCallback<0>), false);
	m_oShortkeys.pPasteToComponent[1] = m_pShortKeyManager->RegisterShortKey("Paste to second component", EasyWindow::KEY_CTRL, EasyWindow::KEY_SHIFT, EasyWindow::KEY_NONE, EasyWindow::KEY_2, new Callback::InstanceCaller<Program, void>(this, &Program::PasteToComponentCallback<1>), false);
	m_oShortkeys.pPasteToComponent[2] = m_pShortKeyManager->RegisterShortKey("Paste to third component", EasyWindow::KEY_CTRL, EasyWindow::KEY_SHIFT, EasyWindow::KEY_NONE, EasyWindow::KEY_3, new Callback::InstanceCaller<Program, void>(this, &Program::PasteToComponentCallback<2>), false);
	m_oShortkeys.pPasteToComponent[3] = m_pShortKeyManager->RegisterShortKey("Paste to fourth component", EasyWindow::KEY_CTRL, EasyWindow::KEY_SHIFT, EasyWindow::KEY_NONE, EasyWindow::KEY_4, new Callback::InstanceCaller<Program, void>(this, &Program::PasteToComponentCallback<3>), false);

	m_pMenus = new Menus();
	m_pStatusBars = new StatusBars();
	new Toolbar();

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
	ClearLogs();
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

void Program::Log(Core::Logger::Category eCategory, const char* pName, const char* pFormattedMessage)
{
	::Log oLog;
	oLog.eCategory = eCategory;
	oLog.pName = Core::StringUtils::StrDup(pName);
	oLog.pMessage = Core::StringUtils::StrDup(pFormattedMessage);

	oLog.iLines = 1;
	char* pCursor = oLog.pMessage;
	while ((pCursor = strchr(pCursor, '\n')) != NULL)
	{
		pCursor++;
		oLog.iLines++;
	}
	m_oLogs.push_back(oLog);
}

void Program::ClearLogs()
{
	for (size_t i = 0; i < m_oLogs.size(); ++i)
	{
		free(m_oLogs[i].pName);
		free(m_oLogs[i].pMessage);
	}

	m_oLogs.clear();
}

IDXGIFactory* Program::GetDXGIFactory() const
{
	return m_oImWindowMgr.GetDXGIFactory();
}

ID3D11Device* Program::GetDX11Device() const
{
	return m_oImWindowMgr.GetDX11Device();
}

ID3D11DeviceContext* Program::GetDX11DeviceContext() const
{
	return m_oImWindowMgr.GetDX11DeviceContext();
}

bool Program::Run()
{
	m_pShortKeyManager->Manage(false);
	m_oClock.Manage();

	if (m_oTexture.IsValid() && m_oDisplayOptions.iMip >= m_oTexture.GetMipCount())
	{
		m_oDisplayOptions.iMip = m_oTexture.GetMipCount() - 1;
	}

	if (m_oTexture.IsValid() && m_oDisplayOptions.iSlice >= m_oTexture.GetSliceCount())
	{
		m_oDisplayOptions.iSlice = m_oTexture.GetSliceCount() - 1;
	}

	return m_bRun && m_oImWindowMgr.Run(false) && m_oImWindowMgr.Run(true);
}

void Program::AskExit()
{
	if (m_pStatusBars->IsDisplayingLog())
	{
		m_pStatusBars->SkipLog();
	}
	else
	{
		m_bRun = false;
	}
}

void Program::CloseCurrentWindow()
{
	if (m_oImWindowMgr.GetFocusedPlatformWindow() != NULL)
	{
		m_oImWindowMgr.ClosePlatformWindow(m_oImWindowMgr.GetFocusedPlatformWindow());
	}
}

void Program::ClearTextureFilePath()
{
	m_sTexturePath.clear();
}

void Program::UpdateTexture2DRes()
{
	ImwSafeDelete(m_pTexture2D);
	if (m_oTexture.IsValid())
	{
		CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&m_oTexture, &m_pTexture2D));
	}
}

void Program::ClearTexture()
{
	CORE_VERIFY_OK(m_oTexture.Destroy());
	m_sTexturePath = "";
	ImwSafeDelete(m_pTexture2D);
}

bool Program::LoadFile(const char* pFile, const Texture::TextureLoaderInfo* pUseLoader)
{
	ErrorCode oErr = LoadFileInternal(pFile, pUseLoader);
	if (oErr == ErrorCode::Ok)
	{
		return true;
	}
	const char* pMsg = (oErr == ErrorCode::Fail) ? "Not supported or corrupted file" : oErr.ToString();
	PlatformUtils::MsgBox(PlatformUtils::MessageBoxStyleEnum::CRITICAL, PlatformUtils::MessageBoxTypeEnum::OK, "Can't load file", pMsg);
	return false;
}

ErrorCode Program::LoadFileInternal(const char* pFile, const Texture::TextureLoaderInfo* pUseLoader)
{
	Core::LogInfo("Program", "Loading file '%s'", pFile);
	ErrorCode oErr = Texture::LoadFromFile(&m_oTexture, pFile, pUseLoader);
	if (oErr == ErrorCode::Ok)
	{
		Core::LogInfo("Program", "File loaded '%s'", pFile);
		m_sTexturePath = pFile;

		UpdateTexture2DRes();
		return ErrorCode::Ok;
	}

	Core::LogError("Program", "Can't load file '%s' : %s", pFile, oErr.ToString());
	return oErr;
}

void Program::Open()
{
	char pBuffer[1024] = { 0 };
	int iSelectedIndex = 0;

	Core::StringBuilder oExts;
	static char* s_pExts = NULL;

	const Texture::TextureLoaderInfo* pLoaders;
	int iLoaderCount;
	GetTextureLoaders(&pLoaders, &iLoaderCount);

	oExts += "All supported formats";
	oExts += (char)0;
	for (int iLoaderIndex = 0; iLoaderIndex < iLoaderCount; ++iLoaderIndex)
	{
		if (iLoaderIndex > 0)
			oExts += ';';
		oExts += pLoaders[iLoaderIndex].pExts;
	}
	oExts += (char)0;

	for (int iLoaderIndex = 0; iLoaderIndex < iLoaderCount; ++iLoaderIndex)
	{
		oExts += pLoaders[iLoaderIndex].pName;
		oExts += " (";
		oExts += pLoaders[iLoaderIndex].pExts;
		oExts += ')';
		oExts += (char)0;
		oExts += pLoaders[iLoaderIndex].pExts;
		oExts += (char)0;
	}

	char* pExts = oExts.Export();
	if (PlatformUtils::OpenFileDialog("Open file", pExts, pBuffer, sizeof(pBuffer), &iSelectedIndex))
	{
		const Texture::TextureLoaderInfo* pLoaderInfo = NULL;
		if (iSelectedIndex > 0)
		{
			pLoaderInfo = &pLoaders[iSelectedIndex - 1];
		}
		LoadFile(pBuffer, pLoaderInfo);
	}
	free(pExts);
}

void Program::Save()
{
	if (m_sTexturePath.empty())
	{
		SaveAs();
		return;
	}

	ErrorCode oErr = Texture::SaveToFile(&m_oTexture, NULL, NULL, m_sTexturePath.c_str());
	if (oErr == ErrorCode::Ok)
	{
		Core::LogInfo("Program", "File saved '%s'", m_sTexturePath.c_str());
	}
	else
	{
		Core::LogError("Program", "Can't save file '%s' : %s", m_sTexturePath.c_str(), oErr.ToString());
	}
}

void Program::SaveAs()
{
	if (m_oTexture.IsValid() == false)
		return;

	char pBuffer[1024] = { 0 };
	if (m_sTexturePath.size() < sizeof(pBuffer))
	{
		strncpy(pBuffer, m_sTexturePath.c_str(), m_sTexturePath.size());
	}
	int iSelectedIndex = 0;

	Core::StringBuilder oExts;
	static char* s_pExts = NULL;

	const Texture::TextureWriterInfo* pWriters;
	int iWriterCount;
	Texture::GetTextureWriters(&pWriters, &iWriterCount);

	for (int iWriterIndex = 0; iWriterIndex < iWriterCount; ++iWriterIndex)
	{
		oExts += pWriters[iWriterIndex].pName;
		oExts += " (";
		oExts += pWriters[iWriterIndex].pExt;
		oExts += ')';
		oExts += (char)0;
		oExts += pWriters[iWriterIndex].pExt;
		oExts += (char)0;

		if (m_sTexturePath.empty() == false
			&& Core::StringUtils::Wildcard(pWriters[iWriterIndex].pExt, m_sTexturePath.c_str(), false))
		{
			iSelectedIndex = iWriterIndex + 1;
		}
	}

	char* pExts = oExts.Export();
	if (PlatformUtils::SaveFileDialog("Save as", pExts, pBuffer, sizeof(pBuffer), &iSelectedIndex))
	{
		const Texture::TextureWriterInfo& oWriter = pWriters[iSelectedIndex];

		CORE_VERIFY_OK(Texture::SaveToFile(&m_oTexture, NULL, &oWriter, pBuffer));

		LoadFile(pBuffer);
	}
	free(pExts);
}

bool Program::OpenPreviousFile()
{
	if (m_sTexturePath.empty() == false)
	{
		char pBuffers[2][1024];
		char* pPreviousFile = pBuffers[0];
		char* pFile = pBuffers[1];
		strcpy(pFile, m_sTexturePath.c_str());
		while (IO::FileSystem::GetPreviousFile(pFile, pPreviousFile, sizeof(pBuffers[0])))
		{
			if (Texture::RetrieveTexturLoaderFromFilename(pPreviousFile) != NULL)
			{
				ErrorCode oErr = LoadFileInternal(pPreviousFile);
				if (oErr == ErrorCode::Ok)
				{
					Core::LogDebug("Program", "File '%s' loaded", pPreviousFile);
					return true;
				}
				else
				{
					Core::LogError("Program", "Can't load file '%s' : %s", pPreviousFile, oErr.ToString());
				}
			}

			pFile = pPreviousFile;
			pPreviousFile = pFile;
		}
	}
	return false;
}

bool Program::OpenNextFile()
{
	if (m_sTexturePath.empty() == false)
	{
		char pBuffers[2][1024];
		char* pNextFile = pBuffers[0];
		char* pFile = pBuffers[1];
		strcpy(pFile, m_sTexturePath.c_str());
		while (IO::FileSystem::GetNextFile(pFile, pNextFile, sizeof(pBuffers[0])))
		{
			if (Texture::RetrieveTexturLoaderFromFilename(pNextFile) != NULL)
			{
				ErrorCode oErr = LoadFileInternal(pNextFile);
				if (oErr == ErrorCode::Ok)
				{
					Core::LogDebug("Program", "File '%s' loaded", pNextFile);
					return true;
				}
				else
				{
					Core::LogError("Program", "Can't load file '%s' : %s", pNextFile, oErr.ToString());
				}
			}

			pFile = pNextFile;
			pNextFile = pFile;
		}
	}
	return false;
}

bool Program::CopyCurrentSlice()
{
	if (IO::Clipboard::SetTexturePNG(m_oTexture, 0, m_oDisplayOptions.iMip, m_oDisplayOptions.iSlice) == ErrorCode::Ok)
	{
		Core::LogInfo("Program", "Current Slice copied to clipboard");
		return true;
	}
	else
	{
		Core::LogError("Program", "Can't copy current Slice to clipboard");
		return false;
	}
}

bool Program::CopyComponent(Graphics::ComponentFlag eComponent)
{
	CORE_TEST_RETURN(m_oTexture.IsValid(), false);

	Graphics::Texture oNewTexture;
	if (Graphics::ExtractChannel(&m_oTexture, &oNewTexture, eComponent) == ErrorCode::Ok
		&& IO::Clipboard::SetTexturePNG(oNewTexture, 0, m_oDisplayOptions.iMip, m_oDisplayOptions.iSlice) == ErrorCode::Ok)
	{
		Core::LogInfo("Program", "Current Slice component %s copied to clipboard", Graphics::ComponentFlagString[eComponent]);
		return true;
	}
	else
	{
		Core::LogError("Program", "Can't copy current Slice component %s to clipboard", Graphics::ComponentFlagString[eComponent]);
		return false;
	}
}

bool Program::CopyComponentIndex(uint8_t iComponentIndex)
{
	CORE_TEST_RETURN(m_oTexture.IsValid(), false);

	const Graphics::PixelFormatInfos& oPixelFormatInfos = Graphics::PixelFormatEnumInfos[m_oTexture.GetPixelFormat()];
	uint8_t iCurrentIndex = 0;
	for (Graphics::ComponentFlag eComponent = Graphics::ComponentFlag::_BEGIN; eComponent <= Graphics::ComponentFlag::_END; eComponent = (Graphics::ComponentFlag)(eComponent << 1))
	{
		if ((oPixelFormatInfos.iComponents & eComponent) == 0 || Graphics::ComponentFlagString[eComponent] == NULL)
			continue;

		if (iCurrentIndex == iComponentIndex)
		{
			return CopyComponent(eComponent);
		}

		++iCurrentIndex;
	}

	return false;
}

bool Program::PasteToNewTexture()
{
	Graphics::Texture oClipboardTexture;
	if (IO::Clipboard::GetTexture(&oClipboardTexture))
	{
		oClipboardTexture.Swap(m_oTexture);
		Program::GetInstance()->ClearTextureFilePath();
		Program::GetInstance()->UpdateTexture2DRes();
		Core::LogInfo("Program", "Image paste to new texture from clipboard");
		return true;
	}
	else
	{
		Core::LogError("Program", "No image in clipboard");
		return false;
	}
}

bool Program::PasteToCurrentSlice()
{
	Graphics::Texture oClipboardTexture;
	if (IO::Clipboard::GetTexture(&oClipboardTexture))
	{
		Graphics::Texture::SliceData oSourceSliceData = oClipboardTexture.GetSliceData(0, 0, 0);
		Graphics::Texture::SliceData oDestSliceData = m_oTexture.GetSliceData(0, m_oDisplayOptions.iMip, m_oDisplayOptions.iSlice);

		if (oSourceSliceData.CopyTo(oDestSliceData) == ErrorCode::Ok)
		{
			Program::GetInstance()->UpdateTexture2DRes();
			Core::LogInfo("Program", "Image paste to current Slice from clipboard");
			return true;
		}
		else
		{
			Core::LogError("Program", "Can't paste image from clipboard to current Slice");
			return false;
		}
	}
	else
	{
		Core::LogError("Program", "No image in clipboard");
		return false;
	}
}

bool Program::PasteToComponent(Graphics::ComponentFlag eComponent)
{
	Graphics::Texture::SliceData oDestSliceData = m_oTexture.GetSliceData(0, m_oDisplayOptions.iMip, m_oDisplayOptions.iSlice);
	Graphics::Texture::ComponentAccessor oDestComponent = oDestSliceData.GetComponentAccesor(eComponent);

	Graphics::Texture oClipboardTexture;
	if (IO::Clipboard::GetTexture(&oClipboardTexture))
	{
		Graphics::Texture::SliceData oSourceSliceData = oClipboardTexture.GetSliceData(0, 0, 0);
		Graphics::Texture::ComponentAccessor oSourceCompoent = oSourceSliceData.GetComponentAccesor(Graphics::ComponentFlag::R);

		if (oSourceCompoent.CopyTo(oDestComponent))
		{
			Program::GetInstance()->UpdateTexture2DRes();
			Core::LogInfo("Program", "Image paste to current Slice component %s from clipboard", Graphics::ComponentFlagString[eComponent]);
			return true;
		}
		else
		{
			Core::LogError("Program", "Can't paste image from clipboard to current Slice component %s", Graphics::ComponentFlagString[eComponent]);
			return false;
		}
	}
	else
	{
		Core::LogError("Program", "No image in clipboard");
		return false;
	}
}

bool Program::PasteToComponentIndex(uint8_t iComponentIndex)
{
	CORE_TEST_RETURN(m_oTexture.IsValid(), false);

	const Graphics::PixelFormatInfos& oPixelFormatInfos = Graphics::PixelFormatEnumInfos[m_oTexture.GetPixelFormat()];
	uint8_t iCurrentIndex = 0;
	for (Graphics::ComponentFlag eComponent = Graphics::ComponentFlag::_BEGIN; eComponent <= Graphics::ComponentFlag::_END; eComponent = (Graphics::ComponentFlag)(eComponent << 1))
	{
		if ((oPixelFormatInfos.iComponents & eComponent) == 0 || Graphics::ComponentFlagString[eComponent] == NULL)
			continue;

		if (iCurrentIndex == iComponentIndex)
		{
			return PasteToComponent(eComponent);
		}

		++iCurrentIndex;
	}

	return false;
}

void Program::ReloadFile()
{
	if (m_sTexturePath.empty() == false)
	{
		LoadFile(m_sTexturePath.c_str());
	}
}

void Program::OpenPreviousFileCallback()
{
	OpenPreviousFile();
}

void Program::OpenNextFileCallback()
{
	OpenNextFile();
}

void Program::CopyCurrentSliceCallback()
{
	CopyCurrentSlice();
}

void Program::PasteToNewTextureCallback()
{
	PasteToNewTexture();
}

void Program::PasteToCurrentSliceCallback()
{
	if (m_oTexture.IsValid())
	{
		PasteToCurrentSlice();
	}
	else
	{
		PasteToNewTexture();
	}
}