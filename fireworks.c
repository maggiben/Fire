// Fireworks.cpp : Defines the entry point for the application.
// by: Andrew Heinlein [Mouse]
// www.mouseindustries.com
// Taken from "Lady Liberty Fireworks" jhanabi.class (decompiled with JAD)

#define WIN32_LEAN_AND_MEAN

//
// includes
//
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include "main.h"

//
// defines
//
#define STV_CLASS "fireworks_class"
#define STV_TITLE "Fireworks"
#define WND_WIDTHHEIGHT 460

#define MAIN_TIMER 12345

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
void OnCreate(void);
void OnDestroy(void);
void OnLButtonDown(int x, int y);
void OnTimer(void);

void InitFireworks(void);
void DeInitFireworks(void);
void OnRender(void);

//
// pep
//
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow){

	m_hInst = hInstance;

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInst;
	wcex.hIcon			= LoadIcon(m_hInst, (LPCTSTR)IDR_ICO_MAIN);
	wcex.hIconSm		= LoadIcon(m_hInst, (LPCTSTR)IDR_ICO_MAIN);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)COLOR_WINDOW;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= STV_CLASS;
	RegisterClassEx(&wcex);

	m_hWnd = CreateWindow(STV_CLASS,
						  STV_TITLE, 
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, CW_USEDEFAULT, 
						  WND_WIDTHHEIGHT, WND_WIDTHHEIGHT, 
						  NULL, NULL, 
						  m_hInst, NULL);

	ShowWindow(m_hWnd, nCmdShow);
	UpdateWindow(m_hWnd);

	MSG msg;
	while(TRUE){
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
			if(msg.message == WM_QUIT){
				break;
			}else{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}else{
			if(m_bInit)
				OnRender();
		}

	}

	UnregisterClass(STV_CLASS,m_hInst);

	return msg.wParam;
}

//
// main window proc
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){
		case WM_CREATE:
			m_hWnd = hWnd;
			OnCreate();
			break;
		case WM_DESTROY:
			OnDestroy();
			PostQuitMessage(0);
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown(LOWORD(lParam),HIWORD(lParam));
			break;
		case WM_TIMER:
			OnTimer();
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
//RGBQUAD* m_pBits = NULL;
int* pix0 = 0;
int m_nNumPixels = 0;
int m_nSizeOfBits = 0;
int m_nPlacementX = 0;
int m_nPlacementY = 0;

//
// On Creation
//
void OnCreate(void)
{

	m_nPlacementX = GetSystemMetrics(SM_CXSIZEFRAME);
	m_nPlacementY = GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYSIZEFRAME);
	int nWidth = WND_WIDTHHEIGHT - m_nPlacementX*2;
	int nHeight = WND_WIDTHHEIGHT - m_nPlacementY - GetSystemMetrics(SM_CYSIZEFRAME);

	m_nSizeOfBits = (((nWidth * 32 + 31) & ~31) >> 3) * nHeight;
	m_nNumPixels = m_nSizeOfBits / sizeof(RGBQUAD);

	memset(&m_bmpInfo,0,sizeof(m_bmpInfo));
	m_bmpInfo.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	m_bmpInfo.bmiHeader.biWidth			= nWidth;
	m_bmpInfo.bmiHeader.biHeight		= nHeight;
	m_bmpInfo.bmiHeader.biSizeImage		= m_nSizeOfBits;
	m_bmpInfo.bmiHeader.biPlanes		= 1;
	m_bmpInfo.bmiHeader.biBitCount		= 32;

	m_hDC = GetWindowDC(m_hWnd);
	m_memBmp = CreateDIBSection(m_hDC, &m_bmpInfo, DIB_RGB_COLORS, (void**)&pix0, NULL, 0);
	memset((void*)pix0,0,m_nSizeOfBits);

	srand(GetTickCount());

	InitFireworks();

	m_bInit = TRUE;

	SetTimer(m_hWnd,MAIN_TIMER,500,NULL);
}

//
// On Destroy
//
void OnDestroy(void)
{
	KillTimer(m_hWnd,MAIN_TIMER);
	DeInitFireworks();
	ReleaseDC(m_hWnd, m_hDC);
	if(m_memBmp)
	{
		DeleteObject(m_memBmp);
	}
}

//
// defines to make life easier
//
#define wnd_cx m_bmpInfo.bmiHeader.biWidth
#define wnd_cy m_bmpInfo.bmiHeader.biHeight
#define bit_set(x,y,c){ pix0[x + (wnd_cy-y-1) * wnd_cx] = c; }
#define randint(l,h) (1+(h*rand()/(RAND_MAX+l)))
#define nextDouble ((double)(1.0*rand()/(RAND_MAX+0.0)))

int bits;
double* bit_px;
double* bit_py;
double* bit_vx;
double* bit_vy;
int* bit_sx;
int* bit_sy;
int* bit_l;
int* bit_f;
int* bit_p;
int* bit_c;
int ru;
int rv;
int m_nSizeOfBits2;
int bit_max = 300;

