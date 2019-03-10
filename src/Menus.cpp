#include "Menus.h"

#include "Program.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureUtils.h"

void Menus::OnMenu()
{
	Graphics::Texture& oTexture = Program::GetInstance()->GetTexture();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open"))
		{
			Program::GetInstance()->Open();
		}
		if (ImGui::MenuItem("Save", NULL, false, oTexture.IsValid()))
		{
			Program::GetInstance()->Save();
		}
		if (ImGui::MenuItem("Save as", NULL, false, oTexture.IsValid()))
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