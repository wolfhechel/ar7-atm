/*******************************************************************************
* FILE PURPOSE:     DSL Driver API functions for Sangam
*********************************************************************************
* FILE NAME:        dsl_hal_support.c
*
* DESCRIPTION:
*       Contains DSL HAL APIs for Modem Control
*
*
* (C) Copyright 2001-02, Texas Instruments, Inc.
*    History
*    Date        Version            Notes
*    06Feb03     0.00.00            RamP   Created
*    21Mar03     0.00.01            RamP   Inserted byteswap functions
*    07Apr03     0.00.02            RamP   Implemented new error reporting scheme
*                                          Changed Commenting to C style only
*    12Apr03     0.00.03            RamP   Added function to set Interrupt Bit
*                                          Masks for bitfield & Mailboxes
*    14Apr03     0.00.04            RamP   Added function to process modem state
*                                          bit fields; renamed REG32 macros
*                                          Changed interrupt bit field settings
*    15Apr03     0.00.05            RamP   Fixed exit condition on dslShutdown
*    21Apr03     0.01.00            RamP   Fixed dslShutdown function & changed
*                                          loop counter for overlay byteswaps
*                (Alpha)                   Added cache writeback for overlays
*                                          Added function acknowledgeInterrupt
*    22Apr03     0.01.01            RamP   Moved acknowledgeInterrupt into api
*    24Apr03     0.01.02            RamP   Added function to compare crc32 with
*                                          pre-computed value as a recovery
*                                          scheme for corrupt overlay pages
*    28Apr03     0.01.03            RamP   Fixed a parameter in crc32 fxn call
*    05May03     0.01.04            RamP   Fixed Message structure access in
*                                          writeHostMailbox function
*    14May03     0.01.05            RamP   Lookup to netService of dsp version
*                (alpha ++)                to determine pots/isdn service
*    21May03     0.01.06            RamP   Added support for CO profiles
*    29May03     0.01.07            RamP   Added critical section tabs for block
*                                          read/write operations
*                                          Added functions to reload overlay pages
*                                          and CO Profiles
*    04Jun03     0.01.08            RamP   Added state transition timing counters
*    06Jun03     0.01.09            RamP   Added Interrupt source parsing function
*                                          Interrupt masking for heartbeat added
*    09Jun03     0.01.10            RamP   Modified modem state bit field processing
*                                          for structure changes in ITIDSLHW
*                                          fixed problem in free memory for CO prof
*    18Jul03     0.01.11            RamP   Optimized free memory for CO profiles &
*                                          overlay pages in the supporting APIs
*    28Jul03     0.02.00            RamP   Modified the process bitfield functn
*                                          for LED & Application State report
*    21Aug03     0.03.00            RamP   Added logic to allocate & communicate
*                                          memory for constellation buffer display
*    29Sep03     0.03.01            RamP   Added API switch calls to advcfg module
*                                          to abstract them from the API module
*    12Oct03     0.03.02            RamP   Added API to gather ADSL2 Messages
*    14Oct03     0.03.03            RamP   Added function to read CMsgsRA
*    23Oct03     0.03.04            RamP   Changed train history index to circular
*                                          buffer upon rollover
*    29Oct03     0.03.05            RamP   Added Adsl2 Delt Message Parsing
*    12Nov03     0.03.06            RamP   Fixed endianness issues with
*                                          Constellation Display
*    14Nov03     0.03.07            RamP   Added function to gather CRates1/RRates1
*                                          before they get overwritten by CRatesRA
*    19Nov03     0.03.08            JohnP  Revised dslhal_support_aocBitSwapProcessing to
*                                          prevent duplicate ATU-R bitswaps going to ACT
*    24Nov03     0.03.09            RamP   Implemented detailed State Tracking through
*                                          Modem State bit fields for ADSL/2
*    12Dec03     0.03.10            RamP   Tokenized advanced configuration code
*    12Dec03     0.03.11            RamP   Added state reset upon IDLE
*    19Dec03     0.03.12            RamP   Added static adsl2 byteswap function for
*                                          handling pointer to pointer cases
*                                          Changed adsl2 messages to correct pointer to
*                                          pointer dereferencing problems in some OS
*    26Dec03     0.03.13            RamP   Setting Current Address for Constellation
*                                          buffer in addition to start address
*                                          Added additional check to overlay page malloc
*    02Mar04     0.03.14            RamP   Changed overlay page number constant to
*                                          refer to the host interface constant
*    20Apr04     0.03.15            RamP   decoupled overlay page check from host
*                                          interface, modified Profiles to support
*                                          multiple profile sections, modified reload
*                                          training info functions
*    27Apr04     0.03.16            RamP   Fixed out of bounds situation with train
*                                          state indices; removed return based on
*                                          MAXSECTIONS, replaced by warning
*    29Apr04     0.03.17            RamP   Added code to process ADSL2+ DELT messages
*                                          Added function to determine dsNumTones
*                                          Added switch case for missing ADSL2 DELT msgs
*    04May04     0.03.18            RamP   Removed bState setting in Idle processing fxn
*    14May04     0.03.19            RamP   Moved SNR fetch function to this module
*    30Jun04     0.04.00            RamP   Changed address translation function to support
*                                          Ohio chipset
*    14Jul04     0.03.20            RamP   Changed DSP unreset function for increased DSP
*                                          processor speed & buck voltage
*    25Aug04     0.03.21            RamP   Removed changes for buck voltage & processor
*                                          speed; to be done with ATM driver & Firmware
*    26Aug04     0.03.22            Brian  Made changes to support switching DSP
*                                          frequency to 250Mhz
*    20Aug04     0.03.23            RamP   Fixed a bug with block write for 16-bit aligned
*                                          addresses where source pointer was corrupted
*    27Jan05     0.04.00            CPH    Added support for Ohio250, code cleanup
*    02Feb05     0.04.01            CPH    Pull dev_host_interface.h out from dsl_hal_register.h
*    02Mar05     0.04.02            CPH    Add DSL phy ENV feature control support
*    13Jun05     0.04.03            CPH    Change download debug msg level to 7. (was 5)
*    14June05    0.05.00            CPH    Added dslhal_support_getTrainedModeEx(), change AnxB
*                                          default training mode to MultiMode.
*    20June05    0.05.01            CPH    Fixed memory read crash bug in dslhal_support_blockRead()
*                                          Claen up & optimize
*    21June05    0.05.02            CPH    CQ9717 Fixed scratch memory odd bytes not swapped problem.
*    29Jun05     0.04.03            SAN    Added logic to flush the memory during the
*                                          dsp firmware download and increased the delay
*                                          from 0x5000 to 0x8000.
*    30Jun05     0.04.04            AV     Removed the flushing of the cache as in some
*                                          cases it can cause a crash. Added a retry
*                                          mechanisim to deal with the download jitter.
*    26Jul05     0.05.00            CPH    - Added dslhal_support_getTrainedMode(),
*                                            dslhal_support_readInternalOffset(),
*                                            dslhal_support_readOffset(),
*                                            dslhal_support_getMaxUsTones()
*                                          - Rename dslhal_support_getNumTones() to dslhal_support_getMaxDsTones()
*                                          - Rename dsNumTones to max_ds_tones in various places.
*    01Aug05     0.06.00            CPH    Fixed dslhal_support_blockRead() problem when the read length is
*                                          less than 4 bytes and not 32bit aligned.
*    05Sept05    0.07.00            CPH    Remove initialization for SNRpsds_p, QLNpsds_p & HLOGpsds_p in
*                                          dslhal_support_hostDspCodedownload().
*    06Oct05     0.08.00            CPH    Comment out redundent code in dslhal_support_writeHostMailbox()
*                                          which could cause crash.
*    7 Oct05     0.08.00            AV/CPH  Changed dprintf to dgprintf for code size reduction.
*    31Oct05     0.10.00            CPH    CQ10020: Fixed non-aligned dest in dslhal_support_blockWrite() with less than 4
*                                          bytes aligned-word causes CG5.1 generated datapump code crash problem.
*    04Nov05     0.11.00            CPH    Fixed T1413 mode got Zero DS/US rate when DSL_BIT_TMODE is set.
*    03Feb06     0.00.36            CPH    CQ10280: Add extended PHY feature bit support.
// UR8_MERGE_START CQ10386 PeterHou
*    15Mar06     0.12.00            CPH    CQ10386: Add SNR format comment for dslhal_support_gatherSnrPerBin().
// UR8_MERGE_END CQ10386
// UR8_MERGE_START CQ10774 Ram
*    19Jul06     0.13.00            Ram    CQ10774: Added EOCAOC Mask to the list of disabled Mailbox Interrupts
*                                          from the DSP. 
*                                          Also changed assignment into an |= to ensure that no DSP defaults for
*                                          oamFeature are overriden
// UR8_MERGE_END CQ10774 Ram
*******************************************************************************/
#include <dev_host_interface.h>
#include <dsl_hal_register.h>
#include <dsl_hal_support.h>

#define NUM_READ_RETRIES 3
static unsigned int dslhal_support_adsl2ByteSwap32(unsigned int in32Bits);
static unsigned int dslhal_support_findProfileIndex(tidsl_t *ptidsl, unsigned int profileAddr);

//dsl phy ENV feature control
unsigned int     _dsl_Feature_0;
unsigned int     _dsl_Feature_1;
unsigned int     _dsl_Feature_0_defined;
unsigned int     _dsl_Feature_1_defined;
//CQ10280
unsigned int     _dsl_PhyControl_0;
unsigned int     _dsl_PhyControl_1;
unsigned int     _dsl_PhyControl_0_defined;
unsigned int     _dsl_PhyControl_1_defined;

unsigned int     _DEBUG_HALT_=0;


/******************************************************************************************
* FUNCTION NAME:     dslhal_support_readInternalOffset
*
*******************************************************************************************
* DESCRIPTION: This function return endian converted 32 bit values from
*              Internal Host-DSP Interface based on the offste array passed in.
*              This is mainly used to get first level address from Internal
*              Host-DSP Interface (Internal used only)
*
* INPUT:  PITIDSLHW_T *ptidsl
*         int offsetnum: # entries in offset[]
*         int *offset
*
* RETURN: endian converted 32 bit value for the specified entry. (32 bit)
*
*****************************************************************************************/
#define DEV_HOST_DSP_OAM_INTERNAL_POINTER_LOCATION 0x80000004
unsigned int dslhal_support_readInternalOffset(tidsl_t *ptidsl, int offsetnum, int *offset)
{
  unsigned int dslReg;
  void *pInternal;

  dslhal_support_blockRead((char *)DEV_HOST_DSP_OAM_INTERNAL_POINTER_LOCATION,
      &pInternal, sizeof(unsigned int));
  pInternal=(void *) dslhal_support_byteSwap32((unsigned int) pInternal);

  dslhal_api_dspInterfaceRead(ptidsl,(unsigned int) pInternal, offsetnum,
      (unsigned int *)offset, (unsigned char *)&dslReg, 4);
  return dslhal_support_byteSwap32(dslReg);
}

/******************************************************************************************
* FUNCTION NAME:     dslhal_support_readOffset
*
*******************************************************************************************
* DESCRIPTION: This function return endian converted 32 bit values from
*              Host-DSP Interface based on the offste array passed in.
*              This is mainly used to get first level address from
*              Host-DSP Interface (Internal used only)
*
* INPUT:  PITIDSLHW_T *ptidsl
*         int offsetnum: # entries in offset[]
*         int *offset
*
* RETURN: endian converted address for the specified entry. (32 bit)
*
*****************************************************************************************/

unsigned int dslhal_support_readOffset(tidsl_t *ptidsl, int offsetnum, int *offset)
{
  unsigned int dslReg;

  dslhal_api_dspInterfaceRead(ptidsl,(unsigned int)ptidsl->pmainAddr, offsetnum,
      (unsigned int *)offset, (unsigned char *)&dslReg, 4);
  return dslhal_support_byteSwap32(dslReg);
}

//
//get DSP Address from offset array. (internal use only)
// type: 0: main pointers Table
//       1: Internal pointers Table
//
unsigned int dslhal_support_addrFromOffset(tidsl_t *ptidsl, int offsetnum, int *offsets, int type)
{
  int rc=0, off=0;
  unsigned int prevAddr,currAddr;
  unsigned int pInternal;

  if (type==0) // main pointers Table
    prevAddr = (unsigned int) ptidsl->pmainAddr;
  else {  // internal pointerss Table
    dslhal_support_blockRead((char *)DEV_HOST_DSP_OAM_INTERNAL_POINTER_LOCATION,
      &pInternal, sizeof(unsigned int));
    prevAddr = dslhal_support_byteSwap32(pInternal);
  }

  currAddr = prevAddr;

  for(off=0;off<offsetnum-1;off++)
  {
    rc += dslhal_support_blockRead((PVOID)(prevAddr+(4*offsets[off])), &currAddr,4);
    currAddr = dslhal_support_byteSwap32(currAddr);
    prevAddr = currAddr;
//    dgprintf(5,"Curr Addr = 0x%x  Current Level: %d \n",currAddr,off);
  }
  currAddr += offsets[offsetnum-1]*4;
  return currAddr;
}

unsigned int dslhal_support_writeFromOffset(tidsl_t *ptidsl, int offsetnum, int *offset, unsigned int data, int type)
{
  unsigned int dspAddr;
  int rc=0;

  dgprintf(4, "dslhal_support_writeFromOffset\n");
  dspAddr = dslhal_support_addrFromOffset(ptidsl, offsetnum, offset, type);
  rc=dslhal_support_hostDspAddressTranslate((unsigned int)dspAddr);
  if(rc== DSLHAL_ERROR_ADDRESS_TRANSLATE)
  {
    dgprintf(1, "dslhal_support_hostDspAddressTranslate failed\n");
    return  DSLHAL_ERROR_ADDRESS_TRANSLATE;
  }

  DSLHAL_REG32(rc) = data; // write
  return DSLHAL_ERROR_NO_ERRORS;
}


/******************************************************************************************
* FUNCTION NAME:     dslhal_support_getTrainedMode
*
*******************************************************************************************
* DESCRIPTION: This function return the current 32 trained mode.
*              Internally called dslhal_support_getTrainedModeEx.
*
* INPUT:  PITIDSLHW_T *ptidsl
*
* RETURN: converted extended TrainMode. (32 bit)
*         If the TIOIDINFO.useBitField is not set then the returned mode is the old (ordinal) mode.
*         If the TIOIDINFO.useBitField is set, then the returned mode is the bitfield mode.
*****************************************************************************************/

unsigned int dslhal_support_getTrainedMode (tidsl_t *ptidsl)
{
  int offset[]={3};
  DEV_HOST_dspWrNegoParaDef_t *dspWrNego_p;
  unsigned short annex_selected, psd_mask_qualifier;
  unsigned char trainedMode8;
  unsigned int TrainedModeEx;

  dgprintf(4, "dslhal_support_getTrainedMode\n");

  dspWrNego_p = (DEV_HOST_dspWrNegoParaDef_t *) dslhal_support_readOffset(ptidsl, 1, offset);

  // read Trained mode, don't need to swap since it's only 'char' (8 bit)
  dslhal_support_blockRead((void*) &dspWrNego_p->trainMode, &trainedMode8, sizeof(trainedMode8));

  // read annex_selected
  dslhal_support_blockRead((void*) &dspWrNego_p->annex_selected,
    &annex_selected, sizeof(dspWrNego_p->annex_selected));

 // read psd_mask_qualifier
  dslhal_support_blockRead((void*) &dspWrNego_p->psd_mask_qualifier,
    &psd_mask_qualifier, sizeof(dspWrNego_p->psd_mask_qualifier));

  ptidsl->AppData.annex_selected = dslhal_support_byteSwap16(annex_selected);
  ptidsl->AppData.psd_mask_qualifier= dslhal_support_byteSwap16(psd_mask_qualifier);

  dgprintf(4," trainMode8=%02x, annex_selected=%04x, psd_mask_qualifier\n",
    trainedMode8, ptidsl->AppData.annex_selected, ptidsl->AppData.psd_mask_qualifier);

  TrainedModeEx = dslhal_support_getTrainedModeEx(trainedMode8, ptidsl->AppData.annex_selected);

  if(!ptidsl->AppData.useBitField)
  {
    // override
    if (trainedMode8 == T1413_MODE)
      TrainedModeEx = DSLTRAIN_T1413_MODE;
    else if (trainedMode8 == GDMT_MODE)
      TrainedModeEx = DSLTRAIN_GDMT_MODE;
  }

  ptidsl->AppData.TrainedMode= TrainedModeEx;

  return TrainedModeEx;
}

/******************************************************************************************
* FUNCTION NAME:     dslhal_support_getTrainedModeEx
*
*******************************************************************************************
* DESCRIPTION: This function construct TrainModeEx(32 bit trainmode) from
*              trainmode (8 bit) and annex_selected.
*              This function is used to support AnnexB/AnnexM where it has extended definition
*              trainModeEx (32 bit).
*
* INPUT:  trainMode (8 bit), annex_selected
*
* RETURN: converted extended TrainMode. (32 bit)
*
*****************************************************************************************/
unsigned int dslhal_support_getTrainedModeEx (unsigned int trainMode8, unsigned int annex_selected)
{
  // construct TrainedModeEx from trainMode and annex_selected
  unsigned int TrainedModeEx= trainMode8;

  dgprintf(4, "dslhal_support_getTrainedModeEx\n");

  switch (annex_selected) {
  case ANNEXI:
    if (trainMode8 & ADSL2_MODE)
      TrainedModeEx = ADSL2_ANNEX_I;
    else if (trainMode8 & ADSL2PLUS_MODE)
      TrainedModeEx = ADSL2PLUS_ANNEX_I;
    break;
  case ANNEXJ:
    if (trainMode8 & ADSL2_MODE)
      TrainedModeEx = ADSL2_ANNEX_J;
    else if (trainMode8 & ADSL2PLUS_MODE)
      TrainedModeEx = ADSL2PLUS_ANNEX_J;
    break;
  case ANNEXM:
    if (trainMode8 & ADSL2_MODE)
      TrainedModeEx = ADSL2_ANNEX_M;
    else if (trainMode8 & ADSL2PLUS_MODE)
      TrainedModeEx = ADSL2PLUS_ANNEX_M;
    break;

  case ANNEXA:
  case ANNEXB:
  case ANNEXL:
  default: // same as trainMode8
    break;
  }; // switch

  TrainedModeEx |= (trainMode8 & DELT_ENABLE); // preserve DELT bit

  dgprintf(4, "trainMode=%02x, annex_selected=%04x -> TrainModeEx=%04x\n",
    trainMode8, annex_selected, TrainedModeEx);
  dgprintf(4, "dslhal_support_getTrainedModeEx Done\n");
  return TrainedModeEx;
}


