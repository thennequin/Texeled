#include "Windows/LoggerWindow.h"

#include "Program.h"

namespace Windows
{
	LoggerWindow::LoggerWindow()
	{
		SetTitle("Log");
	}

	LoggerWindow::~LoggerWindow()
	{
	}

	void LoggerWindow::OnGui()
	{
		Program* pProgram = Program::GetInstance();
		const Logs& oLogs = pProgram->GetLogs();

		if (ImGui::Button("Clear"))
		{
			pProgram->ClearLogs();
		}

		ImGui::Separator();
		if (ImGui::BeginChild("scrolling"))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 1.f));

			ImGui::Columns(3);
			ImGuiWindow* pWindow = ImGui::GetCurrentWindowRead();
			ImGuiColumnsSet* pColumns = pWindow->DC.ColumnsSet;
			if (pColumns->IsFirstFrame)
			{
				ImGui::SetColumnWidth(0, 75.f);
				ImGui::SetColumnWidth(1, 150.f);
			}

			for (size_t iLog = 0, iLogCount = oLogs.size(); iLog < iLogCount; ++iLog)
			{
				const Log& oLog = oLogs[iLog];
				ImVec4 oColor;
				const char* pCategory;
				switch (oLog.eCategory)
				{
				case Core::Logger::Category::Debug:
					pCategory = "Debug";
					oColor = ImVec4(0.5f, 0.5f, 0.5f, 1.f);
					break;
				default:
				case Core::Logger::Category::Info:
					pCategory = "Info";
					oColor = ImVec4(1.f, 1.f, 1.f, 1.f);
					break;
				case Core::Logger::Category::Warning:
					pCategory = "Warning";
					oColor = ImVec4(1.f, 0.5f, 0.f, 1.f);
					break;
				case Core::Logger::Category::Error:
					pCategory = "Error";
					oColor = ImVec4(0.8f, 0.22f, 0.f, 1.f);
					break;
				}

				ImGui::PushStyleColor(ImGuiCol_Text, oColor);
				ImGui::TextUnformatted(pCategory);
				ImGui::NextColumn();
				ImGui::TextUnformatted(oLog.pName);
				ImGui::NextColumn();
				ImGui::TextUnformatted(oLog.pMessage);
				ImGui::NextColumn();
				ImGui::PopStyleColor();
			}
			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}
}