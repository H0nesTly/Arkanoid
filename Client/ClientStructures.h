#pragma once
#include "../Communicate/stdafx.h"
#include  "../Communicate/MessageProtocol.h"

typedef struct clientStructure ClientStructure;

struct clientStructure
{
	TCHAR tcUserName[MAX_LENGTH_NAME];
};
//
//struct configKey
//{
//	WORD wMoveRight;
//	WORD wMoveLeft;
//};