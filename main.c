///////////////////////////////////////////////////////////////////////////////////
// Título:              fire.c                                                   //
// Author:              Benjamin Maggi                                           //
// Descripcion:         Some nice Fire Effect                                    //
// Org. Date:           09/05/2008                                               //
// Last Modification:   09/05/2008                                               //
// Ver:                 0.0.1                                                    //
// compiler:            uses ansi-C / C99 tested with LCC & Pellesc              //
// Author mail:         benjaminmaggi@gmail.com                                  //
// License:             GNU                                                      //
//                                                                               //
// License:                                                                      //
// This program is free software; you can redistribute it                        //
// and/or modify it under the terms of the GNU General Public                    //
// License as published by the Free Software Foundation;                         //
// either version 2 of the License, or (at your option) any                      //
// later version.                                                                //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 //
// GNU General Public License for more details.                                  //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN

//
// includes
//
#include <windows.h>
#include <stdlib.h>
#include "main.h"

//
// defines
//
#define STV_CLASS "shitfire_class"
#define STV_TITLE "Fire"
#define WND_WIDTH 420
#define WND_HEIGHT 300

//
// Global Variables
//
HINSTANCE	m_hInst = NULL;
HWND		m_hWnd = NULL;
BOOL		m_bInit = FALSE;

//
// Foward declarations of functions included in this code module:
//
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OnCreate();
void OnRender();
void OnDestroy();
void dofire();

///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// WinMain                                                                       //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	m_hInst = hInstance;

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInst;
	wcex.hIcon			= NULL;//LoadIcon(m_hInst, (LPCTSTR)IDI_FIREWORKS);
	wcex.hIconSm		= NULL;//LoadIcon(m_hInst, (LPCTSTR)IDI_SMALL);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)COLOR_WINDOW;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= STV_CLASS;
	
	RegisterClassEx(&wcex);

	m_hWnd = CreateWindow(STV_CLASS,
						  STV_TITLE, 
						  WS_OVERLAPPED|WS_SYSMENU,
						  CW_USEDEFAULT, CW_USEDEFAULT, 
						  WND_WIDTH, WND_HEIGHT, 
						  NULL, NULL, 
						  m_hInst, NULL);

	ShowWindow(m_hWnd, nCmdShow);
	UpdateWindow(m_hWnd);

	MSG msg;
	while(TRUE)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if(m_bInit)
				OnRender();
				//dofire();
		}

	}
	
	UnregisterClass(STV_CLASS,m_hInst);
	return msg.wParam;
}

///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// main window proc                                                              //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
			m_hWnd = hWnd;
			OnCreate();
			break;
		case WM_DESTROY:
			OnDestroy();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// memory bitmap info
BITMAPINFO m_bmpInfo;
HDC m_hDC = NULL;
HBITMAP m_memBmp = NULL;
int* m_pBits = NULL;
int m_nNumPixels = 0;
int m_nPlacementX = 0;
int m_nPlacementY = 0;

//
// defines to make life easier
//
#define wnd_cx m_bmpInfo.bmiHeader.biWidth
#define wnd_cy (-m_bmpInfo.bmiHeader.biHeight)
#define MAX_FPS 100

//
//	fire setup
//
int fire_height = 1;	// 0-tall fire, 1-smaller fire...

int i1 = 255;	// red - main
int j1 = 0;		// green - main
int k1 = 0;		// blue - main

int l1 = 255;	// red - coals
int i2 = 255;	// green - coals
int j2 = 0;		// blue - coals

int* work_area;	// work area
//int* palette;	// palette

int seed = 0x12345; // seed for our own rand() (noise())

