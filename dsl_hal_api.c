/*******************************************************************************
* FILE PURPOSE:     DSL Driver API functions for Sangam
*
********************************************************************************
* FILE NAME:        dsl_hal_api.c
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
*    26Feb04     0.01.29            RamP   Added code to get error statistics
*    02Mar04     0.01.30            RamP   Changed overlay page number constant to
*                                          refer to the host interface constant
*    05Mar04     0.01.31            RamP   Scaled dsNoise factors to get dB output
*    01Apr04     0.01.32            RamP   Overflow Check for Training Fails added
*    20Apr04     0.01.33            RamP   decoupled overlay page check from host
*                                          interface, added scratch memory support
*    27Apr04     0.01.34            RamP   Fixed double-swap problem with Tx Power
*    29Apr04     0.01.35            RamP   Overflow check for Training Fails added
*                                          Scaled all statistics arrays for ADSL2+
*                                          added detection for ADSL2+ in gather
*                                          statistics API
*    04May04     0.01.36            RamP   Reset bState based on DSP_IDLE added
*    04May04     0.01.37            RamP   Removed Scaling factor for adsl2USConRate
*    14May04     0.01.38            RamP   Split statistics function to avoid stack
*                                          overflow problem due to local variables
*                                          Modified type & equation for power cutback
*                                          Updated statistics for ADSL2+
*    17May04     0.01.39            RamP   Must #IFDEF out dslhal_api_getAdvancedStats()
*                                          function call body, instead of function call.
*                                          Must #IFDEF out dslhal_staticapi_getHlog().
*    02Aug04     0.01.40            RamP   Fixed problem with DSL Activitiy LED config.
*                                          Fixed initialization issue for currAddr in
*                                          dsp interface read/write APIs.
*    26Aug04     0.01.41            Brian  Made changes to support switching DSP
*                                          frequency to 250Mhz
*    22Sep04     0.01.42            RamP   Corrected fast/intlv determination
*                                          for ADSL2/2+ training modes
*    06Oct04     0.01.43            RamP   Added API functions to configure DSL PHY
*                                          features and to set DSP/MIPS as controller
*                                          for these features
*    29Dec04     0.01.44            DW     CQ 9356: Added some compiler tokens for commonly
*                                          used numbers. Modified the feature-related APIs
*                                          to use these. Added a parameter validity check
*                                          in these APIs.
*    27Jan05     0.01.44            CPH    Added Ohio250 support.
*    02Feb05     0.01.45            CPH    Pull dev_host_interface.h out from dsl_hal_register.h
*    15Jan05     0.01.45         T.Leyrer  Added support for maxBits configuration in
*                                          each direction
*    29Mar05     0.01.46            CPH    Restore dslhal_api_setMaxBitsPerCarrier as function
*
*    10May05                        Arvind  CQ 9605:Added a new API
*                                           dslhal_api_setTrainingMode_BitField()
*                                           for allowing the user to select from
*                                           the dsl modes using bitfields.
*    14Jun05                        CPH     Added AnnexB & M support.
*                                           - Extend dslhal_api_setTrainingMode_BitField()
*                                           - Add dslhal_support_getTrainedModeEx()
*    29Jun05                        CPH    Optimization: consolidate dslhal_api_gatherStatistics() and
*                                          dslhal_api_initStatistics()
*    7July05                        CPH    Optimization: consolidate dslhal_api_pollTrainingStatus() and
*                                          dslhal_api_handleTrainingInterrupt()
*
*    10May05                        Arvind  CQ 9605: Added a new field in the
*                                           tagTIOIDINFO structure, for allowing the user to
*                                           select from the dsl modes using bitfields.
*    2June05     0.01.47            CPH     CQ 9690:Preserve Preserve exisiting PRCR reset status
*                                           for other pheripherals in dslhal_api_boostDspFrequency().
*    29Jun05     0.01.48            AV      Fixed training mode statistics bug.
*    20Jul05     0.01.49            AV      CQ 9802: Moved the order when the OHIO_250_MODE
*                                           is selected relative to when the System PLL is
*                                           initialized. Also, added the power down of the
*                                           SAR, DSP and ADSLSS when the frequency is being
*                                           boosted to 250MHz.
*    10Aug05     0.01.50            CPH     Clean Up, fixed dsGains[] negative index problem.
*    5Sept05     0.01.60            CPH     CQ9776: Added dslhal_api_getHLOGpsds(), dslhal_api_getQLNpsds(),
*                                           and dslhal_api_getSNRpsds().
*    29Sept05    0.01.70            CPH     Fixed ATM driver hang before showtime problem.
*    04Oct05     0.01.71            CPH     Added 7100A1/A2 support.
*    7 Oct05     0.01.80            AV/CPH  Changed dprintf to dgprintf for code size reduction.
*    03Nov05     0.01.80            Manjula CQ10037: Modified dslhal_api_setEocRevisionNumber to support
*                                           Inventory command to set version number.
*    04Nov05     0.11.00            CPH     Fixed T1413 mode got Zero DS/US rate when DSL_BIT_TMODE is set.
*
*    23Nov05     0.12.00            CPH     Added set LPR when get DSP_GASP.
*    30Nov05     0.12.01            KM/JZ   CQ10226: Added processing Loss of Framing (LOF) in 
*                                           dslhal_api_processMailBox(), and setting errored seconds in 
*                                           dslhal_api_gatherStatistics().
*    01Dec05     0.12.00            AV      Fixed an issue with dslhal_api_boostDspFrequency(), where it 
*                                           sometimes would not boost the DSP frequency for Ohio chips.
*    08Aug06     0.13.00            GPet/CPH CQ 10242: Added CLI2Host support. (token:CLI2HOST_SUPPORT)*
* UR8_MERGE_START  Report_SES       Manjula K
*    14Mar06       0.14.00          MK            CQ10369: Added code to report severly errored seconds in 
*                                                 dslhal_api_gatherStatistics(
* UR8_MERGE_END  Report_SES
* UR8_MERGE_START  CQ10386          PeterHou
*    15Mar06       0.15.00          CPH           CQ10386: Add SNR format comment to dslhal_api_getSNRGpsds().
* UR8_MERGE_END    CQ10386
* UR8_MERGE_START GERALD_CQ_REDIRECT_BUGFIX2_20060328 Gerald
*    28Mar06       0.16.00          GPet          CQ10411: Fixed CLIredirect reboot crash
* UR8_MERGE_END GERALD_CQ_REDIRECT_BUGFIX2_20060328
* UR8_MERGE_START  CQ10481          Jack Zhang
*    21Apr06       0.16.00          Jack Zhang    CQ10481: Selected Mode" in /proc/avalanche/avsar_modem_stats 
*                                                 not displaying correct value
* UR8_MERGE_END    CQ10481
* UR8_MERGE_START  CQ10499          Jack Zhang
*    05May06                                      CQ10499: ATM Driver Cleanup/Improvement Umbrella
* UR8_MERGE_END    CQ10499
* UR8_MERGE_START  CQ10442          Manjula K
*    31May06       0.17.00          MK            CQ10442: Added code to handle mailbox message for SRA in
*                                                 dslhal_api_processMailBox()
* UR8_MERGE_END    CQ10442
* UR8_MERGE_START  CQ10905          Jeremy L 
*    05Sept06                                     CQ10905: Modified HAL to display TrainedPath
* UR8_MERGE_END    CQ10905                        correctly for ADSL2/2+
* 
 * UR8_MERGE_START CQ10880   Jack Zhang
 * 8/30/06  JZ   CQ10880: Add DSL HAL API for sending mailbox message for L3
 * UR8_MERGE_END   CQ10880*
// UR8_MERGE_START CQ11007  KCCHEN
// 09/26/06 KCCHEN       CQ11007 : US SNR margin update
// UR8_MERGE_END CQ11007 KCCHEN
*  UR8_MERGE_START CQ10978   Jack Zhang
*  10/4/06  JZ     CQ10978: Request for the DSL Power Management Status Report
*  UR8_MERGE_END   CQ10978*
*  UR8_MERGE_START CQ10979   Jack Zhang
*  10/4/06  JZ     CQ10979: Request for TR-069 Support for RP7.1
*  UR8_MERGE_END   CQ10979*
// UR8_MERGE_START CQ11057 KCCHEN
// 10/12/06 Kuan-Chen Chen   CQ11057: Request US PMD test parameters from CO side
// UR8_MERGE_END CQ11057 KCCHEN
*  UR8_MERGE_START   CQ11228 HZ
*  12/08/06     Hao Zhou CQ11228: Modify the DS Margin report to 0.1dB precision, also take care of 
*                                 possible negative values.
*  UR8_MERGE_END   CQ11228 HZ 
*  UR8_MERGE_START CQ11054   Jack Zhang
*  10/11/06  JZ     CQ11054: Data Precision and Range Changes for TR-069 Conformance
*  UR8_MERGE_END   CQ11054*
*  UR8_MERGE_START_END CQ11247_TR69_DS_LATN_SATN  YW
*  12/18/06   Yan Wang      CQ11247: TR069 range and precision changes for LATNds, SATNds  
******************************************************************************/
#include <dev_host_interface.h>
#include <dsl_hal_register.h>
#include <dsl_hal_support.h>

#ifndef NO_ADV_STATS
#include <dsl_hal_logtable.h>
#endif

#include <dsl_hal_version.h>

//  UR8_MERGE_START CQ11054   Jack Zhang
static unsigned int highprecision_selected = 0;  //By default we use low precision for backward compt.
//  UR8_MERGE_END   CQ11054*
static unsigned int hybrid_selected;
static unsigned int showtimeFlag = FALSE;
static unsigned char triggerDsp250MHZ;
static int dslhal_api_processMailBox(tidsl_t *ptidsl,
  DEV_HOST_dspOamSharedInterface_t *p_dspOamSharedInterface, int intFlag);

/* CQ 9802: Number of power down retries. */
#ifndef PWR_DN_RETRYS
#define PWR_DN_RETRYS   3
#endif

#ifndef NO_ADV_STATS
static short dslhal_staticapi_getHlog(tidsl_t *ptidsl,unsigned int index);
#endif

// DW 12/29/04 CQ 9356 Common declarations
#define NUM_BYTES_PER_INT                          4  // 32 bits to an integer
#define NUM_BYTES_PER_ADDRESS                      4  // 32-bit addresses
#define NUM_PHY_FEATURE_LEVELS                     3  // Pointer to a pointer to a structure format
                                                      // is used for Phy features
#define MAX_PHY_FEATURE_PARAMID                    3  // Currently 4 parameter IDs exist:
                                                      // 0-1 for Phy features and 2-3 for Interop
                                                      // features.

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

//  UR8_MERGE_START CQ11054   Jack Zhang
unsigned int dslhal_api_getHighPrecision()
{
    return highprecision_selected;
}
void dslhal_api_setHighPrecision()

