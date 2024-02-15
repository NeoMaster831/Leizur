#include "Osu/Osu.h"
#include "GUI/Gui.h"
#include "GUI/Gdi.h"
#include <iostream>
#include <thread>

INT APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PSTR, _In_ INT cmd_show) {

	Gdi::Begin();
	Gui::Begin();

	if (!MemInit()) return EXIT_FAILURE;
	auto osuLiveThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)OsuLive::Start, NULL, NULL, NULL);
	auto gdiThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Gdi::Update, NULL, NULL, NULL);
	Gui::Update();

	Gui::End();
	Gdi::End();
	if (gdiThread) CloseHandle(gdiThread);
	if (osuLiveThread) CloseHandle(osuLiveThread);

	return EXIT_SUCCESS;
}