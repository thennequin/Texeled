#include "Windows/AboutWindow.h"

#include "Program.h"

#include "IO/MemoryStream.h"

#include "Texture/TextureLoader.h"

#include "GraphicResources/Texture2D.h"

#include "Resources/Icons/Default_png.h"

#include "ImGuiUtils.h"

#include "Version.h"

namespace Windows
{
	AboutWindow::AboutWindow()
		: ImWindow::ImwWindow(ImWindow::E_WINDOW_MODE_ALONE)
		, m_pIcon(NULL)
	{
		SetTitle("About");

		//Load Image
		Graphics::Texture oTexture;
		IO::MemoryStream oMemStream(Resources::Icons::Default_png::Data, Resources::Icons::Default_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
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

			ImGuiUtils::Link("https://github.com/thennequin/Texeled");
		}
		ImGui::EndGroup();

		ImGui::Text("Third parties :");
		ImVec2 vPadding = ImGui::GetStyle().WindowPadding;
		if (ImGui::BeginChild("Third party", ImVec2(-vPadding.x, -vPadding.y), true));
		{
			const ImVec2 vDummySize(1.f, 6.f);

			ImGui::Text("dear imgui, v1.53");
			ImGuiUtils::Link("https://github.com/ocornut/imgui");

			ImGui::Dummy(vDummySize);

			ImGui::Text("ImWindow");
			ImGuiUtils::Link("https://github.com/thennequin/ImWindow");

			ImGui::Dummy(vDummySize);

			ImGui::Text("EasyWindow");
			ImGuiUtils::Link("https://github.com/thennequin/EasyWindow");

			ImGui::Dummy(vDummySize);

			ImGui::Text("stb_image v2.25");
			ImGui::Text("stb_image_write v1.14");
			ImGui::Text("stb_image_resize v0.97");
			ImGuiUtils::Link("https://github.com/nothings/stb");

			ImGui::Dummy(vDummySize);

			ImGui::Text("libpng v1.6.37");
			ImGuiUtils::Link("http://www.libpng.org/pub/png/libpng.html");

			ImGui::Dummy(vDummySize);

			ImGui::Text("zlib v1.2.11");
			ImGuiUtils::Link("https://zlib.net/");

			ImGui::Dummy(vDummySize);

			ImGui::Text("tinyexr");
			ImGuiUtils::Link("https://github.com/syoyo/tinyexr");

			ImGui::Dummy(vDummySize);

			ImGui::Text("CMP_Core from Compressonator 4.0.4855");
			ImGuiUtils::Link("https://github.com/GPUOpen-Tools/Compressonator");
		}
		ImGui::EndChild();
	}
}