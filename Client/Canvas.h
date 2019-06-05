#pragma once
#include "ClientStructures.h"
#include "../Communicate/GameStructures.h"

#define COLOR_PLAYERPADDLE_ENEMY RGB(255, 0, 0)
#define COLOR_PLAYERPADDLE_WATCHING RGB(0, 0, 255)
#define COLOR_MYPLAYERPADDLE RGB(0, 255, 0)

#define COLOR_BALL RGB(255, 255, 255)

#define COLOR_BLOCK_MAGIC RGB(255, 204, 255)
#define COLOR_BLOCK_NORMAL RGB(178, 102, 255)
#define COLOR_BLOCK_RIGID_UNDAMAGED RGB(0, 0, 153)
#define COLOR_BLOCK_RIGID_DAMAGED RGB(51, 51, 255)

#define COLOR_GAMEBOARD RGB(0, 233, 0)

VOID drawScore(const DWORD, HDC);

VOID drawHealth(const WORD, HDC);

VOID drawBlocks(const Block*, HDC);

VOID drawPlayerPaddles(const Paddle*, HDC);

VOID drawBalls(const Ball*, HDC);

VOID drawBonus(const BonusBlock*, HDC);

VOID drawGameBoard(const GameBoard*, HDC);

VOID drawGame(const Game*, HDC);

