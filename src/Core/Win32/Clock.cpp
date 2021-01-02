#include "Core/Clock.h"

#include <windows.h> // GetTickCount
#undef GetCurrentTime

namespace Core
{
	uint64_t Clock::GetCurrentTime()
	{
		return GetTickCount64();
	}
} // namespace Core