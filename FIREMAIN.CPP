#include <stdio.h>
#include <time.h>
#include <mmsystem.h>
#include "firemain.h"
#include "hscreen.h"

HDC				MainDC;
WINAPP			WinApp;
HIDDEN_SCREEN	*hScreen;
HPALETTE		Pal;
BYTE			*FireTable;
BOOL			Burn = FALSE;
long			Width, Height;
int				OldMX, OldMY;

void SetPalette();
int FireFrame();

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR cmdString, int cmdShow)
{
	HWND        MainWnd;
	MSG         msg;
	WNDCLASS    WndClass;
	FILE		*fp;
	
	WinApp.hInstance = hInstance;
	WinApp.hPrevInst = hPrevInst;
	WinApp.cmdShow   = cmdShow;

	if((*cmdString == '-') || (*cmdString == '/'))
	{
		if( (*(cmdString + 1) == 'c') || (*(cmdString + 1) == 'C'))
		{
			MessageBox(GetFocus(), "This screensaver has no options.",
					   "SETTINGS", MB_OK | MB_ICONEXCLAMATION);
			return(FALSE);
		}
	}

	WndClass.style          = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc    = WndProc;
	WndClass.cbClsExtra     = 0;
	WndClass.cbWndExtra     = 0;
	WndClass.hInstance      = hInstance;
	WndClass.hIcon          = LoadIcon(WinApp.hInstance, "FIRE");
	WndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground  = (HBRUSH)(GetStockObject(NULL_BRUSH));
	WndClass.lpszMenuName   = NULL;
	WndClass.lpszClassName  = "MAINWND";
	RegisterClass(&WndClass);

	Width = GetSystemMetrics(SM_CXSCREEN);
	Height = GetSystemMetrics(SM_CYSCREEN);
	MainWnd   = CreateWindow("MAINWND", "Fire Demo",
							 WS_POPUP,
                             0, 0,
							 Width,
							 Height,
                             NULL, NULL, hInstance, NULL);
//	SetWindowPos(MainWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	ShowWindow(MainWnd, WinApp.cmdShow);
	UpdateWindow(MainWnd);

	for (;;)
	{
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if(FireFrame())
				WaitMessage();
		}
	}
    return 0;
};

LRESULT CALLBACK WndProc(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	char *c;
	int i;
	
	switch(message)
	{
//		case WM_ACTIVATE:
//			SetWindowPos(hWindow, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//			return 0;		
		case WM_CREATE:
			MainDC = GetDC(hWindow);
			SetSystemPaletteUse(MainDC, SYSPAL_NOSTATIC);
			SetSystemPaletteUse(MainDC, SYSPAL_STATIC);
			SetPalette();
			hScreen = new HIDDEN_SCREEN(MainDC, 256, 256);
			hScreen->UseDCPal(MainDC);
			FireTable = new BYTE[65536];
			hScreen->Clear();
			memset(FireTable, 0, 65536);
			srand(987);
			Burn = TRUE;
			OldMX = OldMY = -1;
			sndPlaySound("fire.wav", SND_ASYNC | SND_LOOP);
			return 0;
		case WM_PAINT:
//			SetWindowPos(hWindow, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			BeginPaint(hWindow, &ps);
			SetSystemPaletteUse(MainDC, SYSPAL_NOSTATIC);
			SetSystemPaletteUse(MainDC, SYSPAL_STATIC);
			SetPalette();
			hScreen->StretchToScreen(MainDC, 0, 0, Width, Height);
			EndPaint(hWindow, &ps);
			return 0;
		case WM_MOUSEMOVE:
			if(OldMX == -1 || OldMY == -1)
			{
				OldMX = LOWORD(lParam);
				OldMY = HIWORD(lParam);
			}
			else
				if((OldMX != LOWORD(lParam)) || (OldMY != HIWORD(lParam)))
					SendMessage(hWindow, WM_DESTROY, 0, 0);
			return 0;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_KEYDOWN:
		case WM_DESTROY:
			sndPlaySound(NULL, SND_ASYNC | SND_LOOP);
			Burn = FALSE;
			ReleaseDC(hWindow, MainDC);
			delete FireTable;
			delete hScreen;
			PostQuitMessage(0);
			return 0;
		default:
			return(DefWindowProc(hWindow, message, wParam, lParam));
	}
};

void SetPalette()
{
	NPLOGPALETTE Palette;
	BYTE r, g, b;
	float fr, fg, fb;
		
	Palette = (NPLOGPALETTE)LocalAlloc(LMEM_FIXED,
		(sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * (256))));
	Palette->palVersion = 0x300;
	Palette->palNumEntries = 256;
	GetSystemPaletteEntries(MainDC, 0, 256, (PALETTEENTRY *)Palette->palPalEntry);

	fr = 0; fg = 0; fb = 0;
	for(int i = 0; i < 118; i++)
	{
		Palette->palPalEntry[i + 10].peRed = (BYTE)fr;
		Palette->palPalEntry[i + 10].peGreen = (BYTE)fg;
		Palette->palPalEntry[i + 10].peBlue = (BYTE)fb;
		Palette->palPalEntry[i + 10].peFlags = (BYTE)PC_RESERVED;
		fr += 255.0 / 118.0;
	}

	fr = 255; fg = 0; fb = 0;
	for(i = 118; i < 236; i++)
	{
		Palette->palPalEntry[i + 10].peRed = (BYTE)fr;
		Palette->palPalEntry[i + 10].peGreen = (BYTE)fg;
		Palette->palPalEntry[i + 10].peBlue = (BYTE)fb;
		Palette->palPalEntry[i + 10].peFlags = (BYTE)PC_RESERVED;
		fg += 255.0 / 118.0;
	}

	Pal = CreatePalette((LPLOGPALETTE)Palette);
	LocalFree((NPLOGPALETTE)Palette);
	SelectPalette(MainDC, Pal, FALSE);
	RealizePalette(MainDC);
};

int FireFrame()
{
	int i, j, k;
	BYTE *c, *d, dx;
	BYTE cl[5];
	long val;
	
	if(!Burn)
		return(1);

	c = (BYTE *)FireTable + 65024;
	for(i = 0; i < 256; i++)
	{
		k = rand() & 7;
		if(k < 2)
			dx = 10;
		else
			dx = 245;
		*c = dx;
		*(c + 256) = dx;
		c++;
	}

	for(i = 1; i < 256; i++)
	{
		d = (BYTE *)hScreen->Surface + ((i - 1) << 8) + 1;
		c = (BYTE *)FireTable + (i << 8) + 1;
		for(j = 1; j < 256; j++)
		{
			cl[0] = *(c - 256);
			cl[1] = *(c + 1);
			cl[2] = *(c + 256);
			cl[3] = *(c - 1);
			cl[4] = *c;
			val = 0;
			for(k = 0; k < 4; k++)
				val += cl[k];
			val >>= 2;
			val += cl[4];
			val >>= 1;
			val -= 1;
			if(val < 10) val = 10;
			if(val > 245) val = 245;
			*d = (BYTE)val;
			c++;
			d++;
		}
	}
	hScreen->StretchToScreen(MainDC, 0, 0, Width, Height);
	c = (BYTE *)FireTable;
	d = (BYTE *)hScreen->Surface;
	memcpy(c, d, 65536);
	return 0;
};

