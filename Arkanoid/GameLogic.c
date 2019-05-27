#include "GameLogic.h"
#include <stdlib.h>

VOID moveBall(Ball* ballToMove)
{
	static WORD i = 0;
	ballToMove->ballPosition.x = i;
	i++;
}

//TESTAA
VOID createLevel(Game*gameObj)
{
	createBlocks(5, 6, 30, 10, Magic, gameObj);
	createBlocks(2, 4, 30, 10, Rigid, gameObj);
	createBlocks(2, 3, 30, 10, Normal, gameObj);
	createBall(1, 1, gameObj);

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

	gameObj->blocks[wIndex].wWidth = wWidth;
	gameObj->blocks[wIndex].wHeight = wHeigth;

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
	gameObj->bonusBlock[wIndex].wRadius = wCoordY;

	gameObj->bonusBlock[wIndex].typeOfBonus = toBonus;
}

VOID destroyBonus(WORD wIndex, Game* gameObj)
{
	if (wIndex >= 0 && wIndex < NUM_OF_OBJ_GAME)
	{
		ZeroMemory(&gameObj->bonusBlock[wIndex], sizeof(BonusBlock));
	}
}

VOID catchBonus(WORD wIndex, Game* gameObj)
{
	if (wIndex >= 0 && wIndex < NUM_OF_OBJ_GAME)
	{
		switch (gameObj->blocks[wIndex].)
		{
			case SpeedUp:
			break;
			case SlowDown:
			//diminiu a velocidade da bola
			break;
			case ExtraHealth:
			//adiciona vida
			break;
			case Triple:
				//cria duas bolas
			break;
			//Nao faz nada
			case None:
			default:
			break;
		}
		destroyBonus(wIndex, gameObj);
	}

}