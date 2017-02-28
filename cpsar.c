/**
 *  cpsar.c
 *
 *  TNETDxxxx Software Support\n
 *  Copyright (c) 2002 Texas Instruments Incorporated. All Rights Reserved.
 *
 *  This file contains the HAL for the CPSAR module.  In the software
 *  architecture, the CPSAR module is used exclusively by the AAL5 and AAL2
 *  CPHAL modules.  AAL5 and AAL2 may utilize the same CPSAR instance
 *  simulataneously.
 *
 *  version
 *      5Sep02 Greg        1.00  Original Version created.
 *     18Sep07 CPH   			CQ11466: Added EFM support.
 */

/* register files */
#include "cp_sar_reg.h"

#ifdef AR7_EFM
#ifdef DEBUG_BUILD
#define dgprintf dprintf
extern void dprintf( int uDbgLevel, char * szFmt, ...);
#else
#define dgprintf(uDbgLevel, szFmt, args...)
#endif
#endif


#define _CPHAL_CPSAR
#define _CPHAL

#define WAIT_THRESH           200000
#define IRAM_SIZE             1536
#define MAX_INST              2

/*  OS Data Structure definition  */

typedef void OS_PRIVATE;
typedef void OS_DEVICE;
typedef void OS_SENDINFO;
typedef void OS_RECEIVEINFO;
typedef void OS_SETUP;

/*  CPHAL Data Structure definitions  */

typedef struct cpsar_device  CPSAR_DEVICE;
typedef struct cpsar_device  HAL_DEVICE;
typedef void HAL_RECEIVEINFO;

#define MAX_QUEUE     2
#define MAX_CHAN      19

#ifdef AR7_EFM
#define MAX_EFM_CHAN    0 // Note this also defined in aal5sar.h
#define NUM_EFM_QUEUE   1
#endif


#include "cpcommon_cpsar.h"
#include "cpswhal_cpsar.h"
#include "cpsar.h"
#include "cpcommon_cpsar.c"

static CPSAR_DEVICE *CpsarDev[MAX_INST]= {0,0};

#ifdef AR7_EFM

int disp_Hal_Device (char *buf, int count, CPSAR_DEVICE *pSarDev)
{
  int len = 0;
  int limit = count - 80;

  if (len <= limit)
  {
    len += sprintf (buf + len," RxBufSize=%d\n", pSarDev->ChData[0].RxBufSize);
    len += sprintf (buf + len," RxNumBuffers=%d\n", pSarDev->ChData[0].RxNumBuffers);
    len += sprintf (buf + len," RxServiceMax=%d\n", pSarDev->ChData[0].RxServiceMax);
    len += sprintf (buf + len," TxNumBuffers=%d\n", pSarDev->ChData[0].TxNumBuffers);
    len += sprintf (buf + len," TxNumQueues=%d\n", pSarDev->ChData[0].TxNumQueues);
    len += sprintf (buf + len," DaMask=%d\n", pSarDev->ChData[0].DaMask);
    len += sprintf (buf + len," Priority=%d\n", pSarDev->ChData[0].Priority);
    len += sprintf (buf + len," vpi/vci=%d/%d\n", pSarDev->ChData[0].Vpi, pSarDev->ChData[0].Vci);
  }


 return len;
}

#endif

/*
 *  Returns statistics information.
 *
 *  @param  HalDev   CPHAL module instance. (set by xxxInitModule())
 *
 *  @return 0
 */
static int StatsGet3(CPSAR_DEVICE *HalDev)
  {

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpsar]StatsGet3(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif
   /*
   dbgPrintf("CPSAR General Stats:\n");
   DispHexStat(HalDev, "Base Address",HalDev->dev_base);
   DispStat(HalDev, "Offset (VLYNQ)",HalDev->offset);
   DispStat(HalDev, "Debug Level",HalDev->debug);
   DispStat(HalDev, "Instance",HalDev->Inst);
   DispHexStat(HalDev, "Reset Address",HalDev->ResetBase);
   DispStat(HalDev, "Reset Bit",HalDev->ResetBit);
   */
   return (EC_NO_ERRORS);
  }

/* +GSG 030407 */
static void SetOamMode(HAL_DEVICE *HalDev)
  {
   int Ch;
   volatile bit32u *pTmp;
   int OamMode = (1<<8);

   /* any configuration of OamMode affects all VC's, including AAL2 */
   for (Ch = 0; Ch < MAX_CHAN; Ch++)
     {
      if (Ch < 16)
        pTmp = (pPDSP_AAL5_RX_STATE_WORD_0(HalDev->dev_base) + (Ch*64));
       else
        pTmp = (pPDSP_AAL2_RX_STATE_WORD_0(HalDev->dev_base) + ((Ch-16)*64));

      if (HalDev->OamMode == 0)
        {
         *pTmp &=~ OamMode;
        }
       else
        {
         *pTmp |= OamMode;
        }
     }
  }

