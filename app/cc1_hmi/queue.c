////////////////////////////////////////////////////////////////////////////////
#define _M_QUEUE_C_
////////////////////////////////////////////////////////////////////////////////

#include "types.h"
#include <stdint.h>
#include <string.h>

#include "queue.h"


////////////////////////////////////////////////////////////////////////////////
void queueInit(MsgQueue_Def * MsgSave)
{
    memset(MsgSave->MsgQueue, 0, sizeof(MsgSave->MsgQueue));

    MsgSave->MsgQueueFront = 0;
    MsgSave->MsgQueueRear = 0;
    MsgSave->MsgQueueFlag = QUEUE_EMPTY;
}

////////////////////////////////////////////////////////////////////////////////
void QueueConfig()
{
	queueInit(&TPADdefaultQueue);
	queueInit(&bigGateQueue);
	queueInit(&smallGateQueue);
}

////////////////////////////////////////////////////////////////////////////////
bool PutToQueue(u16 Msg, MsgQueue_Def* MsgSave)
{
    if(MsgSave->MsgQueueFlag == QUEUE_FULL){
        return false;
    }
    MsgSave->MsgQueue[MsgSave->MsgQueueRear] = Msg;
    MsgSave->MsgQueueRear++;

    if(MsgSave->MsgQueueRear >= QUEUE_SIZE){
        MsgSave->MsgQueueRear = 0;
    }

    if(MsgSave->MsgQueueRear == MsgSave->MsgQueueFront){
        MsgSave->MsgQueueFlag = QUEUE_FULL;
    }
    else{
        MsgSave->MsgQueueFlag = QUEUE_NORMAL;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool GetFromQueue(u16 * Dat, MsgQueue_Def * MsgSave)
{
    if(MsgSave->MsgQueueFlag == QUEUE_EMPTY){
        return false;
    }
    *Dat = MsgSave -> MsgQueue[MsgSave->MsgQueueFront];
    MsgSave->MsgQueueFront++;

    if(MsgSave->MsgQueueFront >= QUEUE_SIZE){
        MsgSave->MsgQueueFront = 0;
    }

    if(MsgSave->MsgQueueFront == MsgSave->MsgQueueRear){
        MsgSave->MsgQueueFlag = QUEUE_EMPTY;
    }
    else{
        MsgSave->MsgQueueFlag = QUEUE_NORMAL;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool addToQueue(u16 Msg, MsgQueue_Def * MsgSave)
{
	bool putstatus = PutToQueue(Msg, MsgSave);
	if (!putstatus) {
		u16 temp;
		GetFromQueue(&temp, MsgSave);
		putstatus = PutToQueue(Msg, MsgSave);
	}
	return putstatus;
}
