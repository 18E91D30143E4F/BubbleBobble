// BubbleBobble.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "BubbleBobble.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HWND mhWnd;
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

Sprite* sprite;

RECT clientRect;
HBITMAP backgroundPattern;
HDC hdc;
bool isDrawing = true;
HDC backDC;

HBITMAP spriteHBMP;
HBITMAP backHBMP;
HBITMAP hMask;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Разместите код здесь.

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BUBBLEBOBBLE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BUBBLEBOBBLE));

	MSG msg;

	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BUBBLEBOBBLE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BUBBLEBOBBLE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

	spriteHBMP = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_SPRITE));
	backgroundPattern = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_BACKGROUND));

	sprite = new Sprite();
	InitSprite(sprite);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	mhWnd = hWnd;
	DragAcceptFiles(mhWnd, TRUE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	GetClientRect(mhWnd, &clientRect);
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&DrawFrame, NULL, NULL, NULL);

	return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		GetClientRect(mhWnd, &clientRect);
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
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
	case WM_MOUSEWHEEL:
	{
		if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
		{
			GET_WHEEL_DELTA_WPARAM(wParam) >= 0 ? Move(sprite, -5, 0) : Move(sprite, 5, 0);
		}
		else
		{
			GET_WHEEL_DELTA_WPARAM(wParam) >= 0 ? Move(sprite, 0, -5) : Move(sprite, 0, 5);
		}
		//zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		break;
	}
	case WM_DROPFILES:
	{
		HBITMAP bmp = GetBmpByDragDrop(wParam);
		ChangeSpriteBitmap(sprite, bmp);
		break;
	}
	case WM_KEYDOWN: {
		switch (wParam)
		{
		case VK_LEFT:
			Move(sprite, -5, 0);
			break;
		case VK_RIGHT:
			Move(sprite, 5, 0);
			break;
		case VK_UP:
			Move(sprite, 0, -5);
			break;
		case VK_DOWN:
			Move(sprite, 0, 5);
			break;
		}
		break;
	};
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, backDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
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

void InitSprite(Sprite* sprite)
{
	sprite->coords.x = 0;
	sprite->coords.y = 0;
	sprite->horizontalSpeed = 0;
	sprite->verticalSpeed = 0;
	sprite->Weight = 6;

	sprite->hBitmap = spriteHBMP;

	GetObject(spriteHBMP, sizeof(BITMAP), &sprite->bitmap);
	sprite->hMask = CreateBitmapMask(spriteHBMP, COLORREF(0xFFFFFF));

	sprite->Height = sprite->bitmap.bmHeight;
	sprite->Width = sprite->bitmap.bmWidth;
}

void DrawSpriteOnDC(HDC hdc)
{
	ChangeSpritePosition(sprite, &clientRect);

	HDC memDC = CreateCompatibleDC(hdc);

	int x = sprite->coords.x;
	int y = sprite->coords.y;

	SelectObject(memDC, sprite->hMask);
	StretchBlt(hdc, x, y, sprite->Width, sprite->Height, memDC, 0, 0, sprite->Width, sprite->Height, SRCAND);

	SelectObject(memDC, sprite->hBitmap);
	StretchBlt(hdc, x, y, sprite->Width, sprite->Height, memDC, 0, 0, sprite->Width, sprite->Height, SRCPAINT);

	DeleteDC(memDC);
}

void ChangeSpritePosition(Sprite* sprite, RECT* clientRect)
{
	const double backSpeed = -0.2;
	bool collision = false;
	double transferX = sprite->coords.x + sprite->horizontalSpeed;
	double transferY = sprite->coords.y + sprite->verticalSpeed;

	if (transferX > clientRect->right - sprite->Width) { sprite->coords.x = clientRect->right - sprite->Width; collision = true; }
	if (transferX < 0) { sprite->coords.x = 0; collision = true; }
	if (transferY > clientRect->bottom - sprite->Height) { sprite->coords.y = clientRect->bottom - sprite->Height; collision = true; }
	if (transferY < 0) { sprite->coords.y = 0; collision = true; }
	if (collision) { sprite->horizontalSpeed *= backSpeed;  sprite->verticalSpeed *= backSpeed; }
	else { sprite->coords.x = transferX; sprite->coords.y = transferY; }
}

void Move(Sprite* sprite, double horizonal, double vertical)
{
	sprite->verticalSpeed += vertical / sprite->Weight;
	sprite->horizontalSpeed += horizonal / sprite->Weight;
}

void DrawFrame()
{
	GetClientRect(mhWnd, &clientRect);
	hdc = GetDC(mhWnd);
	backDC = CreateCompatibleDC(hdc);
	backHBMP = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
	HGDIOBJ oldBMP = SelectObject(backDC, backHBMP);
	ReleaseDC(mhWnd, hdc);

	while (isDrawing) {
		HBRUSH newBrush = CreatePatternBrush(backgroundPattern);
		HGDIOBJ oldBrush = SelectObject(backDC, newBrush);

		Rectangle(backDC, clientRect.left - 1, clientRect.top - 1, clientRect.right, clientRect.bottom);

		DrawSpriteOnDC(backDC);

		SelectObject(backDC, oldBrush);
		DeleteObject(newBrush);

		RedrawWindow(mhWnd, NULL, NULL, RDW_INVALIDATE);

		Sleep(1000 / 60);
	}

	SelectObject(backDC, oldBMP);
	DeleteObject(backHBMP);
	DeleteDC(backDC);
}

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;

	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

	SelectObject(hdcMem, hbmColour);
	SelectObject(hdcMem2, hbmMask);

	SetBkColor(hdcMem, crTransparent);

	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);

	return hbmMask;
}

HBITMAP GetBmpByDragDrop(WPARAM wParam)
{
	char* fileName = NULL;
	HDROP drop = (HDROP)wParam;
	UINT filePathesCount = DragQueryFileW(drop, 0xFFFFFFFF, NULL, 512);
	UINT longestFileNameLength = 0;
	HBITMAP hBitmap = NULL;
	for (UINT i = 0; i < filePathesCount; ++i)
	{
		UINT fileNameLength = DragQueryFileW(drop, i, NULL, 512) + 1;
		if (fileNameLength > longestFileNameLength)
		{
			longestFileNameLength = fileNameLength;
			fileName = (char*)realloc(fileName, longestFileNameLength * sizeof(*fileName));
		}
		DragQueryFileA(drop, i, fileName, fileNameLength);
	}
	DragFinish(drop);

	hBitmap = (HBITMAP)LoadImageA(hInst, fileName, IMAGE_BITMAP, 0, 0,
		LR_DEFAULTSIZE | LR_LOADFROMFILE);

	return hBitmap;
}

void ChangeSpriteBitmap(Sprite* sprite, HBITMAP hBitmap)
{
	sprite->hBitmap = hBitmap;

	GetObject(hBitmap, sizeof(BITMAP), &sprite->bitmap);
	sprite->hMask = CreateBitmapMask(hBitmap, COLORREF(0xFFFFFF));

	sprite->Height = sprite->bitmap.bmHeight;
	sprite->Width = sprite->bitmap.bmWidth;
}