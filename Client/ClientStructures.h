#pragma once
#include "../Communicate/stdafx.h"
#include  "../Communicate/MessageProtocol.h"

#define NAME_OF_WINDOWS_PROGRAM TEXT("Cliente - Arkanoid")

typedef struct clientStructure ClientStructure;

typedef enum userMode UserMode;

struct clientStructure
{
	TCHAR tcUserName[MAX_LENGTH_NAME];
	HWND hWndWindow;
	HDC doubleBufferingDC;
	UserMode myMode;
	TCHAR tcIP[16];
};

enum userMode
{
	WaitingForGameToStart,
	PlayingGame,
	WatchingGame
};

//struct configKey
//{
//	WORD wMoveRight;
//	WORD wMoveLeft;
//};