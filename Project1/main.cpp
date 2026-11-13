#include <Windows.h>
#include <gl/GL.h>
#include <math.h>

#pragma comment (lib, "OpenGL32.lib")

#define WINDOW_TITLE "OpenGL Window"


// Global variables: object position and colour

struct Colour { float r, g, b; };

const Colour RED = { 1, 0, 0 };
const Colour GREEN = { 0, 1, 0 };
const Colour BLUE = { 0, 0, 1 };
const Colour WHITE = { 1, 1, 1 };

float objX = 0.0f;          // Object x position
float objY = 0.0f;          // Object y position
Colour objColour = RED;     // Current object colour (starts as a red dot)

bool depthTest = false;
bool clearDepth = true;
bool culling = false;
float angle = 0.0f;


//--------------------------------------------------------------------
LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		if (wParam == 'D') depthTest= !depthTest;
		if (wParam == 'C') clearDepth = !clearDepth;
		if (wParam == 'K') culling = !culling;
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


void Draw3DCube() {
	glBegin(GL_QUADS);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//front
	glColor3f(1, 0, 0);
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glVertex3f(1, 0, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(0, 1, 1);

	//back
	glColor3f(0, 1, 0);
	glNormal3f(0, 0, -1);
	glVertex3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);
	glVertex3f(1, 1, 0);

	//left face
	glColor3f(0, 0, 1);
	glNormal3f(-1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 1, 1);
	glVertex3f(0, 1, 0);

	//right face
	glColor3f(1, 1, 0);
	glNormal3f(1, 0, 0);
	glVertex3f(1, 0, 1);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 1, 0);
	glVertex3f(1, 1, 1);

	//top
	glColor3f(0, 1, 1);
	glNormal3f(0, 1, 0);
	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 1, 0);
	glVertex3f(0, 1, 0);

	//down
	glColor3f(1, 0, 1);
	glNormal3f(0, -1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 0, 1);
	glVertex3f(0, 0, 1);

	glEnd();
}

void display()
{
	glClearColor(0.007, 0.00, 0.10f, 1.0f);

	// clear the colour buffer ANO the depth buffer 
	if (clearDepth) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else			glClear(GL_COLOR_BUFFER_BIT);

	//glEnable(CL CEPTH TEST). In a normal progran you call it ONCE
	if (depthTest) glEnable(GL_DEPTH_TEST); 
	else glDisable(GL_DEPTH_TEST);

	// EXTRA: ds net draw the faces that peint anay fren us (the back faces) 
	if (culling) glEnable(GL_CULL_FACE); 
	else glDisable (GL_CULL_FACE);

	glLoadIdentity(); 
	angle = angle  + 0.034;
	glRotatef(angle, 1.0f, 1.0f, 0.0f); 

	// spin the cube so we can see all its sides
	Draw3DCube();
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

		display();

		SwapBuffers(hdc);
	}

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------