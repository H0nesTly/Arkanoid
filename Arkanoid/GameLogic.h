#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\GameStructures.h"

VOID moveBall(Ball*);

VOID createLevel(Game*);

VOID createGameBoard(WORD, WORD, WORD, WORD, Game*);

VOID createBall(WORD, WORD, Game*);

VOID createPlayerBlock(WORD, WORD, WORD, WORD, const PTCHAR, Game*);

VOID createBlocks(WORD, WORD, WORD, WORD, TypesOfBlock, Game*);

VOID createBonus(WORD, WORD, WORD, WORD, TypeOfBonus, Game*);


