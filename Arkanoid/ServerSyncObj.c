#include "ServerSyncObj.h"

HANDLE hgWriteObject = NULL;
HANDLE hgGameObject = NULL;
HANDLE hgNotifyClient = NULL;
HANDLE hgSyncRWObject = NULL;
HANDLE hgSyncSemaphoreRead = NULL; 
HANDLE hgMutexReadNewMessage = NULL;

HANDLE hgSemaphoreNotifyClientNewMessage = NULL;


BOOL initSyncObject()
{
	//Semaforo usado para detetar novas escritas de mensagens
	hgSyncSemaphoreRead = CreateSemaphore(
		NULL,		//security attributes
		0,			//conta inicial
		MESSAGE_QUEUE_SIZE,		//conta maxima
		NAME_SEMAPHORE_OBJECT_SERVER_READ);

	hgSemaphoreNotifyClientNewMessage = CreateSemaphore(
		NULL,
		0,
		MESSAGE_QUEUE_SIZE,
		NAME_SEMAPHORE_OBJECT_SERVER_WRITE
	);

	hgNotifyClient = CreateEvent(
		NULL,		//security attributes
		TRUE,		//manual reset
		FALSE,		//inital states as nonsignaled
		NAME_EVENT_OBJECT_SERVER_READ);

	hgWriteObject = CreateEvent(
		NULL,		//security attributes
		TRUE,		//manual reset
		FALSE,		//inital states as nonsignaled
		NAME_EVENT_OBJECT_SERVER_WRITE);

	//FIX: RETIRAR PROVALVELMENTE
	hgSyncRWObject = CreateEvent(
		NULL,		//security attributes
		TRUE,		//manual reset
		FALSE,		//inital states as nonsignaled
		NULL);
	
	hgGameObject = CreateEvent(
		NULL,		//security attributes
		TRUE,		//manual reset
		FALSE,		//inital states as nonsignaled
		NAME_EVENT_OBJECT_GAME);
	
	hgMutexReadNewMessage = CreateMutex(
		NULL, 
		FALSE, 
		NAME_MUTEX_OBJECT_CLIENT_WRITE_MESSAGE);

	return hgSemaphoreNotifyClientNewMessage == NULL ||
		hgNotifyClient == NULL ||
		hgGameObject == NULL ||
		hgWriteObject == NULL || 
		hgSyncRWObject == NULL || 
		hgSyncSemaphoreRead == NULL || 
		hgMutexReadNewMessage == NULL? FALSE : TRUE;
}

void freeSyncObject()
{
	//Devemos tb fazer release dos semaforos

	CloseHandle(hgSyncSemaphoreRead);
	CloseHandle(hgSemaphoreNotifyClientNewMessage);
	CloseHandle(hgNotifyClient);
	CloseHandle(hgWriteObject);
	CloseHandle(hgGameObject);
	CloseHandle(hgSyncRWObject);
}