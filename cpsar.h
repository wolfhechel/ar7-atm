/*******************************************************************************
 *  TNETDxxxx Software Support
 *  Copyright (c) 2002 Texas Instruments Incorporated. All Rights Reserved.
 *
 *  FILE:   cpsar.h
 *
 *  DESCRIPTION:
 *      This file contains data structure definitions for the CPSAR HAL.
 *
 *  HISTORY:
 *      6Sep02 Greg        1.00  Original Version created.
 *      18Sept07 CPH       2.0   Added EFM support
 *****************************************************************************/
#ifndef _INC_CPSAR
#define _INC_CPSAR

#define NUM_RX_STATE_WORDS    7
#define NUM_TX_STATE_WORDS    9
#define MAX_CHAN        19


#ifndef _CPHAL_CPSAR
typedef void CPSAR_DEVICE;
#endif

/*
 * HAL Default Parameter Values
 */
#define  CFG_UNI_NNI  0

/**
 *  @ingroup shared_data
 *
 *  List of defined keys for use with Control().
 */
typedef enum
  {
   /* SAR */
   enGET_FIRMWARE,        /**< Used by the SAR to request a pointer to firmware */
   enGET_FIRMWARE_SIZE,   /**< Used by the SAR to request the size of the firmware */
   enEND=9999 /* Last entry */
  }INFO_KEY;

/*
 *  The CPHAL_FUNCTIONS struct defines the CPHAL function pointers used by upper layer
 *  software.  The upper layer software receives these pointers through the
 *  call to cphalInitModule().
 */
typedef struct
  {
  int  (*ChannelSetup)(CPSAR_DEVICE *HalDev, CHANNEL_INFO *HalCh);
  int  (*ChannelTeardown)(CPSAR_DEVICE *HalDev, int Ch, int Mode);
  int  (*Close)(CPSAR_DEVICE *HalDev, int Mode);
  int  (*Control)(CPSAR_DEVICE *HalDev, const char *Key, const char *Action, void *Value);
  int  (*Init)(CPSAR_DEVICE *HalDev);
  int  (*ModeChange)(CPSAR_DEVICE *HalDev, char *DeviceParms);
  int  (*Open)(CPSAR_DEVICE *HalDev);
  int  (*Probe)(CPSAR_DEVICE *HalDev);
  int  (*Shutdown)(CPSAR_DEVICE *HalDev);
  int  (*Tick)(CPSAR_DEVICE *HalDev);
#ifdef AR7_EFM
  int  (*SetEFMmode)(CPSAR_DEVICE *HalDev, int Mode);
  int  (*SetOSDev)(CPSAR_DEVICE *HalDev, OS_DEVICE *Os_Dev);
#endif
  } CPSAR_FUNCTIONS;

/*
 *  This is the data structure for a generic HAL device.  It contains all device
 *  specific data for a single instance of that device.  This includes Rx/Tx
 *  buffer queues, device base address, reset bit, and other information.
 */
typedef struct cpsar_device
  {
   bit32  dev_base;
   bit32  offset;
   bit32  TxTeardownPending[MAX_CHAN];
   bit32  RxTeardownPending[MAX_CHAN];
   bit32  ChIsOpen[MAX_CHAN];
   bit32  ResetBit;
   bit32  debug;
   OS_DEVICE *OsDev;
   OS_FUNCTIONS *OsFunc;
    /*void   *OsOpen;*/
   bit32  UniNni;
   bit32  Inst;
   bit32u DeviceCPID[4];
   bit32u LBSourceLLID[4];
   bit32u OamRate[11];
   CHANNEL_INFO ChData[MAX_CHAN];
   int    InitCount;
   int    OpenCount;
   char   *DeviceInfo;
   bit32u ResetBase;
   DEVICE_STATE State;
   CPSAR_FUNCTIONS *HalFuncPtr;
   int    OamMode; /* +GSG 030407 */
#ifdef AR7_EFM
   int    EFM_mode;
#endif
  }CPSARDEVICE;

extern int cpsarInitModule(CPSAR_DEVICE **HalDev,
                 OS_DEVICE *OsDev,
                 CPSAR_FUNCTIONS **HalFunc,
                 OS_FUNCTIONS *OsFunc,
                 int OsFuncSize,
                 int *HalFuncSize,
                 int Inst);

#endif
