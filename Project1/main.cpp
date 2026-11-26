#include <Windows.h>
#include <math.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "OpenGL32.lib") //Get the OpenGL library
#pragma comment (lib, "GLU32.lib") // link GLU32.lib

#define WINDOW_TITLE "OpenGL Window"

//Create a quadric object
GLUquadricObj* var = NULL;
float objZ = 0.0f;//practical5  //how far the object is along z(negative = farther away
double fovy = 60.0;			// field of view (degrees) for perspective
bool useFrustum = false;	// F key: glFrustum vs gluPerspective


LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
		if (wParam == VK_UP) objZ = objZ - 1.0f;    //farther away
		if (wParam == VK_DOWN) objZ = objZ + 1.0f;  //nearer
		if (wParam == VK_LEFT && fovy > 20.0) fovy = fovy - 10.0;
		if (wParam == VK_RIGHT && fovy < 120.0) fovy = fovy + 10.0;
		if (wParam == 'F') useFrustum = !useFrustum;

		if (wParam == VK_LEFT || wParam == VK_RIGHT || wParam == 'F')	// the projection changed
		{
			glMatrixMode(GL_PROJECTION);								// sheet 2a
			glLoadIdentity();
			if (useFrustum)
			{
				double top = 1.0 * tan(fovy / 2.0 * 3.14159265358979 / 180.0);	// near x tan(fovy/2)
				double right = top * 1.0;										// top x aspect
				glFrustum(-right, right, -top, top, 1.0, 20.0);				// sheet 2c
			}
			else
			{
				gluPerspective(fovy, 1.0, 1.0, 20.0);							// sheet 2b
			}
			glMatrixMode(GL_MODELVIEW);
		}

		if (wParam >= '1' && wParam <= '4')  //test different viewing volumes
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			if (wParam == '1') glOrtho(-1.0, 1.0, -1.0, 1.0, -10.0, 10.0);
			if (wParam == '2') glOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
			if (wParam == '3') glOrtho(-1.0, 1.0, -2.0, 2.0, -10.0, 10.0);
			if (wParam == '4') glOrtho(-0.5, 1.5, -1.0, 1.0, -10.0, 10.0);
			glMatrixMode(GL_MODELVIEW);
		}
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
	glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	static float angle = 0.0f;
	angle = angle + 0.3f;

	glTranslatef(0.0f, 0.0f, objZ);			// UP / DOWN key moves it along z
	glRotatef(15.0f, 1.0f, 0.0f, 0.0f);		// tilt a bit so we can see the top
	glRotatef(angle, 0.0f, 1.0f, 0.0f);		// spin about the y axis

	gluQuadricDrawStyle(var, GLU_FILL);

	//---------------- Cone (point facing down) ----------------
	glPushMatrix();
	glColor3f(0.85f, 0.6f, 0.3f);			// waffle colour
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);		// cylinder z-axis -> -y
	gluCylinder(var, 0.3, 0.0, 0.9, 20, 20);
	glPopMatrix();

	//---------------- Scoop 1 : strawberry (bottom left) ----------------
	glPushMatrix();
	glColor3f(1.0f, 0.55f, 0.7f);
	glTranslatef(-0.14f, 0.1f, 0.0f);
	gluSphere(var, 0.22, 20, 20);
	glPopMatrix();

	//---------------- Scoop 2 : vanilla (bottom right) ----------------
	glPushMatrix();
	glColor3f(1.0f, 0.95f, 0.75f);
	glTranslatef(0.14f, 0.1f, 0.0f);
	gluSphere(var, 0.22, 20, 20);
	glPopMatrix();

	//---------------- Scoop 3 : chocolate (top) ----------------
	glPushMatrix();
	glColor3f(0.45f, 0.25f, 0.12f);
	glTranslatef(0.0f, 0.4f, 0.0f);
	gluSphere(var, 0.22, 20, 20);
	glPopMatrix();
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
	gluPerspective(60.0,1.0,1.0,20.0);
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

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------