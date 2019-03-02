
#include "ImwWindowManagerDX11.h"
#include "windows.h"

int main()
{
	ImWindow::ImwWindowManagerDX11 oMgr;

	oMgr.Init();

	while (oMgr.Run(false) && oMgr.Run(true)) Sleep(16);

	ImGui::Shutdown();

	return 0;
}
