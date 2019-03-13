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
	if (oTexture.IsValid())
	{
		ImGui::Text("%d x %d (%d mip) : %s (%d channels)"
			, oTexture.GetWidth()
			, oTexture.GetHeight()
			, oTexture.GetMipCount()
			, Graphics::EPixelFormat_string[oTexture.GetPixelFormat()]
			, Graphics::EPixelFormatInfos[oTexture.GetPixelFormat()].iComponents
		);
	}
}