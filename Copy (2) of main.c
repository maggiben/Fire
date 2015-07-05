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
DWORD						*pixelData;
RGBQUAD						*pixelBuffer;
DWORD						ThreadID;
inline DWORD PixBuf(register int x,register int y) {return pixelData[x + (y * GRAPHICWINDOW_WIDTH)];}
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
void fuegoASM(void);
void fuegoASM2(void);
void SetPalette(HDC MainDC);
int FireFrame();

//
// defines
//
#define STV_CLASS "fireworks_class"
#define STV_TITLE "Fireworks"
#define WND_WIDTHHEIGHT 360

//
// Global Variables
//
HINSTANCE	m_hInst = NULL;
HWND		m_hWnd = NULL;
BOOL		m_bInit = FALSE;

void OnCreate();
void OnDestroy();
void OnLButtonDown(int x, int y);
void OnTimer();

void InitFireworks();
void DeInitFireworks();
void OnRender();
// Set this to SIDES for only checking on the sides or
// set this to DIAGONALS for only check on the diagonals or
// set this to ALL for checking of all surrounding pixels

#define SIDES

#if defined(ALL)
#define SIDES
#define DIAGONALS
#endif
const unsigned int screen_w = 320;
const unsigned int screen_h = 200;
const unsigned int offset_x = 0;
const unsigned int offset_y = 0;
const unsigned int bufsize = 320*200;

unsigned int mouse_w = 10;
unsigned int mouse_h = 10;

unsigned int decay = 0;

