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
}