static int halControl(CPSAR_DEVICE *HalDev, const char *Key, const char *Action, void *Value)
  {
   int KeyFound=0, ActionFound=0, rc=EC_NO_ERRORS, Ch;  /* +RC3.02*/
   char *TmpKey = (char *)Key;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpsar]halControl(HalDev:%08x, Key:%s, Action:%s, Value:%08x)\n", (bit32u)HalDev,
                Key, Action, (bit32u)Value);
      osfuncSioFlush();
     }
#endif

   if (HalDev->OsFunc->Strcmpi(Key, "Debug") == 0)
     {
      KeyFound=1; /* +RC3.02*/
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1; /* +RC3.02*/
         HalDev->debug = *(int *)Value;
        }
     }

   /* +GSG 030407 */
   if (HalDev->OsFunc->Strcmpi(Key, "OamMode") == 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         HalDev->OamMode = *(int *)Value;

         /* only do this if we're open */
         if (HalDev->OpenCount > 0)
           SetOamMode(HalDev);
        }

      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
         *(int *)Value = HalDev->OamMode;
        }
     }

   if (HalDev->OsFunc->Strcmpi(Key, "Stats3") == 0)
     {
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        StatsGet3(HalDev);
     }

   /* +RC3.02  (if statement) */
   /* Fixes PITS #98 */
   if (HalDev->OsFunc->Strstr(Key, "PdspEnable") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;

         /* Configure PDSP enable bit based on Value*/
         if (*(int *)Value & 1)
           {
            /* enable PDSP */
            PDSP_CTRL_REG(HalDev->dev_base) |= 0x2;
           }
          else
           {
            /* disable PDSP */
            PDSP_CTRL_REG(HalDev->dev_base) &=~ 0x2;
           }
        }
     }

   if (HalDev->OsFunc->Strstr(Key, "FwdUnkVc.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;

         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("FwdUnkVc.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* Configure forwarding of unknown VCI/VPI cells */
         SAR_PDSP_FWD_UNK_VC_REG(HalDev->dev_base) = (((*(int*)Value)<<31) | Ch);
        }
     }

   if (KeyFound == 0) /* +RC3.02 */
     rc = (EC_CPSAR|EC_FUNC_CONTROL|EC_VAL_KEY_NOT_FOUND); /* +RC3.02 */

   if (ActionFound == 0) /* +RC3.02 */
     rc = (EC_CPSAR|EC_FUNC_CONTROL|EC_VAL_ACTION_NOT_FOUND); /* +RC3.02 */

   return(rc);   /* ~RC3.02 */
  }

/*
 * This function opens the specified channel.
 *
 * @param   HalDev  CPHAL module instance. (set by cphalInitModule())
 * @param   Ch      Channel number.
 *
 * @return  0 OK, Non-zero Not OK
 */
static int halChannelSetup(CPSAR_DEVICE *HalDev, CHANNEL_INFO *HalCh)
  {
  int i;
  int Ch = HalCh->Channel;
  int PdspChBlock = Ch;
  int PdspBlockOffset = 0;
  volatile bit32u *pTmp;

#ifdef EFM_DEBUG
  printk("cpsar: halChannelSetup: pktype=%d, da_mask=%d, pri=%d\n",
    HalCh->PktType, HalCh->DaMask, HalCh->Priority);
#endif

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpsar]halChannelSetup(HalDev:%08x, HalCh:%08x)\n", (bit32u)HalDev,
                (bit32u)HalCh);
      osfuncSioFlush();
     }
#endif

  /* Figure out the correct offset from the start of the PDSP
     Scratchpad RAM (starting at 0x8050 in the SAR) */
  if (Ch > 15)
    {
     /* this is an AAL2 channel, which are channels 16-18 */
     PdspChBlock = Ch - 16;
     /* Get the offset to the AAL2 portion of the block (in words) */
     PdspBlockOffset = NUM_PDSP_AAL5_STATE_WORDS + (PdspChBlock*64);
     /* Clear PDSP State RAM */
     /*pTmp = (pPDSP_BLOCK_0(HalDev->dev_base)+PdspBlockOffset);
     for (i=0; i<NUM_PDSP_AAL2_STATE_WORDS; i++)
     *pTmp++ = 0;*/
    }
   else
    {
     /* Get the offset to the AAL5 portion of the block (in words) */
     PdspBlockOffset = (PdspChBlock*64);
     /* Clear PDSP State RAM */
     /*pTmp = (pPDSP_BLOCK_0(HalDev->dev_base)+PdspBlockOffset);
     for (i=0; i<NUM_PDSP_AAL5_STATE_WORDS; i++)
     *pTmp++ = 0;*/
    }

  /* Configure PDSP State RAM */
