#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX 
#include <Windows.h>

#define MOUSE_LEFT 0
#define MOUSE_RIGHT 1
#define MOUSE_MIDDLE 2
#define MOUSE_BACK 3
#define MOUSE_FORWARD 4

class Input
{
public:
	static bool RegisterRawMouseInput();
	static void HandleRawInput(WPARAM wParam, LPARAM lParam);
	static void HandleKeysInput(UINT msg, WPARAM wParam, LPARAM lParam);

	static bool WasKeyPressed(int keycode); // TODO
	static bool IsKeyPressed(int keycode);
	static bool WasMousePressed(int mousecode);
	static bool IsMousePressed(int mousecode);

	static int GetMouseDeltaX() { return s_MouseDeltaX; }
	static int GetMouseDeltaY() { return s_MouseDeltaY; }
	static int GetMouseScrollDelta() { return s_MouseScrollDelta; }

	static void ResetDeltas() { s_MouseDeltaX = 0; s_MouseDeltaY = 0; s_MouseScrollDelta = 0; s_MouseDeltaXRead = true; s_MouseDeltaYRead = true; s_MouseScrollDeltaRead = true; }
	
private:
	static bool s_Keycodes[256];
	static bool s_MouseButtons[5];
	static int s_MouseScrollDelta;
	static int s_MouseDeltaX;
	static int s_MouseDeltaY;

	// If we haven't read from these deltas, we want to add onto it not just set it. Because the movement might be to fast per frame
	static bool s_MouseDeltaXRead;
	static bool s_MouseDeltaYRead;
	static bool s_MouseScrollDeltaRead;
};

