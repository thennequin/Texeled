#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGuiUtils.h"

#include "Program.h"
#include "imgui.h"

#include "Math/Arithmetic.h"

#include "GraphicResources/SamplerState.h"

#include <d3d11.h>


namespace ImGuiUtils
{
	const int					c_iMaxPushedSampler = 32;
	ID3D11SamplerState*			s_oPushedSampler[c_iMaxPushedSampler];
	int							s_iCurrentSamplerIndex = 0;

	void PushSamplerCallback(const ImDrawList* /*pDrawList*/, const ImDrawCmd* pCmd)
	{
		CORE_ASSERT(s_iCurrentSamplerIndex < (c_iMaxPushedSampler - 1));
		Program::GetInstance()->GetDX11DeviceContext()->PSGetSamplers(
			0, 1,
			&s_oPushedSampler[s_iCurrentSamplerIndex]
		);
		++s_iCurrentSamplerIndex;

		ID3D11SamplerState* pResource = (ID3D11SamplerState*)pCmd->UserCallbackData;
		Program::GetInstance()->GetDX11DeviceContext()->PSSetSamplers(0, 1, &pResource);
	}

	void PopSamplerCallback(const ImDrawList* /*pDrawList*/, const ImDrawCmd* /*pCmd*/)
	{
		CORE_ASSERT(s_iCurrentSamplerIndex > 0);
		--s_iCurrentSamplerIndex;
		ID3D11SamplerState* pBackupedSampler = s_oPushedSampler[s_iCurrentSamplerIndex];
		Program::GetInstance()->GetDX11DeviceContext()->PSSetSamplers(0, 1, &pBackupedSampler);
	}

	void PushSampler(GraphicResources::SamplerState* pSampler)
	{
		CORE_ASSERT(pSampler != NULL);
		ImGui::GetWindowDrawList()->AddCallback(PushSamplerCallback, pSampler->GetResource());
	}

	void PopSampler()
	{
		ImGui::GetWindowDrawList()->AddCallback(PopSamplerCallback, NULL);
	}


	const int					c_iMaxPushedPixelShader = 32;
	ID3D11PixelShader*			s_oPushedPixelShader[c_iMaxPushedPixelShader];
	ID3D11ClassInstance*		s_oPushedPixelShaderInstances[c_iMaxPushedPixelShader][256];   // 256 is max according to PSSetShader documentation
	UINT						s_oPushedPixelShaderInstanceCount[c_iMaxPushedPixelShader];
	int							s_iCurrentPixelShaderIndex = 0;

	void PushPixelShaderCallback(const ImDrawList* /*pDrawList*/, const ImDrawCmd* pCmd)
	{
		CORE_ASSERT(s_iCurrentPixelShaderIndex < (c_iMaxPushedSampler - 1));
		Program::GetInstance()->GetDX11DeviceContext()->PSGetShader(
			&s_oPushedPixelShader[s_iCurrentPixelShaderIndex],
			s_oPushedPixelShaderInstances[s_iCurrentPixelShaderIndex],
			&s_oPushedPixelShaderInstanceCount[s_iCurrentPixelShaderIndex]
		);
		++s_iCurrentPixelShaderIndex;

		ID3D11PixelShader* pShader = (ID3D11PixelShader*)pCmd->UserCallbackData;
		Program::GetInstance()->GetDX11DeviceContext()->PSSetShader(pShader, NULL, 0);
	}

	void PopPixelShaderCallback(const ImDrawList* /*pDrawList*/, const ImDrawCmd* /*pCmd*/)
	{
		CORE_ASSERT(s_iCurrentPixelShaderIndex > 0);
		--s_iCurrentPixelShaderIndex;
		Program::GetInstance()->GetDX11DeviceContext()->PSSetShader(
			s_oPushedPixelShader[s_iCurrentPixelShaderIndex],
			s_oPushedPixelShaderInstances[s_iCurrentPixelShaderIndex],
			s_oPushedPixelShaderInstanceCount[s_iCurrentPixelShaderIndex]
		);
	}

	void PushPixelShader(ID3D11PixelShader* pShader)
	{
		CORE_ASSERT(pShader != NULL);
		ImGui::GetWindowDrawList()->AddCallback(PushPixelShaderCallback, pShader);
	}

	void PopPixelShader()
	{
		ImGui::GetWindowDrawList()->AddCallback(PopPixelShaderCallback, NULL);
	}


	const int					c_iMaxPushedPixelShaderConstantBuffer = 32;
	ID3D11Buffer*				s_oPushedPixelShaderConstantBuffer[c_iMaxPushedPixelShaderConstantBuffer];
	int							s_iCurrentPixelShaderConstantBufferIndex = 0;