/******************************************************************************************
* FUNCTION NAME:     dslhal_support_unresetDslSubsystem
*
*******************************************************************************************
* DESCRIPTION: This function unreset Dsl Subsystem
*
* INPUT:  None
*
* RETURN: 0 if Pass; 1 if Fail
*
*****************************************************************************************/
int  dslhal_support_unresetDslSubsystem(void)
{
  unsigned int clock_pllm=0;
  unsigned int chip_id;
  int rc;

  chip_id = DSLHAL_REG32(REG_CVR) & 0xFFFF;

  switch(chip_id)
  {
      case CHIP_AR7:     /* Sangam Chip */
          dgprintf(5, "Start programming PLL for Sangam chip\n");
          /*
           * In Sangam, MIPS_PLL Multiplication factor is REG_MIPS_PLLM[15:12]
           * check PLLM to determine desired dsp clock at 200 or 250Mhz.
           * This value was set in dslhal_api_boostDspFrequency().
           * For this reason, this code need to be running after
           * dslhal_api_boostDspFrequency() has been executed
           */
          clock_pllm = (DSLHAL_REG32(REG_MIPS_PLLM) >> 12) & 0x0F;
          dgprintf(5, "clock_ ID = 0x%08x\n", clock_pllm);
          if ((clock_pllm < 7) || (clock_pllm > 9))
          {
             dgprintf(4," dslhal_support_unresetDslSubsystem()\n");
             /* Put ADSLSS in to reset */
             DSLHAL_REG32(REG_VSERCLKSELR) = 0x1;
             shim_osClockWait(64);
             dgprintf(5,"Selected APLL Reg \n");

             DSLHAL_REG32(REG_MIPS_PLLM) = 0x4;
             shim_osClockWait(64);
             dgprintf(5,"Enable Analog PLL \n");

             DSLHAL_REG32(REG_MIPS_PLLM) = 0x77fe;    /* set default to 200MHz */
             shim_osClockWait(64);
             dgprintf(5,"Set PLL for DSP\n");

             dgprintf(5, "Run DSP at 200Mhz\n");
          }
          else
          {
             dgprintf(5, "Run DSP at the preset freq\n");
          }

          break;

      case CHIP_AR7O_250_212:
      case CHIP_AR7O_212:   /* Ohio Chip */
      /*
       * Comment out the following code because:
       * - The setting for various clock mode (eg. 1:1 sync mode) is different.
       * - Change setting of these registers in DSLHAL might be too late for other
       *   peripherials that relys on the setting based on bootloader setting.
       * =>These setting should've been done in bootLoader
       */
#if 0
          dgprintf(5, "Start programming PLL for Ohio chip\n");
          DSLHAL_REG32(REG_MIPS_PLLCSR) = 0;
          DSLHAL_REG32(REG_MIPS_PREDIV) = DIVEN | (0 & 0x1F);
          DSLHAL_REG32(REG_MIPS_PLLM)   = 0x00000005;
          shim_osClockWait(64);

          while (DSLHAL_REG32(REG_MIPS_PLLSTAT) & GOSTAT)
             dgprintf(5, "Wait for GOSTAT in PLL STAT to go to 0 -- Step 1\n");

          DSLHAL_REG32(REG_MIPS_POSTDIV)  = DIVEN | (0 & 0x1F);
          DSLHAL_REG32(REG_MIPS_PLLCMDEN) = GOSETEN;
          DSLHAL_REG32(REG_MIPS_PLLCMD)   = GOSET;

          while (DSLHAL_REG32(REG_MIPS_PLLSTAT) & GOSTAT)
             dgprintf(5, "Wait for GOSTAT in PLL STAT to go to 0 -- Step 2\n");

          DSLHAL_REG32(REG_MIPS_PLLCSR)  |= PLLEN;
#endif
          break;

     default:
          return(DSLHAL_ERROR_UNRESET_ADSLSS);
  }


  /*  DSLHAL_REG32(0xa8611600) = 0x007f1bdf;*/
  DSLHAL_REG32(REG_PRCR) |= PRCR_ADSLSS;
  shim_osClockWait(64);
  dgprintf(5,"Brought ADSLSS out of Reset \n");

//  DSLHAL_REG32(REG_GPIOENR) &= ~(BIT20|BIT21|BIT22|BIT23|BIT24|BIT25);
  shim_osClockWait(64);
  dgprintf(5,"Configured GPIO 20-25 for McBSP \n");


  /*  DSLHAL_REG32(0xa8611a04) = 0x00000001;
      shim_osClockWait(64); */

  /*
   * The following code is used to boost CODEC voltage for Sangam250 by setting
   * Buck Trim Bits in CTRL2. For Ohio250, the setting of Buck Trim Bits need
   * to be set in datapump code because each reset of CODEC will clean these
   * Buck Trim Bits.
   * For Ohio(212&250), the following code will be skipped.
   */

  if ((chip_id == CHIP_AR7) && (clock_pllm== 9))
  {
     rc = dslhal_support_hostDspAddressTranslate((unsigned int)0x02040010);
     if (rc== DSLHAL_ERROR_ADDRESS_TRANSLATE)
     {
        dgprintf(1, "dslhal_support_hostDspAddressTranslate failed\n");
        return DSLHAL_ERROR_ADDRESS_TRANSLATE;
     }
     DSLHAL_REG32(rc) |= 0x0000f000; /* set Buck Trim Bits in CTRL2 */
     shim_osClockWait(64);
     dgprintf(5,"Set Buck Voltage for DSP\n");
     dgprintf(6,"Current Contents of PRCR: 0x%x\n",(unsigned int)DSLHAL_REG32(REG_PRCR));
  }

  dgprintf(4," dslhal_support_unresetDslSubsystem done\n");
  return DSLHAL_ERROR_NO_ERRORS;
}


