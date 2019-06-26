#include "Windows/AboutWindow.h"

#include "Program.h"

#include "Core/MemoryStream.h"

#include "Graphics/TextureLoader.h"
#include "GraphicResources/Texture2D.h"

#include "Resources/Icons/Default_png.h"

#include "Version.h"

namespace Windows
{
	AboutWindow::AboutWindow()
		: m_pIcon(NULL)
	{
		SetAlone(true);
		SetTitle("About");

		//Load Image
		Graphics::Texture oTexture;
		Core::MemoryStream oMemStream(Resources::Icons::Default_png::Data, Resources::Icons::Default_png::Size);
		CORE_VERIFY(Graphics::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIcon));
		}
	}

	AboutWindow::~AboutWindow()
	{
		if (m_pIcon)
		{
			delete m_pIcon;
			m_pIcon = NULL;
		}
	}

	void AboutWindow::OnGui()
	{
		const Fonts& oFonts = Program::GetInstance()->GetFonts();

		if (m_pIcon != NULL)
		{
			ImGui::Image((ImTextureID)m_pIcon->GetTextureView(), ImVec2(128.f, 128.f));
		
			ImGui::SameLine();
		}
		
		ImGui::BeginGroup();
		{
			ImGui::TextUnformatted("Texeled");
			ImGui::Separator();
		
			ImGui::PushFont(oFonts.pFontConsolas);
			ImGui::TextUnformatted("Version: ");
			ImGui::PopFont();
			ImGui::Text("%d.%d", TEXELED_VERSION_MAJOR, TEXELED_VERSION_MINOR);

			ImGui::PushFont(oFonts.pFontConsolas);
			ImGui::TextUnformatted("Build: ");
			ImGui::PopFont();
			ImGui::Text("%d", TEXELED_BUILD);

			if (TEXELED_REVISION != NULL)
			{
				ImGui::PushFont(oFonts.pFontConsolas);
				ImGui::TextUnformatted("Revision: ");
				ImGui::PopFont();
				ImGui::TextUnformatted(TEXELED_REVISION);
			}
		}
		ImGui::EndGroup();
	}
}