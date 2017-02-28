/**@file************************************************************************
 *  TNETDxxxx Software Support
 *  Copyright (c) 2002 Texas Instruments Incorporated. All Rights Reserved.
 *
 *  FILE:   aal5sar.h
 *
 *  DESCRIPTION:
 *      This file contains data structure definitions for the AAL5 HAL SAR.
 *
 *  HISTORY:
 *      28Feb02 Greg       1.00  Original Version created.
 *      06Mar02 Greg       1.01  Documented structures.
 *      18Jul02 Greg       1.02  Major reorganization
 *
 *****************************************************************************/
#ifndef _INC_AAL5SAR
#define _INC_AAL5SAR

/** \namespace AAL5_Version
This documents version 01.06.06 of the AAL5 CPHAL.
*/
const char *pszVersion_CPAAL5="CPAAL5 01.06.06 "__DATE__" "__TIME__;

#include "cpsar_cpaal5.h"

#define NUM_AAL5_CHAN   16
#define MAX_AAL5_CHAN   15
#define MAX_QUEUE       2
#define MAX_DIRECTION   2

#define PKT_TYPE_AAL5   0   /* +GSG 030508 */
#define PKT_TYPE_NULL   1   /* +GSG 030508 */
#define PKT_TYPE_OAM    2   /* +GSG 030508 */
#define PKT_TYPE_TRANS  3   /* +GSG 030508 */
#define ATM_HEADER_SIZE 4   /* +GSG 030508 */

/*
 * HAL Default Parameter Values
 */
#define CFG_TX_NUM_BUFS   {256,256,256,256,256,256,256,256, 256,256,256,256,256,256,256,256}
#define CFG_RX_NUM_BUFS   {256,256,256,256,256,256,256,256, 256,256,256,256,256,256,256,256}
#define CFG_RX_BUF_SIZE   {1518,1518,1518,1518,1518,1518,1518,1518, 1518,1518,1518,1518,1518,1518,1518,1518}
#define CFG_RX_BUF_OFFSET {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
#define CFG_TX_NUM_QUEUES {1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1}
#define CFG_CPCS_UU       {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
#define CFG_DA_MASK       {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
#define CFG_PRIORITY      {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
#define CFG_PKT_TYPE      {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
#define CFG_VCI           {100,101,102,103,104,105,106,107, 108,109,110,111,112,113,114,115}
#define CFG_VPI           {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
#define CFG_CELL_RATE     {0x30d4,0x30d4,0x30d4,0x30d4,0x30d4,0x30d4,0x30d4,0x30d4, 0x30d4,0x30d4,0x30d4,0x30d4,0x30d4,0x30d4,0x30d4,0x30d4}
#define CFG_QOS_TYPE      {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
#define CFG_MBS           {8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8}
#define CFG_PCR           {1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1}
#define CFG_GFC           {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
#define CFG_CLP           {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
#define CFG_PTI           {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0}
#define CFG_RX_MAX_SERVICE {170,170,170,170,170,170,170,170, 170,170,170,170,170,170,170,170}
#define CFG_TX_MAX_SERVICE {170,170,170,170,170,170,170,170, 170,170,170,170,170,170,170,170}

static int cfg_tx_num_bufs[NUM_AAL5_CHAN]   = CFG_TX_NUM_BUFS;
static int cfg_rx_num_bufs[NUM_AAL5_CHAN]   = CFG_RX_NUM_BUFS;
static int cfg_rx_buf_size[NUM_AAL5_CHAN]   = CFG_RX_BUF_SIZE;
static int cfg_rx_buf_offset[NUM_AAL5_CHAN] = CFG_RX_BUF_OFFSET;
static int cfg_tx_num_queues[NUM_AAL5_CHAN] = CFG_TX_NUM_QUEUES;
static bit32u cfg_cpcs_uu[NUM_AAL5_CHAN]    = CFG_CPCS_UU;
static int cfg_da_mask[NUM_AAL5_CHAN]       = CFG_DA_MASK;
static int cfg_priority[NUM_AAL5_CHAN]      = CFG_PRIORITY;
static int cfg_pkt_type[NUM_AAL5_CHAN]      = CFG_PKT_TYPE;
static int cfg_vci[NUM_AAL5_CHAN]           = CFG_VCI;
static int cfg_vpi[NUM_AAL5_CHAN]           = CFG_VPI;
static bit32u cfg_cell_rate[NUM_AAL5_CHAN]  = CFG_CELL_RATE;
static int cfg_qos_type[NUM_AAL5_CHAN]      = CFG_QOS_TYPE;
static int cfg_mbs[NUM_AAL5_CHAN]           = CFG_MBS;
static int cfg_pcr[NUM_AAL5_CHAN]           = CFG_PCR;
static int cfg_gfc[NUM_AAL5_CHAN]           = CFG_GFC;
static int cfg_clp[NUM_AAL5_CHAN]           = CFG_CLP;
static int cfg_pti[NUM_AAL5_CHAN]           = CFG_PTI;
static int cfg_rx_max_service[NUM_AAL5_CHAN]= CFG_RX_MAX_SERVICE;
static int cfg_tx_max_service[NUM_AAL5_CHAN]= CFG_TX_MAX_SERVICE;
static char *channel_names[]   = CHANNEL_NAMES;