/******************************************************************************************
* FUNCTION NAME:     dslhal_support_resetDslSubsystem
*
*******************************************************************************************
* DESCRIPTION: This function unreset Dsl Subsystem
*
* INPUT:  None
*
* RETURN: 0 if Pass; 1 if Fail
*
*****************************************************************************************/
int  dslhal_support_resetDslSubsystem(void)
{
  dgprintf(4, "dslhal_support_resetDslSubsystem \n");
  /* Put ADSLSS into reset */
  DSLHAL_REG32(REG_PRCR) &= ~PRCR_ADSLSS;
  shim_osClockWait(64);
  /*  DSLHAL_REG32(0xa8611a04) = 0x00000000;
      shim_osClockWait(64); */
  dgprintf(4, "dslhal_support_resetDslSubsystem Done \n");
  return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_support_unresetDsp()
*
*******************************************************************************************
* DESCRIPTION: This fuction takes ax5 daugter board out of reset.
*
* INPUT:  None
*
* RETURN: 0  --successful.
*         1  --failed
*
*****************************************************************************************/
int dslhal_support_unresetDsp(void)
{
#ifdef PRE_SILICON
  /* unsigned char value; */

  rc=dslhal_support_hostDspAddressTranslate((unsigned int)DEV_MDMA0_SRC_ADDR);
  if(rc==DSLHAL_ERROR_ADDRESS_TRANSLATE)
    {
    dgprintf(1, "dslhal_support_hostDspAddressTranslate failed\n");
    return DSLHAL_ERROR_ADDRESS_TRANSLATE;
    }
  dgprintf(5,"MDMA SRC: %08x\n", rc);
  DSLHAL_REG32(rc) = 0x80000001;
  rc=dslhal_support_hostDspAddressTranslate((unsigned int)DEV_MDMA0_DST_ADDR);
  if(rc==DSLHAL_ERROR_ADDRESS_TRANSLATE)
    {
    dgprintf(1, "dslhal_support_hostDspAddressTranslate failed\n");
    return DSLHAL_ERROR_ADDRESS_TRANSLATE;
    }
  dgprintf(5,"MDMA DST: %08x\n", rc);
  DSLHAL_REG32(rc) = 0x02090001;
  rc=dslhal_support_hostDspAddressTranslate((unsigned int)DEV_MDMA0_CTL_ADDR);
  if(rc== DSLHAL_ERROR_ADDRESS_TRANSLATE)
    {
    dgprintf(1, "dslhal_support_hostDspAddressTranslate failed\n");
    return DSLHAL_ERROR_ADDRESS_TRANSLATE;
    }
  dgprintf(5,"MDMA CTL: %08x\n", rc);
  DSLHAL_REG32(rc) = (DEV_MDMA_START | DEV_MDMA_DST_INC | DEV_MDMA_SRC_INC |
                         DEV_MDMA_BURST1 | (1 << DEV_MDMA_LEN_SHF));
  /* statusMask = 0x00000010;*/
#else
  dgprintf(4, "dslhal_support_unresetDsp()\n");

  /* Bring the DSP core out of reset */
  /* DSLHAL_REG32(0xa8611600) = 0x00ff1bdf; */
  DSLHAL_REG32(REG_PRCR) |= PRCR_DSP;
//  shim_osClockWait(64);
  dgprintf(5,"Brought DSP out of Reset \n");

  /* DSLHAL_REG32(0xa8611a0c) = 0x00000007;
     shim_osClockWait(64); */
#endif

  dgprintf(4, "dslhal_support_unresetDsp() done\n");
  return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_support_resetDsp()
*
*******************************************************************************************
* DESCRIPTION: This fuction takes ax5 daugter board into reset.
*
* INPUT:  None
*
* RETURN: 0  --successful.
*         1  --failed
*
*****************************************************************************************/
int dslhal_support_resetDsp(void)
{
  dgprintf(4, "dslhal_support_resetDsp \n");
  /* Put ADSLSS into reset */
  DSLHAL_REG32(REG_PRCR) &= ~PRCR_DSP;
  shim_osClockWait(64);
  dgprintf(4, "dslhal_support_resetDsp Done \n");
  return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_setDsp250MHzTrigger(void)
*
*********************************************************************************************
* DESCRIPTION:
*   Set the trigger to run DSP at 250Mhz.
*
* Input:  none
*
* Return: >=0, success
*         -1,  failure
*
*
********************************************************************************************/
int dslhal_support_setDsp250MHzTrigger(tidsl_t *ptidsl)
{
  DEV_HOST_oamWrNegoParaDef_t  NegoParm;
  DEV_HOST_dspOamSharedInterface_t dspOamSharedInterface, *pdspOamSharedInterface;
  int rc;

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterface_t *)(ptidsl->pmainAddr);

  /* get the pointer to DSP-OAM Shared Interface */
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,
                                     sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.oamWriteNegoParams_p,&NegoParm, sizeof(DEV_HOST_oamWrNegoParaDef_t));

  if (rc)
  {
    dgprintf(5, "dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  NegoParm.mhzFlag = 1;

  rc = dslhal_support_blockWrite(&NegoParm,
                                 (PVOID)dspOamSharedInterface.oamWriteNegoParams_p,
                                 sizeof(DEV_HOST_oamWrNegoParaDef_t));

  if(rc)
  {
    dgprintf(5, "dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_WRITE;
  }

  return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_hostDspAddressTranslate()
*
*********************************************************************************************
* DESCRIPTION:
*   Maps ax5 daugter card dsp memory address to avalanche memory space
*
* Input: unsigned int addr,  dsp memory address.
*
* Return: >=0, unsigned int, mapped Avalanche address(VBUS address).
*         -1,  mapping failed
*
*
********************************************************************************************/
/* static unsigned int bbifmap0,bbifmap1; */

unsigned int dslhal_support_hostDspAddressTranslate( unsigned int addr )
{
  unsigned int addrMap;
  unsigned int chipID=0x05;
  /* This function should just be used to move the memory window of the ADSLSS */
  dgprintf(6, "dslhal_support_hostDspAddressTranslate()\n");
  chipID = DSLHAL_REG32(REG_CVR) & 0xFFFF;
  dgprintf(6, "Chip Version %x\n",chipID);
  /*  select vbus to xbus memory  */
  /*  addrMap = addr & 0xff000000; */
  addrMap = addr & ADSLSSADRMASK;
  dgprintf(6, "dslhal_support_hostDspAddressTranslate() done\n");

  if ( ((chipID == CHIP_AR7O_212) ||(chipID == CHIP_AR7O_250_212))
      &&((addrMap == DSP_PMEM_MASK)||(addrMap == DSP_DMEM_MASK)))
    {
      switch(addrMap)
        {
        case DSP_PMEM_MASK:
#ifdef PRE_SILICON
                            return(ADSLSS2_BASE | (~ADSLSSADRMASK & addr)+ 0x00000100);
#else
                            return(ADSLSS2_BASE | (~ADSLSSADRMASK & addr));
#endif
                            break;
        case DSP_DMEM_MASK:
#ifdef PRE_SILICON
                            return(ADSLSS3_BASE | (~ADSLSSADRMASK & addr)+ 0x00000100);
#else
                            return(ADSLSS3_BASE | (~ADSLSSADRMASK & addr));
#endif
                            break;
        }
    }
  else
    {
      /* AR7 case & default case, neither PMEM nor DMEM for AR7O */
      DSLHAL_REG32(ADSLSSADR) = addrMap;
#ifdef PRE_SILICON
  /* Added 0x100 for Pre-Silicon VLNQ offset.. to be removed for Silicon */
      return ((ADSLSS_BASE | (~ADSLSSADRMASK & addr))+ 0x00000100);
#else
      return ((ADSLSS_BASE | (~ADSLSSADRMASK & addr)));
#endif
    }
  return DSLHAL_ERROR_ADDRESS_TRANSLATE;
}


/******************************************************************************************
* FUNCTION NAME:    dslhal_support_blockWrite
*
*******************************************************************************************
* DESCRIPTION: This rouin simulates DSP memory write as done in ax5 pci nic card
*
* INPUT:  void *buffer, data need to written
*         void *adde,   memory address to be written
*         size_t count, number of bytes to be written
*
* RETURN:  0 --succeeded
*          1 --Failed
*
*****************************************************************************************/
#if 0 //cph org code
int dslhal_support_blockWrite(void *buffer, void *addr, size_t count)
{
  int rc, byteCnt=0;
  unsigned char* ptr;
  union
    {
    unsigned char *cptr;
    short *sptr;
    int *iptr;
    } src;
  union
    {
    int anint;                   /* DSP location */
    unsigned char *cptr;         /* to avoid casts */
    } dst;
  union
    {
    unsigned int anint;
    unsigned char byte[4];
    }data,dword,sword;

  /* Enter Critical Section */
  shim_osCriticalEnter();

  dgprintf(6, "dslhal_support_blockWrite\n");

  dgprintf(6,"addr=0x%X, length=0x%X, buffer=0x%X\n", (unsigned int) addr, (unsigned int) count, (unsigned int)buffer);

  src.cptr = (unsigned char*) buffer;      /* local buffer */
  dst.cptr = addr;        /* DSP memory location */

  /*Maps address first*/
  rc=dslhal_support_hostDspAddressTranslate((unsigned int)addr);
  dgprintf(5, "NewAddr: %08x\n", rc);
  if(rc== DSLHAL_ERROR_ADDRESS_TRANSLATE)
    {
    dgprintf(1, "dslhal_support_hostDspAddressTranslate failed\n");
    return  DSLHAL_ERROR_ADDRESS_TRANSLATE;
    }

  dst.cptr=(unsigned char *)rc;

  /* check wether address is at 32bits boundary */

  if ((dst.anint & 0x3) && count)
    {
    sword.anint = *(unsigned int*)((unsigned int)src.cptr & 0xfffffffc);  // aligned read source
    dword.anint = DSLHAL_REG32((unsigned int)dst.cptr & 0xfffffffc);      // aligned read destination
    sword.anint = (unsigned int) dslhal_support_byteSwap32(sword.anint);
    dword.anint = (unsigned int) dslhal_support_byteSwap32(dword.anint);
    ptr = (unsigned char *)((unsigned int)dst.cptr & 0xfffffffc);


    if((dst.anint & 3) ==3)     /* last byte of a dword */
      {
      dword.byte[3] = sword.byte[3];
      dst.anint++;            /* bump the address by one */
      byteCnt++;
      count--;
      }

    if((dst.anint & 3) ==1)     /* second byte */
      {
        if(count>3)
          {
            dword.byte[3] = sword.byte[3];
            dst.anint++;
            count--;
            byteCnt++;
          }
        if(count>2)
          {
            dword.byte[2] = sword.byte[2];
            dst.anint++;
            count--;
            byteCnt++;
          }
        if(count)
          {
            dword.byte[1] = sword.byte[1];
            dst.anint++;
            count--;
            byteCnt++;
          }
      }

    if((dst.anint & 3) && (count >1))
      {
      dword.byte[2] = sword.byte[2];
      dword.byte[3] = sword.byte[3];
      byteCnt+=2;
      dst.anint += 2;         /* bump the address by two */
      count -= 2;             /* decrement the byte count by two */
      }

    if((dst.anint & 3) && (count==1))
      {
      dword.byte[2] = sword.byte[2];
      dst.anint++;
      byteCnt++;
      count--;
    }

    src.cptr = (char *)((unsigned int)src.cptr & 0xfffffffc); /* fix 032802 */
    dword.anint = dslhal_support_byteSwap32(dword.anint);
    DSLHAL_REG32((unsigned int)ptr) = dword.anint;
    /* Removed by Ram 09-20-2004 */
    /*
    ptr = src.cptr;
    for(rc=0;rc<count;rc++)
      {
        *ptr = *(ptr+byteCnt);
        ptr++;
      }
    */
    }

  /* the dst pointer should now be on a 32-bit boundary   */

  while (count > 3)
    {
     DSLHAL_REG32((unsigned int)dst.cptr) = dslhal_support_byteSwap32(*src.iptr);
     src.iptr++;                  /* bump the data pointer by four */
     dst.anint += 4;              /* bump the address by four */
     count -= 4;                  /* decrement the byte count by four */
     }

  /* write remaining bytes */
  if(count)
    {
    int i;

    // do read-modify-write
    data.anint= DSLHAL_REG32((unsigned int)dst.cptr);
    data.anint=dslhal_support_byteSwap32(data.anint);
    for (i=0; i< count; i++)
      {
        data.byte[i]=*(src.cptr+i);
      }
    data.anint=dslhal_support_byteSwap32(data.anint);
    DSLHAL_REG32((unsigned int)dst.cptr) = data.anint;
    src.cptr +=count;
    dst.anint +=count;
    count=0;
    }
  dgprintf(6, "dslhal_support_blockWrite done\n");
  /* Exit Critical Section */
  shim_osCriticalExit();
  return DSLHAL_ERROR_NO_ERRORS;
}   /* end of dslhal_support_blockWrite() */
#endif

int dslhal_support_blockWrite(void *buffer, void *addr, size_t count)
{
  int rc; //, byteCnt=0;
//  unsigned char* ptr;
  union
    {
    unsigned char *cptr;
    short *sptr;
    int *iptr;
    } src;
  union
    {
    int anint;                   /* DSP location */
    unsigned char *cptr;         /* to avoid casts */
    } dst;
  union
    {
    unsigned int anint;
    unsigned char byte[4];
  }data,dsttmp; //,srctmp;
  int misaligned_flag, i;
  unsigned int trailing_misaligned_bytes;
  unsigned int misaligned_bytes;

  if (count==0) return DSLHAL_ERROR_NO_ERRORS; // do nothing

  /* Enter Critical Section */
  shim_osCriticalEnter();

  dgprintf(6, "dslhal_support_blockWrite\n");

  dgprintf(6,"addr=0x%X, length=0x%X, buffer=0x%X\n", (unsigned int) addr, (unsigned int) count, (unsigned int)buffer);

  src.cptr = (unsigned char*) buffer;      /* local buffer */
  dst.cptr = addr;        /* DSP memory location */
  misaligned_flag = (((unsigned int) addr&0x03) || ((unsigned int) buffer&0x03)) ? 1: 0;

  /*Maps address first*/
  rc=dslhal_support_hostDspAddressTranslate((unsigned int)addr);
  dgprintf(5, "NewAddr: %08x\n", rc);
  if(rc== DSLHAL_ERROR_ADDRESS_TRANSLATE)
    {
    dgprintf(1, "dslhal_support_hostDspAddressTranslate failed\n");
    return  DSLHAL_ERROR_ADDRESS_TRANSLATE;
    }

  dst.cptr=(unsigned char *)rc;

  /* check wether address is at 32bits boundary */
#if 1 //cph optimize
  if (misaligned_flag)
  { //
    // For misaligned dst, do read-modify-write to preserve the un-written data
    // For misaligned src, always use byte handling
    //
    unsigned int pre_aligned_addr=(unsigned int)dst.cptr & 0xfffffffc;
    dsttmp.anint = DSLHAL_REG32(pre_aligned_addr);      // aligned read destination
//    trailing_misaligned_bytes = (count+(dst.anint&3)) & 3;

    misaligned_bytes = count+(dst.anint&3);
    if (misaligned_bytes<4)
      trailing_misaligned_bytes = 0;
    else
      trailing_misaligned_bytes = misaligned_bytes & 3;

    for (i=(dst.anint&3); i<4; i++, count--, dst.anint++)
    {
      if (count==0) break; // in case the data can't even fill up the first aligned-word
      dsttmp.byte[i]= *src.cptr++;
    }
    DSLHAL_REG32(pre_aligned_addr) = dslhal_support_byteSwap32(dsttmp.anint);

    for (i=0; count>0; count--)
    {
      dsttmp.byte[i++]=*src.cptr++;
      i &=3;
      if (i==0)
      {
        // for each aligned 4 bytes do output
        dsttmp.anint=dslhal_support_byteSwap32(dsttmp.anint);
        DSLHAL_REG32((unsigned int)dst.cptr) = dsttmp.anint; // dst.cptr must be aligned
        dst.anint+=4;
        if (count ==(trailing_misaligned_bytes+1))
        {
          // read modify write
          dsttmp.anint = DSLHAL_REG32((unsigned int) dst.anint);
        }
      }
    }
    if (trailing_misaligned_bytes)
    {
      dsttmp.anint=dslhal_support_byteSwap32(dsttmp.anint);
      DSLHAL_REG32((unsigned int)dst.cptr) = dsttmp.anint; // dst.cptr must be aligned
    }
  }
  else //dst & src address already aligned, (dst.anint & 0x3)==0
  {
    while (count > 3)
    {
      DSLHAL_REG32((unsigned int)dst.cptr) = dslhal_support_byteSwap32(*src.iptr);
      src.iptr++;                  /* bump the data pointer by four */
      dst.anint += 4;              /* bump the address by four */
      count -= 4;                  /* decrement the byte count by four */
     }
    /* write remaining bytes */
    if(count)
    {
      int i;

    // do read-modify-write
      data.anint= DSLHAL_REG32((unsigned int)dst.cptr);
      data.anint=dslhal_support_byteSwap32(data.anint);
      for (i=0; i< count; i++)
      {
          data.byte[i]=*(src.cptr+i);
      }
      data.anint=dslhal_support_byteSwap32(data.anint);
      DSLHAL_REG32((unsigned int)dst.cptr) = data.anint;
    }
  }



#else

  if ((dst.anint & 0x3) && count)
  {
    srctmp.anint = *(unsigned int*)((unsigned int)src.cptr & 0xfffffffc); // aligned read source
    dsttmp.anint = DSLHAL_REG32((unsigned int)dst.cptr & 0xfffffffc);     // aligned read destination

    sword.anint = (unsigned int) dslhal_support_byteSwap32(sword.anint);
    dword.anint = (unsigned int) dslhal_support_byteSwap32(dword.anint);
    ptr = (unsigned char *)((unsigned int)dst.cptr & 0xfffffffc);


    if((dst.anint & 3) ==3)     /* last byte of a dword */
      {
      dword.byte[3] = sword.byte[3];
      dst.anint++;            /* bump the address by one */
      byteCnt++;
      count--;
      }

    if((dst.anint & 3) ==1)     /* second byte */
      {
        if(count>3)
          {
            dword.byte[3] = sword.byte[3];
            dst.anint++;
            count--;
            byteCnt++;
          }
        if(count>2)
          {
            dword.byte[2] = sword.byte[2];
            dst.anint++;
            count--;
            byteCnt++;
          }
        if(count)
          {
            dword.byte[1] = sword.byte[1];
            dst.anint++;
            count--;
            byteCnt++;
          }
      }

    if((dst.anint & 3) && (count >1))
      {
      dword.byte[2] = sword.byte[2];
      dword.byte[3] = sword.byte[3];
      byteCnt+=2;
      dst.anint += 2;         /* bump the address by two */
      count -= 2;             /* decrement the byte count by two */
      }

    if((dst.anint & 3) && (count==1))
      {
      dword.byte[2] = sword.byte[2];
      dst.anint++;
      byteCnt++;
      count--;
    }

    src.cptr = (char *)((unsigned int)src.cptr & 0xfffffffc); /* fix 032802 */
    dsttmp.anint = dslhal_support_byteSwap32(dsttmp.anint);
    DSLHAL_REG32((unsigned int)ptr) = dsttmp.anint;
    /* Removed by Ram 09-20-2004 */
    /*
    ptr = src.cptr;
    for(rc=0;rc<count;rc++)
      {
        *ptr = *(ptr+byteCnt);
        ptr++;
      }
    */
    }

  /* the dst pointer should now be on a 32-bit boundary   */

  while (count > 3)
    {
     DSLHAL_REG32((unsigned int)dst.cptr) = dslhal_support_byteSwap32(*src.iptr);
     src.iptr++;                  /* bump the data pointer by four */
     dst.anint += 4;              /* bump the address by four */
     count -= 4;                  /* decrement the byte count by four */
     }

  /* write remaining bytes */
  if(count)
    {
    int i;

    // do read-modify-write
    data.anint= DSLHAL_REG32((unsigned int)dst.cptr);
    data.anint=dslhal_support_byteSwap32(data.anint);
    for (i=0; i< count; i++)
      {
        data.byte[i]=*(src.cptr+i);
      }
    data.anint=dslhal_support_byteSwap32(data.anint);
    DSLHAL_REG32((unsigned int)dst.cptr) = data.anint;
    src.cptr +=count;
    dst.anint +=count;
    count=0;
  }
#endif

  dgprintf(6, "dslhal_support_blockWrite done\n");
  /* Exit Critical Section */
  shim_osCriticalExit();
  return DSLHAL_ERROR_NO_ERRORS;
}   /* end of dslhal_supp */

/******************************************************************************************
* FUNCTION NAME:    dslhal_support_blockRead
*
*********************************************************************************************
* DESCRIPTION: This rouin simulates DSP memory read as done in ax5 pci nic card
*
* INPUT:  void *addr,   memory address to be read
*         void *buffer, dat buffer to be filled with from memmory
*         size_t count, number of bytes to be read
*
* RETURN:  0 --succeeded
*          1 --Failed
*
*****************************************************************************************/

int dslhal_support_blockRead(void *addr, void *buffer, size_t count)
{
    int rc;
    union
        {
        int anint;          /* DSP location */
        char *cptr;         /* to avoid casts */
        } src;
    union
        {
        char byte[4];
        int anint;          /* DSP data */
        } data;
    union
        {
        char *cptr;
        int *iptr;
      } dst;
    unsigned int anword;
    int misaligned_flag, i;

    /* Enter Critical Section */
    shim_osCriticalEnter();

    dgprintf(6,"dslhal_support_blockRead\n");

    //
    // Check if any of dsp & mips buffer is not 32bit aligned.
    // Need byte process later on if mis-aligned (misalign_flag=1)
    //
    misaligned_flag = (((unsigned int) addr&0x03) || ((unsigned int) buffer&0x03)) ? 1: 0;

    src.cptr = addr;        /* DSP memory location */
    dst.cptr = buffer;      /* local buffer */

    dgprintf(6, "Read addr=0x%X, size=0x%X, dst.cptr=%08x\n", (unsigned int)addr, count, dst.cptr);


    /*Maps address first*/
    rc=dslhal_support_hostDspAddressTranslate((unsigned int)addr);
    if(rc== DSLHAL_ERROR_ADDRESS_TRANSLATE)
      {
      dgprintf(1, "dslhal_support_hostDspAddressTranslate failed\n");
      return DSLHAL_ERROR_ADDRESS_TRANSLATE;
      }

    src.cptr=(unsigned char *)rc;

    /**********************************************
    *  if the source is NOT on a 32-bit boundary  *
    *  then we read the full word                 *
    *  and ignore the first part of it            *
    **********************************************/

    if ((src.anint & 3) && count)
    {
      anword = DSLHAL_REG32((unsigned int)src.cptr & 0xfffffffc);
      data.anint = dslhal_support_byteSwap32(anword);

      /************************************
      *  there is no need for case 0      *
      *  notice that there are no breaks  *
      *  each falls through to the next   *
      ************************************/
      //cph Optimize
      for(i=(src.anint & 3);i<4; i++, src.anint++, count--)
      {
        if (count==0) break;  // aligned case don't need byte treament
        *(dst.cptr++) = data.byte[i];
      }
    }

    /* the src pointer should now be on a 32-bit boundary */
    if (misaligned_flag)
    {
      while (count > 3)
      {
        // since src is 32 bit aligned now, we can use 32 bit read
        anword=DSLHAL_REG32((unsigned int)src.cptr);
        data.anint = dslhal_support_byteSwap32(anword);

        // destination not aligned, need byte process
        dst.cptr[0]= data.byte[0];
        dst.cptr[1]= data.byte[1];
        dst.cptr[2]= data.byte[2];
        dst.cptr[3]= data.byte[3];

        src.anint += 4;         /* bump the address by four */
        dst.cptr+=4;
        count -= 4;             /* decrement the byte count by four */
      }
    }
    else
    {
      while (count > 3)
      {
        anword=DSLHAL_REG32((unsigned int)src.cptr);
        *dst.iptr = dslhal_support_byteSwap32(anword);
        src.anint += 4;         /* bump the address by four */
        dst.iptr++;             /* bump the data pointer by four */
        count -= 4;             /* decrement the byte count by four */
      }
    }

    /*******************************
    *  if there's any count left   *
    *  then we read the next word  *
    *  and ignore the end of it    *
    *******************************/


  //cph optimize. count is less than 4
  if (count)
  {
    anword = DSLHAL_REG32((unsigned int)src.cptr);
    data.anint = dslhal_support_byteSwap32(anword);

    for (i=0; i<count; i++)
      dst.cptr[i] = data.byte[i];
  }

  /* Exit Critical Section */
  shim_osCriticalExit();

  return DSLHAL_ERROR_NO_ERRORS;

}   /* end of dslhal_support_blockRead() */


/********************************************************************************************
* FUNCTION NAME:    dslhal_support_readDspMailbox
*
*********************************************************************************************
* DESCRIPTION: Reads a message from the mailbox
*
* ARGUMENTS:   int *pcmd           Pointer to command read
*
* RETURNS:     0 if successful
*              1 if no mail
*              NZ otherwise
*
*****************************************************************************************/

int dslhal_support_readDspMailbox(tidsl_t *ptidsl, int *pcmd, int *ptag, int *pprm1, int *pprm2)
{
  int rc;
  int cmd;
  int tag;
  int prm1;
  int prm2;
  unsigned char dspOutInx;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  DEV_HOST_mailboxControl_t mailboxControl;
  DEV_HOST_dspHostMsg_t dspMailboxMsg[DEV_HOST_DSPQUEUE_LENGTH];

  dgprintf(6,"dslhal_support_readDspMailbox\n");

  /* get the DSP main pointer */

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,
                                     sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  /* Read in the command/response buffer */
  dspOamSharedInterface.dspHostMailboxControl_p = (DEV_HOST_mailboxControl_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspHostMailboxControl_p);


  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspHostMailboxControl_p,
                               &mailboxControl, sizeof(DEV_HOST_mailboxControl_t));

  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  /* Change the endianness of the Mailbox Pointer */
  mailboxControl.dspMsgBuf_p = (DEV_HOST_dspHostMsg_t *) dslhal_support_byteSwap32((unsigned int)mailboxControl.dspMsgBuf_p);

  rc = dslhal_support_blockRead((PVOID)mailboxControl.dspMsgBuf_p,
                                     &dspMailboxMsg, (sizeof(DEV_HOST_dspHostMsg_t)*DEV_HOST_DSPQUEUE_LENGTH));

  if (rc)
    return DSLHAL_ERROR_BLOCK_READ;
  /* Extract the command/response message index */
  mailboxControl.hostInInx &= 7;
  mailboxControl.hostOutInx &= 7;
  mailboxControl.dspOutInx &= 7;
  mailboxControl.dspInInx &= 7;


  /* check for messages in the mailbox */

  if (mailboxControl.dspOutInx == mailboxControl.dspInInx)
    {
    return DSLHAL_ERROR_MAILBOX_NOMAIL;
    /* no messages to read */
    }

  /* use bDRESPOutInx as index to DRESPMsgBuf */

  cmd = dspMailboxMsg[mailboxControl.dspOutInx].cmd;
  tag = dspMailboxMsg[mailboxControl.dspOutInx].tag;
  prm1= dspMailboxMsg[mailboxControl.dspOutInx].param1;
  prm2= dspMailboxMsg[mailboxControl.dspOutInx].param2;

  mailboxControl.dspOutInx++;      /* increment count */
  mailboxControl.dspOutInx &= 7;   /* only two bits */

  dspOutInx = mailboxControl.dspOutInx;

  /* Read in the command response buffer again to take care of changes */
  mailboxControl.dspOutInx = dspOutInx;
  rc = dslhal_support_blockWrite(&mailboxControl.dspOutInx,
                                      &dspOamSharedInterface.dspHostMailboxControl_p->dspOutInx, sizeof(BYTE));

  if (rc)
    return DSLHAL_ERROR_BLOCK_WRITE;

  /* Is the input parameter address non-zero*/

  if (pcmd)
    {
    *pcmd = cmd;
    }
  if (ptag)
    {
    *ptag = tag;
    }
  if (pprm1)
    {
    *pprm1 = prm1;
    }
  if (pprm2)
    {
    *pprm2 = prm2;
    }

  dgprintf(6,"dslhal_support_readDspMailbox done\n");
  dgprintf(6,"cmd=%d, tag=%d\n", cmd, tag);
  dgprintf(6,"dslhal_support_readDspMailbox:cmd: 0x%x, tag=%d\n", cmd, tag);
  return DSLHAL_ERROR_NO_ERRORS;

} /* end of dslhal_support_readDspMailbox() */

/*******************************************************************************************
* FUNCTION NAME:    dslhal_support_writeHostMailbox
*
********************************************************************************************
* DESCRIPTION: Send a message to a mailbox
*
* ARGUMENTS:   int cmd             command to write
*              int tag             tag (currently unused)
*              int p1              parameter 1 (currently unused)
*              int p2              parameter 2 (currently unused)
*
* RETURNS:     0 if successful
*              NZ otherwise
*
*******************************************************************************************/

int dslhal_support_writeHostMailbox(tidsl_t *ptidsl, int cmd, int tag, int p1, int p2)
{
  int rc;
  int index;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  DEV_HOST_mailboxControl_t mailboxControl;
  DEV_HOST_dspHostMsg_t hostMailboxMsg[DEV_HOST_HOSTQUEUE_LENGTH];
  unsigned char hostInInx;

  dgprintf(6,"dslhal_support_writeHostMailbox:cmd: 0x%x, tag=%d\n", cmd, tag);

  dgprintf(6,"cmd=%d, tag=%d\n", cmd, tag);

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,
                                sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
      dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  /* Read in the command/response buffer */
  dspOamSharedInterface.dspHostMailboxControl_p = (DEV_HOST_mailboxControl_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspHostMailboxControl_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspHostMailboxControl_p,
                                &mailboxControl, sizeof(DEV_HOST_mailboxControl_t));

  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  /* Change the endianness of the Mailbox Control Pointer */
  mailboxControl.hostMsgBuf_p = (DEV_HOST_dspHostMsg_t *) dslhal_support_byteSwap32((unsigned int)mailboxControl.hostMsgBuf_p);
  rc = dslhal_support_blockRead((PVOID)mailboxControl.hostMsgBuf_p,
                                &hostMailboxMsg, (sizeof(DEV_HOST_dspHostMsg_t)*DEV_HOST_HOSTQUEUE_LENGTH));

  if (rc)
    return DSLHAL_ERROR_BLOCK_READ;
  /* Extract the command/response message index */
  mailboxControl.hostInInx &= 7;
  mailboxControl.hostOutInx &= 7;
  mailboxControl.dspOutInx &= 7;
  mailboxControl.dspInInx &= 7;

  /* make sure there's room in the mailbox */

  index = mailboxControl.hostInInx;
  mailboxControl.hostInInx++;
  mailboxControl.hostInInx &= 7;
  hostInInx = mailboxControl.hostInInx;
  if (mailboxControl.hostInInx == mailboxControl.hostOutInx)
    {
      /* mailbox is full */
      return DSLHAL_ERROR_MAILBOX_OVERFLOW;
    }

  /* use bOCMDInInx as index to OCMDMsgBuf */
  hostMailboxMsg[index].cmd = (BYTE) cmd;
  hostMailboxMsg[index].tag = (BYTE) tag;
  hostMailboxMsg[index].param1 = (BYTE) p1;
  hostMailboxMsg[index].param2 = (BYTE) p2;
  rc = dslhal_support_blockWrite(&hostMailboxMsg,
                                 (PVOID)mailboxControl.hostMsgBuf_p,
                                 sizeof(DEV_HOST_dspHostMsg_t)*DEV_HOST_HOSTQUEUE_LENGTH);
  if (rc)
    {
      dgprintf(1,"dslhal_support_blockWrite failed\n");
      return DSLHAL_ERROR_BLOCK_READ;
    }
#if 0  // 10/6/05 CPH, comment out the follwing redundent code that could cause crash.
  rc = dslhal_support_blockWrite(&mailboxControl,
                                 &dspOamSharedInterface.dspHostMailboxControl_p,
                                 sizeof(DEV_HOST_mailboxControl_t));
  if (rc)
    return DSLHAL_ERROR_BLOCK_WRITE;
#endif
  /* update the index */
  mailboxControl.hostInInx = hostInInx;
  rc = dslhal_support_blockWrite(&mailboxControl.hostInInx,
                                 &dspOamSharedInterface.dspHostMailboxControl_p->hostInInx,
                                 sizeof(BYTE));
  if (rc)
    return DSLHAL_ERROR_BLOCK_WRITE;

  dgprintf(6,"dslhal_support_writeHostMailbox done\n");
 return DSLHAL_ERROR_NO_ERRORS;

}
/* end of dslhal_support_writeHostMailbox() */


/********************************************************************************************
* FUNCTION NAME:    dslhal_support_readTextMailbox
*
*********************************************************************************************
* DESCRIPTION: Reads a message from the mailbox
*
* ARGUMENTS:   int *pcmd           Pointer to command read
*
* RETURNS:     0 if successful
*              1 if no mail
*              NZ otherwise
*
*****************************************************************************************/

int dslhal_support_readTextMailbox(tidsl_t *ptidsl, int *pmsg1, int *pmsg2)
{
  int rc;
  unsigned int msg1;
  unsigned int msg2;
  unsigned char textOutInx;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  DEV_HOST_mailboxControl_t mailboxControl;
  DEV_HOST_textMsg_t textMailboxMsg[DEV_HOST_TEXTQUEUE_LENGTH];

  dgprintf(6,"dslhal_support_readTextMailbox\n");

  /* get the DSP main pointer */

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,
                                     sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  /* Read in the command/response buffer */
  dspOamSharedInterface.dspHostMailboxControl_p = (DEV_HOST_mailboxControl_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspHostMailboxControl_p);


  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspHostMailboxControl_p,
                               &mailboxControl, sizeof(DEV_HOST_mailboxControl_t));

  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  /* Change the endianness of the Mailbox Pointer */
  mailboxControl.textMsgBuf_p = (DEV_HOST_textMsg_t *) dslhal_support_byteSwap32((unsigned int)mailboxControl.textMsgBuf_p);

  rc = dslhal_support_blockRead((PVOID)mailboxControl.textMsgBuf_p,
                                     &textMailboxMsg, (sizeof(DEV_HOST_textMsg_t)*DEV_HOST_DSPQUEUE_LENGTH));

  if (rc)
    return DSLHAL_ERROR_BLOCK_READ;
  /* Extract the command/response message index */

  mailboxControl.textInInx &= 7;
  mailboxControl.textOutInx &= 7;

  /* check for messages in the mailbox */

  if (mailboxControl.textOutInx == mailboxControl.textInInx)
    {
    return DSLHAL_ERROR_MAILBOX_NOMAIL;
    /* no messages to read */
    }

  /* use bDRESPOutInx as index to DRESPMsgBuf */

  msg1 = textMailboxMsg[mailboxControl.textOutInx].msgPart1;
  msg2 = textMailboxMsg[mailboxControl.textOutInx].msgPart2;
  msg1 = (unsigned int) dslhal_support_byteSwap32((unsigned int)msg1);
  msg2 = (unsigned int) dslhal_support_byteSwap32((unsigned int)msg2);

  mailboxControl.textOutInx++;      /* increment count */
  mailboxControl.textOutInx &= 7;   /* only two bits */

  textOutInx = mailboxControl.textOutInx;

  /* Read in the command response buffer again to take care of changes */

  mailboxControl.textOutInx = textOutInx;

  rc = dslhal_support_blockWrite(&mailboxControl.textOutInx,
                                      &dspOamSharedInterface.dspHostMailboxControl_p->textOutInx, sizeof(BYTE));

  if (rc)
    return DSLHAL_ERROR_BLOCK_WRITE;

  /* Is the input parameter address non-zero*/

  if (pmsg1)
    {
    *pmsg1 = msg1;
    }
  if (pmsg2)
    {
    *pmsg2 = msg2;
    }

  dgprintf(6,"dslhal_support_readTextMailbox done\n");
  dgprintf(6,"msgPart1=%d, msgPart2=%d\n", msg1, msg2);
  dgprintf(6,"dslhal_support_readTextMailbox:Message Part1: 0x%x, tag=0x%x\n", msg1, msg2);
  return DSLHAL_ERROR_NO_ERRORS;

} /* end of dslhal_support_readTextMailbox() */



/********************************************************************************************
* FUNCTION NAME: dslhal_support_hostDspCodeDownload()
*
*********************************************************************************************
* DESCRIPTION:
*   download DSP image from host memory to dsp memory
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/

int dslhal_support_hostDspCodeDownload(tidsl_t * ptidsl)
{

  unsigned int index, numProfile=0;
  int rc = 0, i;
  unsigned char *iptr;             /* image pointer */
  unsigned int numbytes,olayXfer,olayStore;
  /*  unsigned int holdSecPhyAddr=0,holdSecVirtAddr; */
  unsigned int *olayStart;
  size_t len;             /* size of the file */
  size_t expoffset;       /* expected offset for next section header */
  unsigned short checksum;
  unsigned int crc32;
  unsigned char * image;
  char *tmp = (char *)DEV_HOST_DSP_OAM_POINTER_LOCATION;
  DEV_HOST_dspVersionDef_t        dspVersion;
#if SWTC
  DEV_HOST_tcHostCommDef_t        TCHostCommDef;
#endif
  DEV_HOST_oamWrNegoParaDef_t     OamWrNegoParaDef;
  DEV_HOST_dspOamSharedInterface_t dspOamSharedInterface, *pdspOamSharedInterface;
  DEV_HOST_olayDpDef_t            olayDpParms;
  DEV_HOST_profileBase_t          profileList;
#ifndef NO_ACT
  DEV_HOST_consBufDef_t           constDisp;
#endif
#if CO_PROFILES
  DEV_HOST_coData_t               coData;
#endif
  DEV_HOST_olayDpPageDef_t        olayDpPageDef[32];
  union
  {
  char byte[4];
  unsigned short hword[2];
  unsigned int aword;
  } data;

  struct _header
  {
  char signature[6];
  unsigned short sectcount;
  unsigned int length;
  } header;

  struct _section
  {
  unsigned int addr;
  unsigned int length;
  unsigned int offset;
  unsigned int page;
  };/* section[MAXSECTIONS]; */

  struct _section *sptr;
  unsigned int secAddr, secLength, secOffset, secPage;
  unsigned int adjSecLength;
  unsigned int read_retry = NUM_READ_RETRIES;

  dgprintf(5,"dslhal_support_hostDspCodeDownload\n");
  image = ptidsl->fwimage;

  if (!image)
    {
      dgprintf(1,"no image file\n");
    return DSLHAL_ERROR_NO_FIRMWARE_IMAGE;
    }

  iptr=image;

  numbytes = sizeof(header);
  shim_osMoveMemory((char *) &header, (char *)iptr, numbytes);
  header.length = dslhal_support_byteSwap32(header.length);
  header.sectcount = dslhal_support_byteSwap16(header.sectcount);
#if 0
  crc32 = dslhal_support_computeCrc32((unsigned char*)&crcTest[0],20);
  dgprintf(6,"CRC-32 for the crcTest: 0x%x",crc32);
  dgprintf(4,"header.length=%d, header.sectcount=0x%X\n", header.length, header.sectcount);
#endif
  /* point to the checksum */
  /* compute the checksum on CRC32 here */
  iptr = image + header.length-4;
  numbytes = sizeof(data.aword);

  dgprintf(5,"tiload: check checksum\n");
  shim_osMoveMemory((char *)&(data.byte), (char *)iptr, numbytes);

  crc32 = dslhal_support_computeCrc32(image,ptidsl->imagesize);
  dgprintf(5,"CRC-32 for the Binary: 0x%x",crc32);
  /* CRC currently not added to the DSP binary, so this code is commented out */

  data.aword = dslhal_support_byteSwap32(data.aword);
  if (data.aword != crc32)
    {
      dgprintf(1,"Checksum error\n");
    }

  /* Verify signature - Changed from "320C6x" to "TIDSL" for load 80 */

  header.signature[5]='\0';
  dgprintf(5, "signature=%s\n", header.signature);

  if (shim_osStringCmp(header.signature, "TIDSL"))
    {
      dgprintf(1,"Signature not match\n");
    }

  dgprintf(5,"tiload: check sect count\n");
  /* check section count */
  if (header.sectcount > MAXSECTIONS)
    {
      dgprintf(5,"Unusually high Section Count# %d  %d\n", header.sectcount, MAXSECTIONS);
    }
  else
    {
    dgprintf(5,"found %d sections\n", header.sectcount);
    }
  /* Validation of Section offsets */

  /* point to the first section */

  len = header.length;              /* file size in bytes */
  expoffset = sizeof(struct _header) + header.sectcount * sizeof(struct _section);
  ptidsl->numOlayPages = 1;
  ptidsl->numProfiles=0;
  dgprintf(5,"tiload: check offset\n");

  for (index=0; index<header.sectcount; index++) /* parse the sections one by one */
    {
      numbytes = sizeof(struct _header) + index * sizeof(struct _section); /* Compute Section Offset */
      sptr = (struct _section *)(image + numbytes); /* Section Pointer to beginning of the section */

    secAddr   = dslhal_support_byteSwap32(sptr->addr);
    secOffset = dslhal_support_byteSwap32(sptr->offset);
    secLength = dslhal_support_byteSwap32(sptr->length);
    secPage   = dslhal_support_byteSwap32(sptr->page);

    /* validate offset */
    if ( secOffset== 0xffffffff)
      {
      /* special case: zero fill */
      /* offset is valid, don't change expoffset */
      }
    else
      {
      if (secOffset > len-4)
        {
          dgprintf(5,"Offset error\n");
          return DSLHAL_ERROR_FIRMWARE_OFFSET;
        }

      /* determine expected offset of NEXT section  */
      expoffset = secLength + secOffset;

      /* all addresses must be on word boundaries */
      if (secAddr & 3)
        {

        }
      }
    }

  /* check final offset - should just be a checksum left */
/* IMPORTANT 11/24/02 --> Got this error... but bypassed for Pf of Concept*/
  /*
  if (expoffset != len-4)
    {
      dgprintf(5,"Final offset error\n");
      return DSLHAL_ERROR_FIRMWARE_OFFSET;
    }
  */

  /* Actual Code loading to DSP Memory */

  /* Initialize DSP Data Memory before code load*/
  /*
  dgprintf(5,"Zero Prefill DSP DMEM\n");
  DSLHAL_REG32(ADSLSSADR)=0x80000000;
  shim_osZeroMemory((char *)0xa1000000, 0x10000);
  */
  /* Load sections from the image */
  for (index=0; index<header.sectcount; index++) /* Parse each section */
    {
    numbytes = sizeof(header) + index * sizeof(struct _section); /* Compute offset to next section */
    sptr = (struct _section *)(image + numbytes);   /* Point to next section */

    secAddr   = dslhal_support_byteSwap32(sptr->addr);
    secOffset = dslhal_support_byteSwap32(sptr->offset);
    secLength = dslhal_support_byteSwap32(sptr->length);
    secPage   = dslhal_support_byteSwap32(sptr->page);

    data.aword = secAddr;
    checksum += data.byte[0] + data.byte[1] + data.byte[2] + data.byte[3];

    data.aword = secLength;
    checksum += data.byte[0] + data.byte[1] + data.byte[2] + data.byte[3];

    data.aword = secOffset;
    checksum += data.byte[0] + data.byte[1] + data.byte[2] + data.byte[3];

    data.aword = secPage;
    checksum += data.byte[0] + data.byte[1] + data.byte[2] + data.byte[3];


    /* validate offset */
    if (secOffset == 0xffffffff)
      {
      /* special case: zero fill */
      /* offset is valid, don't change expoffset */
      }
    else
      {
      /* real offset */
      if(secOffset > len-4)
        {
        dgprintf(5,"section[%u] offset too big (%X/%X)\n", index,
                secOffset, len-4);

        return DSLHAL_ERROR_FIRMWARE_OFFSET;
        }

      /* determine expected offset of NEXT section */
      expoffset = secLength + secOffset;

      }

    }

  /* check final offset - should just be a checksum left */
  /*
  if(expoffset != len-4)
    {
      dgprintf(1,"sections don't span full file (%X/%X)\n",expoffset,len-2);
      return DSLHAL_ERROR_FIRMWARE_OFFSET;
    }
  */
  dgprintf(5,"tiload: load binary\n");

  for (index=0; index<header.sectcount; index++)
  {
    numbytes = sizeof(header) + index * sizeof(struct _section);
    sptr = (struct _section *)(image + numbytes);

    secAddr   = dslhal_support_byteSwap32(sptr->addr);
    secOffset = dslhal_support_byteSwap32(sptr->offset);
    secLength = dslhal_support_byteSwap32(sptr->length);
    secPage   = dslhal_support_byteSwap32(sptr->page);
    dgprintf(5,"loading section %u\n", index);
    dgprintf(5,"section %u: addr: %X\n", index, secAddr);
    dgprintf(5,"section %u: length: %X\n", index, secLength);
    dgprintf(5,"section %u: offset: %X\n", index, secOffset);
    dgprintf(5,"section %u: page: %X\n", index, secPage);

    /* point to the section's data */
    if(secOffset != 0xffffffff)
    {
      /* Load this section of data */
      iptr = image + secOffset;
      dgprintf(6, "iptr %8x\n", (unsigned int)iptr);
    }

    if(secPage)
    {
      dgprintf(6,"OVERLAY PAGE #%d\n",secPage);
      /* overlay page, don't write to dsp yet, save into host memory*/

      dgprintf(6,"Section Length: %d \n",secLength);
      ptidsl->olayDpPage[secPage].PmemStartWtAddr  = (unsigned int) shim_osAllocateDmaMemory(secLength);
      ptidsl->numOlayPages++;
      if(ptidsl->olayDpPage[secPage].PmemStartWtAddr == NULL)
      {
          dgprintf(1, "overlay page allocate error\n");
          return DSLHAL_ERROR_OVERLAY_MALLOC;
      }
#ifdef PRE_SILICON
      ptidsl->olayDpPage[secPage].overlayHostAddr = ((((ptidsl->olayDpPage[secPage].PmemStartWtAddr)-0x84000000)-0x10000000)+0x030b0000);
#else
      /*      ptidsl->olayDpPage[secPage].overlayHostAddr = ((unsigned int)(ptidsl->olayDpPage[secPage].PmemStartWtAddr)&~0xe0000000); */
      ptidsl->olayDpPage[secPage].overlayHostAddr = virtual2Physical((unsigned int)ptidsl->olayDpPage[secPage].PmemStartWtAddr);
#endif
      dgprintf(6,"Allocated Addr: 0x%x \t Xlated Addr: 0x%x \n",ptidsl->olayDpPage[secPage].PmemStartWtAddr,ptidsl->olayDpPage[secPage].overlayHostAddr);

      ptidsl->olayDpPage[secPage].overlayHostAddr = (unsigned int)dslhal_support_byteSwap32(ptidsl->olayDpPage[secPage].overlayHostAddr);
      ptidsl->olayDpPage[secPage].OverlayXferCount = secLength;
      ptidsl->olayDpPage[secPage].BinAddr          = secAddr;
      ptidsl->olayDpPage[secPage].SecOffset        = secOffset;
      shim_osMoveMemory((char *)ptidsl->olayDpPage[secPage].PmemStartWtAddr, (char *)iptr, secLength);
      /*   RamP Image ByteSwap test */
      olayStart = (unsigned int *)ptidsl->olayDpPage[secPage].PmemStartWtAddr;

      for(olayXfer=0;olayXfer< secLength/4;olayXfer++)
      {
        olayStore = *(unsigned int *)olayStart;
        olayStore = (unsigned int)dslhal_support_byteSwap32(olayStore);
        *(unsigned int*)olayStart = olayStore;
//        dgprintf(5, "Addr:0x%x \t Content: 0x%x \n",olayStart,olayStore);
        dgprintf(7, "Addr:0x%x \t Content: 0x%x \n",olayStart,olayStore);
        olayStart++;
        olayStore=0;
      }
      /* RamP Image ByteSwap test */
      /* compute the CRC of each overlay page and Store the Checksum in a local global variable */
      /* This Value of CRC is to be compared with the header where all the CRC bytes are lumped together */
      ptidsl->olayDpPage[secPage].olayPageCrc32 = dslhal_support_computeCrc32((char *)ptidsl->olayDpPage[secPage].PmemStartWtAddr, ptidsl->olayDpPage[secPage].OverlayXferCount);

      shim_osWriteBackCache((void *)ptidsl->olayDpPage[secPage].PmemStartWtAddr, secLength);
    }
    else
    {
      rc = secAddr&0xff000000;
      if(rc && rc!=0x80000000)
      {
        dgprintf(4,"Not DSP PMEM/DMEM\n");
          /* don't write to dsp, save into host memory*/
        dgprintf(4,"Section Addr: %x Section Length: %d \n",secAddr,secLength);

        //CQ9717 cph fixed odd bytes not swaped problem
        adjSecLength = ((secLength+3)/4)*4; // cph adjusted section length (must be multiple of 4)
        ptidsl->coProfiles[numProfile].PmemStartWtAddr = (unsigned int) shim_osAllocateDmaMemory(adjSecLength);
        ptidsl->numProfiles++;
        if(ptidsl->coProfiles[numProfile].PmemStartWtAddr == NULL)
        {
         dgprintf(1, "memory allocate error\n");
         return DSLHAL_ERROR_OVERLAY_MALLOC;
        }
        ptidsl->coProfiles[numProfile].overlayHostAddr =  virtual2Physical((unsigned int)ptidsl->coProfiles[numProfile].PmemStartWtAddr);
        dgprintf(4,"Num: %d Allocated Addr: 0x%x \t Xlated Addr: 0x%x \n",numProfile, ptidsl->coProfiles[numProfile].PmemStartWtAddr, ptidsl->coProfiles[numProfile].overlayHostAddr);
        ptidsl->coProfiles[numProfile].overlayHostAddr = (unsigned int)dslhal_support_byteSwap32(ptidsl->coProfiles[numProfile].overlayHostAddr);
        ptidsl->coProfiles[numProfile].OverlayXferCount = secLength;
        ptidsl->coProfiles[numProfile].BinAddr = secAddr;
        ptidsl->coProfiles[numProfile].SecOffset = secOffset;

        if (secOffset==0xFFFFFFFF)  //7/20/05, cph virtual address doesn't need move data
        {
          olayStart = (unsigned int *)ptidsl->coProfiles[numProfile].PmemStartWtAddr;

          for(olayXfer=0;olayXfer<adjSecLength/4;olayXfer++)
          {
            dgprintf(7, "Addr:0x%x \t Content:0(ZI)\n",olayStart);
            *olayStart++ = 0; // Zero Init
          }
        }
        else
        {
          shim_osMoveMemory((char *)ptidsl->coProfiles[numProfile].PmemStartWtAddr, (char *)iptr, secLength);

          /*   RamP Image ByteSwap test */
          olayStart = (unsigned int *)ptidsl->coProfiles[numProfile].PmemStartWtAddr;

        //CQ9717, cph fixed odd bytes not swaped problem
          for(olayXfer=0;olayXfer< adjSecLength/4;olayXfer++)
          {
            olayStore = *(unsigned int *)olayStart;
            olayStore = (unsigned int)dslhal_support_byteSwap32(olayStore);
            *(unsigned int*)olayStart = olayStore;
            dgprintf(7, "Addr:0x%x \t Content: 0x%x \n",olayStart,olayStore);
            olayStart++;
            olayStore=0;
          }
        }

        shim_osWriteBackCache((void *)ptidsl->coProfiles[numProfile].PmemStartWtAddr, secLength);
        numProfile++;
        if(numProfile > 6)
          dgprintf(5, "Error in Binary! Too Much Scratch! \n ");
      }
      else
      {
        /* IMPORTANT: write image to DSP memory */
        rc=dslhal_support_blockWrite((void *)iptr, (void *)secAddr, secLength);
        if(rc)
        {
          dgprintf(1,"dslhal_support_blockRead failed\n");
          return DSLHAL_ERROR_BLOCK_READ;
        }
       shim_osClockWait(0x80000);
        /*
          rc=dslhal_support_blockRead((void*)secAddr, (void*)tmpBuffer, secLength);
          if(rc)
          {
            dgprintf(1,"dslhal_support_blockRead failed\n");
            return DSLHAL_ERROR_BLOCK_READ;
          }
          for(i=0;i<secLength;i++)
          {
            if(*iptr
          }*/
      }
    }
  } /* end of write dsp image */

  /***********************************************************************************
   * Start to fillup various values to our hardware structure for late use
   ************************************************************************************/
  dgprintf(5, "Wrote Image; Overlay Pages:%d  Profiles:%d\n",ptidsl->numOlayPages, ptidsl->numProfiles);
  /* get main pointer for data */
  while(read_retry--)
  {
      rc = dslhal_support_blockRead(tmp, &pdspOamSharedInterface, sizeof(unsigned int));
      dgprintf(5, "tmp=0x%X, addr=0x%X\n", (unsigned int)tmp, (unsigned int)pdspOamSharedInterface);
      pdspOamSharedInterface= (DEV_HOST_dspOamSharedInterface_t *)dslhal_support_byteSwap32((unsigned int)pdspOamSharedInterface);
      dgprintf(5, "tmp=0x%X, addr=0x%X\n", (unsigned int)tmp, (unsigned int)pdspOamSharedInterface);

      if (rc)
      {
        dgprintf(1,"dslhal_support_blockRead failed\n");
        return DSLHAL_ERROR_BLOCK_READ;
      }


      if(!pdspOamSharedInterface)
      {
        if(!read_retry)
        {
            dgprintf(1,"Couldn't read main pointer\n");
            return DSLHAL_ERROR_INVALID_PARAM;
        }
        else
            shim_osClockWait(0x100);
      }
      else
        break;
  }

  ptidsl->pmainAddr=pdspOamSharedInterface;

  /* read the OamSharedInterfaceStructure */

  dgprintf(5,"ptidsl->hostIf.mainAddr=0x%X\n", (unsigned int)ptidsl->pmainAddr);

  /* get the pointer to DSP-OAM Shared Interface */
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,
                                     sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  /* Communicate the Allocated Memory Address to DSP to choose CO Profiles */

  /* Change the Endianness of the profileList pointer */
  dspOamSharedInterface.profileList_p = (DEV_HOST_profileBase_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.profileList_p);
  /* Access the profileList Structure */
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.profileList_p,&profileList, sizeof(DEV_HOST_profileBase_t));

  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  dgprintf(2,"Old Addr:%x  New: %x \n",profileList.hostProfileBase_p,ptidsl->coProfiles[0].overlayHostAddr);
  profileList.hostProfileBase_p = (DEV_HOST_coData_t *)dslhal_support_findProfileIndex(ptidsl, dslhal_support_byteSwap32((unsigned int)profileList.hostProfileBase_p));

  //  profileList.hostProfileBase_p = (DEV_HOST_coData_t *)ptidsl->coProfiles[1].overlayHostAddr;
  profileList.dspScratchMem_p0 = (unsigned int *)
                                          dslhal_support_findProfileIndex(ptidsl, dslhal_support_byteSwap32((unsigned int)profileList.dspScratchMem_p0));
  profileList.dspScratchMem_p1 = (unsigned int *)dslhal_support_findProfileIndex(ptidsl,dslhal_support_byteSwap32((unsigned int)profileList.dspScratchMem_p1));
  profileList.dspScratchMem_p2 = (unsigned int *)dslhal_support_findProfileIndex(ptidsl,dslhal_support_byteSwap32((unsigned int)profileList.dspScratchMem_p2));
  profileList.dspScratchMem_p3 = (unsigned int *)dslhal_support_findProfileIndex(ptidsl,dslhal_support_byteSwap32((unsigned int)profileList.dspScratchMem_p3));
  rc = dslhal_support_blockWrite(&profileList,(PVOID)dspOamSharedInterface.profileList_p,sizeof(DEV_HOST_profileBase_t));
     if (rc)
       return DSLHAL_ERROR_BLOCK_WRITE;

  /* Communicate the Allocated Memory Address to DSP to do overlays */

  /* Change the Endianness of the olayDpDef pointer */
  dspOamSharedInterface.olayDpParms_p = (DEV_HOST_olayDpDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.olayDpParms_p);
  /* Access the olayDpDef Structure */
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.olayDpParms_p,&olayDpParms, sizeof(DEV_HOST_olayDpDef_t));

  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }


  for(i=1;i<ptidsl->numOlayPages;i++)
   {
  /* Change the endianness of the olayDpPageDef Pointer */
     olayDpParms.olayDpPage_p[i] = (DEV_HOST_olayDpPageDef_t *) dslhal_support_byteSwap32((unsigned int)olayDpParms.olayDpPage_p[i]);
     /* Access the olayDpPageDef Structure */
     rc = dslhal_support_blockRead((PVOID)olayDpParms.olayDpPage_p[i],&olayDpPageDef[i],sizeof(DEV_HOST_olayDpPageDef_t));
     if (rc)
       return DSLHAL_ERROR_BLOCK_READ;
     olayDpPageDef[i].overlayHostAddr = ptidsl->olayDpPage[i].overlayHostAddr;
     rc = dslhal_support_blockWrite(&olayDpPageDef[i],(PVOID)olayDpParms.olayDpPage_p[i],sizeof(DEV_HOST_olayDpPageDef_t));
     if (rc)
       return DSLHAL_ERROR_BLOCK_WRITE;
   }

  /* Change the endianness of the Datapump Version Pointer */
  dspOamSharedInterface.datapumpVersion_p = (DEV_HOST_dspVersionDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.datapumpVersion_p);

  /* get DSPVersion itself */

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.datapumpVersion_p,&dspVersion, sizeof(DEV_HOST_dspVersionDef_t));

  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  /* table_dsp info */
