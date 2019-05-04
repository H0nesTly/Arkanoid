#include "ServerSyncObj.h"

HANDLE hgWriteObject = NULL;
HANDLE hgReadObject = NULL;


BOOL initSyncObject()
{
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

	return hgReadObject == NULL || hgWriteObject == NULL ? FALSE : TRUE;
}

void freeSyncObject()
{
	CloseHandle(hgReadObject);
	CloseHandle(hgWriteObject);
}