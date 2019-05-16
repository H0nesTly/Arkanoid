#pragma once
#include "stdafx.h"
#include "DllSetup.h"

#define MAX_LENGTH_NAME 20
#define MESSAGE_QUEUE_SIZE 20
#define MESSAGE_QUEUE_READER_SIZE (MESSAGE_QUEUE_SIZE / 2)
#define TYPE_OF_MESSAGE_RESPONSE 0
#define TYPE_OF_MESSAGE_REQUEST 1


#define NAME_EVENT_OBJECT_SERVER_READ TEXT("readEvent")
#define NAME_SEMAPHORE_OBJECT_SERVER_READ TEXT("readSemaphore")
#define NAME_EVENT_OBJECT_SERVER_WRITE TEXT("writeEvent")
#define NAME_MUTEX_OBJECT_CLIENT_WRITE_MESSAGE TEXT("writeMessageMutex")
#define NAME_SHARED_MEMORY_MESSAGE TEXT("dllSharedMemMessage") //NOME da Mem Mapeada 

#define NAME_NAMED_PIPE TEXT("\\\\.\\pipe\\namedPipePath")

#define NAME_SERVER TEXT("Server")

typedef struct messageProtocolDatagram MessageProtocolDatagram;
typedef struct messageProtocolDatagramResponse MessageProtocolDatagramResponse;
typedef struct messageProtocolDatagramRequest MessageProtocolDatagramRequest;
typedef struct messageProtocolDatagram MessageProtocolDatagram;
typedef struct messageProtocolPipe MessageProtocolPipe;
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

struct messageProtocolPipe
{
	WORD wTypeOfMessage;//TYPE_OF_MESSAGE_RESPONSE  \ TYPE_OF_MESSAGE_REQUEST 
	union
	{
		TypeOfRequestMessage request;
		TypeOfResponseMessage response;
	};
	MessageProtocolDatagram messagePD;

};

// LastReadMessage
//	  v 
// [ |#|#|#| ] -> queueOfMessageClientServer
//        ^
//		  LastUnReadMessage	
struct messageQueue
{
	//VARIAVEIS DE INDEX da primeira queue
	WORD wLastUnReadMessageIndex;	//Está variável é usada no Cliente para saber onde colocar a mensagem 
	WORD wLastReadMessageIndex;		//Está variável é usada no lado do Servidor para saber que mensagens ainda não leu

	MessageProtocolDatagramRequest queueOfMessageClientServer[MESSAGE_QUEUE_SIZE];			//Cliente- produtor | Servidor - consumidor

	//VARIAVEIS DE INDEX da segunda queue
	WORD wLastUnReadMessageIndexSC; //Está variável é usada no lado do Cliente para saber que mensagens ainda não leu
	WORD wLastReadMessageIndexSC;	//Está variável é usada no Servidor para saber onde colocar a mensagem 
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
	ResponseLoginSuccess,
	ResponseTop10
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
	DLL_API VOID __cdecl Login(PTCHAR, TypeOfClientConnection);

	DLL_API VOID __cdecl ReceiveBroadcast();

	DLL_API VOID __cdecl SendMessageDll();

	DLL_API VOID __cdecl ReceiveMessage(const PTCHAR);

	#ifdef __cplusplus
}
#endif