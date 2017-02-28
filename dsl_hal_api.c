/*******************************************************************************
* FILE PURPOSE:     DSL Driver API functions for Sangam
*
********************************************************************************
* FILE NAME:        dsl_hal_basicapi.c
*
* DESCRIPTION:  
*       Contains basic DSL HAL APIs for Sangam
*  
*                    
* (C) Copyright 2001-02, Texas Instruments, Inc.
*    History
*    Date        Version            Notes
*    06Feb03     0.00.00            RamP   Original Version Created
*    10Mar03     0.00.01            RamP   Initial Revision for Modular Code Branch
*    19Mar03     0.00.02            RamP   Fixed DSL and DSP Version API Structures
*    20Mar03     0.00.03            RamP   Changed byteswap function names
*    21Mar03     0.00.03         RamP/ZT   Malloc for DSP f/w done in dslStartup
*    25Mar03     0.00.04            RamP   Removed statistics used only by SWTC
*                                          Created Checkpoint 3
*    26Mar03     0.00.05            RamP   Added Memory allocation for fwimage in
*                                          dslStartup function.
*    07Apr03     0.00.06            RamP   Implemented new error reporting scheme
*                                          Changed Commenting to C style only
*    09Apr03     0.00.07            RamP   Reorganized code to delete POST_SILICON
*    10Apr03     0.00.08            RamP   Removed ptidsl from loadFWImage function
*                                          moved size and fwimage initialization to
*                                          dslStartup function
*    14Apr03     0.00.09            RamP   Moved modemStateBitField processing to a
*                                          support function; deleted stateHistory
*                                          renamed the REG32 macro
*    15Apr03     0.00.10            RamP   Changed firmware allocate to shim_ 
*                                          osAllocateVMemory function
*    15Apr03     0.00.11            RamP   Changed host version number to 07.00.00.01
*    16Apr03     0.00.12            RamP   Modified return condition on dslShutdown
*    16Apr03     0.00.13            RamP   Changed host version number to 07.00.00.02
*    21Apr03     0.01.00            RamP   Cleaned up dslShutdown function
*                                          Added new function calls to allocate
*                (Alpha)                   /free overlay pages for different OS
*                                          Fixed typecasting for allocate/free fxns
*                                          Added Interrupt Acknowledge logic
*    22Apr03     0.01.01            RamP   Moved acknowledgeInterrupt into api
*                                          Added static global for intr source
*    24Apr03     0.01.02            RamP   Added processing for OVERLAY_END in
*                                          DSP message handlers, verified crc32
*                                          recovery for overlays
*    28Apr03     0.01.03            RamP   Removed global variable intrSource
*                                          Added parameter to handleInterrupt fxn
*                (Alpha Plus)              to indicate interrupt source
*                                          Changed version number to 01.00.01.00    
*                                          Fixed setTrainingMode function problem 
*    07May03     0.01.04            RamP   Removed delineation state check in 
*                                          message handling functions, added more
*                                          safety for setting lConnected in TC_SYNC
*                                          Changed version number to 01.00.01.01                                      
*    14May03     0.01.05            RamP   Added 3 Switchable Hybrid APIs
*                                          Added additional statistics us/ds TxPower,
*                                          us margin,attenuation, us/ds bitallocation
*                                          moved versioning to dsl_hal_version.h
*    14May03     0.01.06            RamP   Fixed problem with CMsgs2 parsing
*    20May03     0.01.07            RamP   Added Inner/Outer pair API support. Added
*                                          dying gasp message.
*    29May03     0.01.08         ZT/RamP   Added memory optimizations for overlay pages
*                                          and coProfiles; added functions to free,
*                                          reload overlays and profiles
*    04Jun03     0.01.09            RamP   Added tick counters, fail states reporting
*                                          Made statistics fixes for higher data rates
*                                          Added Margin per tone to statistics
*                                          Added configuration checks for trellis/FEC
*    06Jun03     0.01.10            RamP   Added LED, STM Bert, dGasp LPR Config APIs
*                                          Modified interrupt acknowledge logic 
*                                          Added current hybrid flag as statistic
*    09Jun03     0.01.11            RamP   Added function to send dying Gasp to Modem
*                                          fixed problem with reading OamNegoPara var
*                (Beta)                    fixed problem with reading current config
*                                          Added function to configure ATM Bert
*                                          fixed memory leak due to coProfiles
*                                          Added us/ds R/S FEC statistics
*                                          Added additional config capability for LED
*                                          fixed problem in free memory for CO profiles
*    18Jul03     0.01.12            RamP   Fixed problem with reading modemEnv structure
*                                          affects LED, DGaspLpr APIs   
*                                          Sending Dying Gasp from shutdown function
*    01Aug03     0.01.13            RamP   Added preferred training mode to statistics
*    13Aug03     0.01.14            MCB    Set rev id for D3/R1.1 (ADSL2).
*    21Aug03     0.01.15            RamP   Added g.hs and aoc bitswap message gathering
*                                          Added new references to bits n gains table
*                                          Decoupled modem idle/retrain from pair select 
*                                          Added line length and gross gain to statistics 
*    29Sep03     0.01.16            RamP   Replaced advcfg function calls with support
*                                          module function switches
*    01Oct03     0.01.17            RamP   Added enum translation to set training mode
*                                          & to read statistics
*    08Oct03     0.01.18            RamP   Fixed problems with usTxPower statistic in 
*                                          Annex B target, fixed problem with Trellis
*    12Oct03     0.01.19            RamP   Added API calls to gather ADSL2 Messages
*    29Oct03     0.01.20            RamP   Restored TC_SYNC detect logic
*    30Oct03     0.01.21            RamP   Removed Scaling factor for adsl2DSConRate
*                                          Setting Showtime state upon DSP_ACTIVE
*    14Nov03     0.01.22            RamP   Fixed scaling for usTxPower & dsTxPower
*    14Nov03     0.01.23            RamP   Added logic to gather CRates1/RRates1
*                                          by parsing DSP_CRATES1
*    20Nov03     0.01.24            RamP   Added generic & interface Read
*                                          and Write functions to read from 
*                                          DSP - Host Interface
*    24Nov03     0.01.25            RamP   Modified interface Read/Write functions
*                                          to seperate element offsets from pointers
*    19Dec03     0.01.26            RamP   Modified pointer accessing problems with
*                                          block read functions
*    26Dec03     0.01.27            RamP   Made ghsIndex a local variable & added 
*                                          check to avoid buffer overflow     
*    30Dec03     0.01.28            RamP   Added generic mailbox command function
*******************************************************************************/
#include "dsl_hal_register.h"
#include "dsl_hal_support.h"
#include "dsl_hal_logtable.h"
#include "dsl_hal_version.h"

static unsigned int hybrid_selected;
static unsigned int showtimeFlag = FALSE;

#ifdef PRE_SILICON
/*********************************************/
/*   Base Addresses                          */
/*********************************************/
#define DEV_MDMA_BASE                 0x02000500

/*********************************************/
/*   MC DMA Control Registers in DSL         */
/*********************************************/

#define DEV_MDMA0_SRC_ADDR         (DEV_MDMA_BASE + 0x00000000)
#define DEV_MDMA0_DST_ADDR         (DEV_MDMA_BASE + 0x00000004)
#define DEV_MDMA0_CTL_ADDR         (DEV_MDMA_BASE + 0x00000008)
#define DEV_MDMA1_SRC_ADDR         (DEV_MDMA_BASE + 0x00000040)
#define DEV_MDMA1_DST_ADDR         (DEV_MDMA_BASE + 0x00000044)
#define DEV_MDMA1_CTL_ADDR         (DEV_MDMA_BASE + 0x00000048)
#define DEV_MDMA2_SRC_ADDR         (DEV_MDMA_BASE + 0x00000080)
#define DEV_MDMA2_DST_ADDR         (DEV_MDMA_BASE + 0x00000084)
#define DEV_MDMA2_CTL_ADDR         (DEV_MDMA_BASE + 0x00000088)
#define DEV_MDMA3_SRC_ADDR         (DEV_MDMA_BASE + 0x000000C0)
#define DEV_MDMA3_DST_ADDR         (DEV_MDMA_BASE + 0x000000C4)
#define DEV_MDMA3_CTL_ADDR         (DEV_MDMA_BASE + 0x000000C8)

#define DEV_MDMA0_SRC           (*((volatile UINT32 *) DEV_MDMA0_SRC_ADDR))
#define DEV_MDMA0_DST           (*((volatile UINT32 *) DEV_MDMA0_DST_ADDR))
#define DEV_MDMA0_CTL           (*((volatile UINT32 *) DEV_MDMA0_CTL_ADDR))
#define DEV_MDMA1_SRC           (*((volatile UINT32 *) DEV_MDMA1_SRC_ADDR))
#define DEV_MDMA1_DST           (*((volatile UINT32 *) DEV_MDMA1_DST_ADDR))
#define DEV_MDMA1_CTL           (*((volatile UINT32 *) DEV_MDMA1_CTL_ADDR))
#define DEV_MDMA2_SRC           (*((volatile UINT32 *) DEV_MDMA2_SRC_ADDR))
#define DEV_MDMA2_DST           (*((volatile UINT32 *) DEV_MDMA2_DST_ADDR))
#define DEV_MDMA2_CTL           (*((volatile UINT32 *) DEV_MDMA2_CTL_ADDR))
#define DEV_MDMA3_SRC           (*((volatile UINT32 *) DEV_MDMA3_SRC_ADDR))
#define DEV_MDMA3_DST           (*((volatile UINT32 *) DEV_MDMA3_DST_ADDR))
#define DEV_MDMA3_CTL           (*((volatile UINT32 *) DEV_MDMA3_CTL_ADDR))

/* MDMA control bits                     */

#define DEV_MDMA_START             0x80000000
#define DEV_MDMA_STOP              0x00000000
#define DEV_MDMA_STATUS            0x40000000
#define DEV_MDMA_DST_INC           0x00000000
#define DEV_MDMA_DST_FIX           0x02000000   
#define DEV_MDMA_SRC_INC           0x00000000
#define DEV_MDMA_SRC_FIX           0x00800000 
#define DEV_MDMA_BURST1            0x00000000
#define DEV_MDMA_BURST2            0x00100000
#define DEV_MDMA_BURST4            0x00200000 

#define DEV_MDMA_LEN_SHF           2
#define DEV_MDMA_LEN_MASK          0x0000FFFF

#define DMA0   0
#define DMA1   1
#define DMA2   2
#define DMA3   3
#endif
#ifdef DMA
SINT32 getDmaStatus(UINT32 mask)
{
  if(!(IFR & mask))
  {
   return DSLHAL_ERROR_NO_ERRORS;
  }
  else
  {
   ICR = mask ;
   return 1 ;
  }
}

void programMdma(UINT32 dma, UINT32 source, UINT32 destination, UINT32 length, UINT32 wait)
{
  volatile UINT32 statusMask ;

  switch(dma)
  {
  case DMA0:
    {
      DEV_MDMA0_SRC = source ;
      DEV_MDMA0_DST = destination ;
      DEV_MDMA0_CTL = (DEV_MDMA_START | DEV_MDMA_DST_INC | DEV_MDMA_SRC_INC |
                       DEV_MDMA_BURST1 | (length << DEV_MDMA_LEN_SHF)) ;
      statusMask = 0x00000010 ;
    }
    break ;
  case DMA1:
    {
          DEV_MDMA1_SRC = source ;
          DEV_MDMA1_DST = destination ;
          DEV_MDMA1_CTL = (DEV_MDMA_START | DEV_MDMA_DST_INC | DEV_MDMA_SRC_INC |
                           DEV_MDMA_BURST1 | (length << DEV_MDMA_LEN_SHF)) ;
          statusMask = 0x00000020 ;
        }
    break ;
  case DMA2:
    {
          DEV_MDMA2_SRC = source ;
          DEV_MDMA2_DST = destination ;
          DEV_MDMA2_CTL = (DEV_MDMA_START | DEV_MDMA_DST_INC | DEV_MDMA_SRC_INC |
                           DEV_MDMA_BURST1 | (length << DEV_MDMA_LEN_SHF)) ;
          statusMask = 0x00000040 ;
        }
    break ;
  case DMA3:
    {
          DEV_MDMA3_SRC = source ;
          DEV_MDMA3_DST = destination ;
          DEV_MDMA3_CTL = (DEV_MDMA_START | DEV_MDMA_DST_INC | DEV_MDMA_SRC_INC |
                           DEV_MDMA_BURST1 | (length << DEV_MDMA_LEN_SHF)) ;
          statusMask = 0x00000080 ;
        }
    break ;

  }

  if(wait)
  {
    while(!(getDmaStatus(statusMask))) ;
  }

}
#endif



/******************************************************************************************
* FUNCTION NAME:    dslhal_api_dslStartup
*
*******************************************************************************************
* DESCRIPTION: Entry point to initialize and load ax5 daughter board
*
* INPUT:   PITIDSLHW_T *ppIHw  
*
* RETURN:  0 --succeeded
*          1 --Failed
*
*****************************************************************************************/

int dslhal_api_dslStartup(PITIDSLHW_T *ppIHw)
{
  
  ITIDSLHW_T *ptidsl;
  int  i;
  int rc;
  dprintf(4,"dslhal_api_dslStartup() NEW 1\n");
  
  ptidsl=(ITIDSLHW_T *)shim_osAllocateMemory(sizeof(ITIDSLHW_T));
  if(ptidsl==NULL)
    {
    dprintf(1, "unable to allocate memory for ptidsl\n");
    return 1;
    }
  *ppIHw=ptidsl;
  shim_osZeroMemory((char *) ptidsl, sizeof(ITIDSLHW_T));

  /* Unreset the ADSL Subsystem  */
  rc=dslhal_support_unresetDslSubsystem();
  if(rc)
    {
    dprintf(1, "unable to reset ADSL Subsystem \n");
    shim_osFreeMemory((void *) ptidsl, sizeof(ITIDSLHW_T));
    return DSLHAL_ERROR_UNRESET_ADSLSS;
    }
  ptidsl->fwimage = shim_osAllocateVMemory(DSP_FIRMWARE_MALLOC_SIZE);
  if(!ptidsl->fwimage)
    {
      dprintf(1,"Failed to Allocate Memory for DSP firmware binary \n");
      return DSLHAL_ERROR_FIRMWARE_MALLOC;
    }
  /* read firmware file from flash   */
  rc=shim_osLoadFWImage(ptidsl->fwimage);
  if(rc<0)
    {
    dprintf(1, "unable to get fw image\n");
    shim_osFreeVMemory((void *)ptidsl->fwimage,DSP_FIRMWARE_MALLOC_SIZE);
    shim_osFreeMemory((void *) ptidsl, sizeof(ITIDSLHW_T));
    return DSLHAL_ERROR_NO_FIRMWARE_IMAGE;
    }
  else
    {
      ptidsl->imagesize = rc;
     }
  /* Compute the CRC checksum on the image and validate the image */

  /* Validate the image in the RAM */

  /* load fw to DSP  */
 
  if(dslhal_support_hostDspCodeDownload(ptidsl))
    {
    dprintf(0,"dsp load error\n");
    for(i=0; i<NUM_PAGES; i++)
      {
      if(ptidsl->olayDpPage[i].PmemStartWtAddr !=NULL)
        {
        shim_osFreeDmaMemory((void *) ptidsl->olayDpPage[i].PmemStartWtAddr, 
                     ptidsl->olayDpPage[i].OverlayXferCount);
        }
      }
     if(ptidsl->coProfiles.PmemStartWtAddr != NULL)
      shim_osFreeDmaMemory((void *)ptidsl->coProfiles.PmemStartWtAddr, ptidsl->coProfiles.OverlayXferCount);
     if(ptidsl->constDisplay.PmemStartWtAddr != NULL)
      shim_osFreeDmaMemory((void *)ptidsl->constDisplay.PmemStartWtAddr, ptidsl->constDisplay.OverlayXferCount);
    shim_osFreeVMemory((void *)ptidsl->fwimage,DSP_FIRMWARE_MALLOC_SIZE);
    shim_osFreeMemory((void *) ptidsl, sizeof(ITIDSLHW_T));
    return DSLHAL_ERROR_CODE_DOWNLOAD;
    }
  
  /* set flag to indicated overlay pages are loaded */
  ptidsl->bOverlayPageLoaded = 1;
  /* set auto retrain to 1 to disble the overlay page reload */
  ptidsl->bAutoRetrain = 1;

  /* unreset Raptor */
  /* change this to new function */ 
  /* This function should basically bring DSP out of reset bit 23  of PRCR */
  /* Function is ready but bypassed for Pre-Silicon */
   
  rc=dslhal_support_unresetDsp();
  if (rc)
    {
    dprintf(0,"unable to bring DSP out of Reset\n");
    for(i=0; i<NUM_PAGES; i++)
      {
      if(ptidsl->olayDpPage[i].PmemStartWtAddr !=NULL)
        {
        shim_osFreeDmaMemory((void *) ptidsl->olayDpPage[i].PmemStartWtAddr, 
                     ptidsl->olayDpPage[i].OverlayXferCount);
        }
      }
    if(ptidsl->coProfiles.PmemStartWtAddr != NULL)
      shim_osFreeDmaMemory((void *)ptidsl->coProfiles.PmemStartWtAddr, ptidsl->coProfiles.OverlayXferCount);
     if(ptidsl->constDisplay.PmemStartWtAddr != NULL)
      shim_osFreeDmaMemory((void *)ptidsl->constDisplay.PmemStartWtAddr, ptidsl->constDisplay.OverlayXferCount);
    shim_osFreeVMemory((void *)ptidsl->fwimage,DSP_FIRMWARE_MALLOC_SIZE);
    shim_osFreeMemory((void *) ptidsl, sizeof(ITIDSLHW_T));
    return DSLHAL_ERROR_UNRESET_DSP;
    }
  shim_osFreeVMemory((void *)ptidsl->fwimage,DSP_FIRMWARE_MALLOC_SIZE);
  dprintf(4,"dslhal_api_dslStartup() done\n");

  /* Add the code to initialize the host interface variables */
  /* Add code to tickle the host interface */
  return DSLHAL_ERROR_NO_ERRORS;
}


