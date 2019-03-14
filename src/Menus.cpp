#define IMGUI_DEFINE_MATH_OPERATORS
#include "Menus.h"

#include "Program.h"
#include "ShortKeyManager.h"

#include "Graphics/Texture.h"
#include "Graphics/TextureUtils.h"

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
		ImGui::RenderCheckMark(pos + ImVec2(window->MenuColumns.Pos[2] + extra_w + g.FontSize * 0.20f, 0.0f), ImGui::GetColorU32(enabled ? ImGuiCol_Text : ImGuiCol_TextDisabled));

	return pressed;
}

void Menus::OnMenu()
{
	Graphics::Texture& oTexture = Program::GetInstance()->GetTexture();
	const Shortkeys& oShortkeys = Program::GetInstance()->GetShortkeys();
	const Fonts& oFonts = Program::GetInstance()->GetFonts();

	if (ImGui::BeginMenu("File"))
	{
		if (MenuItemPlus("Open", NULL, oShortkeys.pOpen->m_sShortKey.c_str(), oFonts.pFontConsolas, false, true))
		{
			Program::GetInstance()->Open();
		}
		if (MenuItemPlus("Save", NULL, oShortkeys.pSave->m_sShortKey.c_str(), oFonts.pFontConsolas, false, oTexture.IsValid()))
		{
			Program::GetInstance()->Save();
		}
		if (MenuItemPlus("Save as", NULL, oShortkeys.pSaveAs->m_sShortKey.c_str(), oFonts.pFontConsolas, false, oTexture.IsValid()))
		{
			Program::GetInstance()->SaveAs();
		}
		ImGui::EndMenu();
	}

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

			for (int iIndex = 0; iIndex < s_iAvailableConvertionFormatCount; ++iIndex)
			{
				Graphics::EPixelFormat ePixelFormat = s_oAvailableConvertionFormats[iIndex].eFormat;
				const char* pPixelFormatName = Graphics::EPixelFormat_string[ePixelFormat];
				if (ImGui::MenuItem(pPixelFormatName))
				{
					if (Graphics::ConvertPixelFormat(&oTexture, &oTexture, ePixelFormat) == ErrorCode::Ok)
					{
						Program::GetInstance()->UpdateTexture2DRes();
					}
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}

}