///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// Custom 256 Color Pallete                                                      //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////
int pal_size	= 256;		// palette size # colors in array
int palette[]	= 
{
	0x000000, 0x020001, 0x040002, 0x060003, 0x080005, 0x090006, 0x0B0007, 0x0D0008, 0x0F0009, 0x11000A, 0x13000C, 0x15000D, 0x17000E, 0x19000F, 0x1A0010, 0x1C0011,
	0x1E0013, 0x200014, 0x220015, 0x240016, 0x260017, 0x280018, 0x2A001A, 0x2B001B, 0x2D001C, 0x2F001D, 0x31001E, 0x33001F, 0x350021, 0x370022, 0x390023, 0x3B0024,
	0x3C0025, 0x3E0026, 0x400028, 0x420029, 0x44002A, 0x4A0027, 0x500024, 0x560021, 0x5B001E, 0x61001B, 0x670018, 0x6D0015, 0x730012, 0x79000F, 0x7F000C, 0x840009,
	0x8A0006, 0x900003, 0x960000, 0x9A0600, 0x9D0C00, 0xA11200, 0xA51800, 0xA81E00, 0xAC2400, 0xB02A00, 0xB43000, 0xB73700, 0xBB3D00, 0xBF4300, 0xC24900, 0xC64F00,
	0xCA5500, 0xCD5B00, 0xD16100, 0xD56600, 0xD86C00, 0xDC7100, 0xDF7600, 0xE37C00, 0xE68100, 0xEA8600, 0xED8B00, 0xF19100, 0xF49600, 0xF89B00, 0xFBA100, 0xFFA600,
	0xFEAB08, 0xFDB10F, 0xFDB616, 0xFCBC1E, 0xFBC126, 0xFAC72D, 0xFACC34, 0xF9D13C, 0xF8D744, 0xF7DC4B, 0xF6E252, 0xF6E75A, 0xF5ED62, 0xF4F269, 0xF5F372, 0xF6F47A,
	0xF7F483, 0xF8F58B, 0xFAF694, 0xFBF79D, 0xFCF8A5, 0xFDF8AE, 0xFEF9B6, 0xFFFABF, 0xFFFABF, 0xFFFAC0, 0xFFFAC0, 0xFFFAC0, 0xFFFAC0, 0xFFFAC1, 0xFFFAC1, 0xFFFAC1,
	0xFFFAC2, 0xFFFAC2, 0xFFFAC2, 0xFFFAC2, 0xFFFAC3, 0xFFFAC3, 0xFFFAC3, 0xFFFAC4, 0xFFFAC4, 0xFFFAC4, 0xFFFAC5, 0xFFFAC5, 0xFFFAC5, 0xFFFAC5, 0xFFFBC6, 0xFFFBC6,
	0xFFFBC6, 0xFFFBC7, 0xFFFBC7, 0xFFFBC7, 0xFFFBC7, 0xFFFBC8, 0xFFFBC8, 0xFFFBC8, 0xFFFBC9, 0xFFFBC9, 0xFFFBC9, 0xFFFBC9, 0xFFFBCA, 0xFFFBCA, 0xFFFBCA, 0xFFFBCB,
	0xFFFBCB, 0xFFFBCB, 0xFFFBCB, 0xFFFBCC, 0xFFFBCC, 0xFFFBCC, 0xFFFBCD, 0xFFFBCD, 0xFFFBCD, 0xFFFBCD, 0xFFFBCE, 0xFFFBCE, 0xFFFBCE, 0xFFFBCF, 0xFFFBCF, 0xFFFBCF,
	0xFFFBD0, 0xFFFBD0, 0xFFFBD0, 0xFFFBD0, 0xFFFBD1, 0xFFFBD1, 0xFFFBD1, 0xFFFBD2, 0xFFFBD2, 0xFFFBD2, 0xFFFCD2, 0xFFFCD3, 0xFFFCD3, 0xFFFCD3, 0xFFFCD4, 0xFFFCD4,
	0xFFFCD4, 0xFFFCD4, 0xFFFCD5, 0xFFFCD5, 0xFFFCD5, 0xFFFCD6, 0xFFFCD6, 0xFFFCD6, 0xFFFCD6, 0xFFFCD7, 0xFFFCD7, 0xFFFCD7, 0xFFFCD8, 0xFFFCD8, 0xFFFCD8, 0xFFFCD8,
	0xFFFCD9, 0xFFFCD9, 0xFFFCD9, 0xFFFCDA, 0xFFFCDA, 0xFFFCDA, 0xFFFCDB, 0xFFFCDB, 0xFFFCDB, 0xFFFCDB, 0xFFFCDC, 0xFFFCDC, 0xFFFCDC, 0xFFFCDD, 0xFFFCDD, 0xFFFCDD,
	0xFFFCDD, 0xFFFCDE, 0xFFFCDE, 0xFFFCDE, 0xFFFCDF, 0xFFFCDF, 0xFFFDDF, 0xFFFDDF, 0xFFFDE0, 0xFFFDE0, 0xFFFDE0, 0xFFFDE1, 0xFFFDE1, 0xFFFDE1, 0xFFFDE1, 0xFFFDE2,
	0xFFFDE2, 0xFFFDE2, 0xFFFDE3, 0xFFFDE3, 0xFFFDE3, 0xFFFDE3, 0xFFFDE4, 0xFFFDE4, 0xFFFDE4, 0xFFFDE5, 0xFFFDE5, 0xFFFDE5, 0xFFFDE6, 0xFFFDE6, 0xFFFDE6, 0xFFFDE6,
	0xFFFDE7, 0xFFFDE7, 0xFFFDE7, 0xFFFDE8, 0xFFFDE8, 0xFFFDE8, 0xFFFDE8, 0xFFFDE9, 0xFFFDE9, 0xFFFDE9, 0xFFFDEA, 0xFFFDEA, 0xFFFDEA, 0xFFFDEA, 0xFFFDEB, 0xFFFDEB
};
int palette2[]	= 
{
	0xFF0000, 0x010101, 0x020202, 0x030303, 0x040404, 0x050505, 0x060606, 0x070707, 0x080808, 0x090909, 0x0A0A0A, 0x0B0B0B, 0x0C0C0C, 0x0D0D0D, 0x0E0E0E, 0x0F0F0F, 
	0x101010, 0x111111, 0x121212, 0x131313, 0x141414, 0x151515, 0x161616, 0x171717, 0x181818, 0x191919, 0x1A1A1A, 0x1B1B1B, 0x1C1C1C, 0x1D1D1D, 0x1E1E1E, 0x1F1F1F, 
	0x202020, 0x212121, 0x222222, 0x232323, 0x242424, 0x252525, 0x262626, 0x272727, 0x282828, 0x292929, 0x2A2A2A, 0x2B2B2B, 0x2C2C2C, 0x2D2D2D, 0x2E2E2E, 0x2F2F2F, 
	0x303030, 0x313131, 0x323232, 0x333333, 0x343434, 0x353535, 0x363636, 0x373737, 0x383838, 0x393939, 0x3A3A3A, 0x3B3B3B, 0x3C3C3C, 0x3D3D3D, 0x3E3E3E, 0x3F3F3F, 
	0x404040, 0x414141, 0x424242, 0x434343, 0x444444, 0x454545, 0x464646, 0x474747, 0x484848, 0x494949, 0x4A4A4A, 0x4B4B4B, 0x4C4C4C, 0x4D4D4D, 0x4E4E4E, 0x4F4F4F, 
	0x505050, 0x515151, 0x525252, 0x535353, 0x545454, 0x555555, 0x565656, 0x575757, 0x585858, 0x595959, 0x5A5A5A, 0x5B5B5B, 0x5C5C5C, 0x5D5D5D, 0x5E5E5E, 0x5F5F5F, 
	0x606060, 0x616161, 0x626262, 0x636363, 0x646464, 0x656565, 0x666666, 0x676767, 0x686868, 0x696969, 0x6A6A6A, 0x6B6B6B, 0x6C6C6C, 0x6D6D6D, 0x6E6E6E, 0x6F6F6F, 
	0x707070, 0x717171, 0x727272, 0x737373, 0x747474, 0x757575, 0x767676, 0x777777, 0x787878, 0x797979, 0x7A7A7A, 0x7B7B7B, 0x7C7C7C, 0x7D7D7D, 0x7E7E7E, 0x7F7F7F, 
	0x808080, 0x818181, 0x828282, 0x838383, 0x848484, 0x858585, 0x868686, 0x878787, 0x888888, 0x898989, 0x8A8A8A, 0x8B8B8B, 0x8C8C8C, 0x8D8D8D, 0x8E8E8E, 0x8F8F8F, 
	0x909090, 0x919191, 0x929292, 0x939393, 0x949494, 0x959595, 0x969696, 0x979797, 0x989898, 0x999999, 0x9A9A9A, 0x9B9B9B, 0x9C9C9C, 0x9D9D9D, 0x9E9E9E, 0x9F9F9F, 
	0xA0A0A0, 0xA1A1A1, 0xA2A2A2, 0xA3A3A3, 0xA4A4A4, 0xA5A5A5, 0xA6A6A6, 0xA7A7A7, 0xA8A8A8, 0xA9A9A9, 0xAAAAAA, 0xABABAB, 0xACACAC, 0xADADAD, 0xAEAEAE, 0xAFAFAF, 
	0xB0B0B0, 0xB1B1B1, 0xB2B2B2, 0xB3B3B3, 0xB4B4B4, 0xB5B5B5, 0xB6B6B6, 0xB7B7B7, 0xB8B8B8, 0xB9B9B9, 0xBABABA, 0xBBBBBB, 0xBCBCBC, 0xBDBDBD, 0xBEBEBE, 0xBFBFBF, 
	0xC0C0C0, 0xC1C1C1, 0xC2C2C2, 0xC3C3C3, 0xC4C4C4, 0xC5C5C5, 0xC6C6C6, 0xC7C7C7, 0xC8C8C8, 0xC9C9C9, 0xCACACA, 0xCBCBCB, 0xCCCCCC, 0xCDCDCD, 0xCECECE, 0xCFCFCF, 
	0xD0D0D0, 0xD1D1D1, 0xD2D2D2, 0xD3D3D3, 0xD4D4D4, 0xD5D5D5, 0xD6D6D6, 0xD7D7D7, 0xD8D8D8, 0xD9D9D9, 0xDADADA, 0xDBDBDB, 0xDCDCDC, 0xDDDDDD, 0xDEDEDE, 0xDFDFDF, 
	0xE0E0E0, 0xE1E1E1, 0xE2E2E2, 0xE3E3E3, 0xE4E4E4, 0xE5E5E5, 0xE6E6E6, 0xE7E7E7, 0xE8E8E8, 0xE9E9E9, 0xEAEAEA, 0xEBEBEB, 0xECECEC, 0xEDEDED, 0xEEEEEE, 0xEFEFEF, 
	0xF0F0F0, 0xF1F1F1, 0xF2F2F2, 0xF3F3F3, 0xF4F4F4, 0xF5F5F5, 0xF6F6F6, 0xF7F7F7, 0xF8F8F8, 0xF9F9F9, 0xFAFAFA, 0xFBFBFB, 0xFCFCFC, 0xFDFDFD, 0xFEFEFE, 0xFFFFFF
};
///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// On Creation                                                                   //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////
void OnCreate()
{

	// set up placement
	m_nPlacementX = GetSystemMetrics(SM_CXSIZEFRAME);
	m_nPlacementY = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME);
	int nWidth = WND_WIDTH - m_nPlacementX*2;
	int nHeight = WND_HEIGHT - m_nPlacementY - GetSystemMetrics(SM_CYSIZEFRAME);

	DWORD sizeOfBits = (((nWidth * 32 + 31) & ~31) >> 3) * nHeight;
	m_nNumPixels = sizeOfBits / sizeof(RGBQUAD);

	memset(&m_bmpInfo,0,sizeof(m_bmpInfo));
	m_bmpInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	m_bmpInfo.bmiHeader.biWidth		= nWidth;
	m_bmpInfo.bmiHeader.biHeight	= -nHeight; // make negative for "right side up" bitmap
	m_bmpInfo.bmiHeader.biSizeImage = sizeOfBits;
	m_bmpInfo.bmiHeader.biPlanes	= 1;
	m_bmpInfo.bmiHeader.biBitCount	= 32;

	m_hDC = GetWindowDC(m_hWnd);
	m_memBmp = CreateDIBSection(m_hDC, &m_bmpInfo, DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0);

	srand(GetTickCount());

	///////////////////////////////////////////////////////////////////////////////////
	// set up palette                                                                //
	///////////////////////////////////////////////////////////////////////////////////
	/*
	palette = malloc( pal_size * sizeof(int));
    for(int l2 = 0; l2 < pal_size; l2++){
		int j4 = pal_size / 2;
		int i3, k3, i4;
		if(l2 < j4){
			i3 = (i1 * l2) / j4;
			k3 = (j1 * l2) / j4;
			i4 = (k1 * l2) / j4;
		}else{
			i3 = ((l1 - i1) * (l2 - j4)) / j4 + i1;
			k3 = ((i2 - j1) * (l2 - j4)) / j4 + j1;
			i4 = ((j2 - k1) * (l2 - j4)) / j4 + k1;
		}
        palette[l2] = i3 << 16 | k3 << 8 | i4;
    }
	*/
	
	///////////////////////////////////////////////////////////////////////////////////
	// create work area                                                              //
	///////////////////////////////////////////////////////////////////////////////////
	work_area = malloc(m_nNumPixels * sizeof(int));

	///////////////////////////////////////////////////////////////////////////////////
	// we are setup... kick in rendering function                                    //
	///////////////////////////////////////////////////////////////////////////////////
	m_bInit = TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// On Destroy                                                                    //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////
