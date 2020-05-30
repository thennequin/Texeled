#include "StatusBars.h"

#include "Program.h"

#include "Core/StringUtils.h"

#include "Math/Arithmetic.h"

StatusBars::StatusBars()
{

}

StatusBars::~StatusBars()
{

}

void StatusBars::OnStatusBar()
{
	Program* pProgram = Program::GetInstance();
	int iCurrentMip = pProgram->GetDisplayOptions().iMip;
	Graphics::Texture& oTexture = pProgram->GetTexture();
	const Fonts& oFonts = pProgram->GetFonts();
	if (oTexture.IsValid())
	{
		//Width
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::TextUnformatted("Width:");
		ImGui::PopFont();

		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		if (iCurrentMip > 0)
		{
			ImGui::Text("%d(%d)", oTexture.GetWidth(), Math::Max(1, oTexture.GetWidth() >> iCurrentMip));
		}
		else
		{
			ImGui::Text("%d", oTexture.GetWidth());
		}
		ImGui::PopFont();

		//Height
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::SameLine();
		ImGui::TextUnformatted("Height:");
		ImGui::PopFont();

		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		if (iCurrentMip > 0)
		{
			ImGui::Text("%d(%d)", oTexture.GetHeight(), Math::Max(1,oTexture.GetHeight() >> iCurrentMip));
		}
		else
		{
			ImGui::Text("%d", oTexture.GetHeight());
		}
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
		ImGui::Text("%d", Graphics::PixelFormatEnumInfos[oTexture.GetPixelFormat()].iComponentCount);
		ImGui::PopFont();

		//Memory size
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::SameLine();
		ImGui::TextUnformatted("Memory:");
		ImGui::PopFont();

		char pBuffer[128];
		Core::StringUtils::GetReadableSize(oTexture.GetData().GetDataSize(), pBuffer, sizeof(pBuffer));
		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		ImGui::TextUnformatted(pBuffer);
		ImGui::PopFont();
	}
}