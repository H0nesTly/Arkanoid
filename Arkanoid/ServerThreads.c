#include "ServerThreads.h"



DWORD WINAPI ProducerMessageThread(PVOID arg)
{
	UNREFERENCED_PARAMETER(arg);
	return 0;
}

DWORD WINAPI ConsumerMessageThread(PVOID arg)
{
	UNREFERENCED_PARAMETER(arg);
	while (1)
	{
		switch (WaitForSingleObject(hgReadObject, INFINITE))
		{
		case WAIT_OBJECT_0: 
			_tprintf(TEXT("QUEREM ESCREVER \N"));
			break;
		default:
			_tprintf(TEXT("Erro \N"));
			break;
		}
	}
	return 0;	
}