{
    highprecision_selected = 1;
}
//  UR8_MERGE_END   CQ11054*

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
  dgprintf(4,"dslhal_api_dslStartup() NEW 1\n");

  ptidsl=(ITIDSLHW_T *)shim_osAllocateMemory(sizeof(ITIDSLHW_T));
  if(ptidsl==NULL)
  {
    dgprintf(1, "unable to allocate memory for ptidsl\n");
    return 1;
  }
  *ppIHw=ptidsl;
  shim_osZeroMemory((char *) ptidsl, sizeof(ITIDSLHW_T));

  /* Unreset the ADSL Subsystem  */
  rc=dslhal_support_unresetDslSubsystem();
  if(rc)
  {
    dgprintf(1, "unable to reset ADSL Subsystem \n");
    shim_osFreeMemory((void *) ptidsl, sizeof(ITIDSLHW_T));
    return DSLHAL_ERROR_UNRESET_ADSLSS;
  }
  ptidsl->fwimage = shim_osAllocateVMemory(DSP_FIRMWARE_MALLOC_SIZE);
  if(!ptidsl->fwimage)
  {
    dgprintf(1,"Failed to Allocate Memory for DSP firmware binary \n");
    return DSLHAL_ERROR_FIRMWARE_MALLOC;
  }
  /* read firmware file from flash   */
  rc=shim_osLoadFWImage(ptidsl->fwimage);
  if(rc<0)
    {
    dgprintf(1, "unable to get fw image\n");
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
    dgprintf(0,"dsp load error\n");
    for(i=0; i<ptidsl->numOlayPages; i++)
    {
      if(ptidsl->olayDpPage[i].PmemStartWtAddr !=NULL)
      {
        shim_osFreeDmaMemory((void *) ptidsl->olayDpPage[i].PmemStartWtAddr,
                             ptidsl->olayDpPage[i].OverlayXferCount);
      }
    }
    for(i=0; i<ptidsl->numProfiles; i++)
    {
      if(ptidsl->coProfiles[i].PmemStartWtAddr != NULL)
        shim_osFreeDmaMemory((void *)ptidsl->coProfiles[i].PmemStartWtAddr, ptidsl->coProfiles[i].OverlayXferCount);
    }
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

  if (triggerDsp250MHZ)
  {
    dslhal_support_setDsp250MHzTrigger(ptidsl);
  }

  /* unreset Raptor */
  /* change this to new function */
  /* This function should basically bring DSP out of reset bit 23  of PRCR */
  /* Function is ready but bypassed for Pre-Silicon */

  rc=dslhal_support_unresetDsp();

  if (rc)
  {
    dgprintf(0,"unable to bring DSP out of Reset\n");
    for(i=0; i<ptidsl->numOlayPages; i++)
    {
      if(ptidsl->olayDpPage[i].PmemStartWtAddr !=NULL)
      {
        shim_osFreeDmaMemory((void *) ptidsl->olayDpPage[i].PmemStartWtAddr,
                             ptidsl->olayDpPage[i].OverlayXferCount);
      }
    }
    for(i=0; i<ptidsl->numProfiles; i++)
    {
      if(ptidsl->coProfiles[i].PmemStartWtAddr != NULL)
        shim_osFreeDmaMemory((void *)ptidsl->coProfiles[i].PmemStartWtAddr, ptidsl->coProfiles[i].OverlayXferCount);
    }
    if(ptidsl->constDisplay.PmemStartWtAddr != NULL)
      shim_osFreeDmaMemory((void *)ptidsl->constDisplay.PmemStartWtAddr, ptidsl->constDisplay.OverlayXferCount);
    shim_osFreeVMemory((void *)ptidsl->fwimage,DSP_FIRMWARE_MALLOC_SIZE);
    shim_osFreeMemory((void *) ptidsl, sizeof(ITIDSLHW_T));
    return DSLHAL_ERROR_UNRESET_DSP;
  }
  shim_osFreeVMemory((void *)ptidsl->fwimage,DSP_FIRMWARE_MALLOC_SIZE);
  dgprintf(4,"dslhal_api_dslStartup() done\n");

  /* Add the code to initialize the host interface variables */
  ptidsl->AppData.useBitField = FALSE;

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

  dgprintf(5, "dslhal_api_dslShutdown\n");
  rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DSLSS_SHUTDOWN, 0, 0, 0);
  if(rc)
  {
    dgprintf(1, " unable to reset DSP \n");
    rc = DSLHAL_ERROR_RESET_DSP;
  }
  /* DSP need 50 ms to send out the message*/

  shim_osClockWait(60 * 1000);

  rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DGASP, 0, 0, 0);

  /* free memory allocated*/

  for(i=0; i<ptidsl->numOlayPages; i++)
  {
    if(ptidsl->olayDpPage[i].PmemStartWtAddr !=NULL)
    {
      shim_osFreeDmaMemory((void *) ptidsl->olayDpPage[i].PmemStartWtAddr,
                           ptidsl->olayDpPage[i].OverlayXferCount);
    }
  }
  for(i=0; i<ptidsl->numProfiles; i++)
  {
    if(ptidsl->coProfiles[i].PmemStartWtAddr != NULL)
      shim_osFreeDmaMemory((void *)ptidsl->coProfiles[i].PmemStartWtAddr, ptidsl->coProfiles[i].OverlayXferCount);
  }
  if(ptidsl->constDisplay.PmemStartWtAddr != NULL)
    shim_osFreeDmaMemory((void *)ptidsl->constDisplay.PmemStartWtAddr, ptidsl->constDisplay.OverlayXferCount);
  shim_osFreeMemory((void *)ptidsl, sizeof(tidsl_t));
  rc = dslhal_support_resetDsp();
  if(rc)
  {
    dgprintf(1, " unable to reset ADSL subsystem \n");
    rc = DSLHAL_ERROR_RESET_DSP;
  }
  rc = dslhal_support_resetDslSubsystem();
  if(rc)
  {
    dgprintf(1, " unable to reset ADSL subsystem \n");
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
 * FUNCTION NAME: dslhal_api_processMailBox()
 *
 *********************************************************************************************
 * DESCRIPTION: Common routine only used in dslhal_api_handleTrainingInterrupt()
 *    and dslhal_api_pollTrainingStatus()
 * Input: tidsl_t *ptidsl
 *        DEV_HOST_dspOamSharedInterface_t *p_dspOamSharedInterface
 *        intFlag : 0: polling, 1: interrupt
 *
 * Return: modem status
 *         -1    failed
 *
 ********************************************************************************************/
#if 1 //cph
static int dslhal_api_processMailBox(tidsl_t *ptidsl,
  DEV_HOST_dspOamSharedInterface_t *p_dspOamSharedInterface, int intFlag)
{
  int cmd;
  int tag;
  int parm1,parm2;
//  unsigned int msg1;
//  unsigned int msg2;
  int rc;
  unsigned int failState;
  static unsigned int GhsIndex=0;

  dgprintf(4,"dslhal_api_processMailBox\n");

  while (dslhal_support_readDspMailbox(ptidsl,&cmd, &tag, &parm1, &parm2) == DSLHAL_ERROR_NO_ERRORS )
  {
    dgprintf(4,"mailbox message: 0x%x\n", cmd);

    switch (cmd) {
      case DSP_IDLE:
      {
        dgprintf(4,"DSP_IDLE\n");
        ptidsl->lConnected=0;
        ptidsl->AppData.bState=0;
        hybrid_selected=888;

        if ((intFlag == 0)  // if polling(intFlag=0), put some delay
          && (ptidsl->bAutoRetrain == 0) )
        {
          while(ptidsl->bOverlayPageLoaded == 0)
            shim_osClockWait(6400);
        }
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
          if(ptidsl->AppData.trainFails >= 30)
            ptidsl->AppData.trainFails=0;
        }
        for(GhsIndex=0;GhsIndex<10;GhsIndex++)
        {
          for(rc=0;rc<62;rc++)
            ptidsl->AppData.dsl_ghsRxBuf[GhsIndex][rc]=0;
        }
        GhsIndex=0;

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
        break;
      }

      case DSP_ATM_TC_SYNC:
      {
        dgprintf(4,"\nDSP_ATM_TC_SYNC\n");
        showtimeFlag = TRUE;
        ptidsl->lConnected=1;
        if(ptidsl->bAutoRetrain == 0 && ptidsl->bOverlayPageLoaded == 1)
        {
          dslhal_support_clearTrainingInfo(ptidsl);
          ptidsl->bOverlayPageLoaded = 0;
        }
        break;
      }

      case DSP_ACTIVE:
      {
        if (intFlag) // if interrupt (intFlag=1)
          ptidsl->AppData.showtimeCount ++;
        else // polling (intFlag=0)
          ptidsl->lConnected=0;

        ptidsl->AppData.bState = RSTATE_SHOWTIME;

        dgprintf(4,"DSP_ACTIVE");

        //
        // The dslhal_support_getMaxUsTones()
        // below will also call dslhal_support_getTrainedMode(), and read
        // TrainedMode, annex_selected & psd_mask_qualifier.
        //
        ptidsl->AppData.max_us_tones = dslhal_support_getMaxUsTones(ptidsl);
        ptidsl->AppData.max_ds_tones = (ptidsl->AppData.TrainedMode & ADSL2PLUS_MASKS) ? 512: 256; //dslhal_support_getMaxDsTones(ptidsl);
#if 1 // LOF  CQ10226
        // clean the LOF and LPR flags/counters.
        ptidsl->AppData.LOF_f = 0;
        ptidsl->AppData.LOF_errors = 0;
        ptidsl->AppData.coLOF_f = 0;
        ptidsl->AppData.coLofErrors = 0;
        ptidsl->AppData.LPR = 0; // clear Loss-of-Power primitive
#endif
        break;
      }

      case DSP_ATM_NO_TC_SYNC:
      {
        dgprintf(4,"\nDSP_ATM_TC_NOSYNC\n");
        ptidsl->lConnected=0;
        /* add code for reload overlay pages */
        break;
      }

      case DSP_OVERLAY_START:
      {
        dgprintf(2,"DSP_OVERLAY_START: %d \n",tag);
        break;
      }

      case DSP_OVERLAY_END:
      {
        dgprintf(2,"DSP_OVERLAY_END: %d \n",tag);
        rc = dslhal_support_checkOverlayPage(ptidsl,tag);
        if(rc == DSLHAL_ERROR_OVERLAY_CORRUPTED)
        {
          dgprintf(0,"Overlay Page: %d CORRUPTED \n",tag);
          return(0-DSLHAL_ERROR_OVERLAY_CORRUPTED);
        }
        break;
      }

      case DSP_HYBRID:
      {
        dgprintf(2,"DSP_HYBRID (Hybrid Metrics Avail)\n");
        hybrid_selected = tag;
        break;
      }

      case DSP_XMITBITSWAP:
      {
        dgprintf(4,"DSP_XMITBITSWAP\n");
        rc = dslhal_support_aocBitSwapProcessing(ptidsl,0);
        break;
      }

      case DSP_RCVBITSWAP:
      {
        dgprintf(4, "DSP_RCVBITSWAP\n");
        rc = dslhal_support_aocBitSwapProcessing(ptidsl,1);
        break;
      }

      case DSP_GHSMSG:
      {
        dgprintf(3,"DSP_GHSMSG: Rcvd bytes: %d \n",tag);
        dgprintf(3,"Addr: 0x%x\n", p_dspOamSharedInterface->ghsDspRxBuf_p);
        if(GhsIndex > 9)
          GhsIndex=0;
        rc = dslhal_support_blockRead(
          (void *)dslhal_support_byteSwap32((unsigned int)p_dspOamSharedInterface->ghsDspRxBuf_p),
          &ptidsl->AppData.dsl_ghsRxBuf[GhsIndex++][0], tag);
        break;
      }

      case DSP_CRATES1:
      {
        dgprintf(3,"DSP_CRATES1: (C-Rates1 Ready)\n");
        rc = dslhal_support_gatherRateMessages(ptidsl);
        break;
      }

      case DSP_SNR:
      {
        dgprintf(3,"DSP_SNR: SNR Data Ready\n");
        rc = dslhal_support_gatherSnrPerBin(ptidsl,tag);
        break;
      }

      case DSP_EOC:
      {
        dgprintf(3,"DSP_EOC message:tag=%02x, cmd1=%02x, cmd2=%02x (%s%02x)\n", tag, parm1, parm2,
          (parm1 & 0x04) ? "Reg" : "Data",
          ((parm1>>5)|(parm2<<3))&0xFF);
        rc = dslhal_support_gatherEocMessages(ptidsl,tag,parm1,parm2);
        break;
      }

      case DSP_TRAINING_MSGS:
      {
        dgprintf(3,"DSP_TRAINING_MSGSi \n");
        rc = dslhal_support_gatherAdsl2Messages(ptidsl,tag,parm1,parm2);
        break;
      }

      case DSP_CLEAR_EOC:
      {
        dgprintf(2, "DSP_CLEAR_EOC:tag=%02x, cmd1=%02x, cmd2=%02x\n", tag, parm1, parm2);
        ptidsl->AppData.clear_eoc = 1;
        break;
      }

      case DSP_DGASP:
      {
        dgprintf(0,"\nDSP_GASP!!!\n");
        ptidsl->AppData.LPR = 1;
        break;
      }

#if 1 // LOF  CQ10226
      case DSP_LOF:
      {
        dgprintf(0,"\nDSP_LOF!!! tag=%02x, cmd1=%02x, cmd2=%02x\n",  tag, parm1, parm2);
        if (parm1 == 0)
        {
          ptidsl->AppData.LOF_f = parm2;
          ptidsl->AppData.LOF_errors += parm2;
        }
        else
        {
          ptidsl->AppData.coLOF_f = parm2;
          ptidsl->AppData.coLofErrors += parm2;
        }
        break;
      }
#endif

      //UR8_MERGE_START CQ10442 Manjula K
      case DSP_SRA:
      {
        dgprintf(0,"\nDSP_SRA!!! tag=%02x, cmd1=%02x, cmd2=%02x\n",  tag, parm1, parm2);                  
        ptidsl->AppData.DSConRate = (parm2<<8) | parm1;               
        ptidsl->AppData.SRA = 1;    
        break;
      }
      //UR8_MERGE_END CQ10442

      default:
      {
        dgprintf(0,"DSP_Unknown? 0x%x\n", cmd);
        break;
      }
    }; // switch
  } // while

#if 0 //cph999
  dslhal_support_readTextMailbox(ptidsl,&msg1, &msg2);
  dgprintf(5,"Text Message Part1: 0x%x \t Text Message Part2: 0x%x \n",msg1,msg2);
#endif

  return DSLHAL_ERROR_NO_ERRORS;
}
#endif

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
  int rc;

  /*char *tmp;*/
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
#if SWTC
  DEV_HOST_tcHostCommDef_t          TCHostCommDef;
#endif

  dgprintf(5,"dslhal_api_pollTrainingStatus\n");
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterface_t *) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,
                                sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }
#if SWTC
  dspOamSharedInterface.tcHostComm_p =(DEV_HOST_tcHostCommDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.tcHostComm_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.tcHostComm_p,
                                &TCHostCommDef, sizeof(DEV_HOST_tcHostCommDef_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }
#endif

  rc = dslhal_support_processTrainingState(ptidsl);
  if(rc)
  {
    dgprintf(0,"Error Reading Modem Training State \n");
    return DSLHAL_ERROR_MODEMSTATE;
  }
  rc = dslhal_support_processModemStateBitField(ptidsl);
  if(rc)
  {
    dgprintf(0,"Error Reading Modem Training State \n");
    return DSLHAL_ERROR_MODEMSTATE;
  }

  dslhal_api_processMailBox(ptidsl, &dspOamSharedInterface, 0); // 0 for polling

  dgprintf(6,"dslhal_api_pollTrainingStatus done\n");

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
  int rc;

  /*char *tmp;*/
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
#if SWTC
  DEV_HOST_tcHostCommDef_t          TCHostCommDef;
#endif
  dgprintf(6,"dslhal_api_handleTrainingInterrupt\n");
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterface_t *) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,
                                sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }
