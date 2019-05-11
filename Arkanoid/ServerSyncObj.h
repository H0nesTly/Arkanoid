#pragma once
#include "..\Communicate\stdafx.h"
#include "..\Communicate\MessageProtocol.h"

#define NAME_INTERNAL_MUTEX TEXT("criticalSectionName")

BOOL initSyncObject();

void freeSyncObject();