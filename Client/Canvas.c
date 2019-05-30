#include "Canvas.h"

extern HWND gWnd;
extern RECT rectWindowProp;

VOID drawGame(const Game* gameObj, HDC memDC)
{
	if (gameObj != NULL)
	{

		drawGameBoard(&gameObj->myGameBoard, memDC);
	}
}

VOID drawGameBoard(const GameBoard* gameBoardObj, HDC memDC)
{
	HBRUSH hBrush = NULL;
	RECT rect;
	HDC tempDC = CreateCompatibleDC(memDC);

	//alinhar
	rect.left = (rectWindowProp.right - gameBoardObj->wWidth) / 2;
	rect.top = (rectWindowProp.bottom - gameBoardObj->wHeight) / 2;

	rect.right = gameBoardObj->wWidth + rect.left;
	rect.bottom = gameBoardObj->wHeight + rect.top;

	hBrush = CreateSolidBrush(COLOR_GAMEBOARD);
	
	SelectObject(tempDC, hBrush);

	FrameRect(memDC, &rect, hBrush);

	DeleteObject(hBrush);

	DeleteDC(tempDC);
}

