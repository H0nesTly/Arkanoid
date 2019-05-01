#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\MessageProtocol.h"

typedef struct serverSyncObjects ServerSyncObjects;

struct serverSyncObjects
{
	HANDLE hWriteObject;
	HANDLE hReadObject;
};

BOOL initSyncObject(ServerSyncObjects);

void freeSyncObject(ServerSyncObjects);