#ifdef AR7_EFM
  if (!HalDev->EFM_mode)
  {  // following for ATM mode only
#endif
  /* Setup TX PDSP State RAM */
  pTmp = (pPDSP_BLOCK_0(HalDev->dev_base)+PdspBlockOffset);
  *pTmp++ = HalCh->TxVc_CellRate;        /* Set the cell rate in cells/sec */
  *pTmp++ = HalCh->TxVc_QosType;         /* Configure the QoS Type */
  *pTmp++ = HalCh->TxVc_Mbs;             /* Set minimum burst size */
  *pTmp++ = 0;                           /* (skip a register) */
  *pTmp++ = HalCh->TxVc_Pcr;             /* set the peak cell rate */
  *pTmp++ = 0;                           /* new addition 4.9.02 */
  *pTmp++ = HalCh->TxVc_AtmHeader;       /* give the ATM header */
  *pTmp++ = (HalCh->TxVc_OamTc << 8)
            |(HalCh->TxVc_VpOffset);     /* Set the OAM TC Path and VP Offset */

  /* Setup RX PDSP State RAM */
  *pTmp++ = (HalCh->RxVc_OamCh)|
    (HalDev->OamMode << 8) |
    (HalCh->RxVc_OamToHost<<9);          /* Set OAM Channel, Mode, and ToHost options */
  *pTmp++ = HalCh->RxVc_AtmHeader;       /* ATM hdr put on firmware generated OAM */
  *pTmp++ = (HalCh->RxVc_VpOffset)|      /* Set Rx OAM TC Path and VP Offset */
    (HalCh->RxVc_OamTc<<8);

  /* Setup TX VP PDSP State RAM */
  pTmp = (pPDSP_BLOCK_0(HalDev->dev_base)+PdspBlockOffset+16); /*GSG~030703 12->16 */
  *pTmp++ = HalCh->TxVp_AtmHeader;
  *pTmp++ = (HalCh->TxVp_OamTc << 8);

  /* Setup RX VP PDSP State RAM */
  pTmp = (pPDSP_BLOCK_0(HalDev->dev_base)+PdspBlockOffset+20); /*GSG~030703 16->20 */
  *pTmp++ = HalCh->RxVp_AtmHeader;
  *pTmp++ = (HalCh->RxVp_OamCh)|
    (HalCh->RxVp_OamTc<<8)|
    (HalCh->RxVp_OamToHost<<9);           /* Set OAM Channel, Mode, and ToHost options */
  *pTmp++ = 0;
  *pTmp++ = HalCh->RxVp_OamVcList;

  /* Configure forwarding of unknown VCI/VPI cells */
  if (HalCh->PktType == 3)
    SAR_PDSP_FWD_UNK_VC_REG(HalDev->dev_base) = ((HalCh->FwdUnkVc<<31)|Ch);

#ifdef AR7_EFM
  } // above for ATM mode only
#endif


#ifdef EFM_DEBUG
  if (HalDev->EFM_mode && ((HalCh->DaMask !=0) || (HalCh->Priority !=0)))
  {
    printk("err: cpsar:halChannelSetup: Pri=%d, DaMask=%d\n", HalCh->Priority, HalCh->DaMask);
  }
#endif

    /* Configure Tx Channel Mapping Register (turn channel "ON") */
    TX_CH_MAPPING_REG(HalDev->dev_base) = 0x80000000  |
                                        (HalCh->DaMask << 30)
                                        | (HalCh->Priority << 24) | Ch;

  /* Setup Rx Channel in the LUT */
  i=0;
  while (!(RX_LUT_CH_SETUP_REQ_REG(HalDev->dev_base) & 0x80000000))
    {
    if (i > WAIT_THRESH)
      {
       return(EC_CPSAR|EC_FUNC_CHSETUP|EC_VAL_LUT_NOT_READY);
      }
    else
      i++;
    }

  /* RX LUT is ready */
  RX_LUT_CH_SETUP_REQ_REG(HalDev->dev_base) = (HalCh->PktType << 24) | Ch;
