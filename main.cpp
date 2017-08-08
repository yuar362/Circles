/*--------------------------------------------------------------------
Circle moving in a window

Win32 приложение на C++ с использованием Win32 API + GDI, которое:
- представляет из себя окно, с изменяемыми размерами, цвет фона белый;
- в окне отрисовывается мячик-кружок радиусом в 5-10 пикселов черного цвета;
- мячик движется в клиентской области окна с постоянной скоростью и направлением, отражаясь от границ окна
- при каждом отражении скорость мячика изменяется в случайном диапазоне, но не становится равной нулю или чрезмерно большой
- пользователь приложения имеет возможность кликнуть левой кнопкой мыши в клиентской области окна и поставить препятствие - кружочек случайного радиуса 5-100 пикселов красного цвета;
- препятствий можно поставить сколько угодно;
- при столкновении мячика с препятствием препятствие уничтожается (исчезает), а мячик продолжает свое движение без изменений;
(c)Yuriy Anoshin, 2017
--------------------------------------------------------------------*/
#include "Game.h"


#define RADIUS 8
#define VELOCITY 10
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
POINT ptPrevious = { 0 };
HBRUSH brush_black, brush_red;
HPEN pen_black, pen_red;
static POINT    ptCenter;

std::multimap<long int, std::shared_ptr<SimpleCircle>> x_sorted_circles;
std::multimap<long int, std::shared_ptr<SimpleCircle>> y_sorted_circles;
std::shared_ptr < KillerCircle> k_c = nullptr;

HINSTANCE hInst;                                // current instance

												//
												//  FUNCTION: DlgProc(HWND, unsigned, WORD, LONG)
												//
												//  PURPOSE:  Processes messages for the Dialog window.
												//
												//  WM_COMMAND  - process the application menu
												//  WM_PAINT    - Paint the main window
												//  WM_DESTROY  - post a quit message and return
												//
												//

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int iCmdShow)
{
	static  TCHAR szAppName[] = TEXT("Awesome circles");
	HWND        hwnd;
	MSG         msg;
	WNDCLASS    wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName,                         // window class name
		TEXT("Awesome circles"), // window caption
		WS_OVERLAPPEDWINDOW,               // window style
		CW_USEDEFAULT,                     // initial x position
		CW_USEDEFAULT,                     // initial y position
		CW_USEDEFAULT,                     // initial x size
		CW_USEDEFAULT,                     // initial y size
		NULL,                              // parent window handle
		NULL,                              // window menu handle
		hInstance,                         // program instance handle
		NULL);                             // creation parameters
    
    SetTimer(hwnd, 1, 200, nullptr);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Win32API GDI circle:
void Win32APIGDICircle(HDC hdc, LONG radius, POINT ptCenter)
{
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	Ellipse(hdc,
		ptCenter.x - RADIUS,
		ptCenter.y + RADIUS,
		ptCenter.x + RADIUS,
		ptCenter.y - RADIUS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HPEN hPen = NULL;
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC             hdc;
	PAINTSTRUCT     ps;
	
    switch (message)
	{
	case WM_TIMER:
        Game::GetGame(hwnd).CalculateChange();		
		break;
	case WM_SIZE:
        Game::GetGame(hwnd).ChangeBorder();        
		break;
	case WM_PAINT:		
		hdc = BeginPaint(hwnd, &ps);
        Game::GetGame(hwnd).RedrawAll(hdc);
		EndPaint(hwnd, &ps);
		break;
    case WM_ENTERSIZEMOVE:
        Game::GetGame(hwnd).SetWindowMoving(true);
        break;
    case WM_EXITSIZEMOVE:
        Game::GetGame(hwnd).ChangeBorder();
        Game::GetGame(hwnd).SetWindowMoving(false);
        break;
	case WM_LBUTTONDOWN:
	{
		POINT click;
		click.x = LOWORD(lParam);
		click.y = HIWORD(lParam);
        Game::GetGame(hwnd).CreateSimpleCircle(click);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}


	return DefWindowProc(hwnd, message, wParam, lParam);
}

