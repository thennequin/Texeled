#define IMGUI_DEFINE_MATH_OPERATORS
#include "Menus.h"

#include "Program.h"
#include "ShortKeyManager.h"

#include "Graphics/Texture.h"
#include "Graphics/TextureUtils.h"

#include "Windows/AboutWindow.h"
#include "Windows/LoggerWindow.h"

#include "IO/MemoryStream.h"

#include "ImGuiUtils.h"

#include "Resources/Icons/Open_16_png.h"
#include "Resources/Icons/Save_16_png.h"
#include "Resources/Icons/Exit_16_png.h"
#include "Resources/Icons/Log_16_png.h"
#include "Resources/Icons/Help_16_png.h"

Menus::Menus()
	: ImWindow::ImwMenu()
	, m_bResizeKeepRatio(false)
	, m_iResizeNewWidth(0)
	, m_iResizeNewHeight(0)
	, m_fResizeRatio(0.f)
	, m_pIconOpen(NULL)
	, m_pIconSave(NULL)
	, m_pIconExit(NULL)
	, m_pIconLog(NULL)
	, m_pIconHelp(NULL)
{

	//Load icons
	Graphics::Texture oTexture;

	// Open
	{
		IO::MemoryStream oMemStream(Resources::Icons::Open_16_png::Data, Resources::Icons::Open_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconOpen));
		}
	}
	// Save
	{
		IO::MemoryStream oMemStream(Resources::Icons::Save_16_png::Data, Resources::Icons::Save_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconSave));
		}
	}
	// Exit
	{
		IO::MemoryStream oMemStream(Resources::Icons::Exit_16_png::Data, Resources::Icons::Exit_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconExit));
		}
	}
	// Log
	{
		IO::MemoryStream oMemStream(Resources::Icons::Log_16_png::Data, Resources::Icons::Log_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconLog));
		}
	}
	// Help
	{
		IO::MemoryStream oMemStream(Resources::Icons::Help_16_png::Data, Resources::Icons::Help_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconHelp));
		}
	}

}

Menus::~Menus()
{
	if (m_pIconOpen != NULL)
	{
		delete m_pIconOpen;
		m_pIconOpen = NULL;
	}

	if (m_pIconSave != NULL)
	{
		delete m_pIconSave;
		m_pIconSave = NULL;
	}

	if (m_pIconExit != NULL)
	{
		delete m_pIconExit;
		m_pIconExit = NULL;
	}

	if (m_pIconLog != NULL)
	{
		delete m_pIconLog;
		m_pIconLog = NULL;
	}

	if (m_pIconHelp != NULL)
	{
		delete m_pIconHelp;
		m_pIconHelp = NULL;
	}
}

