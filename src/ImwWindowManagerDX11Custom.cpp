#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImwWindowManagerDX11Custom.h"
using namespace ImWindow;


#include "Core/MemoryStream.h"

#include "Graphics/TextureUtils.h"
#include "GraphicResources/Texture2D.h"

#include "Resources/Icons/Default_24_png.h"

ImwWindowManagerDX11Custom::ImwWindowManagerDX11Custom(bool bCustomFrame)
	: ImwWindowManagerDX11(bCustomFrame)
	, m_pIcon(NULL)
{
}

ImwWindowManagerDX11Custom::~ImwWindowManagerDX11Custom()
{
}

bool ImwWindowManagerDX11Custom::InternalInit()
{
	if (ImwWindowManagerDX11::InternalInit() == false)
		return false;

	//Load Image
	Graphics::Texture oTexture;
	Core::MemoryStream oMemStream(Resources::Icons::Default_24_png::Data, Resources::Icons::Default_24_png::Size);
	CORE_VERIFY(Graphics::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
	if (oTexture.IsValid())
	{
		GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIcon);
	}
	return true;
}

void ImwWindowManagerDX11Custom::InternalDestroy()
{
	if (m_pIcon)
	{
		delete m_pIcon;
	}
	ImwWindowManagerDX11::InternalDestroy();
}

const float c_fIconSize = 24.f;
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
	/*ImGui::Dummy(ImVec2(c_fIconSize, c_fIconSize));
	ImRect oRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	const ImVec2 oCenter = oRect.GetCenter();
	const ImVec2 oSize = oRect.GetSize();
	const ImVec2 c_oSpace = ImVec2(1.f, 1.f);

	ImU32 iColor = ImGui::GetColorU32(ImGuiCol_Text);
	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	pDrawList->AddRectFilled(oRect.Min, oCenter - c_oSpace, iColor);
	pDrawList->AddRectFilled(oCenter + c_oSpace, oRect.Max, iColor);
	pDrawList->AddLine(oRect.GetBL(), oRect.GetTR(), iColor);*/

	if (m_pIcon)
	{
		ImGui::Image((ImTextureID)m_pIcon->GetTextureView(), ImVec2(c_fIconSize, c_fIconSize));
		ImGui::SameLine();
	}

	ImwWindowManager::PaintTitleBar(pPlatformWindow);
}
