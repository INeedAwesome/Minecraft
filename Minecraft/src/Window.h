#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX 
#include <Windows.h>

#include <string>
#include <iostream>

class Window
{
public:
	Window() = default;
	~Window() ;

	bool Create(const std::wstring& title, int width, int height);
	bool InitOpenGL();

	void Show();
	void Update();
	void CenterWindow();
	void CenterCursor();

	HWND GetHWND() const { return m_WindowHandle; }

public:
	void SetTitle(const std::wstring& title);
	std::wstring GetTitle() const { return m_Title; }

	unsigned int GetWidth() const { return m_Width; }

	unsigned int GetHeight() const { return m_Height; }

	void SetFullscreen(bool fullscreen);
	bool IsFullscreen() const { return m_IsFullscreen; }

	bool IsMinimized() const { return m_IsMinimized; }

	void SetHasResized(bool resized) { m_HasResized = resized; }
	bool HasResized() const { return m_HasResized; }

	void SetMouseCaptured(bool capture) { m_MouseCaptured = capture; ShowCursor(!capture); }
	bool IsMouseCaptured() const { return m_MouseCaptured; }

private:
	LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	HWND m_WindowHandle;
	DWORD m_WindowStyle;
	std::wstring m_Title;
	std::wstring m_ClassName;
	HGLRC m_OpenGLRenderingContext;

	unsigned int m_Width = 0;
	unsigned int m_Height = 0;

	unsigned int m_MiddleX = 0;
	unsigned int m_MiddleY = 0;

	bool m_IsFullscreen = false; 
	bool m_IsMinimized = false; 
	bool m_HasResized = false;

	bool m_MouseCaptured = false;
};