#ifdef AR7_EFM
  if (!HalDev->EFM_mode)
#endif
  RX_LUT_CH_SETUP_REQ_VC_REG(HalDev->dev_base) = ((HalCh->Vpi << 20) |
    (HalCh->Vci << 4));

  return (EC_NO_ERRORS);
  }

static int halChannelTeardown(CPSAR_DEVICE *HalDev, int Ch, bit32 Mode)
  {
   int waitcnt = 0;
   int PdspBlockOffset = 0, i;
   volatile bit32u *pTmp;

#ifdef __CPHAL_DEBUG
  if (DBG(0))
    {
     dbgPrintf("[cpsar]halChannelTeardown(HalDev:%08x, Ch:%d, Mode:%d\n",
                            (bit32u)HalDev, Ch, Mode);
     osfuncSioFlush();
    }
#endif

#ifdef AR7_EFM
  if (HalDev->EFM_mode)
  {
    if ((Ch < 0) || (Ch > MAX_EFM_CHAN))
     return(EC_CPSAR|EC_FUNC_CHTEARDOWN|EC_VAL_INVALID_CH);
  }
  else
#endif
   if ((Ch < 0) || (Ch > MAX_CHAN))
     return(EC_CPSAR|EC_FUNC_CHTEARDOWN|EC_VAL_INVALID_CH);

#ifdef AR7_EFM
  if (!HalDev->EFM_mode)
  {   // followings for ATM mode only
#endif
   /* Request RX channel teardown through LUT */
   while ((RX_LUT_CH_TEARDOWN_REQ_REG(HalDev->dev_base) & 0x80000000) == 0)
     {
      waitcnt++;
      if (waitcnt == WAIT_THRESH)
        {
         return(EC_CPSAR|EC_FUNC_CHTEARDOWN|EC_VAL_LUT_NOT_READY);
        }
     }

   RX_LUT_CH_TEARDOWN_REQ_REG(HalDev->dev_base) = (Ch & 0xffff);

   /* for AAL2, clear channel PDSP RAM here.  AAL5 does it when the teardown
      has completed (which is asynchronous)*/
   if (Ch > 15)
     {
      /* Get the offset to the AAL2 portion of the block (in words) */
      PdspBlockOffset = NUM_PDSP_AAL5_STATE_WORDS + ((Ch-16)*64);
      /* Clear PDSP State RAM */
      pTmp = (pPDSP_BLOCK_0(HalDev->dev_base)+PdspBlockOffset);
      for (i=0; i<NUM_PDSP_AAL2_STATE_WORDS; i++)
        *pTmp++ = 0;
     }

#ifdef AR7_EFM
  } // above for ATM mode only
#endif

   return (EC_NO_ERRORS);
  }

int InitPdsp(CPSAR_DEVICE *HalDev)
  {
extern void efmcrc_load_pdsp_crc16_lut(void);
extern void efm_pdsp_showtime_init(void);

   bit32u NumOfEntries,i,IRamAddress,iTmp;
   int *SarPdspFirmware; /* ~GSG 030403 */
   int FirmwareSize, rc; /* ~GSG 030403 */ 

#ifdef __CPHAL_DEBUG
  if (DBG(0))
    {
     dbgPrintf("[cpsar]InitPdsp(HalDev:%08x)\n", (bit32u)HalDev);
     osfuncSioFlush();
    }
#endif

   /* Get firmware */
   rc = HalDev->OsFunc->Control(HalDev->OsDev, "Firmware", "Get", &SarPdspFirmware);  /* ~GSG 030403 */
   if (rc)                                                  /* +GSG 030403 */
     return (EC_CPSAR|EC_FUNC_OPEN|EC_VAL_KEY_NOT_FOUND);   /* +GSG 030403 */

   /* Get firmware size */
   rc = HalDev->OsFunc->Control(HalDev->OsDev, "FirmwareSize", "Get", &FirmwareSize); /* ~GSG 030403 */
   if (rc)                                                  /* +GSG 030403 */
     return (EC_CPSAR|EC_FUNC_OPEN|EC_VAL_KEY_NOT_FOUND);   /* +GSG 030403 */

   IRamAddress = (bit32u) pPDSP_CTRL_REG(HalDev->dev_base);

   NumOfEntries = (FirmwareSize)/4;                         /* ~GSG 030403 */
   if (NumOfEntries > IRAM_SIZE)
     {
       /* Note: On Avalanche, they truncated the PDSP firmware and warned */
       /* NumOfEntries = IRAM_SIZE; */
       return(EC_CPSAR|EC_FUNC_INIT|EC_VAL_FIRMWARE_TOO_LARGE);
     }

   for(i=8;i<NumOfEntries;i++)
     (*((bit32 *) (IRamAddress+(i*4))))=SarPdspFirmware[i]; /* ~GSG 030403 */

      /* Check code */
   for(i=8;i<NumOfEntries;i++)
     {
       iTmp=(*((bit32 *) (IRamAddress+(i*4))));
       if (iTmp != SarPdspFirmware[i])                      /* ~GSG 030403 */
         {
           return(EC_CPSAR|EC_FUNC_OPEN|EC_VAL_PDSP_LOAD_FAIL);
         }
     }

#ifdef AR7_EFM
   if (HalDev->EFM_mode)
   {
      efm_pdsp_showtime_init ();
      efmcrc_load_pdsp_crc16_lut(); // Initialize DS TC_CRC16 table for pdsp
   }
#endif

   return(EC_NO_ERRORS);
  }