/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_dslShutdown
 *
 *******************************************************************************************
 * DESCRIPTION: routine to shutdown ax5 modem and free the resource
 *
 * INPUT:   tidsl_t *ptidsl  
 *
 * RETURN:  NULL
 *          
 *
 *****************************************************************************************/

int dslhal_api_dslShutdown(tidsl_t *ptidsl)
{
  int rc= DSLHAL_ERROR_NO_ERRORS;
  int i;
  
  dprintf(5, "dslhal_api_dslShutdown\n");
  rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DSLSS_SHUTDOWN, 0, 0, 0);  
  if(rc)
    {
     dprintf(1, " unable to reset DSP \n");
     rc = DSLHAL_ERROR_RESET_DSP;
    }
  /* DSP need 50 ms to send out the message*/

  shim_osClockWait(60 * 1000);

  rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DGASP, 0, 0, 0);
  
  /* free memory allocated*/

  for(i=0; i<NUM_PAGES; i++)
    {
    if(ptidsl->olayDpPage[i].PmemStartWtAddr !=NULL)
      {
      shim_osFreeDmaMemory((void *) ptidsl->olayDpPage[i].PmemStartWtAddr, 
                   ptidsl->olayDpPage[i].OverlayXferCount);
      }
    }
  if(ptidsl->coProfiles.PmemStartWtAddr != NULL)
     shim_osFreeDmaMemory((void *)ptidsl->coProfiles.PmemStartWtAddr, ptidsl->coProfiles.OverlayXferCount);
  if(ptidsl->constDisplay.PmemStartWtAddr != NULL)
     shim_osFreeDmaMemory((void *)ptidsl->constDisplay.PmemStartWtAddr, ptidsl->constDisplay.OverlayXferCount);
  shim_osFreeMemory((void *)ptidsl, sizeof(tidsl_t));  
  rc = dslhal_support_resetDsp();
  if(rc)
    {
     dprintf(1, " unable to reset ADSL subsystem \n");
     rc = DSLHAL_ERROR_RESET_DSP;
    }
  rc = dslhal_support_resetDslSubsystem();
  if(rc)
    {
     dprintf(1, " unable to reset ADSL subsystem \n");
     rc = DSLHAL_ERROR_RESET_ADSLSS;
    }
return rc; 
}
  
 
/******************************************************************************************
* FUNCTION NAME:    dslhal_api_getDslHalVersion
*
*******************************************************************************************
* DESCRIPTION: This routine supply DSL Driver version.
*
* INPUT:  tidsl_t * ptidsl
*         void *pVer, DSP Driver Version Pointer
*         
* RETURN:  0 --succeeded
*          1 --Failed
* Note: See verdef_u.h for version structure definition.
*****************************************************************************************/

void dslhal_api_getDslHalVersion(void  *pVer)
{
  dslVer *pVersion;
  pVersion = (dslVer *)pVer;
  pVersion->major = (unsigned char) DSLHAL_VERSION_MAJOR;
  pVersion->minor = (unsigned char) DSLHAL_VERSION_MINOR;
  pVersion->bugfix = (unsigned char) DSLHAL_VERSION_BUGFIX;
  pVersion->buildNum = (unsigned char) DSLHAL_VERSION_BUILDNUM;
  pVersion->timeStamp = (unsigned char) DSLHAL_VERSION_TIMESTAMP;  
}
  
/********************************************************************************************
 * FUNCTION NAME: dslhal_api_pollTrainingStatus()
 *
 *********************************************************************************************
 * DESCRIPTION: code to decode modem status and to start modem training 
 * Input: tidsl_t *ptidsl
 * 
 * Return: modem status   
 *         -1    failed
 *
 ********************************************************************************************/

int dslhal_api_pollTrainingStatus(tidsl_t *ptidsl)
{
  int cmd;
  int tag;
  int parm1,parm2;
  int rc;
  unsigned int failState;
  static unsigned int pollGhsIndex=0;

  /*char *tmp;*/
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
#if SWTC
  DEV_HOST_tcHostCommDef_t          TCHostCommDef;
#endif
  
  dprintf(5,"dslhal_api_pollTrainingStatus\n");
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterface_t *) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface, 
                                     sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
