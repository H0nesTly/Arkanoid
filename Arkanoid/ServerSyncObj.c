#include "ServerSyncObj.h"

HANDLE hgWriteObject = NULL;
HANDLE hgReadObject = NULL;


BOOL initSyncObject()
{
	hgReadObject = CreateEvent(
		NULL,
		TRUE,
		FALSE, 
		NAME_EVENT_OBJECT_SERVER_READ);

	hgWriteObject = CreateEvent(
		NULL,
		TRUE,
		FALSE, 
		NAME_EVENT_OBJECT_SERVER_WRITE);

	return hgReadObject == NULL || hgWriteObject == NULL ? FALSE : TRUE;
}

void freeSyncObject()
{
	CloseHandle(hgReadObject);
	CloseHandle(hgWriteObject);
}