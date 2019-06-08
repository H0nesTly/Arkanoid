#include "GameLogic.h"

extern GameServerConfiguration serverConfig;

VOID moveBall(Game* gameObj)
{
	BOOL bCheckMore = TRUE;
	Ball * ballToMove;

	for (int i = 0; i < gameObj->wNumberOfBalls; i++)
	{
		ballToMove = &gameObj->ball[i];

		if ((ballToMove->ballPosition.x + (ballToMove->nMovementVectorX * ballToMove->wUnitsToMove)) < 0)
		{
			ballToMove->nMovementVectorX = ballToMove->nMovementVectorX * -1;
			ballToMove->ballPosition.x = 0 + NORMAL_SPACING;
		}
		else
		{
			if ((ballToMove->ballPosition.x + ballToMove->wWitdh + (ballToMove->nMovementVectorX * ballToMove->wUnitsToMove)) >= DEFAULT_WIDTH_OF_GAMEBOARD)
			{
				ballToMove->nMovementVectorX = ballToMove->nMovementVectorX * -1;
				ballToMove->ballPosition.x = DEFAULT_WIDTH_OF_GAMEBOARD - ballToMove->wWitdh - NORMAL_SPACING;
			}
		}


		if ((ballToMove->ballPosition.y + (ballToMove->nMovementVectorY * ballToMove->wUnitsToMove)) < 0)
		{
			ballToMove->nMovementVectorY = ballToMove->nMovementVectorY * -1;
			ballToMove->ballPosition.y = 0 + NORMAL_SPACING;
		}
		else
		{
			if (ballToMove->ballPosition.y + ballToMove->wHeight + (ballToMove->nMovementVectorX * ballToMove->wUnitsToMove) > DEFAULT_HEIGTH_LOSE_BALL)
			{
				_tprintf(TEXT("\n Vidas restantes %d"), gameObj->wLifes);
				//retorna
				destroyBall(i, gameObj);

				if (gameObj->wNumberOfBalls == 0)
				{
					decrementHealth(gameObj);
				}
			}
		}

		for (size_t i = 0; i < gameObj->wNumberOfPlayerPaddles && bCheckMore; i++)
		{
			if (checkColissionBallObject(ballToMove, &gameObj->PlayerPaddles[i].playerBlockPosition, gameObj->PlayerPaddles[i].wWidth, gameObj->PlayerPaddles[i].wHeight))
				bCheckMore = !bCheckMore;
		}

		for (size_t i = 0; i < gameObj->wNumberOfBonusDropping && bCheckMore; i++)
		{
			if (checkColissionBallObject(ballToMove, &gameObj->bonusBlock[i].bonusCoords, gameObj->bonusBlock[i].wWidth, gameObj->bonusBlock[i].wHeight))
			{
				bCheckMore = !bCheckMore;
			}
		}

		for (WORD i = 0; i < gameObj->wNumberOfBlocks && bCheckMore; i++)
		{
			//se a bola estiver debaixo não pode colidir poupamos algum tempo TODO: X Axis
			if (ballToMove->ballPosition.y + ballToMove->nMovementVectorY * ballToMove->wUnitsToMove <= gameObj->blocks[i].blockPosition.y + gameObj->blocks[i].wHeight)
			{
				if (checkColissionBallObject(ballToMove, &gameObj->blocks[i].blockPosition, gameObj->blocks[i].wWidth, gameObj->blocks[i].wHeight))
				{
					destroyBlock(i, gameObj);
					bCheckMore = !bCheckMore;
				}
			}
		}

		ballToMove->ballPosition.x += ballToMove->nMovementVectorX * ballToMove->wUnitsToMove;
		ballToMove->ballPosition.y += ballToMove->nMovementVectorY * ballToMove->wUnitsToMove;
	}



}