void OnDestroy()
{
	ReleaseDC(m_hWnd, m_hDC);	// done with DC
	DeleteObject(m_memBmp);		// done with bitmap
	free(work_area);			// destroy work area
	//free(palette);			// destory palette only if needed
}


///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// basically "rand()" function                                                   //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////
int noise()
{
	int i = seed;
	i >>= 3;
	i ^= seed;
	int j = i & 0x1;
	i >>= 1;
	seed >>= 1;
	seed |= j << 30;
	return i;
}

///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// On Render                                                                     //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////
void OnRender()
{

	DWORD gtc = GetTickCount();

	// sync code
	static DWORD lastUpdate = 0;
	if(gtc >= lastUpdate + 1000/MAX_FPS)
	{
		lastUpdate = gtc;
	}
	else
	{
		return;
	}

	///////////////////////////////////////////////////////////////////////////////////
	// FPS timer                                                                     //
	///////////////////////////////////////////////////////////////////////////////////
	static DWORD nextUpdate = 0;
	static int count = 0;
	if(nextUpdate < gtc)
	{
		nextUpdate = gtc+1000;
		char fps[127]; wsprintf(fps,"%s [FPS: %i]", STV_TITLE, count);
		SetWindowText(m_hWnd,fps);
		count = 0;
	}
	else
	{
		++count;
	}


	///////////////////////////////////////////////////////////////////////////////////
	// init new row                                                                  //
	///////////////////////////////////////////////////////////////////////////////////
    for(int j3 = (m_nNumPixels - (wnd_cx << 2)) + 5; j3 < m_nNumPixels - wnd_cx * 3 - 5; j3 += 2)
	{
		work_area[j3] = work_area[j3 + wnd_cx] = work_area[j3 + 1] = work_area[j3 + wnd_cx + 1] = noise() % pal_size;
	}

	///////////////////////////////////////////////////////////////////////////////////
	// average out for blur affect                                                   //
	///////////////////////////////////////////////////////////////////////////////////
    for(int l3 = wnd_cx; l3 < m_nNumPixels - wnd_cx * 3; l3++)
	{

        work_area[l3] = ((
							work_area[l3 - 1] + 
							work_area[l3] + 
							work_area[l3 + 1] + 
							work_area[(l3 + wnd_cx) - 1] + 
							work_area[l3 + wnd_cx + 1] + 
							work_area[(l3 + wnd_cx + wnd_cx) - 1] + 
							work_area[l3 + wnd_cx + wnd_cx] + 
							work_area[l3 + wnd_cx + wnd_cx + 1] ) / 8) - fire_height;

		if(work_area[l3] < 0){ work_area[l3] = 0; }		// if lower than zero, make black

		m_pBits[l3] = palette[work_area[l3]];			// set the bit
    }

	///////////////////////////////////////////////////////////////////////////////////
	// finally, "swap buffers"                                                       //
	///////////////////////////////////////////////////////////////////////////////////
	SetDIBitsToDevice(m_hDC,
					  m_nPlacementX,
					  m_nPlacementY,
					  wnd_cx,
					  wnd_cy,
					  0,
					  0,
					  0,
					  wnd_cy,
					  (LPVOID)m_pBits,
					  &m_bmpInfo,
					  DIB_RGB_COLORS);
}

