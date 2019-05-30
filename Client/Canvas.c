#include "Canvas.h"
#include "resource1.h"

extern HWND gWnd;
extern RECT rectWindowProp;
RECT rectOffsetGameBoard;
VOID drawBlocks(const Block* blocksObj, HDC memDc)
{
	HBRUSH hBrush = NULL;
	RECT rect;
	HDC tempDC = CreateCompatibleDC(memDc);

	rect.left = blocksObj->blockPosition.x + rectOffsetGameBoard.left;
	rect.top = blocksObj->blockPosition.y + rectOffsetGameBoard.top;

	rect.right = rect.left + blocksObj->wWidth;
	rect.bottom = rect.top + blocksObj->wHeight;

	switch (blocksObj->typeOfBlock)
	{
	case Normal:
		hBrush = CreateSolidBrush(COLOR_BLOCK_NORMAL);
		break;
	case Magic:
		hBrush = CreateSolidBrush(COLOR_BLOCK_MAGIC);
		break;
	case Rigid:
		if (blocksObj->wNumberOfColisions > 2)
			hBrush = CreateSolidBrush(COLOR_BLOCK_RIGID_DAMAGED);
		else
			hBrush = CreateSolidBrush(COLOR_BLOCK_RIGID_UNDAMAGED);
		break;
	default:
		break;
	}
	//SelectObject(tempDC, hBrush);

	FillRect(memDc, &rect, hBrush);

	DeleteObject(hBrush);
	DeleteDC(tempDC);
}

VOID drawBalls(const Ball* ballObj, HDC memDC)
{
	RECT rect;
	HBITMAP hBmp = NULL;
	BITMAP bmp;
	HDC tempDC = CreateCompatibleDC(memDC);

	ZeroMemory(&rect, sizeof(RECT));

	hBmp = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDB_BITMAP4),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE
	);
	GetObject(hBmp, sizeof(bmp), &bmp);

	SelectObject(tempDC, hBmp);

	rect.left = ballObj->ballPosition.x + rectOffsetGameBoard.left;
	rect.top = ballObj->ballPosition.y + rectOffsetGameBoard.top;

	BitBlt(memDC, rect.left, rect.top, bmp.bmWidth, bmp.bmHeight, tempDC, 0, 0, SRCCOPY);

	DeleteDC(tempDC);
}


VOID drawPlayerBlocks(const PlayerBlock* playerBlockObj, HDC memDC)
{
	RECT rect;
	HBRUSH hBrush = NULL;

	rect.left = rectOffsetGameBoard.left + playerBlockObj->playerBlockPosition.x;
	rect.top = rectOffsetGameBoard.top + playerBlockObj->playerBlockPosition.y;

	rect.right = rect.left + playerBlockObj->wWidth;
	rect.bottom = rect.top + playerBlockObj->wHeight;

	hBrush = CreateSolidBrush(COLOR_PLAYERBLOCK_ENEMY);

	FillRect(memDC, &rect, hBrush);

	DeleteObject(hBrush);
}

VOID drawGameBoard(const GameBoard* gameBoardObj, HDC memDC)
{
	HBRUSH hBrush = NULL;
	RECT rect;
	HDC tempDC = CreateCompatibleDC(memDC);

	//alinhar
	rectOffsetGameBoard.left = rect.left = (rectWindowProp.right - gameBoardObj->wWidth) / 2;
	rectOffsetGameBoard.top = rect.top = (rectWindowProp.bottom - gameBoardObj->wHeight) / 2;

	rectOffsetGameBoard.right = rect.right = gameBoardObj->wWidth + rect.left;
	rectOffsetGameBoard.bottom = rect.bottom = gameBoardObj->wHeight + rect.top;

	hBrush = CreateSolidBrush(COLOR_GAMEBOARD);

	//SelectObject(tempDC, hBrush);

	FrameRect(memDC, &rect, hBrush);

	DeleteObject(hBrush);
	DeleteDC(tempDC);
}

VOID drawGame(const Game* gameObj, HDC memDC)
{
	if (gameObj != NULL)
	{
		drawGameBoard(&gameObj->myGameBoard, memDC);

		drawBlocks(&gameObj->blocks[0], memDC);
		drawBlocks(&gameObj->blocks[1], memDC);
		drawBlocks(&gameObj->blocks[2], memDC);

		drawBalls(&gameObj->ball, memDC);

		drawPlayerBlocks(&gameObj->playerBlocks[0], memDC);
	}
	else
	{
		//espera do jogo
	}
}
