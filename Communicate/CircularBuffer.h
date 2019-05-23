#pragma once
#include "stdafx.h"
#include "MessageProtocol.h"

BOOL isEmpty(CircularBuffer * circularBufferHandler)
{
	return ((circularBufferHandler->wHeadIndex == circularBufferHandler->wTailIndex) && !circularBufferHandler->bBufferFull);
}

VOID advanceTail(CircularBuffer* circularBufferHandler)
{
	circularBufferHandler->bBufferFull = FALSE;
	circularBufferHandler->wTailIndex = (circularBufferHandler->wTailIndex + 1) % MESSAGE_QUEUE_SIZE;
}

VOID advanceHead(CircularBuffer* circularBufferHandler)
{
	if (circularBufferHandler->bBufferFull)
	{
		circularBufferHandler->wTailIndex = (circularBufferHandler->wTailIndex + 1) % MESSAGE_QUEUE_SIZE;
	}
	circularBufferHandler->wHeadIndex = (circularBufferHandler->wHeadIndex + 1) % MESSAGE_QUEUE_SIZE;
	circularBufferHandler->bBufferFull = (circularBufferHandler->wHeadIndex == circularBufferHandler->wTailIndex);
}

/*
 * Escrevemos por cima na possiçao seguinte.
 * Caso o buffer esteja cheio o utimo valor vai ser "overwritten".
 */
VOID addItemToBuffer(CircularBuffer * circularBufferHandler, MessageProtocolDatagram* messageQueue)
{
	ZeroMemory(
		&circularBufferHandler->queueOfMessage[circularBufferHandler->wHeadIndex],
		sizeof(MessageProtocolDatagram));

	CopyMemory(
		&circularBufferHandler->queueOfMessage[circularBufferHandler->wHeadIndex], //destino
		messageQueue,		//fonte
		sizeof(MessageProtocolDatagram));

	advanceHead(circularBufferHandler);
}

VOID removeFirstItemOfBuffer(CircularBuffer* circularBufferHandler, MessageProtocolDatagram* message)
{
	if (!isEmpty(circularBufferHandler))
	{
		*message = circularBufferHandler->queueOfMessage[circularBufferHandler->wTailIndex];
		advanceTail(circularBufferHandler);
	}
}