VOID movePaddle(const PTCHAR userName, Game* gameObj, const short nDirectionToMove)
{
	int nIndex = getPaddleOwnerByName(userName, gameObj);
	Paddle* playerPaddleTemp = NULL;
	Paddle* myPlayerPaddleTemp = NULL;

	if (nIndex >= 0)
	{
		myPlayerPaddleTemp = &gameObj->PlayerPaddles[nIndex];

		//TESTAR!!
		//verificamos se existe algum paddle ao lado
		for (size_t i = 0; i < gameObj->wNumberOfPlayerPaddles; i++)
		{
			if (i != nIndex)
			{
				playerPaddleTemp = &gameObj->PlayerPaddles[i];

				if ((myPlayerPaddleTemp->playerBlockPosition.x + myPlayerPaddleTemp->wUnitsToMove * nDirectionToMove >= playerPaddleTemp->playerBlockPosition.x &&
					myPlayerPaddleTemp->playerBlockPosition.x + myPlayerPaddleTemp->wUnitsToMove * nDirectionToMove <= playerPaddleTemp->playerBlockPosition.x + playerPaddleTemp->wWidth) ||
					(myPlayerPaddleTemp->playerBlockPosition.x + myPlayerPaddleTemp->wUnitsToMove * nDirectionToMove <= playerPaddleTemp->playerBlockPosition.x &&
						myPlayerPaddleTemp->playerBlockPosition.x + myPlayerPaddleTemp->wWidth + myPlayerPaddleTemp->wUnitsToMove * nDirectionToMove >= playerPaddleTemp->playerBlockPosition.x))
				{
					//colisão 
					return;
				}
			}
		}

		//verificamos se está nas bordas
		if ((myPlayerPaddleTemp->playerBlockPosition.x + (myPlayerPaddleTemp->wUnitsToMove * nDirectionToMove)) < 0)
		{
			myPlayerPaddleTemp->playerBlockPosition.x = 0 + NORMAL_SPACING;
			return;
		}
		else
		{
			if ((myPlayerPaddleTemp->playerBlockPosition.x + myPlayerPaddleTemp->wWidth + (myPlayerPaddleTemp->wUnitsToMove * nDirectionToMove)) >= DEFAULT_WIDTH_OF_GAMEBOARD)
			{
				myPlayerPaddleTemp->playerBlockPosition.x = DEFAULT_WIDTH_OF_GAMEBOARD - myPlayerPaddleTemp->wWidth - NORMAL_SPACING * 2;
				return;
			}
		}

		//movemos
		myPlayerPaddleTemp->playerBlockPosition.x += myPlayerPaddleTemp->wUnitsToMove * nDirectionToMove;
	}
}

VOID moveBonus(Game* gameObj, WORD wIndex)
{
	BonusBlock* bonusBlock = &gameObj->bonusBlock[wIndex];
	BOOL bKeepChecking = TRUE;

	if (bonusBlock->bonusCoords.y + bonusBlock->wHeight + bonusBlock->wDropUnits > DEFAULT_HEIGTH_LOSE_BALL)
	{
		destroyBonus(wIndex, gameObj);
		return;
	}

	for (size_t i = 0; i < gameObj->wNumberOfPlayerPaddles; i++)
	{
		if (gameObj->PlayerPaddles[i].playerBlockPosition.y <= bonusBlock->bonusCoords.y + bonusBlock->wDropUnits)
		{
			if (checkColissionBonusObject(bonusBlock, &gameObj->PlayerPaddles[i].playerBlockPosition, gameObj->PlayerPaddles[i].wWidth, gameObj->PlayerPaddles[i].wHeight))
			{
				catchBonus(wIndex, gameObj);
				return;
			}
		}
	}

	bonusBlock->bonusCoords.y += bonusBlock->wDropUnits;
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
	createBlocks(147, 61, 10, 30, Magic, gameObj);
	createBlocks(178, 61, 10, 30, Magic, gameObj);
	createBlocks(209, 61, 10, 30, Magic, gameObj);
	//+50				  
	createBlocks(259, 61, 10, 30, Magic, gameObj);
	createBlocks(290, 61, 10, 30, Magic, gameObj);
	createBlocks(321, 61, 10, 30, Magic, gameObj);

	createBall((rand() % (DEFAULT_WIDTH_OF_GAMEBOARD)), DEFAULT_HEIGTH_OF_GAMEBOARD / 2, gameObj);
}

VOID createGameBoard(WORD wCoordX, WORD wCoordY, WORD wHeight, WORD wWidth, GameBoard* gameObj)
{
	gameObj->gameBoardPosition.x = wCoordX;
	gameObj->gameBoardPosition.y = wCoordY;

	gameObj->wHeight = wHeight;
	gameObj->wWidth = wWidth;
}

