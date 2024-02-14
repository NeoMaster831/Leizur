#pragma once
// Gui.h: GUI Base of all GUIs.
// Made of ImGui
// ImGui Assert Level Handling.
/*
 * 0 - Couldn't Reset Device
 */

#include <d3d9.h>


namespace Gui
{
	static const int WIDTH = 800;
	static const int HEIGHT = 600;

	static HWND Window = nullptr;
	static WNDCLASSEXW WindowClass = {};
	static POINTS LastCursorPosition;
	static PDIRECT3D9 Direct3D = nullptr;
	static LPDIRECT3DDEVICE9 Device = nullptr;
	static D3DPRESENT_PARAMETERS PresentParameters = {};

	static bool Stay = true;

	void CreateHWindow(const wchar_t* windowName, const wchar_t* className);
	void DestroyHWindow();

	bool CreateDevice();
	void ResetDevice();
	void DestroyDevice();

	void CreateImGui();
	void DestroyImGui();

	void BeginRender();
	void EndRender();
	void Render();

	void Begin();
	void Update();
	void End();
};

