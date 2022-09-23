#include "Toolbar.h"

#include "Program.h"

#include "Math/Arithmetic.h"

#include "ImGuiUtils.h"

Toolbar::Toolbar()
	: m_iBackupSlice(-1)
	, m_fBackupGamma(-1.f)
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

	ImGui::SameLine();
	ImGui::PushItemWidth(200.f);
	ImGui::SliderInt("##Mip", &oDisplay.iMip, 0, Math::Max(0, oTexture.GetMipCount() - 1), "Mip:%.0f");
	ImGui::PopItemWidth();

	if (ImGui::IsItemHovered())
	{
		ImGuiIO& oIO = ImGui::GetIO();

		if (oIO.KeyCtrl && oIO.MouseWheel > 0)
		{
			if (oDisplay.iMip > 0)
				oDisplay.iMip--;
		}
		else if (oIO.KeyCtrl && oIO.MouseWheel < 0)
		{
			if (oDisplay.iMip < (oTexture.GetMipCount() - 1))
				oDisplay.iMip++;
		}
	}

	ImGui::SameLine();
	Graphics::Texture::FaceFlags iFaces = oTexture.GetFaces();
	if (iFaces != Graphics::Texture::FaceFlag::NONE)
	{
		ImGui::PushItemWidth(50.f);
		Graphics::Texture::FaceFlag eCurrentFace = Graphics::Texture::GetFace(iFaces, oDisplay.iSlice);
		if (ImGui::BeginCombo("##Face", Graphics::Texture::FaceFlagString[eCurrentFace]))
		{
			int iHoverValue = -1;
			uint8_t iNextBit = 0;
			while ((iNextBit = Math::HighBitNext(iNextBit, iFaces)) != 0)
			{
				Graphics::Texture::FaceFlag eFace = (Graphics::Texture::FaceFlag)(1 << (iNextBit - 1));
				int iSlice = Graphics::Texture::GetFaceIndex(iFaces, eFace);
				if (ImGui::Selectable(Graphics::Texture::FaceFlagString[eFace], eFace == eCurrentFace))
				{
					oDisplay.iSlice = iSlice;
				}

				if (ImGui::IsItemHovered())
				{
					if (m_iBackupSlice == -1)
						m_iBackupSlice = oDisplay.iSlice;

					oDisplay.iSlice = iSlice;
					iHoverValue = iSlice;
				}
			}

			if (iHoverValue == -1 && m_iBackupSlice != -1)
			{
				oDisplay.iSlice = m_iBackupSlice;
				m_iBackupSlice = -1;
			}

			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}
	else
	{
		ImGui::PushItemWidth(200.f);
		ImGui::SliderInt("##Slice", &oDisplay.iSlice, 0, Math::Max(0, oTexture.GetSliceCount() - 1), "Slice:%.0f");
		ImGui::PopItemWidth();
	}

	if (ImGui::IsItemHovered())
	{
		ImGuiIO& oIO = ImGui::GetIO();

		if (oIO.KeyCtrl && oIO.MouseWheel > 0)
		{
			if (oDisplay.iSlice > 0)
				oDisplay.iSlice--;
		}
		else if (oIO.KeyCtrl && oIO.MouseWheel < 0)
		{
			if (oDisplay.iSlice < (oTexture.GetSliceCount() - 1))
				oDisplay.iSlice++;
		}
	}

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

	if (ImGui::IsItemClicked(1))
	{
		fRange[0] = 0.f;
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

	if (ImGui::IsItemClicked(1))
	{
		fRange[1] = 1.f;
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

	const float c_fGammaPresets[] = { 1.f, 1.4f, 1.8f, 2.2f, 2.6f };
	int c_iGammaPresetCount = (sizeof(c_fGammaPresets) / sizeof(*c_fGammaPresets));

	if (ImGui::BeginCombo("Gamma", pFloatBuffer))
	{
		float fValue = m_fBackupGamma != -1.f ? m_fBackupGamma : oDisplay.fGamma;
		float fHoverValue = ImGui::IsWindowHovered() ? (int)oDisplay.fGamma : -1.f;

		ImGui::PushItemWidth(75.f);
		if (ImGui::DragFloat("Custom", &oDisplay.fGamma, 0.01f, 0.01f, 100.f))
		{
			if (oDisplay.fGamma <= 0.f)
			{
				oDisplay.fGamma = 0.01f;
			}
			fValue = oDisplay.fGamma;
			m_fBackupGamma = oDisplay.fGamma;
		}
		ImGui::PopItemWidth();

		for (int iGamma = 0; iGamma < c_iGammaPresetCount; ++iGamma)
		{
			sprintf_s(pFloatBuffer, sizeof(pFloatBuffer), "%.1f", c_fGammaPresets[iGamma]);
			if (ImGui::Selectable(pFloatBuffer, oDisplay.fGamma == c_fGammaPresets[iGamma]))
			{
				fValue = c_fGammaPresets[iGamma];
				oDisplay.fGamma = c_fGammaPresets[iGamma];
				if (m_fBackupGamma != -1.f)
					m_fBackupGamma = c_fGammaPresets[iGamma];
			}

			if (ImGui::IsItemHovered())
			{
				fHoverValue = c_fGammaPresets[iGamma];
				oDisplay.fGamma = c_fGammaPresets[iGamma];
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
			m_fBackupGamma = -1.f;
		}

		ImGui::EndCombo();
	}

	if (ImGui::IsItemHovered())
	{
		ImGuiIO& oIO = ImGui::GetIO();

		int iCurrentGammaPreset = -1;
		for (int iGamma = 0; iGamma < c_iGammaPresetCount; ++iGamma)
		{
			if (oDisplay.fGamma == c_fGammaPresets[iGamma])
			{
				iCurrentGammaPreset = iGamma;
				break;
			}
		}

		if (iCurrentGammaPreset != -1)
		{
			if (oIO.KeyCtrl && oIO.MouseWheel > 0)
			{
				if (iCurrentGammaPreset > 0)
					oDisplay.fGamma = c_fGammaPresets[iCurrentGammaPreset - 1];
			}
			else if (oIO.KeyCtrl && oIO.MouseWheel < 0)
			{
				if (iCurrentGammaPreset < (c_iGammaPresetCount - 1))
					oDisplay.fGamma = c_fGammaPresets[iCurrentGammaPreset + 1];
			}
		}
	}

	ImGui::PopItemWidth();
}
