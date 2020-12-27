#include "Core/Clock.h"

#include <windows.h> // GetTickCount

namespace Core
{
	namespace Clock
	{
		uint64_t GetCurrent()
		{
			return GetTickCount64();
		}
	} // namespace Clock
} // namespace Core