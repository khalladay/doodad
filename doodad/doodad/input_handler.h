#pragma once
#include <stdint.h>
#include <Windows.h>

enum EKeyCode
{
	KEY_BACKSPACE = 0x08,
	KEY_TAB = 0x09,
	KEY_RETURN = 0x0D,
	KEY_SHIFT = 0x10,
	KEY_CONTROL = 0x11,
	KEY_ALT = 0x12,
	KEY_ESCAPE = 0x1B,
	KEY_SPACE = 0x20,
	KEY_PAGEUP = 0x21,
	KEY_PAGEDOWN = 0x22,
	KEY_END = 0x23,
	KEY_HOME = 0x24,
	KEY_LEFT = 0x25,
	KEY_UP = 0x26,
	KEY_RIGHT = 0x27,
	KEY_DOWN = 0x28,
	KEY_SELECT = 0x29,
	KEY_PRINTSCREEN = 0x2C,
	KEY_INSERT = 0x2D,
	KEY_DELETE = 0x2E,
	KEY_0 = 0x30,
	KEY_1 = 0x31,
	KEY_2 = 0x32,
	KEY_3 = 0x33,
	KEY_4 = 0x34,
	KEY_5 = 0x35,
	KEY_6 = 0x36,
	KEY_7 = 0x37,
	KEY_8 = 0x38,
	KEY_9 = 0x39,
	KEY_A = 0x41,
	KEY_B = 0x42,
	KEY_C = 0x43,
	KEY_D = 0x44,
	KEY_E = 0x45,
	KEY_F = 0x46,
	KEY_G = 0x47,
	KEY_H = 0x48,
	KEY_I = 0x49,
	KEY_J = 0x4A,
	KEY_K = 0x4B,
	KEY_L = 0x4C,
	KEY_M = 0x4D,
	KEY_N = 0x4E,
	KEY_O = 0x4F,
	KEY_P = 0x50,
	KEY_Q = 0x51,
	KEY_R = 0x52,
	KEY_S = 0x53,
	KEY_T = 0x54,
	KEY_U = 0x55,
	KEY_V = 0x56,
	KEY_W = 0x57,
	KEY_X = 0x58,
	KEY_Y = 0x59,
	KEY_Z = 0x5A,
	KEY_LEFTWIN = 0x5B,
	KEY_RIGHTWIN = 0x5C,
	KEY_SEPARATOR = 0x6C,
	KEY_DECIMAL = 0x6E,
	KEY_DIVIDE = 0x6F,
	KEY_F1 = 0x70,
	KEY_F2 = 0x71,
	KEY_F3 = 0x72,
	KEY_F4 = 0x73,
	KEY_F5 = 0x74,
	KEY_F6 = 0x75,
	KEY_F7 = 0x76,
	KEY_F8 = 0x77,
	KEY_F9 = 0x78,
	KEY_F10 = 0x79,
	KEY_F11 = 0x7A,
	KEY_F12 = 0x7B,
	KEY_LSHIFT = 0xA0,
	KEY_RSHIFT = 0xA1,
	KEY_LCONTROL = 0xA2,
	KEY_RCONTROL = 0xA3,
	KEY_CAPSLOCK = 0x14,
	KEY_PLUS = 0x6B,
	KEY_MINUS = 0x6D
};

enum class EMouseButton : uint32_t
{
	MOUSE_LEFT = 0,
	MOUSE_MID = 1,
	MOUSE_RIGHT = 2
};


typedef int MouseButtonIndex;


class InputHandler
{
public:
	~InputHandler();

	static InputHandler* GetInstance();
	void Initialize(uint64_t hwnd);

	void MouseMoved(int mouseAbsX, int mouseAbsY, int winWidth, int winHeight);
	void MouseButtonDown(EMouseButton btn);
	void MouseButtonUp(EMouseButton btn);

	void Update(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, uint32_t winWidth, uint32_t winHeight);
	void ProcessMouseButton(MouseButtonIndex button, unsigned long& rawInputButtons);
	void UpdateMousewheel(short zDelta);

	void EndFrame();
	void ForceKeyUp(EKeyCode key);
	bool GetKey(EKeyCode key);
	bool GetKeyDown(EKeyCode key);
	bool GetKeyUp(EKeyCode key);


	int GetMouseDX();
	int GetMouseDY();
	int GetMouseX();
	int GetMouseY();
	int GetMouseWheelDelta();
	int GetMouseButton(EMouseButton btn);
	bool GetMouseButtonClicked(EMouseButton btn);

protected:

	InputHandler();
	typedef int MouseButtonIndex;


	struct InputState* mInputState;
};

InputHandler& Input();
