#include "Window.h"

#include "include/glad/glad.h"

#include "Input.h"

#include "imgui/imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window::~Window()
{
	HDC devideContext = GetDC(m_WindowHandle);
	wglMakeCurrent(devideContext, NULL); // Unnecessary; wglDeleteContext will make the context not current but it's great practice.
	wglDeleteContext(m_OpenGLRenderingContext);

	DestroyWindow(m_WindowHandle);
	UnregisterClass(m_ClassName.c_str(), GetModuleHandle(nullptr));
}

bool Window::Create(const std::wstring& title, int width, int height)
{
	m_Title = title;
	m_ClassName = title + L"OpenGLApplication";
	m_Width = width;
	m_Height = height;

	WNDCLASS wc = { 0 };
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = Window::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hIcon = LoadIcon(GetModuleHandle(nullptr), IDI_WINLOGO);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = 0;
	wc.lpszClassName = m_ClassName.c_str();
	if (!RegisterClass(&wc))
		return false;

	m_WindowStyle = WS_OVERLAPPEDWINDOW;

	RECT windowRect{};
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = m_Width;
	windowRect.bottom = m_Height;
	AdjustWindowRect(&windowRect, m_WindowStyle, FALSE);

	m_WindowHandle = CreateWindowEx(0, m_ClassName.c_str(), m_Title.c_str(), m_WindowStyle, 50, 50, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, 0, 0, GetModuleHandle(nullptr), this);
	if (m_WindowHandle == NULL)
		return false;

	return true;
}

bool Window::InitOpenGL()
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	HDC deviceContext = GetDC(m_WindowHandle);

	int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
	if (pixelFormat == 0)
	{
		std::cout << "Pixel format could not be chosen!" << std::endl;
		return false;
	}
	SetPixelFormat(deviceContext, pixelFormat, &pfd);

	m_OpenGLRenderingContext = wglCreateContext(deviceContext);
	if (m_OpenGLRenderingContext == NULL)
	{
		std::cout << "m_OpenGLRenderingContext was NULL!" << std::endl;
		return false;
	}
	if (wglMakeCurrent(deviceContext, m_OpenGLRenderingContext) == FALSE)
	{
		std::cout << "Could not make context current!" << std::endl;
		return false;
	}

	ReleaseDC(m_WindowHandle, deviceContext);

	gladLoadGL();

	return true;
}

void Window::Show()
{
	ShowWindow(m_WindowHandle, TRUE);
}

void Window::Update()
{
	HDC deviceContext = GetDC(m_WindowHandle);
	SwapBuffers(deviceContext);
}

void Window::SetFullscreen(bool fullscreen)
{
	m_IsFullscreen = fullscreen;

	if (m_IsFullscreen)
	{
		int monitorWidth = GetSystemMetrics(SM_CXSCREEN);
		int monitorHeight = GetSystemMetrics(SM_CYSCREEN);
		m_WindowStyle = WS_VISIBLE;
		SetWindowLongPtr(m_WindowHandle, GWL_STYLE, m_WindowStyle);
		m_Width = monitorWidth;
		m_Height = monitorHeight;
		SetWindowPos(m_WindowHandle, 0, 0, 0, m_Width, m_Height, SWP_SHOWWINDOW);
	}
	else 
	{
		m_WindowStyle = WS_OVERLAPPEDWINDOW;
		SetWindowLongPtr(m_WindowHandle, GWL_STYLE, m_WindowStyle);
		m_Width = 1280;
		m_Height = 720;

		RECT windowRect{};
		windowRect.left = 0; // When this is set to 0, it becomes negative, so then in the SetWindowPos - We "add" the bottom to top so the border is taken into account
		windowRect.top = 0;
		windowRect.right = m_Width;
		windowRect.bottom = m_Height;
		AdjustWindowRect(&windowRect, m_WindowStyle, FALSE);

		// Place the window at 0, 0, we just re-center the window after
		SetWindowPos(m_WindowHandle, 0, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_SHOWWINDOW | SWP_DRAWFRAME);
		CenterWindow();
	}	
	SetHasResized(true);
}

void Window::CenterWindow()
{
	if (m_IsFullscreen)
		return;

	int monitorWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	int monitorHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	
	int posX = (monitorWidth / 2) - (m_Width / 2);
	int posY = (monitorHeight / 2) - (m_Height / 2);

	SetWindowPos(m_WindowHandle, NULL, posX, posY, 0, 0, SWP_NOSIZE);
	SetHasResized(true);
}

void Window::CenterCursor()
{
	RECT winRect{};
	GetWindowRect(m_WindowHandle, &winRect);
	m_MiddleX = winRect.left + (m_Width / 2);
	m_MiddleY = winRect.top + (m_Height / 2);

	SetCursorPos(m_MiddleX, m_MiddleY);
}

void Window::SetTitle(const std::wstring& title)
{
	m_Title = title;
	SetWindowText(m_WindowHandle, m_Title.c_str());
}

LRESULT Window::WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) 
		return true;

	switch (msg)
	{
	case WM_SIZE:
		SetHasResized(true);
		if (wParam == SIZE_MINIMIZED)
			break;

		m_Width = LOWORD(lParam);
		m_Height = HIWORD(lParam);
		break;

	//case WM_ACTIVATEAPP:
	//	SetMouseCaptured(wParam);
	//	break; 


	//case WM_SETFOCUS:
	//	SetMouseCaptured(true);
	//	break;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_INPUT:
	{

		Input::HandleRawInput(wParam, lParam);
		break;
	}
	case WM_KEYDOWN:
	case WM_KEYUP:
		Input::HandleKeysInput(msg, wParam, lParam);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static Window* windowInstance = nullptr;
	
	

	if(msg ==  WM_NCCREATE) {
		CREATESTRUCT* pcs = (CREATESTRUCT*)lParam; 
		windowInstance = (Window*)pcs->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pcs->lpCreateParams);
		return TRUE;
	}
	else 
	{
		if (windowInstance)
			windowInstance->WindowProcedure(hwnd, msg, wParam, lParam);
	}
	

	return 0;
}
