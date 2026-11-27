#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "OpenGL32.lib") //Get the OpenGL library
#pragma comment (lib, "GLU32.lib") // link GLU32.lib

#define WINDOW_TITLE "Practical 6 lighting and material"

GLUquadricObj* var = NULL;  //the quadric object (practical 4B)
bool light0On = true;       //L switches this
bool lightingOn = true;     //G switches this
int material = 0;           //M cycles this: 0 white, 1 green, 2 red, 3 yellow

GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };   //i.   a little white ambient light
GLfloat lightDiffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };   //ii.  RED diffuse light
GLfloat lightPosition[] = { 1.0f, 1.0f, 1.0f, 0.0f };  //iii. x,y,z,w (w = 0: directional light, rays come from (1,1,1))

//the 4 materials M cycles through
GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };   //0
GLfloat green[] = { 0.0f, 1.0f, 0.0f, 1.0f };   //1
GLfloat red[] = { 1.0f, 0.0f, 0.0f, 1.0f };     //2
GLfloat yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };  //3

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
		if (wParam == 'L') light0On = !light0On;
		if (wParam == 'G') lightingOn = !lightingOn;
		if (wParam == 'M') material = (material + 1) % 4;
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
void display() //Runs every frame according to the CPU
{
	glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//sheet 1b: the properties of light 0 (set while the modelview matrix is the identity)
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	//sheet 1c, 1e: switch light 0 on or off (L key)
	if (light0On) glEnable(GL_LIGHT0);
	else          glDisable(GL_LIGHT0);

	//sheet 1d: switch the whole lighting system on or off (G key)
	if (lightingOn) glEnable(GL_LIGHTING);
	else            glDisable(GL_LIGHTING);

	// sheet 2b: the material of the objects drawn after these lines (M key)
	GLfloat* colour = white;
	if (material == 1) colour = green;
	if (material == 2) colour = red;
	if (material == 3) colour = yellow;
	glMaterialfv(GL_FRONT, GL_AMBIENT, colour);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colour);

	glColor3f(0.7f, 0.7f, 0.7f);		// used only when lighting is off
	gluSphere(var, 0.7, 40, 40);
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
	RECT rc = { 0,0,640,640 };
	HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
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
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
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

	gluDeleteQuadric(var);
	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------
