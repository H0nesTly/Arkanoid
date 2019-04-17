#pragma once
#include "stdafx.h"

#define MAX_LENGTH_NAME 20

typedef enum typeOfMessage TypeOfMessage;
typedef struct messageProtocolDatagram MessageProtocolDatagram;

enum typeOfMessage
{
	KeyPressed, TopPlayers, QuitGame
};


//Estrutura da memoria partilhada "Zona de Mensagens"
struct messageProtocolDatagram
{
	//Header
	TCHAR[MAX_LENGTH_NAME] tcReceiver;
	TCHAR[MAX_LENGTH_NAME] tcSender;
	TypeOfMessage typeOfMessageData;
	
	//Body|Data
	TCHAR[MAX_LENGTH_NAME] tcData;
};

//Estrutura da memoria partilhada "Dados do Jogo"
struct gameDataProtocol
{
	
};