#include <Windows.h>
#include <math.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "OpenGL32.lib") //Get the OpenGL library
#pragma comment (lib, "GLU32.lib") // link GLU32.lib

#define WINDOW_TITLE "OpenGL Window"

//Create a quadric object
GLUquadricObj* var = NULL;

//Global variable
bool lineStyle = false;
int slices = 20, stacks = 20;
float angle = 0.0f;

int topSteps = 6;

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
		else if (wParam == 'L') lineStyle = !lineStyle;
		//else if (wParam == VK_UP) if (slices < 60) { slices = slices + 2; stacks = stacks + 2; }
		//else if (wParam == VK_DOWN) if (slices > 4) { slices = slices - 2; stacks = stacks - 2; }
		else if (wParam == VK_UP && topSteps < 11) { topSteps = topSteps + 1; }
		else if (wParam == VK_DOWN && topSteps > 0) { topSteps = topSteps - 1; }
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//-------------------------------------------------------------------- Reusable code
//Describe the frame buffer
bool initPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//32-bit RGB-A colour
	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;

	//Depth
	pfd.cDepthBits = 24;

	pfd.cStencilBits = 0;

	//Double buffer
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// choose pixel format returns the number most similar pixel format available
	// Find the closest match, and get the pixel format
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
// 2D translate x y
// 2D rotate z (+ve is CCW, -ve is CW), according to origin NOT shape center
void display() //Runs every frame according to the CPU
{
	//--------------------------------
	//	OpenGL drawing
	//--------------------------------
	glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	angle = angle + 0.3f;
	glRotatef(angle, 0.0f, 1.0f, 0.0f); //spin about the y aixs

	glColor3f(1.0f, 0.8f, 0.2f);

	//2e
	if (lineStyle)	gluQuadricDrawStyle(var, GLU_LINE);
	else			gluQuadricDrawStyle(var, GLU_FILL);

	//2c
	//gluSphere(var, 0.7 slices,stacks);
	gluCylinder(var, 0.3, topSteps * 0.05 , 1.0 , 20 ,5);

	//--------------------------------
	//	End of OpenGL drawing
	//--------------------------------
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
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
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

	// The projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);

	var = gluNewQuadric(); //Create the quadric object ONCE!!!

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