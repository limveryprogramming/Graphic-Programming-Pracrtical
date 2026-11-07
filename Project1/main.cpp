#include <Windows.h>
#include <gl/GL.h>
#include <math.h>

#pragma comment (lib, "OpenGL32.lib")

#define WINDOW_TITLE "OpenGL Window"

// Global variables

const float PI = 3.14159265f;

int   gWidth = 800;        // Current client width  (updated in WM_SIZE)
int   gHeight = 600;        // Current client height (updated in WM_SIZE)

float colourTime = 0.0f;    // Drives the automatic colour change

//--------------------------------------------------------------------
LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		gWidth = LOWORD(lParam);
		gHeight = HIWORD(lParam);
		if (gHeight == 0) gHeight = 1;
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------

bool initPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// ChoosePixelFormat returns the index of the closest matching pixel format
	int n = ChoosePixelFormat(hdc, &pfd);

	// SetPixelFormat returns whether the pixel format was set successfully
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------

// Draw a filled 5-point star centred at (cx, cy), pointing up.
// outerR = tip radius, innerR = inner vertex radius.
// Centre colour is (cr,cg,cb); tips use (tr,tg,tb) so the star has a gradient.
void DrawStar(float cx, float cy, float outerR, float innerR, float cr, float cg, float cb, float tr, float tg, float tb)
{
	glBegin(GL_TRIANGLE_FAN);

	// Centre of the fan
	glColor3f(cr, cg, cb);
	glVertex2f(cx, cy);

	// 10 vertices alternating between tip (outer) and inner points
	for (int i = 0; i <= 10; i++)
	{
		float radius = (i % 2 == 0) ? outerR : innerR;
		float a = PI / 2.0f + i * (PI / 5.0f);   // start pointing up

		glColor3f(tr, tg, tb);
		glVertex2f(cx + radius * cosf(a), cy + radius * sinf(a));
	}

	glEnd();
}
//--------------------------------------------------------------------

void update()
{
	colourTime += 0.02f;     // Colour cycling speed
}
//--------------------------------------------------------------------

void display()
{
	// Keep the aspect ratio so the star never looks stretched
	float aspect = (float)gWidth / (float)gHeight;
	glViewport(0, 0, gWidth, gHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Three sine waves shifted by 120 degrees give a smooth rainbow cycle
	float r = 0.5f + 0.5f * sinf(colourTime);
	float g = 0.5f + 0.5f * sinf(colourTime + 2.0944f);
	float b = 0.5f + 0.5f * sinf(colourTime + 4.1888f);

	// Star: white centre, tips use the cycling colour
	DrawStar(0.0f, 0.0f, 0.6f, 0.25f,
		1.0f, 1.0f, 1.0f, r, g, b);
}
//--------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WindowProcedure;
	wc.lpszClassName = WINDOW_TITLE;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc)) return false;

	HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	HDC hdc = GetDC(hWnd);

	// Initialize pixel format for the window
	initPixelFormat(hdc);

	// Get an OpenGL context
	HGLRC hglrc = wglCreateContext(hdc);

	// Make the context current
	if (!wglMakeCurrent(hdc, hglrc)) return false;

	//--------------------------------
	//	End initialization
	//--------------------------------

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		update();      // Advance the colour
		display();     // Draw the scene

		SwapBuffers(hdc);
		Sleep(16);     // About 60 frames per second
	}

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------