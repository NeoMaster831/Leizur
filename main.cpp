#include "Osu/Osu.h"
#include "GUI/Gui.h"
#include "GUI/Gdi.h"
#include <iostream>

INT APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PSTR, _In_ INT cmd_show) {
	MemInit();

	Gdi::Begin();
	Gui::Begin();

	auto thread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Gdi::Update, NULL, NULL, NULL);
	Gui::Update();

	Gui::End();
	Gdi::End();
	if (thread) CloseHandle(thread);

	return EXIT_SUCCESS;
}