#ifdef AR7_EFM
// This function initialize GPR counters after each showtime. 
void efm_pdsp_showtime_init(void)
{
  int i;
  volatile unsigned int *GPR=(volatile unsigned int *) (0xa3000000 + 0x20); // start of GPR0

  // clear GPR counts
  for (i=0; i<=6; i++)
  {
    if ((i==4) || (i==5)) continue;
    GPR[i]= 0;
  }
}

#endif

/*
 *  This function probes for the instance of the CPHAL module.  It will call
 *  the OS function @c DeviceFindInfo() to get the information required.
 *
 *  @param   HalDev      CPHAL module instance. (set by xxxInitModule())
 *
 *  @return  0 OK, Otherwise error.
 */

static int halProbe(CPSAR_DEVICE *HalDev)
  {
   int Ret;
   

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpsar]halProbe(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

#ifdef __CPHAL_DEBUG
   if (DBG(1))
     {
      dbgPrintf("[os]DeviceFindInfo(Inst:%d, DeviceName:%s, DeviceInfo:%08x)\n",
                HalDev->Inst, "sar", (bit32u)&HalDev->DeviceInfo);
      osfuncSioFlush();
     }
#endif

   /* Attempt to find the device information */
   Ret = HalDev->OsFunc->DeviceFindInfo(HalDev->Inst, "sar", &HalDev->DeviceInfo);


   if (Ret)
     return(EC_CPSAR|EC_FUNC_PROBE|EC_VAL_DEVICE_NOT_FOUND);

   return(EC_NO_ERRORS);
  }

#ifdef __CPHAL_DEBUG
static void dbgConfigDump(HAL_DEVICE *HalDev)
  {
   dbgPrintf("  [cpsar Inst %d] Config Dump:\n", HalDev->Inst);
   dbgPrintf("    Base    :%08x, offset   :%08d\n",
             HalDev->dev_base, HalDev->offset);
   dbgPrintf("    ResetBit:%08d, ResetBase:%08x\n",
             HalDev->ResetBit, HalDev->ResetBase);
   dbgPrintf("    UniNni  :%08d, debug    :%08d\n",
             HalDev->ResetBit, HalDev->debug);
   osfuncSioFlush();
  }
#endif

/*
 * Sets up HAL default configuration parameter values.
 */
static void ConfigInit(CPSAR_DEVICE *HalDev)
  {
#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpsar]ConfigInit(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif
   /* configure some defaults with tnetd7300 values */
   HalDev->dev_base = 0xa3000000;
   HalDev->offset = 0;
   HalDev->UniNni = CFG_UNI_NNI;
   HalDev->ResetBit = 9;
   HalDev->debug = 0;
   HalDev->ResetBase = 0xa8611600;
  }

/*
 * Retrieve HAL configuration parameter values.
 */
static bit32u ConfigGet(HAL_DEVICE *HalDev)
  {
   bit32u ParmValue, error_code;
   char *DeviceInfo = HalDev->DeviceInfo;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpsar]ConfigGet(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   /* get the configuration parameters common to all modules */
   error_code = ConfigGetCommon(HalDev);
   if (error_code) return (EC_CPSAR|error_code);

   /* get SAR specific configuration parameters */
#ifdef AR7_EFM
  if (!HalDev->EFM_mode)
  { // following for ATM mode only
#endif
   error_code = HalDev->OsFunc->DeviceFindParmUint(DeviceInfo,"UniNni",&ParmValue);
   if (!error_code) HalDev->UniNni = ParmValue;
#ifdef AR7_EFM
  }
#endif

   return (EC_NO_ERRORS);
  }

