#pragma once

#include "common_header.h"
#include <string>
#include "dX11_render_backend.h"

class FileData;
class DX11RenderBackend;

enum EViewState
{
	StartupMessage,
	HelpMessage,
	Normal
};

class DoodadViewDelegate
{
public:
	virtual std::string Eval(std::string input) = 0;
};

class DoodadView : public View
{
public:
	virtual void Setup() override;
	virtual void Teardown() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;
	virtual void DrawUI() override;

	void SetDelegate(DoodadViewDelegate* Delegate);

	~DoodadView();
	int ConsoleHistoryCallback(ImGuiInputTextCallbackData* data);

private:
	void DrawOutputWindow();
	void DrawInputBar();
	void CopyLastOutputToClipboard();

	EViewState mViewState = EViewState::StartupMessage;

	bool mWantsScroll = false;
	

	char mInputBuffer[4096];

	std::vector<std::string> mStartupMessage;
	std::vector<std::string> mHelpMessage;
	std::vector<std::string> mHistory;
	std::vector<std::string> mInputHistory;
	int32_t mInputHistoryIdx = -1;

	DoodadViewDelegate* mDelegate = nullptr;

};