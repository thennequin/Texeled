#include "Toolbar.h"

#include "Program.h"

#include "ImGuiUtils.h"

Toolbar::Toolbar()
{

}

Toolbar::~Toolbar()
{

}

void Toolbar::OnToolBar()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 15.f);
	ProgramModeEnum eMode = Program::GetInstance()->GetMode();
	for (int iMode = 0; iMode < ProgramModeEnum::_COUNT; ++iMode)
	{
		if (iMode > 0)
			ImGui::SameLine(0,0);

		ImGuiUtils::GroupedPartEnum eToolbarMode = ( iMode == 0 ) ? ImGuiUtils::GroupedPartEnum::LEFT : ( iMode == ProgramModeEnum::_LAST ) ? ImGuiUtils::GroupedPartEnum::RIGHT : ImGuiUtils::GroupedPartEnum::MIDDLE;
		if (ImGuiUtils::GroupedButton(ProgramModeEnumStrings[iMode], iMode == eMode, eToolbarMode ))
		{
			Program::GetInstance()->SetMode((ProgramModeEnum)iMode);
		}
	}
	ImGui::PopStyleVar();
}
