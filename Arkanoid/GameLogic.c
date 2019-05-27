#include "GameLogic.h"

VOID moveBall(Ball* ballToMove)
{
	static WORD i = 0;
	ballToMove->ballPosition.x = i;
	i++;
}

VOID createLevel(Game* game)
{
	//criar barras 
	//
}