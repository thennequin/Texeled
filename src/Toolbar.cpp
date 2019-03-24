#include "Toolbar.h"

#include "Program.h"

#include "Math/Math.h"

#include "ImGuiUtils.h"

Toolbar::Toolbar()
{

}

Toolbar::~Toolbar()
{

}

void Toolbar::OnToolBar()
{
	/*
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 15.f);
	ProgramModeEnum eMode = Program::GetInstance()->GetMode();
	for (int iMode = 0; iMode < ProgramModeEnum::_COUNT; ++iMode)
	{
		if (iMode > 0)
			ImGui::SameLine(0,0);

		ImGuiUtils::GroupedPartEnum eToolbarMode = ( iMode == 0 ) ? ImGuiUtils::GroupedPartEnum::LEFT : ( iMode == ProgramModeEnum::_LAST ) ? ImGuiUtils::GroupedPartEnum::RIGHT : ImGuiUtils::GroupedPartEnum::MIDDLE;
		if (ImGuiUtils::GroupedButton(ProgramModeEnumStrings[iMode], iMode == eMode, eToolbarMode ))
		{
			Program::GetInstance()->SetMode((ProgramModeEnum)iMode);
		}
	}
	ImGui::PopStyleVar();
	*/

	DisplayOptions& oDisplay = Program::GetInstance()->GetDisplayOptions();

	const ImVec2 oSize16(16.f, 16.f);
	const ImVec2 oImageUv0 = ImGui::GetFontTexUvWhitePixel();
	const ImVec2 oImageUv1 = ImGui::GetFontTexUvWhitePixel();
	const ImVec4 oImageBgColor(0.f, 0.f, 0.f, 0.f);

	const ImVec4 oImageColor[4] = {
		{1.f, 0.f, 0.f, 1.f},
		{0.f, 1.f, 0.f, 1.f},
		{0.f, 0.f, 1.f, 1.f},
		{1.f, 1.f, 1.f, 1.f},
	};
	const ImVec4 oImageDarkColor[4] = {
		{0.5f, 0.f, 0.f, 1.f},
		{0.f, 0.5f, 0.f, 1.f},
		{0.f, 0.f, 0.5f, 1.f},
		{0.5f, 0.5f, 0.5f, 1.f},
	};

	const bool bKeyCtrlDown = ImGui::GetIO().KeyCtrl;

	ImTextureID hWhiteTexture = ImGui::GetIO().Fonts->TexID;

	ChannelFlags eChannelAll = ChannelFlag::RED | ChannelFlag::GREEN | ChannelFlag::BLUE | ChannelFlag::ALPHA;

	for (int iChannel = 0; iChannel < 4; ++iChannel)
	{
		ChannelFlag eChannel = (ChannelFlag)(1 << iChannel);

		ImGui::PushID(iChannel + 1);
		if (iChannel > 0)
			ImGui::SameLine();

		if (ImGui::ImageButton(hWhiteTexture, oSize16, oImageUv0, oImageUv1, -1, oImageBgColor, (oDisplay.eShowChannels & eChannel) ? oImageColor[iChannel] : oImageDarkColor[iChannel]))
		{
			bool bChannelVisible = (oDisplay.eShowChannels & eChannel) != 0;
			if (bKeyCtrlDown)
			{
				if (bChannelVisible)
				{
					oDisplay.eShowChannels = eChannelAll & ~eChannel;
				}
				else
				{
					oDisplay.eShowChannels = eChannel;
				}
			}
			else
			{
				if (bChannelVisible)
				{
					oDisplay.eShowChannels = oDisplay.eShowChannels & ~eChannel;
				}
				else
				{
					oDisplay.eShowChannels = oDisplay.eShowChannels | eChannel;
				}
			}

		}
		ImGui::PopID();
	}

	Graphics::Texture& oTexture = Program::GetInstance()->GetTexture();

	ImGui::PushItemWidth(200.f);
	ImGui::SameLine();
	ImGui::SliderInt("##Mip", &oDisplay.iMip, 0, Math::Max(0, oTexture.GetMipCount() - 1), "Mip:%.0f");

	ImGui::SameLine();
	ImGui::SliderInt("##Face", &oDisplay.iFace, 0, Math::Max(0, oTexture.GetFaceCount() - 1), "Face/Slice:%.0f");
	ImGui::PopItemWidth();

	ImGui::Checkbox("Show pixel grid", &oDisplay.bShowPixelGrid);

	ImGui::SameLine(0.f, 10.f);
	ImGui::Checkbox("Tiling", &oDisplay.bTiling);
}
