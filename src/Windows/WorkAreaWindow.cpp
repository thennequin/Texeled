
#define IMGUI_DEFINE_MATH_OPERATORS
#include "WorkAreaWindow.h"

#include "Program.h"

#include "GraphicResources/Texture2D.h"
#include "GraphicResources/SamplerState.h"

#include "ImGuiUtils.h"

namespace Windows
{
#define SHADER_SOURCE(...) #__VA_ARGS__
	const char* const c_pWorkAreaWindowPixelShader = SHADER_SOURCE(
		float4 RedChannel = float4(1,0,0,0);
		float4 GreenChannel = float4(0, 1, 0, 0);
		float4 BlueChannel = float4(0, 0, 1, 0);
		float4 AlphaChannel = float4(0, 0, 0, 1);
		float4 AddChannel = float4(0, 0, 0, 0);
		int Mip = 0;
		int TextureIndex = 0;

		sampler sampler0;
		Texture2DArray texture0;

		struct PS_INPUT
		{
			float4 pos : SV_POSITION;
			float4 col : COLOR0;
			float2 uv : TEXCOORD0;
		};

		float4 main(PS_INPUT input) : SV_Target
		{
			float4 out_col = input.col * texture0.SampleLevel(sampler0, float3(input.uv, TextureIndex), Mip);
			out_col =
				out_col.r * RedChannel
				+ out_col.g * GreenChannel
				+ out_col.b * BlueChannel
				+ out_col.a * AlphaChannel
				+ AddChannel;
			return out_col;
		}
	);

	void WorkAreaWindow::BufferData::BuildChannelMix(ChannelFlags eShowChannel)
	{
		const float c_fRGB[4]{ 1.f, 1.f, 1.f, 0.f };
		const float c_fAlpha[4]{ 0.f, 0.f, 0.f, 1.f };
		const float c_fZero[4]{ 0.f, 0.f, 0.f, 0.f };


		if ((eShowChannel == ChannelFlag::ALPHA) || ((eShowChannel & ChannelFlag::ALPHA) == 0))
		{
			memcpy(fChannelsAdd, c_fAlpha, sizeof(c_fAlpha));
		}
		else
		{
			memcpy(fChannelsAdd, c_fZero, sizeof(c_fZero));
		}

		memcpy(fChannelsMix[0], c_fZero, sizeof(c_fZero));
		memcpy(fChannelsMix[1], c_fZero, sizeof(c_fZero));
		memcpy(fChannelsMix[2], c_fZero, sizeof(c_fZero));
		memcpy(fChannelsMix[3], c_fZero, sizeof(c_fZero));

		if (eShowChannel == ChannelFlag::RED)
		{
			memcpy(fChannelsMix[0], c_fRGB, sizeof(c_fRGB));
		}
		else if (eShowChannel == ChannelFlag::GREEN)
		{
			memcpy(fChannelsMix[1], c_fRGB, sizeof(c_fRGB));
		}
		else if (eShowChannel == ChannelFlag::BLUE)
		{
			memcpy(fChannelsMix[2], c_fRGB, sizeof(c_fRGB));
		}
		else if (eShowChannel == ChannelFlag::ALPHA)
		{
			memcpy(fChannelsMix[3], c_fRGB, sizeof(c_fRGB));
		}
		else
		{
			if ((eShowChannel & ChannelFlag::RED) != 0)
				fChannelsMix[0][0] = 1.f;
			if ((eShowChannel & ChannelFlag::GREEN) != 0)
				fChannelsMix[1][1] = 1.f;
			if ((eShowChannel & ChannelFlag::BLUE) != 0)
				fChannelsMix[2][2] = 1.f;
			if ((eShowChannel & ChannelFlag::ALPHA) != 0)
				fChannelsMix[3][3] = 1.f;
		}
	}