#if SWTC
  dspOamSharedInterface.tcHostComm_p = (DEV_HOST_tcHostCommDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.tcHostComm_p);
  rc = dslhal_support_blockRead(&pdspOamSharedInterface->tcHostComm_p,
                                    &pTCHostCommDef, 4);
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

    pTCHostCommDef=(DEV_HOST_tcHostCommDef_t *) dslhal_support_byteSwap32((unsigned int)pTCHostCommDef);

   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.tcHostComm_p,
                               &TCHostCommDef, sizeof(DEV_HOST_tcHostCommDef_t));
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
#endif
  /* Select the Multimode Training */
  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.oamWriteNegoParams_p, &OamWrNegoParaDef, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
switch(dspVersion.netService)
  {
  case 1:  // Set POTS load default training mode to Multi-Mode
           OamWrNegoParaDef.stdMode = MULTI_MODE8;
           OamWrNegoParaDef.stdMode_byte2 = MULTI_MODE8;
           dgprintf(5,"POTS Service \n");
           ptidsl->netService = 1;
           break;
  case 2:  // Set ISDN load default training mode to MultiMode  //was GDMT
//           OamWrNegoParaDef.stdMode = GDMT_MODE;
//           OamWrNegoParaDef.stdMode_byte2 = 0;
           OamWrNegoParaDef.stdMode = MULTI_MODE8;
           OamWrNegoParaDef.stdMode_byte2 = MULTI_MODE8;
           dgprintf(5,"ISDN Service \n");
           ptidsl->netService = 2;
           break;
  default: OamWrNegoParaDef.stdMode = T1413_MODE;
           OamWrNegoParaDef.stdMode_byte2 = 0;
           dgprintf(5,"Default Service \n");
           break;
  }

   ptidsl->AppData.StdMode = (unsigned int)OamWrNegoParaDef.stdMode;
   //UR8_MERGE_START_END CQ10774 Ram
   // CQ10774: Removed |= and changed to = for first initialization