unsigned far char *screen=(unsigned char far*)0xA0000000L;
unsigned far char buffera[320*200];
unsigned far char bufferb[320*200];
unsigned far char *buffer1=buffera;
unsigned far char *buffer2=bufferb;
unsigned far char *destline=NULL;
unsigned far char *line=NULL;
unsigned far char *lineup=NULL;
unsigned far char *linedown=NULL;

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
	m_hInst = hInstance;

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
			m_hWnd = hwnd;
			fuegoDestroy();
			PostQuitMessage(0);
		break;
		case WM_CREATE:
			fuegoInit(100, 100);
			OnCreate();
			HANDLE fuegoThread = CreateThread(NULL, 0, (void *)OnRender, NULL, THREAD_PRIORITY_NORMAL, &ThreadID);
			CloseHandle(fuegoThread);
		break;
		case WM_TIMER:
			OnTimer();
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown(LOWORD(lParam),HIWORD(lParam));
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

	pixelData	= (DWORD*)VirtualAlloc(NULL, GRAPHICWINDOW_WIDTH * GRAPHICWINDOW_HEIGHT * sizeof(RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	pixelBuffer	= (RGBQUAD*)VirtualAlloc(NULL, GRAPHICWINDOW_WIDTH * GRAPHICWINDOW_HEIGHT * sizeof(RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	memset(pixelData,0x00,GRAPHICWINDOW_WIDTH * GRAPHICWINDOW_HEIGHT * sizeof(RGBQUAD));
	memset(pixelBuffer,0x00,GRAPHICWINDOW_WIDTH * GRAPHICWINDOW_HEIGHT * sizeof(RGBQUAD));
	
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
	DWORD FirstPixel;
	char msg[255];

	#define calcoffset(_x, _y) (GRAPHICWINDOW_WIDTH * _y) + _x
	srand(GetTickCount());

	do
	{
		for(int y = 3; y < GRAPHICWINDOW_HEIGHT; y++)
		{
			//if(y == 2) { Sleep(250); }
			for(int x = 0; x < GRAPHICWINDOW_WIDTH; x++)
			{
				DWORD	*ptr;
				DWORD	*pixel = (DWORD *)&pixelData[x + (y * GRAPHICWINDOW_WIDTH)];
				
				int rnd = rand() % 4 + 1;
				switch (rnd)
				{
					case 1:
					{
						ptr = (DWORD *)&pixelData[(x - 1) + ((y - 1) * GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						ptr = (DWORD *)&pixelData[x + ((y - 1) * GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						ptr = (DWORD *)&pixelData[(x + 1) + ((y - 1) * GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						*pixel = *pixel / 3.78;
						break;
					}
					case 2:
					{
						ptr = (DWORD *)&pixelData[(x - 1) + (y * GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						ptr = (DWORD *)&pixelData[x + ((y - 1) * GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						ptr = (DWORD *)&pixelData[(x + 1) + ((y  - 1)* GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						*pixel = *pixel / 1.89;
						break;
					}
					case 3:
					{
						ptr = (DWORD *)&pixelData[(x - 1) + ((y - 1) * GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						ptr = (DWORD *)&pixelData[x + ((y - 1) * GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						ptr = (DWORD *)&pixelData[x + (y * GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						*pixel = *pixel / 10.99;
						break;
					}
					case 4:
					{
						ptr = (DWORD *)&pixelData[(x - 1) + (y * GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						ptr = (DWORD *)&pixelData[x + ((y - 1) * GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						ptr = (DWORD *)&pixelData[(x + 1) + (y * GRAPHICWINDOW_WIDTH)];
						*pixel += *ptr;
						*pixel = *pixel / 11.99;
						break;
					}
				}
			}
		}
		// Fuel //
		for(int y = 0; y < 4; y++)
		{
			for(int x = 0; x < GRAPHICWINDOW_WIDTH; x++)
			{
				DWORD *pos;
				DWORD *pixel = (DWORD *)&pixelData[x + (y * GRAPHICWINDOW_WIDTH)];
				*pixel = rand() % (128-73)+73;
				//pixel = (DWORD *)&pixelData[x + ((y + 1) * GRAPHICWINDOW_WIDTH)];
				//*pixel = rand() % (110-65)+65;//palette[rand() % (256)+1];
			}
		}
		InvalidateRect(mainHwnd, 0, FALSE);
		UpdateWindow(mainHwnd);
		char msg[1024];
		wsprintf(msg, "Ticks: %d",GetTickCount());
		//SetWindowText(mainHwnd,msg);
	}while(TRUE);
}
void fuegoASM(void)
{
	int x;
	int y;
	BYTE			*FireTable;
	int temp=0;
	int mouse_x=20,mouse_y=20;
	int mouse_b=0;
	int done=0;
	unsigned char resp=0;

	FireTable = malloc(65536 * sizeof(BYTE));
	memset(FireTable, 0, 65536);
	do
	{
		// Fuel //
		for(int x = 0; x < GRAPHICWINDOW_WIDTH; x++)
		{
			RGBQUAD *pixel = (RGBQUAD *)&pixelData[x + (0 * GRAPHICWINDOW_WIDTH)];
			pixel->rgbRed = rand() % (128-73)+73;
			pixel = (RGBQUAD *)&pixelData[x + (1 * GRAPHICWINDOW_WIDTH)];
			pixel->rgbRed = rand() % (110-65)+65;
		}
		for(int y = 3; y < GRAPHICWINDOW_HEIGHT; y++)
		{
			destline=	(buffer1+((y  )	*screen_w));
			linedown=	(buffer2+((y-1)	*screen_w));
			line=		(buffer2+((y  )	*screen_w));
			lineup=		(buffer2+((y+1)	*screen_w));
			for(int x = 0; x < GRAPHICWINDOW_WIDTH; x++)
			{
				temp=0;
			#if defined(DIAGONALS)
				temp+=*(lineup  +x+1);
				temp+=*(lineup  +x-1);
				temp+=*(linedown+x+1);
				temp+=*(linedown+x-1);
			#endif
			#if defined(SIDES)
				temp+=*(lineup  +x);
				temp+=*(line    +x+1);
				temp+=*(line	+x-1);
				temp+=*(linedown+x);
			#endif
			#if defined(SIDES) && defined(DIAGONALS)
				temp = (temp>>3);
			#else
				temp = (temp>>2);
			#endif
				if(temp>=decay)temp-=decay;
				else temp=0;
				*(destline+x)=temp;

			}
		}
		InvalidateRect(mainHwnd, 0, FALSE);
		UpdateWindow(mainHwnd);
	}
	while(TRUE);
}
void fuegoASM2(void)
{
	unsigned int x,y;
	BOOL done = FALSE;

	while(!done)
	{
		// Fuel //
		for(int y = 0; y < 4; y++)
		{
			for(int x = 0; x < GRAPHICWINDOW_WIDTH; x++)
			{
				DWORD *pos;
				DWORD *pixel = (DWORD *)&pixelData[x + (y * GRAPHICWINDOW_WIDTH)];
				*pixel = 0xFF0000 ;//+ rand() % (0xFF0000-73)+73;
			}
		}
		// Fuel //
		Sleep(25);
		for(int y = 4; y < GRAPHICWINDOW_HEIGHT; y++)
		{
			for(int x = 0; x < GRAPHICWINDOW_WIDTH; x++)
			{
				DWORD *pixel = (DWORD *)&pixelData[x + (y * GRAPHICWINDOW_WIDTH)];
				int rnd = rand() % 4 + 1;
				switch (rnd)
				{
					case 1:
					{
						*pixel =	PixBuf( x-1, y-1 );
						*pixel +=	PixBuf( x, y-1 );
						*pixel +=	PixBuf( x+1, y-1 );
						*pixel = *pixel / 3.93;
						*pixel = *pixel % 256;
						//*pixel = palette[*pixel];
						break;
					}
					case 2:
					{
						*pixel =	PixBuf( x-1, y);
						*pixel +=	PixBuf( x, y-1 );
						*pixel +=	PixBuf( x+1, y );
						*pixel = *pixel / 2.75;
						*pixel = *pixel % 256;
						//*pixel = palette[*pixel];
						break;
					}
					case 3:
					{
						*pixel =	PixBuf( x-1, y-1 ); 
						*pixel +=	PixBuf( x, y-1 );
						*pixel +=	PixBuf( x, y );
						*pixel = *pixel / 2.93;
						*pixel = *pixel % 256;
						//*pixel = palette[*pixel];
						break;
					}
					case 4:
					{
						*pixel =	PixBuf( x-1, y );
						*pixel +=	PixBuf( x, y-1 ); 
						*pixel +=	PixBuf( x+1, y );
						*pixel = *pixel / 3.93;
						*pixel = *pixel % 256;
						//*pixel = palette[*pixel];
						break;
					}

				}
			}
		}
		InvalidateRect(mainHwnd, 0, FALSE);
		UpdateWindow(mainHwnd);
	}

}
void SetPalette(HDC MainDC)
{
	NPLOGPALETTE Palette;
	HPALETTE		Pal;
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
	for(int i = 118; i < 236; i++)
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
void OnCreate(){

	m_nPlacementX = GetSystemMetrics(SM_CXSIZEFRAME);
	m_nPlacementY = GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYSIZEFRAME);
	int nWidth = WND_WIDTHHEIGHT - m_nPlacementX*2;
	int nHeight = WND_WIDTHHEIGHT - m_nPlacementY - GetSystemMetrics(SM_CYSIZEFRAME);

	m_nSizeOfBits = (((nWidth * 32 + 31) & ~31) >> 3) * nHeight;
	m_nNumPixels = m_nSizeOfBits / sizeof(RGBQUAD);

	memset(&m_bmpInfo,0,sizeof(m_bmpInfo));
	m_bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmpInfo.bmiHeader.biWidth = nWidth;
	m_bmpInfo.bmiHeader.biHeight = nHeight;
	m_bmpInfo.bmiHeader.biSizeImage = m_nSizeOfBits;
	m_bmpInfo.bmiHeader.biPlanes = 1;
	m_bmpInfo.bmiHeader.biBitCount = 32;

	m_hDC = GetWindowDC(m_hWnd);
	m_memBmp = CreateDIBSection(m_hDC, &m_bmpInfo, DIB_RGB_COLORS, (void**)&pix0, NULL, 0);
	memset((void*)pix0,0,m_nSizeOfBits);

	srand(GetTickCount());

	InitFireworks();

	m_bInit = TRUE;

	SetTimer(m_hWnd,12345,500,NULL);
}

//
// On Destroy
//
void OnDestroy()
{
	KillTimer(m_hWnd,12345);
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
void OnRender(){

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
	for(int j = 0; j < m_nSizeOfBits2; j++){
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
	for(int l1 = 0; l1 < bits; l1++){
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
void OnTimer(){
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
void InitFireworks(){
	m_nSizeOfBits2 = (m_nSizeOfBits - (wnd_cx * 2))/sizeof(RGBQUAD);
    bits = 10000;
    bit_px = malloc(bits * sizeof(double));
    bit_py = malloc(bits * sizeof(double));
    bit_vx = malloc(bits * sizeof(double));
    bit_vy = malloc(bits * sizeof(double));
    bit_sx = malloc(bits * sizeof(int));
    bit_sy = malloc(bits * sizeof(int));
    bit_l = malloc(bits * sizeof(int));
    bit_f = malloc(bits * sizeof(int));
    bit_p = malloc(bits * sizeof(int));
    bit_c = malloc(bits * sizeof(int));
    ru = 50;
    rv = 50;

    for(int j = 0; j < bits; j++){
        bit_f[j] = 0;
	}
}

//
//
//
void DeInitFireworks()
{
    free(bit_px);
    free( bit_py);
    free( bit_vx);
    free( bit_vy);
    free( bit_sx);
    free( bit_sy);
    free( bit_l);
    free( bit_f);
    free( bit_p);
    free( bit_c);
}

