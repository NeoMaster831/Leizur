#pragma once
#pragma warning( disable : 4267 4244 )
// Gui.h: GUI Base of all GUIs.
// Made of ImGui
// ImGui Assert Level Handling.
/*
 * 0 - Couldn't Reset Device
 */

#include <d3d9.h>


namespace Gui
{
	inline const int WIDTH = 800;
	inline const int HEIGHT = 600;

	inline HWND Window = nullptr;
	inline WNDCLASSEXW WindowClass = {};
	inline POINTS LastCursorPosition;
	inline PDIRECT3D9 Direct3D = nullptr;
	inline LPDIRECT3DDEVICE9 Device = nullptr;
	inline D3DPRESENT_PARAMETERS PresentParameters = {};

	inline bool Stay = true;

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

