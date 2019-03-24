
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Program.h"

//#define CONSOLE

#ifndef CONSOLE
#include <Windows.h>
#endif

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1337);

#ifdef _DEBUG
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
#endif //_DEBUG

	Program* pInstance = Program::CreateInstance(argc, argv);

	while (pInstance->Run()) Sleep(8);

	Program::DestroyInstance();

	return 0;
}