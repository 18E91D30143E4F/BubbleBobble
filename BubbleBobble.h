#pragma once

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
HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);
void DrawSpriteOnDC(HDC hdc);
void DrawFrame();
void Move(Sprite* sprite, double vertical, double horizonal);
void ChangeSpritePosition(Sprite* sprite, RECT* clientRect);