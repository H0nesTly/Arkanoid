﻿#include "Canvas.h"
#include "resource.h"
#include <stdlib.h>

extern HWND gWnd;
extern RECT rectWindowProp;
extern ClientStructure gClientInfo;
RECT rectOffsetGameBoard;

HBITMAP hBmpBall = NULL;
HBITMAP hBmpBonusSpeedUp = NULL;
HBITMAP hBmpBonusSlowDown = NULL;
HBITMAP hBmpBonusExtraHealth = NULL;
HBITMAP hBmpBonusTriple = NULL;
HBITMAP hBmpLife = NULL;
HBITMAP hBmpMyPaddle = NULL;
HBITMAP hBmpEnemyPaddle = NULL;
HBITMAP hBmpBackGround = NULL;

static VOID initResources()
{
	hBmpBall = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDB_BITMAPBALL),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE
	);

	hBmpBonusSpeedUp = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDB_BITMAPSPEEDUP),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE
	);

	hBmpBonusSlowDown = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDB_BITMAPSLOWDOWN),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE
	);

	hBmpBonusExtraHealth = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDB_BITMAPEXTRAHEALTH),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE
	);

	hBmpBonusTriple = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDB_BITMAPTRIPLE),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE
	);

	hBmpLife = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDB_BITMAPLIFE),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE
	);

	hBmpBackGround = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDB_BITMAP_BACKGROUND),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE
	);

	hBmpMyPaddle = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDB_BITMAP_MY_PADDLE),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE
	);

	hBmpEnemyPaddle = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDB_BITMAP_ENEMY_PADDLE),
		IMAGE_BITMAP,
		0, 0,
		LR_DEFAULTSIZE
	);
}

VOID drawScore(const DWORD wScore, HDC memDC)
{
	RECT rect;
	TCHAR tcScore[20];
	SetTextColor(memDC, RGB(255, 255, 255));
	SetBkMode(memDC, TRANSPARENT);
	rect.left = rectOffsetGameBoard.left + 128;
	rect.top = 15;

	_stprintf_s(tcScore, _countof(tcScore), TEXT("Pontuação : %d"), wScore);
	DrawText(memDC, tcScore, -1, &rect, DT_SINGLELINE | DT_NOCLIP);
}

VOID drawHealth(const WORD wLifesLeft, HDC memDC)
{
	RECT rect;
	BITMAP bmp;
	HDC tempDC = CreateCompatibleDC(memDC);

	ZeroMemory(&rect, sizeof(RECT));
	GetObject(hBmpLife, sizeof(bmp), &bmp);

	SelectObject(tempDC, hBmpLife);


	rect.left = rectOffsetGameBoard.left;
	rect.top = 15;

	//TransparentBlt

	for (int i = 0; i < wLifesLeft; i++)
	{
		BitBlt(memDC, rect.left + bmp.bmWidth * i, rect.top, bmp.bmWidth, bmp.bmWidth, tempDC, 0, 0, SRCCOPY);
	}

	DeleteDC(tempDC);
}

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
		if (blocksObj->wNumberOfColisions == 1)
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
	BITMAP bmp;
	HDC tempDC = CreateCompatibleDC(memDC);

	ZeroMemory(&rect, sizeof(RECT));

	GetObject(hBmpBall, sizeof(bmp), &bmp);

	SelectObject(tempDC, hBmpBall);

	rect.left = ballObj->ballPosition.x + rectOffsetGameBoard.left;
	rect.top = ballObj->ballPosition.y + rectOffsetGameBoard.top;

	BitBlt(memDC, rect.left, rect.top, ballObj->wWitdh, ballObj->wHeight, tempDC, 0, 0, SRCCOPY);

	DeleteDC(tempDC);
}

