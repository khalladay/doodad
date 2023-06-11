#pragma once
#include <string>
#include "common_header.h"

struct WindowPosition
{
	int32_t x;
	int32_t y;
};

class View
{
public:
	virtual void Setup() = 0;
	virtual void Teardown() = 0;
	virtual void Tick(float deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void DrawUI() = 0;
	virtual ~View() {}
};

class DoodadController;
class DoodadView;

class DoodadApp
{
public:

	static DoodadApp* GetInstance();
	void Initialize(HWND wndHdl, HINSTANCE instance, const char* pathToExe);
	void Tick();
	void Draw();
	bool ShouldExit();
	uint32_t GetScreenW() const { return screenW; }
	uint32_t GetScreenH() const { return screenH; }

	void ResizeWindow(RECT newWinRect);
	WindowPosition GetWindowPos();
	std::string getExecutablePath() const;

	HWND appWindowHandle;

private:
	bool wantsExit;
	std::string pathToExe;
	uint32_t screenW;
	uint32_t screenH;

	DoodadController* mController;
	DoodadView* mView;
	class DX11RenderBackend* dx11;

};

DoodadApp& App();
