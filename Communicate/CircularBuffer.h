#pragma once
#include "stdafx.h"
#include "MessageProtocol.h"
#include <minwinbase.h>

BOOL isEmpty(CircularBuffer * circularBufferHandler)
{
	return (circularBufferHandler->wHeadIndex == circularBufferHandler->wTailIndex);
}

/*
 * Escrevemos por cima na possiçao seguinte.
 * Caso o buffer esteja cheio o utimo valor vai ser "overwritten".
 */
VOID addItemToBuffer(CircularBuffer * circularBuffer, MessageProtocolDatagram* messageQueue)
{
	ZeroMemory(
		&circularBuffer->queueOfMessage[circularBuffer->wHeadIndex], 
		sizeof(MessageProtocolDatagram));

	CopyMemory (
		&circularBuffer->queueOfMessage[circularBuffer->wHeadIndex], //destino
		messageQueue,		//fonte
		sizeof(MessageProtocolDatagram));

	circularBuffer->wHeadIndex = (circularBuffer->wHeadIndex + 1) % MESSAGE_QUEUE_SIZE;
}

VOID removeFirstItemOfBuffer(CircularBuffer* circularBuferHandler, MessageProtocolDatagram* message)
{
	if ( ! isEmpty(circularBuferHandler))
	{
		*message = circularBuferHandler->queueOfMessage[circularBuferHandler->wTailIndex];
		circularBuferHandler->wTailIndex = (circularBuferHandler->wTailIndex + 1) % MESSAGE_QUEUE_SIZE;
	}
}