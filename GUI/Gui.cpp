#include "Gui.h"

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx9.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../Osu/Osu.h"
#include "Gdi.h"
#include <windowsx.h>
#include <thread>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK GuiWindowProcess(
	HWND window,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam)) return true;

	switch (message) {

	case WM_SYSCOMMAND: {
		if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
	} break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	} return 0;

	case WM_LBUTTONDOWN: {
		Gui::LastCursorPosition = MAKEPOINTS(lParam);
	} return 0;

	case WM_MOUSEMOVE: {
		if (wParam == MK_LBUTTON) {
			auto points = MAKEPOINTS(lParam);
			auto rect = ::RECT{};
			GetWindowRect(Gui::Window, &rect);
			rect.left += points.x - Gui::LastCursorPosition.x;
			rect.top += points.y - Gui::LastCursorPosition.y;

			if (Gui::LastCursorPosition.x >= 0 && Gui::LastCursorPosition.x <= Gui::WIDTH
				&& Gui::LastCursorPosition.y >= 0 && Gui::LastCursorPosition.y <= 19)
				SetWindowPos(
					Gui::Window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}
	} return 0;

	}
	return DefWindowProc(window, message, wParam, lParam);
}

void Gui::CreateHWindow(const wchar_t* windowName, const wchar_t* className) {
	WindowClass.cbSize = sizeof(WNDCLASSEXW);
	WindowClass.style = CS_CLASSDC;
	WindowClass.lpfnWndProc = GuiWindowProcess;
	WindowClass.hInstance = GetModuleHandle(NULL);
	WindowClass.lpszClassName = className;

	RegisterClassExW(&WindowClass);
	Window = CreateWindowW(
		className,
		windowName,
		WS_POPUP,
		0, 0, WIDTH, HEIGHT,
		0, 0, WindowClass.hInstance, 0
	);

	ShowWindow(Window, SW_SHOWDEFAULT);
	UpdateWindow(Window);
}

void Gui::DestroyHWindow() {
	DestroyWindow(Window);
	UnregisterClassW(WindowClass.lpszClassName, WindowClass.hInstance);
}

bool Gui::CreateDevice() {
	Direct3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!Direct3D) return false;
	
	ZeroMemory(&PresentParameters, sizeof(PresentParameters));

	PresentParameters.Windowed = TRUE;
	PresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	PresentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
	PresentParameters.EnableAutoDepthStencil = TRUE;
	PresentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	return Direct3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		Window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&PresentParameters,
		&Device) >= 0;
}

void Gui::ResetDevice() {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	auto result = Device->Reset(&PresentParameters);
	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

void Gui::DestroyDevice() {
	if (Device) {
		Device->Release(); Device = nullptr;
	}
	if (Direct3D) {
		Direct3D->Release(); Direct3D = nullptr;
	}
}

void Gui::CreateImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX9_Init(Device);
}

void Gui::DestroyImGui() {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Gui::BeginRender() {
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) Stay = false;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Gui::EndRender() {
	ImGui::EndFrame();

	Device->SetRenderState(D3DRS_ZENABLE, FALSE);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	Device->Clear(0, 0,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (Device->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		Device->EndScene();
	}

	auto result = Device->Present(0, 0, 0, 0);

	if (result == D3DERR_DEVICELOST && Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

void Gui::Render() {
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin("Leizur",
		&Stay,
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	
	ImGui::LabelText("", OsuLive::lastBeatmapHash.c_str());
	if (OsuLive::lastBeatmapHash != "No Beatmap" && OsuLive::currentBeatmap.Title != "null") {
		ImGui::LabelText(OsuLive::currentBeatmap.Title.c_str(), OsuLive::currentBeatmap.Artist.c_str());
		ImGui::LabelText(std::to_string(OsuLive::currentBeatmap.BeatmapVersion).c_str(),
			OsuLive::currentBeatmap.Version.c_str());
		ImGui::LabelText(std::to_string(OsuLive::currentBeatmap.ApproachRate).c_str(),
			std::to_string(OsuLive::currentBeatmap.SliderMultiplier).c_str());
		
		
		try {
			auto point = OsuLive::currentBeatmap.GetReqCursorAtSpecificTime(OsuLive::osu.GetElaspedTime());
			ImGui::LabelText(std::to_string(point.y).c_str(),
				std::to_string(point.x).c_str());
			point = OsuLive::Translate2RealCoords(point);
			ImGui::LabelText(std::to_string(point.y).c_str(),
				std::to_string(point.x).c_str());
			auto ho = OsuLive::currentBeatmap.GetNextHitObject(OsuLive::osu.GetElaspedTime());
			if (ho.type & (1 << 1)) {
				ImGui::LabelText("slides", std::to_string(ho.sliderParam.slides).c_str());
			}
		}
		catch (int expn) {
			if (expn == 0x12000007 || expn == 0x12000008 || expn == 0x12000009 || expn == 0x1200000a ) goto Cont;
		}
	}
Cont:
	ImGui::End();
}

void Gui::Begin() {
	CreateHWindow(L"Leizur", L"LeizurClass");
	CreateDevice();
	CreateImGui();
}

void Gui::Update() {
	while (Stay) {
		BeginRender();
		Render();
		EndRender();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 240)); // my hz is 240 tho
	}
}

void Gui::End() {
	DestroyImGui();
	DestroyDevice();
	DestroyHWindow();
}