// Disabling Bitswap Mailbox notification by default. 
// Simply remove the EOCAOC mask to enable Mailbox notifications for OAM Bitswap Events.
   OamWrNegoParaDef.oamFeature = dslhal_support_byteSwap32(((OAMFEATURE_TC_SYNC_DETECT_MASK)|(OAMFEATURE_EOCAOC_INTERRUPT_MASK) | (DEV_HOST_SRA_INTERRUPT_MASK)));

  /* Set the flag to start retraining if the margin of the modem drops below
     default margin during showtime */

  OamWrNegoParaDef.marginMonitorShwtme = FALSE;
    /*  Set the flag to start retraining if the margin of the modem drops below default margin during training */

  OamWrNegoParaDef.marginMonitorTrning = FALSE;
  OamWrNegoParaDef.dsToneTurnoff_f = 0;
  dslhal_support_blockWrite(&OamWrNegoParaDef,
                                 (PVOID)dspOamSharedInterface.oamWriteNegoParams_p, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  rc=dslhal_support_setInterruptMask(ptidsl,0);
  if(rc!=DSLHAL_ERROR_NO_ERRORS)
    return rc;

  // DSL phy feature control
  if (_dsl_Feature_0_defined) {
    dslhal_api_setPhyFeatureController(ptidsl, INTEROP_FEATURELIST0_PARAMID, _dsl_Feature_0);
    dslhal_api_enableDisablePhyFeatures(ptidsl, INTEROP_FEATURELIST0_PARAMID, _dsl_Feature_0);
  }
  if (_dsl_Feature_1_defined) {
    dslhal_api_setPhyFeatureController(ptidsl, INTEROP_FEATURELIST1_PARAMID, _dsl_Feature_1);
    dslhal_api_enableDisablePhyFeatures(ptidsl, INTEROP_FEATURELIST1_PARAMID, _dsl_Feature_1);
  }

  //CQ10280
  if (_dsl_PhyControl_0_defined) {
    dslhal_api_setPhyFeatureController(ptidsl, DSL_PHY_FEATURELIST0_PARAMID, _dsl_PhyControl_0);
    dslhal_api_enableDisablePhyFeatures(ptidsl, DSL_PHY_FEATURELIST0_PARAMID, _dsl_PhyControl_0);
  }
  if (_dsl_PhyControl_1_defined) {
    dslhal_api_setPhyFeatureController(ptidsl, DSL_PHY_FEATURELIST1_PARAMID, _dsl_PhyControl_1);
    dslhal_api_enableDisablePhyFeatures(ptidsl, DSL_PHY_FEATURELIST1_PARAMID, _dsl_PhyControl_1);
  }

  // set Debug Halt if _DEBUG_HALT_ not zero
  if (_DEBUG_HALT_)
  {
    // set debugHalt to 1, this is used for concurrent debugging
    int offset_debugHalt[] = {7, 0};
    dslhal_support_writeFromOffset(ptidsl, 2, offset_debugHalt, 1, 1); // write to Internal Table
  }

  /* Co Profile Test */
#if CO_PROFILES
  dspOamSharedInterface.profileList_p = (DEV_HOST_profileBase_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.profileList_p);
  /* Access the profileList Structure */
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.profileList_p,&profileList, sizeof(DEV_HOST_profileBase_t));
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  profileList.hostProfileBase_p = (DEV_HOST_coData_t *)dslhal_support_byteSwap32((unsigned int)profileList.hostProfileBase_p);
  rc = dslhal_support_blockRead((PVOID)profileList.hostProfileBase_p,&coData, sizeof(DEV_HOST_coData_t));
  if (rc)
   {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  dgprintf(2,"Current Profile Vendor Id: %x \n",coData.phyAgcPgaTarget);
  coData.phyAgcPgaTarget = 0xcaba;
  rc = dslhal_support_blockWrite(&coData,(PVOID)profileList.hostProfileBase_p,sizeof(DEV_HOST_coData_t));
  if(rc)
    return DSLHAL_ERROR_BLOCK_WRITE;
#endif
  /* End of CO Profile Test */

#ifndef NO_ACT
  /* Constellation Display Buffer Allocate */
  ptidsl->constDisplay.PmemStartWtAddr = (unsigned int) shim_osAllocateDmaMemory(DSP_CONSTELLATION_BUFFER_SIZE);
  if(ptidsl->constDisplay.PmemStartWtAddr == NULL)
    {
      dgprintf(1, "memory allocate error\n");
      return DSLHAL_ERROR_OVERLAY_MALLOC;
     }
  shim_osZeroMemory((void*)ptidsl->constDisplay.PmemStartWtAddr,DSP_CONSTELLATION_BUFFER_SIZE);
  ptidsl->constDisplay.overlayHostAddr =  virtual2Physical((unsigned int)ptidsl->constDisplay.PmemStartWtAddr);
      dgprintf(4,"Allocated Addr: 0x%x \t Xlated Addr: 0x%x \n",ptidsl->constDisplay.PmemStartWtAddr, ptidsl->constDisplay.overlayHostAddr);
      ptidsl->constDisplay.OverlayXferCount = DSP_CONSTELLATION_BUFFER_SIZE;

  /* Communicate the Allocated Buffer for DSP load Constellation Data */

  /* Change the Endianness of the profileList pointer */
  dspOamSharedInterface.consDispVar_p = (DEV_HOST_consBufDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.consDispVar_p);
  /* Access the profileList Structure */
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.consDispVar_p,&constDisp, sizeof(DEV_HOST_consBufDef_t));

  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  dgprintf(2,"Constellation Old Addr:%x  New: %x \n",constDisp.consDispStartAddr,ptidsl->constDisplay.overlayHostAddr);
  constDisp.consDispStartAddr = (unsigned int)dslhal_support_byteSwap32(ptidsl->constDisplay.overlayHostAddr);
  constDisp.consDispCurrentAddr = constDisp.consDispStartAddr;
  constDisp.consDispBufLen = (unsigned int)dslhal_support_byteSwap32(DSP_CONSTELLATION_BUFFER_SIZE);
  rc = dslhal_support_blockWrite(&constDisp,(PVOID)dspOamSharedInterface.consDispVar_p,sizeof(DEV_HOST_consBufDef_t));
     if (rc)
       return DSLHAL_ERROR_BLOCK_WRITE;
