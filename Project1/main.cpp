#include <Windows.h>
#include <gl/GL.h>
#include <math.h>
#include <stdio.h>

#pragma comment (lib, "OpenGL32.lib")

#define WINDOW_TITLE "OpenGL Window"

const float PI = 3.14159265f;

// ------------------------------------------------------------------
// Windmill controls
//   Left  arrow / A : spin anti-clockwise
//   Right arrow / D : spin clockwise
//   Up    arrow / W : accelerate
//   Down  arrow / S : decelerate
//   Space           : stop
//   Esc             : quit
// ------------------------------------------------------------------

float bladeAngle = 0.0f;     // current rotation of the blades (degrees)
float speed = 90.0f;         // spinning speed (degrees per second)
int   direction = -1;        // -1 = clockwise, +1 = anti-clockwise
const float SPEED_STEP = 30.0f;
const float MAX_SPEED = 720.0f;

LARGE_INTEGER freq, lastTime;   // for frame-rate independent spinning


//--------------------------------------------------------------------
LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);

		if (wParam == VK_LEFT || wParam == 'A') direction = 1;    // anti-clockwise
		if (wParam == VK_RIGHT || wParam == 'D') direction = -1;  // clockwise

		if (wParam == VK_UP || wParam == 'W')                     // accelerate
		{
			speed += SPEED_STEP;
			if (speed > MAX_SPEED) speed = MAX_SPEED;
		}
		if (wParam == VK_DOWN || wParam == 'S')                   // decelerate
		{
			speed -= SPEED_STEP;
			if (speed < 0.0f) speed = 0.0f;
		}
		if (wParam == VK_SPACE) speed = 0.0f;                     // stop
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

	int n = ChoosePixelFormat(hdc, &pfd);
	return SetPixelFormat(hdc, n, &pfd) ? true : false;
}
//--------------------------------------------------------------------
//  Basic shape helpers
//--------------------------------------------------------------------

void drawCircle(float cx, float cy, float r, int segments = 40)
{
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);
	for (int i = 0; i <= segments; i++)
	{
		float a = 2.0f * PI * i / segments;
		glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
	}
	glEnd();
}

void drawRect(float x1, float y1, float x2, float y2)
{
	glBegin(GL_QUADS);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
}

//--------------------------------------------------------------------
//  Background
//--------------------------------------------------------------------

void drawBackground()
{
	// sky
	glColor3f(0.53f, 0.81f, 0.92f);
	drawRect(-1.4f, -0.4f, 1.4f, 1.0f);

	// sun
	glColor3f(1.0f, 0.9f, 0.0f);
	drawCircle(1.0f, 0.75f, 0.12f);

	// clouds
	glColor3f(1.0f, 1.0f, 1.0f);
	drawCircle(-0.95f, 0.70f, 0.08f);
	drawCircle(-0.85f, 0.73f, 0.10f);
	drawCircle(-0.75f, 0.70f, 0.08f);

	drawCircle(0.45f, 0.80f, 0.07f);
	drawCircle(0.54f, 0.83f, 0.09f);
	drawCircle(0.63f, 0.80f, 0.07f);

	// grass
	glColor3f(0.3f, 0.7f, 0.2f);
	drawRect(-1.4f, -1.0f, 1.4f, -0.4f);

	// path to the door
	glColor3f(0.8f, 0.7f, 0.5f);
	glBegin(GL_QUADS);
	glVertex2f(-0.05f, -0.40f);
	glVertex2f(0.05f, -0.40f);
	glVertex2f(0.20f, -1.00f);
	glVertex2f(-0.20f, -1.00f);
	glEnd();
}

//--------------------------------------------------------------------
//  Windmill body (does not spin)
//--------------------------------------------------------------------

void drawTower()
{
	// tower (trapezium)
	glColor3f(0.6f, 0.4f, 0.2f);
	glBegin(GL_QUADS);
	glVertex2f(-0.25f, -0.40f);
	glVertex2f(0.25f, -0.40f);
	glVertex2f(0.15f, 0.30f);
	glVertex2f(-0.15f, 0.30f);
	glEnd();

	// roof (triangle)
	glColor3f(0.7f, 0.1f, 0.1f);
	glBegin(GL_TRIANGLES);
	glVertex2f(-0.20f, 0.30f);
	glVertex2f(0.20f, 0.30f);
	glVertex2f(0.00f, 0.50f);
	glEnd();

	// door
	glColor3f(0.35f, 0.2f, 0.1f);
	drawRect(-0.06f, -0.40f, 0.06f, -0.20f);
}

//--------------------------------------------------------------------
//  Blades (these spin)
//--------------------------------------------------------------------

void drawOneBlade()
{
	// one plain white blade
	glColor3f(1.0f, 1.0f, 1.0f);
	drawRect(-0.04f, 0.0f, 0.04f, 0.5f);
}

void drawBlades()
{
	glPushMatrix();
	glTranslatef(0.0f, 0.38f, 0.0f);        // move to the hub
	glRotatef(bladeAngle, 0.0f, 0.0f, 1.0f); // spin around the hub

	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glRotatef(i * 90.0f, 0.0f, 0.0f, 1.0f);
		drawOneBlade();
		glPopMatrix();
	}

	// hub
	glColor3f(0.3f, 0.3f, 0.3f);
	drawCircle(0.0f, 0.0f, 0.04f);

	glPopMatrix();
}

//--------------------------------------------------------------------

void display()
{
	// time since last frame, so the speed is the same on any computer
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	float dt = (float)(now.QuadPart - lastTime.QuadPart) / freq.QuadPart;
	lastTime = now;

	bladeAngle += direction * speed * dt;
	if (bladeAngle > 360.0f) bladeAngle -= 360.0f;
	if (bladeAngle < -360.0f) bladeAngle += 360.0f;

	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// keep shapes round on an 800x600 window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.333, 1.333, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	drawBackground();
	drawTower();
	drawBlades();
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
	initPixelFormat(hdc);
	HGLRC hglrc = wglCreateContext(hdc);
	if (!wglMakeCurrent(hdc, hglrc)) return false;

	//--------------------------------
	//	End initialization
	//--------------------------------

	ShowWindow(hWnd, nCmdShow);

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&lastTime);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	char title[128];
	DWORD lastTitleUpdate = 0;

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

		// show the current state in the title bar
		if (GetTickCount() - lastTitleUpdate > 100)
		{
			sprintf_s(title, "Windmill  |  %s  |  speed: %.0f deg/s   (A/D direction, W/S speed, Space stop)",
				speed == 0.0f ? "STOPPED" : (direction < 0 ? "CLOCKWISE" : "ANTI-CLOCKWISE"), speed);
			SetWindowTextA(hWnd, title);
			lastTitleUpdate = GetTickCount();
		}
	}

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------