/*
 *  The HAL_FUNCTIONS struct defines the function pointers for all HAL functions
 *  accessible to upper layer software.  It is populated by calling
 *  halInitModules().
 *
 *  Note that this list is still under definition.
 */

/*
 *  This is the data structure for a transmit buffer descriptor.  The first
 *  four 32-bit words of the BD represent the CPPI 3.0 defined buffer descriptor
 *  words.  The other words are SAR/HAL implementation specific.
 */
typedef struct
  {
   bit32 HNext;      /**< Hardware's pointer to next buffer descriptor */
   bit32 BufPtr;     /**< Pointer to the data buffer */
   bit32 Off_BLen;   /**< Contains buffer offset and buffer length */
   bit32 mode;       /**< SOP, EOP, Ownership, EOQ, Teardown Complete bits */
   bit32 AtmHeader;  /**< Atm Header to be used for each fragment */
   bit32 Word5;      /**< General control information for the packet */
   bit32 Res6;
   bit32 Res7;
   void *Next;
   void *OsInfo;
#ifdef __CPHAL_DEBUG
   bit32 DbgSop;
   bit32 DbgData;
   bit32 DbgFraglist;
#endif
   void *Eop;
  }HAL_TCB;

/*
 *  This is the data structure for a receive buffer descriptor.  The first
 *  six 32-bit words of the BD represent the CPPI 3.0 defined buffer descriptor
 *  words.  The other words are HAL implementation specific.
 */
typedef volatile struct hal_private
  {
  bit32 HNext;      /**< Hardware's pointer to next buffer descriptor */
  bit32 BufPtr;     /**< Pointer to the data buffer */
  bit32 Off_BLen;   /**< Contains buffer offset and buffer length */
  bit32 mode;       /**< SOP, EOP, Ownership, EOQ, Teardown, Q Starv, Length */
  bit32 AtmHeader;
  bit32 UuCpi;
  bit32 Res6;
  bit32 Res7;
  void *DatPtr;
  void *Next;
  void *OsInfo;
  void *Eop;
  bit32 FragCount;
  bit32 Ch;
  HAL_DEVICE *HalDev;
  }HAL_RCB;


#define MAX_NEEDS 512                                             /*MJH+030409*/
/*
 *  This is the data structure for a generic HAL device.  It contains all device
 *  specific data for a single instance of that device.  This includes Rx/Tx
 *  buffer queues, device base address, reset bit, and other information.
 */
typedef struct hal_device
  {
  HAL_RCB *RcbPool[NUM_AAL5_CHAN];
  bit32u rxbufseq;
  bit32 RxActQueueCount[NUM_AAL5_CHAN];
  HAL_RCB *RxActQueueHead[NUM_AAL5_CHAN];
  HAL_RCB *RxActQueueTail[NUM_AAL5_CHAN];
  bit32 RxActive[NUM_AAL5_CHAN];
  bit32 dev_base;
  HAL_TCB *TcbPool[NUM_AAL5_CHAN][MAX_QUEUE];
  bit32 offset;
  bit32 TxActQueueCount[NUM_AAL5_CHAN][MAX_QUEUE];
  HAL_TCB *TxActQueueHead[NUM_AAL5_CHAN][MAX_QUEUE];
  HAL_TCB *TxActQueueTail[NUM_AAL5_CHAN][MAX_QUEUE];
  bit32 TxActive[NUM_AAL5_CHAN][MAX_QUEUE];
  bit32 TxTeardownPending[NUM_AAL5_CHAN];
  bit32 RxTeardownPending[NUM_AAL5_CHAN];
  bit32 ChIsOpen[NUM_AAL5_CHAN][MAX_DIRECTION];
  bit32 ChIsSetup[NUM_AAL5_CHAN][MAX_DIRECTION];
  bit32 interrupt;
  bit32 debug;
  OS_DEVICE *OsDev;
  OS_FUNCTIONS *OsFunc;
  CPSAR_FUNCTIONS *SarFunc;
  CPSAR_DEVICE *SarDev;
  /*void *OsOpen;*/
    /*FRAGLIST fraglist[MAX_FRAG];*/
  FRAGLIST *fraglist;
  char *TcbStart[NUM_AAL5_CHAN][MAX_QUEUE];
  char *RcbStart[NUM_AAL5_CHAN];
    /*bit32 RcbSize[NUM_AAL5_CHAN];*/
  bit32 InRxInt[NUM_AAL5_CHAN];
  STAT_INFO Stats;
  bit32  Inst;
  bit32u DeviceCPID[4];
  bit32u LBSourceLLID[4];
  CHANNEL_INFO ChData[NUM_AAL5_CHAN];
  DEVICE_STATE State;
  char *DeviceInfo;
  HAL_FUNCTIONS *HalFuncPtr;
  int NextRxCh;
  int NextTxCh[2];
  int StrictPriority; /* +GSG 030304 */
  bit32u NeedsCount;                                              /*MJH+030409*/
  HAL_RECEIVEINFO *Needs[MAX_NEEDS];                              /*MJH+030409*/
  bit32u SarFrequency;  /* +GSG 030416 */
  int MaxFrags;
  bit32u TurboDslErrors;
  bit32u OamLbTimeout;
  }HALDEVICE;

#endif
