#ifndef __CORE_CLOCK_H__
#define __CORE_CLOCK_H__

#include <stdint.h> // uint64_t

namespace Core
{
	class Clock
	{
	public:
		Clock();
		~Clock();

		void								Manage();

		uint64_t							GetFrameTime() const;
		uint64_t							GetDeltaTime() const;

		static uint64_t						GetCurrentTime();
	protected:
		uint64_t							m_iFrameTime;
		uint64_t							m_iDeltaTime;
	};
} // namespace Core

#endif //__CORE_CLOCK_H__