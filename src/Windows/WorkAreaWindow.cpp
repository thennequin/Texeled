
#define IMGUI_DEFINE_MATH_OPERATORS
#include "WorkAreaWindow.h"

#include "Program.h"

#include "Math/Arithmetic.h"

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
		float Range0;
		float Range1;
		float Gamma = 1.f;

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

			out_col = (pow(out_col, 1.0 / Gamma) - Range0) / (Range1 - Range0);

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
		: ImWindow::ImwWindow(ImWindow::E_WINDOW_MODE_ALONE)
		, m_pCheckboardTexture2DRes(NULL)
	{
		SetTitle("WorkArea");
		SetFillingSpace(true);

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
					unsigned char iColor = ((iX < iHalfWidth && iY < iHalfHeight) || (iX >= iHalfWidth && iY >= iHalfHeight)) ? 191 : 255;
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
		m_oBufferData.fRange[0] = oDisplayOptions.fRange[0];
		m_oBufferData.fRange[1] = oDisplayOptions.fRange[1];
		m_oBufferData.fGamma = oDisplayOptions.fGamma;
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
		DisplayOptions& oDisplayOptions = Program::GetInstance()->GetDisplayOptions();

		if (m_oBufferData.iMip != oDisplayOptions.iMip
			|| m_oBufferData.iFace != oDisplayOptions.iFace
			|| m_eCurrentShowChannels != oDisplayOptions.eShowChannels
			|| m_oBufferData.fRange[0] != oDisplayOptions.fRange[0]
			|| m_oBufferData.fRange[1] != oDisplayOptions.fRange[1]
			|| m_oBufferData.fGamma != oDisplayOptions.fGamma)
		{
			m_eCurrentShowChannels = oDisplayOptions.eShowChannels;
			m_oBufferData.iMip = oDisplayOptions.iMip;
			m_oBufferData.iFace = oDisplayOptions.iFace;
			m_oBufferData.BuildChannelMix(m_eCurrentShowChannels);
			m_oBufferData.fRange[0] = oDisplayOptions.fRange[0];
			m_oBufferData.fRange[1] = oDisplayOptions.fRange[1];
			m_oBufferData.fGamma = oDisplayOptions.fGamma;
			CORE_VERIFY_OK(GraphicResources::FillConstantBuffer(m_pGlobalConstantBuffer, &m_oBufferData));
		}

		ImGuiIO& oIO = ImGui::GetIO();

		const int iCurrentMip = oDisplayOptions.iMip;

		Graphics::Texture& oTexture = Program::GetInstance()->GetTexture();
		GraphicResources::Texture2D* pTexture2DRes = Program::GetInstance()->GetTexture2DRes();

		if (oTexture.IsValid() && pTexture2DRes != NULL)
		{
			const Graphics::PixelFormatInfos& oPixelFormatInfo = Graphics::PixelFormatEnumInfos[oTexture.GetPixelFormat()];

			ImVec2 oTextureSize((float)oTexture.GetWidth(), (float)oTexture.GetHeight());
			ImVec2 oTextureMipSize((float)(oTexture.GetWidth() >> iCurrentMip), (float)(oTexture.GetHeight() >> iCurrentMip));
			double fTextureRatio = oTextureSize.x / oTextureSize.y;

			ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
			ImDrawList* pDrawList = ImGui::GetWindowDrawList();

			bool bIsActive = false;
			const ImGuiID id = pWindow->GetID("##ImageArea");
			const ImRect oBB(pWindow->DC.CursorPos, pWindow->DC.CursorPos + ImGui::GetContentRegionAvail());
			const ImVec2 oBBSize = oBB.GetSize();
			float fAreaRatio = oBBSize.x / oBBSize.y;
			ImGui::ItemSize(oBB);
			if (ImGui::ItemAdd(oBB, id))
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
			}

			double oSafeAreaSize[2] = { oBBSize.x * 0.8f, oBBSize.y * 0.8f };
			double fPixelRatio;

			if (fAreaRatio < fTextureRatio)
			{
				fPixelRatio = oSafeAreaSize[0] / (double)oTextureSize.x;
			}
			else
			{
				fPixelRatio = oSafeAreaSize[1] / (double)oTextureSize.y;
			}

			ImVec2 oDiff((float)(oBBSize.x - oTextureSize.x * fPixelRatio), (float)(oBBSize.y - oTextureSize.y * fPixelRatio));

			double fZoomPixelRatio = (fPixelRatio * m_fZoom);

			ImVec2 oOffset = m_oOffset;
			ImVec2 oTextureMin = (ImVec2(0.f, 0.f) - m_oOffset - oDiff / 2.f) / fZoomPixelRatio;
			ImVec2 oTextureMax = (oBBSize - m_oOffset - oDiff / 2.f) / fZoomPixelRatio;

			ImVec2 oImageStart = oBB.Min;
			ImVec2 oImageEnd = oBB.Max;
			ImVec2 oUv0 = ImVec2(oTextureMin.x / oTextureSize.x, oTextureMin.y / oTextureSize.y);
			ImVec2 oUv1 = ImVec2(oTextureMax.x / oTextureSize.x, oTextureMax.y / oTextureSize.y);
			ImVec2 oUvRange = oUv1 - oUv0;
			ImVec2 oTextureStart = oBB.Min - oBBSize * ((oUv0) / oUvRange);
			ImVec2 oTextureEnd = oBB.Min + oBBSize * ((oUvRange - (oUv1 - ImVec2(1.f, 1.f))) / oUvRange);
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

			float fTest = fPixelRatio * oTextureSize.x / oTextureMipSize.x;
			double fMipPixelSize[2] = { fPixelRatio * oTextureSize.x / oTextureMipSize.x, fPixelRatio * oTextureSize.y / oTextureMipSize.y };
			ImVec2 oZoomMipPixelRatio = ImVec2((float)(fMipPixelSize[0] * m_fZoom), (float)(fMipPixelSize[1] * m_fZoom));

			ImVec2 oCursorRealPos = ImGui::GetMousePos();
			ImVec2 oCursorPos = oCursorRealPos - oBB.Min - oDiff / 2.f;
			ImVec2 oCursorMipCoord = (oCursorPos - m_oOffset) / oZoomMipPixelRatio;

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

				//Draw pixel grid
				if (oDisplayOptions.bShowPixelGrid && (Math::Min(fMipPixelSize[0], fMipPixelSize[1]) * m_fCurrentZoom) > 10.0)
				{
					pDrawList->ChannelsSplit(2);

					double fStepX = fMipPixelSize[0] * m_fCurrentZoom;
					double fStepY = fMipPixelSize[1] * m_fCurrentZoom;
					ImU32 iColorBlock = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.f));
					ImU32 iColorPixel = ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.f));
					ImU32 iColorBack = ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.2f, 0.5f));

					// Vertical lines
					int iCurrentX = floor(oUv0.x / (fStepX * (double)oTextureMipSize.x));
					if (iCurrentX < 0)
						iCurrentX = 0;
					double fStartX = oImageStart.x - oUv0.x * (fStepX * (double)oTextureMipSize.x);
					while (fStartX < oBB.Min.x)
					{
						fStartX += fStepX;
						iCurrentX++;
					}

					if (oDisplayOptions.bTiling && (fStartX  - fStepX ) > oImageStart.x )
					{
						while ((fStartX - fStepX) > oImageStart.x)
						{
							fStartX -= fStepX;
							iCurrentX--;
						}
					}

					ImVec2 oEnd = ImVec2(Math::Min(oImageEnd.x, oBB.Max.x) + 0.5f, Math::Min(oImageEnd.y, oBB.Max.y) + 0.5f);

					for (; fStartX <= oEnd.x; fStartX += fStepX, iCurrentX++)
					{
						bool bIsBlock = oPixelFormatInfo.iBlockWidth > 1 && (Math::Abs(iCurrentX) % oPixelFormatInfo.iBlockWidth) == 0;
						ImVec2 oLineStart = ImVec2((float)fStartX, oImageStart.y);
						ImVec2 oLineEnd = ImVec2((float)fStartX, oImageEnd.y);
						pDrawList->ChannelsSetCurrent(0);
						pDrawList->AddLine(oLineStart, oLineEnd, iColorBack, bIsBlock ? 5.f : 3.f);
						pDrawList->ChannelsSetCurrent(1);
						pDrawList->AddLine(oLineStart, oLineEnd, bIsBlock ? iColorBlock : iColorPixel, bIsBlock ? 2.f : 1.f);
					}

					// Horizontal lines
					int iCurrentY = floor(oUv0.y / (fStepY * (double)oTextureMipSize.y));
					if (iCurrentY < 0)
						iCurrentY = 0;
					double fStartY = oImageStart.y - oUv0.y * fStepY * oTextureMipSize.y;
					while (fStartY < oBB.Min.y)
					{
						fStartY += fStepY;
						iCurrentY++;
					}

					if (oDisplayOptions.bTiling && (fStartY - fStepY) > oImageStart.y)
					{
						while ((fStartY - fStepY) > oImageStart.y)
						{
							fStartY -= fStepY;
							iCurrentY--;
						}
					}

					for (; fStartY <= oEnd.y; fStartY += fStepY, iCurrentY++)
					{
						bool bIsBlock = oPixelFormatInfo.iBlockHeight > 1 && (Math::Abs(iCurrentY) % oPixelFormatInfo.iBlockHeight) == 0;
						ImVec2 oLineStart = ImVec2(oImageStart.x, (float)fStartY);
						ImVec2 oLineEnd = ImVec2(oImageEnd.x, (float)fStartY);
						pDrawList->ChannelsSetCurrent(0);
						pDrawList->AddLine(oLineStart, oLineEnd, iColorBack, bIsBlock ? 5.f : 3.f);
						pDrawList->ChannelsSetCurrent(1);
						pDrawList->AddLine(oLineStart, oLineEnd, bIsBlock ? iColorBlock : iColorPixel, bIsBlock ? 2.f : 1.f);
					}

					pDrawList->ChannelsMerge();
				}
			}

			if (ImGui::IsItemHovered())
			{
				if (oIO.MouseWheel != 0.f)
				{
					float fWheel = oIO.MouseWheel;
					if (!oIO.KeyCtrl)
					{
						ImVec2 oNew = (oCursorPos - m_oOffset) / (float)m_fZoom;

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

						m_oOffset = oCursorPos - oNew * (float)m_fZoom;
					}
				}

				if (ImGui::GetIO().KeyShift)
				{
					const float c_fPixelSize = 20.f;
					const int c_iPixelAroundCount = 4;

					int oHoveredPixel[2] = { (int)roundf(oCursorMipCoord.x - 0.5f), (int)roundf(oCursorMipCoord.y - 0.5f) };

					if (oDisplayOptions.bTiling || (oHoveredPixel[0] >= 0.f && oHoveredPixel[0] <= oTextureSize.x &&
						oHoveredPixel[1] >= 0.f && oHoveredPixel[1] <= oTextureSize.y))
					{
						ImGui::BeginTooltip();

						//Display hovered pixel
						{
							ImGui::Dummy(ImVec2(c_fPixelSize * (c_iPixelAroundCount * 2 + 1), c_fPixelSize * (c_iPixelAroundCount * 2 + 1)));
							ImVec2 oPixelsMin = ImGui::GetItemRectMin();
							ImVec2 oPixelsMax = ImGui::GetItemRectMax();
							ImVec2 oPixelsSize = oPixelsMax - oPixelsMin;

							ImDrawList* pTooltipDrawList = ImGui::GetWindowDrawList();
							pTooltipDrawList->AddImage((ImTextureID)m_pCheckboardTexture2DRes->GetTextureView(),
								oPixelsMin, oPixelsMax,
								ImVec2(0.f, 0.f), ImVec2((c_iPixelAroundCount * 2 + 1) * 2.f, (c_iPixelAroundCount * 2 + 1) * 2.f)
							);

							ImVec2 oPixelPos((float)oHoveredPixel[0] + 1, (float)oHoveredPixel[1] + 1);
							ImVec2 oUvMin = (oPixelPos - ImVec2((float)c_iPixelAroundCount + 1, (float)c_iPixelAroundCount + 1)) / oTextureMipSize;
							ImVec2 oUvMax = (oPixelPos + ImVec2((float)c_iPixelAroundCount , (float)c_iPixelAroundCount)) / oTextureMipSize;

							ImGuiUtils::PushPixelShader(m_pPixelShader);
							ImGuiUtils::PushSampler((oDisplayOptions.bTiling ? m_pSamplerStatePointRepeat : m_pSamplerStatePointClamp));
							ImGuiUtils::PushPixelShaderConstantBuffer(m_pGlobalConstantBuffer);

							pTooltipDrawList->AddImage((ImTextureID)pTexture2DRes->GetTextureView(),
								oPixelsMin, oPixelsMax,
								oUvMin, oUvMax
							);

							ImGuiUtils::PopPixelShaderConstantBuffer();
							ImGuiUtils::PopSampler();
							ImGuiUtils::PopPixelShader();

							ImVec2 oPixelMin = oPixelsMin + oPixelsSize / (c_iPixelAroundCount * 2 + 1) * c_iPixelAroundCount;
							ImVec2 oPixelMax = oPixelsMax - oPixelsSize / (c_iPixelAroundCount * 2 + 1) * c_iPixelAroundCount;
							pTooltipDrawList->AddRect(oPixelMin, oPixelMax, ImGui::GetColorU32(ImVec4(1.f, 0.f, 0.f, 1.f)), 0.f, 15, 2.f);
						}

						const Fonts& oFonts = Program::GetInstance()->GetFonts();

						//Display Pixel position
						{
							int iX = oHoveredPixel[0] % oTexture.GetWidth();
							int iY = oHoveredPixel[1] % oTexture.GetHeight();
							while (iX < 0) iX += oTexture.GetWidth();
							while (iY < 0) iY += oTexture.GetHeight();

							ImGui::PushFont(oFonts.pFontConsolasBold);
							ImGui::TextUnformatted("X:");
							ImGui::PopFont();
							ImGui::SameLine();
							ImGui::PushFont(oFonts.pFontConsolas);
							ImGui::Text("%d", iX);
							ImGui::PopFont();
							ImGui::SameLine(0.f, 10.f);
							ImGui::PushFont(oFonts.pFontConsolasBold);
							ImGui::TextUnformatted(" Y:");
							ImGui::PopFont();
							ImGui::SameLine();
							ImGui::PushFont(oFonts.pFontConsolas);
							ImGui::Text("%d", iY);
							ImGui::PopFont();
						}

						ImGui::EndTooltip();
					}
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
		}
	}

	void WorkAreaWindow::OnDropFiles(int iCount, char** pFiles, const ImVec2& /*oPos*/)
	{
		if (iCount == 1)
		{
			Program::GetInstance()->LoadFile(pFiles[0]);
		}
	}
}
//namespace Windows