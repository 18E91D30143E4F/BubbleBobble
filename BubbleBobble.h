#pragma once

#include <windows.h>
#include <shellapi.h>

#include "resource.h"


struct Coords {
	int x;
	int y;
};

typedef struct Sprite {
	BITMAP bitmap;
	HBITMAP hBitmap;
	HBITMAP hMask;

	Coords coords;

	int Height;
	int Width;

	double Weight;

	double horizontalSpeed;
	double verticalSpeed;

} Sprite;

void InitSprite(Sprite* sprite);
void DrawSpriteOnDC(HDC hdc);
void DrawFrame();
void Move(Sprite* sprite, double vertical, double horizonal);
void ChangeSpritePosition(Sprite* sprite, RECT* clientRect);
void ChangeSpriteBitmap(Sprite* sprite, HBITMAP hBitmap);

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);
HBITMAP GetBmpByDragDrop(WPARAM wParam);