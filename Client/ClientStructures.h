#pragma once
#include "../Communicate/stdafx.h"
#include  "../Communicate/MessageProtocol.h"

#define NAME_OF_WINDOWS_PROGRAM TEXT("Cliente - Arkanoid")

typedef struct clientStructure ClientStructure;

struct clientStructure
{
	TCHAR tcUserName[MAX_LENGTH_NAME];
	HWND hWndWindow;
};
//
//struct configKey
//{
//	WORD wMoveRight;
//	WORD wMoveLeft;
//};