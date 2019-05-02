#pragma once
#include "stdafx.h"

#define MAX_LENGTH_NAME 20
#define MESSAGE_QUEUE_SIZE 20
#define MESSAGE_QUEUE_READER_SIZE (MESSAGE_QUEUE_SIZE / 2)


#define NAME_EVENT_OBJECT_SERVER_READ TEXT("readEvent")
#define NAME_EVENT_OBJECT_SERVER_WRITE TEXT("writeEvent")
#define NAME_SHARED_MEMORY_MESSAGE TEXT("dllSharedMemMessage") //NOME da Mem Mapeada 

typedef struct messageProtocolDatagram MessageProtocolDatagram;
typedef struct messageQueue MessageQueue;

typedef enum typeOfMessage TypeOfMessage;

//Estrutura da memoria partilhada "Zona de Mensagens"
struct messageProtocolDatagram
{
	//Header
	TCHAR tcSender[MAX_LENGTH_NAME];
	TypeOfMessage typeOfMessageData;
	
	//Body|Data
	TCHAR tcData[MAX_LENGTH_NAME];
};

struct messageQueue
{
	//VARIAVEIS DE INDEX da primeira queue
	WORD wFirstUnReadMessageIndex;
	WORD wLastUnReadMessageIndex;

	MessageProtocolDatagram queueOfMessageClientServer[MESSAGE_QUEUE_SIZE];			//Cliente- produtor | Servidor - consumidor

	//VARIAVEIS DE INDEX da segunda queue
	WORD wFirstUnReadMessageIndexSC;
	WORD wLastUnReadMessageIndexSC;
	MessageProtocolDatagram queueOfMessageServerClient[MESSAGE_QUEUE_READER_SIZE];	//Servidor - Produtor | Cliente - consumidor
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


#ifdef COMMUNICATE_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
	#endif

	/*Enviamos mensagem ao servidor*/
	DLL_API VOID __cdecl Login(LPVOID  lpSharedMem, PTCHAR username);

	DLL_API VOID __cdecl ReceiveBroadcast();

	DLL_API VOID __cdecl SendMessageDll();

	DLL_API VOID __cdecl ReceiveMessage();

	#ifdef __cplusplus
}
#endif