#if SWTC
  dspOamSharedInterface.tcHostComm_p =(DEV_HOST_tcHostCommDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.tcHostComm_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.tcHostComm_p, 
                                    &TCHostCommDef, sizeof(DEV_HOST_tcHostCommDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
#endif
  
  rc = dslhal_support_processTrainingState(ptidsl);
  if(rc)
    {
      dprintf(0,"Error Reading Modem Training State \n");
      return DSLHAL_ERROR_MODEMSTATE;
    }
  rc = dslhal_support_processModemStateBitField(ptidsl);
  if(rc)
    {
      dprintf(0,"Error Reading Modem Training State \n");
      return DSLHAL_ERROR_MODEMSTATE;
    }
  /*
  rc = dslhal_support_readDelineationState(ptidsl);
  if(rc)
    {
      dprintf(0,"Error Reading Delineation State \n");
      return DSLHAL_ERROR_MODEMSTATE;
    }
  */
  while (dslhal_support_readDspMailbox(ptidsl,&cmd, &tag, &parm1, &parm2) == DSLHAL_ERROR_NO_ERRORS )
    {
    dprintf(4,"mailbox message: 0x%x\n", cmd);
    /*    
    for(rc=0;rc<8;rc++)
      {
        dslhal_support_readTextMailbox(ptidsl,&msg1, &msg2);
      }
    */ 

    if (cmd == DSP_IDLE) 
      {
      dprintf(4,"DSP_IDLE\n");
      ptidsl->lConnected=0; 
      hybrid_selected=888;
      /* add code for reload overlay pages */
      if(ptidsl->bAutoRetrain == 0)
       {
         while(ptidsl->bOverlayPageLoaded == 0)
         {
           shim_osClockWait(6400);
         }
         //dslhal_support_restoreTrainingInfo(ptidsl);
         //ptidsl->bOverlayPageLoaded = 1;
       }
      /* command DSP to ACTREQ */
      if(showtimeFlag == TRUE)
        {
           dslhal_api_resetTrainFailureLog(ptidsl);
           dslhal_support_advancedIdleProcessing(ptidsl);
           showtimeFlag = FALSE;
        }
      failState = (unsigned int)parm1;
      if(failState!=0)
        {
          ptidsl->AppData.trainFailStates[ptidsl->AppData.trainFails]=failState;
          ptidsl->AppData.trainFails++;
          if(ptidsl->AppData.trainFails > 30)
             ptidsl->AppData.trainFails=0;
        }
      for(pollGhsIndex=0;pollGhsIndex<10;pollGhsIndex++)
        {
          for(rc=0;rc<62;rc++)
            ptidsl->AppData.dsl_ghsRxBuf[pollGhsIndex][rc]=0;
        }  
      pollGhsIndex=0;
      rc = dslhal_support_writeHostMailbox(ptidsl,HOST_ACTREQ, 0, 0, 0);
      if (rc)
        return DSLHAL_ERROR_MAILBOX_WRITE;
      }

    if(cmd == DSP_ATM_TC_SYNC)
      {
      dprintf(4,"\nTC_SYNC\n");
      showtimeFlag = TRUE;
      ptidsl->lConnected=1; 
      if(ptidsl->bAutoRetrain == 0 && ptidsl->bOverlayPageLoaded == 1)
       {
         dslhal_support_clearTrainingInfo(ptidsl);
         ptidsl->bOverlayPageLoaded = 0;
        }
      }
    if(cmd == DSP_ACTIVE)
      {
      dprintf(4,"DSP_ACTIVE");
      ptidsl->lConnected=0;
      ptidsl->AppData.bState = RSTATE_SHOWTIME;
      dprintf(4,"US Connect Rate: %u \n",ptidsl->AppData.USConRate);
      dprintf(4,"DS Connect Rate: %u \n",ptidsl->AppData.DSConRate);
      }
    if(cmd == DSP_ATM_NO_TC_SYNC)
      {
      dprintf(4,"\nTC_NOSYNC\n");
      ptidsl->lConnected=0; 
      }
    if(cmd == DSP_DGASP)
      {
      dprintf(0,"\n GASP!!! \n");
      }          
    if(cmd == DSP_OVERLAY_END)
       {
         dprintf(4,"Overlay Page Done %d \n",tag);
         rc = dslhal_support_checkOverlayPage(ptidsl,tag);
         if(rc == DSLHAL_ERROR_OVERLAY_CORRUPTED)
          {
            dprintf(0,"Overlay Page: %d CORRUPTED \n",tag);
            return (0-DSLHAL_ERROR_OVERLAY_CORRUPTED);
          }
        }   
    if(cmd == DSP_HYBRID)
       {
         dprintf(2,"Hybrid Metrics Available: %d\n",tag);
         hybrid_selected = tag;
       }
    if(cmd == DSP_DGASP)
      {
      dprintf(0,"\n GASP!!! \n");
      }   
    if(cmd == DSP_XMITBITSWAP)
      {
       dslhal_support_aocBitSwapProcessing(ptidsl,0);
      }
    if(cmd == DSP_RCVBITSWAP)
      {
       dslhal_support_aocBitSwapProcessing(ptidsl,1);
      }
    if(cmd == DSP_GHSMSG)
      {
       dprintf(3,"Ghs Message Received, bytes: %d \n",tag);
       dprintf(3,"Addr: 0x%x\n",dspOamSharedInterface.ghsDspRxBuf_p);
       if(pollGhsIndex > 9)
           pollGhsIndex=0;
       rc = dslhal_support_blockRead((void *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.ghsDspRxBuf_p), &ptidsl->AppData.dsl_ghsRxBuf[pollGhsIndex++][0], tag);
      }
    if(cmd == DSP_CRATES1)
      {
       dprintf(3,"DSP C-Rates1 Data Ready \n");
       rc = dslhal_support_gatherRateMessages(ptidsl);
      }
    if(cmd == DSP_SNR)
      {
       dprintf(3,"DSP SNR Data Ready \n");
       rc = dslhal_support_gatherSnrPerBin(ptidsl,tag);
      }
    if(cmd == DSP_EOC)
      {
       dprintf(3,"DSP_EOC message \n"); 
       rc = dslhal_support_gatherEocMessages(ptidsl,tag,parm1,parm2);
      }
    
    if(cmd == DSP_TRAINING_MSGS)
      {
       dprintf(3,"DSP_TRAINING_MSGS \n"); 
       rc = dslhal_support_gatherAdsl2Messages(ptidsl,tag,parm1,parm2);
      }
    }
  dprintf(6,"dslhal_api_pollTrainingStatus done\n");
  return(ptidsl->AppData.bState);
  
}  /* end of dslhal_api_pollTrainingStatus() */

/********************************************************************************************
* FUNCTION NAME: dslhal_api_handleTrainingInterrupt()
*
*********************************************************************************************
* DESCRIPTION: Code to handle ax5 hardware interrupts
*
* Input: tidsl_t *ptidsl
*        int *pMsg, pointer to returned hardware messages. Each byte represent a messge
*        int *pTag, pointer to returned hardware message tags. Each byte represent a tag.
* Return: 0    success  
*         1    failed
*
********************************************************************************************/
int dslhal_api_handleTrainingInterrupt(tidsl_t *ptidsl, int intrSource)
{
  int cmd;
  int tag;
  int parm1,parm2;
  unsigned int msg1;
  unsigned int msg2;
  int rc;
  unsigned int failState;
  static unsigned int interruptGhsIndex=0;
    /*char *tmp;*/
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
#if SWTC
  DEV_HOST_tcHostCommDef_t          TCHostCommDef;
#endif
  dprintf(6,"dslhal_api_handleTrainingInterrupt\n");
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterface_t *) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface, 
                                     sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
#if SWTC
  dspOamSharedInterface.tcHostComm_p =(DEV_HOST_tcHostCommDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.tcHostComm_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.tcHostComm_p, 
                                    &TCHostCommDef, sizeof(DEV_HOST_tcHostCommDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
#endif
  
  if(intrSource & MASK_BITFIELD_INTERRUPTS)
    {
      dspOamSharedInterface.modemStateBitField_p =(DEV_HOST_modemStateBitField_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemStateBitField_p);
      rc = dslhal_support_processTrainingState(ptidsl);
      if(rc)
       {
        dprintf(0,"Error Reading Modem Training State \n");
        return DSLHAL_ERROR_MODEMSTATE;
        }
       rc = dslhal_support_processModemStateBitField(ptidsl);
       if(rc)
       {
        dprintf(0,"Error Reading Modem Training State \n");
        return DSLHAL_ERROR_MODEMSTATE;
        }
    }
  if(intrSource & MASK_MAILBOX_INTERRUPTS)
    {
      /*
       rc = dslhal_support_readDelineationState(ptidsl); 
       if(rc)
       {
         dprintf(0,"Error Reading Delineation State \n");
         return DSLHAL_ERROR_MODEMSTATE;
        }
      */
       while (dslhal_support_readDspMailbox(ptidsl,&cmd, &tag, &parm1, &parm2) == DSLHAL_ERROR_NO_ERRORS )
       {
         dprintf(4,"mailbox message: 0x%x\n", cmd);
         /*    
         for(rc=0;rc<8;rc++)
         {
           dslhal_support_readTextMailbox(ptidsl,&msg1, &msg2);
         }
          */ 
         if (cmd == DSP_IDLE) 
         {
           dprintf(4,"DSP_IDLE\n");
           ptidsl->lConnected=0;
           hybrid_selected=888;
           if(showtimeFlag == TRUE)
           {
             dslhal_api_resetTrainFailureLog(ptidsl);
             dslhal_support_advancedIdleProcessing(ptidsl);
             showtimeFlag = FALSE;
            }
           failState = (unsigned int)parm1;
           if(failState!=0)
           {
             ptidsl->AppData.trainFailStates[ptidsl->AppData.trainFails]=failState;
             ptidsl->AppData.trainFails++;
             if(ptidsl->AppData.trainFails > 30)
               ptidsl->AppData.trainFails=0;
            }
           for(interruptGhsIndex=0;interruptGhsIndex<10;interruptGhsIndex++)
             {
               for(rc=0;rc<62;rc++)
                 ptidsl->AppData.dsl_ghsRxBuf[interruptGhsIndex][rc]=0;
             }  
           interruptGhsIndex=0;

           /* add code for reload overlay pages */
           if(ptidsl->bAutoRetrain == 0 && ptidsl->bOverlayPageLoaded == 0)
             {
             dslhal_support_restoreTrainingInfo(ptidsl);
             ptidsl->bOverlayPageLoaded = 1;
             }
           /* command DSP to ACTREQ */
           rc = dslhal_support_writeHostMailbox(ptidsl, HOST_ACTREQ, 0, 0, 0);
           if (rc)
             return DSLHAL_ERROR_MAILBOX_WRITE;
         }
         if(cmd == DSP_ATM_TC_SYNC)
           {
           dprintf(4,"\nTC_SYNC\n");
           showtimeFlag = TRUE;
           ptidsl->lConnected=1;
           if(ptidsl->bAutoRetrain == 0 && ptidsl->bOverlayPageLoaded == 1)
             {
             dslhal_support_clearTrainingInfo(ptidsl);
             ptidsl->bOverlayPageLoaded = 0;
             }
           }
         if(cmd == DSP_ACTIVE)
           {
             ptidsl->lConnected=0;
             ptidsl->AppData.bState = RSTATE_SHOWTIME;
             dprintf(4,"DSP_ACTIVE");
             dprintf(4,"US Connect Rate: %u \n",ptidsl->AppData.USConRate);
             dprintf(4,"DS Connect Rate: %u \n",ptidsl->AppData.DSConRate);
           }
         if(cmd == DSP_ATM_NO_TC_SYNC)
           {
             dprintf(4,"\nTC_NOSYNC\n");
             ptidsl->lConnected=0;
             /* add code for reload overlay pages */
           }
         if(cmd == DSP_OVERLAY_END)
           {
             dprintf(4,"Overlay Page Done %d \n",tag);
             rc = dslhal_support_checkOverlayPage(ptidsl,tag);
             if(rc == DSLHAL_ERROR_OVERLAY_CORRUPTED)
               {
                 dprintf(4,"Overlay Page: %d CORRUPTED \n",tag);
                 return(0-DSLHAL_ERROR_OVERLAY_CORRUPTED);
               }
           }
         if(cmd == DSP_HYBRID)
           {
             dprintf(2,"Hybrid Metrics Available\n");
             hybrid_selected = tag;
           }
         if(cmd == DSP_XMITBITSWAP)
           {
            rc = dslhal_support_aocBitSwapProcessing(ptidsl,0);
           }
         if(cmd == DSP_RCVBITSWAP)
           {
            rc = dslhal_support_aocBitSwapProcessing(ptidsl,1);
           }
         if(cmd == DSP_GHSMSG)
           {
            dprintf(3,"Ghs Message Received, bytes: %d \n",tag);
            dprintf(3,"Addr: 0x%x\n",dspOamSharedInterface.ghsDspRxBuf_p);
            if(interruptGhsIndex > 9)
              interruptGhsIndex=0;
            rc = dslhal_support_blockRead((void *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.ghsDspRxBuf_p), &ptidsl->AppData.dsl_ghsRxBuf[interruptGhsIndex++][0], tag);
            }
         if(cmd == DSP_CRATES1)
          {
           dprintf(3,"DSP C-Rates1 Data Ready \n");
           rc = dslhal_support_gatherRateMessages(ptidsl);
          }
         if(cmd == DSP_SNR)
          {
           dprintf(3,"DSP SNR Data Ready \n");
           rc = dslhal_support_gatherSnrPerBin(ptidsl,tag);
          }
         if(cmd == DSP_EOC)
          {
           dprintf(3,"DSP_EOC message \n");
           rc = dslhal_support_gatherEocMessages(ptidsl,tag,parm1,parm2);
          }
         if(cmd == DSP_TRAINING_MSGS)
          {
           dprintf(3,"DSP_TRAINING_MSGS \n"); 
           rc = dslhal_support_gatherAdsl2Messages(ptidsl,tag,parm1,parm2);
          }
        }       
     
       dslhal_support_readTextMailbox(ptidsl,&msg1, &msg2);
       dprintf(5,"Text Message Part1: 0x%x \t Text Message Part2: 0x%x \n",msg1,msg2);
    }     
  dprintf(6,"dslhal_api_handleTrainingInterrupt done\n");
  return(ptidsl->AppData.bState);
} /* end of dslhal_api_handleTrainingInterrupt() */

  


/******************************************************************************************    
 * FUNCTION NAME:    dslhal_api_dslRetrain(tidsl_t *ptidsl)    
 *    
 *******************************************************************************************    
 * DESCRIPTION: This fuction sends CMD_ACTREQ to the DSP to issue a retrain    
 *    
 * INPUT:  PITIDSLHW_T *ptidsl    
 *           
 * RETURN: 0 SUCCESS    
 *         1 FAILED    
 *      
 *****************************************************************************************/    
unsigned int dslhal_api_dslRetrain(tidsl_t *ptidsl)
{
   int rc; 
   
   dprintf(5, "dslhal_cfg_dslRetrain \n"); 
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_QUIET, 0, 0, 0); 
   if(rc)
    {  
      dprintf(1,"dslhal_cfg_dslRetrain failed\n");  
      return DSLHAL_ERROR_CTRL_API_FAILURE;  
    }  
    return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************    
 * FUNCTION NAME:    dslhal_api_sendQuiet(tidsl_t *ptidsl)    
 *    
 *******************************************************************************************    
 * DESCRIPTION: This fuction sends the CMD_QUIET message to the DSP    
 *    
 * INPUT:  PITIDSLHW_T *ptidsl    
 *           
 * RETURN: 0 SUCCESS    
 *         1 FAILED    
 *      
 *****************************************************************************************/    
unsigned int dslhal_api_sendQuiet(tidsl_t *ptidsl)
{
   int rc; 
   
   dprintf(5, "dslhal_api_sendQuiet\n"); 
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_QUIET, 0, 0, 0); 
   if(rc)
    {  
      dprintf(1,"dslhal_api_sendQuiet failed\n");  
      return DSLHAL_ERROR_CTRL_API_FAILURE;    
    }  
    return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************     
 * FUNCTION NAME:    dslhal_api_sendIdle(tidsl_t *ptidsl)     
 *     
 *******************************************************************************************     
 * DESCRIPTION: This fuction sends the CMD_IDLE message to the DSP     
 *     
 * INPUT:  PITIDSLHW_T *ptidsl     
 *            
 * RETURN: 0 SUCCESS     
 *         1 FAILED     
 *       
 *****************************************************************************************/     
unsigned int dslhal_api_sendIdle(tidsl_t *ptidsl) 
{ 
   int rc;  
    
   dprintf(5, "dslhal_api_sendIdle\n");  
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_IDLE, 0, 0, 0);  
   if(rc) 
     {   
      dprintf(1,"dslhal_api_sendIdle failed\n");   
      return DSLHAL_ERROR_CTRL_API_FAILURE;    
     }   
    return DSLHAL_ERROR_NO_ERRORS; 
} 

/******************************************************************************************    
 * FUNCTION NAME:    dslhal_api_sendDgasp(tidsl_t *ptidsl)    
 *    
 *******************************************************************************************    
 * DESCRIPTION: This fuction sends the HOST_DGASP message to the DSP    
 *    
 * INPUT:  PITIDSLHW_T *ptidsl    
 *           
 * RETURN: 0 SUCCESS    
 *         1 FAILED    
 *      
 *****************************************************************************************/    
unsigned int dslhal_api_sendDgasp(tidsl_t *ptidsl)
{
   int rc; 
   
   dprintf(5, "dslhal_api_sendDgasp\n"); 
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DGASP, 0, 0, 0); 
   if(rc)
    {  
      dprintf(1,"dslhal_api_sendDgasp failed\n");  
      return DSLHAL_ERROR_CTRL_API_FAILURE;    
    }  
    return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setTrainingMode(tidsl_t *ptidsl,unsigned int trainmode)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Desired Training Mode {None/Multimode/G.dmt/lite
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int trainmode :Should be between 0 and 4; 0:No Mode 1:Multimode
*                                 2: T1.413, 3:G.dmt, 4: G.lite
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_api_setTrainingMode(tidsl_t *ptidsl,unsigned int trainmode)
{
   DEV_HOST_oamWrNegoParaDef_t NegoPara;
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc;
   dprintf(5," dslhal_api_setTrainingMode()\n");
   if(trainmode>255)
     {
       dprintf(3,"Invalid Value for Desired Training Mode (must be <255)\n");
       return DSLHAL_ERROR_INVALID_PARAM;
     }

   if(!ptidsl)
    {
      dprintf(3, "Error: PTIDSL pointer invalid\n");
      return DSLHAL_ERROR_INVALID_PARAM;
    }

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface, 
                                     sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);

   rc = dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&NegoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  /* Enum Translation to maintain backwards compatibility for train modes */
  if(trainmode == DSLTRAIN_MULTI_MODE)
    trainmode = MULTI_MODE;
  if(trainmode == DSLTRAIN_T1413_MODE)
    trainmode = T1413_MODE;
  if(trainmode == DSLTRAIN_GDMT_MODE)
    trainmode = GDMT_MODE;
   
  NegoPara.stdMode = trainmode;
  dprintf(5,"Train Mode: 0x%x\n",trainmode); 
  rc = dslhal_support_blockWrite(&NegoPara,(PVOID)dspOamSharedInterface.oamWriteNegoParams_p, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
 
  dprintf(5," dslhal_api_setTrainingMode() Done\n");
  return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_api_getDspVersion
*
*******************************************************************************************
* DESCRIPTION: This routine supply AX5 daugther card DSP version.
*
* INPUT:  tidsl_t * ptidsl
*         void *pVer, DSP version struct is returned starting at this pointer
*         
* RETURN:  0 --succeeded
*          1 --Failed
* Note: See verdef_u.h for version structure definition.
*****************************************************************************************/
int dslhal_api_getDspVersion(tidsl_t *ptidsl, void  *pVer)
{
  /*  DEV_HOST_dspVersionDef_t  dspVersion; */
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc;
   dprintf(5, "dslhal_api_getDspVersion\n");
   if(!ptidsl)
     {
       dprintf(3, "Error: PTIDSL pointer invalid\n");
       return DSLHAL_ERROR_INVALID_PARAM;
     }
   if(!pVer)
     return DSLHAL_ERROR_INVALID_PARAM;
   
   *(unsigned int *) pVer = 0;
 
   pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   dspOamSharedInterface.datapumpVersion_p = (DEV_HOST_dspVersionDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.datapumpVersion_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.datapumpVersion_p,
                                      pVer, sizeof(dspVer));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
  pVer = (DEV_HOST_dspVersionDef_t *)(dslhal_support_byteSwap32((unsigned int)pVer)); 
  return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_gatherStatistics()
*
*********************************************************************************************
* DESCRIPTION: Read statistical infromation from ax5 modem daugter card. 
* Input: tidsl_t *ptidsl
* 
* Return: 0    success
*         1    failed
*
********************************************************************************************/
void dslhal_api_gatherStatistics(tidsl_t * ptidsl)
{
  int rc,optIdxU,optIdxD,i; 
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  DEV_HOST_dspWrNegoParaDef_t   rateparms;
  DEV_HOST_oamWrNegoParaDef_t   configParms;
  DEV_HOST_modemStatsDef_t      StatisticsDef;
  DEV_HOST_errorStats_t usIntlvError, usFastError, dsIntlvError, dsFastError;
  DEV_HOST_atmStats_t           atmStats;
  DEV_HOST_usAtmStats_t         usAtmStats0, usAtmStats1;
  DEV_HOST_dsAtmStats_t         dsAtmStats0,dsAtmStats1;
  DEV_HOST_dspWrSuperFrameCntDef_t   SuperFrameCnt;
  DEV_HOST_msg_t                atuc_msg, aturMsg;
  DEV_HOST_eocVarDef_t          eocVar;
  DEV_HOST_dspWrSharedTables_t  sharedTables;
  DEV_HOST_phyPerf_t            phyPerf;
  unsigned char                 usBits[64],dsBits[256];
  unsigned char                 dsPowerCutBack;
  int                           usNumLoadedTones=0, dsNumLoadedTones=0;

  dprintf(5, "dslhal_api_gatherStatistics\n");
  
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
  
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface, 
                                     sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }
  if(!ptidsl->bStatisticsInitialized && ptidsl->lConnected == LINE_CONNECTED)
  {
    dslhal_api_initStatistics(ptidsl);
    ptidsl->bStatisticsInitialized = TRUE;
  }

  dspOamSharedInterface.dspWriteNegoParams_p = (DEV_HOST_dspWrNegoParaDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspWriteNegoParams_p, 
                               &rateparms, sizeof(DEV_HOST_dspWrNegoParaDef_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }
  if(!rc) 
    {    
      /* trained mode */
    ptidsl->AppData.dsl_modulation = (unsigned int)rateparms.trainMode;
    if(rateparms.trainMode == T1413_MODE)
      ptidsl->AppData.dsl_modulation = DSLTRAIN_T1413_MODE;
    if(rateparms.trainMode == GDMT_MODE)
      ptidsl->AppData.dsl_modulation = DSLTRAIN_GDMT_MODE;
      /* rate */
    /*    shim_osMoveMemory((void *)ptidsl->AppData.bCRates1, (void *)rateparms.cRates1, 120); */
    ptidsl->AppData.bCRates2 = rateparms.cRates2;
    /*    shim_osMoveMemory((void *)ptidsl->AppData.bRRates1, (void *)rateparms.rRates1, 44); */
    ptidsl->AppData.bRRates2 = rateparms.rRates2;
    shim_osMoveMemory((void *)ptidsl->AppData.bCMsgs1, (void *)rateparms.cMsgs1, 6);
    shim_osMoveMemory((void *)ptidsl->AppData.bCMsgs2, (void *)rateparms.cMsgs2, 4);
    shim_osMoveMemory((void *)ptidsl->AppData.bRMsgs2, (void *)rateparms.rMsgs2, 4);
    ptidsl->AppData.atucVendorId = (unsigned int)rateparms.atucVendorId;
    ptidsl->AppData.lineLength = (unsigned int)dslhal_support_byteSwap16((unsigned short)rateparms.lineLength);
    ptidsl->AppData.atucRevisionNum = (unsigned int)rateparms.atucGhsRevisionNum;         
    ptidsl->AppData.usLineAttn = (ptidsl->AppData.bCMsgs2[3] >>2)&0x003f;
    ptidsl->AppData.usMargin = (ptidsl->AppData.bCMsgs2[2])&0x001f;
    
    if((rateparms.cRates2 & 0x0f) == 0x01) 
      optIdxU = 0;
    else if((rateparms.cRates2 & 0x0f) == 0x02)  
      optIdxU = 1;
    else if((rateparms.cRates2 & 0x0f) == 0x04) 
      optIdxU = 2;
    else if((rateparms.cRates2 & 0x0f) == 0x08)  
      optIdxU = 3;
    else 
      optIdxU = -1;
    
    dprintf(5, "optIdxU=%d\n", optIdxU);
   
    /* Obtain the US Rates using Opt# and CRates1 Table */
    /* Rate(US) = [Bf(LS0) + Bi(LS0)]*32                */
    if(ptidsl->AppData.dsl_modulation <= DSLTRAIN_GLITE_MODE)
      ptidsl->AppData.USConRate = ((rateparms.cRates1[optIdxU][CRATES1_BF_LS0] + rateparms.cRates1[optIdxU][CRATES1_BI_LS0]) * 32);
    else
      ptidsl->AppData.USConRate =  32 * dslhal_support_byteSwap16((unsigned short)rateparms.adsl2USRate);

    ptidsl->AppData.USPeakCellRate = ptidsl->AppData.USConRate; 
    
    if(((rateparms.cRates2 >> 4) & 0x0f) == 0x01) 
      optIdxD = 0;
    else if(((rateparms.cRates2 >> 4) & 0x0f) == 0x02)  
      optIdxD = 1;
    else if(((rateparms.cRates2 >> 4) & 0x0f) == 0x04) 
      optIdxD = 2;
    else if(((rateparms.cRates2 >> 4) & 0x0f) == 0x08)  
      optIdxD = 3;
    else 
      optIdxD = -1;
    /* Obtain the DS Rates using Opt# and CRates1 Table */
    /* Rate(DS) = [Bf(AS0) + Bi(AS0)]*32                */
    if(ptidsl->AppData.dsl_modulation <= DSLTRAIN_GLITE_MODE)
      ptidsl->AppData.DSConRate =  (((rateparms.cRates1[optIdxD][CRATES1_BF_AS0]|((rateparms.cRates1[optIdxD][CRATES1_BF_DSRS]&0x80)<<1))+ (rateparms.cRates1[optIdxD][CRATES1_BI_AS0]|((rateparms.cRates1[optIdxD][CRATES1_BI_DSRS]&0x80)<<1)))* 32);
    else
      ptidsl->AppData.DSConRate = dslhal_support_byteSwap16((unsigned short)rateparms.adsl2DSRate);

    dprintf(5, "ptidsl->AppData.wDSConRate=%d\n", ptidsl->AppData.DSConRate);
    /* Determine which Path has Modem Trained with */
    if((rateparms.cRates1[optIdxU][CRATES1_BF_LS0]) && (rateparms.cRates1[optIdxD][CRATES1_BF_AS0]))
      ptidsl->AppData.TrainedPath = FAST_PATH;
    else
      ptidsl->AppData.TrainedPath = INTERLEAVED_PATH;

    /* Set the mode in which the modem is trained */
    ptidsl->AppData.TrainedMode = (unsigned int)rateparms.trainMode;
    if(rateparms.trainMode == T1413_MODE)
      ptidsl->AppData.TrainedMode = DSLTRAIN_T1413_MODE;
    if(rateparms.trainMode == GDMT_MODE)
      ptidsl->AppData.TrainedMode = DSLTRAIN_GDMT_MODE;

    if(ptidsl->AppData.TrainedPath == FAST_PATH)
       ptidsl->AppData.dsFastParityBytesPerSymbol = (rateparms.cRates1[optIdxU][CRATES1_BF_DSRS]&0x1f);
    else
       ptidsl->AppData.dsIntlvParityBytesPerSymbol = (rateparms.cRates1[optIdxU][CRATES1_BI_DSRS]&0x1f);
    ptidsl->AppData.dsSymbolsPerCodeWord = (rateparms.cRates1[optIdxU][CRATES1_BFI_DSS]&0x1f);
    ptidsl->AppData.dsInterleaverDepth = ((rateparms.cRates1[optIdxU][CRATES1_BFI_DSI])|((rateparms.cRates1[optIdxU][CRATES1_BFI_DSS]&0xc0)<<2));

    if(ptidsl->AppData.TrainedPath == FAST_PATH)
       ptidsl->AppData.usFastParityBytesPerSymbol = (rateparms.cRates1[optIdxU][CRATES1_BF_USRS]&0x1f);
    else
       ptidsl->AppData.usIntlvParityBytesPerSymbol = (rateparms.cRates1[optIdxU][CRATES1_BI_USRS]&0x1f);
    ptidsl->AppData.usSymbolsPerCodeWord = (rateparms.cRates1[optIdxU][CRATES1_BFI_USS]&0x1f);
    ptidsl->AppData.usInterleaverDepth = ((rateparms.cRates1[optIdxU][CRATES1_BFI_USI])|((rateparms.cRates1[optIdxU][CRATES1_BFI_USS]&0xc0)<<2));
    }

  dspOamSharedInterface.modemStats_p = (DEV_HOST_modemStatsDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemStats_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemStats_p,&StatisticsDef, sizeof(DEV_HOST_modemStatsDef_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }
  /* Populate the Error Structure Variables */

  /* US Interleave Path Error Statistics */

  /* Change the endianness of the Pointer */
  StatisticsDef.usErrorStatsIntlv_p = (DEV_HOST_errorStats_t *) dslhal_support_byteSwap32((unsigned int)StatisticsDef.usErrorStatsIntlv_p);

  rc = dslhal_support_blockRead((PVOID)StatisticsDef.usErrorStatsIntlv_p,&usIntlvError, (sizeof(DEV_HOST_errorStats_t)));
  
  if (rc) 
    return;

  /* DS Interleave Path Error Statistics */

  /* Change the endianness of the Pointer */
  StatisticsDef.dsErrorStatsIntlv_p = (DEV_HOST_errorStats_t *) dslhal_support_byteSwap32((unsigned int)StatisticsDef.dsErrorStatsIntlv_p);

  rc = dslhal_support_blockRead((PVOID)StatisticsDef.dsErrorStatsIntlv_p,&dsIntlvError, (sizeof(DEV_HOST_errorStats_t)));
  
  if (rc) 
    return;

  /* US Fast Path Error Statistics */

  /* Change the endianness of the Pointer */
  StatisticsDef.usErrorStatsFast_p = (DEV_HOST_errorStats_t *) dslhal_support_byteSwap32((unsigned int)StatisticsDef.usErrorStatsFast_p);

  rc = dslhal_support_blockRead((PVOID)StatisticsDef.usErrorStatsFast_p,&usFastError, (sizeof(DEV_HOST_errorStats_t)));
  
  if (rc) 
    return;

  /* DS Fast Path Error Statistics */

  /* Change the endianness of the Pointer */
  StatisticsDef.dsErrorStatsFast_p = (DEV_HOST_errorStats_t *) dslhal_support_byteSwap32((unsigned int)StatisticsDef.dsErrorStatsFast_p);

  rc = dslhal_support_blockRead((PVOID)StatisticsDef.dsErrorStatsFast_p,&dsFastError, (sizeof(DEV_HOST_errorStats_t)));
  
  if (rc) 
    return;
  if(!rc) 
  {
    if(ptidsl->AppData.bState > 2)
    {
      /* Get CRC Errors Stats for both US and DS */
      ptidsl->AppData.dsICRC_errors   = dslhal_support_byteSwap32((unsigned int)dsIntlvError.crcErrors);
      ptidsl->AppData.dsFCRC_errors   = dslhal_support_byteSwap32((unsigned int)dsFastError.crcErrors);
      ptidsl->AppData.usICRC_errors   = dslhal_support_byteSwap32((unsigned int)usIntlvError.crcErrors);
      ptidsl->AppData.usFCRC_errors   = dslhal_support_byteSwap32((unsigned int)usFastError.crcErrors);
      /* Get FEC Errors Stats for both US and DS */
      ptidsl->AppData.dsIFEC_errors   = dslhal_support_byteSwap32((unsigned int)dsIntlvError.fecErrors);
      ptidsl->AppData.dsFFEC_errors   = dslhal_support_byteSwap32((unsigned int)dsFastError.fecErrors);
      ptidsl->AppData.usIFEC_errors   = dslhal_support_byteSwap32((unsigned int)usIntlvError.fecErrors);
      ptidsl->AppData.usFFEC_errors   = dslhal_support_byteSwap32((unsigned int)usFastError.fecErrors);
      /* Get NCD Errors Stats for both US and DS */
      ptidsl->AppData.dsINCD_error   = dslhal_support_byteSwap32((unsigned int)dsIntlvError.ncdError);
      ptidsl->AppData.dsFNCD_error   = dslhal_support_byteSwap32((unsigned int)dsFastError.ncdError);
      ptidsl->AppData.usINCD_error   = dslhal_support_byteSwap32((unsigned int)usIntlvError.ncdError);
      ptidsl->AppData.usFNCD_error   = dslhal_support_byteSwap32((unsigned int)usFastError.ncdError);
      /* Get LCD Errors Stats for both US and DS */
      ptidsl->AppData.dsILCD_errors   = dslhal_support_byteSwap32((unsigned int)dsIntlvError.lcdErrors);
      ptidsl->AppData.dsFLCD_errors   = dslhal_support_byteSwap32((unsigned int)dsFastError.lcdErrors);
      ptidsl->AppData.usILCD_errors   = dslhal_support_byteSwap32((unsigned int)usIntlvError.lcdErrors);
      ptidsl->AppData.usFLCD_errors   = dslhal_support_byteSwap32((unsigned int)usFastError.lcdErrors);
      /*Get HEC Errors Stats for both US and DS */
      ptidsl->AppData.dsIHEC_errors   = dslhal_support_byteSwap32((unsigned int)dsIntlvError.hecErrors);
      ptidsl->AppData.dsFHEC_errors   = dslhal_support_byteSwap32((unsigned int)dsFastError.hecErrors);
      ptidsl->AppData.usIHEC_errors   = dslhal_support_byteSwap32((unsigned int)usIntlvError.hecErrors);
      ptidsl->AppData.usFHEC_errors   = dslhal_support_byteSwap32((unsigned int)usFastError.hecErrors);

      /* Get LOS and SEF error Stats */
      ptidsl->AppData.LOS_errors      = dslhal_support_byteSwap32(StatisticsDef.losErrors);            
      ptidsl->AppData.SEF_errors      = dslhal_support_byteSwap32(StatisticsDef.sefErrors);
      ptidsl->AppData.coLosErrors = dslhal_support_byteSwap32(StatisticsDef.farEndLosErrors);
      ptidsl->AppData.coRdiErrors = dslhal_support_byteSwap32(StatisticsDef.farEndRdiErrors);
  
  dspOamSharedInterface.atmStats_p = (DEV_HOST_atmStats_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.atmStats_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.atmStats_p,&atmStats, sizeof(DEV_HOST_atmStats_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }

  /* Populate the US/DS ATM Stats Variables */

  /* US ATM Statistics */

  /* Change the endianness of the Pointer */
  atmStats.us0_p = (DEV_HOST_usAtmStats_t *) dslhal_support_byteSwap32((unsigned int)atmStats.us0_p);

  rc = dslhal_support_blockRead((PVOID)atmStats.us0_p,&usAtmStats0, (sizeof(DEV_HOST_usAtmStats_t)));
  
  if (rc) 
    return;

  atmStats.us1_p = (DEV_HOST_usAtmStats_t *) dslhal_support_byteSwap32((unsigned int)atmStats.us1_p);

  rc = dslhal_support_blockRead((PVOID)atmStats.us1_p,&usAtmStats1, (sizeof(DEV_HOST_usAtmStats_t)));
  
  if (rc) 
    return;

 /* DS ATM Statistics */

  /* Change the endianness of the Pointer */
  atmStats.ds0_p = (DEV_HOST_dsAtmStats_t *) dslhal_support_byteSwap32((unsigned int)atmStats.ds0_p);

  rc = dslhal_support_blockRead((PVOID)atmStats.ds0_p,&dsAtmStats0, (sizeof(DEV_HOST_dsAtmStats_t)));
  
  if (rc) 
    return;
  atmStats.ds1_p = (DEV_HOST_dsAtmStats_t *) dslhal_support_byteSwap32((unsigned int)atmStats.ds1_p);

  rc = dslhal_support_blockRead((PVOID)atmStats.ds1_p,&dsAtmStats1, (sizeof(DEV_HOST_dsAtmStats_t)));
  
  if (rc) 
    return;

      /* Get ATM Stats for both US and DS for Channel 0*/
      ptidsl->AppData.usAtm_count[0]     = dslhal_support_byteSwap32(usAtmStats0.goodCount);
      ptidsl->AppData.usIdle_count[0]    = dslhal_support_byteSwap32(usAtmStats0.idleCount);
#if SWTC
      ptidsl->AppData.usPdu_count[0]     = dslhal_support_byteSwap32(usAtmStats0.pduCount);
#endif
      ptidsl->AppData.dsGood_count[0]    = dslhal_support_byteSwap32(dsAtmStats0.goodCount);
      ptidsl->AppData.dsIdle_count[0]    = dslhal_support_byteSwap32(dsAtmStats0.idleCount);
#if SWTC
      ptidsl->AppData.dsPdu_count[0]     = dslhal_support_byteSwap32(dsAtmStats0.pduCount);
#endif
      ptidsl->AppData.dsBadHec_count[0]  = dslhal_support_byteSwap32((dsAtmStats0.badHecCount));
      ptidsl->AppData.dsOVFDrop_count[0] = dslhal_support_byteSwap32((dsAtmStats0.ovflwDropCount));
      /* Get ATM Stats for both US and DS for Channel 1*/
      ptidsl->AppData.usAtm_count[1]     = dslhal_support_byteSwap32(usAtmStats1.goodCount);
      ptidsl->AppData.usIdle_count[1]    = dslhal_support_byteSwap32(usAtmStats1.idleCount);
#if SWTC
      ptidsl->AppData.usPdu_count[1]     = dslhal_support_byteSwap32(usAtmStats1.pduCount);
#endif
      ptidsl->AppData.dsGood_count[1]    = dslhal_support_byteSwap32(dsAtmStats1.goodCount);
      ptidsl->AppData.dsIdle_count[1]    = dslhal_support_byteSwap32(dsAtmStats1.idleCount);
#if SWTC
      ptidsl->AppData.dsPdu_count[1]     = dslhal_support_byteSwap32(dsAtmStats1.pduCount);
#endif
      ptidsl->AppData.dsBadHec_count[1]  = dslhal_support_byteSwap32((dsAtmStats1.badHecCount));
      ptidsl->AppData.dsOVFDrop_count[1] = dslhal_support_byteSwap32((dsAtmStats1.ovflwDropCount));

    /* Determine the US and DS Superframe Count */

  dspOamSharedInterface.dspWriteSuperFrameCnt_p = (DEV_HOST_dspWrSuperFrameCntDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspWriteSuperFrameCnt_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspWriteSuperFrameCnt_p,&SuperFrameCnt, sizeof(DEV_HOST_dspWrSuperFrameCntDef_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }
      ptidsl->AppData.usSuperFrmCnt   = dslhal_support_byteSwap32(SuperFrameCnt.wSuperFrameCntUstrm);
      ptidsl->AppData.dsSuperFrmCnt   = dslhal_support_byteSwap32(SuperFrameCnt.wSuperFrameCntDstrm);
      
      /* Determine Frame Mode and Max Frame Mode */

  dspOamSharedInterface.atucMsg_p = (DEV_HOST_msg_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.atucMsg_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.atucMsg_p,&atuc_msg, sizeof(DEV_HOST_msg_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }

  ptidsl->AppData.FrmMode         = (unsigned int)atuc_msg.framingMode;
  ptidsl->AppData.MaxFrmMode      = (unsigned int)atuc_msg.maxFrameMode;

  /* Determine Gross Gain */

  dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,&aturMsg, sizeof(DEV_HOST_msg_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }
  ptidsl->AppData.grossGain = (unsigned int)aturMsg.grossGain;

      /* Determine DS Line Attenuation & Margin */
  dspOamSharedInterface.eocVar_p = (DEV_HOST_eocVarDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.eocVar_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.eocVar_p,&eocVar, sizeof(DEV_HOST_eocVarDef_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }
      ptidsl->AppData.dsLineAttn      = (unsigned int)eocVar.lineAtten;
      ptidsl->AppData.dsMargin        = (unsigned int)eocVar.dsMargin;
    }
  } 

  /* Read in the Shared Tables structure */      
  dspOamSharedInterface.dspWrSharedTables_p = (DEV_HOST_dspWrSharedTables_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspWrSharedTables_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspWrSharedTables_p,&sharedTables, sizeof(DEV_HOST_dspWrSharedTables_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }

  /* Read the ATU-R Bits and Gains Table */
  sharedTables.aturBng_p = (unsigned char *)dslhal_support_byteSwap32((unsigned int)sharedTables.aturBng_p);
  rc = dslhal_support_blockRead((PVOID)sharedTables.aturBng_p,ptidsl->AppData.rBng,255*2*sizeof(unsigned char));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }
  /* Read the ATU-C Bits and Gains Table */
  sharedTables.atucBng_p = (unsigned char *)dslhal_support_byteSwap32((unsigned int)sharedTables.atucBng_p);
  if(ptidsl->netService == 2) /* for Annex_B */
    {
      rc = dslhal_support_blockRead((PVOID)sharedTables.atucBng_p,ptidsl->AppData.cBng,126*sizeof(unsigned char));
      if (rc)
       {
        dprintf(1,"dslhal_support_blockRead failed\n");
        return;
        }
      for(i=0;i<US_BNG_LENGTH*2;i++)
         usBits[i]=0;
      for(i=1;i<US_BNG_LENGTH*2;i++)
       {
        usBits[i]=((ptidsl->AppData.cBng[(i-1)*2])&0xf);
        dprintf(5,"Bit #%d : 0x%x\n",i,usBits[i]);
        }
      for(i=1;i<US_BNG_LENGTH*2;i++)
       {
        if(usBits[i])
        usNumLoadedTones++;
        }
    }
  else
    {
      rc = dslhal_support_blockRead((PVOID)sharedTables.atucBng_p,ptidsl->AppData.cBng,62*sizeof(unsigned char));
      if (rc)
       {
        dprintf(1,"dslhal_support_blockRead failed\n");
        return;
        }
      for(i=0;i<US_BNG_LENGTH;i++)
         usBits[i]=0;
      for(i=1;i<US_BNG_LENGTH;i++)
       {
        usBits[i]=((ptidsl->AppData.cBng[(i-1)*2])&0xf);
        dprintf(5,"Bit #%d : 0x%x\n",i,usBits[i]);
        }
      for(i=1;i<US_BNG_LENGTH;i++)
       {
        if(usBits[i])
        usNumLoadedTones++;
        }
    }

  /* Determine Number U/S of Loaded Tones */

  /* U/S Power Computation */
  dspOamSharedInterface.phyPerf_p = (DEV_HOST_phyPerf_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.phyPerf_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.phyPerf_p,
                                &phyPerf, sizeof(DEV_HOST_phyPerf_t));
  if (rc)
    {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return;
    }  
  ptidsl->AppData.currentHybridNum = phyPerf.currentHybridNumUsed;
  phyPerf.usAvgGain = dslhal_support_byteSwap32(phyPerf.usAvgGain);
  ptidsl->AppData.usTxPower = LOG43125 + phyPerf.usAvgGain + (256*US_NOMINAL_POWER)+log10[usNumLoadedTones-1];
  dprintf(7,"Avg Gain: 0x%x  usNumLoadedTones: 0x%x    log: 0x%x\n",phyPerf.usAvgGain, usNumLoadedTones, log10[usNumLoadedTones-1]);
  
  /* Determine Number D/S of Loaded Tones */
  dsBits[0]=0;
  for(i=0;i<DS_BNG_LENGTH;i++)
    {
     dsBits[i]=0;
     /* ptidsl->AppData.rBng[i-1]=dslhal_support_byteSwap32((unsigned int)ptidsl->AppData.rBng[i-1]);*/
    }
  for(i=1;i<DS_BNG_LENGTH;i++)
    {
     dsBits[i]=((ptidsl->AppData.rBng[(i-1)*2])&0xf);
     dprintf(5,"Bit #%d : 0x%x\n",i,dsBits[i]);
    }
  for(i=1;i<DS_BNG_LENGTH;i++)
    {
     if(dsBits[i])
       dsNumLoadedTones++;
    }
  /* D/S Power Computation */
  dspOamSharedInterface.phyPerf_p = (DEV_HOST_phyPerf_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.phyPerf_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.phyPerf_p,
                                &phyPerf, sizeof(DEV_HOST_phyPerf_t));
  if (rc)
    {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return;
    }  
  /* D/S Power Cutback */
  dsPowerCutBack = (unsigned char)((((ptidsl->AppData.bCMsgs1[0]) >>6) &0x3)+(((ptidsl->AppData.bCMsgs1[1]) &0x1) <<2));
  phyPerf.dsAvgGain = dslhal_support_byteSwap32(phyPerf.dsAvgGain);
  ptidsl->AppData.dsTxPower = LOG43125 + phyPerf.dsAvgGain + (256*((2*(dsPowerCutBack-1))-52)) + log10[dsNumLoadedTones-1];
  dprintf(7,"Avg Gain: %d  dsNumLoadedTones: %d  log: %d  pcb: %d \n",phyPerf.dsAvgGain, dsNumLoadedTones, log10[dsNumLoadedTones-1], dsPowerCutBack);
  /* ds bit allocation */
  sharedTables.bitAllocTblDstrm_p = (unsigned char *)dslhal_support_byteSwap32((unsigned int)sharedTables.bitAllocTblDstrm_p);
  rc = dslhal_support_blockRead((PVOID)sharedTables.bitAllocTblDstrm_p,ptidsl->AppData.BitAllocTblDstrm, 256*sizeof(unsigned char));
  if(rc)
    {
      dprintf(1,"dslhal_support_blockRead failed \n");
      return;
    }

    /* us bit allocation */
  sharedTables.bitAllocTblUstrm_p = (unsigned char *)dslhal_support_byteSwap32((unsigned int)sharedTables.bitAllocTblUstrm_p);
  rc = dslhal_support_blockRead((PVOID)sharedTables.bitAllocTblUstrm_p,ptidsl->AppData.BitAllocTblUstrm, 32*sizeof(unsigned char));
  if(rc)
    {
      dprintf(1,"dslhal_support_blockRead failed \n");
      return;
    }
    /* margin per tone */
  sharedTables.marginTblDstrm_p = (signed char *)dslhal_support_byteSwap32((unsigned int)sharedTables.marginTblDstrm_p);
  rc = dslhal_support_blockRead((PVOID)sharedTables.marginTblDstrm_p,ptidsl->AppData.marginTblDstrm, 256*sizeof(signed char));
  if(rc)
    {
      dprintf(1,"dslhal_support_blockRead failed \n");
      return;
    }
  /* Read Configured Options */
  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.oamWriteNegoParams_p, 
                               &configParms, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }
  else 
    {
      /* r-Msg1 */
    ptidsl->AppData.StdMode = (unsigned int)configParms.stdMode;
    if(configParms.stdMode == T1413_MODE)
      ptidsl->AppData.StdMode = DSLTRAIN_T1413_MODE;
    if(configParms.stdMode == GDMT_MODE)
      ptidsl->AppData.StdMode = DSLTRAIN_GDMT_MODE; 
    if(configParms.stdMode == MULTI_MODE)
      ptidsl->AppData.StdMode = DSLTRAIN_MULTI_MODE; 

    shim_osMoveMemory((void *)ptidsl->AppData.bRMsgs1, (void *)configParms.rMsgs1, 6*sizeof(char));
    if((ptidsl->AppData.bRMsgs1[2] & 0x02) == 0x02) 
      { 
      dprintf(7,"Trellis!\n");
      ptidsl->configFlag |= CONFIG_FLAG_TRELLIS;
      }
    else
      ptidsl->configFlag &= ~CONFIG_FLAG_TRELLIS;  
    if(ptidsl->AppData.bRMsgs1[2]&0x01)
      ptidsl->configFlag |= CONFIG_FLAG_EC;
    else
      ptidsl->configFlag &= ~CONFIG_FLAG_EC;      
    }
    return;
}


/********************************************************************************************
* FUNCTION NAME: dslhal_api_initStatistics()
*
*********************************************************************************************
* DESCRIPTION: init statistical information of ax5 modem daugter card. 
*
* Input: tidsl_t *ptidsl
* 
* Return: NULL
*         
********************************************************************************************/
void dslhal_api_initStatistics(tidsl_t * ptidsl)
{
  int rc;
  /*TCHostCommDef TCHostCommParms; */
  int optIdxU, optIdxD;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  DEV_HOST_dspWrNegoParaDef_t   rateparms;
  DEV_HOST_modemStatsDef_t      StatisticsDef;
  DEV_HOST_errorStats_t usIntlvError, usFastError, dsIntlvError, dsFastError;
  DEV_HOST_atmStats_t           atmStats;
  DEV_HOST_usAtmStats_t         usAtmStats0, usAtmStats1;
  DEV_HOST_dsAtmStats_t         dsAtmStats0,dsAtmStats1;
  DEV_HOST_dspWrSuperFrameCntDef_t   SuperFrameCnt;
  DEV_HOST_msg_t                atuc_msg, aturMsg;
  DEV_HOST_eocVarDef_t          eocVar;
  
  dprintf(5, "dslhal_api_initStatistics\n");
  
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterface_t *) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }
  dspOamSharedInterface.dspWriteNegoParams_p = (DEV_HOST_dspWrNegoParaDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspWriteNegoParams_p,&rateparms, sizeof(DEV_HOST_dspWrNegoParaDef_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }
  if(!rc) 
    {
      /*    shim_osMoveMemory((void *)ptidsl->AppData.bCRates1, (void *)rateparms.cRates1, SIZE_OF_CRATES1_TABLE); */
    ptidsl->AppData.bCRates2 = rateparms.cRates2;
    /*    shim_osMoveMemory((void *)ptidsl->AppData.bRRates1, (void *)rateparms.rRates1, 44); */
    ptidsl->AppData.bRRates2 = rateparms.rRates2;
    shim_osMoveMemory((void *)ptidsl->AppData.bCMsgs1, (void *)rateparms.cMsgs1, 6);
    shim_osMoveMemory((void *)ptidsl->AppData.bCMsgs2, (void *)rateparms.cMsgs2, 4);
    shim_osMoveMemory((void *)ptidsl->AppData.bRMsgs2, (void *)rateparms.rMsgs2, 4); 
    
    ptidsl->AppData.atucVendorId = dslhal_support_byteSwap32((unsigned int)rateparms.atucVendorId);
    ptidsl->AppData.lineLength = (unsigned int)dslhal_support_byteSwap16((unsigned short)rateparms.lineLength);
    ptidsl->AppData.atucRevisionNum = rateparms.atucGhsRevisionNum;
    ptidsl->AppData.usLineAttn = (ptidsl->AppData.bCMsgs2[3] >>2)&0x003f;
    ptidsl->AppData.usMargin = (ptidsl->AppData.bCMsgs2[2])&0x001f;
    
    /* Get the UpStream Connection Rate */
    /* Based on the Bit Pattern Get the Opt# */
    if((rateparms.cRates2 & 0x0f) == 0x01) 
      optIdxU = 0;
    else if((rateparms.cRates2 & 0x0f) == 0x02)  
      optIdxU = 1;
    else if((rateparms.cRates2 & 0x0f) == 0x04) 
      optIdxU = 2;
    else if((rateparms.cRates2 & 0x0f) == 0x08)  
      optIdxU = 3;
    else 
      optIdxU = -1;
    dprintf(5, "optIdxU=%d\n", optIdxU);
    /* Obtain the US Rates using Opt# and CRates1 Table */
    /* Rate(US) = [Bf(LS0) + Bi(LS0)]*32                */
    if(ptidsl->AppData.dsl_modulation <= DSLTRAIN_GLITE_MODE)
      ptidsl->AppData.USConRate = ((rateparms.cRates1[optIdxU][CRATES1_BF_LS0] + rateparms.cRates1[optIdxU][CRATES1_BI_LS0]) * 32);
    else
      ptidsl->AppData.USConRate =  dslhal_support_byteSwap16((unsigned short)rateparms.adsl2USRate);
    ptidsl->AppData.USPeakCellRate = ptidsl->AppData.USConRate; 
    
    /* Get the DownStream Connection Rate */
    /* Based on the Bit Pattern Get the Opt# */
    if(((rateparms.cRates2 >> 4) & 0x0f) == 0x01) 
      optIdxD = 0;
    else if(((rateparms.cRates2 >> 4) & 0x0f) == 0x02)  
      optIdxD = 1;
    else if(((rateparms.cRates2 >> 4) & 0x0f) == 0x04) 
      optIdxD = 2;
    else if(((rateparms.cRates2 >> 4) & 0x0f) == 0x08)  
      optIdxD = 3;
    else 
      optIdxD = -1;
    /* Obtain the DS Rates using Opt# and CRates1 Table */
    /* Rate(DS) = [Bf(AS0) + Bi(AS0)]*32                */
    if(ptidsl->AppData.dsl_modulation <= DSLTRAIN_GLITE_MODE)
      ptidsl->AppData.DSConRate =  (((rateparms.cRates1[optIdxD][CRATES1_BF_AS0]|((rateparms.cRates1[optIdxD][CRATES1_BF_DSRS]&0x80)<<1))+ (rateparms.cRates1[optIdxD][CRATES1_BI_AS0]|((rateparms.cRates1[optIdxD][CRATES1_BI_DSRS]&0x80)<<1)))* 32);
    else
      ptidsl->AppData.DSConRate =  dslhal_support_byteSwap16((unsigned short)rateparms.adsl2DSRate);
    dprintf(5, "ptidsl->AppData.wDSConRate=%d\n", ptidsl->AppData.DSConRate);
    /* Determine which Path has Modem Trained with */
    if((rateparms.cRates1[optIdxU][CRATES1_BF_LS0]) && (rateparms.cRates1[optIdxD][CRATES1_BF_AS0]))
      ptidsl->AppData.TrainedPath = FAST_PATH;
    else
      ptidsl->AppData.TrainedPath = INTERLEAVED_PATH;

    /* Set the mode in which the modem is trained */
    ptidsl->AppData.TrainedMode = (unsigned int)rateparms.trainMode;
    if(rateparms.trainMode == T1413_MODE)
      ptidsl->AppData.TrainedMode = DSLTRAIN_T1413_MODE;
    if(rateparms.trainMode == GDMT_MODE)
      ptidsl->AppData.TrainedMode = DSLTRAIN_GDMT_MODE;

    if(ptidsl->AppData.TrainedPath == FAST_PATH)
       ptidsl->AppData.dsFastParityBytesPerSymbol = (rateparms.cRates1[optIdxU][CRATES1_BF_DSRS]&0x1f);
    else
       ptidsl->AppData.dsIntlvParityBytesPerSymbol = (rateparms.cRates1[optIdxU][CRATES1_BI_DSRS]&0x1f);
    ptidsl->AppData.dsSymbolsPerCodeWord = (rateparms.cRates1[optIdxU][CRATES1_BFI_DSS]&0x1f);
    ptidsl->AppData.dsInterleaverDepth = ((rateparms.cRates1[optIdxU][CRATES1_BFI_DSI])|((rateparms.cRates1[optIdxU][CRATES1_BFI_DSS]&0xc0)<<2));

    if(ptidsl->AppData.TrainedPath == FAST_PATH)
       ptidsl->AppData.usFastParityBytesPerSymbol = (rateparms.cRates1[optIdxU][CRATES1_BF_USRS]&0x1f);
    else
       ptidsl->AppData.usIntlvParityBytesPerSymbol = (rateparms.cRates1[optIdxU][CRATES1_BI_USRS]&0x1f);
    ptidsl->AppData.usSymbolsPerCodeWord = (rateparms.cRates1[optIdxU][CRATES1_BFI_USS]&0x1f);
    ptidsl->AppData.usInterleaverDepth = ((rateparms.cRates1[optIdxU][CRATES1_BFI_USI])|((rateparms.cRates1[optIdxU][CRATES1_BFI_USS]&0xc0)<<2));
    }
  
  /* get the Statistics itself */

  dspOamSharedInterface.modemStats_p = (DEV_HOST_modemStatsDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemStats_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemStats_p,&StatisticsDef, sizeof(DEV_HOST_modemStatsDef_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return;
    }

  /* Populate the Error Structure Variables */

  /* US Interleave Path Error Statistics */

  /* Change the endianness of the Pointer */
  StatisticsDef.usErrorStatsIntlv_p = (DEV_HOST_errorStats_t *) dslhal_support_byteSwap32((unsigned int)StatisticsDef.usErrorStatsIntlv_p);

  rc = dslhal_support_blockRead((PVOID)StatisticsDef.usErrorStatsIntlv_p,&usIntlvError, (sizeof(DEV_HOST_errorStats_t)));
  
  if (rc) 
    return;

  /* DS Interleave Path Error Statistics */

  /* Change the endianness of the Pointer */
  StatisticsDef.dsErrorStatsIntlv_p = (DEV_HOST_errorStats_t *) dslhal_support_byteSwap32((unsigned int)StatisticsDef.dsErrorStatsIntlv_p);

  rc = dslhal_support_blockRead((PVOID)StatisticsDef.dsErrorStatsIntlv_p,&dsIntlvError, (sizeof(DEV_HOST_errorStats_t)));
  
  if (rc) 
    return;

  /* US Fast Path Error Statistics */

  /* Change the endianness of the Pointer */
  StatisticsDef.usErrorStatsFast_p = (DEV_HOST_errorStats_t *) dslhal_support_byteSwap32((unsigned int)StatisticsDef.usErrorStatsFast_p);

  rc = dslhal_support_blockRead((PVOID)StatisticsDef.usErrorStatsFast_p,&usFastError, (sizeof(DEV_HOST_errorStats_t)));
  
  if (rc) 
    return;


  /* DS Fast Path Error Statistics */

  /* Change the endianness of the Pointer */
  StatisticsDef.dsErrorStatsFast_p = (DEV_HOST_errorStats_t *) dslhal_support_byteSwap32((unsigned int)StatisticsDef.dsErrorStatsFast_p);

  rc = dslhal_support_blockRead((PVOID)StatisticsDef.dsErrorStatsFast_p,&dsFastError, (sizeof(DEV_HOST_errorStats_t)));
  
  if (rc) 
    return;

  if(ptidsl->AppData.bState > 2)
  {
    /* Get CRC Errors Stats for both US and DS */
    ptidsl->AppData.dsICRC_errors   = dslhal_support_byteSwap32((unsigned int)dsIntlvError.crcErrors);
    ptidsl->AppData.dsFCRC_errors   = dslhal_support_byteSwap32((unsigned int)dsFastError.crcErrors);
    ptidsl->AppData.usICRC_errors   = dslhal_support_byteSwap32((unsigned int)usIntlvError.crcErrors);
    ptidsl->AppData.usFCRC_errors   = dslhal_support_byteSwap32((unsigned int)usFastError.crcErrors);
    /* Get FEC Errors Stats for both US and DS */
    ptidsl->AppData.dsIFEC_errors   = dslhal_support_byteSwap32((unsigned int)dsIntlvError.fecErrors);
    ptidsl->AppData.dsFFEC_errors   = dslhal_support_byteSwap32((unsigned int)dsFastError.fecErrors);
    ptidsl->AppData.usIFEC_errors   = dslhal_support_byteSwap32((unsigned int)usIntlvError.fecErrors);
    ptidsl->AppData.usFFEC_errors   = dslhal_support_byteSwap32((unsigned int)usFastError.fecErrors);
    /* Get NCD Errors Stats for both US and DS */
    ptidsl->AppData.dsINCD_error   = dslhal_support_byteSwap32((unsigned int)dsIntlvError.ncdError);
    ptidsl->AppData.dsFNCD_error   = dslhal_support_byteSwap32((unsigned int)dsFastError.ncdError);
    ptidsl->AppData.usINCD_error   = dslhal_support_byteSwap32((unsigned int)usIntlvError.ncdError);
    ptidsl->AppData.usFNCD_error   = dslhal_support_byteSwap32((unsigned int)usFastError.ncdError);
    /* Get LCD Errors Stats for both US and DS */
    ptidsl->AppData.dsILCD_errors   = dslhal_support_byteSwap32((unsigned int)dsIntlvError.lcdErrors);
    ptidsl->AppData.dsFLCD_errors   = dslhal_support_byteSwap32((unsigned int)dsFastError.lcdErrors);
    ptidsl->AppData.usILCD_errors   = dslhal_support_byteSwap32((unsigned int)usIntlvError.lcdErrors);
    ptidsl->AppData.usFLCD_errors   = dslhal_support_byteSwap32((unsigned int)usFastError.lcdErrors);
    /*Get HEC Errors Stats for both US and DS */
    ptidsl->AppData.dsIHEC_errors   = dslhal_support_byteSwap32((unsigned int)dsIntlvError.hecErrors);
    ptidsl->AppData.dsFHEC_errors   = dslhal_support_byteSwap32((unsigned int)dsFastError.hecErrors);
    ptidsl->AppData.usIHEC_errors   = dslhal_support_byteSwap32((unsigned int)usIntlvError.hecErrors);
    ptidsl->AppData.usFHEC_errors   = dslhal_support_byteSwap32((unsigned int)usFastError.hecErrors);
    
    /* Get LOS and SEF error Stats */
    ptidsl->AppData.LOS_errors      = dslhal_support_byteSwap32(StatisticsDef.losErrors);              
    ptidsl->AppData.SEF_errors      = dslhal_support_byteSwap32(StatisticsDef.sefErrors);
    ptidsl->AppData.coLosErrors = dslhal_support_byteSwap32(StatisticsDef.farEndLosErrors);
    ptidsl->AppData.coRdiErrors = dslhal_support_byteSwap32(StatisticsDef.farEndRdiErrors);    
    
    dspOamSharedInterface.atmStats_p = (DEV_HOST_atmStats_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.atmStats_p);
    
    rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.atmStats_p,&atmStats, sizeof(DEV_HOST_atmStats_t));
    
    if (rc)
      {
        dprintf(1,"dslhal_support_blockRead failed\n");
        return;
      }

    /* Populate the US/DS ATM Stats Variables */
    
    /* US ATM Statistics */

    /* Change the endianness of the Pointer */
    atmStats.us0_p = (DEV_HOST_usAtmStats_t *) dslhal_support_byteSwap32((unsigned int)atmStats.us0_p);
    
    rc = dslhal_support_blockRead((PVOID)atmStats.us0_p,&usAtmStats0, (sizeof(DEV_HOST_usAtmStats_t)));
                                       
    if (rc) 
     return;

    /* Change the endianness of the Pointer */
    atmStats.us1_p = (DEV_HOST_usAtmStats_t *) dslhal_support_byteSwap32((unsigned int)atmStats.us1_p);
    
    rc = dslhal_support_blockRead((PVOID)atmStats.us1_p,&usAtmStats1, (sizeof(DEV_HOST_usAtmStats_t)));
                                       
    if (rc) 
     return;


  /* DS ATM Statistics */
                                       
  /* Change the endianness of the Pointer */
    atmStats.ds0_p = (DEV_HOST_dsAtmStats_t *) dslhal_support_byteSwap32((unsigned int)atmStats.ds0_p);
    
    rc = dslhal_support_blockRead((PVOID)atmStats.ds0_p,&dsAtmStats0, (sizeof(DEV_HOST_dsAtmStats_t)));
                                       
    if (rc) 
    return;

  /* Change the endianness of the Pointer */
    atmStats.ds1_p = (DEV_HOST_dsAtmStats_t *) dslhal_support_byteSwap32((unsigned int)atmStats.ds1_p);
    
    rc = dslhal_support_blockRead((PVOID)atmStats.ds1_p,&dsAtmStats1, (sizeof(DEV_HOST_dsAtmStats_t)));
                                       
    if (rc) 
    return;
    /* Get ATM Stats for both US and DS Channel 0*/
    ptidsl->AppData.usAtm_count[0]     = dslhal_support_byteSwap32(usAtmStats0.goodCount);
    ptidsl->AppData.usIdle_count[0]    = dslhal_support_byteSwap32(usAtmStats0.idleCount);
#if SWTC
    ptidsl->AppData.usPdu_count[0]     = dslhal_support_byteSwap32(usAtmStats0.pduCount);
#endif
    ptidsl->AppData.dsGood_count[0]    = dslhal_support_byteSwap32(dsAtmStats0.goodCount);
    ptidsl->AppData.dsIdle_count[0]    = dslhal_support_byteSwap32(dsAtmStats0.idleCount);
#if SWTC
    ptidsl->AppData.dsPdu_count[0]     = dslhal_support_byteSwap32(dsAtmStats0.pduCount);
#endif
    ptidsl->AppData.dsBadHec_count[0]  = dslhal_support_byteSwap32((dsAtmStats0.badHecCount));
    ptidsl->AppData.dsOVFDrop_count[0] = dslhal_support_byteSwap32((dsAtmStats0.ovflwDropCount));

    /* Get ATM Stats for both US and DS Channel 1*/
    ptidsl->AppData.usAtm_count[1]     = dslhal_support_byteSwap32(usAtmStats1.goodCount);
    ptidsl->AppData.usIdle_count[1]    = dslhal_support_byteSwap32(usAtmStats1.idleCount);
#if SWTC
    ptidsl->AppData.usPdu_count[1]     = dslhal_support_byteSwap32(usAtmStats1.pduCount);
#endif
    ptidsl->AppData.dsGood_count[1]    = dslhal_support_byteSwap32(dsAtmStats1.goodCount);
    ptidsl->AppData.dsIdle_count[1]    = dslhal_support_byteSwap32(dsAtmStats1.idleCount);
#if SWTC
    ptidsl->AppData.dsPdu_count[1]     = dslhal_support_byteSwap32(dsAtmStats1.pduCount);
#endif
    ptidsl->AppData.dsBadHec_count[1]  = dslhal_support_byteSwap32((dsAtmStats1.badHecCount));
    ptidsl->AppData.dsOVFDrop_count[1] = dslhal_support_byteSwap32((dsAtmStats1.ovflwDropCount));

    
    /* Determine the US and DS Superframe Count */

    dspOamSharedInterface.dspWriteSuperFrameCnt_p = (DEV_HOST_dspWrSuperFrameCntDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspWriteSuperFrameCnt_p);
    
    rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspWriteSuperFrameCnt_p,&SuperFrameCnt, sizeof(DEV_HOST_dspWrSuperFrameCntDef_t));
    
    if (rc)
    {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return;
    }

    ptidsl->AppData.usSuperFrmCnt   = dslhal_support_byteSwap32(SuperFrameCnt.wSuperFrameCntUstrm);
    ptidsl->AppData.dsSuperFrmCnt   = dslhal_support_byteSwap32(SuperFrameCnt.wSuperFrameCntDstrm);
    
    /* Determine Frame Mode and Max Frame Mode */
    
    dspOamSharedInterface.atucMsg_p = (DEV_HOST_msg_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.atucMsg_p);
    
    rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.atucMsg_p,&atuc_msg, sizeof(DEV_HOST_msg_t));
  
    if (rc)
    {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return;
    }

    ptidsl->AppData.FrmMode         = (unsigned int)atuc_msg.framingMode;
    ptidsl->AppData.MaxFrmMode      = (unsigned int)atuc_msg.maxFrameMode;
    
    /* Determine Gross Gain */

    dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);

    rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,&aturMsg, sizeof(DEV_HOST_msg_t));
  
    if (rc)
      {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return;
      }
    ptidsl->AppData.grossGain = (unsigned int)aturMsg.grossGain;
    /* Determine DS Line Attenuation & Margin */
    dspOamSharedInterface.eocVar_p = (DEV_HOST_eocVarDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.eocVar_p);
    
    rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.eocVar_p,&eocVar, sizeof(DEV_HOST_eocVarDef_t));
    
    if (rc)
    {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return;
    }

    ptidsl->AppData.dsLineAttn      = (unsigned int)eocVar.lineAtten;
    ptidsl->AppData.dsMargin        = (unsigned int)eocVar.dsMargin;
    }
    
#if __HOST_FORINTERNALUSEONLY_R_H__
  ptidsl->AppData.BER =  dslhal_INTERNAL_computeAtmBitErrorRate(ptidsl);
#endif
    dprintf(5, "initstatistics done\n");
    return;
  }

/****************************************************************************************** 
 * FUNCTION NAME:    dslhal_api_disableLosAlarm(tidsl_t *ptidsl,unsigned int set) 
 * 
 ******************************************************************************************* 
 * DESCRIPTION: This fuction enables/disables all the LOS alarms 
 * 
 * INPUT:  PITIDSLHW_T *ptidsl 
 *         unsigned int set; //if set is TRUE: LOS Alarms are disabled else enabled
 * RETURN: 0 SUCCESS 
 *         1 FAILED 
 * NOTES:  Currently not supported in any version other than MR4 Patch release..  
 *****************************************************************************************/ 
unsigned int dslhal_api_disableLosAlarm(tidsl_t *ptidsl,unsigned int set) 
{ 
   DEV_HOST_oamWrNegoParaDef_t NegoPara;
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc;
   dprintf(5," dslhal_api_setTrainingMode()\n");
   if(!ptidsl)
    {
      dprintf(3, "Error: PTIDSL pointer invalid\n");
      return DSLHAL_ERROR_INVALID_PARAM;
    }

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface, 
                                     sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);

   rc = dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&NegoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  if(set) 
    {
   NegoPara.disableLosAlarm = TRUE;
   /*  NegoPara.marginMonitorTrning = TRUE;
       NegoPara.marginMonitorShwtme = TRUE;*/
    } 
  else
    {
    NegoPara.disableLosAlarm = FALSE;
    /*  NegoPara.marginMonitorTrning = FALSE;
        NegoPara.marginMonitorShwtme = FALSE;*/
    }

  rc = dslhal_support_blockWrite(&NegoPara,(PVOID)dspOamSharedInterface.oamWriteNegoParams_p, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  dprintf(5," dslhal_api_disableLosAlarm() Done\n"); 
  return DSLHAL_ERROR_NO_ERRORS; 
} 

/******************************************************************************************  
 * FUNCTION NAME:    dslhal_api_setMarginThreshold(tidsl_t *ptidsl,int threshold)  
 *  
 *******************************************************************************************  
 * DESCRIPTION: This fuction does sets the Margin threshold 
 *  
 * INPUT:  PITIDSLHW_T *ptidsl  
 *         int threshold 
 *        
 *                                  
 * RETURN: 0 SUCCESS  
 *         1 FAILED  
 *  
 *****************************************************************************************/  
unsigned int dslhal_api_setMarginThreshold(tidsl_t *ptidsl, int threshold)  
{  
   DEV_HOST_oamWrNegoParaDef_t NegoPara;  
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc;  
   
   dprintf(5," dslhal_ctrl_setThreshold()\n");  
    
   if(!ptidsl)  
     {  
      dprintf(3, "Error: PTIDSL pointer invalid\n");  
      return DSLHAL_ERROR_INVALID_PARAM;  
     }  
  
   pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);


   rc = dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&NegoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  NegoPara.marginThreshold = threshold;
  
  rc = dslhal_support_blockWrite(&NegoPara,dspOamSharedInterface.oamWriteNegoParams_p, sizeof(DEV_HOST_oamWrNegoParaDef_t));

  if(rc)
    return DSLHAL_ERROR_MARGIN_API_FAILURE;

  dprintf(5," dslhal_api_setThreshold() Done\n");  
  return DSLHAL_ERROR_NO_ERRORS;  
}  


/****************************************************************************************** 
 * FUNCTION NAME:    dslhal_api_setMonitorFlags(tidsl_t *ptidsl, unsigned int trainflag,unsigned int shwtflag) 
 * 
 ******************************************************************************************* 
 * DESCRIPTION: This fuction does sets the Margin monitoring flag
 * 
 * INPUT:  PITIDSLHW_T *ptidsl 
 *         unsigned int trainflag
 *         unsigned int shwtflag 
 *                                 
 * RETURN: 0 SUCCESS 
 *         1 FAILED 
 * 
 *****************************************************************************************/ 
unsigned int dslhal_api_setMarginMonitorFlags(tidsl_t *ptidsl,unsigned int trainflag,unsigned int shwtflag) 
{ 
   DEV_HOST_oamWrNegoParaDef_t NegoPara; 
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc; 
  
   dprintf(5," dslhal_ctrl_setMonitorFlags()\n"); 
   
   if(!ptidsl) 
     { 
      dprintf(3, "Error: PTIDSL pointer invalid\n"); 
      return DSLHAL_ERROR_INVALID_PARAM; 
     } 

   pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);


   rc = dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&NegoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  if (trainflag) 
    { 
        NegoPara.marginMonitorTrning = TRUE; 
    } 
  else 
    { 
        NegoPara.marginMonitorTrning = FALSE; 
    } 
  if (shwtflag) 
    { 
        NegoPara.marginMonitorShwtme = TRUE; 
    } 
  else 
    { 
        NegoPara.marginMonitorShwtme = FALSE; 
    } 
 
  rc = dslhal_support_blockWrite(&NegoPara,dspOamSharedInterface.oamWriteNegoParams_p, sizeof(DEV_HOST_oamWrNegoParaDef_t));
    if(rc)
      return DSLHAL_ERROR_MARGIN_API_FAILURE;
  dprintf(5," dslhal_api_setMonitorFlags() Done\n"); 
  return DSLHAL_ERROR_NO_ERRORS; 
} 