//
// On Render
//
void OnRender(void){

	Sleep(15);
	//static DWORD lastUpdate = 0;
	//if(GetTickCount() >= lastUpdate + 15){
	//	lastUpdate = GetTickCount();
	//}else{
	//	return;
	//}

	// FPS timer
	static DWORD nextUpdate = 0;
	static int count = 0;
	if(nextUpdate < GetTickCount()){
		nextUpdate = GetTickCount()+1000;
		char fps[127]; wsprintf(fps,"%s [FPS: %i]", STV_TITLE, count); SetWindowText(m_hWnd,fps);
		count = 0;
	}else{
		++count;
	}

	// fade code
	for(int j = 0; j < m_nSizeOfBits2; j++)
	{
		int k = pix0[j];
		int l = pix0[j + 1];
		int i1 = pix0[j + wnd_cx];
		int j1 = pix0[j + wnd_cx + 1];
		int i = (k & 0xff0000) >> 16;
		int k1 = ((((l & 0xff0000) >> 16) - i) * ru >> 8) + i;
		i = (k & 0xff00) >> 8;
		int l1 = ((((l & 0xff00) >> 8) - i) * ru >> 8) + i;
		i = k & 0xff;
		int i2 = (((l & 0xff) - i) * ru >> 8) + i;
		i = (i1 & 0xff0000) >> 16;
		int j2 = ((((j1 & 0xff0000) >> 16) - i) * ru >> 8) + i;
		i = (i1 & 0xff00) >> 8;
		int k2 = ((((j1 & 0xff00) >> 8) - i) * ru >> 8) + i;
		i = i1 & 0xff;
		int l2 = (((j1 & 0xff) - i) * ru >> 8) + i;
		int i3 = ((j2 - k1) * rv >> 8) + k1;
		int j3 = ((k2 - l1) * rv >> 8) + l1;
		int k3 = ((l2 - i2) * rv >> 8) + i2;
		pix0[j] = i3 << 16 | j3 << 8 | k3 | 0xff000000;
	}

	// render code
    
	for(int k = 0; k < bits; k++)
		switch(bit_f[k]){
			case 1:
				bit_vy[k] += nextDouble / 50.0;
				bit_px[k] += bit_vx[k];
				bit_py[k] += bit_vy[k];
				bit_l[k]--;
				if(bit_l[k] == 0 || bit_px[k] < 0.0 || bit_py[k] < 0.0 || bit_px[k] > (double)wnd_cx || bit_py[k] > (double)(wnd_cy - 3)){
					bit_c[k] = 0xff000000;
					bit_f[k] = 0;
				}else
					if(bit_p[k] == 0){
						if((int)(nextDouble * 2.0) == 0)
						bit_set((int)bit_px[k], (int)bit_py[k], -1);
					}else{
						bit_set((int)bit_px[k], (int)bit_py[k], bit_c[k]);
					}
				break;
			case 2:
				bit_sy[k] -= 5;
				if((double)bit_sy[k] <= bit_py[k]){
					bit_f[k] = 1;
				}
				if((int)(nextDouble * 20.0) == 0){
					int i = (int)(nextDouble * 2.0);
					int j = (int)(nextDouble * 5.0);
					bit_set(bit_sx[k] + i, bit_sy[k] + j, -1);
				}
				break;
			default:
				break;
	}


	// finally, "swap buffers"
	SetDIBitsToDevice(m_hDC,
					  m_nPlacementX,
					  m_nPlacementY,
					  wnd_cx,
					  wnd_cy,
					  0,
					  0,
					  0,
					  wnd_cy,
					  (LPVOID)pix0,
					  &m_bmpInfo,
					  DIB_RGB_COLORS);
}

//
//
//
void OnLButtonDown(int i, int j){

	int k = (int)(nextDouble * 256.0);
	int l = (int)(nextDouble * 256.0);
	int i1 = (int)(nextDouble * 256.0);
	int j1 = k << 16 | l << 8 | i1 | 0xff000000;
	int k1 = 0;
	for(int l1 = 0; l1 < bits; l1++)
	{
		if(bit_f[l1] != 0)
			continue;
		bit_px[l1] = i;
		bit_py[l1] = j;
		double d = nextDouble * 6.2800000000000002;
		double d1 = nextDouble;
		bit_vx[l1] = sin(d) * d1;
		bit_vy[l1] = cos(d) * d1;
		bit_l[l1] = (int)(nextDouble * 100.0) + 100;
		bit_p[l1] = (int)(nextDouble * 3.0);
		bit_c[l1] = j1;
		bit_sx[l1] = i;
		bit_sy[l1] = wnd_cy - 5;
		bit_f[l1] = 2;
		if(++k1 == bit_max)
			break;
	}
}

//
//
//
void OnTimer(void)
{
	if(rand()%2){
		int x = randint(0,wnd_cx);
		int y = randint(0,wnd_cy)+5;
		y = min(y,wnd_cy);
		OnLButtonDown(x,y);
	}
}

//
//
//
void InitFireworks(void)
{
	m_nSizeOfBits2 = (m_nSizeOfBits - (wnd_cx * 2))/sizeof(RGBQUAD);
    bits	= 10000;
    bit_px	= malloc(bits * sizeof(double));
    bit_py	= malloc(bits * sizeof(double));
    bit_vx	= malloc(bits * sizeof(double));
    bit_vy	= malloc(bits * sizeof(double));
    bit_sx	= malloc(bits * sizeof(int));
    bit_sy	= malloc(bits * sizeof(int));
    bit_l	= malloc(bits * sizeof(int));
    bit_f	= malloc(bits * sizeof(int));
    bit_p	= malloc(bits * sizeof(int));
    bit_c	= malloc(bits * sizeof(int));

    ru = 50;
    rv = 50;

	for(int j = 0; j < bits; j++)
	{
		bit_f[j] = 0;
	}
}

//
//
//
void DeInitFireworks(void)
{
    free(bit_px);
    free(bit_py);
    free(bit_vx);
    free(bit_vy);
    free(bit_sx);
    free(bit_sy);
    free(bit_l);
    free(bit_f);
    free(bit_p);
    free(bit_c);
}
