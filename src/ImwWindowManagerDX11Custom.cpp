#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImwWindowManagerDX11Custom.h"
using namespace ImWindow;

ImwWindowManagerDX11Custom::ImwWindowManagerDX11Custom(bool bCustomFrame)
	: ImwWindowManagerDX11( bCustomFrame )
{
}

ImwWindowManagerDX11Custom::~ImwWindowManagerDX11Custom()
{
}

const float c_fIconSize = 20.f;
float ImwWindowManagerDX11Custom::GetTitleBarHeight() const
{
	ImGuiContext* pContext = m_pMainPlatformWindow->GetContext();
	float fContentSize = pContext->Style.FramePadding.y * 2.f + pContext->FontSize;
	if ((c_fIconSize +1.f)> fContentSize)
		fContentSize = c_fIconSize + 1.f;
	return pContext->Style.WindowPadding.y + fContentSize;
}

void ImwWindowManagerDX11Custom::PaintTitleBar(ImwPlatformWindow* pPlatformWindow)
{
	//Draw simple icon in title bar
	ImGui::Dummy(ImVec2(c_fIconSize, c_fIconSize));
	ImRect oRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	const ImVec2 oCenter = oRect.GetCenter();
	const ImVec2 oSize = oRect.GetSize();
	const ImVec2 c_oSpace = ImVec2(1.f, 1.f);

	ImU32 iColor = ImGui::GetColorU32(ImGuiCol_Text);
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	pDrawList->AddRectFilled(oRect.Min, oCenter - c_oSpace, iColor);
	pDrawList->AddRectFilled(oCenter + c_oSpace, oRect.Max, iColor);
	pDrawList->AddLine(oRect.GetBL(), oRect.GetTR(), iColor);

	ImGui::SameLine();
	ImwWindowManager::PaintTitleBar(pPlatformWindow);
}