/******************************************************************************************  
 * FUNCTION NAME:    dslhal_api_setEocSerialNumber(tidsl_t *ptidsl,char *SerialNum)  
 *  
 *******************************************************************************************  
 * DESCRIPTION: This fuction Sets the eoc Serial Number  
 *  
 * INPUT:  PITIDSLHW_T *ptidsl  
 *         char *SerialNum : Input eoc Serial Number
 *  
 * RETURN: 0 SUCCESS  
 *         1 FAILED  
 *  
 *****************************************************************************************/  
unsigned int dslhal_api_setEocSerialNumber(tidsl_t *ptidsl,char *SerialNumber)  
{  
   DEV_HOST_eocVarDef_t eocVar;
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc;  
   dprintf(5," dslhal_api_setEocSerialNumber()\n");  
 
   if(!ptidsl) 
     { 
      dprintf(3, "Error: PTIDSL pointer invalid\n"); 
      return DSLHAL_ERROR_INVALID_PARAM; 
     } 

   pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

  dspOamSharedInterface.eocVar_p = (DEV_HOST_eocVarDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.eocVar_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.eocVar_p, 
                               &eocVar, sizeof(DEV_HOST_eocVarDef_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
   
  shim_osMoveMemory(eocVar.serialNumber,SerialNumber,32); 

  rc= dslhal_support_blockWrite(&eocVar,dspOamSharedInterface.eocVar_p,sizeof(DEV_HOST_eocVarDef_t)); 
    if(rc)
      return DSLHAL_ERROR_EOCREG_API_FAILURE;
  dprintf(5," dslhal_api_setEocSerialNumber() Done\n");  
   
  return DSLHAL_ERROR_NO_ERRORS;
  
}  


/****************************************************************************************** 
 * FUNCTION NAME:    dslhal_api_setEocVendorId(tidsl_t *ptidsl,char *VendorID) 
 * 
 ******************************************************************************************* 
 * DESCRIPTION: This fuction Sets the eoc Vendor ID 
 * 
 * INPUT:  PITIDSLHW_T *ptidsl 
 *         char *VendorID : Input eoc Vendor ID
 * 
 * RETURN: 0 SUCCESS 
 *         1 FAILED 
 * 
 *****************************************************************************************/ 
unsigned int dslhal_api_setEocVendorId(tidsl_t *ptidsl,char *VendorID) 
{ 
  DEV_HOST_oamWrNegoParaDef_t NegoPara;  
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  int rc; 
  dprintf(5," dslhal_api_setEocVendorId()\n"); 

  if(!ptidsl) 
    { 
      dprintf(3, "Error: PTIDSL pointer invalid\n"); 
      return DSLHAL_ERROR_INVALID_PARAM; 
    }   
  
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
  
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return DSLHAL_ERROR_BLOCK_READ;
    }

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);


   rc = dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&NegoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  
  shim_osMoveMemory(NegoPara.gdmtVendorId,VendorID,8);
  rc= dslhal_support_blockWrite(&NegoPara,dspOamSharedInterface.oamWriteNegoParams_p, sizeof(DEV_HOST_oamWrNegoParaDef_t));
    if(rc)
      return DSLHAL_ERROR_EOCREG_API_FAILURE;
  
  dprintf(5," dslhal_api_setEocVendorId() Done\n"); 
  return DSLHAL_ERROR_NO_ERRORS; 
} 

