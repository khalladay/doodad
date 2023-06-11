#include "doodad_app.h"
#include "win_init.h"
#include "dx11_render_backend.h"
#include "doodad_view.h"
#include "common_header.h"
#include "doodad_controller.h"
#include "doodad_view.h"

DoodadApp& App() { return *DoodadApp::GetInstance(); }

DoodadApp* DoodadApp::GetInstance()
{
	static DoodadApp instance;
	return &instance;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::string DoodadApp::getExecutablePath() const
{
	checkf(!pathToExe.empty(), "Path to exe was not set");
	return pathToExe;
}

void DoodadApp::Initialize(HWND wndHdl, HINSTANCE instance, const char* pathToApp)
{
	appWindowHandle = wndHdl;
	checkf(pathToApp, "Attemtping to initialize ScenarioBookApp with a null executable path");

	pathToExe = pathToApp;

	RECT rect;
	GetClientRect(wndHdl, &rect);
	screenW = rect.right;
	screenH = rect.bottom;

	Input().Initialize((uint64_t)wndHdl);

	os::wndProcDelegate.bind([this](HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) -> void
	{
		switch (msg)
		{
		case WM_SIZE:
		{
			RECT rect;

			GetClientRect(hWnd, &rect);
			screenW = rect.right;
			screenH = rect.bottom;
			DX().ResizeSwapchain(screenW, screenH);
		}break;
		case WM_CLOSE:
		{
			wantsExit = true;
		}break;
	
		}

		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		Input().Update(hWnd, msg, wParam, lParam, screenW, screenH);
	});

	auto ctx = ImGui::CreateContext();
	ImGui::SetCurrentContext(ctx);

	ImGui_ImplWin32_Init(wndHdl);
	ImGui_ImplDX11_Init(DX().device, DX().devCon);

	mController = new DoodadController();
	mView = new DoodadView();

	mView->Setup();
	mView->SetDelegate(mController);
}

void DoodadApp::ResizeWindow(RECT r)
{
	SetWindowPos(appWindowHandle, 0,
		r.left, r.top,
		r.right - r.left,
		r.bottom - r.top,
		SWP_NOZORDER | SWP_NOACTIVATE);

	UpdateWindow(appWindowHandle);
}

WindowPosition DoodadApp::GetWindowPos()
{
	RECT winRect;
	GetWindowRect(appWindowHandle, &winRect);
	return { winRect.left, winRect.top };
}

void DoodadApp::Tick()
{
	static double lastTime = 0.0f;
	double currentTime = os::getMilliseconds();

	double delta = lastTime == 0.0f ? 0.16 : currentTime - lastTime;
	lastTime = currentTime;
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	mView->Tick((float)delta);
	Input().EndFrame();

}

bool DoodadApp::ShouldExit()
{
	return wantsExit;
}

void DoodadApp::Draw()
{
	if (mView != nullptr)
	{
		mView->Draw();
	}
}

