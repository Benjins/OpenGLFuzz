#include <stdio.h>

#include "fuzzr.h"


#include <algorithm>
#include <random>
#include <chrono>

#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "Opengl32.lib")


// I guess this is more accurately an OpenGL ES fuzzer...or at least that's how I'll be using it

typedef HGLRC(APIENTRYP PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC,
	HGLRC
	hShareContext,
	const int
	* attribList);

#define WGL_CONTEXT_MAJOR_VERSION_ARB   0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB   0x2092


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

	const int32 ViewportWidth = 1280;
	const int32 ViewportHeight = 720;

	RECT winRect = { 0 };
	winRect.right = ViewportWidth;
	winRect.bottom = ViewportHeight;
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

	//HGLRC
	{


		HGLRC glContext = wglCreateContext(hdc);
		//wglCreateContextAttribsARB;// (hdc, );
		if (!wglMakeCurrent(hdc, glContext)) {
			OutputDebugStringA("OIh god, no OpenGL cont3ext.\n  :O");
		}

		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;

		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

		int attributes[8] = {};
		int attributeCount = 0;

		attributes[attributeCount++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
		attributes[attributeCount++] = 3;
		attributes[attributeCount++] = WGL_CONTEXT_MINOR_VERSION_ARB;
		attributes[attributeCount++] = 3;


		HGLRC glContextNew = wglCreateContextAttribsARB(hdc, 0, attributes);
		wglMakeCurrent(hdc, glContextNew);
		wglDeleteContext(glContext);
	}

#define WIN_DEBUG_PRINT(format, ...) do { char buffer[1024]; snprintf(buffer, sizeof(buffer), format, ##  __VA_ARGS__); printf("%s", buffer); OutputDebugStringA(buffer); } while(0)
	WIN_DEBUG_PRINT("GL version: %s\n", glGetString(GL_VERSION));
	WIN_DEBUG_PRINT("GL vendor: %s\n", glGetString(GL_VENDOR));
	WIN_DEBUG_PRINT("GL Renderer: %s\n", glGetString(GL_RENDERER));
#undef WIN_DEBUG_PRINT

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	auto rd = std::default_random_engine(seed);

	GLCtxState Ctx;
	Ctx.InitFuncPtrs();

	static float PositionData[] = {
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
	};

	auto start = std::chrono::high_resolution_clock::now();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH);
	glDisable(GL_BLEND);
	glCullFace(GL_NONE);

	glViewport(0, 0, ViewportWidth, ViewportHeight);
	glScissor(0, 0, ViewportWidth, ViewportHeight);

	const int32 NumIters = 10 * 1000;
	for (int32 i = 0; i < NumIters; i++) {
		
		// This is for...compatibility? Core? Forward-compat? 3.3? 4.0? 3.0?
		// I don't know. I think we should remove this for GLES testing, but for now it lets us render it on desktop
		GLuint vao;
		Ctx.glGenVertexArrays_func(1, &vao);
		Ctx.glBindVertexArray_func(vao);
		
		std::vector<GLCmd> Commands;
		Commands.push_back(GLCmdMakeTexture(0));
		Commands.push_back(GLCmdMakeTexture(1));

		Commands.push_back(GLCmdActiveTextureSlot(0));
		Commands.push_back(GLCmdBindTexture(0));
		Commands.push_back(GLCmdSetTextureMinFilter(TexFilter::LINEAR));
		Commands.push_back(GLCmdSetTextureMagFilter(TexFilter::NEAREST));

		Commands.push_back(GLCmdActiveTextureSlot(1));
		Commands.push_back(GLCmdBindTexture(1));
		Commands.push_back(GLCmdSetTextureMinFilter(TexFilter::LINEAR));
		Commands.push_back(GLCmdSetTextureMagFilter(TexFilter::NEAREST));

		Commands.push_back(GLCmdMakeShader(0, ShaderType::Vertex));
		Commands.push_back(GLCmdMakeShader(1, ShaderType::Pixel));
		Commands.push_back(GLCmdSetShaderSource(0, "#version 330\nattribute vec4 pos; void main() { gl_Position = pos; }"));
		Commands.push_back(GLCmdSetShaderSource(1, "#version 330\nvoid main() { gl_FragColor = vec4(0.2, 0.6, 0.5, 1.0); }"));
		Commands.push_back(GLCmdCompileShader(0));
		Commands.push_back(GLCmdCompileShader(1));
		Commands.push_back(GLCmdMakeProgram(0));
		Commands.push_back(GLCmdAttachShader(0, 0));
		Commands.push_back(GLCmdAttachShader(0, 1));
		Commands.push_back(GLCmdLinkProgram(0));
		Commands.push_back(GLCmdValidateProgram(0));
		Commands.push_back(GLCmdUseProgram(0));

		Commands.push_back(GLCmdMakeBuffer(0));
		Commands.push_back(GLCmdBindBuffer(0));
		Commands.push_back(GLCmdBufferData(sizeof(PositionData), PositionData, false));

		Commands.push_back(GLCmdGetVertexAttribLocation(0, 0, "pos"));
		Commands.push_back(GLCmdVertexAttribPointer(0, 4));
		Commands.push_back(GLCmdEnableVertexAttrib(0));

		Commands.push_back(GLCmdDrawArrays(6));

		Commands.push_back(GLCmdDisableVertexAttrib(0));

		Commands.push_back(GLCmdDestroyBuffer(0));

		Commands.push_back(GLCmdDestroyShader(0));
		Commands.push_back(GLCmdDestroyShader(1));
		Commands.push_back(GLCmdDestroyProgram(0));

		Commands.push_back(GLCmdDestroyTexture(1));
		Commands.push_back(GLCmdDestroyTexture(0));

		// NOTE: This will crash Intel drivers if you let it run
		//std::shuffle(Commands.begin(), Commands.end(), rd);

		ExecuteGLCmds(&Ctx, Commands);

		glFinish();

		SwapBuffers(hdc);

		Ctx.Reset();

		// Same as above: this shouldn't be needed...but it is
		Ctx.glDeleteVertexArrays_func(1, &vao);
	}

	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

	printf("It took %3.2fms to run %d iterations\n", time_span.count() * 1000.0, NumIters);

	return 0;
}



LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hwnd, message, wParam, lParam);
}
