#include "StatusBars.h"

#include "Program.h"

#include "Core/StringUtils.h"

#include "Math/Arithmetic.h"

#include "IO/MemoryStream.h"

#include "Graphics/TextureUtils.h"

#include "Windows/LoggerWindow.h"

#include "Resources/Icons/Log_16_png.h"

const uint64_t c_iLogDisplayTime = 5000;
const uint64_t c_iLogDisplayTransitionTime = 150;
const uint64_t c_iLogDisplayHighlightTime = 1000;

StatusBars::StatusBars()
	: m_iLastLogTime(0)
	, m_eLastLogCategory(Core::Logger::Category::Info)
	, m_fLastLogFade( 0.f )
{
	Core::Logger::RegisterLoggerOutputer(this);

	// Load icon Log_16
	{
		Graphics::Texture oTexture;
		IO::MemoryStream oMemStream(Resources::Icons::Log_16_png::Data, Resources::Icons::Log_16_png::Size);
		CORE_VERIFY(Texture::LoadFromStream(&oTexture, &oMemStream) == ErrorCode::Ok);
		if (oTexture.IsValid())
		{
			CORE_VERIFY_OK(GraphicResources::Texture2D::CreateFromTexture(&oTexture, &m_pIconLog));
		}
	}
}

StatusBars::~StatusBars()
{
	if (m_pIconLog != NULL)
	{
		delete m_pIconLog;
		m_pIconLog = NULL;
	}
}

