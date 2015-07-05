/****************************************************************************
 *                                                                          *
 * File    : main.c                                                         *
 *                                                                          *
 * Purpose : Generic Win32 application.                                     *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <stdlib.h>
#include "main.h"

#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))

BITMAPINFO Bitmap;
int GRAPHICWINDOW_X			= 0;
int GRAPHICWINDOW_Y			= 0;
int GRAPHICWINDOW_WIDTH		= 400;
int GRAPHICWINDOW_HEIGHT	= 200;
RGBQUAD						*pixelData;
RGBQUAD						*pixelBuffer;
DWORD						ThreadID;

COLORREF palette[] = 
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


BOOL fuegoInit(int iWidth, int iHeight);
BOOL fuegoDestroy(void);
void fuego(void);

/** Prototypes **************************************************************/

static LRESULT WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);
static void Main_OnPaint(HWND);
static void Main_OnCommand(HWND, int, HWND, UINT);
static void Main_OnDestroy(HWND);
static LRESULT WINAPI AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

/** Global variables ********************************************************/

static HANDLE	ghInstance;
static HWND		mainHwnd;

/****************************************************************************
 *                                                                          *
 * Function: WinMain                                                        *
 *                                                                          *
 * Purpose : Initialize the application.  Register a window class,          *
 *           create and display the main window and enter the               *
 *           message loop.                                                  *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    INITCOMMONCONTROLSEX icc;
    WNDCLASS wc;
    HWND hwnd;
    MSG msg;

    ghInstance = hInstance;

    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES /*|ICC_COOL_CLASSES|ICC_DATE_CLASSES|ICC_PAGESCROLLER_CLASS|ICC_USEREX_CLASSES|... */;
    InitCommonControlsEx(&icc);

    /* Register the main window class */
    wc.lpszClassName	= _T("fireClass");
    wc.lpfnWndProc		= MainWndProc;
    wc.style			= CS_OWNDC;
    wc.hInstance		= ghInstance;
    wc.hIcon			= LoadIcon(ghInstance, MAKEINTRESOURCE(IDR_ICO_MAIN));
    wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName		= MAKEINTRESOURCE(IDR_MNU_MAIN);
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    if (!RegisterClass(&wc))
        return 1;

    /* Create the main window */
    hwnd = CreateWindow(_T("fireClass"),
        _T("fire Program"),
        WS_OVERLAPPEDWINDOW,
        200,
        220,
        600,
        400,
        NULL,
        NULL,
        ghInstance,
        NULL
    );
    if (!hwnd) return 1;

    /* Show and paint the main window */
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* Pump messages until we are done */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