static int halInit(CPSAR_DEVICE *HalDev)
  {
   bit32u Ret;   

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpsar]halInit(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

#ifdef __CPHAL_DEBUG
   if (DBG(7))
     {
      dbgPrintf("[cpsar halInit()]InitCount = %d\n", HalDev->InitCount);
      osfuncSioFlush();
     }
#endif


   /* Only run the init code for the first calling module per instance */
   if (HalDev->InitCount > 1)
     {
      return (EC_NO_ERRORS);
     }

   /* Configure HAL defaults */
   ConfigInit(HalDev);

   /* Retrieve HAL configuration parameters from data store */
   Ret = ConfigGet(HalDev);
   if (Ret) return (Ret);

#ifdef __CPHAL_DEBUG
   if (DBG(9))
     dbgConfigDump(HalDev);
#endif


   return(EC_NO_ERRORS);
  }

#ifdef AR7_EFM
static int halSetEFMmode(CPSAR_DEVICE *HalDev, int Mode)
{
  HalDev->EFM_mode = Mode;
  return (EC_NO_ERRORS);
}

static int halSetOSDev(CPSAR_DEVICE *HalDev, OS_DEVICE *OS_Dev)
{
  HalDev->OsDev = OS_Dev;
  return (EC_NO_ERRORS);
}
#endif

static int halOpen(CPSAR_DEVICE *HalDev)
  {
   int Ret, Ticks=64;
   int i;              /*+GSG 030407*/
   volatile int *pTmp; /*+GSG 030407*/


#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpsar]halOpen(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

#ifdef __CPHAL_DEBUG
   if (DBG(7))
     {
      dbgPrintf("[cpsar halOpen()]OpenCount = %d\n", HalDev->OpenCount);
      osfuncSioFlush();
     }
#endif

   /* Only run the open code for the first calling module per instance */
   if (HalDev->OpenCount++ > 0)
     {
      return (EC_NO_ERRORS);
     }


   /* Take SAR out of reset */
   if (((*(volatile bit32u *)(HalDev->ResetBase)) & (1<<HalDev->ResetBit)) != 0)
     {
       /** @todo Should I somehow call AAL5/AAL2 Close() here? All I've done
           here is copy the Close code from each and paste it here. */

#ifdef __CPHAL_DEBUG
      if (DBG(7))
        {
         dbgPrintf("[cpsar halOpen()]Module was already out of reset.\n");
         dbgPrintf("                 Closing module and resetting.\n");
         osfuncSioFlush();
        }
#endif

      /* Disable the Tx CPPI DMA */
      TX_CPPI_CTL_REG(HalDev->dev_base) = 0;

      /* Disable the Rx CPPI DMA */
      RX_CPPI_CTL_REG(HalDev->dev_base) = 0;

      /* Disable the PDSP */
      PDSP_CTRL_REG(HalDev->dev_base) &=~ 0x00000002;

      /* disable interrupt masks */
      SAR_TX_MASK_CLR(HalDev->dev_base) = 0xffffffff;
      SAR_RX_MASK_CLR(HalDev->dev_base) = 0xffffffff;

#ifndef NO_RESET /* GSG+ 030428 */
      /* clear reset bit */
      (*(volatile bit32u *)(HalDev->ResetBase)) &=~ (1<<HalDev->ResetBit); /* ~GSG 030307 */
      HalDev->OsFunc->Control(HalDev->OsDev, "Sleep", "", &Ticks);

      /* set reset bit */
      (*(volatile bit32u *)(HalDev->ResetBase)) |= (1<<HalDev->ResetBit); /* ~GSG 030307 */
      HalDev->OsFunc->Control(HalDev->OsDev, "Sleep", "", &Ticks);
#endif /* GSG+ 030428 */
     }
    else
     {
      (*(volatile bit32u *)(HalDev->ResetBase)) |= (1<<HalDev->ResetBit); /* ~GSG 030307 */
      HalDev->OsFunc->Control(HalDev->OsDev, "Sleep", "", &Ticks);
     }


   /* Configure UNI/NNI */
#ifdef AR7_EFM
  if (!HalDev->EFM_mode)  // following for ATM only
#endif
   RX_LUT_GLOBAL_CFG_REG(HalDev->dev_base) |= (HalDev->UniNni & 0x1);

   /* Clear entire PDSP state RAM */                /*+GSG 030407*/
   pTmp = (pTX_DMA_STATE_WORD_0(HalDev->dev_base)); /*+GSG 030407*/
   for (i=0; i<PDSP_STATE_RAM_SIZE; i++)            /*+GSG 030407*/
     *pTmp++ = 0;                                   /*+GSG 030407*/

   /* Configure Oam Mode */                         /*+GSG 030407*/
#ifdef AR7_EFM
  if (!HalDev->EFM_mode) // following for ATM mode only
#endif
   SetOamMode(HalDev);                              /*+GSG 030407*/

   /* Initialize PDSP */
   Ret=InitPdsp(HalDev);
   if(Ret)
     return(Ret);

   /* Reset and Enable the PDSP */
   PDSP_CTRL_REG(HalDev->dev_base) = 0x00080003;   

   return(EC_NO_ERRORS);
  }

