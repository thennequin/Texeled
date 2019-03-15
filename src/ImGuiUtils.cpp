#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGuiUtils.h"

#include "Program.h"
#include "imgui.h"

#include "GraphicResources/SamplerState.h"

#include <d3d11.h>


namespace ImGuiUtils
{
	const int c_iMaxPushedSampler = 32;
	ID3D11SamplerState* s_oPushedSampler[c_iMaxPushedSampler];
	int s_iCurrentSamplerIndex = 0;
	void PushSamplerCallback(const ImDrawList* pDrawList, const ImDrawCmd* pCmd)
	{
		ID3D11SamplerState* pBackupedSampler;
		Program::GetInstance()->GetDX11DeviceContext()->PSGetSamplers(0, 1, &pBackupedSampler);
		s_oPushedSampler[s_iCurrentSamplerIndex] = pBackupedSampler;
		++s_iCurrentSamplerIndex;

		ID3D11SamplerState* pResource = (ID3D11SamplerState*)pCmd->UserCallbackData;
		Program::GetInstance()->GetDX11DeviceContext()->PSSetSamplers(0, 1, &pResource);
	}

	void PopSamplerCallback(const ImDrawList* pDrawList, const ImDrawCmd* /*pCmd*/)
	{
		--s_iCurrentSamplerIndex;
		ID3D11SamplerState* pBackupedSampler = s_oPushedSampler[s_iCurrentSamplerIndex];
		Program::GetInstance()->GetDX11DeviceContext()->PSSetSamplers(0, 1, &pBackupedSampler);
		s_oPushedSampler[s_iCurrentSamplerIndex] = NULL;
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
		if( !ImGui::ItemAdd( bb, &id ) )
			return false;

		
		if( window->DC.ButtonRepeat ) flags |= ImGuiButtonFlags_Repeat;
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, flags );

		// Render
		const ImU32 col = ImGui::GetColorU32( ( hovered && held ) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button );
		
		int iRoundingFlag = 0;
		if( ePart == GroupedPart::LEFT )
		{
			iRoundingFlag = ImGuiCorner_TopLeft | ImGuiCorner_BottomLeft;
		}
		else if( ePart == GroupedPartEnum::RIGHT )
		{
			iRoundingFlag = ImGuiCorner_TopRight | ImGuiCorner_BottomRight;
		}

		window->DrawList->AddRectFilled( bb.Min, bb.Max, col, style.FrameRounding, iRoundingFlag );
		window->DrawList->AddRect( bb.Min + ImVec2( 1, 1 ), bb.Max + ImVec2( 1, 1 ), ImGui::GetColorU32( ImGuiCol_BorderShadow ), style.FrameRounding, iRoundingFlag );
		window->DrawList->AddRect( bb.Min, bb.Max, ImGui::GetColorU32( ImGuiCol_Border ), style.FrameRounding, iRoundingFlag );

		ImGui::RenderTextClipped( bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb );

		return pressed;
	}
}