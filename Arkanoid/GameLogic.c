#include "GameLogic.h"
#include <stdlib.h>

VOID moveBall(Game* gameObj)
{
	//if (ballToMove->ballPosition.y < 550)
	//{
	//	//perde uma vida 
	//}

	Ball * ballToMove = &gameObj->ball;

	if ((ballToMove->ballPosition.x + (ballToMove->nMovementVectorX * ballToMove->wVelocity)) < 0)
	{
		ballToMove->nMovementVectorX = ballToMove->nMovementVectorX * -1;
		ballToMove->ballPosition.x = 1;
	}
	else
	{
		if ((ballToMove->ballPosition.x + ballToMove->wWitdh + (ballToMove->nMovementVectorX * ballToMove->wVelocity)) >= DEFAULT_WIDTH_OF_GAMEBOARD)
		{
			ballToMove->nMovementVectorX = ballToMove->nMovementVectorX * -1;
			ballToMove->ballPosition.x = DEFAULT_WIDTH_OF_GAMEBOARD - 1;
		}
	}


	if ((ballToMove->ballPosition.y + (ballToMove->nMovementVectorY * ballToMove->wVelocity)) < 0)
	{
		ballToMove->nMovementVectorY = ballToMove->nMovementVectorY * -1;
		ballToMove->ballPosition.y = 1;
	}
	else
	{
		if (ballToMove->ballPosition.y + ballToMove->wHeigth + (ballToMove->nMovementVectorX * ballToMove->wVelocity) > DEFAULT_HEIGTH_OF_GAMEBOARD - 20)
		{
			//perdeu!!
		}
	}


	for (size_t i = 0; i < gameObj->wNumberOfBlocks; i++)
	{
		checkColissionBallObject(ballToMove, &gameObj->blocks[i].blockPosition, gameObj->blocks[i].wWidth, gameObj->blocks[i].wHeight);
	}

	ballToMove->ballPosition.x += ballToMove->nMovementVectorX * ballToMove->wVelocity;
	ballToMove->ballPosition.y += ballToMove->nMovementVectorY * ballToMove->wVelocity;

}

VOID createLevel(Game*gameObj)
{
	createGameBoard(0, 0, DEFAULT_HEIGTH_OF_GAMEBOARD, DEFAULT_WIDTH_OF_GAMEBOARD, &gameObj->myGameBoard);

	createBlocks(35, 40, 10, 30, Magic, gameObj);
	createBlocks(66, 40, 10, 30, Magic, gameObj);
	createBlocks(97, 40, 10, 30, Magic, gameObj);
	//+50
	createBlocks(147, 40, 10, 30, Magic, gameObj);
	createBlocks(178, 40, 10, 30, Magic, gameObj);
	createBlocks(209, 40, 10, 30, Magic, gameObj);
	//+50
	createBlocks(259, 40, 10, 30, Magic, gameObj);
	createBlocks(290, 40, 10, 30, Magic, gameObj);
	createBlocks(321, 40, 10, 30, Magic, gameObj);
	//LINHA	
	createBlocks(35, 51, 10, 30, Rigid, gameObj);
	createBlocks(66, 51, 10, 30, Rigid, gameObj);
	createBlocks(97, 51, 10, 30, Rigid, gameObj);
	//+50
	createBlocks(147, 51, 10, 30, Rigid, gameObj);
	createBlocks(178, 51, 10, 30, Rigid, gameObj);
	createBlocks(209, 51, 10, 30, Rigid, gameObj);
	//+50
	createBlocks(259, 51, 10, 30, Rigid, gameObj);
	createBlocks(290, 51, 10, 30, Rigid, gameObj);
	createBlocks(321, 51, 10, 30, Rigid, gameObj);

	//LINHA	
	createBlocks(35, 61, 10, 30, Normal, gameObj);
	createBlocks(66, 61, 10, 30, Normal, gameObj);
	createBlocks(97, 61, 10, 30, Normal, gameObj);
	//+50				  
	createBlocks(147, 61, 10, 30, Normal, gameObj);
	createBlocks(178, 61, 10, 30, Normal, gameObj);
	createBlocks(209, 61, 10, 30, Normal, gameObj);
	//+50				  
	createBlocks(259, 61, 10, 30, Normal, gameObj);
	createBlocks(290, 61, 10, 30, Normal, gameObj);
	createBlocks(321, 61, 10, 30, Normal, gameObj);

	createBall(10, 15, gameObj);

}

VOID createGameBoard(WORD wCoordX, WORD wCoordY, WORD wHeigth, WORD wWidth, GameBoard* gameObj)
{
	gameObj->gameBoardPosition.x = wCoordX;
	gameObj->gameBoardPosition.y = wCoordY;

	gameObj->wHeight = wHeigth;
	gameObj->wWidth = wWidth;
}