VOID createBall(WORD wCoordX, WORD wCoordY, Game* gameObj)
{
	WORD wIndex = gameObj->wNumberOfBalls++;

	gameObj->ball[wIndex].ballPosition.x = wCoordX;
	gameObj->ball[wIndex].ballPosition.y = wCoordY;

	gameObj->ball[wIndex].wHeight = gameObj->ball[wIndex].wWitdh = 8;

	if ((rand() % 2) == 1)
		gameObj->ball[wIndex].nMovementVectorX = 1;
	else
		gameObj->ball[wIndex].nMovementVectorX = -1;

	gameObj->ball[wIndex].nMovementVectorY = -1;

	setBallVelocity(&gameObj->ball[wIndex], serverConfig.wVelocityBall);

	gameObj->ball[wIndex].wUnitsToMove = serverConfig.wVelocityBall;
}

VOID createPlayerPaddle(const PTCHAR OwnerUserName, Game* gameObj)
{
	WORD wIndex = gameObj->wNumberOfPlayerPaddles++;
	WORD wWidthOfPaddle = generateWidthOfPlayePaddle(gameObj);

	gameObj->PlayerPaddles[wIndex].playerBlockPosition.x = 1 + wWidthOfPaddle * gameObj->wNumberOfPlayerPaddles;
	gameObj->PlayerPaddles[wIndex].playerBlockPosition.y = DEFAULT_POS_Y_PLAYER_PADDLE;

	gameObj->PlayerPaddles[wIndex].wHeight = HEIGTH_OF_PLAYER_PADDLE;
	gameObj->PlayerPaddles[wIndex].wWidth = wWidthOfPaddle;

	gameObj->PlayerPaddles[wIndex].wUnitsToMove = DEFAULT_PADDLE_VELOCITY;

	_tcscpy_s(gameObj->PlayerPaddles[wIndex].playerOwnerOfBlock.playerInfo.tcUserName,
		_countof(gameObj->PlayerPaddles[wIndex].playerOwnerOfBlock.playerInfo.tcUserName),
		OwnerUserName);
}

