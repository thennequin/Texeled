#include "Menus.h"

#include "Program.h"
#include "Graphics/Texture.h"

void Menus::OnMenu()
{
	Graphics::Texture& oTexture = Program::GetInstance()->GetTexture();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open"))
		{
			Program::GetInstance()->Open();
		}
		if (ImGui::MenuItem("Save", NULL, false, oTexture.IsValid()))
		{
			Program::GetInstance()->Save();
		}
		if (ImGui::MenuItem("Save as", NULL, false, oTexture.IsValid()))
		{
			Program::GetInstance()->SaveAs();

		}
		ImGui::EndMenu();
	}
}