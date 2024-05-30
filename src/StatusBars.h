#ifndef _STATUS_BARS_H_
#define _STATUS_BARS_H_

#include "Core/Logger.h"
#include "Core/String.h"

#include "GraphicResources/Texture2D.h"

#include "ImWindow/ImwStatusBar.h"

class StatusBars : ImWindow::ImwStatusBar, Core::Logger::LoggerOutputer
{
public:
	StatusBars();
	virtual ~StatusBars();

	virtual void						OnStatusBar();

	virtual void						Log(Core::Logger::Category eCategory, const char* pName, const char* pFormattedMessage);

	bool								IsDisplayingLog() const;
	void								SkipLog();

protected:
	GraphicResources::Texture2D*		m_pIconLog;
	uint64_t							m_iLastLogTime;
	Core::Logger::Category				m_eLastLogCategory;
	Core::String						m_sLastLogMessage;
	float								m_fLastLogFade;
	uint64_t							m_iPreviousTime;
};

#endif //_STATUS_BARS_H_