VOID createBlocks(WORD wCoordX, WORD wCoordY, WORD wHeight, WORD wWidth, TypesOfBlock toBlock, Game* gameObj)
{
	WORD wIndex = gameObj->wNumberOfBlocks++;

	gameObj->blocks[wIndex].blockPosition.x = wCoordX;
	gameObj->blocks[wIndex].blockPosition.y = wCoordY;

	gameObj->blocks[wIndex].wWidth = wWidth;
	gameObj->blocks[wIndex].wHeight = wHeight;

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

VOID createBonus(WORD wCoordX, WORD wCoordY, WORD wHeight, WORD wWidth, TypeOfBonus toBonus, Game* gameObj)
{
	WORD wIndex = gameObj->wNumberOfBonusDropping++;

	gameObj->bonusBlock[wIndex].bonusCoords.x = wCoordX;
	gameObj->bonusBlock[wIndex].bonusCoords.y = wCoordY;

	gameObj->bonusBlock[wIndex].wWidth = wWidth;
	gameObj->bonusBlock[wIndex].wHeight = wHeight;

	gameObj->bonusBlock[wIndex].wDropUnits = 3;

	gameObj->bonusBlock[wIndex].typeOfBonus = toBonus;
}

VOID destroyBlock(WORD wIndex, Game* gameObj)
{
	double valueGenerate, lastProb = 0;
	if (wIndex >= 0 && wIndex < NUM_OF_BLOCK_OBJ_GAME)
	{
		switch (gameObj->blocks[wIndex].typeOfBlock)
		{
		case Magic:
			//VALOR SEMPRE MUITO PARECIDOS
			for (int i = 0; i < 2; i++)
				valueGenerate = ((double)rand()) / RAND_MAX;

			_tprintf(TEXT("\n Valor generado %.2f"), valueGenerate);

			lastProb += serverConfig.probTriple;

			if (valueGenerate <= lastProb)
			{
				createBonus(gameObj->blocks[wIndex].blockPosition.x,
					gameObj->blocks[wIndex].blockPosition.y,
					16,
					16,
					Triple,
					gameObj);
			}
			else
			{
				lastProb += serverConfig.probSlowDowns;
				if (valueGenerate <= lastProb)
				{
					createBonus(gameObj->blocks[wIndex].blockPosition.x,
						gameObj->blocks[wIndex].blockPosition.y,
						16,
						16,
						SlowDown,
						gameObj);
				}
				else
				{
					lastProb += serverConfig.probSpeedUp;
					if (valueGenerate <= lastProb)
					{
						createBonus(gameObj->blocks[wIndex].blockPosition.x,
							gameObj->blocks[wIndex].blockPosition.y,
							16,
							16,
							SpeedUp,
							gameObj);
					}
					else
					{
						createBonus(gameObj->blocks[wIndex].blockPosition.x,
							gameObj->blocks[wIndex].blockPosition.y,
							16,
							16,
							ExtraHealth,
							gameObj);
					}
				}
			}

			break;
		case Normal:
			break;
		case  Rigid:
			if (gameObj->blocks[wIndex].wNumberOfColisions > 1)
			{
				gameObj->blocks[wIndex].wNumberOfColisions--;
				return;
			}
			break;
		default:
			break;
		}

		ZeroMemory(&gameObj->blocks[wIndex], sizeof(Block));

		for (size_t i = wIndex; i + 1 < gameObj->wNumberOfBlocks; i++)
		{
			CopyMemory(&gameObj->blocks[i],
				&gameObj->blocks[i + 1],
				sizeof(Block));

			ZeroMemory(&gameObj->blocks[i + 1], sizeof(Block));
		}
		gameObj->wNumberOfBlocks--;
		gameObj->dwScore += 10;
	}
}

VOID destroyBonus(WORD wIndex, Game* gameObj)
{
	if (wIndex >= 0 && wIndex < NUM_OF_OBJ_GAME)
	{
		ZeroMemory(&gameObj->bonusBlock[wIndex], sizeof(BonusBlock));

		for (size_t i = wIndex; i + 1 < gameObj->wNumberOfBonusDropping; i++)
		{
			CopyMemory(&gameObj->bonusBlock[i],
				&gameObj->bonusBlock[i + 1],
				sizeof(BonusBlock));

			ZeroMemory(&gameObj->bonusBlock[i + 1], sizeof(BonusBlock));
		}
		gameObj->wNumberOfBonusDropping--;
	}
}

VOID destroyPlayerPaddle(const PTCHAR username, Game* gameObj)
{
	for (size_t i = 0; i < gameObj->wNumberOfPlayerPaddles; i++)
	{
		if (_tcscmp(username, gameObj->PlayerPaddles->playerOwnerOfBlock.playerInfo.tcUserName) == 0)
		{
			ZeroMemory(&gameObj->PlayerPaddles[i], sizeof(Paddle));
			for (size_t j = i; j + 1 < gameObj->wNumberOfPlayerPaddles; j++)
			{
				CopyMemory(&gameObj->PlayerPaddles[j],
					&gameObj->PlayerPaddles[j + 1],
					sizeof(Paddle));

				ZeroMemory(&gameObj->PlayerPaddles[j + 1], sizeof(Paddle));
			}
			gameObj->wNumberOfPlayerPaddles--;
			break;
		}
	}
}

VOID destroyBall(WORD wIndex, Game* gameObj)
{
	if (wIndex >= 0 && wIndex < NUM_OF_OBJ_GAME)
	{
		ZeroMemory(&gameObj->ball[wIndex], sizeof(Ball));

		for (size_t i = wIndex; i + 1 < gameObj->wNumberOfBalls; i++)
		{
			CopyMemory(&gameObj->ball[i],
				&gameObj->ball[i + 1],
				sizeof(Ball));

			ZeroMemory(&gameObj->ball[i + 1], sizeof(Ball));
		}

		if (gameObj->wNumberOfBalls > 0)
			gameObj->wNumberOfBalls--;
		else
			gameObj->wNumberOfBalls = 0;
	}
}

VOID catchBonus(WORD wIndex, Game* gameObj)
{
	WORD auxX;
	if (wIndex >= 0 && wIndex < NUM_OF_OBJ_GAME)
	{
		switch (gameObj->bonusBlock[wIndex].typeOfBonus)
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
			for (int i = 0; i < 2; i++)
			{
				auxX = (rand() % (DEFAULT_WIDTH_OF_GAMEBOARD));
				createBall(auxX, DEFAULT_HEIGTH_OF_GAMEBOARD / 2, gameObj);
			}
			break;
			//Nao faz nada
		case None:
		default:
			break;
		}
		destroyBonus(wIndex, gameObj);
	}
}

VOID setBallVelocity(Ball* balleObj, WORD wVelocity)
{
	if (wVelocity == 0)
	{
		wVelocity = DEFAULT_BALL_VELOCITY;
	}
	balleObj->wVelocity = wVelocity;
}