void StatusBars::OnStatusBar()
{
	Program* pProgram = Program::GetInstance();
	const Core::Clock& oClock = pProgram->GetClock();
	const Fonts& oFonts = pProgram->GetFonts();

	const uint64_t iDisplayTime = oClock.GetFrameTime() - m_iLastLogTime;

	if (iDisplayTime <= (c_iLogDisplayTime - c_iLogDisplayTransitionTime) && m_fLastLogFade < 1.f)
	{
		m_fLastLogFade += oClock.GetDeltaTime() / (float)c_iLogDisplayTransitionTime;
		if (m_fLastLogFade > 1.f)
			m_fLastLogFade = 1.f;
	}
	else if (iDisplayTime > (c_iLogDisplayTime - c_iLogDisplayTransitionTime) && m_fLastLogFade > 0.f)
	{
		m_fLastLogFade -= oClock.GetDeltaTime() / (float)c_iLogDisplayTransitionTime;
		if (m_fLastLogFade < 0.f)
			m_fLastLogFade = 0.f;
	}

	const float fCursorY = ImGui::GetCursorPosY();
	float fCursorOffset = ImGui::GetTextLineHeight() * 2.f;

	float fCursorX = ImGui::GetCursorPosX();

	ImGui::BeginGroup();
	ImGui::SetCursorPosY(fCursorY - fCursorOffset * m_fLastLogFade);

	{
		const Graphics::Texture& oTexture = pProgram->GetTexture();
		const int iCurrentMip = pProgram->GetDisplayOptions().iMip;

		//Width
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::TextUnformatted("Width:");
		ImGui::PopFont();

		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		if (oTexture.IsValid())
		{

			if (iCurrentMip > 0)
			{
				ImGui::Text("%d(%d)", oTexture.GetWidth(), Math::Max(1, oTexture.GetWidth() >> iCurrentMip));
			}
			else
			{
				ImGui::Text("%d", oTexture.GetWidth());
			}
		}
		else
		{
			ImGui::Text("-");
		}
		ImGui::PopFont();

		//Height
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::SameLine();
		ImGui::TextUnformatted("Height:");
		ImGui::PopFont();

		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		if (oTexture.IsValid())
		{
			if (iCurrentMip > 0)
			{
				ImGui::Text("%d(%d)", oTexture.GetHeight(), Math::Max(1, oTexture.GetHeight() >> iCurrentMip));
			}
			else
			{
				ImGui::Text("%d", oTexture.GetHeight());
			}
		}
		else
		{
			ImGui::Text("-");
		}
		ImGui::PopFont();

		//Face/Slice
		if (oTexture.IsValid() && oTexture.GetSliceCount() > 1)
		{
			ImGui::PushFont(oFonts.pFontConsolas);
			ImGui::SameLine();
			ImGui::TextUnformatted("Face:");
			ImGui::PopFont();

			ImGui::PushFont(oFonts.pFontConsolasBold);
			ImGui::SameLine(0, 0);
			ImGui::Text("%d", oTexture.GetSliceCount());
			ImGui::PopFont();
		}

		//Mips
		if (oTexture.IsValid() && oTexture.GetMipCount() > 1)
		{
			ImGui::PushFont(oFonts.pFontConsolas);
			ImGui::SameLine();
			ImGui::TextUnformatted("Mip:");
			ImGui::PopFont();

			ImGui::PushFont(oFonts.pFontConsolasBold);
			ImGui::SameLine(0, 0);
			ImGui::Text("%d", oTexture.GetMipCount());
			ImGui::PopFont();
		}

		//Format
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::SameLine();
		ImGui::TextUnformatted("Format:");
		ImGui::PopFont();

		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		if (oTexture.IsValid())
		{
			ImGui::Text("%s", Graphics::PixelFormatEnumInfos[oTexture.GetPixelFormat()].pName);
		}
		else
		{
			ImGui::Text("-");
		}
		ImGui::PopFont();

		//Channels
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::SameLine();
		ImGui::TextUnformatted("Channels:");
		ImGui::PopFont();

		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		if (oTexture.IsValid())
		{
			ImGui::Text("%d", Graphics::PixelFormatEnumInfos[oTexture.GetPixelFormat()].iComponentCount);
		}
		else
		{
			ImGui::Text("-");
		}
		ImGui::PopFont();

		//Memory size
		ImGui::PushFont(oFonts.pFontConsolas);
		ImGui::SameLine();
		ImGui::TextUnformatted("Memory:");
		ImGui::PopFont();

		ImGui::PushFont(oFonts.pFontConsolasBold);
		ImGui::SameLine(0, 0);
		if (oTexture.IsValid())
		{
			char pBuffer[128];
			Core::StringUtils::GetReadableSize(oTexture.GetDataSize(), pBuffer, sizeof(pBuffer));
			ImGui::TextUnformatted(pBuffer);
		}
		else
		{
			ImGui::Text("-");
		}
		ImGui::PopFont();
	}

	if (iDisplayTime <= c_iLogDisplayTime)
	{
		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(fCursorX, fCursorY + fCursorOffset * (1.f - m_fLastLogFade)));

		ImVec4 oColor;
		switch (m_eLastLogCategory)
		{
		case Core::Logger::Category::Debug:
			oColor = ImVec4(0.5f, 0.5f, 0.5f, 1.f);
			break;
		default:
		case Core::Logger::Category::Info:
			//oColor = ImVec4(0.8f, 0.8f, 0.8f, 1.f);
			oColor = ImVec4(0.3f, 0.8f, 0.3f, 1.f);
			break;
		case Core::Logger::Category::Warning:
			oColor = ImVec4(1.f, 0.5f, 0.f, 1.f);
			break;
		case Core::Logger::Category::Error:
			oColor = ImVec4(0.8f, 0.22f, 0.f, 1.f);
			break;
		}

		float fHighlight = (1.f - Math::Clamp(((float)iDisplayTime - (float)c_iLogDisplayTransitionTime) / (float)c_iLogDisplayHighlightTime, 0.f, 1.f));

		oColor.x = Math::Clamp(oColor.x * (1.f + fHighlight) + fHighlight * 0.2f, 0.f, 1.f);
		oColor.y = Math::Clamp(oColor.y * (1.f + fHighlight) + fHighlight * 0.2f, 0.f, 1.f);
		oColor.z = Math::Clamp(oColor.z * (1.f + fHighlight) + fHighlight * 0.2f, 0.f, 1.f);

		if (ImGui::SmallButton("X") && iDisplayTime < (c_iLogDisplayTime - c_iLogDisplayTransitionTime))
		{
			m_iLastLogTime = Core::Clock::GetCurrentTime() - (c_iLogDisplayTime - c_iLogDisplayTransitionTime);
		}

		if(ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Hide log");
		}
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, oColor);
		ImGui::TextUnformatted(m_sLastLogMessage.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::EndGroup();
}

void StatusBars::Log(Core::Logger::Category eCategory, const char* /*pName*/, const char* pFormattedMessage)
{
#ifndef _DEBUG
	if (eCategory == Core::Logger::Category::Debug)
		return;
#endif
	uint64_t iCurrent = Core::Clock::GetCurrentTime();
	m_iLastLogTime = iCurrent;
	m_eLastLogCategory = eCategory;
	m_sLastLogMessage = pFormattedMessage;
}

bool StatusBars::IsDisplayingLog() const
{
	return m_fLastLogFade > 0.f;
}

void StatusBars::SkipLog()
{
	Program* pProgram = Program::GetInstance();
	const Core::Clock& oClock = pProgram->GetClock();

	uint64_t iResetTime = oClock.GetFrameTime() - (c_iLogDisplayTime - c_iLogDisplayTransitionTime);
	if (m_iLastLogTime > iResetTime)
		m_iLastLogTime = iResetTime;
}