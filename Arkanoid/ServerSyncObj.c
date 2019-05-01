#include "ServerSyncObj.h"

BOOL initSyncObject(ServerSyncObjects sSyncObj)
{
	sSyncObj.hReadObject = CreateEvent(
		NULL,
		TRUE,
		FALSE, 
		NAME_EVENT_OBJECT_SERVER_READ);

	sSyncObj.hWriteObject = CreateEvent(
		NULL,
		TRUE,
		FALSE, 
		NAME_EVENT_OBJECT_SERVER_WRITE);

	return sSyncObj.hReadObject == NULL || sSyncObj.hWriteObject == NULL ? FALSE : TRUE;
}

void freeSyncObject(ServerSyncObjects sSyncObj)
{
	CloseHandle(sSyncObj.hReadObject);
	CloseHandle(sSyncObj.hWriteObject);
}