#define YSTART          100
#define XSTART          150
#define XEND            210
#define FIRESTRENGTH    15
#define BUF_HEIGHT		80
#define BUF_WIDTH		120
void dofire()
{
    int x, y;
	int a, b, i, j, delta;

	///////////////////////////////////////////////////////////////////////////////////
	// init new row                                                                  //
	///////////////////////////////////////////////////////////////////////////////////
    for(int j3 = (m_nNumPixels - (wnd_cx << 2)) + 5; j3 < m_nNumPixels - wnd_cx * 3 - 5; j3 += 2)
	{
		//m_pBits[j3] = m_pBits[j3 + wnd_cx] = m_pBits[j3 + 1] = m_pBits[j3 + wnd_cx + 1] = noise() % pal_size;
	}

	DWORD buff[120][120];
	ZeroMemory(buff, 120 * 120 * sizeof(DWORD));
	//#define WND_WIDTH 420
	//#define WND_HEIGHT 300
    for(int y = 1; y < 120; y++)
	{
		for(int x = 1; x < 120; x++)
		{
			buff[x][y] = 0x00FF00;
		}
	}
	CopyMemory(m_pBits, buff, 120 * 120 * sizeof(DWORD));
	///////////////////////////////////////////////////////////////////////////////////
	// finally, "swap buffers"                                                       //
	///////////////////////////////////////////////////////////////////////////////////
	SetDIBitsToDevice(m_hDC,
					  m_nPlacementX,
					  m_nPlacementY,
					  wnd_cx,
					  wnd_cy,
					  0,
					  0,
					  0,
					  wnd_cy,
					  (LPVOID)buff,
					  &m_bmpInfo,
					  DIB_RGB_COLORS);
}
