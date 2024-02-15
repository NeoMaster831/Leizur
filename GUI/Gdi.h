#pragma once
#pragma warning( disable : 4267 4244 )
#include <Windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

// Gdi.h: Draw some shits with GDI+

namespace Gdi {

	inline const int WIDTH = GetSystemMetrics(SM_CXSCREEN);
	inline const int HEIGHT = GetSystemMetrics(SM_CYSCREEN);
	
	inline HWND Window = nullptr;
	inline WNDCLASSEXW WindowClass = {};

	inline ULONG_PTR GdiPlusToken;

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