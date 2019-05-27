#include "GameLogic.h"
#include <stdlib.h>

VOID moveBall(Ball* ballToMove)
{
	static WORD i = 0;
	ballToMove->ballPosition.x = i;
	i++;
}


VOID createLevel(Game*gameObj)
{
	createGameBoard(10, 10, 150, 150, gameObj);
}

VOID createGameBoard(WORD wCoordX, WORD wCoordY, WORD wHeigth, WORD wWidth, Game* gameObj)
{
	gameObj->myGameBoard.gameBoardPosition.x = wCoordX;
	gameObj->myGameBoard.gameBoardPosition.y = wCoordY;

	gameObj->myGameBoard.wHeight = wHeigth;
	gameObj->myGameBoard.wWidth = wWidth;
}

VOID createBall(WORD wCoordX, WORD wCoordY, Game* gameObj)
{
	gameObj->ball.ballPosition.x = wCoordX;
	gameObj->ball.ballPosition.y = wCoordY;
}

VOID createPlayerBlock(WORD wCoordX, WORD wCoordY, WORD wHeigth, WORD wWidth, const PTCHAR OwnerUserName, Game* gameObj)
{
	WORD wIndex = 0;

	while (gameObj->playerBlocks[wIndex].wWidth != 0)
	{
		wIndex++;
	}

	gameObj->playerBlocks[wIndex].playerBlockPosition.x = wCoordX;
	gameObj->playerBlocks[wIndex].playerBlockPosition.y = wCoordY;

	gameObj->playerBlocks[wIndex].wHeight = wHeigth;
	gameObj->playerBlocks[wIndex].wWidth = wWidth;

	_tcscpy_s(gameObj->playerBlocks[wIndex].playerOwnerOfBlock.playerInfo.tcUserName,
		_countof(gameObj->playerBlocks[wIndex].playerOwnerOfBlock.playerInfo.tcUserName),
		OwnerUserName);
}

VOID createBlocks(WORD wCoordX, WORD wCoordY, WORD wHeigth, WORD wWidth, TypesOfBlock toBlock, Game* gameObj)
{
	WORD wIndex = 0;

	while (gameObj->blocks[wIndex].wWidth != 0)
	{
		wIndex++;
	}

	gameObj->blocks[wIndex].blockPosition.x = wCoordX;
	gameObj->blocks[wIndex].blockPosition.y = wCoordY;

	gameObj->blocks[wIndex].typeOfBlock = toBlock;

	switch (toBlock)
	{
	case Magic:
	case Normal:
		gameObj->blocks[wIndex].wNumberOfColisions = 1;
		break;
	case Rigid:
		gameObj->blocks[wIndex].wNumberOfColisions = 2;
		break;
	default:
		gameObj->blocks[wIndex].wNumberOfColisions = 1;
		break;
	}
}

VOID createBonus(WORD wCoordX, WORD wCoordY, WORD wHeigth, WORD wWidth, TypeOfBonus toBonus, Game* gameObj)
{
	WORD wIndex = 0;

	while (gameObj->bonusBlock[wIndex].wRadius != 0)
	{
		wIndex++;
	}

	gameObj->bonusBlock[wIndex].bonusCoords.x = wCoordX;
	gameObj->bonusBlock[wIndex].bonusCoords.y = wCoordY;

	gameObj->bonusBlock[wIndex].typeOfBonus = toBonus;
}