static int halClose(CPSAR_DEVICE *HalDev, int Mode)
  {
   int Ticks = 64;

#ifdef __CPHAL_DEBUG
  if (DBG(0))
    {
     dbgPrintf("[cpsar]halClose(HalDev:%08x, Mode:%d)\n", (bit32u)HalDev, Mode);
     osfuncSioFlush();
    }
#endif


   /* handle the error case if there is nothing open */
   if (HalDev->OpenCount == 0)
     {
      return(EC_CPSAR|EC_FUNC_CLOSE|EC_VAL_MODULE_ALREADY_CLOSED);
     }

#ifdef __CPHAL_DEBUG
   if (DBG(7))
     {
      dbgPrintf("[cpsar halClose()]OpenCount = %d\n", HalDev->OpenCount);
      osfuncSioFlush();
     }
#endif

   /* Only run the close code for the last calling module per instance */
   if (HalDev->OpenCount-- > 1)
     {
      return (EC_NO_ERRORS);
     }

   /* Disable the PDSP */
   PDSP_CTRL_REG(HalDev->dev_base) &=~ 0x00000002;

#ifndef NO_RESET /* GSG +030428 */
   /* put device back into reset */
   (*(volatile bit32u *)(HalDev->ResetBase)) &=~ (1<<HalDev->ResetBit); /* ~GSG 030307 */
   HalDev->OsFunc->Control(HalDev->OsDev, "Sleep", "", &Ticks);
#endif /* GSG +030428 */

   return(EC_NO_ERRORS);
  }

