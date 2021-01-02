#include "Core/Clock.h"

namespace Core
{
	Clock::Clock()
	{
		m_iDeltaTime = 0;
	}

	Clock::~Clock()
	{
	}

	void Clock::Manage()
	{
		uint64_t iCurrentTime = GetCurrentTime();
		m_iDeltaTime = iCurrentTime - m_iFrameTime;
		m_iFrameTime = iCurrentTime;
	}

	uint64_t Clock::GetFrameTime() const
	{
		return m_iFrameTime;
	}

	uint64_t Clock::GetDeltaTime() const
	{
		return m_iDeltaTime;
	}
} // namespace Core