/******************************************************************************************   
 * FUNCTION NAME:    dslhal_api_setEocRevisionNumber(tidsl_t *ptidsl,char *RevNum)   
 *   
 *******************************************************************************************   
 * DESCRIPTION: This fuction Sets the eoc Revision Number   
 *   
 * INPUT:  PITIDSLHW_T *ptidsl   
 *         char *RevNum : Input eoc Revision Number 
 *   
 * RETURN: 0 SUCCESS   
 *         1 FAILED   
 *   
 *****************************************************************************************/   
unsigned int dslhal_api_setEocRevisionNumber(tidsl_t *ptidsl,char *RevNumber)   
{   

   DEV_HOST_eocVarDef_t eocVar;
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc;  
   /*add for UR2 test */
   UINT8        selfTestResults[2];
   memset(selfTestResults,0x00,sizeof(selfTestResults));
   /* add for UR2 test */
   dprintf(5," dslhal_api_setEocRevisionNumber()\n");   
   
  if(!ptidsl)  
     {  
      dprintf(3, "Error: PTIDSL pointer invalid\n");  
      return DSLHAL_ERROR_INVALID_PARAM;  
     }  

   pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
     dprintf(1,"dslhal_support_blockRead failed\n");
     return DSLHAL_ERROR_BLOCK_READ;
     }

  dspOamSharedInterface.eocVar_p = (DEV_HOST_eocVarDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.eocVar_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.eocVar_p, 
                               &eocVar, sizeof(DEV_HOST_eocVarDef_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  shim_osMoveMemory(eocVar.revNumber,RevNumber,4);
  /* add for UR2 test */
  shim_osMoveMemory(eocVar.dummy,selfTestResults,2);
  /* add for UR2 test */
  rc=dslhal_support_blockWrite(&eocVar,dspOamSharedInterface.eocVar_p,sizeof(DEV_HOST_eocVarDef_t)); 
    if(rc)
      return DSLHAL_ERROR_EOCREG_API_FAILURE;
  dprintf(5," dslhal_api_setEocRevisionNumber Done\n");   
  return DSLHAL_ERROR_NO_ERRORS;   
}   

/******************************************************************************************      
 * FUNCTION NAME:    dslhal_api_setAturConfig(tidsl_t *ptidsl,char *ATURConfig)      
 *      
 *******************************************************************************************      
 * DESCRIPTION: This fuction Sets the eoc ATUR Config Register  
 *      
 * INPUT:  PITIDSLHW_T *ptidsl      
 *         char *ATURConfig : Input eoc ATUR Config Register    
 *      
 * RETURN: 0 SUCCESS      
 *         1 FAILED      
 *      
 *****************************************************************************************/      
unsigned int dslhal_api_setAturConfig(tidsl_t *ptidsl,char *ATURConfig)
{    

  DEV_HOST_eocVarDef_t eocVar;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  int rc;  
  dprintf(5," dslhal_api_setAturConfig()\n");    

  if(!ptidsl)  
     {  
      dprintf(3, "Error: PTIDSL pointer invalid\n");  
      return DSLHAL_ERROR_INVALID_PARAM;  
     }  

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
  
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  
  dspOamSharedInterface.eocVar_p = (DEV_HOST_eocVarDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.eocVar_p);
  
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.eocVar_p,
                                     &eocVar, sizeof(DEV_HOST_eocVarDef_t));
  
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }  
     shim_osMoveMemory(eocVar.aturConfig,ATURConfig,30);
   rc= dslhal_support_blockWrite(&eocVar,dspOamSharedInterface.eocVar_p,sizeof(DEV_HOST_eocVarDef_t)); 
    if(rc)
      return DSLHAL_ERROR_EOCREG_API_FAILURE;
  dprintf(5," dslhal_api_setAturConfig() Done\n");    
  return DSLHAL_ERROR_NO_ERRORS;   
}


