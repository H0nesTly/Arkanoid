#pragma once
#include <windows.h>
#include  "../Communicate/MessageProtocol.h"


struct clientStructure
{
	TCHAR tcUserName[MAX_LENGTH_NAME];
};

struct configKey
{
	WORD wMoveRight;
	WORD wMoveLeft;
};