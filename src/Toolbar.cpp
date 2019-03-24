#include "Toolbar.h"

#include "Program.h"

#include "Math/Math.h"

#include "ImGuiUtils.h"

Toolbar::Toolbar()
	: m_fBackupGamma(-1.f)
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

	//Hack for keeping text alignement
	ImGui::GetCurrentWindow()->DC.CurrentLineTextBaseOffset = ImGui::GetCurrentWindow()->DC.PrevLineTextBaseOffset;
	ImGui::Text("Range:");
	const float c_fEpsylon = 0.001f;
	float fRange[2] = { oDisplay.fRange[0], oDisplay.fRange[1] };
	ImGui::PushItemWidth(150.f);
	ImGui::SameLine();
	if (ImGui::DragFloat("Min##ColorRange", &fRange[0], 0.005f, -999.f, fRange[1] - c_fEpsylon))
	{
		if (fRange[0] >= (fRange[1]))
			fRange[0] = fRange[1] - c_fEpsylon;

		oDisplay.fRange[0] = fRange[0];
	}

	ImGui::SameLine();
	if (ImGui::DragFloat("Max##ColorRange", &fRange[1], 0.005f, fRange[0] + c_fEpsylon, 999.f))
	{
		if (fRange[1] <= (fRange[0]))
			fRange[1] = fRange[0] + c_fEpsylon;

		oDisplay.fRange[1] = fRange[1];
	}
	ImGui::PopItemWidth();

	char pFloatBuffer[65];
	sprintf_s(pFloatBuffer, sizeof(pFloatBuffer), "%.1f", oDisplay.fGamma);

	ImGui::SameLine();
	ImGui::VerticalSeparator();

	ImGui::SameLine();
	ImGui::PushItemWidth(100.f);
	if (ImGui::BeginCombo("Gamma", pFloatBuffer))
	{
		ImGui::PushItemWidth(75.f);
		if (ImGui::InputFloat("Custom", &oDisplay.fGamma,0.f, 0.f, 2))
		{
			if (oDisplay.fGamma <= 0.f)
			{
				oDisplay.fGamma = 0.001f;
			}
		}
		ImGui::PopItemWidth();

		float fValue = m_fBackupGamma != -1.f ? m_fBackupGamma : oDisplay.fGamma;
		float fHoverValue = ImGui::IsWindowHovered() ? (int)oDisplay.fGamma : -1.f;

		const float c_fGammas[] = { 1.f, 1.4f, 1.8f, 2.2f, 2.6f };
		for (int iGamma = 0; iGamma < (sizeof(c_fGammas) / sizeof(*c_fGammas)); ++iGamma)
		{
			sprintf_s(pFloatBuffer, sizeof(pFloatBuffer), "%.1f", c_fGammas[iGamma]);
			if (ImGui::Selectable(pFloatBuffer, oDisplay.fGamma == c_fGammas[iGamma]))
			{
				fValue = c_fGammas[iGamma];
				oDisplay.fGamma = c_fGammas[iGamma];
				if (m_fBackupGamma != -1.f)
					m_fBackupGamma = c_fGammas[iGamma];
			}

			if (ImGui::IsItemHovered())
			{
				fHoverValue = c_fGammas[iGamma];
				oDisplay.fGamma = c_fGammas[iGamma];
			}
		}

		if (fHoverValue != -1.f)
		{
			if (m_fBackupGamma == -1.f)
				m_fBackupGamma = fValue;
		}
		else if (m_fBackupGamma != -1.f)
		{
			oDisplay.fGamma = m_fBackupGamma;
		}

		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
}