static int halShutdown(CPSAR_DEVICE *HalDev)
  {
#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpsar]halShutdown(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   /* handle the error case */
   if (HalDev->InitCount == 0)
     {
      return(EC_CPSAR|EC_FUNC_CLOSE|EC_VAL_MODULE_ALREADY_SHUTDOWN);
     }

#ifdef __CPHAL_DEBUG
   if (DBG(7))
     {
      dbgPrintf("[cpsar halShutdown()]InitCount = %d\n", HalDev->InitCount);
      osfuncSioFlush();
     }
#endif

   /* Only run the shutdown code for the last calling module per instance */
   if (HalDev->InitCount-- > 1)
     {
      return (EC_NO_ERRORS);
     }

   /* free the SAR functions */
#ifdef __CPHAL_DEBUG
   if (DBG(6))
     {
      dbgPrintf("  [cpsar halShutdown()]: Free CPSAR function pointers\n");
      osfuncSioFlush();
     }
   if (DBG(1)||DBG(3))
     {
      dbgPrintf("[os]Free(MemPtr:%08x)\n", (bit32u)HalDev->HalFuncPtr);
      osfuncSioFlush();
     }
#endif
   /* free the HalFunc */
   HalDev->OsFunc->Free(HalDev->HalFuncPtr);

   /* we have a static global, so I should clear it's value as well */
   CpsarDev[HalDev->Inst] = 0;

#ifdef __CPHAL_DEBUG
   if (DBG(6))
     {
      dbgPrintf("  [cpsar halShutdown()]Free HalDev\n");
      osfuncSioFlush();
     }
   if (DBG(1)||DBG(3))
     {
      dbgPrintf("[os]Free(MemPtr:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif
   /* free the CPSAR device */
   HalDev->OsFunc->Free(HalDev);

   return(EC_NO_ERRORS);
  }

static int halTick(CPSAR_DEVICE *HalDev)
  {
#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[cpsar]halTick(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   return(EC_NO_ERRORS);
  }

/*
 *  The CPSAR version of InitModule() should be passed the OS_FUNCTIONS pointer,
 *  and will return the HalDev pointer.
 *
 *  @param   HalDev  Pointer to CPSAR module information.  This will
 *                   be used by the OS when communicating to this module via
 *                   CPSAR.
 *  @param   OsDev   Pointer to OS device information.  This will be saved by
 *                   the CPSAR and returned to the OS when required.
 *  @param   HalFunc HAL_FUNCTIONS pointer.
 *  @param   Size    Pointer to the size of the HAL_FUNCTIONS structure.  (If
 *                   HalFunc is 0, the value will be set by CPSAR, otherwise
 *                   ignored)
 *  @param   Inst    The instance number of the module to initialize. (start at
 *                   0).
 *
 *  @return  0 OK, Nonzero - error.
 */
/*
int cpsarInitModule(CPSAR_DEVICE **HalDev,
                 OS_DEVICE *OsDev,
                 CPSAR_FUNCTIONS *HalFunc,
                 OS_FUNCTIONS *OsFunc,
                 int *Size,
                 int Inst)
*/
int cpsarInitModule(CPSAR_DEVICE **HalDev,
                 OS_DEVICE *OsDev,
                 CPSAR_FUNCTIONS **HalFunc,
                 OS_FUNCTIONS *OsFunc,
                 int OsFuncSize,
                 int *HalFuncSize,
                 int Inst)
  {
   CPSAR_DEVICE *HalPtr;
   CPSAR_FUNCTIONS *HalFuncPtr;

   /*
   if ( HalFunc == 0 )
     {
      *Size = sizeof(CPSAR_FUNCTIONS);
      return(EC_NO_ERRORS);
     }
   */

   if (CpsarDev[Inst] != 0)
     {
      /* this SAR module has been connected to before, so do not
         allocate another CPSAR_DEVICE */
      HalPtr = CpsarDev[Inst];

      /* increase count of attached modules */
      HalPtr->InitCount++;
     }
    else
     {
      /* allocate the CPSAR_DEVICE structure */
      HalPtr = (CPSAR_DEVICE *) OsFunc->MallocDev(sizeof(CPSAR_DEVICE));
      if(!HalPtr)
        return(EC_CPSAR|EC_FUNC_HAL_INIT|EC_VAL_MALLOC_DEV_FAILED);

      HalFuncPtr = (CPSAR_FUNCTIONS *) OsFunc->Malloc(sizeof(CPSAR_FUNCTIONS));
      if (!HalFuncPtr)
        return (EC_CPSAR|EC_FUNC_HAL_INIT|EC_VAL_MALLOC_FAILED);

      /* Initialize the size of hal functions */
      *HalFuncSize = sizeof (CPSAR_FUNCTIONS);

      /* ensure the device structure is cleared */
      OsFunc->Memset(HalPtr, 0, sizeof(CPSAR_DEVICE));

      /* clear the function pointers */
      OsFunc->Memset(HalFuncPtr, 0, sizeof(CPSAR_FUNCTIONS));

      /* Supply pointers for the CPSAR API functions */
      HalFuncPtr->ChannelSetup    = halChannelSetup;
      HalFuncPtr->ChannelTeardown = halChannelTeardown;
      HalFuncPtr->Close           = halClose;
      HalFuncPtr->Control         = halControl;
      HalFuncPtr->Init            = halInit;
      HalFuncPtr->Open            = halOpen;
      HalFuncPtr->Probe           = halProbe;
      HalFuncPtr->Shutdown        = halShutdown;
      HalFuncPtr->Tick            = halTick;
#ifdef AR7_EFM
      HalFuncPtr->SetEFMmode      = halSetEFMmode;
      HalFuncPtr->SetOSDev        = halSetOSDev;
#endif

      /* keep a reference to HalFuncPtr so I can free it later */
      HalPtr->HalFuncPtr = HalFuncPtr;

      /* store the CPSAR_DEVICE, so the CPSAR module will know whether
         it is in use for the given instance */
      CpsarDev[Inst] = HalPtr;

      /* increase count of attached modules */
      HalPtr->InitCount++;
     }

   /* @todo Does this need modification to deal with multiple callers/
      drivers?  If different callers will use different OsDev/OsFunc,
      then the current code will not work.
   */

   /* initialize the CPSAR_DEVICE structure */
   HalPtr->OsDev  = OsDev;
   /*HalPtr->OsOpen = OsDev;*/
   HalPtr->Inst   = Inst;
   HalPtr->OsFunc = OsFunc;

   /* pass the HalPtr back to the caller */
   *HalDev = HalPtr;
   *HalFunc = HalPtr->HalFuncPtr;

   return (EC_NO_ERRORS);
  }