	void PushPixelShaderConstantBufferCallback(const ImDrawList* /*pDrawList*/, const ImDrawCmd* pCmd)
	{
		CORE_ASSERT(s_iCurrentPixelShaderConstantBufferIndex < (c_iMaxPushedSampler - 1));
		Program::GetInstance()->GetDX11DeviceContext()->PSGetConstantBuffers(
			0,
			1,
			&s_oPushedPixelShaderConstantBuffer[s_iCurrentPixelShaderConstantBufferIndex]
		);
		++s_iCurrentPixelShaderConstantBufferIndex;

		ID3D11Buffer* pBuffer = (ID3D11Buffer*)pCmd->UserCallbackData;
		Program::GetInstance()->GetDX11DeviceContext()->PSSetConstantBuffers(
			0,
			1,
			&pBuffer
		);
	}

	void PopPixelShaderConstantBufferCallback(const ImDrawList* /*pDrawList*/, const ImDrawCmd* /*pCmd*/)
	{
		CORE_ASSERT(s_iCurrentPixelShaderConstantBufferIndex > 0);
		--s_iCurrentPixelShaderConstantBufferIndex;
		Program::GetInstance()->GetDX11DeviceContext()->PSSetConstantBuffers(
			0,
			1,
			&s_oPushedPixelShaderConstantBuffer[s_iCurrentPixelShaderConstantBufferIndex]
		);
	}

	void PushPixelShaderConstantBuffer(ID3D11Buffer* pBuffer)
	{
		CORE_ASSERT(pBuffer != NULL);
		ImGui::GetWindowDrawList()->AddCallback(PushPixelShaderConstantBufferCallback, pBuffer);
	}

	void PopPixelShaderConstantBuffer()
	{
		ImGui::GetWindowDrawList()->AddCallback(PopPixelShaderConstantBufferCallback, NULL);
	}

	bool GroupedButton( const char* label, bool bSelected, GroupedPartEnum ePart)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if( window->SkipItems )
			return false;

		int flags = 0;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID( label );
		const ImVec2 label_size = ImGui::CalcTextSize( label, NULL, true );

		ImVec2 pos = window->DC.CursorPos;
		if( ( flags & ImGuiButtonFlags_AlignTextBaseLine ) && style.FramePadding.y < window->DC.CurrentLineTextBaseOffset ) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
			pos.y += window->DC.CurrentLineTextBaseOffset - style.FramePadding.y;
		ImVec2 size = ImGui::CalcItemSize( ImVec2(0.f, 0.f) , label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f );

		const ImRect bb( pos, pos + size );
		ImGui::ItemSize( bb, style.FramePadding.y );
		if( !ImGui::ItemAdd( bb, id ) )
			return false;

