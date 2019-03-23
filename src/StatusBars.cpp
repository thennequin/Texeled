#include "StatusBars.h"

#include "Program.h"

StatusBars::StatusBars()
{

}

StatusBars::~StatusBars()
{

}

void StatusBars::OnStatusBar()
{
	Graphics::Texture& oTexture = Program::GetInstance()->GetTexture();
	const Fonts& oFonts = Program::GetInstance()->GetFonts();
	if (oTexture.IsValid())
	{
		//Width
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::TextUnformatted("Width:");
		ImGui::PopFont();

		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		ImGui::Text("%d", oTexture.GetWidth());
		ImGui::PopFont();

		//Height
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::SameLine();
		ImGui::TextUnformatted("Height:");
		ImGui::PopFont();

		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		ImGui::Text("%d", oTexture.GetHeight());
		ImGui::PopFont();

		//Face/Slice
		if (oTexture.GetFaceCount() > 1)
		{
			ImGui::PushFont(oFonts.pFontConsolas);
			ImGui::SameLine();
			ImGui::TextUnformatted("Face:");
			ImGui::PopFont();

			ImGui::PushFont(oFonts.pFontConsolasBold);
			ImGui::SameLine(0, 0);
			ImGui::Text("%d", oTexture.GetFaceCount());
			ImGui::PopFont();
		}

		//Mips
		if (oTexture.GetMipCount() > 1)
		{
			ImGui::PushFont(oFonts.pFontConsolas);
			ImGui::SameLine();
			ImGui::TextUnformatted("Mip:");
			ImGui::PopFont();

			ImGui::PushFont(oFonts.pFontConsolasBold);
			ImGui::SameLine(0, 0);
			ImGui::Text("%d", oTexture.GetMipCount());
			ImGui::PopFont();
		}

		//Format
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::SameLine();
		ImGui::TextUnformatted("Format:");
		ImGui::PopFont();

		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		ImGui::Text("%s", Graphics::PixelFormatEnumInfos[oTexture.GetPixelFormat()].pName);
		ImGui::PopFont();

		//Channels
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::SameLine();
		ImGui::TextUnformatted("Channels:");
		ImGui::PopFont();

		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		ImGui::Text("%d", Graphics::PixelFormatEnumInfos[oTexture.GetPixelFormat()].iComponents);
		ImGui::PopFont();
	}
}