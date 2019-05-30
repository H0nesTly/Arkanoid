#include "Canvas.h"

extern HWND gWnd;

VOID drawGame(const Game* gameObj, HDC memDC)
{
	drawGameBoard(&gameObj->myGameBoard,memDC);
}

VOID drawGameBoard(const GameBoard* gameBoardObj, HDC memDC)
{
	HDC tempDC = NULL;
	HBRUSH hBrush = NULL;
		RECT rect;

	//tempDC = CreateCompatibleDC(wndH->memDC);

	hBrush = CreateSolidBrush(COLOR_GAMEBOARD);
	
	SelectObject(memDC, hBrush);
	
	 //Rectangle(memDC, 
		// gameBoardObj->gameBoardPosition.x,
		// gameBoardObj->gameBoardPosition.y,
		// gameBoardObj->gameBoardPosition.x + gameBoardObj->wWidth,
		// gameBoardObj->gameBoardPosition.y + gameBoardObj->wHeight
	 //);
	Rectangle(memDC, 
		 0,
		 0,
		 40,
		 90
	 );
	
		GetClientRect(gWnd, &rect);
}