#if SWTC
  dspOamSharedInterface.tcHostComm_p =(DEV_HOST_tcHostCommDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.tcHostComm_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.tcHostComm_p,
                                &TCHostCommDef, sizeof(DEV_HOST_tcHostCommDef_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }
#endif

  if(intrSource & MASK_BITFIELD_INTERRUPTS)
  {
//cph    dspOamSharedInterface.modemStateBitField_p =(DEV_HOST_modemStateBitField_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemStateBitField_p);
    rc = dslhal_support_processTrainingState(ptidsl);
    if(rc)
    {
      dgprintf(0,"Error Reading Modem Training State \n");
      return DSLHAL_ERROR_MODEMSTATE;
    }
    rc = dslhal_support_processModemStateBitField(ptidsl);
    if(rc)
    {
      dgprintf(0,"Error Reading Modem Training State \n");
      return DSLHAL_ERROR_MODEMSTATE;
    }
  }

  if(intrSource & MASK_MAILBOX_INTERRUPTS)
  {
    dslhal_api_processMailBox(ptidsl, &dspOamSharedInterface, 1); // 1 for interrupt
  }

  dgprintf(6,"dslhal_api_handleTrainingInterrupt done\n");
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

  dgprintf(5, "dslhal_cfg_dslRetrain \n");
  rc = dslhal_support_writeHostMailbox(ptidsl, HOST_QUIET, 0, 0, 0);
  if(rc)
  {
    dgprintf(1,"dslhal_cfg_dslRetrain failed\n");
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

  dgprintf(5, "dslhal_api_sendQuiet\n");
  rc = dslhal_support_writeHostMailbox(ptidsl, HOST_QUIET, 0, 0, 0);
  if(rc)
  {
    dgprintf(1,"dslhal_api_sendQuiet failed\n");
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

  dgprintf(5, "dslhal_api_sendIdle\n");
  rc = dslhal_support_writeHostMailbox(ptidsl, HOST_IDLE, 0, 0, 0);
  if(rc)
  {
    dgprintf(1,"dslhal_api_sendIdle failed\n");
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

  dgprintf(5, "dslhal_api_sendDgasp\n");
  rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DGASP, 0, 0, 0);
  if(rc)
  {
    dgprintf(1,"dslhal_api_sendDgasp failed\n");
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


  dgprintf(5," dslhal_api_setTrainingMode()\n");

//  if(trainmode>255)
  if(trainmode>0xFFFF) // new trainmode is 16 bit
  {
    dgprintf(3,"Invalid Value (%08x) for Desired Training Mode (must be <0xFFFF)\n", trainmode);
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,
                                sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&NegoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  /* Enum Translation to maintain backwards compatibility for train modes */
  NegoPara.stdMode_byte2 = 0;  // clear std_mode extention byte2-4
  NegoPara.stdMode_bytes34 = 0;
  if(!ptidsl->AppData.useBitField)
  {
      if(trainmode == DSLTRAIN_MULTI_MODE)
        trainmode = MULTI_MODE8;
      if(trainmode == DSLTRAIN_T1413_MODE)
        trainmode = T1413_MODE;
      if(trainmode == DSLTRAIN_GDMT_MODE)
        trainmode = GDMT_MODE;
  }

  NegoPara.stdMode = trainmode & 0xFF;
  if (NegoPara.stdMode==MULTI_MODE8)  // for backward compatibility
    NegoPara.stdMode_byte2 = MULTI_MODE8; // extend multi mode to extra byte
  else
    NegoPara.stdMode_byte2 = (trainmode >> 8) & 0xFF;

  dgprintf(5,"Train Mode: 0x%x\n",trainmode);
  rc = dslhal_support_blockWrite(&NegoPara,(PVOID)dspOamSharedInterface.oamWriteNegoParams_p, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;

  dgprintf(5," dslhal_api_setTrainingMode() Done\n");
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
  dgprintf(5, "dslhal_api_getDspVersion\n");
  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  if(!pVer)
    return DSLHAL_ERROR_INVALID_PARAM;

  *(unsigned int *) pVer = 0;

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.datapumpVersion_p = (DEV_HOST_dspVersionDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.datapumpVersion_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.datapumpVersion_p,
                                pVer, sizeof(dspVer));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
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
  int rc,optIdxU,optIdxD;
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
#if 1 // LOF  CQ10226
  DEV_HOST_MGMTCount_t          mgmt_Count;
  BIS_MGMT_CountersDef_t        BIS_MGMT_Counters;
#endif
  // UR8_MERGE_START CQ11057 KCCHEN
  DEV_HOST_BIS_PMD_TEST_PARAMETERS_FROM_CO_Def_t *BisPmdTestParametersUS_p;
 // UR8_MERGE_START_END CQ11247_TR69_DS_LATN_SATN  YW  
  SINT16                  usMargin;
  int                  usMargintmp;
  int offset_BisPmdTestParametersUS[] = {7,20};
  // UR8_MERGE_END CQ11057 KCCHEN
//  int                           dsNumTones=256;
//  unsigned int                  TrainedModeEx; //cph999
  unsigned int                  ghsATUCVendorIdAddr;
  int offset[]= {7, 11};        // DEV_HOST_modemEnv_t.ghsATUCVendorIdAddr
//  UR8_MERGE_START_END   CQ11228 HZ
  SINT16                dsMargintmp;

  dgprintf(5, "dslhal_api_gatherStatistics\n");

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,
                                sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return;
  }
  if(!ptidsl->bStatisticsInitialized && ptidsl->lConnected == LINE_CONNECTED)
  {
    //    dslhal_api_initStatistics(ptidsl);
    ptidsl->bStatisticsInitialized = TRUE;
  }

  dspOamSharedInterface.dspWriteNegoParams_p = (DEV_HOST_dspWrNegoParaDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspWriteNegoParams_p,
                                &rateparms, sizeof(DEV_HOST_dspWrNegoParaDef_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return;
  }
  if(!rc)
  {
#if 1 //cph999
      // TrainedMode was set when just enter showtime
//      ptidsl->AppData.dsl_modulation = ptidsl->AppData.TrainedMode;
      ptidsl->AppData.dsl_modulation = dslhal_support_getTrainedMode(ptidsl);
#else
      ptidsl->AppData.annex_selected = dslhal_support_byteSwap16(rateparms.annex_selected);
      ptidsl->AppData.psd_mask_qualifier= dslhal_support_byteSwap16(rateparms.psd_mask_qualifier);
      TrainedModeEx= dslhal_support_getTrainedModeEx(rateparms.trainMode, rateparms.annex_selected);
      /* If the new Bit field method is being used don't do the translation. */
      /* trained mode */
      ptidsl->AppData.dsl_modulation = TrainedModeEx;      //cph99
      if(!ptidsl->AppData.useBitField)
      {
        /* trained mode */
//cph99, move up  ptidsl->AppData.dsl_modulation = TrainedModeEx;
        if (TrainedModeEx == T1413_MODE)
          ptidsl->AppData.dsl_modulation = DSLTRAIN_T1413_MODE;
        else if (TrainedModeEx == GDMT_MODE)
          ptidsl->AppData.dsl_modulation = DSLTRAIN_GDMT_MODE;
      }
#endif

//   if(TrainedModeEx & ADSL2PLUS_MASKS)
//      dsNumTones = 512;
    /* rate */
    /*    shim_osMoveMemory((void *)ptidsl->AppData.bCRates1, (void *)rateparms.cRates1, 120); */
    ptidsl->AppData.bCRates2 = rateparms.cRates2;
    /*    shim_osMoveMemory((void *)ptidsl->AppData.bRRates1, (void *)rateparms.rRates1, 44); */
    ptidsl->AppData.bRRates2 = rateparms.rRates2;
    shim_osMoveMemory((void *)ptidsl->AppData.bCMsgs1, (void *)rateparms.cMsgs1, 12);
    shim_osMoveMemory((void *)ptidsl->AppData.bCMsgs2, (void *)rateparms.cMsgs2, 4);
    shim_osMoveMemory((void *)ptidsl->AppData.bRMsgs2, (void *)rateparms.rMsgs2, 4);
    ptidsl->AppData.atucVendorId = (unsigned int)rateparms.atucVendorId;
    ptidsl->AppData.lineLength = (unsigned int)dslhal_support_byteSwap16((unsigned short)rateparms.lineLength);
    ptidsl->AppData.atucRevisionNum = (unsigned int)rateparms.atucGhsRevisionNum;
    if(ptidsl->AppData.dsl_modulation < ADSL2_MODE)
      {
        // UR8_MERGE_START CQ11247_TR69_DS_LATN_SATN  YW
        ptidsl->AppData.usLineAttn = ((ptidsl->AppData.bCMsgs2[3] >>2)&0x003f); // 0.5dB step 
        ptidsl->AppData.usMargin = (ptidsl->AppData.bCMsgs2[2])&0x001f; // 1dB step 
        if (dslhal_api_getHighPrecision())
        {
          ptidsl->AppData.usLineAttn *= 5; // 0.1dB step
          ptidsl->AppData.usMargin   *= 10; // 0.1dB step
        }
        // UR8_MERGE_END CQ11247_TR69_DS_LATN_SATN  YW
      }
    else
      {
        // UR8_MERGE_START CQ11247_TR69_DS_LATN_SATN  YW
        /* Attenuation is precise to the steps of 0.1 dB */
        ptidsl->AppData.usLineAttn = (ptidsl->adsl2TrainingMessages.cParams[1] & 0x3);
        ptidsl->AppData.usLineAttn <<=8;
        ptidsl->AppData.usLineAttn += ptidsl->adsl2TrainingMessages.cParams[0];
        // UR8_MERGE_END CQ11247_TR69_DS_LATN_SATN  YW
        // UR8_MERGE_START CQ11057 KCCHEN
        BisPmdTestParametersUS_p = (DEV_HOST_BIS_PMD_TEST_PARAMETERS_FROM_CO_Def_t *)PHYS_TO_K1(dslhal_support_readInternalOffset(ptidsl, 2, offset_BisPmdTestParametersUS));
        usMargintmp = *(((unsigned int *)&BisPmdTestParametersUS_p->usMargin));
        // UR8_MERGE_START CQ11247_TR69_DS_LATN_SATN  YW
        usMargin = (SINT16) (usMargintmp&0xffff);
        ptidsl->AppData.usMargin = (int)usMargin;
        // UR8_MERGE_END CQ11247_TR69_DS_LATN_SATN  YW
        // UR8_MERGE_END CQ11057 KCCHEN
        // UR8_MERGE_START CQ11247_TR69_DS_LATN_SATN  YW
        if (dslhal_api_getHighPrecision() == 0)
        {
          ptidsl->AppData.usLineAttn /= 5; // 0.5dB step
          ptidsl->AppData.usMargin   /= 10; // 1dB step
        }
        // UR8_MERGE_END CQ11247_TR69_DS_LATN_SATN  YW
      }
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

    dgprintf(5, "optIdxU=%d\n", optIdxU);

    /* Obtain the US Rates using Opt# and CRates1 Table */
    /* Rate(US) = [Bf(LS0) + Bi(LS0)]*32                */
//    if(ptidsl->AppData.dsl_modulation <= DSLTRAIN_GLITE_MODE)
    if (dslhal_support_IsADSL1Mode(ptidsl))
      ptidsl->AppData.USConRate = ((rateparms.cRates1[optIdxU][CRATES1_BF_LS0] + rateparms.cRates1[optIdxU][CRATES1_BI_LS0]) * 32);
    else
      ptidsl->AppData.USConRate =  dslhal_support_byteSwap16((unsigned short)rateparms.adsl2USRate);

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
//    if(ptidsl->AppData.dsl_modulation <= DSLTRAIN_GLITE_MODE)
    if (dslhal_support_IsADSL1Mode(ptidsl))
      ptidsl->AppData.DSConRate =  (((rateparms.cRates1[optIdxD][CRATES1_BF_AS0]|((rateparms.cRates1[optIdxD][CRATES1_BF_DSRS]&0x80)<<1))+ (rateparms.cRates1[optIdxD][CRATES1_BI_AS0]|((rateparms.cRates1[optIdxD][CRATES1_BI_DSRS]&0x80)<<1)))* 32);
    else
      ptidsl->AppData.DSConRate = dslhal_support_byteSwap16((unsigned short)rateparms.adsl2DSRate);

    dgprintf(5, "ptidsl->AppData.wDSConRate=%d\n", ptidsl->AppData.DSConRate);
    /* Determine which Path has Modem Trained with */
//    if(ptidsl->AppData.dsl_modulation <= DSLTRAIN_GLITE_MODE)
    if (dslhal_support_IsADSL1Mode(ptidsl))
      {
         if((rateparms.cRates1[optIdxU][CRATES1_BF_LS0]) && (rateparms.cRates1[optIdxD][CRATES1_BF_AS0]))
            ptidsl->AppData.TrainedPath = FAST_PATH;
         else
            ptidsl->AppData.TrainedPath = INTERLEAVED_PATH;
      }
    // UR8_MERGE_START - CQ10905 - Jeremy L
    // checks to see if ADSL2 or ADSL2 Annex M
    else if (dslhal_support_getTrainedMode(ptidsl) == 8 || dslhal_support_getTrainedMode(ptidsl) == 1024) 
      {
    // ADSL2 R-PARAMs message is 808 in length
    // Rp and Dp are in the 10th octet per 992.3, but of the PMS-TC section of R-PARAMS which is after PMD-TC
         if((ptidsl->adsl2TrainingMessages.rParams[790] & 0x07) == 0)
            ptidsl->AppData.TrainedPath = FAST_PATH;
         else
            ptidsl->AppData.TrainedPath = INTERLEAVED_PATH;
      }
    // since ADSL1 and 2 are checked above, this would indicated ADSL2+
    else
      {
    // ADSL2+ R-PARAMs message is 2088 in length
    // Rp and Dp are in the 10th octet per 992.3, but of the PMS-TC section of R-PARAMS which is after PMD-TC
         if((ptidsl->adsl2TrainingMessages.rParams[2070] & 0x07) == 0)
            ptidsl->AppData.TrainedPath = FAST_PATH;
         else
            ptidsl->AppData.TrainedPath = INTERLEAVED_PATH;
      }
     // UR8_MERGE_END - CQ10905 - Jeremy L
     
    /* Set the mode in which the modem is trained */
#if 0 //cph99
    ptidsl->AppData.TrainedMode = TrainedModeEx;
    ptidsl->AppData.max_ds_tones = (TrainedModeEx & ADSL2PLUS_MASKS) ? 512: 256; // dslhal_support_getMaxDsTones(ptidsl);
    ptidsl->AppData.max_us_tones = dslhal_support_getMaxUsTones(ptidsl);

    /* Do the translation only if the bit feild mode is not set for backward compatibility */
    if(!ptidsl->AppData.useBitField)
    {
        if(rateparms.trainMode == T1413_MODE)
            ptidsl->AppData.TrainedMode = DSLTRAIN_T1413_MODE;
        if(rateparms.trainMode == GDMT_MODE)
            ptidsl->AppData.TrainedMode = DSLTRAIN_GDMT_MODE;
    }
#endif

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
    dgprintf(1,"dslhal_support_blockRead failed\n");
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
//  UR8_MERGE_START CQ10979   Jack Zhang
      ptidsl->AppData.dsACTPSD = dslhal_support_byteSwap32(StatisticsDef.dsACTPSD);
      ptidsl->AppData.usACTPSD = dslhal_support_byteSwap32(StatisticsDef.usACTPSD);
      ptidsl->AppData.dsHLINSC = dslhal_support_byteSwap32(StatisticsDef.dsHLINSC);
//  UR8_MERGE_END   CQ10979*
//  UR8_MERGE_START CQ10978   Jack Zhang
      ptidsl->AppData.pwrStatus = StatisticsDef.pwrStatus;
//  UR8_MERGE_END   CQ10978*

      dspOamSharedInterface.atmStats_p = (DEV_HOST_atmStats_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.atmStats_p);

      rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.atmStats_p,&atmStats, sizeof(DEV_HOST_atmStats_t));

      if (rc)
      {
        dgprintf(1,"dslhal_support_blockRead failed\n");
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
        dgprintf(1,"dslhal_support_blockRead failed\n");
        return;
      }
      ptidsl->AppData.usSuperFrmCnt   = dslhal_support_byteSwap32(SuperFrameCnt.wSuperFrameCntUstrm);
      ptidsl->AppData.dsSuperFrmCnt   = dslhal_support_byteSwap32(SuperFrameCnt.wSuperFrameCntDstrm);

      /* Determine Frame Mode and Max Frame Mode */

      dspOamSharedInterface.atucMsg_p = (DEV_HOST_msg_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.atucMsg_p);

      rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.atucMsg_p,&atuc_msg, sizeof(DEV_HOST_msg_t));

      if (rc)
      {
        dgprintf(1,"dslhal_support_blockRead failed\n");
        return;
      }

      ptidsl->AppData.FrmMode         = (unsigned int)atuc_msg.framingMode;
      ptidsl->AppData.MaxFrmMode      = (unsigned int)atuc_msg.maxFrameMode;

      /* Determine Gross Gain */

      dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);

      rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,&aturMsg, sizeof(DEV_HOST_msg_t));

      if (rc)
      {
        dgprintf(1,"dslhal_support_blockRead failed\n");
        return;
      }
      ptidsl->AppData.grossGain = (unsigned int)aturMsg.grossGain;
      // UR8_MERGE_START CQ11057 KCCHEN
        /* Determine DS Line Attenuation & Margin */
        dspOamSharedInterface.eocVar_p = (DEV_HOST_eocVarDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.eocVar_p);

        rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.eocVar_p,&eocVar, sizeof(DEV_HOST_eocVarDef_t));

        if (rc)
        {
          dgprintf(1,"dslhal_support_blockRead failed\n");
          return;
        }
      // UR8_MERGE_END CQ11057 KCCHEN
      // UR8_MERGE_START CQ11247_TR69_DS_LATN_SATN  YW
      ptidsl->AppData.dsLineAttn      = (unsigned int)eocVar.lineAtten;
      ptidsl->AppData.dsLineAttn = dslhal_support_byteSwap16(ptidsl->AppData.dsLineAttn);

      if (dslhal_api_getHighPrecision() == 0)
      {
        ptidsl->AppData.dsLineAttn /= 5; // 0.5dB step
      }      
      // UR8_MERGE_END CQ11247_TR69_DS_LATN_SATN  YW
//  UR8_MERGE_START   CQ11228 HZ
      dsMargintmp = (SINT16)dslhal_support_byteSwap16(eocVar.dsMargin);
      if(dslhal_api_getHighPrecision())  //By default we use low precision for backward compt.
      {
        ptidsl->AppData.dsMargin        = (signed int)dsMargintmp;
      }else
      {
        ptidsl->AppData.dsMargin        = (((signed int)dsMargintmp)*13107 + 0x8000) >> 16; // divide 5 to get 0.5 dB units
      }
//  UR8_MERGE_END   CQ11228 HZ      
    }
  }

