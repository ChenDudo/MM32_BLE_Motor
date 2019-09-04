////////////////////////////////////////////////////////////////////////////////
#ifndef __CIRCULAR_QUEUE_H_
#define __CIRCULAR_QUEUE_H_
////////////////////////////////////////////////////////////////////////////////

//#include <stdbool.h>

//#define QUEUE_SIZE 200
#define QUEUE_SIZE 20

typedef enum{
    QUEUE_EMPTY,	//	0
    QUEUE_NORMAL,	//	1
    QUEUE_FULL		//	2
}QueueState_Def;

typedef struct {
	u16 MsgQueue[QUEUE_SIZE];
    u16 MsgQueueFront;
    u16 MsgQueueRear;
    QueueState_Def MsgQueueFlag;
}MsgQueue_Def ;

////////////////////////////////////////////////////////////////////////////////
#ifdef _M_QUEUE_C_
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL MsgQueue_Def TPADdefaultQueue;											// save key baseValue queue
GLOBAL MsgQueue_Def bigGateQueue;												// when scan larger , save current value
GLOBAL MsgQueue_Def smallGateQueue;												// when scan smaller , save current value

#undef GLOBAL
////////////////////////////////////////////////////////////////////////////////
void QueueConfig();
bool PutToQueue(u16 Msg, MsgQueue_Def * MsgSave);
bool GetFromQueue(u16 * Dat, MsgQueue_Def * MsgSave);

bool addToQueue(u16 Msg, MsgQueue_Def * MsgSave);
//bool GetQueueFullStatus(MsgQueue_Def * MsgSave);
//bool GetQueueEmptyStatus(MsgQueue_Def * MsgSave);
////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////