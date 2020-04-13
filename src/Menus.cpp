#define IMGUI_DEFINE_MATH_OPERATORS
#include "Menus.h"

#include "Program.h"
#include "ShortKeyManager.h"

#include "Graphics/Texture.h"
#include "Graphics/TextureUtils.h"

#include "Windows/AboutWindow.h"
#include "Windows/LoggerWindow.h"

bool MenuItemPlus(const char* label, ImFont* pLabelFont, const char* shortcut, ImFont* pShortkeyFont, bool selected, bool enabled)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImVec2 pos = window->DC.CursorPos;
	ImGui::PushFont(pLabelFont);
	ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	ImGui::PopFont();
	ImGui::PushFont(pShortkeyFont);
	ImVec2 shortcut_size = shortcut ? ImGui::CalcTextSize(shortcut, NULL) : ImVec2(0.0f, 0.0f);
	ImGui::PopFont();
	float w = window->MenuColumns.DeclColumns(label_size.x, shortcut_size.x, (float)(int)(g.FontSize * 1.20f)); // Feedback for next frame
	float extra_w = ImMax(0.0f, ImGui::GetContentRegionAvail().x - w);

	ImGui::PushFont(pLabelFont);
	bool pressed = ImGui::Selectable(label, false, ImGuiSelectableFlags_MenuItem | ImGuiSelectableFlags_DrawFillAvailWidth | (enabled ? 0 : ImGuiSelectableFlags_Disabled), ImVec2(w, 0.0f));
	ImGui::PopFont();
	if (shortcut_size.x > 0.0f)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, g.Style.Colors[ImGuiCol_TextDisabled]);
		ImGui::PushFont(pShortkeyFont);
		ImGui::RenderText(pos + ImVec2(window->MenuColumns.Pos[1] + extra_w, 0.0f), shortcut, NULL, false);
		ImGui::PopFont();
		ImGui::PopStyleColor();
	}

	if (selected)
		ImGui::RenderCheckMark(pos + ImVec2(window->MenuColumns.Pos[2] + extra_w + g.FontSize * 0.20f, 0.0f), ImGui::GetColorU32(enabled ? ImGuiCol_Text : ImGuiCol_TextDisabled), g.FontSize);

	return pressed;
}

Menus::Menus()
	: ImWindow::ImwMenu()
	, m_bResizeKeepRatio(false)
	, m_iResizeNewWidth(0)
	, m_iResizeNewHeight(0)
	, m_fResizeRatio(0.f)
{
}

void Menus::OnMenu()
{
	Program* pProgram = Program::GetInstance();
	Graphics::Texture& oTexture = pProgram->GetTexture();
	const Shortkeys& oShortkeys = pProgram->GetShortkeys();
	const Fonts& oFonts = pProgram->GetFonts();

	if (ImGui::BeginMenu("File"))
	{
		if (MenuItemPlus("Open", NULL, oShortkeys.pOpen->m_sShortKey.c_str(), oFonts.pFontConsolas, false, true))
		{
			pProgram->Open();
		}
		if (MenuItemPlus("Save", NULL, oShortkeys.pSave->m_sShortKey.c_str(), oFonts.pFontConsolas, false, oTexture.IsValid()))
		{
			pProgram->Save();
		}
		if (MenuItemPlus("Save as", NULL, oShortkeys.pSaveAs->m_sShortKey.c_str(), oFonts.pFontConsolas, false, oTexture.IsValid()))
		{
			pProgram->SaveAs();
		}
		ImGui::Separator();
		if (MenuItemPlus("Open previous file", NULL, oShortkeys.pOpenPreviousFile->m_sShortKey.c_str(), oFonts.pFontConsolas, false, pProgram->GetTextureFilePath() != NULL))
		{
			pProgram->OpenPreviousFile();
		}
		if (MenuItemPlus("Open next file", NULL, oShortkeys.pOpenNextFile->m_sShortKey.c_str(), oFonts.pFontConsolas, false, pProgram->GetTextureFilePath() != NULL))
		{
			pProgram->OpenNextFile();
		}
		ImGui::Separator();
		if (MenuItemPlus("Exit", NULL, NULL, oFonts.pFontConsolas, false, true))
		{
			pProgram->AskExit();
		}
		ImGui::EndMenu();
	}

	bool bOpenResizeMenu = false;

	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::BeginMenu("Convert pixel format to", oTexture.IsValid()))
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

				if (ImGui::MenuItem(oPixelFormatInfos.pName, NULL, false, bAvailable))
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
		if (ImGui::MenuItem("Resize", NULL, false, bIsResizablePixelFormat))
		{
			bOpenResizeMenu = true;
		}
		if (oTexture.IsValid() && bIsResizablePixelFormat == false && ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Resize not supported for this pixel format");
		}

		if (ImGui::MenuItem("Generate all mips", NULL, false, bIsResizablePixelFormat))
		{
			if (Graphics::GenerateMips(&oTexture, &oTexture, false) == ErrorCode::Ok)
				Program::GetInstance()->UpdateTexture2DRes();
		}

		if (ImGui::MenuItem("Generate missing mips", NULL, false, bIsResizablePixelFormat))
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
		if (ImGui::MenuItem("Logs"))
		{
			ImWindow::ImwWindowManager* pWindowManager = Program::GetInstance()->GetWindowManager();
			pWindowManager->Dock(new Windows::LoggerWindow(), ImWindow::E_DOCK_ORIENTATION_BOTTOM, 0.3f);
		}

		ImGui::Separator();

		if (ImGui::MenuItem("About"))
		{
			const ImVec2 c_vAboutBoxSize = ImVec2(600.f, 160.f);

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