#if 1 // LOF  CQ10226
  dspOamSharedInterface.mgmt_Count_p = (DEV_HOST_MGMTCount_t *)
            dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.mgmt_Count_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.mgmt_Count_p,
            &mgmt_Count, sizeof(DEV_HOST_MGMTCount_t));
  if (!rc)
  {
    mgmt_Count.bis_Mgmt_Count_DS_p = (BIS_MGMT_CountersDef_t *)
                    dslhal_support_byteSwap32((unsigned int)mgmt_Count.bis_Mgmt_Count_DS_p);
    rc = dslhal_support_blockRead((PVOID)mgmt_Count.bis_Mgmt_Count_DS_p,
                    &BIS_MGMT_Counters, sizeof(BIS_MGMT_CountersDef_t));
    if (!rc)
    {
      ptidsl->AppData.erroredSeconds = dslhal_support_byteSwap32((unsigned int)BIS_MGMT_Counters.errored_seconds);
      ptidsl->AppData.severelyerrsecs = dslhal_support_byteSwap32((unsigned int)BIS_MGMT_Counters.severly_errored_seconds); //UR8_MERGE_END Report_SES Manjula K
//      printf("erroredSeconds: %X, errored_seconds: %X \n", ptidsl->AppData.erroredSeconds, BIS_MGMT_Counters.errored_seconds);
//      printf("severelyerrsecs: %X, severly_errored_seconds: %X \n", ptidsl->AppData.severelyerrsecs, BIS_MGMT_Counters.severly_errored_seconds); //UR8_MERGE_END Report_SES Manjula K
//  UR8_MERGE_START CQ10979   Jack Zhang
      ptidsl->AppData.totalInitErrs    = dslhal_support_byteSwap32((unsigned int)mgmt_Count.totalInitErrs); 
      ptidsl->AppData.totalInitTOs     = dslhal_support_byteSwap32((unsigned int)mgmt_Count.totalInitTOs); 
      ptidsl->AppData.showtimeInitErrs = dslhal_support_byteSwap32((unsigned int)mgmt_Count.showtimeInitErrs); 
      ptidsl->AppData.showtimeInitTOs  = dslhal_support_byteSwap32((unsigned int)mgmt_Count.showtimeInitTOs); 
      ptidsl->AppData.lastshowInitErrs = dslhal_support_byteSwap32((unsigned int)mgmt_Count.lastshowInitErrs); 
      ptidsl->AppData.lastshowInitTOs  = dslhal_support_byteSwap32((unsigned int)mgmt_Count.lastshowInitTOs); 
//  UR8_MERGE_END   CQ10979*
    }
    else
    {
      dgprintf(1,"dslhal_support_blockRead failed\n");
      return;
    }
  }
  else
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return;
  }
#endif

  /* Read in the Shared Tables structure */
  dspOamSharedInterface.dspWrSharedTables_p = (DEV_HOST_dspWrSharedTables_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspWrSharedTables_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspWrSharedTables_p,&sharedTables, sizeof(DEV_HOST_dspWrSharedTables_t));

  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return;
  }
  /* Read the ATU-R Bits and Gains Table */
  sharedTables.aturBng_p = (unsigned char *)dslhal_support_byteSwap32((unsigned int)sharedTables.aturBng_p);
  rc = dslhal_support_blockRead((PVOID)sharedTables.aturBng_p,ptidsl->AppData.rBng,
      ptidsl->AppData.max_ds_tones*sizeof(unsigned short));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return;
  }
  /* Read the ATU-C Bits and Gains Table */
  sharedTables.atucBng_p = (unsigned char *)dslhal_support_byteSwap32((unsigned int)sharedTables.atucBng_p);
  if(ptidsl->netService == 2) /* for Annex_B */
    rc = dslhal_support_blockRead((PVOID)sharedTables.atucBng_p,ptidsl->AppData.cBng,126*sizeof(unsigned char));
  else
    rc = dslhal_support_blockRead((PVOID)sharedTables.atucBng_p,ptidsl->AppData.cBng,62*sizeof(unsigned char));
  if (rc)
    {
      dgprintf(1,"dslhal_support_blockRead failed\n");
      return;
    }

  /* U/S Power Computation */
  dspOamSharedInterface.phyPerf_p = (DEV_HOST_phyPerf_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.phyPerf_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.phyPerf_p,
                                &phyPerf, sizeof(DEV_HOST_phyPerf_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return;
  }
  ptidsl->AppData.currentHybridNum = phyPerf.currentHybridNumUsed;

  /* ds bit allocation */
  sharedTables.bitAllocTblDstrm_p = (unsigned char *)dslhal_support_byteSwap32((unsigned int)sharedTables.bitAllocTblDstrm_p);
  rc = dslhal_support_blockRead((PVOID)sharedTables.bitAllocTblDstrm_p,ptidsl->AppData.BitAllocTblDstrm,
      ptidsl->AppData.max_ds_tones*sizeof(unsigned char));
  if(rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed \n");
    return;
  }

  /* us bit allocation */
  sharedTables.bitAllocTblUstrm_p = (unsigned char *)dslhal_support_byteSwap32((unsigned int)sharedTables.bitAllocTblUstrm_p);
//UR8_MERGE_START CQ10499   Jack Zhang
  rc = dslhal_support_blockRead((PVOID)sharedTables.bitAllocTblUstrm_p,ptidsl->AppData.BitAllocTblUstrm, 
                                       ptidsl->AppData.max_us_tones*sizeof(unsigned char));
//UR8_MERGE_END CQ10499   Jack Zhang
  if(rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed \n");
    return;
  }

  /* margin per tone */
  sharedTables.marginTblDstrm_p = (signed char *)dslhal_support_byteSwap32((unsigned int)sharedTables.marginTblDstrm_p);
  rc = dslhal_support_blockRead((PVOID)sharedTables.marginTblDstrm_p,ptidsl->AppData.marginTblDstrm,
      ptidsl->AppData.max_ds_tones*sizeof(signed char));
  if(rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed \n");
    return;
  }
  /* Read Configured Options */
  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.oamWriteNegoParams_p,
                                &configParms, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return;
  }
  else
  {
    /* r-Msg1 */
  // UR8_MERGE_START CQ10481   Jack Zhang
    ptidsl->AppData.StdMode = (unsigned int)( configParms.stdMode +
                                              (configParms.stdMode_byte2<<8));
  // UR8_MERGE_END CQ10481   Jack Zhang

    /* Translate only if we are using bit feild mode. */
    if(!ptidsl->AppData.useBitField)
    {
        if(configParms.stdMode == T1413_MODE)
            ptidsl->AppData.StdMode = DSLTRAIN_T1413_MODE;
        else if(configParms.stdMode == GDMT_MODE)
            ptidsl->AppData.StdMode = DSLTRAIN_GDMT_MODE;
        else if(configParms.stdMode == MULTI_MODE8)
            ptidsl->AppData.StdMode = DSLTRAIN_MULTI_MODE;
    }

    shim_osMoveMemory((void *)ptidsl->AppData.bRMsgs1, (void *)configParms.rMsgs1, 6*sizeof(char));
    if((ptidsl->AppData.bRMsgs1[2] & 0x02) == 0x02)
    {
      dgprintf(7,"Trellis!\n");
      ptidsl->configFlag |= CONFIG_FLAG_TRELLIS;
    }
    else
      ptidsl->configFlag &= ~CONFIG_FLAG_TRELLIS;
    if(ptidsl->AppData.bRMsgs1[2]&0x01)
      ptidsl->configFlag |= CONFIG_FLAG_EC;
    else
      ptidsl->configFlag &= ~CONFIG_FLAG_EC;

#if 1 // TR69
    // extract t1413ATUCVendorId
    if ((!ptidsl->AppData.useBitField &&
          ptidsl->AppData.TrainedMode == DSLTRAIN_T1413_MODE)
      || (ptidsl->AppData.useBitField &&
          ptidsl->AppData.TrainedMode == T1413_ANSI))
    {
      // Trained Mode is T1413 mode
      ptidsl->AppData.t1413ATUC.VendorId[0] = (ptidsl->AppData.bCMsgs1[3]>>4)
        | (ptidsl->AppData.bCMsgs1[4]<<4);

      ptidsl->AppData.t1413ATUC.VendorId[1] = (ptidsl->AppData.bCMsgs1[5]<<4)
        | (ptidsl->AppData.bCMsgs1[4]>>4);

      ptidsl->AppData.t1413ATUC.t1413Revision = ((ptidsl->AppData.bCMsgs1[3] & 0x03) << 1)
        | (ptidsl->AppData.bCMsgs1[2]>> 7);

      ptidsl->AppData.t1413ATUC.VendorRevision = (ptidsl->AppData.bCMsgs1[2]>>2) & 0x1F;

      ptidsl->AppData.t1413ATUR.t1413Revision = ((ptidsl->AppData.bRMsgs1[3] & 0x03) << 1)
        | (ptidsl->AppData.bCMsgs1[2]>> 7);

      ptidsl->AppData.t1413ATUR.VendorRevision = (ptidsl->AppData.bRMsgs1[2]>>2) & 0x1F;

      // extract t1413ATURVendorId
      shim_osMoveMemory((void*)ptidsl->AppData.t1413ATUR.VendorId,(void *)configParms.t1413VendorId, 2);

      shim_osZeroMemory((char *)ptidsl->AppData.ghsATUCVendorId, 8);
      shim_osZeroMemory((char *)ptidsl->AppData.ghsATURVendorId, 8);
    }
    else
    {
      // extract ghsATUCVendorId
      ghsATUCVendorIdAddr=dslhal_support_readInternalOffset(ptidsl, 2, offset);
      dslhal_support_blockRead((void*)ghsATUCVendorIdAddr, &ptidsl->AppData.ghsATUCVendorId, 8);

      // extract ghsATURVendorId
      shim_osMoveMemory((void*)ptidsl->AppData.ghsATURVendorId,(void *)configParms.gdmtVendorId, 8);

      shim_osZeroMemory((char *)&ptidsl->AppData.t1413ATUC, sizeof(T1413INFO));
      shim_osZeroMemory((char *)&ptidsl->AppData.t1413ATUR, sizeof(T1413INFO));
    }

#endif
  }

  if (ptidsl->AppData.bState == RSTATE_SHOWTIME)
  { // Don't call getAdvStat before showtime, it will hang
    rc = dslhal_api_getAdvancedStats(ptidsl);
  }
  return;
}

#ifdef NOT_DRV_BUILD
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
  dslhal_api_gatherStatistics(ptidsl);
  return;
}
#endif

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
  dgprintf(5," dslhal_api_setTrainingMode()\n");
  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,
                                sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&NegoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
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
  dgprintf(5," dslhal_api_disableLosAlarm() Done\n");
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

  dgprintf(5," dslhal_ctrl_setThreshold()\n");

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);


  rc = dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&NegoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  NegoPara.marginThreshold = threshold;

  rc = dslhal_support_blockWrite(&NegoPara,dspOamSharedInterface.oamWriteNegoParams_p, sizeof(DEV_HOST_oamWrNegoParaDef_t));

  if(rc)
    return DSLHAL_ERROR_MARGIN_API_FAILURE;

  dgprintf(5," dslhal_api_setThreshold() Done\n");
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

  dgprintf(5," dslhal_ctrl_setMonitorFlags()\n");

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);


  rc = dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&NegoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
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
  dgprintf(5," dslhal_api_setMonitorFlags() Done\n");
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
  dgprintf(5," dslhal_api_setEocSerialNumber()\n");

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.eocVar_p = (DEV_HOST_eocVarDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.eocVar_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.eocVar_p,
                                &eocVar, sizeof(DEV_HOST_eocVarDef_t));

  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  shim_osMoveMemory(eocVar.serialNumber,SerialNumber,32);

  rc= dslhal_support_blockWrite(&eocVar,dspOamSharedInterface.eocVar_p,sizeof(DEV_HOST_eocVarDef_t));
  if(rc)
    return DSLHAL_ERROR_EOCREG_API_FAILURE;
  dgprintf(5," dslhal_api_setEocSerialNumber() Done\n");

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
  dgprintf(5," dslhal_api_setEocVendorId()\n");

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);


  rc = dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&NegoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  shim_osMoveMemory(NegoPara.gdmtVendorId,VendorID,8);
  rc= dslhal_support_blockWrite(&NegoPara,dspOamSharedInterface.oamWriteNegoParams_p, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if(rc)
    return DSLHAL_ERROR_EOCREG_API_FAILURE;

  dgprintf(5," dslhal_api_setEocVendorId() Done\n");
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
  int count=0;  //CQ10037
  char *temp = RevNumber; //CQ10037
  
  dgprintf(5," dslhal_api_setEocRevisionNumber()\n");

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.eocVar_p = (DEV_HOST_eocVarDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.eocVar_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.eocVar_p,
                                &eocVar, sizeof(DEV_HOST_eocVarDef_t));

  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }
  
  //CQ10037: If length of RevNumber is 16 bytes , then it is used to set version number in ADSL2, ADSL2+ modes
  // else it is used to set revision number in G.992.1 mode.
  
  while (*temp++ != '\0') count++;                    
  
   if (count==16)
  {
    shim_osMoveMemory(eocVar.revNumber_2p,RevNumber,16);    
  }
  else {
    shim_osMoveMemory(eocVar.revNumber,RevNumber,4);   
  }
    
  rc=dslhal_support_blockWrite(&eocVar,dspOamSharedInterface.eocVar_p,sizeof(DEV_HOST_eocVarDef_t));
  if(rc)
    return DSLHAL_ERROR_EOCREG_API_FAILURE;
  dgprintf(5," dslhal_api_setEocRevisionNumber Done\n");
  return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_boostDspFrequency(void)
 *
 *******************************************************************************************
 * DESCRIPTION: This fuction will boost DSP frequency to 250MHz
 *
 * INPUT:  none
 *
 * RETURN: 0 SUCCESS
 *         1 FAILED
 *
 *****************************************************************************************/
