#include "Windows/AboutWindow.h"

#include "Program.h"

#include "IO/MemoryStream.h"

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

	#define _STRINGIFY(x) #x
	#define STRINGIFY(x) _STRINGIFY(x)

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
			ImGui::TextUnformatted("Version:");
			ImGui::PopFont();
			ImGui::SameLine();
			ImGui::TextUnformatted(STRINGIFY(TEXELED_VERSION));

			ImGui::PushFont(oFonts.pFontConsolas);
			ImGui::TextUnformatted("Build:");
			ImGui::PopFont();
			ImGui::SameLine();
			ImGui::TextUnformatted(STRINGIFY(TEXELED_BUILD));

#ifdef TEXELED_BRANCH
			ImGui::PushFont(oFonts.pFontConsolas);
			ImGui::TextUnformatted("Branch:");
			ImGui::PopFont();
			ImGui::SameLine();
			ImGui::TextUnformatted(STRINGIFY(TEXELED_BRANCH));
#endif //TEXELED_BRANCH

#ifdef TEXELED_REVISION
			ImGui::PushFont(oFonts.pFontConsolas);
			ImGui::TextUnformatted("Revision:");
			ImGui::PopFont();
			ImGui::SameLine();
			ImGui::TextUnformatted(STRINGIFY(TEXELED_REVISION));
#endif //TEXELED_REVISION
		}
		ImGui::EndGroup();
	}
}