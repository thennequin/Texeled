#ifndef __CORE_CLOCK_H__
#define __CORE_CLOCK_H__

#include <stdint.h> // uint64_t

namespace Core
{
	namespace Clock
	{
		extern uint64_t GetCurrent();
	} // namespace Clock
} // namespace Core

#endif //__CORE_CLOCK_H__