		if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat) flags |= ImGuiButtonFlags_Repeat;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, flags );

		// Render
		const ImU32 col = ImGui::GetColorU32( ( bSelected || (hovered && held) ) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button );

		int iRoundingFlag = 0;
		if( ePart == GroupedPart::LEFT )
		{
			iRoundingFlag = ImDrawCornerFlags_Left;
		}
		else if( ePart == GroupedPartEnum::RIGHT )
		{
			iRoundingFlag = ImDrawCornerFlags_Right;
		}

		window->DrawList->AddRectFilled( bb.Min, bb.Max, col, style.FrameRounding, iRoundingFlag );
		window->DrawList->AddRect( bb.Min + ImVec2( 1, 1 ), bb.Max + ImVec2( 1, 1 ), ImGui::GetColorU32( ImGuiCol_BorderShadow ), style.FrameRounding, iRoundingFlag );
		window->DrawList->AddRect( bb.Min, bb.Max, ImGui::GetColorU32( ImGuiCol_Border ), style.FrameRounding, iRoundingFlag );

		ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb );

		return pressed;
	}

	void TextPathWrapped(const char* pPath)
	{
		const float fMaxWidth = ImGui::GetContentRegionAvailWidth() - 1.f;
		const float fPathSize = ImGui::CalcTextSize(pPath, NULL).x;


		if (fPathSize > fMaxWidth)
		{
			const float fDotSize = ImGui::CalcTextSize("...", NULL).x;

			const char* pLastDelimiter = Math::Max(strrchr(pPath, '/'), strrchr(pPath, '\\'));
			const float fNameSize = ImGui::CalcTextSize(pLastDelimiter, NULL).x;

			const float fMaxPathSize = fMaxWidth - fDotSize - fNameSize;
			const char* pEnd = pLastDelimiter - 1;
			while (ImGui::CalcTextSize(pPath, pEnd).x > fMaxPathSize && pEnd > pPath)
			{
				pEnd--;
			}

			ImGui::BeginGroup();

			if (pPath != pEnd)
			{
				ImGui::TextUnformatted(pPath, pEnd);
				ImGui::SameLine(0.f, 0.f);
				ImGui::TextUnformatted("...\\");
				ImGui::SameLine(0.f, 0.f);
			}
			ImGui::TextUnformatted(pLastDelimiter + 1);

			ImGui::EndGroup();
		}
		else
		{
			ImGui::TextUnformatted(pPath);
		}
	}

	void Link(const char* pUrl)
	{
		ImVec4 oColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_Text, oColor);

		ImGui::TextUnformatted(pUrl);

		bool bHovered = ImGui::IsItemHovered();

		if (bHovered)
		{
			if (ImGui::IsMouseClicked(0))
			{
				//TODO : change of to open URL
				ShellExecute(NULL, NULL, pUrl, NULL, NULL, SW_SHOWNORMAL);
			}
		}
		else
		{
			ImVec2 vMin = ImGui::GetItemRectMin();
			ImVec2 vMax = ImGui::GetItemRectMax();
			vMin.y = vMax.y;
			ImGui::GetWindowDrawList()->AddLine(vMin, vMax, ImGui::ColorConvertFloat4ToU32(oColor), 1.0f);
		}

		ImGui::PopStyleColor();
	}

	const ImVec2 c_vMenuItemIconSize(16.f, 16.f);

	bool BeginMenu(const char* pLabel, bool bEnabled)
	{
		ImGui::Dummy(c_vMenuItemIconSize);
		ImGui::SameLine();
		bool bOpen = ImGui::BeginMenu(pLabel, bEnabled);

		return bOpen;
	}

	bool MenuItemPlus(const char* pLabel, ImFont* pLabelFont, const char* pShortcut, ImFont* pShortkeyFont, bool bSelected, bool bEnabled, ImTextureID pIcon, ImVec4 vIconColor)
	{
		ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
		if (pWindow->SkipItems)
			return false;

		ImGuiContext& oContext = *GImGui;
		ImVec2 vPos = pWindow->DC.CursorPos;
		ImGui::PushFont(pLabelFont);
		ImVec2 vLabelSize = ImGui::CalcTextSize(pLabel, NULL, true);
		ImGui::PopFont();
		ImGui::PushFont(pShortkeyFont);
		ImVec2 vShortcutSize = pShortcut ? ImGui::CalcTextSize(pShortcut, NULL) : ImVec2(0.0f, 0.0f);
		ImGui::PopFont();
		float fWidth = pWindow->MenuColumns.DeclColumns(c_vMenuItemIconSize.x + oContext.Style.ItemSpacing.x + vLabelSize.x, vShortcutSize.x, 0.f/*, (float)(int)(oContext.FontSize * 1.20f)*/); // Feedback for next frame
		float fExtraWidth = ImMax(0.0f, ImGui::GetContentRegionAvail().x - fWidth);

		ImGui::PushFont(pLabelFont);
		ImGui::PushID(pLabel);
		bool bPressed = ImGui::Selectable("##MenuItem", false, ImGuiSelectableFlags_MenuItem | ImGuiSelectableFlags_DrawFillAvailWidth | (bEnabled ? 0 : ImGuiSelectableFlags_Disabled), ImVec2(fWidth, 0.0f));
		bool bHovered = ImGui::IsItemHovered();
		ImGui::PopID();
		ImGui::PopFont();

		if (pIcon != NULL)
		{
			if (bEnabled == false)
			{
				vIconColor.w *= 0.5f;
			}
			pWindow->DrawList->AddImage(pIcon, vPos, vPos + c_vMenuItemIconSize, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImGui::GetColorU32(vIconColor));
		}

		if (bEnabled == false)
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
		ImGui::RenderText(vPos + ImVec2(pWindow->MenuColumns.Pos[0] + c_vMenuItemIconSize.x + oContext.Style.ItemSpacing.x, 0.0f), pLabel, NULL, false);
		if (bEnabled == false)
			ImGui::PopStyleColor();

		if (vShortcutSize.x > 0.0f)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, oContext.Style.Colors[ImGuiCol_TextDisabled]);
			ImGui::PushFont(pShortkeyFont);
			float fColSize = pWindow->MenuColumns.Pos[2] - pWindow->MenuColumns.Pos[1];
			ImGui::RenderText(vPos + ImVec2(pWindow->MenuColumns.Pos[1] + fExtraWidth + (fColSize - vShortcutSize.x), 0.0f), pShortcut, NULL, false);
			ImGui::PopFont();
			ImGui::PopStyleColor();
		}

		if (bSelected)
			ImGui::RenderCheckMark(vPos + ImVec2(pWindow->MenuColumns.Pos[2] + fExtraWidth + oContext.FontSize * 0.20f, 0.0f), ImGui::GetColorU32(bEnabled ? ImGuiCol_Text : ImGuiCol_TextDisabled), oContext.FontSize);

		return bPressed;
	}
}