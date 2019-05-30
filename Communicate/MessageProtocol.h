#pragma once
#pragma warning(disable: 4201)
#include "stdafx.h"
#include "DllSetup.h"

#define MESSAGE_QUEUE_SIZE 20
#define MESSAGE_QUEUE_READER_SIZE (MESSAGE_QUEUE_SIZE / 2)
#define TYPE_OF_MESSAGE_RESPONSE 0
#define TYPE_OF_MESSAGE_REQUEST 1
#define NUM_TOP 10


#define NAME_EVENT_OBJECT_SERVER_READ TEXT("readEvent")
#define NAME_EVENT_OBJECT_GAME TEXT("GameEvent")
#define NAME_SEMAPHORE_OBJECT_SERVER_READ TEXT("readSemaphore")
#define NAME_SEMAPHORE_OBJECT_SERVER_WRITE TEXT("writeSemaphore")
#define NAME_EVENT_OBJECT_SERVER_WRITE TEXT("writeEvent")
#define NAME_MUTEX_OBJECT_CLIENT_WRITE_MESSAGE TEXT("writeMessageMutex")
#define NAME_SHARED_MEMORY_MESSAGE TEXT("dllSharedMemMessage") //NOME da Mem Mapeada 

#define NAME_NAMED_PIPE_WRITE_TO_SERVER TEXT("\\\\.\\pipe\\namedPipeWrite")
#define NAME_NAMED_PIPE_READ_FROM_SERVER TEXT("\\\\.\\pipe\\namedPipeRead")

#define NAME_SERVER TEXT("Server")

typedef struct scorePlayer ScorePlayer;
typedef struct messageProtocolDatagram MessageProtocolDatagram;
typedef struct messageProtocolDatagramResponse MessageProtocolDatagramResponse;
typedef struct messageProtocolDatagramRequest MessageProtocolDatagramRequest;
typedef struct messageProtocolDatagram MessageProtocolDatagram;
typedef struct messageProtocolPipe MessageProtocolPipe;
typedef struct messageQueue MessageQueue;
typedef struct circularBuffer CircularBuffer;

typedef enum responseOfMessage TypeOfResponseMessage;
typedef enum requestOfMessage TypeOfRequestMessage;

struct scorePlayer
{
	TCHAR jogador[MAX_LENGTH_NAME];
	double pontuacao;
};

//Estrutura da memoria partilhada "Zona de Mensagens"
struct messageProtocolDatagram
{
	//Header
	union
	{
		TypeOfRequestMessage request;
		TypeOfResponseMessage response;
	};

	TCHAR tcSender[MAX_LENGTH_NAME];
	TCHAR tcDestination[MAX_LENGTH_NAME];

	//Body|Data
	union
	{
		TCHAR tcData[MAX_LENGTH_NAME];
		ScorePlayer listOfHighScores[NUM_TOP];
	};
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

struct circularBuffer
{
	//VARIAVEIS DE INDEX da primeira queue
	WORD wHeadIndex;		//Está variável é usada no Cliente para saber onde colocar a mensagem 
	WORD wTailIndex;		//Está variável é usada no lado do Servidor para saber que mensagens ainda não leu
	BOOL bBufferFull;

	MessageProtocolDatagram queueOfMessage[MESSAGE_QUEUE_SIZE];
};

// TailIndex
//	  v 
// [ |#|#|#| ] -> queueOfMessages
//        ^
//		  HeadIndex	
struct messageQueue
{
	CircularBuffer circularBufferClientServer; //Cliente - produtor | Servidor - consumidor

	CircularBuffer circularBufferServerClient; //Servidor - Produtor | Cliente - consumidor

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
	DLL_API VOID __cdecl Login(const PTCHAR, HWND, TypeOfClientConnection);

	DLL_API VOID __cdecl ReceiveBroadcast(BOOL*);

	DLL_API VOID __cdecl SendMessageDll(BOOL*, TypeOfRequestMessage);

	DLL_API VOID __cdecl ReceiveMessage(const PTCHAR, BOOL*);

	VOID writeMessageToServerSharedMemory(MessageQueue*, TypeOfRequestMessage, const PTCHAR, const PTCHAR);

	VOID writeMessageToServerPipeRequest(MessageProtocolPipe*, TypeOfRequestMessage, const PTCHAR pSender, const PTCHAR);

	#ifdef __cplusplus
}
#endif

VOID fillMessageToProtocolDatagram(MessageProtocolDatagram*, const PTCHAR, const PTCHAR);