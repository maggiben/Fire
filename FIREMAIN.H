#include <stdlib.h>

typedef struct
{
	HINSTANCE 	hInstance;
    HINSTANCE 	hPrevInst;
	int			cmdShow;

} WINAPP;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
