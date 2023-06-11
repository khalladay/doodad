#include "input_handler.h"
#include <Windows.h>
#include "debug.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

const int KEYBOARD_ID = 0;
const int MOUSE_ID = 1;

struct InputState
{
	int mouseDY;
	int mouseX;
	int mouseY;

	int mouseLastX = -1;
	int mouseLastY = -1;

	int mouseDX;
	int mouseWheelDelta;
	__declspec(align(8))  BYTE inputBuffer[48]; //RAWINPUT buffer - somewhere online claims this can be up to 48 for keyboard and mouse on 64 bit

	unsigned char lastFrameKeyStates[256];
	unsigned char keyStates[256];

	bool lastFrameMouseButtons[3]; //for detecting clicks
	bool mouseButtons[3];
	char padding[5];
};


InputHandler& Input() { return *InputHandler::GetInstance(); }

InputHandler* InputHandler::GetInstance()
{
	static InputHandler instance;
	return &instance;
}

void InputHandler::Initialize(uint64_t hwnd)
{
	HWND wndHdl = (HWND)hwnd;
	mInputState = new InputState();
	memset(mInputState->keyStates, 0, sizeof(mInputState->keyStates));

}

InputHandler::InputHandler()
{
}

void InputHandler::MouseButtonDown(EMouseButton btn)
{
	mInputState->mouseButtons[(uint32_t)btn] = true;
}

void InputHandler::MouseButtonUp(EMouseButton btn)
{
	mInputState->mouseButtons[(uint32_t)btn] = false;
}

void InputHandler::MouseMoved(int mouseAbsX, int mouseAbsY, int winWidth, int winHeight)
{

	InputState& inputState = *mInputState;

	if (mouseAbsX < 0) { mouseAbsX = 0; }
	if (mouseAbsY < 0) { mouseAbsY = 0; }

	if (mouseAbsX > winWidth) { mouseAbsX = winWidth; }
	if (mouseAbsY > winHeight) { mouseAbsY = winHeight; }

	if (inputState.mouseLastX == -1)
	{
		inputState.mouseX = mouseAbsX;
		inputState.mouseLastX = mouseAbsX;
		inputState.mouseDX = 0;

		inputState.mouseY = mouseAbsY;
		inputState.mouseLastY = mouseAbsY;
		inputState.mouseDY = 0;


	}
	else
	{
		inputState.mouseDX = mouseAbsX - inputState.mouseLastX;;
		inputState.mouseLastX = inputState.mouseX;
		inputState.mouseX = mouseAbsX;

		inputState.mouseDY = mouseAbsY - inputState.mouseLastY;
		inputState.mouseLastY = inputState.mouseY;
		inputState.mouseY = mouseAbsY;

	}

}

void InputHandler::Update(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, uint32_t winWidth, uint32_t winHeight)
{
	//get this from the os since it can adjust for the window much easier with win32 functions
		//and we don't want to have to include those functions here	

	switch (msg)
	{
	case WM_MBUTTONDOWN:
	{
		MouseButtonDown(EMouseButton::MOUSE_MID);
	}break;
	case WM_MBUTTONUP:
	{
		MouseButtonUp(EMouseButton::MOUSE_MID);
	}break;
	case WM_RBUTTONDOWN:
	{
		MouseButtonDown(EMouseButton::MOUSE_RIGHT);

	}break;
	case WM_RBUTTONUP:
	{
		MouseButtonUp(EMouseButton::MOUSE_RIGHT);

	}break;
	case WM_LBUTTONDOWN:
	{
		MouseButtonDown(EMouseButton::MOUSE_LEFT);
	}break;
	case WM_LBUTTONUP:
	{
		MouseButtonUp(EMouseButton::MOUSE_LEFT);
	}break;
	case WM_MOUSEMOVE:
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(hWnd, &p);
		MouseMoved(p.x, p.y, winWidth, winHeight);
	}break;
	case WM_MOUSEWHEEL:
	{
		short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		UpdateMousewheel(zDelta);

	}break;
	case WM_KEYDOWN:
	{
		mInputState->keyStates[wParam] = true;
	}break;
	case WM_KEYUP:
	{
		mInputState->keyStates[wParam] = false;
	}break;
	}

}

