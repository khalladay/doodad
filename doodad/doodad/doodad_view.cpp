#include "doodad_view.h"
#include "input_handler.h"

void DoodadView::Setup()
{
	mStartupMessage.push_back("Welcome To Doodad\nType \"help\" for help");
	mHelpMessage.push_back("Help Message Here");
}

void DoodadView::Teardown()
{

}

void DoodadView::Tick(float deltaTime)
{
}

void DoodadView::Draw()
{
	auto* dx11 = DX11RenderBackend::getInstance();
	dx11->SetDefaultRenderTargets();

	float bgColor[4] = { 0.4f, 0.4f, 0.4f, 0.0f };
	float noColor[4] = { 0,0,0,0 };
	dx11->devCon->ClearRenderTargetView(dx11->rtView, bgColor);
	dx11->devCon->ClearDepthStencilView(dx11->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(App().GetScreenW());
	viewport.Height = static_cast<float>(App().GetScreenH());
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dx11->devCon->RSSetViewports(1, &viewport);

	DrawUI();
	HRESULT err = dx11->swapChain->Present(0, 0);
	check(err == S_OK);

}

void DoodadView::DrawUI()
{
	ImGui::NewFrame();

	ImGui::SetNextWindowPos({0,0});
	ImGui::SetNextWindowSize({ App().GetScreenW() / 1.0f, App().GetScreenH() / 1.0f  });
	ImGui::Begin("Setup",nullptr, ImGuiWindowFlags_NoDecoration);
	DrawOutputWindow();
	DrawInputBar();

	ImGui::End();
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DoodadView::SetDelegate(DoodadViewDelegate* Delegate)
{
	check(Delegate != nullptr);
	mDelegate = Delegate;
}

void DoodadView::DrawOutputWindow()
{
    const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollRegion##", ImVec2(0, -footerHeightToReserve), false, 0))
    {
        // Display colored command output.
        static const float timestamp_width = ImGui::CalcTextSize("00:00:00:0000").x;    // Timestamp.
        int count = 0;                                                                       // Item count.

        // Wrap items.
        ImGui::PushTextWrapPos();

        // Display items.
		std::vector<std::string>* outputBuffer = nullptr;
		switch (mViewState)
		{
		case EViewState::StartupMessage: outputBuffer = &mStartupMessage; break;
		case EViewState::HelpMessage: outputBuffer = &mHelpMessage; break;
		case EViewState::Normal: outputBuffer = &mHistory; break;
		}

		check(outputBuffer != nullptr);

		for (const auto& item : *outputBuffer)
		{
			ImGui::TextUnformatted(item.data());
		}

        // Stop wrapping since we are done displaying console items.
        ImGui::PopTextWrapPos();

        // Auto-scroll logs.
		if ((mWantsScroll && (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())))
			ImGui::SetScrollHereY(1.0f);
		mWantsScroll = false;

    }
	ImGui::EndChild();

}

int DoodadView::ConsoleHistoryCallback(ImGuiInputTextCallbackData* data)
{
	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackHistory:
	{
		// Example of HISTORY
		const int prevHistoryIdx = mInputHistoryIdx;
		if (ImGui::IsKeyPressed(EKeyCode::KEY_UP))
		{
			mInputHistoryIdx = min(mInputHistory.size() - 1, mInputHistoryIdx + 1);
		}
		else if (ImGui::IsKeyPressed(EKeyCode::KEY_DOWN))
		{
			mInputHistoryIdx = max(-1, mInputHistoryIdx - 1);
		}
		// A better implementation would preserve the data on the current input line along with cursor position.
		if (prevHistoryIdx != mInputHistoryIdx)
		{
			const char* history_str = (mInputHistoryIdx >= 0) ? mInputHistory[mInputHistory.size() -1 - mInputHistoryIdx].c_str() : "";
			data->DeleteChars(0, strlen(mInputBuffer));
			data->InsertChars(0, history_str);
		}
	}
	}
	return 0;
}

void DoodadView::CopyLastOutputToClipboard()
{
	if (mHistory.size() == 0) return;

	if (!OpenClipboard(App().appWindowHandle))
	{
		return;
	}	

	EmptyClipboard();

	std::string lastOutput = mHistory[mHistory.size() - 1];
	
	//allocate global memory to store clipboard contents
	HGLOBAL globalMem = GlobalAlloc(GMEM_FIXED, (lastOutput.size() + 1) * sizeof(TCHAR));

	if (globalMem == NULL)
	{
		CloseClipboard();
		return;
	}

	memcpy(globalMem, lastOutput.c_str(), lastOutput.size());


	SetClipboardData(CF_TEXT, globalMem);
	CloseClipboard();
}


void DoodadView::DrawInputBar()
{
	// Only reclaim after enter key is pressed!
	bool reclaimFocus = false;
	
	//hacky way to get an imgui input text callback to route to this object's cb func.
	//since lambda's can only be function ptrs if they don't capture
	static DoodadView* StaticViewPointer = this;

	if (ImGui::IsKeyPressed(EKeyCode::KEY_BACKSPACE))
	{
		mInputHistoryIdx = -1;
	}

	if (ImGui::IsKeyPressed(EKeyCode::KEY_TAB))
	{
		CopyLastOutputToClipboard();
	}

	// Input widget. (Width an always fixed width)
	ImGui::PushItemWidth(-1);
	if (ImGui::InputText("##Input",
		mInputBuffer,
		sizeof(mInputBuffer),
		ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory,
		[](ImGuiInputTextCallbackData* data) {return StaticViewPointer->ConsoleHistoryCallback(data); }))
	{
		reclaimFocus = true;
		if (strlen(mInputBuffer) > 0)
		{
			if (mViewState == EViewState::StartupMessage)
			{
				mViewState = EViewState::Normal;
			}

			if (!strcmp(mInputBuffer, "help"))
			{
				mViewState = EViewState::HelpMessage;
			}
			else
			{
				mViewState = EViewState::Normal;
				
				if (mDelegate)
				{
					mInputHistory.push_back(mInputBuffer);
					std::string output = mDelegate->Eval(mInputBuffer);
					mHistory.push_back(output);
					mInputHistoryIdx = -1;

				}

				mWantsScroll = true;
			}
		}
		mInputHistoryIdx = -1;

		memset(mInputBuffer, 0, sizeof(mInputBuffer));
	}
	ImGui::PopItemWidth();


	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaimFocus)
	{
		ImGui::SetKeyboardFocusHere(-1); // Focus on command line after clearing.
	}
}


DoodadView::~DoodadView() {}