/****************************************************************************
 *                                                                          *
 * Function: MainWndProc                                                    *
 *                                                                          *
 * Purpose : Process application messages.                                  *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	mainHwnd = hwnd;
    switch (msg)
    {
        case WM_COMMAND:
    		switch (LOWORD(wParam))
    		{
        		case IDM_ABOUT:
				break;
			}
		break;

        case WM_DESTROY:
			fuegoDestroy();
			PostQuitMessage(0);
		break;
		case WM_CREATE:
			fuegoInit(100, 100);
			HANDLE fuegoThread = CreateThread(NULL, 0, (void *)fuego ,NULL, THREAD_PRIORITY_NORMAL, &ThreadID);
			CloseHandle(fuegoThread);
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			RECT rc;
			BeginPaint(hwnd, &ps);
			GetClientRect(hwnd, &rc);
			
			SetDIBitsToDevice(
				ps.hdc,						// Target device HDC
				0,							// X sink position
				0,							// Y sink position
				GRAPHICWINDOW_WIDTH,		// Destination width
				GRAPHICWINDOW_HEIGHT,		// Destination height
				0,							// X source position
				0,							// Adjusted Y source position
				(UINT)0,					// Start scan line
				Bitmap.bmiHeader.biHeight,	// Scan lines present
				pixelData,					// Image data
				&Bitmap,					// DIB header
				DIB_RGB_COLORS);			// Type of palette

			EndPaint(hwnd, &ps);
		}
		break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL fuegoInit(int iWidth, int iHeight)
{
    ZeroMemory(&Bitmap, sizeof(BITMAPINFO));
    Bitmap.bmiHeader.biSize				= sizeof(Bitmap.bmiHeader);
    Bitmap.bmiHeader.biWidth			= GRAPHICWINDOW_WIDTH;
    Bitmap.bmiHeader.biHeight			= GRAPHICWINDOW_HEIGHT;
    Bitmap.bmiHeader.biPlanes			= 1;
    Bitmap.bmiHeader.biBitCount			= 32;	//bits por pixel
    Bitmap.bmiHeader.biCompression		= BI_RGB;
    Bitmap.bmiHeader.biSizeImage		= 0;
    Bitmap.bmiHeader.biXPelsPerMeter	= 0;
    Bitmap.bmiHeader.biYPelsPerMeter	= 0;
    Bitmap.bmiHeader.biClrUsed			= 0;
    Bitmap.bmiHeader.biClrImportant		= 0;

	pixelData	= (RGBQUAD*)VirtualAlloc(NULL, GRAPHICWINDOW_WIDTH * GRAPHICWINDOW_HEIGHT * sizeof(RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	pixelBuffer	= (RGBQUAD*)VirtualAlloc(NULL, GRAPHICWINDOW_WIDTH * GRAPHICWINDOW_HEIGHT * sizeof(RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	memset(pixelData,0x00,GRAPHICWINDOW_WIDTH * GRAPHICWINDOW_HEIGHT * sizeof(RGBQUAD));
	
	return TRUE;
}

BOOL fuegoDestroy(void)
{
	VirtualFree(pixelData, 0, MEM_RELEASE);
	VirtualFree(pixelBuffer, 0, MEM_RELEASE);
	return TRUE;

}

void fuego(void)
{
	int x;
	int y;
	RGBQUAD *pixel;
	#define calcoffset(_x, _y) (GRAPHICWINDOW_WIDTH * _y) + _x
	srand(GetTickCount());

	do
	{
	for(int y = 1; y < GRAPHICWINDOW_HEIGHT; y++)
	{
		if(y == 2) { Sleep(110); }
		for(int x = 1; x < GRAPHICWINDOW_WIDTH; x++)
		{
			DWORD tmpRed;
			RGBQUAD *pixel = (RGBQUAD *)&pixelData[x + (y * GRAPHICWINDOW_WIDTH)];
			DWORD *tmp	= (DWORD *)&pixelData[x + (y * GRAPHICWINDOW_WIDTH)];
			DWORD *a	= (DWORD *)&pixelData[(x - 1) + ((y - 1) * GRAPHICWINDOW_WIDTH)];
			DWORD *b	= (DWORD *)&pixelData[(x - 1) + ((y + 1) * GRAPHICWINDOW_WIDTH)];
			DWORD *c	= (DWORD *)&pixelData[(x + 1) + ((y - 1) * GRAPHICWINDOW_WIDTH)];
			DWORD *d	= (DWORD *)&pixelData[(x + 1) + ((y + 1) * GRAPHICWINDOW_WIDTH)];
			int rnd = rand() % 4 + 1;
			switch (rnd)
			{
				case 1: (*tmp = *a + *b - *c + *d / 0.93);
				//case 2: *tmp = *c - *d / 4.93;
				//case 3: *tmp = *b + *d / 2.93;
				//case 4: *tmp = *c + *d / 2.93;
			}
			//*tmp = *tmp % 20 + 1;
			//*tmp = palette[*tmp % 2000 + 1];

		}
	}
	// Fuel //
	for(int y = 0; y < 1; y++)
	{
		for(int x = 0; x < GRAPHICWINDOW_WIDTH; x++)
		{
			RGBQUAD *pixel = (RGBQUAD *)&pixelData[x + (y * GRAPHICWINDOW_WIDTH)];
			pixel->rgbRed   = palette[rand() % (203-50)+73];
			pixel->rgbGreen = 0x00; //rand() % (103-50)+73;
			pixel->rgbBlue  = 0x00;
		}
	}

	RECT rect;
	rect.left   = GRAPHICWINDOW_X;
	rect.top    = GRAPHICWINDOW_Y;
	rect.right  = GRAPHICWINDOW_X + GRAPHICWINDOW_WIDTH;
	rect.bottom = GRAPHICWINDOW_Y + GRAPHICWINDOW_HEIGHT;
	InvalidateRect(mainHwnd, 0, FALSE);
	UpdateWindow(mainHwnd);
	char msg[1024];
	wsprintf(msg, "Ticks: %d",GetTickCount());
	SetWindowText(mainHwnd,msg);
	}while(TRUE);
}