#endif
  dgprintf(5,"dslhal_support_hostDspCodeDownload() completed.\n");
  return DSLHAL_ERROR_NO_ERRORS;

}  /* end of dslhal_support_hostDspCodeDownload() */

#ifdef NOT_DRV_BUILD
/********************************************************************************************
* FUNCTION NAME: dslhal_support_readDelineationState()
*
*********************************************************************************************
* DESCRIPTION:
*   download DSP image from host memory to dsp memory
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_readDelineationState(tidsl_t * ptidsl)
{
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  DEV_HOST_atmStats_t           atmStats;
  DEV_HOST_dsAtmStats_t         dsAtmStats0;
  unsigned int rc=0, delinState=0;
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterface_t *) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
      dgprintf(1,"dslhal_support_blockRead failed\n");
      return DSLHAL_ERROR_BLOCK_READ;
    }

  dspOamSharedInterface.atmStats_p = (DEV_HOST_atmStats_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.atmStats_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.atmStats_p,&atmStats, sizeof(DEV_HOST_atmStats_t));

  if (rc)
    {
      dgprintf(1,"dslhal_support_blockRead failed\n");
      return DSLHAL_ERROR_BLOCK_READ;
    }

  atmStats.ds0_p = (DEV_HOST_dsAtmStats_t *) dslhal_support_byteSwap32((unsigned int)atmStats.ds0_p);

  rc = dslhal_support_blockRead((PVOID)atmStats.ds0_p,&dsAtmStats0, (sizeof(DEV_HOST_dsAtmStats_t)));

  if (rc)
    return rc;
  delinState = dslhal_support_byteSwap32(dsAtmStats0.delineationState);
  if(delinState == TC_SYNC)
     ptidsl->lConnected = 1;
  else
     ptidsl->lConnected = 0;
  return DSLHAL_ERROR_NO_ERRORS;
}
#endif

/********************************************************************************************
* FUNCTION NAME: dslhal_support_processModemStateBitField()
*
*********************************************************************************************
* DESCRIPTION:
*   download DSP image from host memory to dsp memory
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/

unsigned int dslhal_support_processModemStateBitField(tidsl_t * ptidsl)
{
  int rc, offset[2]={2,0};
  int modemStateBitFields[NUMBER_OF_BITFIELDS],changedField=0;
  rc = dslhal_api_dspInterfaceRead(ptidsl,(unsigned int)ptidsl->pmainAddr,2,(unsigned int *)&offset,
                                  (unsigned char *)&modemStateBitFields,NUMBER_OF_BITFIELDS*sizeof(int));
  if (rc)
    return DSLHAL_ERROR_BLOCK_READ;
  for(rc=0;rc<NUMBER_OF_BITFIELDS;rc++)
    dgprintf(4,"Bit Field %d: 0x%x \n",rc+1,dslhal_support_byteSwap32((unsigned int)modemStateBitFields[rc]));

  for(rc=NUMBER_OF_BITFIELDS;rc>0;rc--)
    {
     if(ptidsl->modemStateBitField[rc-1]!=modemStateBitFields[rc-1])
       {
         changedField = rc;
         break;
       }
    }
  if(changedField)
    {
      for(rc=0;rc<32;rc++)
        {
           if(modemStateBitFields[changedField-1] & dslhal_support_byteSwap32((BITFIELD_SCAN >> rc)))
             break;
         }
      dgprintf(5,"Changed Field : %d  Changed Bit : %d \n",changedField,(31-rc));
      ptidsl->rState = ((changedField*100) + (31-rc));
      dgprintf(5,"Modem State : %d \n",ptidsl->rState);
      shim_osMoveMemory((void *)ptidsl->modemStateBitField,(void *)modemStateBitFields, 4*NUMBER_OF_BITFIELDS);
    }

  switch(changedField)
    {
    case 1:  if((ptidsl->rState >= ATU_RIDLE) && (ptidsl->AppData.bState < RSTATE_IDLE))
                   ptidsl->AppData.bState = RSTATE_IDLE;
             if((ptidsl->rState >= GDMT_NSFLR) && (ptidsl->AppData.bState < RSTATE_INIT))
                 ptidsl->AppData.bState = RSTATE_INIT;
             if((ptidsl->rState >= GDMT_ACKX) && (ptidsl->AppData.bState < RSTATE_HS))
                 ptidsl->AppData.bState = RSTATE_HS;
             break;

    case 2:  if((ptidsl->rState >= T1413_NSFLR) && (ptidsl->AppData.bState < RSTATE_INIT))
                 ptidsl->AppData.bState = RSTATE_INIT;
             if((ptidsl->rState >= T1413_ACKX) && (ptidsl->AppData.bState < RSTATE_HS))
                 ptidsl->AppData.bState = RSTATE_HS;
             if((ptidsl->rState == ATU_RSHOWTIME) && (ptidsl->AppData.bState < RSTATE_SHOWTIME))
                 ptidsl->AppData.bState = RSTATE_SHOWTIME;
             break;

    case 3:  if((ptidsl->rState >= ADSL2_COMB3) && (ptidsl->AppData.bState < RSTATE_INIT))
                 ptidsl->AppData.bState = RSTATE_INIT;
             if((ptidsl->rState >= ADSL2_RPARAMS) && (ptidsl->AppData.bState < RSTATE_HS))
                 ptidsl->AppData.bState = RSTATE_HS;
             break;

    case 4:  break;
    default: break;
    }

  ptidsl->stateTransition = modemStateBitFields[1];
  switch(ptidsl->AppData.bState)
    {
    case RSTATE_IDLE:       ptidsl->AppData.idleTick=shim_osClockTick();
                            ptidsl->AppData.initTick=0;
                            ptidsl->AppData.showtimeTick=0;
                            break;
    case RSTATE_HS:         if(!ptidsl->AppData.initTick)
                              {
                                ptidsl->AppData.initTick=shim_osClockTick();
                                }
                            ptidsl->AppData.showtimeTick=0;
                            break;
    case RSTATE_SHOWTIME:   if(!ptidsl->AppData.showtimeTick)
                              ptidsl->AppData.showtimeTick=shim_osClockTick();
                            break;
    default:                break;
                              }
  return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_setInterruptMask()
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

unsigned int dslhal_support_setInterruptMask(tidsl_t * ptidsl,unsigned int inputMask)
{
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  DEV_HOST_hostInterruptMask_t interruptMask;
  unsigned int rc=0;
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterface_t *) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
      dgprintf(1,"dslhal_support_blockRead failed\n");
      return DSLHAL_ERROR_BLOCK_READ;
    }
  dspOamSharedInterface.hostInterruptMask_p =(DEV_HOST_hostInterruptMask_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.hostInterruptMask_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.hostInterruptMask_p,
                                     &interruptMask, sizeof(DEV_HOST_hostInterruptMask_t));
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  if(inputMask & MASK_MAILBOX_INTERRUPTS)
    {
      dgprintf(7,"Mailbox Interrupts Masked \n");
      dgprintf(7,"interruptMask.maskBitField1 = %d \n",dslhal_support_byteSwap32(interruptMask.maskBitField1));
      interruptMask.maskBitField1 |= dslhal_support_byteSwap32(MASK_MAILBOX_INTERRUPTS);
      dgprintf(7,"interruptMask.maskBitField1 = %d \n",dslhal_support_byteSwap32(interruptMask.maskBitField1));
    }
  if(inputMask & MASK_BITFIELD_INTERRUPTS)
    {
      dgprintf(7,"Bit field Interrupts Masked \n");
      dgprintf(7,"interruptMask.maskBitField1 = %d \n",dslhal_support_byteSwap32(interruptMask.maskBitField1));
      interruptMask.maskBitField1 |= dslhal_support_byteSwap32(MASK_BITFIELD_INTERRUPTS);
      dgprintf(7,"interruptMask.maskBitField1 = %d \n",dslhal_support_byteSwap32(interruptMask.maskBitField1));
    }
  if(inputMask & MASK_HEARTBEAT_INTERRUPTS)
    {
      dgprintf(7,"Bit field Interrupts Masked \n");
      dgprintf(7,"interruptMask.maskBitField1 = %d \n",dslhal_support_byteSwap32(interruptMask.maskBitField1));
      interruptMask.maskBitField1 |= dslhal_support_byteSwap32(MASK_HEARTBEAT_INTERRUPTS);
      dgprintf(7,"interruptMask.maskBitField1 = %d \n",dslhal_support_byteSwap32(interruptMask.maskBitField1));
    }
  dslhal_support_blockWrite(&interruptMask,
                                 dspOamSharedInterface.hostInterruptMask_p, sizeof(DEV_HOST_hostInterruptMask_t));
  dgprintf(5,"dslhal_support_setInterruptMask() completed.\n");
  return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_parseInterruptSource()
*
*********************************************************************************************
* DESCRIPTION:
*   Parses the Interrupt Source Bit Field
*
* Return: interrupt Code if successful
*         negative error code if failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/

unsigned int dslhal_support_parseInterruptSource(tidsl_t * ptidsl)
{
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  DEV_HOST_hostInterruptSource_t interruptSource;
  unsigned int rc=0,intrCode=0;
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterface_t *) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
      dgprintf(1,"dslhal_support_blockRead failed\n");
      return (0-DSLHAL_ERROR_BLOCK_READ);
    }
  dspOamSharedInterface.hostInterruptSource_p =(DEV_HOST_hostInterruptSource_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.hostInterruptSource_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.hostInterruptSource_p,
                                     &interruptSource, sizeof(DEV_HOST_hostInterruptSource_t));
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return (0-DSLHAL_ERROR_BLOCK_READ);
    }
  if(interruptSource.sourceBitField1 & dslhal_support_byteSwap32(MASK_MAILBOX_INTERRUPTS))
    {
      dgprintf(7,"Mailbox Interrupts Acknowledge \n");
      intrCode |= 0x00000011;
    }
  if(interruptSource.sourceBitField1 & dslhal_support_byteSwap32(MASK_BITFIELD_INTERRUPTS))
    {
      dgprintf(7,"Bit field Interrupt Acknowledge \n");
      intrCode |= 0x00001002;
    }
  if(interruptSource.sourceBitField1 & dslhal_support_byteSwap32(MASK_HEARTBEAT_INTERRUPTS))
    {
      dgprintf(7,"HeartBeat Interrupt Acknowledge \n");
      intrCode |= 0x00100004;
    }

  interruptSource.sourceBitField1 &=0x0;
  rc=dslhal_support_blockWrite(&interruptSource,
                                 dspOamSharedInterface.hostInterruptSource_p, sizeof(DEV_HOST_hostInterruptSource_t));
  if(rc)
    return (0-DSLHAL_ERROR_BLOCK_WRITE);
  dgprintf(5,"dslhal_support_parseInterruptSource() completed.\n");
  return intrCode;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_byteSwap16()
*
*********************************************************************************************
* DESCRIPTION:
*   input 16 bit short, byte swap from little endian to big endian or vise versa
*
********************************************************************************************/

unsigned short dslhal_support_byteSwap16(unsigned short in16Bits)
{
    unsigned short out16Bits;

#ifdef EB
    unsigned char *pchar;
    unsigned char tmp;
#endif

    out16Bits = in16Bits;

#ifdef EB
    pchar = (unsigned char *)(&out16Bits);

    tmp = *pchar;
    *pchar = *(pchar + 1);
    *(pchar + 1) = tmp;
#endif

    return out16Bits;

} /* end of dslhal_support_byteSwap16() */

/********************************************************************************************
* FUNCTION NAME: dslhal_support_byteSwap32()
*
*********************************************************************************************
* DESCRIPTION:
*   input 32 bit int, byte swap from little endian to big endian or vise versa
*
********************************************************************************************/

unsigned int dslhal_support_byteSwap32_BE (unsigned int in32Bits)
{
    int out32Bits;

#ifdef EB
    unsigned char tmp;
    unsigned char *pchar;
#endif

    out32Bits = in32Bits;

#ifdef EB
    pchar = (unsigned char *)(&out32Bits);

    tmp = *pchar;
    *pchar = *(pchar + 3);
    *(pchar + 3) = tmp;

    tmp = *(pchar + 1);
    *(pchar + 1) = *(pchar + 2);
    *(pchar + 2) = tmp;
#endif

    return out32Bits;

} /* end of dslhal_support_byteSwap32() */


unsigned int dslhal_support_shortSwap32(unsigned int in32Bits)
{
    int out32Bits;

#ifdef EB
    unsigned short tmp;
    unsigned short *pshort;
#endif

    out32Bits = in32Bits;

#ifdef EB
    pshort = (unsigned short *)(&out32Bits);

    tmp = *pshort;
    *pshort = *(pshort + 1);
    *(pshort+1)= tmp;
#endif

    return out32Bits;

} /* end of dslhal_support_byteSwap32() */

