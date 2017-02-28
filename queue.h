
#if !defined( __QUEUE_H__ )
#define __QUEUE_H__

typedef spinlock_t OS_SPIN_LOCK;
#define osFreeSpinLock(pLock) while(0)
void osAcquireSpinLock(OS_SPIN_LOCK *pLock);
void osReleaseSpinLock(OS_SPIN_LOCK *pLock);
void osAllocateSpinLock(OS_SPIN_LOCK *pLock);

//#define osAcquireSpinLock(pLock) spin_lock(pLock)
//#define osReleaseSpinLock(pLock) spin_unlock(pLock)
//#define osAllocateSpinLock(pLock) spin_lock_init(pLock)


typedef struct _TI_LIST_ENTRY {
  struct _TI_LIST_ENTRY *Flink;
  struct _TI_LIST_ENTRY *Blink;
} TI_LIST_ENTRY, *PTI_LIST_ENTRY, TQE, *PTQE;

typedef struct _TIATM_LIST_ENTRY 
{
  TI_LIST_ENTRY  Link;
} TIATM_LIST_ENTRY, *PTIATM_LIST_ENTRY;

//-------------------------------------------------------------------------
// QueueInitList -- Macro which will initialize a queue to NULL.
//-------------------------------------------------------------------------
#define QueueInitList(_L) (_L)->Link.Flink = (_L)->Link.Blink = (PTI_LIST_ENTRY)0;

//-------------------------------------------------------------------------
// QueueEmpty -- Macro which checks to see if a queue is empty.
//-------------------------------------------------------------------------
#define QueueEmpty(_L) (QueueGetHead((_L)) == (PTIATM_LIST_ENTRY)0)

//-------------------------------------------------------------------------
// QueueGetHead -- Macro which returns the head of the queue, but does not
// remove the head from the queue.
//-------------------------------------------------------------------------
#define QueueGetHead(_L) ((PTIATM_LIST_ENTRY)((_L)->Link.Flink))

#define QueueGetNext(Elem) ((PTIATM_LIST_ENTRY)((Elem)->Link.Flink))

//-------------------------------------------------------------------------
// QueuePushHead -- Macro which puts an element at the head of the queue.
//-------------------------------------------------------------------------
#define QueuePushHead(_L,_E) \
  if (!((_E)->Link.Flink = (_L)->Link.Flink)) \
  { \
    (_L)->Link.Blink = (PTI_LIST_ENTRY)(_E); \
  } \
(_L)->Link.Flink = (PTI_LIST_ENTRY)(_E);

//-------------------------------------------------------------------------
// QueueRemoveHead -- Macro which removes the head of the head of queue.
//-------------------------------------------------------------------------
#define QueueRemoveHead(_L) \
{                                                     \
  PTIATM_LIST_ENTRY ListElem;                        \
  if (ListElem = (PTIATM_LIST_ENTRY)(_L)->Link.Flink) \
  {   \
    if(!((_L)->Link.Flink = ListElem->Link.Flink)) \
      (_L)->Link.Blink = (PTI_LIST_ENTRY) 0; \
  } \
}

//-------------------------------------------------------------------------
// QueuePutTail -- Macro which puts an element at the tail (end) of the queue.
//-------------------------------------------------------------------------
#define QueuePutTail(_L,_E) \
{ \
  if ((_L)->Link.Blink) \
  { \
    ((PTIATM_LIST_ENTRY)(_L)->Link.Blink)->Link.Flink = (PTI_LIST_ENTRY)(_E); \
    (_L)->Link.Blink = (PTI_LIST_ENTRY)(_E); \
  } \
  else \
  { \
    (_L)->Link.Flink = \
    (_L)->Link.Blink = (PTI_LIST_ENTRY)(_E); \
  } \
  (_E)->Link.Flink = (PTI_LIST_ENTRY)0; \
}

//-------------------------------------------------------------------------
// QueuePutTailWithLock -- Macro which puts an element at the tail (end) of 
// the queue, using spin lock.
//-------------------------------------------------------------------------
#define QueuePutTailWithLock(_L,_E, pLock) \
{ \
  osAcquireSpinLock(pLock); \
  if ((_L)->Link.Blink) \
  { \
    ((PTIATM_LIST_ENTRY)(_L)->Link.Blink)->Link.Flink = (PTI_LIST_ENTRY)(_E); \
    (_L)->Link.Blink = (PTI_LIST_ENTRY)(_E); \
  } \
  else \
  { \
    (_L)->Link.Flink = \
    (_L)->Link.Blink = (PTI_LIST_ENTRY)(_E); \
  } \
  (_E)->Link.Flink = (PTI_LIST_ENTRY)0; \
  osReleaseSpinLock(pLock); \
}

//-------------------------------------------------------------------------
// QueueGetTail -- Macro which returns the tail of the queue, but does not
// remove the tail from the queue.
//-------------------------------------------------------------------------
#define QueueGetTail(_L) ((PTIATM_LIST_ENTRY)((_L)->Link.Blink))

//-------------------------------------------------------------------------
// QueuePopHead -- Macro which  will pop the head off of a queue (list), and
//                 return it (this differs only from queueremovehead only in 
//                 the 1st line)
//-------------------------------------------------------------------------
#define QueuePopHead(_L) \
(PTIATM_LIST_ENTRY) (_L)->Link.Flink; QueueRemoveHead(_L);

#define QueueRemoveTail(_L)  \
{                                                     \
  PTIATM_LIST_ENTRY ListElem;                        \
  ListElem = (PTIATM_LIST_ENTRY)(_L)->Link.Flink; \
  if(ListElem == (PTIATM_LIST_ENTRY)(_L)->Link.Blink) \
  {   \
    (_L)->Link.Flink = (_L)->Link.Blink = (PTI_LIST_ENTRY) 0; \
  } \
  else \
  {    \
    while(ListElem->Link.Flink != (PTI_LIST_ENTRY)(_L)->Link.Blink) \
    { \
      ListElem = (PTIATM_LIST_ENTRY)ListElem->Link.Flink; \
    } \
    (_L)->Link.Blink = (PTI_LIST_ENTRY) ListElem; \
    ListElem->Link.Flink = (PTI_LIST_ENTRY)0; \
  } \
}

#define QueueRemoveItem(_L, Elem)  \
{                                                     \
  PTIATM_LIST_ENTRY ListElem;                        \
  ListElem = (PTIATM_LIST_ENTRY)(_L)->Link.Flink; \
  if(ListElem == Elem) \
  { \
    QueueRemoveHead(_L); \
  } \
  else \
  { \
    while(ListElem) \
    { \
      if(Elem == (PTIATM_LIST_ENTRY)ListElem->Link.Flink) \
      { \
        ListElem->Link.Flink = ((PTIATM_LIST_ENTRY)Elem)->Link.Flink; \
        if(Elem == (PTIATM_LIST_ENTRY)(_L)->Link.Blink) \
          (_L)->Link.Blink = (PTI_LIST_ENTRY) 0; \
        break; \
      } \
      ListElem = (PTIATM_LIST_ENTRY)ListElem->Link.Flink; \
    }\
  } \
  ((PTIATM_LIST_ENTRY)Elem)->Link.Flink = (PTI_LIST_ENTRY) 0; \
}

#define QueuePopTail(_L)  \
((PTIATM_LIST_ENTRY)((_L)->Link.Blink)); QueueRemoveTail(_L); 

#endif