void InputHandler::EndFrame()
{
	InputState& derefInput = *mInputState;
	derefInput.mouseDX = 0;
	derefInput.mouseDY = 0;
	derefInput.mouseWheelDelta = 0;

	memcpy(derefInput.lastFrameMouseButtons, derefInput.mouseButtons, sizeof(bool) * 3);
	memcpy(derefInput.lastFrameKeyStates, derefInput.keyStates, sizeof(mInputState->keyStates));
}

void InputHandler::ForceKeyUp(EKeyCode key)
{
	mInputState->keyStates[key] = false;
}


void InputHandler::ProcessMouseButton(MouseButtonIndex button, unsigned long& rawInputButtons)
{
	unsigned int buttonDown = 0;
	unsigned int buttonUp = 0;

	if (button == (uint32_t)EMouseButton::MOUSE_LEFT)
	{
		buttonUp = rawInputButtons & RI_MOUSE_LEFT_BUTTON_UP;
		buttonDown = rawInputButtons & RI_MOUSE_LEFT_BUTTON_DOWN;

		checkf((buttonUp && buttonDown) == false, "Received conflicting input messages for left mouse button");
	}
	else if (button == (uint32_t)EMouseButton::MOUSE_MID)
	{
		buttonUp = rawInputButtons & RI_MOUSE_MIDDLE_BUTTON_UP;
		buttonDown = rawInputButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN;

		checkf((buttonUp && buttonDown) == false, "Received conflicting input messages for middle mouse button");
	}
	else if (button == (uint32_t)EMouseButton::MOUSE_RIGHT)
	{
		buttonUp = rawInputButtons & RI_MOUSE_RIGHT_BUTTON_UP;
		buttonDown = rawInputButtons & RI_MOUSE_RIGHT_BUTTON_DOWN;

		checkf((buttonUp && buttonDown) == false, "Received conflicting input messages for right mouse button");
	}

	if (buttonDown) mInputState->mouseButtons[button] = true;
	if (buttonUp) mInputState->mouseButtons[button] = false;
}

int InputHandler::GetMouseDX()
{
	return mInputState->mouseDX;
}

int InputHandler::GetMouseDY()
{
	return mInputState->mouseDY;
}

int InputHandler::GetMouseX()
{
	return mInputState->mouseX;
}

int InputHandler::GetMouseY()
{
	return mInputState->mouseY;
}

bool InputHandler::GetKey(EKeyCode key)
{
	return mInputState->keyStates[key] > 0;

}

bool InputHandler::GetKeyDown(EKeyCode key)
{
	bool thisFrameKeyState = GetKey(key);
	bool lastFrame = mInputState->lastFrameKeyStates[key] > 0;
	return thisFrameKeyState && !lastFrame;
}

bool InputHandler::GetKeyUp(EKeyCode key)
{
	bool thisFrameKeyState = GetKey(key);
	bool lastFrame = mInputState->lastFrameKeyStates[key] > 0;
	return !thisFrameKeyState && lastFrame;

}

void InputHandler::UpdateMousewheel(short zDelta)
{
	mInputState->mouseWheelDelta = zDelta / 120;
}

int InputHandler::GetMouseButton(EMouseButton btn)
{
	return mInputState->mouseButtons[(int)btn];
}

bool InputHandler::GetMouseButtonClicked(EMouseButton btn)
{
	bool thisFrame = GetMouseButton(btn);
	bool lastFrame = mInputState->lastFrameMouseButtons[(int)btn];

	return thisFrame && !lastFrame;
}

int InputHandler::GetMouseWheelDelta()
{
	return mInputState->mouseWheelDelta;
}

InputHandler::~InputHandler()
{

}