/********************************************************************************************
* FUNCTION NAME: dslhal_support_computeCrc32()
*
*********************************************************************************************
* DESCRIPTION:
*   Computes the CRC-32 for the input data
*
* Return: 32 bit CRC of the input data
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_computeCrc32(unsigned char *data, int len)
{
    unsigned int        result;
    int                 i,j;
    unsigned char       octet;

    if ((len < 4) || (data==NULL))
        return(0xdeaddead);
    result = *data++ << 24;
    result |= *data++ << 16;
    result |= *data++ << 8;
    result |= *data++;
    result = ~ result;

    len -=4;

    for (i=0; i<len; i++)
    {
        octet = *(data++);
        for (j=0; j<8; j++)
        {
            if (result & 0x80000000)
            {
               result = (result << 1) ^ CRC32_QUOTIENT ^ (octet >> 7);
            }
            else
            {
               result = (result << 1) ^ (octet >> 7);
            }
            octet <<= 1;
        }
    }
    return ~result;            /* The complement of the remainder */
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_checkOverlayPage()
*
*********************************************************************************************
* DESCRIPTION:
*   Computes the CRC-32 for the input data and compares it with reference
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_checkOverlayPage(tidsl_t *ptidsl, unsigned int tag)
{
  unsigned int computedCrc;
  if((unsigned char *)ptidsl->olayDpPage[tag].PmemStartWtAddr == NULL)
  {
     dgprintf(5,"Null Address for Page: %d\n",tag);
         return DSLHAL_ERROR_OVERLAY_MALLOC;
  }
  computedCrc = dslhal_support_computeCrc32((unsigned char *)ptidsl->olayDpPage[tag].PmemStartWtAddr, ptidsl->olayDpPage[tag].OverlayXferCount);
  dgprintf(6,"\n Pre-Computed CRC32 = 0x%x \t Current CRC32 = 0x%x \n",ptidsl->olayDpPage[tag].olayPageCrc32,computedCrc);
  if(computedCrc != ptidsl->olayDpPage[tag].olayPageCrc32)
      return DSLHAL_ERROR_OVERLAY_CORRUPTED;
  else
      return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_clearTrainingInfo()
*
*********************************************************************************************
* DESCRIPTION:
*   Computes the CRC-32 for the input data and compares it with reference
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/

int dslhal_support_clearTrainingInfo(tidsl_t *ptidsl)
{
  int i;

  for(i=0; i<ptidsl->numOlayPages; i++)
    {
    if(ptidsl->olayDpPage[i].PmemStartWtAddr !=NULL)
      {
      shim_osFreeDmaMemory((void *) ptidsl->olayDpPage[i].PmemStartWtAddr,
                   ptidsl->olayDpPage[i].OverlayXferCount);
      ptidsl->olayDpPage[i].PmemStartWtAddr =NULL;
      }
    }
  for(i=0; i<ptidsl->numProfiles; i++)
      {
        if(ptidsl->coProfiles[i].PmemStartWtAddr != NULL)
    {
            shim_osFreeDmaMemory((void *)ptidsl->coProfiles[i].PmemStartWtAddr, ptidsl->coProfiles[i].OverlayXferCount);
            ptidsl->coProfiles[i].PmemStartWtAddr = NULL;
    }
      }
  return 0;
}


/********************************************************************************************
* FUNCTION NAME: dslhal_support_reloadTrainingInfo()
*
*********************************************************************************************
* DESCRIPTION:
*   Reload overlay pages from flash or memory
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/

int dslhal_support_reloadTrainingInfo(tidsl_t * ptidsl)
{

  int rc = 0, i;
  unsigned int olayXfer,olayStore;
  unsigned int *olayStart;

  unsigned int crc32;
  DEV_HOST_dspOamSharedInterface_t dspOamSharedInterface;
  DEV_HOST_olayDpDef_t            olayDpParms;
  DEV_HOST_olayDpPageDef_t        olayDpPageDef[ptidsl->numOlayPages];
  DEV_HOST_profileBase_t          profileList;

  unsigned int secLength, secOffset, secPage;

  /* co profile */
  for(i=0; i<ptidsl->numProfiles;i++)
    {

      secLength = ptidsl->coProfiles[i].OverlayXferCount;
      secOffset = ptidsl->coProfiles[i].SecOffset;
      ptidsl->coProfiles[i].PmemStartWtAddr = (unsigned int) shim_osAllocateDmaMemory(secLength);
      if(ptidsl->coProfiles[i].PmemStartWtAddr == NULL)
       {
         dgprintf(1, "memory allocate error\n");
         return DSLHAL_ERROR_OVERLAY_MALLOC;
         }
          /* holdSecPhyAddr = virtual2Physical((unsigned int)holdSecVirtAddr); */
      ptidsl->coProfiles[i].overlayHostAddr =  virtual2Physical((unsigned int)ptidsl->coProfiles[i].PmemStartWtAddr);
      dgprintf(4,"Allocated Addr: 0x%x \t Xlated Addr: 0x%x \n",ptidsl->coProfiles[i].PmemStartWtAddr, ptidsl->coProfiles[i].overlayHostAddr);
      ptidsl->coProfiles[i].overlayHostAddr = (unsigned int)dslhal_support_byteSwap32(ptidsl->coProfiles[i].overlayHostAddr);
      rc = shim_read_overlay_page((void *)ptidsl->coProfiles[i].PmemStartWtAddr, secOffset, secLength);
      if(rc != secLength)
      {
       dgprintf(1, "shim_read_overlay_page failed\n");
       return DSLHAL_ERROR_OVERLAY_MALLOC;
       }


  /*shim_osMoveMemory((char *)ptidsl->coProfiles[i].PmemStartWtAddr, (char *)iptr, secLength);*/
  /*   RamP Image ByteSwap test */
      olayStart = (unsigned int *)ptidsl->coProfiles[i].PmemStartWtAddr;

  for(olayXfer=0;olayXfer< secLength/4;olayXfer++)
  {
    olayStore = *(unsigned int *)olayStart;
    olayStore = (unsigned int)dslhal_support_byteSwap32(olayStore);
    *(unsigned int*)olayStart = olayStore;
//    dgprintf(5, "Addr:0x%x \t Content: 0x%x \n",olayStart,olayStore);
    dgprintf(7, "Addr:0x%x \t Content: 0x%x \n",olayStart,olayStore);
    olayStart++;
    olayStore=0;
  }
  shim_osWriteBackCache((void *)ptidsl->coProfiles[i].PmemStartWtAddr, secLength);
    }

  for (secPage=1;secPage<ptidsl->numOlayPages; secPage++)
  {

    dgprintf(6,"OVERLAY PAGE #%d\n",secPage);

    secLength = ptidsl->olayDpPage[secPage].OverlayXferCount;

    dgprintf(4,"Section[%d] Length: %d \n",secPage, secLength);

    secOffset = ptidsl->olayDpPage[secPage].SecOffset;
    ptidsl->olayDpPage[secPage].PmemStartWtAddr  = (unsigned int) shim_osAllocateDmaMemory(secLength);
    if(ptidsl->olayDpPage[secPage].PmemStartWtAddr == NULL)
    {
      dgprintf(1, "overlay page allocate error\n");
      return DSLHAL_ERROR_OVERLAY_MALLOC;
    }

    rc = shim_read_overlay_page((void *)ptidsl->olayDpPage[secPage].PmemStartWtAddr,secOffset, secLength);
    if(rc != secLength)
    {
      dgprintf(1, "overlay page read error\n");
      return DSLHAL_ERROR_OVERLAY_MALLOC;
    }

    /* ptidsl->olayDpPage[secPage].overlayHostAddr = ((unsigned int)(ptidsl->olayDpPage[secPage].PmemStartWtAddr)&~0xe0000000); */
    ptidsl->olayDpPage[secPage].overlayHostAddr = virtual2Physical((unsigned int)ptidsl->olayDpPage[secPage].PmemStartWtAddr);
    dgprintf(6,"Allocated Addr: 0x%x \t Xlated Addr: 0x%x \n",ptidsl->olayDpPage[secPage].PmemStartWtAddr,ptidsl->olayDpPage[secPage].overlayHostAddr);

    ptidsl->olayDpPage[secPage].overlayHostAddr = (unsigned int)dslhal_support_byteSwap32(ptidsl->olayDpPage[secPage].overlayHostAddr);
    /*ptidsl->olayDpPage[secPage].OverlayXferCount = secLength;
      ptidsl->olayDpPage[secPage].BinAddr          = secAddr;
      shim_osMoveMemory((char *)ptidsl->olayDpPage[secPage].PmemStartWtAddr, (char *)iptr, secLength);
    */
    olayStart = (unsigned int *)ptidsl->olayDpPage[secPage].PmemStartWtAddr;

    for(olayXfer=0;olayXfer< secLength/4;olayXfer++)
    {
      olayStore = *(unsigned int *)olayStart;
      olayStore = (unsigned int)dslhal_support_byteSwap32(olayStore);
      *(unsigned int*)olayStart = olayStore;
 //     dgprintf(5, "Addr:0x%x \t Content: 0x%x \n",olayStart,olayStore);
      dgprintf(7, "Addr:0x%x \t Content: 0x%x \n",olayStart,olayStore);
      olayStart++;
      olayStore=0;
    }
      /* RamP Image ByteSwap test */
      /* compute the CRC of each overlay page and Store the Checksum in a local global variable */
      /* This Value of CRC is to be compared with the header where all the CRC bytes are lumped together */
    crc32 = dslhal_support_computeCrc32((char *)ptidsl->olayDpPage[secPage].PmemStartWtAddr, ptidsl->olayDpPage[secPage].OverlayXferCount);
    if(crc32 != ptidsl->olayDpPage[secPage].olayPageCrc32)
      return DSLHAL_ERROR_OVERLAY_MALLOC;

    shim_osWriteBackCache((void *)ptidsl->olayDpPage[secPage].PmemStartWtAddr, secLength);
  }


  dgprintf(5,"ptidsl->hostIf.mainAddr=0x%X\n", (unsigned int)ptidsl->pmainAddr);

  /* get the pointer to DSP-OAM Shared Interface */
  rc = dslhal_support_blockRead(ptidsl->pmainAddr, &dspOamSharedInterface,
                                     sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  /* Communicate the Allocated Memory Address to DSP to choose CO Profiles */

  /* Change the Endianness of the profileList pointer */
  dspOamSharedInterface.profileList_p = (DEV_HOST_profileBase_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.profileList_p);
  /* Access the profileList Structure */
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.profileList_p,&profileList, sizeof(DEV_HOST_profileBase_t));

  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  dgprintf(2,"Old Addr:%x  New: %x \n",profileList.hostProfileBase_p,ptidsl->coProfiles[0].overlayHostAddr);
  profileList.hostProfileBase_p = (DEV_HOST_coData_t *)dslhal_support_findProfileIndex(ptidsl, dslhal_support_byteSwap32((unsigned int)profileList.hostProfileBase_p));

  //  profileList.hostProfileBase_p = (DEV_HOST_coData_t *)ptidsl->coProfiles[1].overlayHostAddr;
  profileList.dspScratchMem_p0 = (unsigned int *)
                                          dslhal_support_findProfileIndex(ptidsl, dslhal_support_byteSwap32((unsigned int)profileList.dspScratchMem_p0));
  profileList.dspScratchMem_p1 = (unsigned int *)dslhal_support_findProfileIndex(ptidsl,dslhal_support_byteSwap32((unsigned int)profileList.dspScratchMem_p1));
  profileList.dspScratchMem_p2 = (unsigned int *)dslhal_support_findProfileIndex(ptidsl,dslhal_support_byteSwap32((unsigned int)profileList.dspScratchMem_p2));
  profileList.dspScratchMem_p3 = (unsigned int *)dslhal_support_findProfileIndex(ptidsl,dslhal_support_byteSwap32((unsigned int)profileList.dspScratchMem_p3));
  rc = dslhal_support_blockWrite(&profileList,(PVOID)dspOamSharedInterface.profileList_p,sizeof(DEV_HOST_profileBase_t));
     if (rc)
       return DSLHAL_ERROR_BLOCK_WRITE;

  /* Communicate the Allocated Memory Address to DSP to do overlays */

  /* Change the Endianness of the olayDpDef pointer */
  dspOamSharedInterface.olayDpParms_p = (DEV_HOST_olayDpDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.olayDpParms_p);
  /* Access the olayDpDef Structure */
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.olayDpParms_p,&olayDpParms, sizeof(DEV_HOST_olayDpDef_t));

  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }


  for(i=1;i<ptidsl->numOlayPages;i++)
   {
  /* Change the endianness of the olayDpPageDef Pointer */
     olayDpParms.olayDpPage_p[i] = (DEV_HOST_olayDpPageDef_t *) dslhal_support_byteSwap32((unsigned int)olayDpParms.olayDpPage_p[i]);
     /* Access the olayDpPageDef Structure */
     rc = dslhal_support_blockRead((PVOID)olayDpParms.olayDpPage_p[i],&olayDpPageDef[i],sizeof(DEV_HOST_olayDpPageDef_t));
     if (rc)
       return DSLHAL_ERROR_BLOCK_READ;
     olayDpPageDef[i].overlayHostAddr = ptidsl->olayDpPage[i].overlayHostAddr;
     rc = dslhal_support_blockWrite(&olayDpPageDef[i],(PVOID)olayDpParms.olayDpPage_p[i],sizeof(DEV_HOST_olayDpPageDef_t));
     if (rc)
       return DSLHAL_ERROR_BLOCK_WRITE;
   }

  ptidsl->bOverlayPageLoaded = 1;
  return DSLHAL_ERROR_NO_ERRORS;
}
  /* end of dslhal_support_reloadTrainingInfo() */


