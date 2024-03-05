#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImwWindowManagerCustom.h"
using namespace ImWindow;

#include "IO/MemoryStream.h"

#include "Texture/TextureLoader.h"
#include "GraphicResources/Texture2D.h"

#include "Resources/Icons/Default_24_png.h"

#include "Program.h"

#include "ImGuiUtils.h"

ImwWindowManagerCustom::ImwWindowManagerCustom(bool bCustomFrame)
	: ImwWindowManagerDX11(bCustomFrame)
	, m_pIcon(NULL)
{
}

ImwWindowManagerCustom::~ImwWindowManagerCustom()
{
	InternalDestroy();
}

bool ImwWindowManagerCustom::InternalInit()
{
	if (ImwWindowManagerDX11::InternalInit() == false)
		return false;

	//Load Image
	Graphics::Texture oTexture;
	IO::MemoryStream oMemStream(Resources::Icons::Default_24_png::Data, Resources::Icons::Default_24_png::Size);
	CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
	if (oTexture.IsValid())
	{
		CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIcon));
	}
	return true;
}

void ImwWindowManagerCustom::InternalDestroy()
{
	if (m_pIcon)
	{
		delete m_pIcon;
		m_pIcon = NULL;
	}
	ImwWindowManagerDX11::InternalDestroy();
}

const float c_fIconSize = 24.f;
float ImwWindowManagerCustom::GetTitleBarHeight() const
{
	ImGuiContext* pContext = m_pMainPlatformWindow->GetContext();
	float fContentSize = pContext->Style.FramePadding.y * 2.f + pContext->FontSize;
	if ((c_fIconSize + 1.f) > fContentSize)
		fContentSize = c_fIconSize + 1.f;
	return pContext->Style.WindowPadding.y + fContentSize;
}

void ImwWindowManagerCustom::PaintTitleBar(ImwPlatformWindow* pPlatformWindow, bool bDrawTitle)
{
	ImGuiStyle& oStyle = ImGui::GetStyle();
	ImVec4 oBackupChildWindowBg = oStyle.Colors[ImGuiCol_ChildWindowBg];
	oStyle.Colors[ImGuiCol_ChildWindowBg].w = 0.f;

	if (ImGui::BeginChild("TitleBar", ImVec2(ImGui::GetContentRegionAvailWidth() - 3.f * 24.f, GetTitleBarHeight()), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs))
	{
		if (m_pIcon)
		{
			ImGui::Image((ImTextureID)m_pIcon->GetTextureView(), ImVec2(c_fIconSize, c_fIconSize));

			if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
			{
				((ImwPlatformWindowCustom*)pPlatformWindow)->m_eHoveredArea = E_PLATFORMWINDOWHOVEREDAREA_MENU;
			}
			ImGui::SameLine();
		}

		const Fonts& oFonts = Program::GetInstance()->GetFonts();

		ImGui::GetCurrentWindow()->DC.CurrentLineTextBaseOffset = (c_fIconSize - ImGui::GetFontSize()) / 2.f;
		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::TextUnformatted(GetMainTitle());
		ImGui::PopFont();
		ImGui::SameLine();

		if (pPlatformWindow->IsMainWindow())
		{
			if( Program::GetInstance()->GetTexture().IsValid() )
			{
				const Core::String& sPath = Program::GetInstance()->GetTextureFilePath();
				const char* pPath = sPath.c_str();
				if(sPath.empty())
				{
					pPath = "< Untitled >";
				}

				ImGui::PushFont( oFonts.pFontConsolas );
				ImGui::TextUnformatted( "-" );
				ImGui::SameLine();

				ImGuiUtils::TextPathWrapped( pPath );

				ImGui::PopFont();
			}
		}
	}
	ImGui::EndChild();
	ImGui::SameLine();
	oStyle.Colors[ImGuiCol_ChildWindowBg] = oBackupChildWindowBg;

	ImwWindowManager::PaintTitleBar(pPlatformWindow, false);
}

void ImwWindowManagerCustom::ClosePlatformWindow(ImwPlatformWindow* pPlatformWindow)
{
	OnClosePlatformWindow(pPlatformWindow);
}