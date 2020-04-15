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
}