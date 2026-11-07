#include <Windows.h>
#include <gl/GL.h>
#include <math.h>

#pragma comment (lib, "OpenGL32.lib")

#define WINDOW_TITLE "OpenGL Window"


LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
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

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
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
struct Colour { float r, g, b; };
Colour RED = { 1,0,0 };
Colour GREEN = { 0,1,0 };
Colour BLUE = { 0,0,1 };
Colour BLACK = { 0,0,0 };
Colour WHITE = { 1,1,1 };
Colour YELLOW = { 1,1,0 };


void DrawCircle(float r,Colour c) {

	//DRAW CIRCLE
	glColor3f(c.r, c.g, c.b);

	double radius = r;
	glBegin(GL_TRIANGLE_FAN);

	for (int angle = 0;angle < 360; angle++) {
		float x = radius * cos(angle);
		float y = radius * sin(angle);

		glVertex2f(x, y);
	}

	glEnd();
}

void DrawRectangle(float x, float y, float w, float h, Colour c) {

	glColor3f(c.r, c.g, c.b);
	glBegin(GL_QUADS);
	
	glVertex2f(x, y + h);   // top left
	glVertex2f(x, y);       // bottom left
	glVertex2f(x + w, y);       // bottom right
	glVertex2f(x + w, y + h);   // top right

	glEnd();
}


void DrawTriangle(float x1, float y1,float x2, float y2,float x3, float y3,Colour c) {
	glColor3f(c.r, c.g, c.b);
	glBegin(GL_TRIANGLES);

	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glEnd();
}

void DrawLine(float x1, float y1, float x2, float y2, Colour c) {
	glColor3f(c.r, c.g, c.b);
	glLineWidth(5);
	glBegin(GL_LINES);

	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}
void display()
{
	
	glClear(GL_COLOR_BUFFER_BIT);

	//draw function
	DrawRectangle(-1,-1,2,2,WHITE);

	DrawCircle(0.8,YELLOW);

	//FACE
	//EYES
	DrawLine(-0.6, 0.4, -0.2, 0.4 , BLACK);
	DrawLine(0.6, 0.4, 0.2, 0.4, BLACK);
	DrawLine(-0.6, -0.4, 0.6, -0.4, BLACK);


}
//--------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) //espect racio
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

	//	initialize pixel format for the window
	initPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
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