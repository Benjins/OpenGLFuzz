#include <stdio.h>

#include "fuzzr.h"


#include <algorithm>
#include <random>
#include <chrono>

#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "Opengl32.lib")


// I guess this is more accurately an OpenGL ES fuzzer...or at least that's how I'll be using it


LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int main() {
	printf("Test yoooo.\n");

	HINSTANCE instance = GetModuleHandle(0);

	WNDCLASS windowCls = {};
	windowCls.hInstance = instance;
	windowCls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	windowCls.lpfnWndProc = MainWindowProc;
	windowCls.lpszClassName = L"bns-glfuzz-window";

	RegisterClass(&windowCls);

	RECT winRect = { 0 };
	winRect.right = 640;
	winRect.bottom = 480;
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, false);

	LONG winWidth = winRect.right - winRect.left;
	LONG winHeight = winRect.bottom - winRect.top;

	HWND window = CreateWindow(windowCls.lpszClassName, L"GLFuzz", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 50, 50, winWidth, winHeight, 0, 0, instance, 0);

	HDC hdc = GetDC(window);

	PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
	desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
	desiredPixelFormat.nVersion = 1;
	desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	desiredPixelFormat.cColorBits = 32;
	desiredPixelFormat.cAlphaBits = 8;
	desiredPixelFormat.cDepthBits = 32;
	desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;

	int actualFormatIndex = ChoosePixelFormat(hdc, &desiredPixelFormat);
	PIXELFORMATDESCRIPTOR actualPixelFormat;
	DescribePixelFormat(hdc, actualFormatIndex, sizeof(actualPixelFormat), &actualPixelFormat);
	SetPixelFormat(hdc, actualFormatIndex, &actualPixelFormat);

	HGLRC glContext = wglCreateContext(hdc);
	if (!wglMakeCurrent(hdc, glContext)) {
		OutputDebugStringA("OIh god, no OpenGL cont3ext.\n  :O");
	}

#define WIN_DEBUG_PRINT(format, ...) do { char buffer[1024]; snprintf(buffer, sizeof(buffer), format, ##  __VA_ARGS__); OutputDebugStringA(buffer); } while(0)
	WIN_DEBUG_PRINT("GL version: %s\n", glGetString(GL_VERSION));
	WIN_DEBUG_PRINT("GL vendor: %s\n", glGetString(GL_VENDOR));
	WIN_DEBUG_PRINT("GL Renderer: %s\n", glGetString(GL_RENDERER));
#undef WIN_DEBUG_PRINT

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	auto rd = std::default_random_engine(seed);

	GLCtxState Ctx;
	for (int32 i = 0; i < 100; i++) {
		std::vector<GLCmd> Commands;
		Commands.push_back(GLCmdMakeTexture(0));
		Commands.push_back(GLCmdMakeTexture(1));
		Commands.push_back(GLCmdDestroyTexture(1));
		Commands.push_back(GLCmdDestroyTexture(0));

		std::shuffle(Commands.begin(), Commands.end(), rd);

		ExecuteGLCmds(&Ctx, Commands);

		Ctx.Reset();
	}


	return 0;
}



LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hwnd, message, wParam, lParam);
}
