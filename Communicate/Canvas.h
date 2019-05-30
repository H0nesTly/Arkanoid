#pragma once
#include "DllSetup.h"
#include "GameStructures.h"

#define COLOR_PLAYERBLOCK_ENEMY RGB(255, 0, 0)
#define COLOR_MYPLAYERBLOCK_ENEMY RGB(0, 255, 0)

#define COLOR_BALL RGB(255, 255, 255)

#define COLOR_BLOCK_MAGIC RGB(255, 204, 255)
#define COLOR_BLOCK_NORMAL RGB(178, 102, 255)
#define COLOR_BLOCK_RIGID_UNDAMAGED RGB(0, 0, 153)
#define COLOR_BLOCK_RIGID_DAMAGED RGB(51, 51, 255)


VOID drawBlocks(Block*, HDC);

VOID drawBalls(Ball*, HDC);

VOID drawBonus(BonusBlock*, HDC);

VOID drawGameBoard(GameBoard* gameBoardObj, HDC memDC)
{
	//desenhar quadrado
}

VOID drawGame(Game* gameObj, Windowhandlers* wHandlers)
{
	drawGameBoard(&gameObj->myGameBoard, wHandlers->memDC);
}
