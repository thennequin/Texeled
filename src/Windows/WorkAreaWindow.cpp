
#define IMGUI_DEFINE_MATH_OPERATORS
#include "WorkAreaWindow.h"

#include "Program.h"

#include "GraphicResources/Texture2D.h"
#include "GraphicResources/SamplerState.h"

#include "ImGuiUtils.h"

namespace Windows
{
	WorkAreaWindow::WorkAreaWindow()
		: m_pCheckboardTexture2DRes(NULL)
	{
		SetTitle("WorkArea");
		SetAlone(true);

		m_bTiling = false;
		m_bShowPixelGrid = true;
		m_fZoom = 1.0;
		m_oOffset = ImVec2(0.f, 0.f);

		m_fCurrentZoom = m_fZoom;
		m_oCurrentOffset = m_oOffset;

		Graphics::Texture::Desc oCheckerboardDesc;
		oCheckerboardDesc.ePixelFormat = Graphics::E_PIXELFORMAT_RGBA8_UNORM;
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

		m_pSamplerStatePoint = new GraphicResources::SamplerState();
		GraphicResources::SamplerState::Desc oSamplerDesc;
		oSamplerDesc.eFilter = GraphicResources::SamplerFilter::NEAREST_MIPMAP_LINEAR;
		oSamplerDesc.eWrapU = GraphicResources::Wrap::REPEAT;
		oSamplerDesc.eWrapV = GraphicResources::Wrap::REPEAT;
		oSamplerDesc.eWrapW = GraphicResources::Wrap::REPEAT;
		CORE_VERIFY_OK(m_pSamplerStatePoint->Create(oSamplerDesc));
	}

	WorkAreaWindow::~WorkAreaWindow()
	{
		ImwSafeDelete(m_pCheckboardTexture2DRes);
		ImwSafeDelete(m_pSamplerStatePoint);
	}

	void WorkAreaWindow::OnGui()
	{
		ImGuiIO& oIO = ImGui::GetIO();

		ImGui::SameLine(0.f, 10.f);
		ImGui::Checkbox("Show pixel grid", &m_bShowPixelGrid);

		ImGui::SameLine(0.f, 10.f);
		ImGui::Checkbox("Tiling", &m_bTiling);

		const int iCurrentMip = 0;

		Graphics::Texture& oTexture = Program::GetInstance()->GetTexture();
		GraphicResources::Texture2D* pTexture2DRes = Program::GetInstance()->GetTexture2DRes();

		ImGui::Separator();
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
			if (!m_bTiling)
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
				/*
				ImWindow::ImwPlatformWindowSokol::SetPipeline(m_hPipeline);
				ImWindow::ImwPlatformWindowSokol::SetUniformBlock(SG_SHADERSTAGE_FS, 0, &m_oUniformBlock, sizeof(m_oUniformBlock));
				*/

				ImGuiUtils::PushSampler(m_pSamplerStatePoint);
				pDrawList->AddImage((ImTextureID)pTexture2DRes->GetTextureView(),
					oImageStart, oImageEnd,
					oUv0, oUv1);
				ImGuiUtils::PopSampler();

				/*
				ImWindow::ImwPlatformWindowSokol::ReleaseUniformBlock(SG_SHADERSTAGE_FS, 0);
				ImWindow::ImwPlatformWindowSokol::RestorePipeline();
				*/

				if (m_bShowPixelGrid && (fMipPixelRatio*m_fCurrentZoom) > 10.0)
				{
					double fStep = fMipPixelRatio * m_fCurrentZoom;
					ImU32 iColor = ImGui::GetColorU32(ImVec4(0.4f, 0.4f, 0.4f, 1.f));
					//Draw pixel grid

					double fStart;

					fStart = oImageStart.x - oUv0.x * fStep * oTextureMipSize.x;
					while (fStart < oAreaMin.x)
						fStart += fStep;

					while (m_bTiling && fStart > oImageStart.y - fStep)
						fStart -= fStep;

					for (; fStart <= oImageEnd.x && fStart <= oAreaMax.x; fStart += fStep)
					{
						pDrawList->AddLine(ImVec2(fStart, oImageStart.y), ImVec2(fStart, oImageEnd.y), iColor);
					}

					fStart = oImageStart.y - oUv0.y * fStep * oTextureMipSize.y;
					while (fStart < oAreaMin.y)
						fStart += fStep;

					while (m_bTiling && fStart > oImageStart.y - fStep)
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

			//Display image informations
			{
				char pInformations[2048];
				snprintf(pInformations, 2048,
					"Width: %d\n"
					"Height: %d\n"
					"Components: %d\n"
					"Pixel format: %s",
					oTexture.GetWidth(),
					oTexture.GetHeight(),
					Graphics::PixelFormat::ComponentCount(oTexture.GetPixelFormat()),
					Graphics::EPixelFormat_string[oTexture.GetPixelFormat()]
				);

				ImVec2 vInformationSize = ImGui::CalcTextSize(pInformations);
				const ImVec2& vPadding = ImGui::GetStyle().FramePadding;

				if (vInformationSize.x > 0)
				{
					pDrawList->AddRectFilled(
						ImVec2(oAreaMin.x + vPadding.x, oAreaMax.y - vPadding.y * 3.f - vInformationSize.y),
						ImVec2(oAreaMin.x + vPadding.x * 3 + vInformationSize.x, oAreaMax.y - vPadding.y),
						ImGui::GetColorU32(ImGuiCol_PopupBg),
						5.f);

					pDrawList->AddText(
						ImVec2(oAreaMin.x + vPadding.x * 2.f, oAreaMax.y - vPadding.y * 2.f - vInformationSize.y),
						ImGui::GetColorU32(ImGuiCol_Text),
						pInformations);
				}
			}
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