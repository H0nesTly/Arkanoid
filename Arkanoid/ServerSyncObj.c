#include "ServerSyncObj.h"

HANDLE hgWriteObject = NULL;
HANDLE hgReadObject = NULL;
HANDLE hgSyncRWObject = NULL;
HANDLE hgSyncSemaphoreRead = NULL;


BOOL initSyncObject()
{
	//Semaforo usado para detetar novas escritas de mensagens
	hgSyncSemaphoreRead = CreateSemaphore(
		NULL,		//security attributes
		0,		//conta inicial
		MESSAGE_QUEUE_SIZE,		//conta maxima
		NAME_SEMAPHORE_OBJECT_SERVER_READ);

	hgReadObject = CreateEvent(
		NULL,		//security attributes
		TRUE,		//manual reset
		FALSE,		//inital states as nonsignaled
		NAME_EVENT_OBJECT_SERVER_READ);

	hgWriteObject = CreateEvent(
		NULL,		//security attributes
		TRUE,		//manual reset
		FALSE,		//inital states as nonsignaled
		NAME_EVENT_OBJECT_SERVER_WRITE);

	hgSyncRWObject = CreateEvent(
		NULL,		//security attributes
		TRUE,		//manual reset
		FALSE,		//inital states as nonsignaled
		NULL);
	

	return hgReadObject == NULL || hgWriteObject == NULL || hgSyncRWObject == NULL || hgSyncSemaphoreRead == NULL? FALSE : TRUE;
}

void freeSyncObject()
{
	CloseHandle(hgSyncSemaphoreRead);
	CloseHandle(hgReadObject);
	CloseHandle(hgWriteObject);
	CloseHandle(hgSyncRWObject);
}