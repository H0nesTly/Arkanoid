#pragma once
#include "stdafx.h"

#define MAX_LENGTH_NAME 20
#define NAME_SHARED_MEMORY_MESSAGE TEXT("dllSharedMemMessage")
#define MESSAGE_BUFFER_SIZE 20

typedef enum typeOfMessage TypeOfMessage;
typedef struct messageProtocolDatagram MessageProtocolDatagram;

enum typeOfMessage
{
	LoginMessage ,KeyPressedMessage, TopPlayersMessage, QuitGameMessage
};


//Estrutura da memoria partilhada "Zona de Mensagens"
struct messageProtocolDatagram
{
	//Header
	TCHAR tcReceiver[MAX_LENGTH_NAME];
	TCHAR tcSender[MAX_LENGTH_NAME];
	TypeOfMessage typeOfMessageData;
	
	//Body|Data
	TCHAR tcData[MAX_LENGTH_NAME];
};

//Estrutura da memoria partilhada "Dados do Jogo"
//struct gameDataProtocol
//{
//	Game gameInstance;
//};