unsigned int dslhal_api_boostDspFrequency(void)
{
  unsigned int rc;
  unsigned int chipId;
  unsigned int buck_trim_read;

  rc =dslhal_support_hostDspAddressTranslate((unsigned int)0x02040010);
  if(rc== DSLHAL_ERROR_ADDRESS_TRANSLATE)
  {
     dgprintf(1, "dslhal_support_hostDspAddressTranslate failed\n");
     return DSLHAL_ERROR_ADDRESS_TRANSLATE;
  }

  chipId= DSLHAL_REG32(REG_CVR) & 0xFFFF;
  switch (chipId) 
  {
  case CHIP_AR7:  /* Sangam */
    dgprintf(4, "Sangam clock boost 250\n");
    /* Put ADSLSS in to reset */
    DSLHAL_REG32(REG_VSERCLKSELR) = 0x1;
    shim_osClockWait(64);
    dgprintf(4,"REG_VSERCLKSELR<-0x01\n");

    DSLHAL_REG32(REG_MIPS_PLLM) = 0x4;
    shim_osClockWait(64);
    dgprintf(4,"Enable Analog PLL \n");

    DSLHAL_REG32(REG_MIPS_PLLM) = 0x97fe;
    shim_osClockWait(64);

    triggerDsp250MHZ = 1;

    break;

  case CHIP_AR7O_250_212:  /* Ohio250/Ohio212(new) */
  // Could be 212 or 250,

    /* read buck trim bits and save for later use */
    DSLHAL_REG32(REG_PRCR) &= ~(PRCR_DSP|PRCR_SAR);
    shim_osClockWait(64);
    DSLHAL_REG32(REG_PRCR) |=  (PRCR_ADSLSS); /* reset ADSLSS */
    shim_osClockWait(64);
    DSLHAL_REG32(REG_ADSLSS_PRCR) |= ADSLSS_PRCR_DSL_SPA;   /* reset ADSLSS DSL_SPA */
    shim_osClockWait(256);

    // Now both 7100&7200 dsp run at 250Mhz.
    // TB - Mask out the byte of interest
    buck_trim_read= (DSLHAL_REG32(REG_BUCKTRIM_READ) >>8)&0xff;
    //
    // If buck trim is '111', then it's Ohio250, boot dsp clock to
    // 250Mhz, otherwise it's Ohio212, don't boost
    // 111 or 110 --> 250Mhz.
    if((buck_trim_read != 0xFF) && ((buck_trim_read==0x07) || (buck_trim_read==0x06)))
    {
      int retry_attmpts;
      volatile unsigned int *dsp_pmem_ptr = (volatile unsigned int *)DSP_PMEM_START_ADDR;
      int pattern = 0xABCDABCD;

      // boost dsp clock to 250Mhz
      dgprintf(4, "Ohio clock boost 250\n");

      triggerDsp250MHZ = 1;
      
      for(retry_attmpts = 0; retry_attmpts <= PWR_DN_RETRYS; retry_attmpts++)
      {
          /* Ensure that the peripherals in the ADSLSS are in reset */
          DSLHAL_REG32(REG_PRCR) &= ~(PRCR_DSP|PRCR_SAR|PRCR_ADSLSS_ONLY);
          shim_osClockWait(64);

          /* Disabling the system PLL dividers */
          DSLHAL_REG32(REG_SYSTEM_PLLCSR) &= ~PLLEN;
          shim_osClockWait(640);
          dgprintf(4,"REG_SYSTEM_PLLCSR<-0x00\n");

          /* Disabling the MIPS PLL dividers */
          DSLHAL_REG32(REG_MIPS_PLLCSR) &= ~PLLEN;
          shim_osClockWait(640);
          dgprintf(4,"REG_MIPS_PLLCSR<-0x00\n");

          /* Disable (gate off) clock from MIPS PLL to ADSLSS */
          DSLHAL_REG32(REG_SYSTEM_POSTDIV2) = 0;
          shim_osClockWait(640);
          dgprintf(4,"REG_SYSTEM_POSTDIV2<-0x00\n");

        /* AV:
              Added a check for this should be added for the code below for 1:1 mode
              to ensure that the MIPS PLL has been configured generate the 212MHz
              output for the PHY.
        */
          if(DSLHAL_REG32(REG_MIPS_PLLM) != OHIO_MIPS_PLLM_RATIO)
          {
              DSLHAL_REG32(REG_MIPS_PLLM) =  OHIO_MIPS_PLLM_RATIO;      // programming the multiplier.
              shim_osClockWait(64);
              dgprintf(4,"REG_MIPS_PLLM<-0x05\n");
          }

          /* CQ9802: Power down the peripherals in the ADSLSS */
          DSLHAL_REG32(REG_PDCR) |= (PDCR_SARP | PDCR_ADSLP | PDCR_ADSPP);
          shim_osClockWait(640);

          /* Enable the 250 mode */
          DSLHAL_REG32(REG_VSERCLKSELR) |= OHIO_250_MODE;
          shim_osClockWait(640);
          dgprintf(4,"REG_VSERCLKSELR<-0x100\n");

          /* CQ9802: Power up the peripherals in the ADSLSS */
          DSLHAL_REG32(REG_PDCR) &= ~(PDCR_SARP | PDCR_ADSLP | PDCR_ADSPP);
          shim_osClockWait(640);

          /* Re-enable the MIPS PLL */
          DSLHAL_REG32(REG_MIPS_PLLCSR) |= PLLEN;
          shim_osClockWait(640);
          dgprintf(4,"REG_MIPS_PLLCSR<-0x01\n");

          DSLHAL_REG32(REG_SYSTEM_PLLM) = OHIO_SYS_PLLM_RATIO ;  // programming the multiplier.
          shim_osClockWait(640);
          dgprintf(4,"REG_SYSTEM_PLLM<-0x09\n");

          DSLHAL_REG32(REG_SYSTEM_POSTDIV2) = DIVEN | OHIO_SYS_POSTDIV2_RATIO;  // programming the post divider
          shim_osClockWait(640);
          dgprintf(4,"REG_SYSTEM_POSTDIV2<-0x8000\n");

          DSLHAL_REG32(REG_SYSTEM_PLLCMDEN) |= GOSETEN2; // enabling the post division.
          shim_osClockWait(640);
          dgprintf(4,"REG_SYSTEM_PLLCMDEN<-0x02\n");

          while ((DSLHAL_REG32(REG_SYSTEM_PLLSTAT) & 0x1) != 0)   // wait till we get the ready.
          {
            shim_osClockWait(0x64);
          }

          DSLHAL_REG32(REG_SYSTEM_PLLCMD) |= GOSET2; // this write actually cause the division.
          shim_osClockWait(640);
          dgprintf(4,"REG_SYSTEM_PLLCMD<-0x01\n");


          DSLHAL_REG32(REG_SYSTEM_PLLCSR) |= PLLEN ;  // enabling the pll clock.
          shim_osClockWait(640);
          dgprintf(4,"REG_SYSTEM_PLLCSR<-0x01\n");


          /* AV: Added test for making sure that we come out of reset correctly */
          /* Reset ADSLSS so we can read and write the PMEM */
          DSLHAL_REG32(REG_PRCR) |=  (PRCR_ADSLSS);
          shim_osClockWait(640);

          /* Ensure ADSLADR2 maps to the start of DSP PMEM */
          DSLHAL_REG32(ADSLADR2) = 0;

          /* Write a pattern to DSP PMEM */
          *dsp_pmem_ptr = (volatile unsigned int)dslhal_support_byteSwap32(pattern);

          /* Delay to read back */
          shim_osClockWait(640);

          /* Check 1st instruction. If 0, we have encountered the problem. */
          if( *dsp_pmem_ptr == (volatile unsigned int)dslhal_support_byteSwap32(pattern) )
              break;

          dgprintf(4,"Retrying to read the DSP's memory.\n");

          shim_osClockWait(640);
      }//end retry for loop.

      if(retry_attmpts > PWR_DN_RETRYS)
          return DSLHAL_ERROR_RESET_ADSLSS;
    }
    else
    {   
        /* We cannot boost the frequency. */
        return(DSLHAL_ERROR_GENERAL_FAILURE);
    }

    break;

  }

  return(DSLHAL_ERROR_NO_ERRORS);
}

#ifdef NOT_DRV_BUILD
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
  dgprintf(5," dslhal_api_setAturConfig()\n");

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.eocVar_p = (DEV_HOST_eocVarDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.eocVar_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.eocVar_p,
                                &eocVar, sizeof(DEV_HOST_eocVarDef_t));

  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }
  shim_osMoveMemory(eocVar.aturConfig,ATURConfig,30);
  rc= dslhal_support_blockWrite(&eocVar,dspOamSharedInterface.eocVar_p,sizeof(DEV_HOST_eocVarDef_t));
  if(rc)
    return DSLHAL_ERROR_EOCREG_API_FAILURE;
  dgprintf(5," dslhal_api_setAturConfig() Done\n");
  return DSLHAL_ERROR_NO_ERRORS;
}

#endif

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
  dgprintf(5," dslhal_api_setRateAdaptFlag()\n");

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,
                                &aturMsg, sizeof(DEV_HOST_msg_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  if(flag)
    aturMsg.rateAdapt = TRUE;
  else
    aturMsg.rateAdapt = FALSE;

  rc= dslhal_support_blockWrite(&aturMsg,dspOamSharedInterface.aturMsg_p,sizeof(DEV_HOST_msg_t));
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  dgprintf(5," dslhal_api_setRateAdaptFlag() Done\n");
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
  dgprintf(5," dslhal_api_setTrellisFlag()\n");
  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);
  rc += dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,&aturMsg, sizeof(DEV_HOST_msg_t));
  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);
  rc += dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&negoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
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

  dgprintf(5," dslhal_api_setTrellisFlag() Done\n");
  return DSLHAL_ERROR_NO_ERRORS;
}

