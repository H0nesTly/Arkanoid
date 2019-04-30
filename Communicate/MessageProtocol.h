#pragma once
#include "stdafx.h"

#define MAX_LENGTH_NAME 20
#define MESSAGE_QUEUE_SIZE 20
#define MESSAGE_QUEUE_SERVER_READER_SIZE (MESSAGE_QUEUE_SIZE / 2)

#define NAME_SHARED_MEMORY_MESSAGE TEXT("dllSharedMemMessage")
#define NAME_SERVER_USERNAME TEXT("serverUN")

typedef struct messageProtocolDatagram MessageProtocolDatagram;

typedef enum typeOfMessage TypeOfMessage;

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

enum typeOfMessage
{
	LoginMessage ,KeyPressedMessage, TopPlayersMessage, QuitGameMessage
};

enum typeOf