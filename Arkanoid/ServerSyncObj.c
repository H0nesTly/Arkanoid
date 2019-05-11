#include "ServerSyncObj.h"

HANDLE hgWriteObject = NULL;
HANDLE hgNotifyClient = NULL;
HANDLE hgSyncRWObject = NULL;
HANDLE hgSyncSemaphoreRead = NULL; 
HANDLE hgMutexReadNewMessage = NULL;


BOOL initSyncObject()
{
	//Semaforo usado para detetar novas escritas de mensagens
	hgSyncSemaphoreRead = CreateSemaphore(
		NULL,		//security attributes
		0,			//conta inicial
		MESSAGE_QUEUE_SIZE,		//conta maxima
		NAME_SEMAPHORE_OBJECT_SERVER_READ);

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
	
	hgMutexReadNewMessage = CreateMutex(
		NULL, 
		FALSE, 
		NAME_MUTEX_OBJECT_CLIENT_WRITE_MESSAGE);

	return hgNotifyClient == NULL ||
		hgWriteObject == NULL || 
		hgSyncRWObject == NULL || 
		hgSyncSemaphoreRead == NULL || 
		hgMutexReadNewMessage == NULL? FALSE : TRUE;
}

void freeSyncObject()
{
	CloseHandle(hgSyncSemaphoreRead);
	CloseHandle(hgNotifyClient);
	CloseHandle(hgWriteObject);
	CloseHandle(hgSyncRWObject);
}