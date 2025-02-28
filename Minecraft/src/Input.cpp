#include "Input.h"

#include <Windowsx.h>
#include <iostream>

bool Input::s_Keycodes[256] = {};
bool Input::s_MouseButtons[5] = {};
int Input::s_MouseScrollDelta = 0;
int Input::s_MouseDeltaX = 0;
int Input::s_MouseDeltaY = 0;
bool Input::s_MouseDeltaXRead = false;
bool Input::s_MouseDeltaYRead = false;
bool Input::s_MouseScrollDeltaRead = false;

bool Input::RegisterRawMouseInput()
{
	RAWINPUTDEVICE rawInputDevice{};
	rawInputDevice.usUsagePage = 0x0001;
	rawInputDevice.usUsage = 0x0002;
	//rawInputDevice.dwFlags = RIDEV_NOLEGACY;
	rawInputDevice.dwFlags = 0;
	rawInputDevice.hwndTarget = 0;

	int success = RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice));
	if (success == FALSE)
	{
		return false;
	}
	return true;
}

void Input::HandleRawInput(WPARAM wParam, LPARAM lParam)
{
	if (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT)
	{
		UINT rawDataSize = 0;

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &rawDataSize, sizeof(RAWINPUTHEADER));
		BYTE* byte = new BYTE[rawDataSize];

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, byte, &rawDataSize, sizeof(RAWINPUTHEADER)) != rawDataSize)
			std::cout << "GetRawInputData does not return correct size !" << std::endl;

		RAWINPUT* raw = (RAWINPUT*)byte;

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			if (s_MouseDeltaXRead)
				s_MouseDeltaX += raw->data.mouse.lLastX;
			if (s_MouseDeltaYRead)
				s_MouseDeltaY += raw->data.mouse.lLastY;

			if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
				s_MouseButtons[MOUSE_LEFT] = true;
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
				s_MouseButtons[MOUSE_LEFT] = false;
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
				s_MouseButtons[MOUSE_RIGHT] = true;
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
				s_MouseButtons[MOUSE_RIGHT] = false;
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
				s_MouseButtons[MOUSE_MIDDLE] = true;
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
				s_MouseButtons[MOUSE_MIDDLE] = false;
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
				s_MouseButtons[MOUSE_BACK] = true;
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
				s_MouseButtons[MOUSE_BACK] = false;
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
				s_MouseButtons[MOUSE_FORWARD] = true;
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
				s_MouseButtons[MOUSE_FORWARD] = false;
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
			{
				if (s_MouseScrollDeltaRead)
				{
					s_MouseScrollDelta += (short)raw->data.mouse.usButtonData / WHEEL_DELTA;
				}
			}
		}
	}
}

void Input::HandleKeysInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		s_Keycodes[wParam] = true; 
		break;

	case WM_KEYUP:
		s_Keycodes[wParam] = false;
		break;

	default:
		break;
	}
	
}

bool Input::WasKeyPressed(int keycode)
{
	bool ret = s_Keycodes[keycode];
	s_Keycodes[keycode] = false;
	return ret;
}

bool Input::IsKeyPressed(int keycode)
{
	return s_Keycodes[keycode];
}

bool Input::WasMousePressed(int mousecode)
{
	bool ret = s_MouseButtons[mousecode];
	s_MouseButtons[mousecode] = false;
	return ret;
}

bool Input::IsMousePressed(int mousecode)
{
	return s_MouseButtons[mousecode];
}
