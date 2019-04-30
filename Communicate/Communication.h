#pragma once
#include  "stdafx.h"

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
	#endif

	__declspec(dllexport) VOID __cdecl Login();

	__declspec(dllexport) VOID __cdecl ReceiveBroadcast();

	__declspec(dllexport) VOID __cdecl SendMessageDll();

	__declspec(dllexport) VOID __cdecl ReceiveMessage();

	#ifdef __cplusplus
}
#endif