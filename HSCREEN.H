#ifndef HSCREEN_H
#define HSCREEN_H

#include <windows.h>
#include "hdib.h"

typedef struct
{
	BITMAPINFOHEADER 	bmiHeader;
	RGBQUAD			    bmiColors[256];

} BMPINFO;

class HIDDEN_SCREEN
{
private:
	WORD        Width, Height;
	int         Orientation;
	HPALETTE    MainPal;
public:
	BMPINFO     BmpInfo;
	HDC         HiddenDC;
	HBITMAP     HiddenBMP;
	HBITMAP     OldBMP;
	void far    *Surface;

	HIDDEN_SCREEN(HDC hdc, int width, int height);
	~HIDDEN_SCREEN();

	int 	BltToScreen(HDC Screen, int x, int y);
	int 	BltToScreen2(HDC Screen, int w, int h, int x, int y);
	int 	StretchToScreen(HDC Screen, int x, int y, int w, int h);
	int 	Clear(COLORREF Col);
	int 	Clear();

	int		 UseHalftonePalette(HDC dc);
	int		 UseDCPal(HDC hdc);

	WORD GetWidth() {return Width;};
	WORD GetHeight() {return Height;};
	int  GetType() {return Orientation;};
	HPALETTE GetPal() {return MainPal;};
};

void    ClearSystemPalette(void);
void    AdjustClientRegion(HWND hWindow, int x1, int y1, int w, int h);

#endif