/********************************************************************************************
* FUNCTION NAME: dslhal_support_restoreTrainingInfo()
*
*********************************************************************************************
* DESCRIPTION:
*   Computes the CRC-32 for the input data and compares it with reference
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/


int dslhal_support_restoreTrainingInfo(tidsl_t *ptidsl)
{
  int rc;

  rc=1;
  while(rc != 0)
    {
    dslhal_support_clearTrainingInfo(ptidsl);
    //shim_osCriticalEnter();
    rc = dslhal_support_reloadTrainingInfo(ptidsl);
    //shim_osCriticalExit();
    shim_osClockWait(6400);
    }
  return 0;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_advancedIdleProcessing()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced Idle State Processing Functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_advancedIdleProcessing(tidsl_t *ptidsl)
{
  int rc=0;
#ifndef NO_ACT
  rc +=  dslhal_advcfg_resetBitSwapMessageLog(ptidsl,0);
  rc += dslhal_advcfg_resetBitSwapMessageLog(ptidsl,1);
  rc += dslhal_advcfg_resetTrainStateHistory(ptidsl);
  rc += dslhal_advcfg_getReasonForDrop(ptidsl);
#endif
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  else
    return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_aocBitSwapProcessing()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced Idle State Processing Functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_aocBitSwapProcessing(tidsl_t *ptidsl,unsigned int usDs)
{
  int rc=0;
#ifndef NO_ACT
  int i;
  int differentCmd_f;
  unsigned int dsSwapInx;

  static UINT8 lastAturBitSwapCommands[6] = {0, 0, 0, 0, 0, 0};
  static UINT8 lastAturBitSwapBinNum[6] = {0, 0, 0, 0, 0, 0};

  if (usDs == 0)
  {
    dgprintf(4," DSP_XMITBITSWAP\n");
    rc += dslhal_advcfg_getAocBitswapBuffer(ptidsl,0);
    ptidsl->usBitSwapInx++;
    if (ptidsl->usBitSwapInx > 29)
      ptidsl->usBitSwapInx=0;
  }

  if (usDs == 1)
  {
    dgprintf(4," DSP_RCVBITSWAP\n");
    rc += dslhal_advcfg_getAocBitswapBuffer(ptidsl,1);
    differentCmd_f = FALSE;
    dsSwapInx = ptidsl->dsBitSwapInx;
    if (! rc)
    {
      for (i = 0; i < 6; i++)
      {
        if (lastAturBitSwapCommands[i] != ptidsl->dsBitSwap[dsSwapInx].bitSwapCommand[i])
        {
          differentCmd_f = TRUE;
          break;
        }
      }
      if (! differentCmd_f)
      {
        for (i = 0; i < 6; i++)
        {
          if (lastAturBitSwapBinNum[i] != ptidsl->dsBitSwap[dsSwapInx].bitSwapBinNum[i])
          {
            differentCmd_f = TRUE;
            break;
          }
        }
      }
      //CPE data pump seems to occasionally send us the same bit swap twice in a row with different sframe counter.
      //Since these are never counted twice by the debug output of AC5, we should not count them twice either.
      //So, we ignore the sframe_counter in determining whether the most recent bitswap is a duplicate.
      if (differentCmd_f)
      {
          shim_osMoveMemory((void *)lastAturBitSwapCommands, (void *)ptidsl->dsBitSwap[dsSwapInx].bitSwapCommand, 6);
          shim_osMoveMemory((void *)lastAturBitSwapBinNum, (void *)ptidsl->dsBitSwap[dsSwapInx].bitSwapBinNum, 6);
          ptidsl->dsBitSwapInx++;
          if (ptidsl->dsBitSwapInx > 29)
            ptidsl->dsBitSwapInx = 0;
      }
    }
  }
#endif
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  else
    return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_gatherEocMessages()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced EOC Buffering functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_gatherEocMessages(tidsl_t *ptidsl,int usDs, int msgPart1, int msgPart2)
{
  int rc=0;
#ifndef NO_ACT
  rc = dslhal_advcfg_logEocMessages(ptidsl,usDs, msgPart1, msgPart2);
#endif
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  else
    return DSLHAL_ERROR_NO_ERRORS;
}
/********************************************************************************************
* FUNCTION NAME: dslhal_support_gatherSnrPerBin()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced Snr per bin buffering Functions
// UR8_MERGE_START CQ10386 PeterHou
*   Note (Important): This function is used to gather training SNR. The function can only be
*       used internally (when DSLHAL is handling DSP_SNR). After showtime, the datapump holding
*       training SNR is reused for other purpose, therefore calling this function will not
*       getting meaningful information.
*       The training SNR gathered is stored in AppData.rxSnrPerBin0, AppData.rxSnrPerBin1, and
*       AppData.rxSnrPerBin2. After training, the information in these buffer representing
*       training SNR.
*       Also note that each entry in the buffer is a 16 bit little endian value.
*       To convert each 16 bit entry to dB, you need to:
*       1. Endian conversion by dslhal_support_byteSwap16 (if Host not in little endian).
*       2. Divide each 16 bit value by 85.04 (= 256/(10*Log2))
// UR8_MERGE_END CQ10386
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_gatherSnrPerBin(tidsl_t *ptidsl,unsigned int snrBufferOpt)
{
  DEV_HOST_snrBuffer_t snrBuffer;
  DEV_HOST_dspOamSharedInterface_t dspOamSharedInterface;
  int rc, max_ds_tones;

  dgprintf(5,"dslhal_support_gatherSnrPerBin\n");
  rc = dslhal_support_blockRead(ptidsl->pmainAddr, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  if(snrBufferOpt <0 || snrBufferOpt >2)
    {
    dgprintf(1,"Invalid input for Snr Buffer\n");
    return DSLHAL_ERROR_INVALID_PARAM;
    }
  //
  // Note we can't use ptidsl->max_ds_tones in the following statement because
  // this function could be invoked before showtime. (tidsl->max_ds_tones won't
  // be ready before showtime).
  //
  max_ds_tones = dslhal_support_getMaxDsTones(ptidsl);
  dspOamSharedInterface.snrBuffer_p = (DEV_HOST_snrBuffer_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.snrBuffer_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.snrBuffer_p,&snrBuffer, sizeof(DEV_HOST_snrBuffer_t));
  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  snrBuffer.buffer1_p = (short *)dslhal_support_byteSwap32((unsigned int)snrBuffer.buffer1_p);
  snrBuffer.buffer2_p = (short *)dslhal_support_byteSwap32((unsigned int)snrBuffer.buffer2_p);
  if(snrBufferOpt <2)
    {
      rc = dslhal_support_blockRead((PVOID)snrBuffer.buffer1_p,(void *)ptidsl->AppData.rxSnrPerBin0, max_ds_tones*2);
      if (rc)
       {
         dgprintf(1,"dslhal_support_blockRead failed\n");
         return DSLHAL_ERROR_BLOCK_READ;
       }
#ifdef DEBUG_BUILD
      dgprintf(5," Rx Snr 0\n");
      for(rc=0;rc<max_ds_tones;)
        {
          dgprintf(5,"0x%04x \n",dslhal_support_byteSwap16(ptidsl->AppData.rxSnrPerBin0[rc++]));
          if((rc%8==0)&&(rc!=0))
            dgprintf(5,"\n");
        }
#endif
    }
   if(snrBufferOpt ==1)
     {
       rc = dslhal_support_blockRead((PVOID)snrBuffer.buffer2_p,(void *)ptidsl->AppData.rxSnrPerBin1, max_ds_tones*2);
       if (rc)
       {
         dgprintf(1,"dslhal_support_blockRead failed\n");
         return DSLHAL_ERROR_BLOCK_READ;
       }
#ifdef DEBUG_BUILD
       dgprintf(5," Rx Snr 1\n");
       for(rc=0;rc<max_ds_tones;)
         {
           dgprintf(5,"0x%04x \n",dslhal_support_byteSwap16(ptidsl->AppData.rxSnrPerBin1[rc++]));
           if((rc%8==0)&&(rc!=0))
             dgprintf(5,"\n");
         }
#endif
     }
  if(snrBufferOpt ==2)
    {
      rc = dslhal_support_blockRead((PVOID)snrBuffer.buffer1_p,(void *)ptidsl->AppData.rxSnrPerBin2, max_ds_tones*2);
      if (rc)
       {
         dgprintf(1,"dslhal_support_blockRead failed\n");
         return DSLHAL_ERROR_BLOCK_READ;
       }
#ifdef DEBUG_BUILD
      dgprintf(5," Rx Snr 2\n");
      for(rc=0;rc<max_ds_tones;)
        {
          dgprintf(5,"0x%04x \n",dslhal_support_byteSwap16(ptidsl->AppData.rxSnrPerBin2[rc++]));
          if((rc%8==0)&&(rc!=0))
            dgprintf(5,"\n");
        }
#endif

    }

  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  else
    return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_processTrainingState()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced Training State Processing Functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_processTrainingState(tidsl_t *ptidsl)
{
  int rc=0;
#ifndef NO_ACT

  if(ptidsl->trainStateInx<120)
    {
     rc = dslhal_advcfg_getTrainingState(ptidsl,(void *)&ptidsl->trainHistory[ptidsl->trainStateInx++]);
     if((ptidsl->trainStateInx >1) &&(ptidsl->trainHistory[(ptidsl->trainStateInx-1)].subStateIndex ==
        ptidsl->trainHistory[(ptidsl->trainStateInx-2)].subStateIndex))
        ptidsl->trainStateInx--;
    }
  else
      ptidsl->trainStateInx = 0;
#endif
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  else
    return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_gatherAdsl2Messages()
*
*********************************************************************************************
* DESCRIPTION:
*   Gathers ADSL2 Training Messages
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_gatherAdsl2Messages(tidsl_t *ptidsl,int tag, int param1, int param2)
{
  int rc=0;
  unsigned int adsl2MsgLoc;
  switch(tag)
    {
    case CMSGFMT_INDEX:
       dgprintf(5,"C-Msg-FMT rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSGFMT_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.cMsgFmt,CMSGFMT_SIZE);
       break;
    case RMSGFMT_INDEX:
    case RMSGFMT2_INDEX:
       dgprintf(5,"R-Msg-FMT rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSGFMT_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.rMsgFmt,RMSGFMT_SIZE);
       break;
    case CMSGPCB_INDEX:
       dgprintf(5,"C-Msg-PCB rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSGPCB_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.cMsgPcb,CMSGPCB_SIZE);
       ptidsl->adsl2TrainingMessages.cMsgPcbLen = CMSGPCB_SIZE;
       break;
    case CMSGPCB2_INDEX:
       dgprintf(5,"C-Msg-PCB2 rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSGPCB2_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.cMsgPcb,CMSGPCB2_SIZE);
       ptidsl->adsl2TrainingMessages.cMsgPcbLen = CMSGPCB2_SIZE;
#ifndef NO_ACT
       rc += dslhal_advcfg_setBlackOutBits(ptidsl);
#endif
       break;
    case CMSGPCB2L_INDEX:
       dgprintf(5,"C-Msg-PCB2L rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSGPCB2L_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.cMsgPcb,CMSGPCB2L_SIZE);
       ptidsl->adsl2TrainingMessages.cMsgPcbLen = CMSGPCB2L_SIZE;
       break;
    case RMSGPCB_INDEX:
       dgprintf(5,"R-Msg-PCB rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSGPCB_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.rMsgPcb,RMSGPCB_SIZE);
       ptidsl->adsl2TrainingMessages.rMsgPcbLen = RMSGPCB_SIZE;
       break;
    case RMSGPCB2L_INDEX:
       dgprintf(5,"R-Msg-PCB 2L rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSGPCB2L_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.rMsgPcb,RMSGPCB2L_SIZE);
       ptidsl->adsl2TrainingMessages.rMsgPcbLen = RMSGPCB2L_SIZE;
       break;
    case RMSGPCB2PL_INDEX:
       dgprintf(5,"R-Msg-PCB 2PL rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSGPCB2PL_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.rMsgPcb,RMSGPCB2PL_SIZE);
       ptidsl->adsl2TrainingMessages.rMsgPcbLen = RMSGPCB2PL_SIZE;
       break;
    case CMSG1ADSL2_INDEX:
       dgprintf(5,"C-Msg1 rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSG1ADSL2_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.cMsg1,CMSG1ADSL2_SIZE);
       ptidsl->adsl2TrainingMessages.cMsg1Len = CMSG1ADSL2_SIZE;
       break;
    case CMSG1ADSL2P_INDEX:
       dgprintf(5,"C-Msg1 Plus rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSG1ADSL2P_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.cMsg1,CMSG1ADSL2P_SIZE);
       ptidsl->adsl2TrainingMessages.cMsg1Len = CMSG1ADSL2P_SIZE;
       break;
    case CMSG2ADSL2_INDEX:
       dgprintf(5,"C-Msg2 rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSG2ADSL2_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.cMsg2,CMSG2ADSL2_SIZE);
       ptidsl->adsl2TrainingMessages.cMsg2Len = CMSG2ADSL2_SIZE;
       break;
    case RMSG1ADSL2_INDEX:
       dgprintf(5,"R-Msg1 rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG1ADSL2_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.rMsg1,RMSG1ADSL2_SIZE);
       ptidsl->adsl2TrainingMessages.rMsg1Len = RMSG1ADSL2_SIZE;
       break;
    case RMSG2ADSL2_INDEX:
       dgprintf(5,"R-Msg2 rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG2ADSL2_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.rMsg2,RMSG2ADSL2_SIZE);
       ptidsl->adsl2TrainingMessages.rMsg2Len = RMSG2ADSL2_SIZE;
       break;
    case RMSG2ADSL2P_INDEX:
       dgprintf(5,"R-Msg2 Plus rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG2ADSL2P_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.rMsg2,RMSG2ADSL2P_SIZE);
       ptidsl->adsl2TrainingMessages.rMsg2Len = RMSG2ADSL2P_SIZE;
       break;
    case CPARAMS_INDEX:
       dgprintf(5,"C-Params rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CPARAMS_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.cParams,CPARAMS_SIZE);
       ptidsl->adsl2TrainingMessages.cParamsLen = CPARAMS_SIZE;
       break;
    case RPARAMS_INDEX:
       dgprintf(5,"R-Params rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RPARAMS_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.rParams,RPARAMS_SIZE);
       ptidsl->adsl2TrainingMessages.rParamsLen = RPARAMS_SIZE;
       break;
    case CPARAMSPL_INDEX:
       dgprintf(5,"C-Params Plus rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CPARAMSPL_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.cParams,CPARAMSPL_SIZE);
       ptidsl->adsl2TrainingMessages.cParamsLen = CPARAMSPL_SIZE;
       break;
    case RPARAMSPL_INDEX:
       dgprintf(5,"R-Params Plus rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RPARAMS_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2TrainingMessages.rParams,RPARAMSPL_SIZE);
       ptidsl->adsl2TrainingMessages.rParamsLen = RPARAMSPL_SIZE;
       break;
    case RMSG1LD_INDEX:
       dgprintf(5,"R-Msg1 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG1LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg1Ld,RMSG1LD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsg1LdLen = RMSG1LD_SIZE;
       break;
    case RMSG2LD_INDEX:
       dgprintf(5,"R-Msg2 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG2LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg2Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG3LD_INDEX:
       dgprintf(5,"R-Msg3 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG3LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg3Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG4LD_INDEX:
       dgprintf(5,"R-Msg4 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG4LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg4Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG5LD_INDEX:
       dgprintf(5,"R-Msg5 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG5LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg5Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG6LD_INDEX:
       dgprintf(5,"R-Msg6 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG6LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg6Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG7LD_INDEX:
       dgprintf(5,"R-Msg7 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG7LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg7Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG8LD_INDEX:
       dgprintf(5,"R-Msg8 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG8LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg8Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG9LD_INDEX:
       dgprintf(5,"R-Msg9 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG9LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg9Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG10LD_INDEX:
       dgprintf(5,"R-Msg10 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG10LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg10Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG11LD_INDEX:
       dgprintf(5,"R-Msg11 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG11LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg11Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG12LD_INDEX:
       dgprintf(5,"R-Msg12 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG12LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg12Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG13LD_INDEX:
       dgprintf(5,"R-Msg13 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG13LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg13Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG14LD_INDEX:
       dgprintf(5,"R-Msg14 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG14LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg14Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG15LD_INDEX:
       dgprintf(5,"R-Msg15 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG15LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg15Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG16LD_INDEX:
       dgprintf(5,"R-Msg16 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG16LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg16Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;
    case RMSG17LD_INDEX:
       dgprintf(5,"R-Msg17 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, RMSG17LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.rMsg17Ld,RMSGxLD_SIZE);
       ptidsl->adsl2DiagnosticMessages.rMsgxLdLen = RMSGxLD_SIZE;
       break;

    case CMSG1LD_INDEX:
       dgprintf(5,"C-Msg1 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSG1LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.cMsg1Ld,CMSG1LD_SIZE);
       ptidsl->adsl2DiagnosticMessages.cMsg1LdLen = CMSG1LD_SIZE;
       break;
    case CMSG2LD_INDEX:
       dgprintf(5,"C-Msg2 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSG2LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.cMsg2Ld,CMSG2LD_SIZE);
       ptidsl->adsl2DiagnosticMessages.cMsg2LdLen = CMSG2LD_SIZE;
       break;
    case CMSG3LD_INDEX:
       dgprintf(5,"C-Msg3 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSG3LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.cMsg3Ld,CMSG3LD_SIZE);
       ptidsl->adsl2DiagnosticMessages.cMsg3LdLen = CMSG3LD_SIZE;
       break;
    case CMSG4LD_INDEX:
       dgprintf(5,"C-Msg4 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSG4LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.cMsg4Ld,CMSG4LD_SIZE);
       ptidsl->adsl2DiagnosticMessages.cMsg4LdLen = CMSG4LD_SIZE;
       break;
    case CMSG5LD_INDEX:
       dgprintf(5,"C-Msg5 LD rec'd\n");
       adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                     (ptidsl, CMSG5LD_INDEX);
       rc += dslhal_support_blockRead((PVOID)adsl2MsgLoc,
             ptidsl->adsl2DiagnosticMessages.cMsg5Ld,CMSG5LD_SIZE);
       ptidsl->adsl2DiagnosticMessages.cMsg5LdLen = CMSG5LD_SIZE;
       break;
    default:
       dgprintf(5,"Unknown ADSL2 Message rec'd\n");
       break;
    }
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  else
    return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_getAdsl2MessageLocation()
*
*********************************************************************************************
* DESCRIPTION:
*   Gets the address to the ADSL2 Message being looked up
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_getAdsl2MessageLocation(tidsl_t *ptidsl,int msgOffset)
{
  int rc=0;

  DEV_HOST_dspOamSharedInterface_t *pSharedInterface, sharedInterface;
  DEV_HOST_dspWrNegoParaDef_t   dspNegoPara;
  int  adsl2MsgString, adsl2MsgAddr;

  pSharedInterface = (DEV_HOST_dspOamSharedInterface_t *) ptidsl->pmainAddr;
  rc += dslhal_support_blockRead(pSharedInterface, &sharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  sharedInterface.dspWriteNegoParams_p = (DEV_HOST_dspWrNegoParaDef_t *) dslhal_support_adsl2ByteSwap32((unsigned int)sharedInterface.dspWriteNegoParams_p);
  rc += dslhal_support_blockRead((PVOID)sharedInterface.dspWriteNegoParams_p,&dspNegoPara, sizeof(DEV_HOST_dspWrNegoParaDef_t));

  if (rc)
    {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  adsl2MsgString = dslhal_support_adsl2ByteSwap32((unsigned int)dspNegoPara.adsl2DeltMsgs_p);
  dgprintf(5,"ADSL2 Message String Address: 0x%x\n",adsl2MsgString);
  rc += dslhal_support_blockRead((PVOID)(adsl2MsgString +
                                  ((sizeof(unsigned char *)*msgOffset))),
                                  &adsl2MsgAddr, sizeof(int));
  adsl2MsgAddr = dslhal_support_adsl2ByteSwap32((unsigned int)adsl2MsgAddr);
  dgprintf(5," Message Address: 0x%x\n",adsl2MsgAddr);

       if(rc)
          return DSLHAL_ERROR_BLOCK_READ;
       else
          return adsl2MsgAddr;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_getCMsgsRa()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced Training Message functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_getCMsgsRa(tidsl_t *ptidsl,void *cMsg)
{
  int rc=0;
  DEV_HOST_raMsgsDef_t         raMsgParms;
  DEV_HOST_dspOamSharedInterface_t  dspOamSharedInterface;
  rc += dslhal_support_blockRead(ptidsl->pmainAddr, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));

  dspOamSharedInterface.raMsgs_p = (DEV_HOST_raMsgsDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.raMsgs_p);

  rc += dslhal_support_blockRead((PVOID)dspOamSharedInterface.raMsgs_p,
                               &raMsgParms, sizeof(DEV_HOST_raMsgsDef_t));
  shim_osMoveMemory((void *)cMsg,(void *)raMsgParms.cMsgsRaString,6);

  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  else
    return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_support_gatherRateMessages()
*
*********************************************************************************************
* DESCRIPTION:
*        Gathers Rate Training Messages
* Input
*        tidsl_t *ptidsl : Pointer to application structure
*
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_gatherRateMessages(tidsl_t * ptidsl)
{
  int rc;
  DEV_HOST_dspWrNegoParaDef_t  negoParms;
  DEV_HOST_dspOamSharedInterface_t  dspOamSharedInterface;

  dgprintf(1, "dslhal_support_gatherRateMessages\n");

  rc += dslhal_support_blockRead(ptidsl->pmainAddr, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  dspOamSharedInterface.dspWriteNegoParams_p = (DEV_HOST_dspWrNegoParaDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspWriteNegoParams_p, &negoParms, sizeof(DEV_HOST_dspWrNegoParaDef_t));
  if (rc)
    return DSLHAL_ERROR_BLOCK_READ;
  else
    {
      shim_osMoveMemory((void *)ptidsl->AppData.bCRates1, (void *)negoParms.cRates1, 120);
      shim_osMoveMemory((void *)ptidsl->AppData.bRRates1, (void *)negoParms.rRates1, 44);
    }
return DSLHAL_ERROR_NO_ERRORS;
}

//
//Note:
//  For performance reason, if it is certained that dslhal_api_gatherStatistics()
//  has been called after showtime, use ptidsl->AppData.max_ds_tones instead
//  of calling dslhal_support_getMaxDsTones
//
unsigned int dslhal_support_getMaxDsTones(tidsl_t *ptidsl)
{
  unsigned int TrainedModeEx = dslhal_support_getTrainedMode(ptidsl);
  return (TrainedModeEx & ADSL2PLUS_MASKS) ? 512: 256;
}

//
//Note:
//  For performance reason, if it is certained that dslhal_api_gatherStatistics()
//  has been called after showtime, use ptidsl->AppData.max_us_tones instead
//  of calling dslhal_support_getMaxUsTones
//
unsigned int dslhal_support_getMaxUsTones(tidsl_t *ptidsl)
{
  int max_us_tones=32;
  dslhal_support_getTrainedMode(ptidsl); // annex_select & psd_mask_qualifier will be updated
  if (ptidsl->AppData.annex_selected & ANNEXB)
    max_us_tones = 64;
  else if (ptidsl->AppData.annex_selected & ANNEXM)
  { // check EU
    max_us_tones = 32+
      ((ptidsl->AppData.psd_mask_qualifier & ANNEXM_US_SUBMODE_BITMASK) >> ANNEXMJ_SUBMODE_MASK_BITPOS)*4;
  }

  return max_us_tones;
}

#if 1
// Kept for backward compatibility
unsigned int dslhal_support_getNumTones(tidsl_t *ptidsl)
{
  return dslhal_support_getMaxDsTones(ptidsl);
}
#endif

static unsigned int dslhal_support_findProfileIndex(tidsl_t *ptidsl, unsigned int binAddr)
{
   int i=0;
   for(i=0; i<ptidsl->numProfiles; i++)
     {
     dgprintf(5, "Comparing %x with %x \n",binAddr, ptidsl->coProfiles[i].BinAddr);
     if(ptidsl->coProfiles[i].BinAddr == binAddr)
        return (ptidsl->coProfiles[i].overlayHostAddr);
     }
   return dslhal_support_byteSwap32(binAddr);
}

static unsigned int dslhal_support_adsl2ByteSwap32(unsigned int in32Bits)
{
    int out32Bits=0;

#ifdef EB
  out32Bits = (in32Bits << 24);
  out32Bits |=((in32Bits & 0x0000ff00) << 8);
  out32Bits |=((in32Bits & 0xff000000) >> 24);
  out32Bits |=((in32Bits & 0x00ff0000) >> 8);
#else
    out32Bits = in32Bits;
#endif

    return out32Bits;

} /* end of dslhal_support_byteSwap32() */

/******************************************************************************************
* FUNCTION NAME:     dslhal_support_IsADSL1Mode
*
*******************************************************************************************
* DESCRIPTION: This function check current train mode see if it's GDMT, T1413, or GLITE.
*              Detect & take care both old train mode scheme & new train mode scheme.
*              It is expected the ptidsl->AppData.dsl_modulation is been set before calling
*              this function.
*
* INPUT:  PITIDSLHW_T *ptidsl
*
* RETURN: TRUE: It's one of ADSL1 (GDMT, T1413, or GLITE) mode.
*         FALSE: It's not ADSL1 mode.
*****************************************************************************************/
int dslhal_support_IsADSL1Mode(tidsl_t *ptidsl)
{
  unsigned int dsl_modulation = ptidsl->AppData.dsl_modulation & ~0x01; // mask out DELT mode

  if (ptidsl->AppData.useBitField)
  {
    // bitfield TrainMode: GDMT=2, T1413=0x80, GLITE=4
    if (dsl_modulation &
      (GDMT_ANNEX_A_OR_B | GLITE_ANNEX_A_AND_B | T1413_ANSI))
      return 1; // It's ADSL1 mode
  }
  else
  { // ordinal number TrainMode: T1413=2, GDMT=3, GLITE=4
    if (dsl_modulation <= DSLTRAIN_GLITE_MODE)

      return 1; // It's ADSL1 mode
  }
  return 0; // not ADSL1 mode
}