VOID createBall(WORD wCoordX, WORD wCoordY, Game* gameObj)
{
	gameObj->ball.ballPosition.x = wCoordX;
	gameObj->ball.ballPosition.y = wCoordY;

	gameObj->ball.wHeigth = gameObj->ball.wWitdh = 8;

	gameObj->ball.nMovementVectorX = 1;
	gameObj->ball.nMovementVectorY = -1;

	gameObj->ball.wVelocity = DEFAULT_BALL_VELOCITY;
}

VOID createPlayerBlock(WORD wCoordX, WORD wCoordY, WORD wHeigth, WORD wWidth, const PTCHAR OwnerUserName, Game* gameObj)
{
	WORD wIndex = 0;

	while (gameObj->PlayerPaddles[wIndex].wWidth != 0)
	{
		wIndex++;
	}

	gameObj->PlayerPaddles[wIndex].playerBlockPosition.x = wCoordX;
	gameObj->PlayerPaddles[wIndex].playerBlockPosition.y = wCoordY;

	gameObj->PlayerPaddles[wIndex].wHeight = wHeigth;
	gameObj->PlayerPaddles[wIndex].wWidth = wWidth;

	_tcscpy_s(gameObj->PlayerPaddles[wIndex].playerOwnerOfBlock.playerInfo.tcUserName,
		_countof(gameObj->PlayerPaddles[wIndex].playerOwnerOfBlock.playerInfo.tcUserName),
		OwnerUserName);
}

VOID createBlocks(WORD wCoordX, WORD wCoordY, WORD wHeigth, WORD wWidth, TypesOfBlock toBlock, Game* gameObj)
{
	WORD wIndex = gameObj->wNumberOfBlocks++;

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

VOID destroyBlock(WORD wIndex, Game* gameObj)
{
	if (wIndex >= 0 && wIndex < NUM_OF_OBJ_GAME)
	{
		ZeroMemory(&gameObj->blocks[wIndex], sizeof(Block));
	}
}

VOID destroyBonus(WORD wIndex, Game* gameObj)
{
	if (wIndex >= 0 && wIndex < NUM_OF_OBJ_GAME)
	{
		ZeroMemory(&gameObj->bonusBlock[wIndex], sizeof(BonusBlock));
	}
}

VOID destroyPlayerBlock(const PTCHAR username, WORD wNumberOfPlayers, Game* gameObj)
{

}

//TODO: Apagar com index
VOID destroyBall(WORD wIndex, Game* gameObj)
{
	ZeroMemory(&gameObj->ball, sizeof(Ball));
}

VOID catchBonus(WORD wIndex, Game* gameObj)
{
	if (wIndex >= 0 && wIndex < NUM_OF_OBJ_GAME)
	{
		switch (gameObj->blocks[wIndex].typeOfBlock)
		{
		case SpeedUp:
			break;
		case SlowDown:
			//diminiu a velocidade da bola
			break;
		case ExtraHealth:
			//adiciona vida
			incrementHealth(gameObj);
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

VOID incrementHealth(Game* gameObj)
{
	gameObj->wLifes++;
}

BOOL decrementHealth(Game* gameObj)
{
	return gameObj->wLifes > 0 ? gameObj->wLifes-- : FALSE;
}


//TODO melhor a velociadae muito grande
BOOL checkColissionBallObject(Ball* ballObject, const Coords* coordsObj2, const WORD wWidthObj2, const WORD wHeigthObj2)
{
	WORD wObjectBallRigth, wObjectBallBottom;
	WORD wObject2Rigth, wObject2Bottom;

	wObjectBallRigth = ballObject->ballPosition.x + ballObject->wWitdh;
	wObjectBallBottom = ballObject->ballPosition.y + ballObject->wHeigth;

	wObject2Rigth = coordsObj2->x + wWidthObj2;
	wObject2Bottom = coordsObj2->y + wHeigthObj2;

	if (wObjectBallRigth > coordsObj2->x &&
		ballObject->ballPosition.x < wObjectBallRigth &&
		wObjectBallBottom + ballObject->nMovementVectorY * ballObject->wVelocity> coordsObj2->y &&
		ballObject->ballPosition.y + ballObject->nMovementVectorY  * ballObject->wVelocity < wObject2Bottom)
	{
		ballObject->nMovementVectorY = ballObject->nMovementVectorY * -1;
		return TRUE;
	}

	if (wObjectBallRigth + ballObject->nMovementVectorX  * ballObject->wVelocity > coordsObj2->x &&
		ballObject->ballPosition.x + ballObject->nMovementVectorX  * ballObject->wVelocity < wObjectBallRigth &&
		wObjectBallBottom > coordsObj2->y &&
		ballObject->ballPosition.y < wObject2Bottom)
	{
		ballObject->nMovementVectorX = ballObject->nMovementVectorX * -1;
		return TRUE;
	}

	return  FALSE;

}