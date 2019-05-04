#pragma once
#include "stdafx.h"

#define MAX_LENGTH_NAME 20
#define MESSAGE_QUEUE_SIZE 20
#define MESSAGE_QUEUE_READER_SIZE (MESSAGE_QUEUE_SIZE / 2)


#define NAME_EVENT_OBJECT_SERVER_READ TEXT("readEvent")
#define NAME_EVENT_OBJECT_SERVER_WRITE TEXT("writeEvent")
#define NAME_SHARED_MEMORY_MESSAGE TEXT("dllSharedMemMessage") //NOME da Mem Mapeada 

#define NAME_SERVER TEXT("Server")

typedef struct messageProtocolDatagram MessageProtocolDatagram;
typedef struct messageProtocolDatagramResponse MessageProtocolDatagramResponse;
typedef struct messageProtocolDatagramRequest MessageProtocolDatagramRequest;
typedef struct messageProtocolDatagram MessageProtocolDatagram;
typedef struct messageQueue MessageQueue;

typedef enum responseOfMessage TypeOfResponseMessage;
typedef enum requestOfMessage TypeOfRequestMessage;

//Estrutura da memoria partilhada "Zona de Mensagens"
struct messageProtocolDatagram
{
	//Header
	TCHAR tcSender[MAX_LENGTH_NAME];
	TCHAR tcDestination[MAX_LENGTH_NAME];

	//Body|Data
	TCHAR tcData[MAX_LENGTH_NAME];
};

struct messageProtocolDatagramResponse
{
	TypeOfResponseMessage response;
	MessageProtocolDatagram messagePD;
};

struct messageProtocolDatagramRequest
{
	TypeOfRequestMessage request;
	MessageProtocolDatagram messagePD;
};

struct messageQueue
{
	//VARIAVEIS DE INDEX da primeira queue
	WORD wFirstUnReadMessageIndex;
	WORD wLastUnReadMessageIndex;

	MessageProtocolDatagramRequest queueOfMessageClientServer[MESSAGE_QUEUE_SIZE];			//Cliente- produtor | Servidor - consumidor

	//VARIAVEIS DE INDEX da segunda queue
	WORD wFirstUnReadMessageIndexSC;
	WORD wLastUnReadMessageIndexSC;
	MessageProtocolDatagramResponse queueOfMessageServerClient[MESSAGE_QUEUE_READER_SIZE];	//Servidor - Produtor | Cliente - consumidor
};

//Estrutura da memoria partilhada "Dados do Jogo"
//struct gameDataProtocol
//{
//	Game gameInstance;
//};

enum responseOfMessage
{
	ResponseFail = -1,
	ResponseLoginFail,
	ResponseLoginSuccess
};

enum requestOfMessage
{
	LoginMessage = 0,	//User quer fazer login 
	KeyPressedMessage,	//user introduziu uma tecla 
	TopPlayersMessage,	//Foi pedido o top 10
	QuitGameMessage		//User que fazer exitgame
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