VOID setHealth(Game* gameObj, WORD wLifes)
{
	if (wLifes == 0)
	{
		wLifes = DEFAULT_HEALTH;
	}

	gameObj->wLifes = wLifes;
}

VOID incrementHealth(Game* gameObj)
{
	gameObj->wLifes += 1;
}

BOOL decrementHealth(Game* gameObj)
{
	return gameObj->wLifes > 0 ? gameObj->wLifes-- : FALSE;
}

BOOL checkColissionBallObject(Ball* ballObject, const Coords* coordsObj2, const WORD wWidthObj2, const WORD wHeightObj2)
{
	WORD wObjectBallRigth, wObjectBallBottom;
	WORD wObject2Rigth, wObject2Bottom;

	wObjectBallRigth = ballObject->ballPosition.x + ballObject->wWitdh;
	wObjectBallBottom = ballObject->ballPosition.y + ballObject->wHeight;

	wObject2Rigth = coordsObj2->x + wWidthObj2;
	wObject2Bottom = coordsObj2->y + wHeightObj2;

	//Se o vetor X estive a andar colide com alguem?
	if (ballObject->ballPosition.x + (ballObject->nMovementVectorX * ballObject->wUnitsToMove) < wObject2Rigth &&
		wObjectBallRigth + (ballObject->nMovementVectorX * ballObject->wUnitsToMove) > coordsObj2->x && //lados 
		ballObject->ballPosition.y < wObject2Bottom  &&
		wObjectBallBottom > coordsObj2->y)
	{
		ballObject->nMovementVectorX = ballObject->nMovementVectorX * -1;
		return TRUE;
	}

	//Se o vetor y estive a andar colide com alguem?
	if (ballObject->ballPosition.y + (ballObject->nMovementVectorY  * ballObject->wUnitsToMove) < wObject2Bottom &&
		wObjectBallBottom + (ballObject->nMovementVectorY * ballObject->wUnitsToMove) > coordsObj2->y &&
		ballObject->ballPosition.x < wObject2Rigth &&
		wObjectBallRigth > coordsObj2->x)
	{
		ballObject->nMovementVectorY = ballObject->nMovementVectorY * -1;
		return TRUE;
	}

	return  FALSE;
}

BOOL checkColissionBonusObject(const BonusBlock* bonusObject, const Coords* coordsObj2, const WORD wWidthObj2, const WORD wHeightObj2)
{
	WORD wObject1Rigth, wObject1Bottom;
	WORD wObject2Rigth, wObject2Bottom;

	wObject1Rigth = bonusObject->bonusCoords.x + bonusObject->wWidth;
	wObject1Bottom = bonusObject->bonusCoords.y + bonusObject->wHeight;

	wObject2Rigth = coordsObj2->x + wWidthObj2;
	wObject2Bottom = coordsObj2->y + wHeightObj2;

	//Se o vetor X estive a andar colide com alguem?
//if (bonusObject->bonusCoords.x + (ballObject->nMovementVectorX * ballObject->wUnitsToMove) < wObject2Rigth &&
//	wObject1Rigth + (ballObject->nMovementVectorX * ballObject->wUnitsToMove) > coordsObj2->x && //lados 
//	ballObject->ballPosition.y < wObject2Bottom  &&
//	wObjectBallBottom > coordsObj2->y)
//{
//	ballObject->nMovementVectorX = ballObject->nMovementVectorX * -1;
//	return TRUE;
//}

//Se o vetor y estive a andar colide com alguem?
	if (bonusObject->bonusCoords.y + bonusObject->wDropUnits  < wObject2Bottom &&
		wObject1Bottom + bonusObject->wDropUnits > coordsObj2->y &&
		bonusObject->bonusCoords.x < wObject2Rigth &&
		wObject1Rigth > coordsObj2->x)
	{
		return TRUE;
	}

	return  FALSE;
}

WORD generateWidthOfPlayePaddle(const Game* gameObj)
{
	return DEFAULT_WIDTH_OF_PLAYER_PADDLE;
}

int getPaddleOwnerByName(const PTCHAR UserName, Game* gameObj)
{
	for (size_t i = 0; i < gameObj->wNumberOfPlayerPaddles; i++)
	{
		if (_tcscmp(UserName, gameObj->PlayerPaddles[i].playerOwnerOfBlock.playerInfo.tcUserName) == 0)
		{
			return i;
		}
	}
	return -1;
}