/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setRateAdaptFlag(tidsl_t *ptidsl,unsigned int flag)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Rate Adapt Enable Flag
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int flag; //if flag = TRUE set rateadapt flag else reset it
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_api_setRateAdaptFlag(tidsl_t *ptidsl,unsigned int flag)
{
   DEV_HOST_msg_t aturMsg;
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc;
   dprintf(5," dslhal_api_setRateAdaptFlag()\n");
   
  if(!ptidsl)  
     {  
      dprintf(3, "Error: PTIDSL pointer invalid\n");  
      return DSLHAL_ERROR_INVALID_PARAM;  
     }  

   pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return DSLHAL_ERROR_BLOCK_READ;
     }
   
   dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,
                                      &aturMsg, sizeof(DEV_HOST_msg_t));
   if (rc)
     {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return DSLHAL_ERROR_BLOCK_READ;
     }
   
   if(flag)
     aturMsg.rateAdapt = TRUE;
   else
     aturMsg.rateAdapt = FALSE;
   
   rc= dslhal_support_blockWrite(&aturMsg,dspOamSharedInterface.aturMsg_p,sizeof(DEV_HOST_msg_t)); 
   if(rc)
     return DSLHAL_ERROR_CONFIG_API_FAILURE;
   dprintf(5," dslhal_api_setRateAdaptFlag() Done\n");
   return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setTrellisFlag(tidsl_t *ptidsl,unsigned int flag)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Trellis Coding Enable Flag
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int flag;  // if flag = TRUE, set trellis flag else reset
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_api_setTrellisFlag(tidsl_t *ptidsl,unsigned int flag)
{

   DEV_HOST_msg_t aturMsg;
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   DEV_HOST_oamWrNegoParaDef_t negoPara;
   int rc;
   dprintf(5," dslhal_api_setTrellisFlag()\n");   
   if(!ptidsl)
     {
       dprintf(3, "Error: PTIDSL pointer invalid\n");
       return DSLHAL_ERROR_INVALID_PARAM;
     }
   pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);
   rc += dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,&aturMsg, sizeof(DEV_HOST_msg_t));
   dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);
   rc += dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&negoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

  if(flag)
    {
      aturMsg.trellis = TRUE;
      negoPara.rMsgs1[2] |= 0x02;
    }
  else
    {
      aturMsg.trellis = FALSE;
      negoPara.rMsgs1[2] &= 0xFD;
    }   
  rc=0;
  rc+=dslhal_support_blockWrite(&aturMsg,dspOamSharedInterface.aturMsg_p,sizeof(DEV_HOST_msg_t)); 
  rc+= dslhal_support_blockWrite(&negoPara,dspOamSharedInterface.oamWriteNegoParams_p, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if(rc)
     return DSLHAL_ERROR_CONFIG_API_FAILURE;

  dprintf(5," dslhal_api_setTrellisFlag() Done\n");
  return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setMaxBitsPerCarrier(tidsl_t *ptidsl,unsigned int maxbits)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Maximum bits per carrier value
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int maxbits : should be a value between 0-15
*
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_api_setMaxBitsPerCarrier(tidsl_t *ptidsl,unsigned int maxbits)
{

   DEV_HOST_msg_t aturMsg;
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc;

   dprintf(5," dslhal_api_setMaxBitsPerCarrier()\n");
   if(maxbits>15)
     {
       dprintf(3,"Maximum Number of Bits per carrier cannot exceed 15!\n");
       return DSLHAL_ERROR_INVALID_PARAM;
     }

   if(!ptidsl)
    {
      dprintf(3, "Error: PTIDSL pointer invalid\n");
       return DSLHAL_ERROR_INVALID_PARAM;
    }
   pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,
                                      &aturMsg, sizeof(DEV_HOST_msg_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

  aturMsg.maxBits = maxbits;
   
   rc=dslhal_support_blockWrite(&aturMsg,dspOamSharedInterface.aturMsg_p,sizeof(DEV_HOST_msg_t)); 
   if(rc)
     return DSLHAL_ERROR_CONFIG_API_FAILURE;
   dprintf(5," dslhal_api_setMaxBitsPerCarrier() Done\n");
  return DSLHAL_ERROR_NO_ERRORS;
}


/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setMaxInterleaverDepth(tidsl_t *ptidsl,unsigned int maxdepth)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Maximum Interleave Depth Supported
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int maxdepth : Should be between 0 and 3 depending on intlv buffer
*                                 size 64-512
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_api_setMaxInterleaverDepth(tidsl_t *ptidsl,unsigned int maxdepth)
{
   DEV_HOST_msg_t aturMsg;
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc;
   dprintf(5," dslhal_api_setMaxInterleaverDepth()\n");
   if(maxdepth>3)
     {
       dprintf(3,"Invalid Value for maximum interleave depth (must be <3)\n");
       return DSLHAL_ERROR_INVALID_PARAM;
     }

   if(!ptidsl)
    {
      dprintf(3, "Error: PTIDSL pointer invalid\n");
       return DSLHAL_ERROR_INVALID_PARAM;
    }
   pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,
                                      &aturMsg, sizeof(DEV_HOST_msg_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

  aturMsg.maxIntlvDepth = maxdepth;
   
   rc=dslhal_support_blockWrite(&aturMsg,dspOamSharedInterface.aturMsg_p,sizeof(DEV_HOST_msg_t)); 
   if(rc)
     return DSLHAL_ERROR_CONFIG_API_FAILURE;
   dprintf(5," dslhal_api_setMaxInterleaverDepth() Done\n");
   return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_acknowledgeInterrupt()
*
*********************************************************************************************
* DESCRIPTION:
*   Sets the host interrupt bit masks
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_acknowledgeInterrupt(tidsl_t * ptidsl)
{
  unsigned int interruptSources=0;
  /* Clear out the DSLSS Interrupt Registers to acknowledge Interrupt */
  if(DSLHAL_REG32(dslhal_support_hostDspAddressTranslate(DSP_INTERRUPT_SOURCE_REGISTER))&MASK_MAILBOX_INTERRUPTS)
    {
      DSLHAL_REG32(dslhal_support_hostDspAddressTranslate(DSP_INTERRUPT_CLEAR_REGISTER))|=MASK_MAILBOX_INTERRUPTS;
      dprintf(5,"Mailbox Interrupt \n");
    }
  if(DSLHAL_REG32(dslhal_support_hostDspAddressTranslate(DSP_INTERRUPT_SOURCE_REGISTER))&MASK_BITFIELD_INTERRUPTS)
    {
      DSLHAL_REG32(dslhal_support_hostDspAddressTranslate(DSP_INTERRUPT_CLEAR_REGISTER))|=MASK_BITFIELD_INTERRUPTS;
      dprintf(5,"Bitfield Interrupt \n");
    }
  if(DSLHAL_REG32(dslhal_support_hostDspAddressTranslate(DSP_INTERRUPT_SOURCE_REGISTER))&MASK_HEARTBEAT_INTERRUPTS)
    {
      DSLHAL_REG32(dslhal_support_hostDspAddressTranslate(DSP_INTERRUPT_CLEAR_REGISTER))|=MASK_HEARTBEAT_INTERRUPTS;
      dprintf(5,"HeartBeat Interrupt \n");
    }
  interruptSources = dslhal_support_parseInterruptSource(ptidsl);
  if(interruptSources < 0)
    return DSLHAL_ERROR_INTERRUPT_FAILURE;
  else  
    return interruptSources;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_disableDspHybridSelect()
*
*********************************************************************************************
* DESCRIPTION:
*   Sets the host interrupt bit masks
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_disableDspHybridSelect(tidsl_t * ptidsl,unsigned int disable)
{
  int rc;
  DEV_HOST_phyPerf_t phyPerf;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   dspOamSharedInterface.phyPerf_p = (DEV_HOST_phyPerf_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.phyPerf_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.phyPerf_p,
                                      &phyPerf, sizeof(DEV_HOST_phyPerf_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   if(disable==1)
     {
       phyPerf.disableDspHybridSelect_f = TRUE;
       //       hybrid_selected = 888;
     }
   else
     {
       phyPerf.disableDspHybridSelect_f = FALSE;
       //       hybrid_selected = 888;
     }
  rc=dslhal_support_blockWrite(&phyPerf,dspOamSharedInterface.phyPerf_p,sizeof(DEV_HOST_phyPerf_t)); 
   if(rc)
     return DSLHAL_ERROR_HYBRID_API_FAILURE;
   else
     return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_selectHybrid()
*
*********************************************************************************************
* DESCRIPTION:
*   Sets the host interrupt bit masks
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_selectHybrid(tidsl_t * ptidsl,unsigned int hybridNum)
{
  int rc;
  DEV_HOST_phyPerf_t phyPerf;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  if(hybridNum<1||hybridNum>4)
   {
     dprintf(3,"Invalid Value for Hybrid Number \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }

  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   dspOamSharedInterface.phyPerf_p = (DEV_HOST_phyPerf_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.phyPerf_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.phyPerf_p,
                                      &phyPerf, sizeof(DEV_HOST_phyPerf_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   phyPerf.hostSelectHybridNum = hybridNum;
   rc=dslhal_support_blockWrite(&phyPerf,dspOamSharedInterface.phyPerf_p,sizeof(DEV_HOST_phyPerf_t)); 
   if(rc)
     return DSLHAL_ERROR_HYBRID_API_FAILURE;
   else
     return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_reportHybridMetrics()
*
*********************************************************************************************
* DESCRIPTION:
*   Sets the host interrupt bit masks
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_reportHybridMetrics(tidsl_t * ptidsl,int *metric)
{
  int rc,i;
  DEV_HOST_phyPerf_t phyPerf;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  if(hybrid_selected>5)
    {
      dprintf(4,"Hybrid Metrics Not Yet Available \n");
    }
  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return (0-DSLHAL_ERROR_INVALID_PARAM);
   }  
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return (0-DSLHAL_ERROR_BLOCK_READ);
     }
   
   dspOamSharedInterface.phyPerf_p = (DEV_HOST_phyPerf_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.phyPerf_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.phyPerf_p,
                                      &phyPerf, sizeof(DEV_HOST_phyPerf_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return (0-DSLHAL_ERROR_BLOCK_READ);
     }
   rc = sizeof(phyPerf.hybridCost);
   for(i=0;i<(rc/4);i++)
     {
       metric[i] = dslhal_support_byteSwap32(phyPerf.hybridCost[i]);
     }
   return hybrid_selected;
}

/******************************************************************************************    
 * FUNCTION NAME:    dslhal_api_selectInnerOuterPair(tidsl_t *ptidsl,unsigned int pairSelect)
 *    
 *******************************************************************************************    
 * DESCRIPTION: This fuction selects inner/outer pair on RJ11.  
 *    
 * INPUT:  PITIDSLHW_T *ptidsl , unsigned int pairSelect
 *           
 * RETURN: 0 SUCCESS    
 *         1 FAILED    
 *      
 *****************************************************************************************/    
unsigned int dslhal_api_selectInnerOuterPair(tidsl_t *ptidsl,unsigned int pairSelect)
{
   int rc; 
   
   dprintf(5, "dslhal_api_sendQuiet\n"); 
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_RJ11SELECT, (unsigned int)pairSelect, 0, 0); 
   if(rc)
    {  
      dprintf(1,"dslhal_api_sendQuiet failed\n");  
      return DSLHAL_ERROR_CTRL_API_FAILURE;    
    }  
    return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************    
 * FUNCTION NAME:    dslhal_api_resetTrainFailureLog(tidsl_t *ptidsl)
 *    
 *******************************************************************************************    
 * DESCRIPTION: This fuction resets the failed state log stored
 *    
 * INPUT:  PITIDSLHW_T *ptidsl
 *           
 * RETURN: 0 SUCCESS    
 *         1 FAILED    
 *      
 *****************************************************************************************/    
unsigned int dslhal_api_resetTrainFailureLog(tidsl_t *ptidsl)
{
    
   int rc;
   dprintf(5, "dslhal_api_resetTrainFailureLog \n"); 
   for(rc=0;rc<ptidsl->AppData.trainFails;rc++)
     {
       ptidsl->AppData.trainFailStates[rc]=0;
     }
   ptidsl->AppData.trainFails = 0;
   return DSLHAL_ERROR_NO_ERRORS; 
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_configureLed()
*
*********************************************************************************************
* DESCRIPTION:
*   Sets the host interrupt bit masks
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_configureLed(tidsl_t * ptidsl,unsigned int idLed, unsigned int onOff)
{
  int rc;
  DEV_HOST_modemEnvPublic_t modemEnv;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  if(idLed>2 || onOff>2)
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }

  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   dspOamSharedInterface.modemEnvPublic_p = (DEV_HOST_modemEnvPublic_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemEnvPublic_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemEnvPublic_p,
                                      &modemEnv, sizeof(DEV_HOST_modemEnvPublic_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   if(idLed==ID_DSL_LINK_LED)
     {
       modemEnv.overrideDslLinkLed_f = TRUE;
       if(onOff!=2)
          modemEnv.dslLinkLedState_f = onOff;
     }
   if(idLed==ID_DSL_ACT_LED)
     {
       modemEnv.overrideDslLinkLed_f = TRUE;
       if(onOff!=2)
          modemEnv.dslLinkLedState_f = onOff;
     }
   if(idLed==ID_RESTORE_DEFAULT_LED)
     {
       modemEnv.overrideDslLinkLed_f = FALSE;
       modemEnv.overrideDslActLed_f = FALSE;
     }
   rc=dslhal_support_blockWrite(&modemEnv,dspOamSharedInterface.modemEnvPublic_p,sizeof(DEV_HOST_modemEnvPublic_t)); 
   if(rc)
     return DSLHAL_ERROR_MODEMENV_API_FAILURE;
   else
     return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_configureExternBert()
*
*********************************************************************************************
* DESCRIPTION:
*   Sets the host interrupt bit masks
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_configureExternBert(tidsl_t * ptidsl,unsigned int configParm, unsigned int parmVal)
{
  int rc;
  DEV_HOST_modemEnvPublic_t modemEnv;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  if(configParm>1 || parmVal>1)
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }

  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   dspOamSharedInterface.modemEnvPublic_p = (DEV_HOST_modemEnvPublic_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemEnvPublic_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemEnvPublic_p,
                                      &modemEnv, sizeof(DEV_HOST_modemEnvPublic_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   if(configParm==0)
     {
       modemEnv.externalBert = parmVal;
     }
   if(configParm==1)
     {
       modemEnv.usBertPattern = parmVal;
     }
   rc=dslhal_support_blockWrite(&modemEnv,dspOamSharedInterface.modemEnvPublic_p,sizeof(DEV_HOST_modemEnvPublic_t)); 
   if(rc)
     return DSLHAL_ERROR_MODEMENV_API_FAILURE;
   else
     return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_configureAtmBert()
*
*********************************************************************************************
* DESCRIPTION:
*   Sets the host interrupt bit masks
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_configureAtmBert(tidsl_t * ptidsl,unsigned int configParm, unsigned int parmVal)
{
  int rc;
  DEV_HOST_atmDsBert_t atmDsBert;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  if(configParm>1 || parmVal>1)
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }

  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   dspOamSharedInterface.atmDsBert_p = (DEV_HOST_atmDsBert_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.atmDsBert_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.atmDsBert_p,
                                      &atmDsBert, sizeof(DEV_HOST_atmDsBert_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   if(configParm==0)
     {
       atmDsBert.atmBertFlag = parmVal;
       rc=dslhal_support_blockWrite(&atmDsBert,dspOamSharedInterface.atmDsBert_p,sizeof(DEV_HOST_atmDsBert_t)); 
       if(rc)
         return DSLHAL_ERROR_CONFIG_API_FAILURE;
     }
   if(configParm==1)
     {
       ptidsl->AppData.atmBertBitCountLow = atmDsBert.bitCountLow;
       ptidsl->AppData.atmBertBitCountHigh = atmDsBert.bitCountHigh;
       ptidsl->AppData.atmBertBitErrorCountLow = atmDsBert.bitErrorCountLow;
       ptidsl->AppData.atmBertBitErrorCountHigh = atmDsBert.bitErrorCountHigh;
     }
   return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_configureDgaspLpr()
*
*********************************************************************************************
* DESCRIPTION:
*   Configures dying gasp LPR signal
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_configureDgaspLpr(tidsl_t * ptidsl,unsigned int configParm, unsigned int parmVal)
{
  int rc;
  DEV_HOST_modemEnvPublic_t modemEnv;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  if(configParm>1 || parmVal>1)
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }

  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
   
   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   dspOamSharedInterface.modemEnvPublic_p = (DEV_HOST_modemEnvPublic_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemEnvPublic_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemEnvPublic_p,
                                      &modemEnv, sizeof(DEV_HOST_modemEnvPublic_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   if(configParm==0)
     {
       modemEnv.dGaspLprIndicator_f = parmVal;
     }
   if(configParm==1)
     {
       modemEnv.overrideDspLprGasp_f = parmVal;
     }
   rc=dslhal_support_blockWrite(&modemEnv,dspOamSharedInterface.modemEnvPublic_p,sizeof(DEV_HOST_modemEnvPublic_t)); 
   if(rc)
     return DSLHAL_ERROR_MODEMENV_API_FAILURE;
   else
     return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_genericDspRead()
*
*********************************************************************************************
* DESCRIPTION:
*   Reads from a Generic Location in the DSP Host Interface
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_genericDspRead(tidsl_t * ptidsl,unsigned int offset1, unsigned int offset2, 
                                       unsigned int offset3, unsigned char *localBuffer, unsigned int numBytes)
{
  int rc=0;
  unsigned int  hostIfLoc,structLoc,elementLoc;
  hostIfLoc = (unsigned int)ptidsl->pmainAddr;
  if(numBytes<=0 || !localBuffer || !ptidsl)
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  rc += dslhal_support_blockRead((PVOID)(hostIfLoc+sizeof(int)*offset1), &structLoc,sizeof(int));
  structLoc = dslhal_support_byteSwap32(structLoc);  
  rc += dslhal_support_blockRead((PVOID)(structLoc+sizeof(int)*offset2), &elementLoc,sizeof(int));
  elementLoc = dslhal_support_byteSwap32(elementLoc);    
  dprintf(3,"Host IF Location: 0x%x  Struct1 Location: 0x%x Element Location: 0x%x \n",hostIfLoc, structLoc, elementLoc);
  rc += dslhal_support_blockRead((PVOID)(elementLoc+(offset3*4)), localBuffer,numBytes);
  if (rc)
       return DSLHAL_ERROR_BLOCK_READ;
  return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_genericDspWrite()
*
*********************************************************************************************
* DESCRIPTION:
*   Writes to a Generic Location in the DSP Host Interface
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_genericDspWrite(tidsl_t * ptidsl,unsigned int offset1, unsigned int offset2, 
                                        unsigned int offset3, unsigned char *localBuffer, unsigned int numBytes)
{

  int rc=0;
  unsigned int  hostIfLoc,structLoc,elementLoc;
  hostIfLoc = (unsigned int)ptidsl->pmainAddr;
  if(numBytes<=0 || !localBuffer || !ptidsl)
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  rc += dslhal_support_blockRead((PVOID)(hostIfLoc+(offset1*4)), &structLoc,4);  
  structLoc = dslhal_support_byteSwap32(structLoc);  
  rc += dslhal_support_blockRead((PVOID)(structLoc+(offset2*4)), &elementLoc,4);
  elementLoc = dslhal_support_byteSwap32(elementLoc);  
  dprintf(3,"Host IF Location: 0x%x  Struct1 Location: 0x%x Element Location: 0x%x \n",hostIfLoc, structLoc, elementLoc);
  rc += dslhal_support_blockWrite(localBuffer,(PVOID)(elementLoc+(offset3*4)),numBytes);
  if (rc)
       return DSLHAL_ERROR_BLOCK_READ;
  return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_dspInterfaceRead()
*
*********************************************************************************************
* DESCRIPTION:
*   Reads from a Generic Location in the DSP Host Interface
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_dspInterfaceRead(tidsl_t * ptidsl,unsigned int baseAddr, unsigned int numOffsets, 
                                         unsigned int *offsets, unsigned char *localBuffer, unsigned int numBytes)
{
  int rc=0, off=0;
  unsigned int  prevAddr,currAddr;
  prevAddr = baseAddr;
  if(numBytes<=0 || !localBuffer || !ptidsl || !offsets)
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  for(off=0;off<numOffsets-1;off++)
    {
      rc += dslhal_support_blockRead((PVOID)(prevAddr+(4*offsets[off])), &currAddr,4);
      currAddr = dslhal_support_byteSwap32(currAddr);  
      prevAddr = currAddr;
      dprintf(5,"Curr Addr = 0x%x  Current Level: %d \n",currAddr,off);
    }
  currAddr = currAddr + offsets[numOffsets-1]*4;
  rc += dslhal_support_blockRead((PVOID)(currAddr),localBuffer,numBytes);
  if (rc)
       return DSLHAL_ERROR_BLOCK_READ;
  return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_dspInterfaceWrite()
*
*********************************************************************************************
* DESCRIPTION:
*   Writes to a Generic Location in the DSP Host Interface
*
* Return: 0  success
*         1  failed
* 
* NOTE: 
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_dspInterfaceWrite(tidsl_t * ptidsl,unsigned int baseAddr, unsigned int numOffsets, 
                                          unsigned int *offsets,unsigned char *localBuffer, unsigned int numBytes)
{

  int rc=0, off=0;
  unsigned int  prevAddr,currAddr;
  prevAddr = baseAddr;
  if(numBytes<=0 || !localBuffer || !ptidsl || !offsets)
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }  
  for(off=0;off<numOffsets-1;off++)
    {
      rc += dslhal_support_blockRead((PVOID)(prevAddr+(4*offsets[off])), &currAddr,4);
      currAddr = dslhal_support_byteSwap32(currAddr);  
      prevAddr = currAddr;
      dprintf(5,"Curr Addr = 0x%x  Current Level: %d \n",currAddr,off);
    }
  currAddr = currAddr + offsets[numOffsets-1]*4;
  rc += dslhal_support_blockWrite(localBuffer,(PVOID)(currAddr),numBytes);
  if (rc)
       return DSLHAL_ERROR_BLOCK_READ;
  return DSLHAL_ERROR_NO_ERRORS;
}


/******************************************************************************************     
 * FUNCTION NAME:    dslhal_api_sendMailboxCommand(tidsl_t *ptidsl, unsigned int cmd)     
 *     
 *******************************************************************************************     
 * DESCRIPTION: This fuction sends the passed mailbox command to the DSP     
 *     
 * INPUT:  PITIDSLHW_T *ptidsl     
 *         unsigned int cmd
 *            
 * RETURN: 0 SUCCESS     
 *         1 FAILED     
 *       
 *****************************************************************************************/     
/* Function is commented out for now since, its not tested */
 /*unsigned int dslhal_api_sendMailboxCommand(tidsl_t *ptidsl, unsigned int cmd) 
{ 
   int rc;  
    
   dprintf(5, "dslhal_api_sendMailboxCommand\n");  
   rc = dslhal_support_writeHostMailbox(ptidsl, cmd, 0, 0, 0);  
   if(rc) 
     {   
      dprintf(1,"dslhal_api_sendMailboxCommand failed\n");   
      return DSLHAL_ERROR_CTRL_API_FAILURE;    
     }   
    return DSLHAL_ERROR_NO_ERRORS; 
} */


