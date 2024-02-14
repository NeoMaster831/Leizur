#include "Gdi.h"
#include <thread>

LRESULT CALLBACK GdiWindowProcess(
	HWND window,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
) {
	switch (message) {

	case WM_DESTROY: {
		PostQuitMessage(0);
	} return 0;

	}
	return DefWindowProc(window, message, wParam, lParam);
}

void Gdi::CreateHWindow(const wchar_t* windowName, const wchar_t* className) {
	WindowClass = {
		sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW,
		GdiWindowProcess, 0, 0, GetModuleHandle(0),
		0, 0, 0, 0, className, 0
	};
	RegisterClassExW(&WindowClass);
	Window = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
		className, windowName,
		WS_POPUP, 0, 0, WIDTH, HEIGHT,
		NULL, NULL, WindowClass.hInstance, NULL);
	SetWindowPos(Window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	ShowWindow(Window, SW_SHOW);
}

void Gdi::DestroyHWindow() {
	DestroyWindow(Window);
	UnregisterClassW(WindowClass.lpszClassName, WindowClass.hInstance);
}

void Gdi::InitGdiPlus() {
	GdiplusStartupInput gpsi;
	GdiplusStartup(&GdiPlusToken, &gpsi, NULL);
}

void Gdi::ShutdownGdiPlus() {
	GdiplusShutdown(GdiPlusToken);
}

void Gdi::Begin() {
	CreateHWindow(L"LeizurGdi", L"LeizurGdiClass");
	InitGdiPlus();
}

void Gdi::Draw(Bitmap* pBitmap) {
	Graphics graphics(pBitmap);
	Color color(128, 255, 0, 0);
	SolidBrush brush(color);

	POINT cursorPos;
	GetCursorPos(&cursorPos);
	INT x = cursorPos.x;
	INT y = cursorPos.y;

	graphics.FillEllipse(&brush, x - 25, y - 25, 50, 50);
}

void Gdi::Render() {
	HDC dcScreen = GetDC(Window);
	HDC dcMemory = CreateCompatibleDC(dcScreen);
	Bitmap bitmap(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), PixelFormat32bppARGB);
	
	Draw(&bitmap);

	HBITMAP hBitmap;
	bitmap.GetHBITMAP(Color::Black, &hBitmap);
	auto oldBitmap = (HBITMAP)SelectObject(dcMemory, hBitmap);

	UINT width = bitmap.GetWidth();
	UINT height = bitmap.GetHeight();

	POINT ptZero = { 0, 0 };
	SIZE size = { (LONG)width, (LONG)height };
	POINT ptTopLeft = ptZero;

	BLENDFUNCTION blend = { 0 };
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat = AC_SRC_ALPHA;

	UpdateLayeredWindow(Window, dcScreen, &ptTopLeft, &size, dcMemory, &ptZero, 0, &blend, ULW_ALPHA);
	
	SelectObject(dcMemory, oldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(dcMemory);
	ReleaseDC(Window, dcScreen);
}

void Gdi::Update() {
	while (true) {
		Render();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 480));
		// osu's gameplay fps is 480
	}
}

void Gdi::End() {
	ShutdownGdiPlus();
	DestroyHWindow();
}