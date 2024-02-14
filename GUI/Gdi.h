#pragma once
#include <Windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

// Gdi.h: Draw some shits with GDI+

namespace Gdi {

	static const int WIDTH = GetSystemMetrics(SM_CXSCREEN);
	static const int HEIGHT = GetSystemMetrics(SM_CYSCREEN);
	
	static HWND Window = nullptr;
	static WNDCLASSEXW WindowClass = {};

	static ULONG_PTR GdiPlusToken;

	void CreateHWindow(const wchar_t* windowName, const wchar_t* className);
	void DestroyHWindow();

	void InitGdiPlus();
	void ShutdownGdiPlus();

	void Render();
	void Draw(Bitmap* pBitmap);

	void Begin();
	void Update();
	void End();
}