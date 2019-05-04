#include "ServerSyncObj.h"

HANDLE hgWriteObject = NULL;
HANDLE hgReadObject = NULL;
HANDLE hgSyncRWObject = NULL;


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

	hgSyncRWObject = CreateEvent(
		NULL,		//security attributes
		TRUE,		//manual reset
		FALSE,		//inital states as nonsignaled
		NULL);
	

	return hgReadObject == NULL || hgWriteObject == NULL || hgSyncRWObject == NULL ? FALSE : TRUE;
}

void freeSyncObject()
{
	CloseHandle(hgReadObject);
	CloseHandle(hgWriteObject);
	CloseHandle(hgSyncRWObject);
}