#ifdef NOT_DRV_BUILD
/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setMaxBitsPerCarrierDownstream(tidsl_t *ptidsl,unsigned int maxbits)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Maximum bits per carrier value in downstream band
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int maxbits : should be a value between 0-15
*
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_api_setMaxBitsPerCarrierDownstream(tidsl_t *ptidsl,unsigned int maxbits)
{

  DEV_HOST_msg_t atucMsg;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  int rc;

  dgprintf(5," dslhal_api_setMaxBitsPerCarrierDownstream()\n");
  if(maxbits>15)
  {
    dgprintf(3,"Maximum Number of Bits per carrier cannot exceed 15!\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.atucMsg_p = (DEV_HOST_msg_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.atucMsg_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.atucMsg_p,
                                &atucMsg, sizeof(DEV_HOST_msg_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  atucMsg.maxBits = maxbits;

  rc=dslhal_support_blockWrite(&atucMsg,dspOamSharedInterface.atucMsg_p,sizeof(DEV_HOST_msg_t));
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  dgprintf(5," dslhal_api_setMaxBitsPerCarrierDownstream() Done\n");
  return DSLHAL_ERROR_NO_ERRORS;
}

#endif

#ifdef NOT_DRV_BUILD
/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setMaxBitsPerCarrier(tidsl_t *ptidsl,unsigned int maxbits)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Maximum bits per carrier value in Upstream band
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
  return dslhal_api_setMaxBitsPerCarrierUpstream (ptidsl, maxbits);
}
#endif

/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setMaxBitsPerCarrierUpstream(tidsl_t *ptidsl,unsigned int maxbits)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Maximum bits per carrier value in Upstream band
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int maxbits : should be a value between 0-15
*
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_api_setMaxBitsPerCarrierUpstream(tidsl_t *ptidsl,unsigned int maxbits)
{

  DEV_HOST_msg_t aturMsg;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  int rc;

  dgprintf(5," dslhal_api_setMaxBitsPerCarrierUpstream()\n");
  if(maxbits>15)
  {
    dgprintf(3,"Maximum Number of Bits per carrier cannot exceed 15!\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,
                                &aturMsg, sizeof(DEV_HOST_msg_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  aturMsg.maxBits = maxbits;

  rc=dslhal_support_blockWrite(&aturMsg,dspOamSharedInterface.aturMsg_p,sizeof(DEV_HOST_msg_t));
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  dgprintf(5," dslhal_api_setMaxBitsPerCarrierUpstream() Done\n");
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
  dgprintf(5," dslhal_api_setMaxInterleaverDepth()\n");
  if(maxdepth>3)
  {
    dgprintf(3,"Invalid Value for maximum interleave depth (must be <3)\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,
                                &aturMsg, sizeof(DEV_HOST_msg_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  aturMsg.maxIntlvDepth = maxdepth;

  rc=dslhal_support_blockWrite(&aturMsg,dspOamSharedInterface.aturMsg_p,sizeof(DEV_HOST_msg_t));
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
  dgprintf(5," dslhal_api_setMaxInterleaverDepth() Done\n");
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
    dgprintf(5,"Mailbox Interrupt \n");
  }
  if(DSLHAL_REG32(dslhal_support_hostDspAddressTranslate(DSP_INTERRUPT_SOURCE_REGISTER))&MASK_BITFIELD_INTERRUPTS)
  {
    DSLHAL_REG32(dslhal_support_hostDspAddressTranslate(DSP_INTERRUPT_CLEAR_REGISTER))|=MASK_BITFIELD_INTERRUPTS;
    dgprintf(5,"Bitfield Interrupt \n");
  }
  if(DSLHAL_REG32(dslhal_support_hostDspAddressTranslate(DSP_INTERRUPT_SOURCE_REGISTER))&MASK_HEARTBEAT_INTERRUPTS)
  {
    DSLHAL_REG32(dslhal_support_hostDspAddressTranslate(DSP_INTERRUPT_CLEAR_REGISTER))|=MASK_HEARTBEAT_INTERRUPTS;
    dgprintf(5,"HeartBeat Interrupt \n");
  }
  interruptSources = dslhal_support_parseInterruptSource(ptidsl);
  if(interruptSources < 0)
    return DSLHAL_ERROR_INTERRUPT_FAILURE;
  else
    return interruptSources;
}

#ifdef NOT_DRV_BUILD
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
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.phyPerf_p = (DEV_HOST_phyPerf_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.phyPerf_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.phyPerf_p,
                                &phyPerf, sizeof(DEV_HOST_phyPerf_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
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
#endif

#ifdef NOT_DRV_BUILD
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
    dgprintf(3,"Invalid Value for Hybrid Number \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.phyPerf_p = (DEV_HOST_phyPerf_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.phyPerf_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.phyPerf_p,
                                &phyPerf, sizeof(DEV_HOST_phyPerf_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }
  phyPerf.hostSelectHybridNum = hybridNum;
  rc=dslhal_support_blockWrite(&phyPerf,dspOamSharedInterface.phyPerf_p,sizeof(DEV_HOST_phyPerf_t));
  if(rc)
    return DSLHAL_ERROR_HYBRID_API_FAILURE;
  else
    return DSLHAL_ERROR_NO_ERRORS;
}

#endif

#ifdef NOT_DRV_BUILD
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
    dgprintf(4,"Hybrid Metrics Not Yet Available \n");
  }
  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return (0-DSLHAL_ERROR_INVALID_PARAM);
  }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return (0-DSLHAL_ERROR_BLOCK_READ);
  }

  dspOamSharedInterface.phyPerf_p = (DEV_HOST_phyPerf_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.phyPerf_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.phyPerf_p,
                                &phyPerf, sizeof(DEV_HOST_phyPerf_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return (0-DSLHAL_ERROR_BLOCK_READ);
  }
  rc = sizeof(phyPerf.hybridCost);
  for(i=0;i<(rc/4);i++)
  {
    metric[i] = dslhal_support_byteSwap32(phyPerf.hybridCost[i]);
  }
  return hybrid_selected;
}
#endif

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

  dgprintf(5, "dslhal_api_sendQuiet\n");
  rc = dslhal_support_writeHostMailbox(ptidsl, HOST_RJ11SELECT, (unsigned int)pairSelect, 0, 0);
  if(rc)
  {
    dgprintf(1,"dslhal_api_sendQuiet failed\n");
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
  dgprintf(5, "dslhal_api_resetTrainFailureLog \n");
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
    dgprintf(3,"Invalid input parameter \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.modemEnvPublic_p = (DEV_HOST_modemEnvPublic_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemEnvPublic_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemEnvPublic_p,
                                &modemEnv, sizeof(DEV_HOST_modemEnvPublic_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
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
    modemEnv.overrideDslActLed_f = TRUE;
    if(onOff!=2)
      modemEnv.dslActLedState_f = onOff;
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

#ifdef NOT_DRV_BUILD
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
    dgprintf(3,"Invalid input parameter \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.modemEnvPublic_p = (DEV_HOST_modemEnvPublic_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemEnvPublic_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemEnvPublic_p,
                                &modemEnv, sizeof(DEV_HOST_modemEnvPublic_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
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

#endif

#ifdef NOT_DRV_BUILD
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
    dgprintf(3,"Invalid input parameter \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.atmDsBert_p = (DEV_HOST_atmDsBert_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.atmDsBert_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.atmDsBert_p,
                                &atmDsBert, sizeof(DEV_HOST_atmDsBert_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
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

#endif

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
    dgprintf(3,"Invalid input parameter \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  if(!ptidsl)
  {
    dgprintf(3, "Error: PTIDSL pointer invalid\n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
  }

  dspOamSharedInterface.modemEnvPublic_p = (DEV_HOST_modemEnvPublic_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemEnvPublic_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemEnvPublic_p,
                                &modemEnv, sizeof(DEV_HOST_modemEnvPublic_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
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

#ifdef NOT_DRV_BUILD
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
    dgprintf(3,"Invalid input parameter \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  rc += dslhal_support_blockRead((PVOID)(hostIfLoc+sizeof(int)*offset1), &structLoc,sizeof(int));
  structLoc = dslhal_support_byteSwap32(structLoc);
  rc += dslhal_support_blockRead((PVOID)(structLoc+sizeof(int)*offset2), &elementLoc,sizeof(int));
  elementLoc = dslhal_support_byteSwap32(elementLoc);
  dgprintf(3,"Host IF Location: 0x%x  Struct1 Location: 0x%x Element Location: 0x%x \n",hostIfLoc, structLoc, elementLoc);
  rc += dslhal_support_blockRead((PVOID)(elementLoc+(offset3*4)), localBuffer,numBytes);
  if (rc)
    return DSLHAL_ERROR_BLOCK_READ;
  return DSLHAL_ERROR_NO_ERRORS;
}

#endif

#ifdef NOT_DRV_BUILD
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
    dgprintf(3,"Invalid input parameter \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  rc += dslhal_support_blockRead((PVOID)(hostIfLoc+(offset1*4)), &structLoc,4);
  structLoc = dslhal_support_byteSwap32(structLoc);
  rc += dslhal_support_blockRead((PVOID)(structLoc+(offset2*4)), &elementLoc,4);
  elementLoc = dslhal_support_byteSwap32(elementLoc);
  dgprintf(3,"Host IF Location: 0x%x  Struct1 Location: 0x%x Element Location: 0x%x \n",hostIfLoc, structLoc, elementLoc);
  rc += dslhal_support_blockWrite(localBuffer,(PVOID)(elementLoc+(offset3*4)),numBytes);
  if (rc)
    return DSLHAL_ERROR_BLOCK_READ;
  return DSLHAL_ERROR_NO_ERRORS;
}

#endif

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
  currAddr = baseAddr;
  if(numBytes<=0 || !localBuffer || !ptidsl || !offsets)
  {
    dgprintf(3,"Invalid input parameter \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  for(off=0;off<numOffsets-1;off++)
  {
    rc += dslhal_support_blockRead((PVOID)(prevAddr+(4*offsets[off])), &currAddr,4);
    currAddr = dslhal_support_byteSwap32(currAddr);
    prevAddr = currAddr;
    dgprintf(5,"Curr Addr = 0x%x  Current Level: %d \n",currAddr,off);
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
  currAddr = baseAddr;
  if(numBytes<=0 || !localBuffer || !ptidsl || !offsets)
  {
    dgprintf(3,"Invalid input parameter \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }
  for(off=0;off<numOffsets-1;off++)
  {
    rc += dslhal_support_blockRead((PVOID)(prevAddr+(4*offsets[off])), &currAddr,4);
    currAddr = dslhal_support_byteSwap32(currAddr);
    prevAddr = currAddr;
    dgprintf(5,"Curr Addr = 0x%x  Current Level: %d \n",currAddr,off);
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

   dgprintf(5, "dslhal_api_sendMailboxCommand\n");
   rc = dslhal_support_writeHostMailbox(ptidsl, cmd, 0, 0, 0);
   if(rc)
   {
   dgprintf(1,"dslhal_api_sendMailboxCommand failed\n");
   return DSLHAL_ERROR_CTRL_API_FAILURE;
   }
   return DSLHAL_ERROR_NO_ERRORS;
   } */

// * UR8_MERGE_START CQ10880   Jack Zhang
/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_sendL3Command(tidsl_t *ptidsl)
 *
 *******************************************************************************************
 * DESCRIPTION: This fuction sends a L3 command to the DSP via the mailbox message 
 *
 * INPUT:  PITIDSLHW_T *ptidsl
 *
 * RETURN: 0 SUCCESS
 *         1 FAILED
 *
 *****************************************************************************************/
unsigned int dslhal_api_sendL3Command(tidsl_t *ptidsl)
{
   int rc;

   dgprintf(5, "dslhal_api_sendL3Command\n");
   //printk("dslhal_api_sendDspMessage (%d, %d, %d, %d)\n", cmd, tag, p1, p2);
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_L3_MSG, 0, 0, 0);
   if(rc)
   {
      dgprintf(1,"dslhal_api_sendL3Command failed\n");
      return DSLHAL_ERROR_CTRL_API_FAILURE;
   }
   return DSLHAL_ERROR_NO_ERRORS;
}
// * UR8_MERGE_END   CQ10880*

/********************************************************************************************
* FUNCTION NAME: dslhal_api_getAdvancedStats()
*
*********************************************************************************************
* DESCRIPTION: Read statistical infromation from ax5 modem daugter card.
* Input: tidsl_t *ptidsl
*
* Note: This function assumed dslhal_api_gatherstatistics() already been invoked.
* Return: 0    success
*         1    failed
*
********************************************************************************************/
unsigned int dslhal_api_getAdvancedStats(tidsl_t * ptidsl)
{
#ifndef NO_ADV_STATS
  int rc,i;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  DEV_HOST_dspWrSharedTables_t  sharedTables;
  DEV_HOST_phyPerf_t            phyPerf;
  unsigned char                 usBits[64],dsBits[512], rMsgRa[12];
  int                           dsPowerCutBack, usPowerCutBack;
  short                         hLog,dsGains[512], tmp16;
  unsigned short                snrBits[16]={0,1072,1242,1650,1837,2092,2364,
                                             2611,2883,3121,3393,3640,3903,
                                             4150,4414,4660};
  int                           offset[2]={5,1}, codingGain=0;
  int                           usNumLoadedTones=0, dsNumLoadedTones=0;

  dgprintf(5, "dslhal_api_gatherStatistics\n");
//  max_ds_tones = dslhal_support_getMaxDsTones(ptidsl);
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,
                                sizeof(DEV_HOST_dspOamSharedInterface_t));

  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return rc;
  }

  /* Read in the Shared Tables structure */
  dspOamSharedInterface.dspWrSharedTables_p = (DEV_HOST_dspWrSharedTables_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspWrSharedTables_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspWrSharedTables_p,&sharedTables, sizeof(DEV_HOST_dspWrSharedTables_t));
  if (rc)
     return DSLHAL_ERROR_BLOCK_READ;

  /* Read the ATU-R Bits and Gains Table */
  sharedTables.aturBng_p = (unsigned char *)dslhal_support_byteSwap32((unsigned int)sharedTables.aturBng_p);
  rc = dslhal_support_blockRead((PVOID)sharedTables.aturBng_p,ptidsl->AppData.rBng,
    ptidsl->AppData.max_ds_tones*2*sizeof(unsigned char));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return rc;
  }

  shim_osZeroMemory(usBits, sizeof(usBits));
  shim_osZeroMemory(dsBits, sizeof(dsBits));
  shim_osZeroMemory((void *)dsGains, sizeof(dsGains));

  /* Read the ATU-C Bits and Gains Table */
  sharedTables.atucBng_p = (unsigned char *)dslhal_support_byteSwap32((unsigned int)sharedTables.atucBng_p);

#if 1 //cph, consolidate AnexA & B code
    rc = dslhal_support_blockRead((PVOID)sharedTables.atucBng_p,ptidsl->AppData.cBng,
        (ptidsl->AppData.max_us_tones-1)*2*sizeof(unsigned char));

    if (rc)
    {
      dgprintf(1,"dslhal_support_blockRead failed\n");
      return rc;
    }
    for(i=1;i<ptidsl->AppData.max_us_tones;i++)
    {
      usBits[i]=((ptidsl->AppData.cBng[(i-1)*2])&0xf);
      dgprintf(4,"Bit #%d : 0x%x\n",i,usBits[i]);
    }
    for(i=1;i<ptidsl->AppData.max_us_tones;i++)
    {
      if(usBits[i])
        usNumLoadedTones++;
    }

#else
  if(ptidsl->netService == 2) /* for Annex_B */
  {
    rc = dslhal_support_blockRead((PVOID)sharedTables.atucBng_p,ptidsl->AppData.cBng,126*sizeof(unsigned char));
    if (rc)
    {
      dgprintf(1,"dslhal_support_blockRead failed\n");
      return rc;
    }


//    for(i=0;i<US_BNG_LENGTH*2;i++)
//      usBits[i]=0;
//    for(i=1;i<US_BNG_LENGTH*2;i++)
    for(i=1;i<ptidsl->AppData.max_us_tones;i++)
    {
      usBits[i]=((ptidsl->AppData.cBng[(i-1)*2])&0xf);
      dgprintf(5,"Bit #%d : 0x%x\n",i,usBits[i]);
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
      dgprintf(1,"dslhal_support_blockRead failed\n");
      return rc;
    }
//    for(i=0;i<US_BNG_LENGTH;i++)
//      usBits[i]=0;
    for(i=1;i<US_BNG_LENGTH;i++)
    {
      usBits[i]=((ptidsl->AppData.cBng[(i-1)*2])&0xf);
      dgprintf(5,"Bit #%d : 0x%x\n",i,usBits[i]);
    }
    for(i=1;i<US_BNG_LENGTH;i++)
    {
      if(usBits[i])
        usNumLoadedTones++;
    }
  }
#endif
  /* Determine Number U/S of Loaded Tones */

  /* U/S Power Computation */
  dspOamSharedInterface.phyPerf_p = (DEV_HOST_phyPerf_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.phyPerf_p);
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.phyPerf_p,
                                &phyPerf, sizeof(DEV_HOST_phyPerf_t));
  if (rc)
  {
    dgprintf(1,"dslhal_support_blockRead failed\n");
    return rc;
  }
  phyPerf.usAvgGain = dslhal_support_byteSwap32(phyPerf.usAvgGain);
  if(ptidsl->AppData.dsl_modulation < ADSL2_MODE)
    usPowerCutBack = US_NOMINAL_POWER;
  else
    {
      usPowerCutBack = (((ptidsl->adsl2TrainingMessages.rMsgPcb[0] & 0xc0) >>6)+((ptidsl->adsl2TrainingMessages.rMsgPcb[1] & 0xf) <<2));
      rc = (((ptidsl->adsl2TrainingMessages.cMsgPcb[0] & 0xc0) >>6)+((ptidsl->adsl2TrainingMessages.cMsgPcb[1] & 0xf) <<2));
      if(usPowerCutBack < rc)
         usPowerCutBack = rc;
      usPowerCutBack =  US_NOMINAL_POWER - usPowerCutBack;
    }
  ptidsl->AppData.usTxPower = LOG43125 + phyPerf.usAvgGain + (256*usPowerCutBack)+log10[usNumLoadedTones-1];
  dgprintf(7,"Avg Gain: 0x%x  usNumLoadedTones: 0x%x  USPCB=%d  log: 0x%x\n",phyPerf.usAvgGain, usNumLoadedTones, usPowerCutBack, log10[usNumLoadedTones-1]);

  /* Determine Number D/S of Loaded Tones */
//use shim_osZeroMemory() & move up
//  dsBits[0]=0;
//  for(i=0;i<dsNumTones;i++)
//    {
//     dsBits[i]=0;
//     dsGains[i]=0;
//    }
  for(i=1;i<ptidsl->AppData.max_ds_tones;i++)
    {
     dsBits[i]=((ptidsl->AppData.rBng[(i-1)*2])&0xf);
     dsGains[i]= ((ptidsl->AppData.rBng[(2*i)-1]) <<4);
     dsGains[i]+= (((ptidsl->AppData.rBng[(i-1)*2])&0xf0) >>4);
     dgprintf(5,"Bit #%d : 0x%x  0x%x\n",i,ptidsl->AppData.rBng[i],dsGains[i]);
    }
  for(i=1;i<ptidsl->AppData.max_ds_tones;i++)
    {
     if(dsBits[i])
       dsNumLoadedTones++;
    }
  /* D/S Power Computation */
  /* D/S Power Cutback */
  if(ptidsl->AppData.dsl_modulation < ADSL2_MODE)
    dsPowerCutBack = 2*((((((ptidsl->AppData.bCMsgs1[0]) >>6) &0x3)+(((ptidsl->AppData.bCMsgs1[1]) &0x1) <<2))) - 1) - 52;
  else
    {
      dsPowerCutBack = (ptidsl->adsl2TrainingMessages.rMsgPcb[0] & 0x3f);
      rc = (ptidsl->adsl2TrainingMessages.cMsgPcb[0] & 0x3f);
      if(dsPowerCutBack < rc)
        dsPowerCutBack = rc;
      dsPowerCutBack = -40 - dsPowerCutBack;
    }
  phyPerf.dsAvgGain = dslhal_support_byteSwap32(phyPerf.dsAvgGain);
  ptidsl->AppData.dsTxPower = LOG43125 + phyPerf.dsAvgGain + (256*dsPowerCutBack) + log10[dsNumLoadedTones-1];
  dgprintf(7,"Avg Gain: %d  dsNumLoadedTones: %d  log: %d  pcb: %d \n",phyPerf.dsAvgGain, dsNumLoadedTones, log10[dsNumLoadedTones-1], dsPowerCutBack);

  /* Advanced Error Parameters */
  /* Downstream PSD Computation : from Powercutback obtained from C-Msg1 */
  /*  dsPsd = (2*(dsPowerCutBack - 1) - 52); */
  rc = dslhal_api_dspInterfaceRead(ptidsl,(unsigned int)ptidsl->pmainAddr,2,(unsigned int *)&offset, (unsigned char *)&rMsgRa,12);
  if(rc)
    return DSLHAL_ERROR_BLOCK_READ;
  else
    /* Derivation of Coding Gain from R-MSG-RA */
    codingGain = (((rMsgRa[2] & 0xe0) >> 5) + ((rMsgRa[3] & 0x1)<<3));

  dgprintf(7,"dsPowerCutBack:%d Cg:%d \n",dsPowerCutBack,codingGain/2);
  for(i=1;i<ptidsl->AppData.max_ds_tones;i++)
  {
    /* Compute G(i) = 20*log10(g(i)) computed in-place in the dsNoise global variable */
    /* log10 table already is 256 * 10log10 */
    /* dsGains is scaled by 512, so log of that term factored out */
    if(dsGains[i]==0)
      ptidsl->AppData.dsNoise[i] = 0;
    else
    {
//      ptidsl->AppData.dsNoise[i] = -2*(log10[((dsGains[i])/8)-1] - log10[63]);
      tmp16 = (dsGains[i]<8) ? 8: dsGains[i];
      ptidsl->AppData.dsNoise[i] = -2*(log10[(tmp16/8)-1] - log10[63]);
    }

    hLog = dslhal_staticapi_getHlog(ptidsl,i);
    /* Compute DS Noise in dB from all derived factors with appropriate scaling to get output in dB */
    ptidsl->AppData.dsNoise[i] = (256*hLog)/10 + 256*dsPowerCutBack - (256*snrBits[(dsBits[i])])/85 - 128*(ptidsl->AppData.marginTblDstrm[i] - codingGain);
    /* Debug Print to verify output */
    dgprintf(7, "i=%3d hLog: %3d snr: %3d M: %3d  No:%3d\n",
            i,hLog/10,(snrBits[(dsBits[i])]/85),ptidsl->AppData.marginTblDstrm[i]/2,ptidsl->AppData.dsNoise[i]/256);
  }
#endif
  return DSLHAL_ERROR_NO_ERRORS;
}
// UR8_MERGE_START CQ11057 KCCHEN
/********************************************************************************************
* FUNCTION NAME: dslhal_api_getUSPMDTest()
*
*********************************************************************************************
* DESCRIPTION:
*   Get US PMD Test parameters in ADSL2/2+ mode
*
* Input:  PITIDSLHW_T *ptidsl
*         unsigned short *outbuf: Output buffer supplied from caller.
*                 for ADSL2 mode, 1*256=256 bytes are expected.
*                 for ADSL2+ mode, 1*512=512 bytes are expected.
*         int flag: 0: training. (1: showtime).
*         **Note: Currently only showtime is supported.
*
* Return: DSLHAL_ERROR_NO_ERRORS  success
*         otherwise               failed
*
********************************************************************************************/
#ifndef NO_ADV_STATS
unsigned int dslhal_api_getPMDTestus(tidsl_t * ptidsl, CoPMDTestParams_t * co_pmdtest_params_p, int flag)
{
  unsigned int offset[2]={7, 20};
  unsigned int *USPMD= (unsigned int *)
    PHYS_TO_K1(dslhal_support_readInternalOffset(ptidsl, 2, offset));
  int i, num_swap32;
  unsigned int *outbufInt_p= (unsigned int *) co_pmdtest_params_p;
   
  dgprintf(5,"dslhal_api_getUSPMD\n");

  if (flag!=1)
    return DSLHAL_ERROR_INVALID_PARAM;

  // Must be ADSL2/2+ mode
  if (!(ptidsl->AppData.TrainedMode & (ADSL2PLUS_MASKS|ADSL2_MASKS)) )
    return DSLHAL_ERROR_UNSUPPORTED_MODE;

  num_swap32 = (sizeof(CoPMDTestParams_t))>>2;
  
  for (i=0; i< num_swap32; i++)
  {
    *outbufInt_p++= dslhal_support_byteSwap32(*USPMD++);
  }
  #ifdef EB
  for (i=0; i<ptidsl->AppData.max_us_tones; i++)
  {
    co_pmdtest_params_p->TestParmCOHlogfMsg[i] = dslhal_support_byteSwap16(co_pmdtest_params_p->TestParmCOHlogfMsg[i]);
  }
  co_pmdtest_params_p->co_latn = dslhal_support_byteSwap16(co_pmdtest_params_p->co_latn);
  co_pmdtest_params_p->co_satn = dslhal_support_byteSwap16(co_pmdtest_params_p->co_satn);
  co_pmdtest_params_p->usMargin = dslhal_support_byteSwap16(co_pmdtest_params_p->usMargin);
  co_pmdtest_params_p->co_attndr = dslhal_support_byteSwap32(co_pmdtest_params_p->co_attndr);
  co_pmdtest_params_p->co_near_actatp = dslhal_support_byteSwap16(co_pmdtest_params_p->co_near_actatp);
  co_pmdtest_params_p->co_far_actatp = dslhal_support_byteSwap16(co_pmdtest_params_p->co_far_actatp);
  #endif
  return DSLHAL_ERROR_NO_ERRORS;
}
#endif
// UR8_MERGE_END CQ11057 KCCHEN
/********************************************************************************************
* FUNCTION NAME: dslhal_api_getQLNpsds()
*
*********************************************************************************************
* DESCRIPTION:
*   Get downstream Quiet Line Noise (QLNpsds) in ADSL2/2+ mode, as defined in g.997.1.
*
* Input:  PITIDSLHW_T *ptidsl
*         unsigned short *outbuf: Output buffer supplied from caller.
*                 for ADSL2 mode, 1*256=256 bytes are expected.
*                 for ADSL2+ mode, 1*512=512 bytes are expected.
*         int flag: 0: training QLNpsds. (1: showtime QLNpsds).
*         **Note: Currently only training QLNpsds is supported.
*
* Return: DSLHAL_ERROR_NO_ERRORS  success
*         otherwise               failed
*
********************************************************************************************/
#ifndef NO_ADV_STATS
unsigned int dslhal_api_getQLNpsds(tidsl_t * ptidsl, unsigned char *outbuf, int flag)
{
  unsigned int offset[2]={7, 15};
  unsigned int *QLNpsds_p= (unsigned int *)
    PHYS_TO_K1(dslhal_support_readInternalOffset(ptidsl, 2, offset));
  int i, num_swap32;
  unsigned int *outbufInt_p= (unsigned int *) outbuf;

  dgprintf(5,"dslhal_api_getQLNpsds\n");

  if (flag!=0)
    return DSLHAL_ERROR_INVALID_PARAM;

  // Must be ADSL2/2+ mode
  if (!(ptidsl->AppData.TrainedMode & (ADSL2PLUS_MASKS|ADSL2_MASKS)) )
    return DSLHAL_ERROR_UNSUPPORTED_MODE;

  // currently only training HLOGpsds is supported
  // The HLOG internal buffer is in external SDRAM.
  num_swap32 = ptidsl->AppData.max_ds_tones>>2;
  for (i=0; i< num_swap32; i++)
  {
    *outbufInt_p++= dslhal_support_byteSwap32(*QLNpsds_p++);
  }

  return DSLHAL_ERROR_NO_ERRORS;
}
#endif

/********************************************************************************************
* FUNCTION NAME: dslhal_api_getSNRGpsds()
*
*********************************************************************************************
* DESCRIPTION:
*   Get downstream SNRpsds in ADSL2/2+ mode, as defined in g.997.1.
*
* Input:  PITIDSLHW_T *ptidsl
*         unsigned short *outbuf: Output buffer supplied from caller.
*                 for ADSL2 mode, 1*256=256 bytes are expected.
*                 for ADSL2+ mode, 1*512=512 bytes are expected.
*         int flag: (0: training SNRpsds). 1: showtime SNRpsds.
*         **Note: Currently only showtime SNRpsds is supported.
// UR8_MERGE_START  CQ10386 PeterHou
*         Note: To convert the result to dB, use the following formula (see g.997.1)
*         SNR_dB[i] = -32 + (outbuf[i]/2)
// UR8_MERGE_END CQ10386
*
* Return: DSLHAL_ERROR_NO_ERRORS  success
*         otherwise               failed
*
********************************************************************************************/
#ifndef NO_ADV_STATS
unsigned int dslhal_api_getSNRpsds(tidsl_t * ptidsl, unsigned char *outbuf, int flag)
{
  unsigned int offset[2]={7, 16};
  unsigned int *SNRpsds_p= (unsigned int *)
    PHYS_TO_K1(dslhal_support_readInternalOffset(ptidsl, 2, offset));
  int i, num_swap32;
  unsigned int *outbufInt_p= (unsigned int *) outbuf;

  dgprintf(5, "dslhal_api_getSNRpsds\n");

  if (flag!=1)
    return DSLHAL_ERROR_INVALID_PARAM;

  // Must be ADSL2/2+ mode
  if (!(ptidsl->AppData.TrainedMode & (ADSL2PLUS_MASKS|ADSL2_MASKS)) )
    return DSLHAL_ERROR_UNSUPPORTED_MODE;

  // currently only training HLOGpsds is supported
  // The HLOG internal buffer is in external SDRAM.
  num_swap32 = ptidsl->AppData.max_ds_tones>>2;

  for (i=0; i< num_swap32; i++)
  {
    *outbufInt_p++= dslhal_support_byteSwap32(*SNRpsds_p++);
  }

  return DSLHAL_ERROR_NO_ERRORS;
}
#endif
//*  UR8_MERGE_START CQ10979   Jack Zhang
/********************************************************************************************
* FUNCTION NAME: dslhal_api_getHLINpsds()
*
*********************************************************************************************
* DESCRIPTION:
*   Get downstream HLINpsds in ADSL2/2+ mode, as defined in g.997.1.
*
* Input:  PITIDSLHW_T *ptidsl
*         unsigned short *outbuf: Output buffer supplied from caller.
*                 for ADSL2 mode, 4*256=1024 bytes are expected.
*                 for ADSL2+ mode, 4*512=2048 bytes are expected.
*         int flag: (0: training HLINpsds). 1: showtime HLINpsds.
*
* Return: DSLHAL_ERROR_NO_ERRORS  success
*         otherwise               failed
********************************************************************************************/
#ifndef NO_ADV_STATS
unsigned int dslhal_api_getHLINpsds(tidsl_t * ptidsl, unsigned char *outbuf, int flag)
{
  unsigned int offset[2]={13, 11};
  short *HLINpsds_p= (short *)PHYS_TO_K1(dslhal_support_readOffset(ptidsl, 2, offset));
  int i, num_swap16;
  short *outbufShort_p= (short *) outbuf;

  dgprintf(5, "dslhal_api_getHLINpsds\n");

  if (flag!=1)
    return DSLHAL_ERROR_INVALID_PARAM;

  if (!HLINpsds_p)
    return DSLHAL_ERROR_BLOCK_READ;

  // Must be ADSL2/2+ mode
  if (!(ptidsl->AppData.TrainedMode & (ADSL2PLUS_MASKS|ADSL2_MASKS)) )
    return DSLHAL_ERROR_UNSUPPORTED_MODE;

  // currently only training HLINpsds is supported
  // The HLIN internal buffer is in external SDRAM.
  num_swap16 = 2 * ptidsl->AppData.max_ds_tones;

  for (i=0; i< num_swap16; i++)
  {
//    *outbufShort_p++= dslhal_support_byteSwap16(*HLINpsds_p++);
    *outbufShort_p++= *HLINpsds_p++;
  }

  return DSLHAL_ERROR_NO_ERRORS;
}
#endif
//*  UR8_MERGE_END   CQ10979*

#ifndef NO_ADV_STATS
#ifdef NOT_DRV_BUILD
/********************************************************************************************
* FUNCTION NAME: dslhal_api_getHLOGpsds()
*
*********************************************************************************************
* DESCRIPTION:
*   Get downstream HLOGpsds in ADSL2/2+ mode, as defined in g.997.1.
*
* Input:  PITIDSLHW_T *ptidsl
*         unsigned short *outbuf: Output buffer supplied from caller.
*                 for ADSL2 mode, 2*256=512 bytes are expected.
*                 for ADSL2+ mode, 2*512=1k bytes are expected.
*         int flag: 0: training HLOGpsds. (1: showtime HLOGpsds.)
*         **Note: Currently only training HLOGpsds is supported.
*
* Return: DSLHAL_ERROR_NO_ERRORS  success
*         otherwise               failed
*
********************************************************************************************/

unsigned int dslhal_api_getHLOGpsds(tidsl_t * ptidsl, unsigned short *outbuf, int flag)
{
  unsigned int offset[2]={7, 14};
  unsigned int *HLOGpsds_p= (unsigned int *)
    PHYS_TO_K1(dslhal_support_readInternalOffset(ptidsl, 2, offset));
  int i, num_swap32;
  unsigned int *outbufInt_p= (unsigned int *) outbuf;

  dgprintf(5, "dslhal_api_getHLOGpsds\n");

  if (flag!=0)
    return DSLHAL_ERROR_INVALID_PARAM;

  // Must be ADSL2/2+ mode
  if (!(ptidsl->AppData.TrainedMode & (ADSL2PLUS_MASKS|ADSL2_MASKS)) )
    return DSLHAL_ERROR_UNSUPPORTED_MODE;

  // currently only training HLOGpsds is supported
  // The HLOG internal buffer is in external SDRAM.
  num_swap32 = ptidsl->AppData.max_ds_tones>>1;
  for (i=0; i< num_swap32; i++)
  {
    *outbufInt_p++= dslhal_support_shortSwap32(*HLOGpsds_p++);
  }

  return DSLHAL_ERROR_NO_ERRORS;
}
#endif /* NOT_DRV_BUILD */

static short dslhal_staticapi_getHlog(tidsl_t *ptidsl,unsigned int index)
{
  int temVar; //,dsNumTones;

  dgprintf(4, "dslhal_staticapi_getHlog\n");

  /* Derivation of hLog from R-MSG6-LD and R-MSG7-LD for Non-ADSL2+ */
  if(ptidsl->AppData.max_ds_tones == 256)
    {
      if(index < 128)
       {
         temVar=0;
         temVar = ((ptidsl->adsl2DiagnosticMessages.rMsg6Ld[(2*index)+3]) & 0x3);
         return (60 - (((temVar << 8) + (ptidsl->adsl2DiagnosticMessages.rMsg6Ld[(2*index)+2]))));
        }
      if(index > 127 && index <256)
       {
         temVar=0;
         temVar = ((ptidsl->adsl2DiagnosticMessages.rMsg7Ld[(2*(index-128))+3]) & 0x3);
         return (60 - (((temVar << 8) + (ptidsl->adsl2DiagnosticMessages.rMsg7Ld[(2*(index-128))+2]))));
       }
    }
  /* Derivation of hLog from R-MSGx-LD for ADSL2+ modes */
  if(ptidsl->AppData.max_ds_tones == 512)
    {
      if(index < 128)
       {
         temVar=0;
         temVar = ((ptidsl->adsl2DiagnosticMessages.rMsg6Ld[(2*index)+3]) & 0x3);
         return (60 - (((temVar << 8) + (ptidsl->adsl2DiagnosticMessages.rMsg10Ld[(2*index)+2]))));
        }
      if(index > 127 && index <256)
       {
         temVar=0;
         temVar = ((ptidsl->adsl2DiagnosticMessages.rMsg11Ld[(2*(index-128))+3]) & 0x3);
         return (60 - (((temVar << 8) + (ptidsl->adsl2DiagnosticMessages.rMsg11Ld[(2*(index-128))+2]))));
       }
      if(index > 255 && index <384)
       {
         temVar=0;
         temVar = ((ptidsl->adsl2DiagnosticMessages.rMsg11Ld[(2*(index-256))+3]) & 0x3);
         return (60 - (((temVar << 8) + (ptidsl->adsl2DiagnosticMessages.rMsg12Ld[(2*(index-256))+2]))));
       }
      if(index > 383 && index <512)
       {
         temVar=0;
         temVar = ((ptidsl->adsl2DiagnosticMessages.rMsg11Ld[(2*(index-384))+3]) & 0x3);
         return (60 - (((temVar << 8) + (ptidsl->adsl2DiagnosticMessages.rMsg13Ld[(2*(index-384))+2]))));
       }
    }
  return DSLHAL_ERROR_NO_ERRORS;
}
#endif /* NO_ADV_STATS */

/********************************************************************************************
* FUNCTION NAME: dslhal_api_setPhyFeatureController()
*
*********************************************************************************************
* DESCRIPTION:
*   This function configures those PHY features that will be controlled by the MIPS.
*   This will over-ride DSP defaults.
*
* Return: DSLHAL_ERROR_NO_ERRORS :  Success
*         DSLHAL_ERROR_BLOCK_READ:  DSP Read failed
*         DSLHAL_ERROR_BLOCK_WRITE: DSP Write failed
*         DSLHAL_ERROR_INVALID_PARAM: Invalid parameters supplied
* Notes:
*      The dslhal_api_enableDisablePhyFeature function should be called after this fxn
*      to actually turn on/off the specific features
*
********************************************************************************************/
unsigned int dslhal_api_setPhyFeatureController(tidsl_t * ptidsl, unsigned int paramId, unsigned int phyCtrlWord)
{
  unsigned int offset[] = {35,0,1}, currentControlWord=0, rc=0;

  if(paramId > MAX_PHY_FEATURE_PARAMID)
  {
    dgprintf(3,"Invalid input parameter \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  offset[1] = paramId;

  rc += dslhal_api_dspInterfaceRead(ptidsl,
                                    (unsigned int) ptidsl->pmainAddr,
                                    NUM_PHY_FEATURE_LEVELS,
                                    (unsigned int *) &offset,
                                    (unsigned char *)&currentControlWord,
                                    NUM_BYTES_PER_ADDRESS);

  if(rc)
    return DSLHAL_ERROR_BLOCK_READ;

  currentControlWord = dslhal_support_byteSwap32(phyCtrlWord);

  rc += dslhal_api_dspInterfaceWrite(ptidsl,
                                     (unsigned int) ptidsl->pmainAddr,
                                     NUM_PHY_FEATURE_LEVELS,
                                     (unsigned int *) &offset,
                                     (unsigned char *)&currentControlWord,
                                     NUM_BYTES_PER_ADDRESS);

  if(rc)
    return DSLHAL_ERROR_BLOCK_WRITE;

  return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_enableDisablePhyFeatures()
*
*********************************************************************************************
* DESCRIPTION:
*   This function configures those PHY features that will be controlled by the MIPS.
*   This will over-ride DSP-defaults
*
* Return: DSLHAL_ERROR_NO_ERRORS :  Success
*         DSLHAL_ERROR_BLOCK_READ:  DSP Read failed
*         DSLHAL_ERROR_BLOCK_WRITE: DSP Write failed
*         DSLHAL_ERROR_INVALID_PARAM: Invalid parameters supplied
* Notes:
*      This function is useless if called on its own. The dslhal_api_supplyPhyFeatureCodeWord
*      and the dslhal_api_setPhyFeatureController function should be called with reqd params
*      to enable this function to turn on/off DSP features
********************************************************************************************/
unsigned int dslhal_api_enableDisablePhyFeatures(tidsl_t * ptidsl, unsigned int paramId, unsigned int phyFeatureSet)
{
  unsigned int offset[] = {35,0,0}, currentFeatureSet=0, rc=0;

  if(paramId > MAX_PHY_FEATURE_PARAMID)
  {
    dgprintf(3,"Invalid input parameter \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  offset[1] = paramId;
  rc += dslhal_api_dspInterfaceRead(ptidsl,
                                    (unsigned int) ptidsl->pmainAddr,
                                    NUM_PHY_FEATURE_LEVELS,
                                    (unsigned int *)&offset,
                                    (unsigned char *)&currentFeatureSet,
                                    NUM_BYTES_PER_INT);

  if(rc)
    return DSLHAL_ERROR_BLOCK_READ;

  currentFeatureSet = dslhal_support_byteSwap32(phyFeatureSet);

  rc += dslhal_api_dspInterfaceWrite(ptidsl,
                                     (unsigned int)ptidsl->pmainAddr,
                                     NUM_PHY_FEATURE_LEVELS,
                                     (unsigned int *)&offset,
                                     (unsigned char *)&currentFeatureSet,
                                     NUM_BYTES_PER_INT);

  if(rc)
    return DSLHAL_ERROR_BLOCK_WRITE;

  return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_api_readPhyFeatureSettings()
*
*********************************************************************************************
* DESCRIPTION:
*   This function reads the PHY features control & enable/disable settings
*
* Return: DSLHAL_ERROR_NO_ERRORS :  Success
*         DSLHAL_ERROR_BLOCK_READ:  DSP Read failed
*         DSLHAL_ERROR_BLOCK_WRITE: DSP Write failed
*         DSLHAL_ERROR_INVALID_PARAM: Invalid parameters supplied
* Notes:
*      The input to this function will be a pointer to a structure that consists of two integers
*      one each to hold the control word and the enableDisable word.
*
********************************************************************************************/
unsigned int dslhal_api_readPhyFeatureSettings(tidsl_t * ptidsl, unsigned int paramId, void *phyFeature)
{
  unsigned int offset[] = {35,0,0}, currentFeatureSet=0, rc=0;
  phySettings *pPhySetting;

  pPhySetting = (phySettings *) phyFeature;

  if(paramId > MAX_PHY_FEATURE_PARAMID)
  {
    dgprintf(3,"Invalid input parameter \n");
    return DSLHAL_ERROR_INVALID_PARAM;
  }

  offset[1] = paramId;
  rc += dslhal_api_dspInterfaceRead(ptidsl,
                                    (unsigned int) ptidsl->pmainAddr,
                                    NUM_PHY_FEATURE_LEVELS,
                                    (unsigned int *)&offset,
                                    (unsigned char *) &currentFeatureSet,
                                    NUM_BYTES_PER_INT);

  if(rc)
    return DSLHAL_ERROR_BLOCK_READ;

  pPhySetting->phyEnableDisableWord = dslhal_support_byteSwap32(currentFeatureSet);

  offset[2] = 1;
  rc += dslhal_api_dspInterfaceRead(ptidsl,
                                    (unsigned int) ptidsl->pmainAddr,
                                    NUM_PHY_FEATURE_LEVELS,
                                    (unsigned int *)&offset,
                                    (unsigned char *)&currentFeatureSet,
                                    NUM_BYTES_PER_INT);

  pPhySetting->phyControlWord = dslhal_support_byteSwap32(currentFeatureSet);

  if(rc)
    return DSLHAL_ERROR_BLOCK_WRITE;

  return DSLHAL_ERROR_NO_ERRORS;
}


#ifdef CLI2HOST_SUPPORT
//
//!!! Note: The following functions are not platform neutral because
//          direct IO (printf) is used in dslhal_api_redirectPrint().
//    For it to be used in Linux, printf() need to be replaced by printk()
//

/********************************************************************************************
* FUNCTION NAME: dslhal_api_redirectInit() (CQ:10242)
*
*********************************************************************************************
* DESCRIPTION:
*   Initializes the redirect buffer on the DSP, only call once!
*
* Input: pointer ptidsl
*
* Return:
*
* NOTE:
*
********************************************************************************************/

// --------------------------------------------------------------------------------------
// Typedef for the redirection of the CLI debug logs to the Linux kernel
// --------------------------------------------------------------------------------------

//This definition describes the buffersize without the 2 integers at the end of the buffer
//First int after the circular buffer is writePointer used from the DSP, the second entry
//is the size. We have this information at the end due to current datapump code which should
//touched only at a few points, when optimizing to just 1 instead of now 2 buffers (DSP&MIPS)
#define DEV_HOST_CLI_BUFFERSIZE (1024+2*sizeof(UINT32) - 2*sizeof(UINT32))

void dslhal_api_redirectInit(tidsl_t *ptidsl)
{
    DEV_HOST_Cli2lctl_t clictl, *clictl_p;
    int offset_cli[] = {37};

    dprintf(5,"dslhal_redirectInit()\n");
    clictl_p = (DEV_HOST_Cli2lctl_t*) dslhal_support_readOffset(ptidsl, 1, (unsigned int *)&offset_cli);
    dslhal_support_blockRead((void *) clictl_p, &clictl, sizeof(DEV_HOST_Cli2lctl_t));

    ptidsl->AppData.cliBuffsize = DEV_HOST_CLI_BUFFERSIZE; // only use once here!!!
#ifdef EB
    ptidsl->AppData.readPointer = ptidsl->AppData.cliBuffsize - 4*sizeof(UINT8);
#else
    ptidsl->AppData.readPointer = ptidsl->AppData.cliBuffsize - 1*sizeof(UINT8);
#endif //EB

    ptidsl->AppData.p_cliBuffAddr
      = (unsigned char *) PHYS_TO_K1((UINT32)shim_osAllocateDmaMemory(ptidsl->AppData.cliBuffsize + 2*sizeof(UINT32)));

    dprintf(1, "DMAmemory:%x", ptidsl->AppData.p_cliBuffAddr);

    clictl.buffsize = (UINT16)dslhal_support_byteSwap16((unsigned short)ptidsl->AppData.cliBuffsize);
    clictl.buffaddr = (PUINT8)dslhal_support_byteSwap32(virtual2Physical(
                (unsigned int)ptidsl->AppData.p_cliBuffAddr));

    dprintf(1, "PhysicalAddr: %x \n", clictl.buffaddr);
    dprintf(1, "Phy - Size: %x \n",   clictl.buffsize);

    // on the datapump set to 0 as well
    *(UINT32 *)(ptidsl->AppData.p_cliBuffAddr + ptidsl->AppData.cliBuffsize) = 0;

    dslhal_support_blockWrite((PVOID)&clictl,(PVOID)clictl_p,
                                        sizeof(DEV_HOST_Cli2lctl_t));
}




/********************************************************************************************
* FUNCTION NAME: dslhal_api_redirectMode() (CQ:10242)
*
*********************************************************************************************
* DESCRIPTION:
*   Sets the redirect mode after initializing:
*
*   Input:
*       - ptidsl pointer
*       - mode:
*         0  CLI2MCBSP
*         1  CLI2HOST
*         2  CLI2BOTH
*
* Return:
*
* NOTE:
*
********************************************************************************************/
void dslhal_api_redirectMode(tidsl_t *ptidsl, unsigned char mode)
{
    DEV_HOST_Cli2lctl_t clictl, *clictl_p;
    int offset_cli[] = {37};
    dprintf(5, "dslhal_api_redirectMode()\n");

    clictl_p= (DEV_HOST_Cli2lctl_t *) dslhal_support_readOffset(ptidsl, 1, (unsigned int *)&offset_cli);
    dslhal_support_blockRead((void *) clictl_p,
                              &clictl, sizeof(DEV_HOST_Cli2lctl_t));

    clictl.redirect = ptidsl->AppData.cliRedirect = mode;
    dprintf(1, "set CLI2Host mode: %x\n", clictl.redirect);

    dslhal_support_blockWrite((PVOID)&clictl,
                              (PVOID)clictl_p,
                              sizeof(DEV_HOST_Cli2lctl_t));


//GP>>>>
//    dslhal_support_blockRead((void *) clictl_p, &clictl, sizeof(DEV_HOST_Cli2lctl_t));
//    printf("DEBmode:%x\n",clictl.redirect);
//    printf("DEBack:%x\n",clictl.ack);
//    printf("DEBbuffsize:%x\n",clictl.buffsize);
//    printf("DEBbuffaddr:%x\n",clictl.buffaddr);
//----
}




/********************************************************************************************
* FUNCTION NAME: dslhal_api_redirectPrint() (CQ:10242)
*
*********************************************************************************************
* DESCRIPTION:
*   Prints the CLI buffer, if mode appropreate mode is set
*
* Input: pointer ptidsl
*
* Return:
*
* NOTE:
*
********************************************************************************************/
void dslhal_api_redirectPrint(tidsl_t *ptidsl)
{
    UINT32 writePointer;
    UINT16 counter;
    int delta;
#ifdef EB
    int readTrigger;
#endif

    dprintf(5, " dslhal_api_redirectPrint()\n");

    if ((ptidsl->AppData.cliRedirect == CLI2HOST) || (ptidsl->AppData.cliRedirect == CLI2BOTH))
    {
        writePointer = DSLHAL_REG32((UINT32)(ptidsl->AppData.p_cliBuffAddr + ptidsl->AppData.cliBuffsize));
        delta = writePointer - (ptidsl->AppData.readPointer + 1);

        if (delta < 0)
            delta += ptidsl->AppData.cliBuffsize;

#ifdef EB
        //algorithm works in BE only, when buffer is aligned to 4 bytes
        //buffer is not empty, so read: First increment then read!
        readTrigger = delta-7;
        if (readTrigger >= 0)
        {
            for (counter = 0; counter<=readTrigger; counter+=4)
            {
                ptidsl->AppData.readPointer+=4;
                if (ptidsl->AppData.readPointer == ptidsl->AppData.cliBuffsize)
                    ptidsl->AppData.readPointer = 0;
                printf("%c%c%c%c",
                  ptidsl->AppData.p_cliBuffAddr[ptidsl->AppData.readPointer+3],
                  ptidsl->AppData.p_cliBuffAddr[ptidsl->AppData.readPointer+2],
                  ptidsl->AppData.p_cliBuffAddr[ptidsl->AppData.readPointer+1],
                  ptidsl->AppData.p_cliBuffAddr[ptidsl->AppData.readPointer+0]);
            }
        }
#else //EB, EL
        for (counter=0; counter<delta; counter++)
        {
            ptidsl->AppData.readPointer++;
            if (ptidsl->AppData.readPointer == ptidsl->AppData.cliBuffsize)
                ptidsl->AppData.readPointer = 0;
            printf("%c", ptidsl->AppData.p_cliBuffAddr[ptidsl->AppData.readPointer]);
        }
#endif //EL
   }
}


//UR8_MERGE_START GERALD_CQ_REDIRECT_BUGFIX2_20060328 Gerald
/********************************************************************************************
* FUNCTION NAME: dslhal_api_redirectFree()
*
*********************************************************************************************
* DESCRIPTION:
*   Frees the memory
*
*   Input:
*       - ptidsl pointer
*
* NOTE: This is used for rebooting
*
********************************************************************************************/
void dslhal_api_redirectFree(tidsl_t *ptidsl)
{
    DEV_HOST_Cli2lctl_t clictl, *clictl_p;
    int offset_cli[] = {37};
    clictl_p= (DEV_HOST_Cli2lctl_t *) dslhal_support_readOffset(ptidsl, 1, (unsigned int *)&offset_cli);
    clictl.redirect = ptidsl->AppData.cliRedirect = 0;
    dprintf(1, "set CLI2Host mode: %x\n", clictl.redirect);

    dslhal_support_blockWrite((PVOID)&clictl,
                              (PVOID)clictl_p,
                              sizeof(DEV_HOST_Cli2lctl_t));

    dprintf(5, "dslhal_api_redirectFree()\n");
    shim_osFreeDmaMemory(ptidsl->AppData.p_cliBuffAddr,
                         ptidsl->AppData.cliBuffsize + 2*sizeof(UINT32));
}
//UR8_MERGE_END GERALD_CQ_REDIRECT_BUGFIX2_20060328



#endif  // CLI2HOST_SUPPORT