VOID drawBonus(const BonusBlock* bonusObj, HDC memDC)
{
	RECT rect;
	HBITMAP hBmp = NULL;
	BITMAP bmp;
	HDC tempDC = CreateCompatibleDC(memDC);

	ZeroMemory(&rect, sizeof(RECT));

	switch (bonusObj->typeOfBonus)
	{
	case ExtraHealth:
		hBmp = hBmpBonusExtraHealth;
		break;
	case SpeedUp:
		hBmp = hBmpBonusSpeedUp;
		break;
	case Triple:
		hBmp = hBmpBonusTriple;
		break;
	case SlowDown:
		hBmp = hBmpBonusSlowDown;
		break;
	}

	GetObject(hBmp, sizeof(bmp), &bmp);

	SelectObject(tempDC, hBmp);

	rect.left = bonusObj->bonusCoords.x + rectOffsetGameBoard.left;
	rect.top = bonusObj->bonusCoords.y + rectOffsetGameBoard.top;

	BitBlt(memDC, rect.left, rect.top, bonusObj->wWidth, bonusObj->wHeight, tempDC, 0, 0, SRCCOPY);

	DeleteDC(tempDC);
}

VOID drawPlayerPaddles(const Paddle* playerBlockObj, HDC memDC)
{
	RECT rect;
	HBITMAP hBmp = NULL;
	BITMAP bmp;
	HDC tempDC = CreateCompatibleDC(memDC);

	ZeroMemory(&rect, sizeof(RECT));

	rect.left = rectOffsetGameBoard.left + playerBlockObj->playerBlockPosition.x;
	rect.top = rectOffsetGameBoard.top + playerBlockObj->playerBlockPosition.y;

	if (gClientInfo.myMode == WatchingGame)
	{
		hBmp = hBmpEnemyPaddle;
	}
	else
	{
		if (_tcscmp(gClientInfo.tcUserName, playerBlockObj->playerOwnerOfBlock.playerInfo.tcUserName) == 0)
		{
			hBmp = hBmpMyPaddle;
			drawScore(playerBlockObj->playerOwnerOfBlock.dwScore, memDC);
		}
		else
		{
			hBmp = hBmpEnemyPaddle;
		}
	}

	GetObject(hBmp, sizeof(bmp), &bmp);

	SelectObject(tempDC, hBmp);

	BitBlt(memDC, rect.left, rect.top, playerBlockObj->wWidth, playerBlockObj->wHeight, tempDC, 0, 0, SRCCOPY);

	DeleteDC(tempDC);
}

VOID drawGameBoard(const GameBoard* gameBoardObj, HDC memDC)
{
	BITMAP bmp;
	HDC tempDC = CreateCompatibleDC(memDC);

	//alinhar
	rectOffsetGameBoard.left = (rectWindowProp.right - gameBoardObj->wWidth) / 2;
	rectOffsetGameBoard.top = (rectWindowProp.bottom - gameBoardObj->wHeight) / 2;

	rectOffsetGameBoard.right = gameBoardObj->wWidth + rectOffsetGameBoard.left;
	rectOffsetGameBoard.bottom = gameBoardObj->wHeight + rectOffsetGameBoard.top;

	GetObject(hBmpBackGround, sizeof(bmp), &bmp);

	SelectObject(tempDC, hBmpBackGround);

	BitBlt(memDC, rectOffsetGameBoard.left, rectOffsetGameBoard.top, rectOffsetGameBoard.right, rectOffsetGameBoard.bottom, tempDC, 0, 0, SRCCOPY);

	DeleteDC(tempDC);
}

VOID drawGame(const Game* gameObj, HDC memDC)
{
	if (gameObj != NULL)
	{
		drawGameBoard(&gameObj->myGameBoard, memDC);

		drawHealth(gameObj->wLifes, memDC);

		for (WORD i = 0; i < gameObj->wNumberOfBlocks; ++i)
		{
			drawBlocks(&gameObj->blocks[i], memDC);
		}

		for (int i = 0; i < gameObj->wNumberOfBalls; ++i)
			drawBalls(&gameObj->ball[i], memDC);

		for (WORD i = 0; i < gameObj->wNumberOfPlayerPaddles; i++)
			drawPlayerPaddles(&gameObj->PlayerPaddles[i], memDC);

		for (WORD i = 0; i < gameObj->wNumberOfBonusDropping; i++)
			drawBonus(&gameObj->bonusBlock[i], memDC);
	}
	else
	{
		initResources();
	}
}