	WorkAreaWindow::WorkAreaWindow()
		: m_pCheckboardTexture2DRes(NULL)
	{
		SetTitle("WorkArea");
		SetAlone(true);

		m_fZoom = 1.0;
		m_oOffset = ImVec2(0.f, 0.f);

		m_fCurrentZoom = m_fZoom;
		m_oCurrentOffset = m_oOffset;

		Graphics::Texture::Desc oCheckerboardDesc;
		oCheckerboardDesc.ePixelFormat = Graphics::PixelFormatEnum::RGBA8_UNORM;
		oCheckerboardDesc.iWidth = 16;
		oCheckerboardDesc.iHeight = 16;
		Graphics::Texture oCheckerboardTexture;
		if (oCheckerboardTexture.Create(oCheckerboardDesc) == ErrorCode::Ok)
		{
			const Graphics::Texture::TextureFaceData& oFaceData = oCheckerboardTexture.GetData().GetFaceData(0, 0);

			int iHalfWidth = oCheckerboardDesc.iWidth >> 1;
			int iHalfHeight = oCheckerboardDesc.iHeight >> 1;
			for (int iY = 0; iY < oCheckerboardDesc.iHeight; ++iY)
			{
				unsigned char* pData = (unsigned char*)oFaceData.pData + iY * oCheckerboardDesc.iWidth * Graphics::PixelFormat::BitPerPixel(oCheckerboardDesc.ePixelFormat) / 8;
				for (int iX = 0; iX < oCheckerboardDesc.iWidth; ++iX)
				{
					int iColor = ((iX < iHalfWidth && iY < iHalfHeight) || (iX >= iHalfWidth && iY >= iHalfHeight)) ? 191 : 255;
					pData[4 * iX + 0] = iColor;
					pData[4 * iX + 1] = iColor;
					pData[4 * iX + 2] = iColor;
					pData[4 * iX + 3] = 255;
				}
			}

			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oCheckerboardTexture, &m_pCheckboardTexture2DRes));
		}

		GraphicResources::SamplerState::Desc oSamplerDesc;

		oSamplerDesc.eWrapU = GraphicResources::WrapEnum::REPEAT;
		oSamplerDesc.eWrapV = GraphicResources::WrapEnum::REPEAT;
		oSamplerDesc.eWrapW = GraphicResources::WrapEnum::REPEAT;

		m_pSamplerStatePointRepeat = new GraphicResources::SamplerState();
		oSamplerDesc.eFilter = GraphicResources::SamplerFilterEnum::NEAREST_MIPMAP_LINEAR;
		CORE_VERIFY_OK(m_pSamplerStatePointRepeat->Create(oSamplerDesc));

		m_pSamplerStateLinearRepeat = new GraphicResources::SamplerState();
		oSamplerDesc.eFilter = GraphicResources::SamplerFilterEnum::LINEAR_MIPMAP_LINEAR;
		CORE_VERIFY_OK(m_pSamplerStateLinearRepeat->Create(oSamplerDesc));

		oSamplerDesc.eWrapU = GraphicResources::WrapEnum::CLAMP_TO_EDGE;
		oSamplerDesc.eWrapV = GraphicResources::WrapEnum::CLAMP_TO_EDGE;
		oSamplerDesc.eWrapW = GraphicResources::WrapEnum::CLAMP_TO_EDGE;

		m_pSamplerStatePointClamp = new GraphicResources::SamplerState();
		oSamplerDesc.eFilter = GraphicResources::SamplerFilterEnum::NEAREST_MIPMAP_LINEAR;
		CORE_VERIFY_OK(m_pSamplerStatePointClamp->Create(oSamplerDesc));

		m_pSamplerStateLinearClamp = new GraphicResources::SamplerState();
		oSamplerDesc.eFilter = GraphicResources::SamplerFilterEnum::LINEAR_MIPMAP_LINEAR;
		CORE_VERIFY_OK(m_pSamplerStateLinearClamp->Create(oSamplerDesc));

		CORE_VERIFY_OK(GraphicResources::CompilePixelShader(c_pWorkAreaWindowPixelShader, "main", &m_pPixelShader, &m_oGlobalConstantBufferDesc));

		m_oGlobalConstantBufferDesc.InitBuffer(&m_oBufferData);

		CORE_VERIFY_OK(GraphicResources::CreateConstanteBuffer(&m_oGlobalConstantBufferDesc, &m_pGlobalConstantBuffer));

		DisplayOptions& oDisplayOptions = Program::GetInstance()->GetDisplayOptions();
		m_eCurrentShowChannels = oDisplayOptions.eShowChannels;
		m_oBufferData.iMip = oDisplayOptions.iMip;
		m_oBufferData.iFace = oDisplayOptions.iFace;
		m_oBufferData.BuildChannelMix(m_eCurrentShowChannels);
		CORE_VERIFY_OK(GraphicResources::FillConstantBuffer(m_pGlobalConstantBuffer, &m_oBufferData));
	}

	WorkAreaWindow::~WorkAreaWindow()
	{
		ImwSafeDelete(m_pCheckboardTexture2DRes);
		ImwSafeDelete(m_pSamplerStatePointRepeat);
		ImwSafeDelete(m_pSamplerStatePointClamp);
		ImwSafeDelete(m_pSamplerStateLinearRepeat);
		ImwSafeDelete(m_pSamplerStateLinearClamp);
	}

	void WorkAreaWindow::OnGui()
	{
		ID3D11DeviceContext* pDX11DeviceContext = Program::GetInstance()->GetDX11DeviceContext();
		DisplayOptions& oDisplayOptions = Program::GetInstance()->GetDisplayOptions();

		if (m_oBufferData.iMip != oDisplayOptions.iMip || m_oBufferData.iFace != oDisplayOptions.iFace || m_eCurrentShowChannels != oDisplayOptions.eShowChannels)
		{
			m_eCurrentShowChannels = oDisplayOptions.eShowChannels;
			m_oBufferData.iMip = oDisplayOptions.iMip;
			m_oBufferData.iFace = oDisplayOptions.iFace;
			m_oBufferData.BuildChannelMix(m_eCurrentShowChannels);
			CORE_VERIFY_OK(GraphicResources::FillConstantBuffer(m_pGlobalConstantBuffer, &m_oBufferData));
		}

		ImGuiIO& oIO = ImGui::GetIO();

		const int iCurrentMip = oDisplayOptions.iMip;

		Graphics::Texture& oTexture = Program::GetInstance()->GetTexture();
		GraphicResources::Texture2D* pTexture2DRes = Program::GetInstance()->GetTexture2DRes();

		if (oTexture.IsValid() && pTexture2DRes != NULL)
		{
			ImVec2 oTextureSize((float)oTexture.GetWidth(), (float)oTexture.GetHeight());
			ImVec2 oTextureMipSize((float)(oTexture.GetWidth() >> iCurrentMip), (float)(oTexture.GetHeight() >> iCurrentMip));
			double fTextureRatio = oTextureSize.x / oTextureSize.y;

			ImGui::BeginChild("##PreviewImageChild", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);

			ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
			ImDrawList* pDrawList = ImGui::GetWindowDrawList();

			ImVec2 oAreaMin = ImGui::GetItemRectMin();
			ImVec2 oAreaMax = ImGui::GetItemRectMax();
			ImVec2 oAreaSize = ImGui::GetItemRectSize();
			float fAreaRatio = oAreaSize.x / oAreaSize.y;

			bool bIsActive = false;
			if (!pWindow->SkipItems)
			{
				const ImGuiID id = pWindow->GetID("##ImageArea");
				ImVec2 size = ImGui::CalcItemSize(ImGui::GetWindowSize(), 0.0f, 0.0f);
				const ImRect bb(pWindow->DC.CursorPos, pWindow->DC.CursorPos + size);
				ImGui::ItemSize(bb);
				if (ImGui::ItemAdd(bb, &id))
				{
					const bool hovered = ImGui::IsItemHovered();
					bIsActive = ImGui::IsItemActive();
					if (hovered)
					{
						ImGui::SetHoveredID(id);

						if (oIO.MouseClicked[0] || oIO.MouseClicked[1] || oIO.MouseClicked[2])
						{
							ImGui::SetActiveID(id, pWindow);
						}
					}

					if (ImGui::GetCurrentContext()->ActiveId == id
						&& oIO.MouseDown[0] == false
						&& oIO.MouseDown[1] == false
						&& oIO.MouseDown[2] == false)
					{
						ImGui::SetActiveID(0, pWindow);
					}
					ImRect oArea(oAreaMin, oAreaMax);
				}
			}

			ImVec2 oSafeAreaSize(oAreaSize.x * 0.8f, oAreaSize.y * 0.8f);
			double fPixelRatio;
			double fMipPixelRatio;
			if (fAreaRatio < fTextureRatio)
			{
				fPixelRatio = oSafeAreaSize.x / (double)oTextureSize.x;
				fMipPixelRatio = oSafeAreaSize.x / (double)oTextureMipSize.x;
			}
			else
			{
				fPixelRatio = oSafeAreaSize.y / (double)oTextureSize.y;
				fMipPixelRatio = oSafeAreaSize.y / (double)oTextureMipSize.y;
			}

			ImVec2 oDiff(oAreaSize.x - oTextureSize.x * fPixelRatio, oAreaSize.y - oTextureSize.y * fPixelRatio);

			ImVec2 oCursorRealPos = ImGui::GetMousePos();
			ImVec2 oCursorPos = oCursorRealPos - oAreaMin - oDiff / 2.f;
			ImVec2 oCursorCoord = (oCursorPos - m_oOffset) / m_fZoom / fPixelRatio;

			ImVec2 oOffset = m_oOffset;
			ImVec2 oTextureMin = (ImVec2(0.f, 0.f) - m_oOffset - oDiff / 2.f) / fPixelRatio / m_fZoom;
			ImVec2 oTextureMax = (oAreaSize - m_oOffset - oDiff / 2.f) / fPixelRatio / m_fZoom;

			ImVec2 oImageStart = oAreaMin;
			ImVec2 oImageEnd = oAreaMax;
			ImVec2 oUv0 = ImVec2(oTextureMin.x / oTextureSize.x, oTextureMin.y / oTextureSize.y);
			ImVec2 oUv1 = ImVec2(oTextureMax.x / oTextureSize.x, oTextureMax.y / oTextureSize.y);
			ImVec2 oUvRange = oUv1 - oUv0;
			ImVec2 oTextureStart = oAreaMin - oAreaSize * ((oUv0) / oUvRange);
			ImVec2 oTextureEnd = oAreaMin + oAreaSize * ((oUvRange - (oUv1 - ImVec2(1.f, 1.f))) / oUvRange);
			if (!oDisplayOptions.bTiling)
			{
				if (oUv0.x < 0.f)
				{
					oImageStart.x = oTextureStart.x;
					oUv0.x = 0.f;
				}
				if (oUv0.y < 0.f)
				{
					oImageStart.y = oTextureStart.y;
					oUv0.y = 0.f;
				}

				if (oUv1.x > 1.f)
				{
					oImageEnd.x = oTextureEnd.x;
					oUv1.x = 1.f;
				}
				if (oUv1.y > 1.f)
				{
					oImageEnd.y = oTextureEnd.y;
					oUv1.y = 1.f;
				}
			}

			if (!pWindow->SkipItems)
			{
				pDrawList->AddRectFilled(oImageStart, oImageEnd, ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)));

				if (m_pCheckboardTexture2DRes != NULL)
				{
					pDrawList->AddImage((ImTextureID)m_pCheckboardTexture2DRes->GetTextureView(),
						oImageStart, oImageEnd,
						ImVec2(0.f, 0.f), (oImageEnd - oImageStart) / ImVec2(16.f, (float)16.f));
				}

				ImGuiUtils::PushPixelShader(m_pPixelShader);
				ImGuiUtils::PushSampler(oDisplayOptions.bShowPixelGrid
					? (oDisplayOptions.bTiling ? m_pSamplerStatePointRepeat : m_pSamplerStatePointClamp)
					: (oDisplayOptions.bTiling ? m_pSamplerStateLinearRepeat : m_pSamplerStateLinearClamp)
				);
				ImGuiUtils::PushPixelShaderConstantBuffer(m_pGlobalConstantBuffer);
				pDrawList->AddImage((ImTextureID)pTexture2DRes->GetTextureView(),
					oImageStart, oImageEnd,
					oUv0, oUv1);
				ImGuiUtils::PopSampler();
				ImGuiUtils::PopPixelShaderConstantBuffer();
				ImGuiUtils::PopPixelShader();

				if (oDisplayOptions.bShowPixelGrid && (fMipPixelRatio*m_fCurrentZoom) > 10.0)
				{
					double fStep = fMipPixelRatio * m_fCurrentZoom;
					ImU32 iColor = ImGui::GetColorU32(ImVec4(0.4f, 0.4f, 0.4f, 1.f));
					//Draw pixel grid

					double fStart;

					fStart = oImageStart.x - oUv0.x * fStep * oTextureMipSize.x;
					while (fStart < oAreaMin.x)
						fStart += fStep;

					while (oDisplayOptions.bTiling && fStart > oImageStart.y - fStep)
						fStart -= fStep;

					for (; fStart <= oImageEnd.x && fStart <= oAreaMax.x; fStart += fStep)
					{
						pDrawList->AddLine(ImVec2(fStart, oImageStart.y), ImVec2(fStart, oImageEnd.y), iColor);
					}

					fStart = oImageStart.y - oUv0.y * fStep * oTextureMipSize.y;
					while (fStart < oAreaMin.y)
						fStart += fStep;

					while (oDisplayOptions.bTiling && fStart > oImageStart.y - fStep)
						fStart -= fStep;

					for (; fStart <= oImageEnd.y && fStart <= oAreaMax.y; fStart += fStep)
					{
						pDrawList->AddLine(ImVec2(oImageStart.x, fStart), ImVec2(oImageEnd.x, fStart), iColor);
					}
				}
			}

			if (ImGui::IsItemHovered() && oIO.MouseWheel != 0.f)
			{
				float fWheel = oIO.MouseWheel;
				if (!oIO.KeyCtrl)
				{
					ImVec2 oNew = (oCursorPos - m_oOffset) / m_fZoom;

					if (fWheel < 0.f)
					{
						m_fZoom *= pow(0.8f, -fWheel);
						//m_fZoom *= 0.8f;
					}
					else
					{
						m_fZoom *= pow(1.2f, fWheel);
						//m_fZoom *= 1.2f;
					}

					if (fPixelRatio * m_fZoom > 500)
					{
						m_fZoom = 500 / fPixelRatio;
					}
					else if (fPixelRatio * m_fZoom < 0.0001)
					{
						m_fZoom = 0.0001 / fPixelRatio;
					}

					m_oOffset = oCursorPos - oNew * m_fZoom;
				}
			}

			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(2))
			{
				m_oOffset = m_oOffset + oIO.MouseDelta;
			}

			if (m_fCurrentZoom != m_fZoom)
			{
				m_fCurrentZoom += (m_fCurrentZoom - m_fZoom) * oIO.DeltaTime;
			}

			if (m_oCurrentOffset.x != m_fZoom)
			{
				m_oCurrentOffset.x += (m_oCurrentOffset.x - m_oOffset.x) * oIO.DeltaTime;
			}

			if (m_oCurrentOffset.y != m_fZoom)
			{
				m_oCurrentOffset.y += (m_oCurrentOffset.y - m_oOffset.x) * oIO.DeltaTime;
			}

			m_fCurrentZoom = m_fZoom;
			m_oCurrentOffset = m_oOffset;

			ImGui::EndChild();
		}
	}

	void WorkAreaWindow::OnDropFiles(int iCount, char** pFiles, const ImVec2& oPos)
	{
		Program::GetInstance()->LoadFile(pFiles[0]);
		//LoadFile(pFiles[0]);
	}
}
//namespace Windows