void Menus::OnMenu()
{
	Program* pProgram = Program::GetInstance();
	Graphics::Texture& oTexture = pProgram->GetTexture();
	const Shortkeys& oShortkeys = pProgram->GetShortkeys();
	const Fonts& oFonts = pProgram->GetFonts();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGuiUtils::MenuItemPlus("Open", NULL, oShortkeys.pOpen->m_sShortKey.c_str(), oFonts.pFontConsolas, false, true, (ImTextureID)m_pIconOpen->GetTextureView()))
		{
			pProgram->Open();
		}
		if (ImGuiUtils::MenuItemPlus("Save", NULL, oShortkeys.pSave->m_sShortKey.c_str(), oFonts.pFontConsolas, false, oTexture.IsValid(), (ImTextureID)m_pIconSave->GetTextureView()))
		{
			pProgram->Save();
		}
		if (ImGuiUtils::MenuItemPlus("Save as", NULL, oShortkeys.pSaveAs->m_sShortKey.c_str(), oFonts.pFontConsolas, false, oTexture.IsValid()))
		{
			pProgram->SaveAs();
		}
		ImGui::Separator();
		if (ImGuiUtils::MenuItemPlus("Open previous file", NULL, oShortkeys.pOpenPreviousFile->m_sShortKey.c_str(), oFonts.pFontConsolas, false, pProgram->GetTextureFilePath() != NULL))
		{
			pProgram->OpenPreviousFile();
		}
		if (ImGuiUtils::MenuItemPlus("Open next file", NULL, oShortkeys.pOpenNextFile->m_sShortKey.c_str(), oFonts.pFontConsolas, false, pProgram->GetTextureFilePath() != NULL))
		{
			pProgram->OpenNextFile();
		}
		ImGui::Separator();
		if (ImGuiUtils::MenuItemPlus("Exit", NULL, oShortkeys.pClose->m_sShortKey.c_str(), oFonts.pFontConsolas, false, true, (ImTextureID)m_pIconExit->GetTextureView()))
		{
			pProgram->AskExit();
		}
		ImGui::EndMenu();
	}

	bool bOpenResizeMenu = false;

	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGuiUtils::BeginMenu("Convert pixel format to", oTexture.IsValid()))
		{
			static Graphics::PixelFormat::ConvertionInfoList s_oAvailableConvertionFormats;
			static int s_iAvailableConvertionFormatCount = 0;
			if (ImGui::GetCurrentWindowRead()->WasActive == false)
			{
				s_iAvailableConvertionFormatCount = Graphics::PixelFormat::GetAvailableConvertion(oTexture.GetPixelFormat(), true, &s_oAvailableConvertionFormats);
			}

			int iTextureWidth = oTexture.GetWidth();
			int iTextureHeight = oTexture.GetHeight();
			for (int iIndex = 0; iIndex < s_iAvailableConvertionFormatCount; ++iIndex)
			{
				Graphics::PixelFormatEnum ePixelFormat = s_oAvailableConvertionFormats[iIndex].eFormat;
				const Graphics::PixelFormatInfos& oPixelFormatInfos = Graphics::PixelFormatEnumInfos[ePixelFormat];

				bool bGoodWidth = ((iTextureWidth / oPixelFormatInfos.iBlockWidth) * oPixelFormatInfos.iBlockWidth == iTextureWidth);
				bool bGoodHeight = ((iTextureHeight / oPixelFormatInfos.iBlockHeight) * oPixelFormatInfos.iBlockHeight == iTextureHeight);
				bool bAvailable = bGoodWidth && bGoodHeight;

				if (ImGuiUtils::MenuItemPlus(oPixelFormatInfos.pName, NULL, NULL, NULL, false, bAvailable))
				{
					if (Graphics::ConvertPixelFormat(&oTexture, &oTexture, ePixelFormat) == ErrorCode::Ok)
					{
						Program::GetInstance()->UpdateTexture2DRes();
					}
				}
				if (bAvailable == false && ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted("Pixel format not available");
					if (bGoodWidth == false)
						ImGui::Text("Texture width multiplier should be %d", oPixelFormatInfos.iBlockWidth);
					if (bGoodHeight == false)
						ImGui::Text("Texture height multiplier should be %d", oPixelFormatInfos.iBlockWidth);
					ImGui::EndTooltip();
				}
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		bool bIsResizablePixelFormat = oTexture.IsValid() && Graphics::IsPixelFormatResizable(oTexture.GetPixelFormat());
		if (ImGuiUtils::MenuItemPlus("Resize", NULL, NULL, NULL, false, bIsResizablePixelFormat))
		{
			bOpenResizeMenu = true;
		}
		if (oTexture.IsValid() && bIsResizablePixelFormat == false && ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Resize not supported for this pixel format");
		}

		if (ImGuiUtils::MenuItemPlus("Generate all mips", NULL, NULL, NULL, false, bIsResizablePixelFormat))
		{
			if (Graphics::GenerateMips(&oTexture, &oTexture, false) == ErrorCode::Ok)
				Program::GetInstance()->UpdateTexture2DRes();
		}

		if (ImGuiUtils::MenuItemPlus("Generate missing mips", NULL, NULL, NULL, false, bIsResizablePixelFormat))
		{
			if (Graphics::GenerateMips(&oTexture, &oTexture, true) == ErrorCode::Ok)
				Program::GetInstance()->UpdateTexture2DRes();
		}

		ImGui::EndMenu();
	}

	if (bOpenResizeMenu)
	{
		if (oTexture.IsValid())
		{
			ImGui::OpenPopup("TextureResize");
			m_bResizeKeepRatio = true;
			m_iResizeNewWidth = oTexture.GetWidth();
			m_iResizeNewHeight = oTexture.GetHeight();
			m_fResizeRatio = (double)m_iResizeNewWidth / (double)m_iResizeNewHeight;
		}
	}

	if (ImGui::BeginPopupModal("TextureResize", 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Checkbox("Keep ratio", &m_bResizeKeepRatio);
		if (ImGui::DragInt("Width", &m_iResizeNewWidth, 0.5f))
		{
			if (m_iResizeNewWidth < 1)
				m_iResizeNewWidth = 1;
			if (m_iResizeNewWidth > 65536)
				m_iResizeNewWidth = 65536;

			if (m_bResizeKeepRatio)
				m_iResizeNewHeight = (int)(m_iResizeNewWidth / m_fResizeRatio);
		}

		if (ImGui::DragInt("Height", &m_iResizeNewHeight, 0.5f))
		{
			if (m_iResizeNewHeight < 1)
				m_iResizeNewHeight = 1;
			if (m_iResizeNewHeight > 16384)
				m_iResizeNewHeight = 16384;

			if (m_bResizeKeepRatio)
				m_iResizeNewWidth = (int)(m_iResizeNewHeight * m_fResizeRatio);
		}

		if (oTexture.GetMipCount() > 1)
		{
			ImGui::TextDisabled("Mip maps will be erased");
		}

		if (ImGui::Button("Resize"))
		{
			ErrorCode oErr = Graphics::ResizeTexture(&oTexture, &oTexture, m_iResizeNewWidth, m_iResizeNewHeight);
			CORE_VERIFY_OK(oErr);
			if (oErr == ErrorCode::Ok)
			{
				Program::GetInstance()->UpdateTexture2DRes();
			}
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginMenu("Help"))
	{
		if (ImGuiUtils::MenuItemPlus("Logs", NULL, NULL, NULL, false, true, (ImTextureID)m_pIconLog->GetTextureView()))
		{
			ImWindow::ImwWindowManager* pWindowManager = Program::GetInstance()->GetWindowManager();
			pWindowManager->Dock(new Windows::LoggerWindow(), ImWindow::E_DOCK_ORIENTATION_BOTTOM, 0.3f);
		}

		ImGui::Separator();

		if (ImGuiUtils::MenuItemPlus("About", NULL, NULL, NULL, false, true, (ImTextureID)m_pIconHelp->GetTextureView()))
		{
			const ImVec2 c_vAboutBoxSize = ImVec2(600.f, 350.f);

			// Centering floating window
			ImWindow::ImwWindowManager* pWindowManager = Program::GetInstance()->GetWindowManager();
			ImVec2 vMainPos = pWindowManager->GetMainPlatformWindow()->GetPosition();
			ImVec2 vMainSize = pWindowManager->GetMainPlatformWindow()->GetSize();
			ImVec2 vCenter = vMainPos + vMainSize / 2.f - c_vAboutBoxSize / 2.f;

			pWindowManager->Float(new Windows::AboutWindow(), vCenter, c_vAboutBoxSize);
		}
		ImGui::EndMenu();
	}
}