#include "Core/Assert.h"
#include "Core/Logger.h"

#include <stdio.h>
#include <stdarg.h>

#include <Windows.h>

namespace Core
{
	bool Assert(bool bCondition, const char* pCondition, const char* pFile, int iLine, const char* pFunction, const char* pFormat, ...)
	{
		if (bCondition == false)
		{
			LogError( "Assert", "%s", pCondition );

			va_list args;
			va_start(args, pFormat);
			char pMsgBuffer[2048];
			vsnprintf(pMsgBuffer, 2048, pFormat, args);
			va_end(args);

			char pBuffer[4096];
			if (pMsgBuffer[0] != 0)
				snprintf(pBuffer, 4096, "%s\n\nCondition:\n%s\n\nFile:\n%s(%d)\n\nFunction:\n%s", pMsgBuffer, pCondition, pFile, iLine, pFunction);
			else
				snprintf(pBuffer, 4096, "Condition:\n%s\nFile:\n%s(%d)\n\nFunction:\n%s", pCondition, pFile, iLine, pFunction);
			int iRet = MessageBox(NULL, pBuffer, "Assert: Do you want to break?", MB_ICONERROR | MB_YESNO);
			return iRet == IDYES;
		}
		return false;
	}
}
//Core