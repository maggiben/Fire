#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <mem.h>
#include <math.h>
#include "hscreen.h"

HIDDEN_SCREEN::HIDDEN_SCREEN(HDC hdc, int width, int height)
{
	BMPINFO Info;

	Width = (WORD)width;
	Height = (WORD)height;

	Orientation = -1;//(int)Info.bmiHeader.biHeight;
// Use Top-Down for easier access to bytes

	Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.bmiHeader.biWidth = Width;
	Info.bmiHeader.biHeight = -Height;
	Info.bmiHeader.biPlanes = 1;
	Info.bmiHeader.biBitCount = 8;
	Info.bmiHeader.biCompression = BI_RGB;
	Info.bmiHeader.biSizeImage = Width * Height;
	Info.bmiHeader.biXPelsPerMeter = 0;
	Info.bmiHeader.biYPelsPerMeter = 0;
	Info.bmiHeader.biClrUsed = 256;
	Info.bmiHeader.biClrImportant = 256;

	HiddenBMP = CreateDIBSection(hdc, (BITMAPINFO *)&Info, DIB_PAL_COLORS,
						  &Surface, NULL, 0);
	MainPal = CreateHalftonePalette(hdc);
	HiddenDC = CreateCompatibleDC(hdc);
	OldBMP = SelectObject(HiddenDC, HiddenBMP);

	GetPaletteEntries(MainPal, 0, 256, (PALETTEENTRY far *)Info.bmiColors);
	for(int Counter = 0; Counter < 256; Counter++)
	{
		BYTE Temp = Info.bmiColors[Counter].rgbBlue;
		Info.bmiColors[Counter].rgbBlue = Info.bmiColors[Counter].rgbRed;
		Info.bmiColors[Counter].rgbRed = Temp;
	}

	PatBlt(HiddenDC, 0, 0, width, height, WHITENESS);

	BmpInfo = Info;
};

HIDDEN_SCREEN::~HIDDEN_SCREEN()
{
	SelectObject(HiddenDC, OldBMP);
	DeleteObject(HiddenBMP);
	DeleteDC(HiddenDC);
	DeleteObject(MainPal);
};

int HIDDEN_SCREEN::BltToScreen(HDC Screen, int x, int y)
{
	BitBlt(Screen, x, y, Width, Height, HiddenDC, 0, 0, SRCCOPY);
	return 0;
};

int HIDDEN_SCREEN::BltToScreen2(HDC Screen, int w, int h, int x, int y)
{
	BitBlt(Screen, x, y, w, h, HiddenDC, 0, 0, SRCCOPY);
	return 0;
};

int HIDDEN_SCREEN::StretchToScreen(HDC Screen, int x, int y, int w, int h)
{
	StretchBlt(Screen, x, y, w, h, HiddenDC, 0, 0, Width, Height - 3, SRCCOPY);
	return 0;
};

int HIDDEN_SCREEN::Clear()
{
	PatBlt(HiddenDC, 0, 0, Width, Height, BLACKNESS);
	return 0;
};

int HIDDEN_SCREEN::Clear(COLORREF Col)
{
	HBRUSH Brush, Oldbrush;

	Brush = CreateSolidBrush(Col);
	Oldbrush = SelectObject(HiddenDC, Brush);
	PatBlt(HiddenDC, 0, 0, Width, Height, PATCOPY);
	SelectObject(HiddenDC, Oldbrush);
	DeleteObject(Brush);
	return 0;
};

int HIDDEN_SCREEN::UseDCPal(HDC hdc)
{
	RGBQUAD Color[256];
	int i;
	BYTE Temp;
	
	GetSystemPaletteEntries(hdc, 0, 256, (PALETTEENTRY *)Color);
	for(i = 0; i < 256; i++)
	{
		Temp = Color[i].rgbBlue;
		Color[i].rgbBlue = Color[i].rgbRed;
		Color[i].rgbRed = Temp;
	}
	SetDIBColorTable(HiddenDC, 0, 256, Color);
	return 0;
};
	
int HIDDEN_SCREEN::UseHalftonePalette(HDC hdc)
{
	HPALETTE Pal, Old;
	RGBQUAD	Colors[256];

	Pal = CreateHalftonePalette(hdc);
	Old = SelectPalette(hdc, Pal, FALSE);
	RealizePalette(hdc);
	DeleteObject(Old);

	GetPaletteEntries(Pal, 0, 256, (PALETTEENTRY far *)Colors);
	for(int i = 0; i < 256; i++)
	{
		BYTE Temp = Colors[i].rgbBlue;
		Colors[i].rgbBlue = Colors[i].rgbRed;
		Colors[i].rgbRed = Temp;
	}
	SetDIBColorTable(HiddenDC, 0, 256, Colors);
	return 0;
};

void AdjustClientRegion(HWND hWindow, int x1, int y1, int w, int h)
{
	RECT r;
	long NewWidth, NewHeight;

	GetClientRect(hWindow, &r);
	NewWidth = w - (r.right - r.left);
	NewHeight = h - (r.bottom - r.top);
	GetWindowRect(hWindow, &r);
	SetWindowPos(hWindow, HWND_TOP, 0, 0, r.right - r.left + NewWidth, r.bottom - r.top + NewHeight, SWP_NOZORDER);
};
