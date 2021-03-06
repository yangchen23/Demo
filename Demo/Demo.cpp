//Demo.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Demo.h"
#define MAX_LOADSTRING 100
int map[16][16],dir[8][2]={1,1,1,0,1,-1,0,-1,-1,-1,-1,0,-1,1,0,1}, gameover = 0;

// 颜色
COLORREF gray = RGB(0xCC, 0xCC, 0xCC);
COLORREF background_color = RGB(0xCC, 0x99, 0x66);
COLORREF white = RGB(0xCC, 0xCC, 0xCC);
COLORREF black = RGB(0x0, 0x0, 0x0);

// 笔刷
HBRUSH gray_brush = CreateSolidBrush(gray);
HBRUSH background_brush = CreateSolidBrush(background_color);
HBRUSH white_brush = GetSysColorBrush(white);
HBRUSH black_brush = CreateSolidBrush(black);

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Win(HWND, UINT, WPARAM, LPARAM);
void                DrawChessboard(HDC, LPRECT);
void                GetClickedPoint(LPPOINT, LPRECT,int *,int *,int );
void                DrawPiece(HWND, HDC hdc, int, int, int);
void                DrawAllPieces(HWND, HDC);  //重绘棋子
int                 Judge(int , int);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEMO));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DEMO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
	// 获取桌面大小
   HWND hDesktop = GetDesktopWindow();
   RECT rectDesktop;
   GetWindowRect(hDesktop, &rectDesktop);
	//设置窗口  窗口名  起点.x  起点.y  长度.x  长度.y  是否repaint
   MoveWindow(hWnd, (rectDesktop.right - rectDesktop.left) / 16 * 3, rectDesktop.top, (rectDesktop.right - rectDesktop.left) / 8 * 5, (rectDesktop.bottom - rectDesktop.top) / 16 * 15, TRUE);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int count = 0;
	int x, y, r;//x y是鼠标点击棋盘上的点的坐标   r是控制半径  count判断谁下棋
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			//设置窗口背景颜色
			RECT rect;
			GetClientRect(hWnd, &rect);
			if (gameover == 1)
				SelectObject(hdc, gray_brush);
			else
				SelectObject(hdc, background_brush);
			PatBlt(hdc, 0, 0, rect.right, rect.bottom, PATCOPY);

    		// TODO: 在此处添加使用 hdc 的任何绘图代码...
			DrawChessboard(hdc,&rect);
			DrawAllPieces(hWnd, hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_LBUTTONUP:
		if (!gameover) 
		{
			RECT rect;
			GetClientRect(hWnd, &rect);
			POINT point;
			GetCursorPos(&point);
			ScreenToClient(hWnd, &point);
			GetClickedPoint(&point, &rect, &x, &y, (rect.bottom - rect.top) / 32);
			if (x != 0 && y != 0 && map[x][y] == 0)
			{
				map[x][y] = count % 2 == 0 ? 1 : -1;
				DrawPiece(hWnd, GetDC(hWnd), x, y, count);
				if (Judge(x, y))
				{
					gameover = 1;
					InvalidateRect(hWnd, &rect, TRUE);
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_WIN), hWnd, Win);
				}
				count++;
			}
		}
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Win(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void DrawChessboard(HDC hdc, LPRECT rect)
{
	for (int i = 1; i <= 15; i++)
	{
		//使用double使键盘与棋子对齐
		double x = (rect->right - rect->left) / 16.0;
		double y = (rect->bottom - rect->top) / 16.0;
		MoveToEx(hdc, x, y * i, NULL);
		LineTo(hdc, x * 15, y * i);
		MoveToEx(hdc, x * i, y, NULL);
		LineTo(hdc, x * i, y * 15);
	}
}

void GetClickedPoint(LPPOINT point, LPRECT rect, int *x, int *y, int r)
{
	for (int i = 1; i <= 15; i++)
	{
		for (int j = 1; j <= 15; j++)
		{
			if ((((rect->right - rect->left) / 16 * i - point->x)*((rect->right - rect->left) / 16 * i - point->x) + ((rect->bottom - rect->top) / 16 * j - point->y)*((rect->bottom - rect->top) / 16 * j - point->y)) <= r * r)
			{
				*x = i;
				*y = j;
				return;
			}
		}
	}
	*x = 0;
	*y = 0;
}

void DrawAllPieces(HWND hWnd, HDC hdc)
{
	for (int i = 1; i < 16; i++)
	{
		for (int j = 1; j < 16; j++)
		{
			if (map[i][j] == 0)
				continue;
			DrawPiece(hWnd, GetDC(hWnd), i, j, map[i][j] == 1 ? 0 : 1);
		}
	}
}

void DrawPiece(HWND hWnd, HDC hdc, int x, int y, int count)
{
	//PAINTSTRUCT ps;
	//HDC hdc = BeginPaint(hWnd, &ps);
	RECT rect;
	GetClientRect(hWnd, &rect);

	//调试方案
	//MoveToEx(hdc, rect->left, rect->top, NULL);
	//LineTo(hdc, rect->right, rect->bottom);
	
	if (count % 2 == 0)
		SelectObject(hdc, white_brush);
	else
		SelectObject(hdc, black_brush);

	Ellipse(hdc, x*(rect.right - rect.left) / 16 - (rect.right - rect.left) / 48, y*(rect.bottom - rect.top) / 16 - (rect.bottom - rect.top) / 48, x*(rect.right - rect.left) / 16 + (rect.right - rect.left) / 48, y*(rect.bottom - rect.top) / 16 + (rect.bottom - rect.top) / 48);

	//EndPaint(hWnd, &ps);
}

int Judge(int x, int y)
{
	int judges[3][3];
	for (int k = 0; k < 8; k++)
	{
		int ans = 0, tx = x, ty = y;
		for (int i = 0; i < 5; i++)
		{
			tx += dir[k][0];
			ty += dir[k][1];
			if (tx < 1 || tx>15 || ty < 1 || ty>15)
				break;
			if (map[tx][ty] != map[x][y])
				break;
			ans++;
		}
		judges[dir[k][0] + 1][dir[k][1] + 1] = ans;
	}
	if ((judges[0][0] + judges[2][2] >= 4) || (judges[0][1] + judges[2][1] >= 4) || (judges[0][2] + judges[2][0] >= 4) || (judges[1][0] + judges[1][2] >= 4))
		return 1;
	else
		return 0;
}