#include "ServerThreads.h"



DWORD WINAPI ProducerMessageThread(PVOID arg)
{
	UNREFERENCED_PARAMETER(arg);
	return 0;
}

DWORD WINAPI ConsumerMessageThread(PVOID arg)
{
	UNREFERENCED_PARAMETER(arg);
	return 0;	
}
