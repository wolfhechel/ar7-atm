#ifndef __CTRL_MAIN_H__
#define __CTRL_MAIN_H__ 1

// *****************************************************************
// CTL_interface_U.h
// This file contains the definitions of the constants and structures that the
// controller module must share with other modules.
// Rev B version
//*****************************************************************
// History
// 10/13/1997  Mike Locke       Written
// 12/16/97    Mike Locke       Add some #defines for use by the ISRs
// 12/17/97    Mike Locke       Add stuff needed for TI EVM support
// 12/18/97    Mike Locke       Change DDI structure to include ATUR requirements
// 1/1/98      Mike Locke       Pass AFE buffer as a pointer
// 2/10/98     Mike Locke       Increase element sizes in serial structure to work
//                              around linker bug
// 2/11/98     Mike Locke       Increase number of encoder DDI buffers to 3
// 2/11/98     Mike Locke       Add fields to ddi structure to get deterministic
//                              response timing to commands.
// 2/12/98     Mike Locke       Added prototypes for DDI control
// 3/18/98     Mike Locke       Increase NUM_UP_DI_BUF to 5 so that it is reliable
// 5/8/98      Mike Locke       Increase EXTENDER_QUEUE size to
//                              accomodate loader initialization
// 6/5/98      Mike Locke       Add declaration for DI_decstop_U()
// 6/15/98     Mike Locke       Add flag to deal with shortened frame
// 8/24/98     Vineet Ganju     Add prototypes for DI bitswap control functions
// 1/29/99     Mike Locke       Change AFEBUFSIZE in order to eliminate need for sumdecimate
// 08/06/99    Michael Seidl    Split datapump into DMT ISR and background tasks,
//                              added structure for IRP/CSR push/pop
// 12/16/99    N. Warke         Convert to FDM codebase for ATUR by removing PREC, CES & FDEC
// 01/05/00    Jack Huang       Add SW DI, change number of US/DS DI buffer to be 1
// 01/17/00    Barnett          Mod's in support of merging NIC hardware rev 6/7 T1.413 codebases.
// 01/20/00    Jack Huang       Removed afe_prec buffer for FDD mode
// 01/21/00    Wagner           derfmake changes
// 02/09/00    Barnett          Mod's in support of merging
//                              3000P_rev6rev7_Glite_Ghs_101.20000201 to 3000P_rev6rev7_FDD.20000207.
// 05/03/00    Michael Seidl    Mod's in support of merging KAPIL changes to the common baseline
// 05/09/00    Jack Huang       Simplified ddi_type for SWDI and unionize with Viterbi
//                              u3u2u1, u3 buffers
// 05/17/00    Jack Huang       Change FLUSH to be 11 for new Viterbi implementation
// 06/07/00    Seidl/Ding       combined rcv/xmt afe_buffer for KAPIL. Simplified token constructs
// 07/12/00    Derf             moved complex_* to typedefs.h
// 07/14/00    Derf/Seidl       Converted ctl_util_u.c 1-line functions to macros
// 08/02/00    Michael Seidl    added DMA synchronizer for Ax5
// 08/12/00    Yinong Ding      Introduced DPLL related functions
// 08/31/00    Michael Seidl    fixed the frame adjustment algo for KAPIL. DMA's got out of sync.
// 09/18/00    Michael Seidl    added DMA_1ST_IN_SYNC state to the DMA synchronizer
// 01/11/01    U.Dasgupta       Cleanup of unused GHS variables.
// 09/21/00    U.Dasgupta/      Added an option to have separate transmit and receive buffers.
//             F.Mujica         This code is selectable using token "SEPARATE_TX_RX_BUFFERS".
// 10/02/00    U.Dasgupta/      Removed DPLL related prototypes and put them in ssm_timing_dpll_r.h
//             F.Mujica
// 10/17/00    Michael Seidl    enabled power down features for ax05.
// 01/26/01    Michael Seidl    cleaned up some definitions KAPIL to satisfy naming conventions.
// 04/03/01    J. Bergsagel     Removed references to DSPDP_CHIPSET_GEN
//                              Removed FDD token (assumed always 1)
//                              Removed SWDI token (assumed always 1)
//                              Removed AD11_20 token (assumed always 1)
//                              Removed ATUC token (assumed always 0)
//                              Removed STD token (assumed always 1)
//                              Removed TID token (assumed always 0)
//                              Removed TII token (assumed always 1)
// 04/12/01    Barnett          Moved PowerDownEnable from modem_type to host i/f struct.
// 04/13/01    Jack Huang       made ifft mode selectable at run-time
// 04/23/01    M. Halleck       Merge Astro Wu's DDC enhancements
// 05/10/01    Michael Seidl    merged ANNEX B from 4000R
// 05/10/01    Michael Seidl    resolved holes in data memory map in support of CCS 2.0 beta
// 05/10/01    Michael Seidl    moved macros for AGC functions to AGC module
// 06/05/01    M. Capps         Changed DSP_DEBUG to ENHANCED_SERIAL_DEBUG
// 07/03/01    M. Capps         replaced ENHANCED_SERIAL_DEBUG with !DDC;
//                              added qprintf() decl;  added inline DBG_DDC_displayString definition;
//                              removed useless !DDC components from structures
// 07/16/01    U.Dasgupta       removed prototypes for Set_coarseGain_U and Set_fineGain_U
//                              as they are not used anymore.
// 07/25/01    J. Bergsagel     Changed typedefs.h to dpsys_typedefs.h
// 08/02/01    Michael Seidl    renamed KAPIL token to !AD1X
// 08/09/01    S.Yim            Moved definition for TRUE and FALSE to dpsys_typedefs.h
// 10/26/01    Michael Seidl    moved CLI specific stuff to ctl_cli.h
// 10/26/01    Michael Seidl    moved CLI specific stuff to ctl_cli.h
// 01/30/02    D. Mannering     Update AFE/FFT register structures for Version
//                              6 of register map.
// 02/06/02    D. Mannering     Added FPGA rx memory offsets
// 01/23/02    Umesh Iyer       merged DEBUG_LOG mods from MR3 developer branch
// 02/25/02    Umesh Iyer       Added loop attenuation DEBUG_LOG code.
// 03/06/02    N. Warke         Defined two new AFE interface functions: Set_GPIO and Set_Hybrid
//                              Also defined constants for Kapil GPIOs that can be used along with Set_GPIO
// 04/25/02    D Mannering      Added prototypes  CTRL_MAIN_initAfePort
//                              and CTRL_MAIN_startAfeDma
// 05/29/02    S.Yim            Removed AD1X
// 07/15/02    D. Mannering     Add  LOG2_RXTOTX_RATE_RATIO_128,
//                              TX_ENTRY_OFFSET_128 and startControlBits
//                              to CTRL_MAIN_modemEnv_t
// 07/29/02    S.Yim            Added idmaAdjust in CTRL_MAIN_modemEnv_s - samples to adjust
// 08/02/02    D. Mannering     Remove framesize from CTRL_MAIN_modemEnv_s
// 09/05/02    D. Mannering     Remove !FPGA PATH(1,2)_RX_MEM_OFFSET definitions and
//                              protype for void UTIL_MEM_moveInts
// 09/09/02    D. Mannering     Removed unused fields from CTRL_MAIN_modemEnv_s
//                              (idmaAdjust,dmaSync,txAheadCnt,rxAheadCnt) and unused
//                              structures
// 08/19/02    Paul Hunt        Merged HYBRID_SWITCH changes from MR6
// 09/16/02    J. Bergsagel     Put back in !FPGA PATH(1,2)_RX_MEM_OFFSET defs. for chiptest code
// 09/17/02    J. Bergsagel     Added CTRL_MAIN_exceptionCounts_t struct typedef
// 10/01/02    J. Bergsagel     Changed dmaError to realTimeErrors and moved to
//                              CTRL_MAIN_exceptionCounts_t, and changed to UINT32
// 10/11/02    J. Bergsagel     Changed order of members in CTRL_MAIN_modemEnv_t for proper alignment.
// 11/11/02    D. Mannering     Removed startControlBits from  CTRL_MAIN_modemEnv_t
// 11/15/02    D. Mannering     Add IDMA frame size change flag to  CTRL_MAIN_modemEnv_t
// 01/08/02    A. Redfern       Removed xmtRateSwitch_f and changeRate_f.
// 01/20/03    Sameer V         Added TC dma errors to CTRL_MAIN_exceptionCounts_t.
// 01/21/03    MCB              Implemented Ax7 UNIT-MODULE modular software framework.
// 01/23/03    F. Mujica        Moved switchable hybrid code to mi_hybridswitch_r.h (soon to be phy_hyb.h)
// 02/03/03    D. Mannering     Added CO profile code
// 02/13/03    D. Mannering     Added ODMA diags and RS realtime and codec filter overflow
//                              regs to CTRL_MAIN_exceptionCounts_t
// 02/21/03    D. Mannering     Added DEBUG_DUMP.
// 03/24/03    Mallesh          Changed initial debug_dump address so that debug_dump
//                              gets allocated after debug_log.
//                              The debug_log defines have been moved to msm_decl_r.h
// 04/09/03    S. Yim           Added diagAnlg_f element to CTRL_MAIN_modemEnv_t struct
// 04/24/03    D. Mannering     Move CO profile structs and protypes to ctrl_prof.h
// 05/05/03    D. Mannering     Review comments - changed idma comments to odma, removed unneed
//                              prototype definition, renamed co ids and moved to ctrl_prof.h
// 05/16/03    D. Mannering     Removed duplicate code for DEBUG_DUMP
// 05/20/03    D. Mannering     Changed for coding stardards (int => SINT32, etc)
// 05/20/03    Prabha           Added DI LoopBack support
// 06/11/03    D. Mannering     Added swap buffers flag for ADSL_PLUS
// 10/11/03    K. Nagaraj       swapBuffers_f is now present for all the modes but is used only by ADSL2+ mode.
// 10/17/03    D. Mannering     Changed the number of tx buffers from 2 to 3 and added txBufferIndex
//                              to CTRL_MAIN_modemEnv_t struct
// 12/23/03    D. Mannering     Removed changeIdmaFrame_f from the CTRL_MAIN_modemEnv_t structure.
// 12/24/03    D. Mannering     Added chipIsSangam to CTRL_MAIN_modemEnv_t structure.
// 02/03/04    D. Mannering     Added token OHIO_SUPPORT, changed chipIsSangam to chipId,
//                              Added fft1Scale, fft2Scale, and generateNextTxBuffer to CTRL_MAIN_modemEnv_t.
// 02/24/04    D. Mannering     Removed FPGA memory path offsets
// 04/01/04    Sameer V         Added dynamic memory management using DSPBIOS for multimode operation for
//                              ADSL1/Re/ADSL2/ADSL2+.
//                              Added flags to enable/disable EC and PHY_PATH_ENABLE based on the training mode selected.
//                              Increased the size of rcv_buff to support ADSL2+ mode.
// 06/07/04    S.Yim            Increased size of array for viterbi to support ADSL2+ mode
// 08/09/04    D. Mannering     Added changeRxRate to CTRL_MAIN_modemEnv_t.
// 08/12/04    B.Zhang          Decleared softwaretrap CTRL_MSM_AFE_myFail() for AFE Diag
// 02/09/05    Kapil Gulati     Added support for Ohio250 (i.e. 7200) Chip Id.
// 03/24/04    Venkat R         Removed DDC Token & struct def displaytype
//                              and fn proto dbg_ddc_displaystring
// 04/01/05    Venkat R         Removed CO_PROFILE (Set to 1) Token
// 05/19/05    Venkat R         CQ9600: Increased allocation for Annex M
//                              Removed some unused constant declarations
// 07/14/05    Shanti K         Modified the data structure CTRL_MAIN_modemEnv_t
// 07/07/05    Mike Locke       Add memory allocation function prototypes CQ9583
// 11/10/05    Venkat R         CQ9583: Add variables to AMR/IER/DP variables to int_save type to
//                              support NONBIOS mode
// 12/12/05    C Urrutia        CQ10073: Memory allocation for AGC/RFI_detection routines
//*********************************************************************************************

#include "env_def_defines.h"
#include "env_def_typedefs.h"
#include "dev_host_interface.h"

// Number of modem instances
#define CHANNELS                1


// Total number of elements in the downstream AFE buffer.
// Will be 552 with stretched cyclic prefix.
// The last framesize/2 (a variable setup by a utility) entries will be
// exchanged with the AFE.  Will be 1088 or 1104 on the B level
// DSP, which also requires the UP and DOWN directions to have
// the same values on the ATU-C.  On the B level modem, the last
// framesize entries will be exchanged with the AFE.

#define AFE_BUFSIZE        544
#define CP_LENGTH           32

#define LOG2_RXTOTX_RATE_RATIO_128  1
#define LOG2_RXTOTX_RATE_RATIO_64       2
#define LOG2_RXTOTX_RATE_RATIO_32       3

#define TX_ENTRY_OFFSET_32    64
#define TX_ENTRY_OFFSET_64    128
#define TX_ENTRY_OFFSET_128   256

// Initial framesize to use.  This value should be 1024 with the
// B level modem

#define INITIAL_FRAMESIZE 512

/* used to adjust memory receive memory address offsets for FPGA code */
#if !OHIO_SUPPORT
#if FPGA
 #define PATH1_RX_MEM_OFFSET 0x10000000
 #define PATH2_RX_MEM_OFFSET 0x20000000
#else
 #define PATH1_RX_MEM_OFFSET 0
 #define PATH2_RX_MEM_OFFSET 0
#endif
#endif
/* number of Frames between rx CP on and tx CP on */
#define FRAMES_BETWEEN_CP   3
// Number of frequency bins  for transmission.  Not used for A level
// modem.

#define DOWN_DI_BUFSIZE 256
#define NUM_DOWN_DI_BUF 1
#define NUM_UP_DI_BUF 1


// Number of frequency bins  for reception.  Not used for A level
// modem.   May be 64 in some configurations.
#define SAMPLES_PER_DDI_INT     552


// Must be a power of 2
#define AFEQUEUE_SIZE           16
#define EXTENDERQUEUE_SIZE      32


// Definitions associated with the utilities
#define MIN_FRAMESIZE 1024
#define MAX_FRAMESIZE 2208

#define MIN_VCXO_CTRL -32768
#define MAX_VCXO_CTRL 32767

#define MIN_XMT_ATTN 0
#define MAX_XMT_ATTN 6

#define MIN_FINE_GAIN 16
#define MAX_FINE_GAIN 63

#define MIN_COARSE_GAIN 0

#define MAX_COARSE_GAIN 7

#define MAX_ECHO_ATTN 0
#define MIN_ECHO_ATTN 63

#if DEBUG_DUMP
#define HOST_DUMP_ADDRESS    0x16040000
#endif


// Commands to CTRL_MAIN_sendCodecCmds

enum { VCXO_QUEUE, AFE_QUEUE, EXTENDER_QUEUE, VCXO_QUEUE2};



// DDI ISR states

#define DDIrcv_idle             0
#define DDIrcv_steadystate      1

#define DDIxmt_idle             0
#define DDIxmt_waitgo           1
#define DDIxmt_steadystate      2

#if OHIO_SUPPORT
#define SANGAM_CHIP_ID         0x05
#define OHIO212_CHIP_ID        0x18
#define OHIO250_CHIP_ID        0x2b

// This is not an ID of any real chip but simply a constant used
// internally by DSP to distinguish between Ohio family vs
// Sangam family of chips.
#define OHIO_CHIP_ID           0xFF
#endif
// DIM_PER_CONS (2) * MAX_US_NYQ_TONES (64)
// CQ9600: The terms above are also defined in ctrl_msm_swif.h, do not want a dependency on
// this file, so a direct declaration is used
#define US_DI_BUFSIZE                   128


// ***********************************************************
// Structures
//************************************************************

typedef struct {
  SINT16 afeReceive[AFE_BUFSIZE];
} afe_bufftype;

// Trick to simplify migration to various AFE buffer schemes

 #define Up_AFE_BufferType afe_bufftype *

 #define Down_AFE_BufferType afe_bufftype *


typedef struct {
#if OHIO_SUPPORT
  UINT16 transmit[3*(AFE_BUFSIZE >> LOG2_RXTOTX_RATE_RATIO_64)] ; // three transmit buffers
#else
  UINT16 transmit[2*(AFE_BUFSIZE >> LOG2_RXTOTX_RATE_RATIO_64)] ; // two transmit buffers
#endif
  afe_bufftype  receive1[2];      // double buffers for incoming samples first path
  afe_bufftype  receive2[2];      // double buffers for second path
// above transmit and receive buffers always need to be at the top of the struct for alignment
  UINT32 afeStatus;               // the AFE status register must be read every interrupt
  UINT32 fft1Status;              // status from fft1
  UINT32 fft2Status;              // status from fft2
  SINT32 txEntryOffset;           // offset for 2nd tx buffer
  SINT32 doubleTxRate;            // Select 64-tone Vs 32-tone IFFT
  UINT16 startFlag_f;             // flag to help identify real time problems
  SINT16 afeBuffIndex;            // flag to alternate ping-pong buffer (rx and tx)
  UINT16 reset_f;                 // used to hold reset flag
  UINT16 log2RxToTxRateRatio ;    // receive to transmit frame length ratio in log2
  SINT16 teqMode ;                // type of hardware teq
#if OHIO_SUPPORT
  UINT16 chipId ;                 // Sangam/Ohio chip type
#else
  UINT16 changeIdmaFrame_f ;      // if true change IDMA frame size
#endif
  UINT32 diagAnlg_f;              // if true, start analog diagnostics (exit digital diagnostics)
#if OHIO_SUPPORT
  SINT32 txBufferIndex ;          // Index to which buffer the DSP is to write. (0,1,2)
  UINT16 fft1Scale ;              // FFT scaled value for path 1
  UINT16 fft2Scale ;              // FFT scaled value for path 2
  UINT16 generateNextTxBuffer;    // Flag to generate extra buffer for CP enable
  SINT16 changeRxRate ;           // Make Rx change indicator
#endif
  UINT16 swapBuffers_f ;          // if true, swap order of input buffers when combining
  UINT16 phy_path_enable_f ;      // if true, path is enabled
  UINT16 phy_ec_enable_f ;        // if true, EC is enabled
  UINT32 scale_down_fft_enabled;  // if TRUE, scale down the input to last stage of FFT
  UINT16 NSCus;                   // Nyq Frequency Tones for each Annex, 32 for Annex A , 64 for Annex B & M
  UINT16 NSCus_shift_fact;        // Corresponds to NSCus ration to ANNEX NSCus of 32, 32 corresponds to 0,64 corresponds to 1
                                  // for use by the controller
} CTRL_MAIN_modemEnv_t ;



// Some #defines to be used by the assembly, all byte offsets.

#define AFE_QUEUE_EMPTY_OFF     0
#define EXT_QUEUE_EMPTY_OFF     2
#define VCXOFLAG_OFF            8
#define VCXOLEVEL_OFF           4
#define AFEQUEUE_IN_OFF         10
#define AFEQUEUE_OUT_OFF        12
#define EXTENDERQUEUE_IN_OFF    14
#define EXTENDERQUEUE_OUT_OFF   16
#define AFEQUEUE_OFF            20
#define EXTENDERQUEUE_OFF       (AFEQUEUE_OFF+4*AFEQUEUE_SIZE)



// The following must be kept up to date.  All are byte offsets.
#define RCVCOMMAND_OFF  0
#define XMTCOMMAND_OFF  1
#define RCVBITSWAP_OFF  2
#define XMTBITSWAP_OFF  3
#define RCVSTATE_OFF    4
#define XMTSTATE_OFF    5
// XMTBUFF_IN_OFF must be even
#define XMTBUFF_IN_OFF  6
#define XMTBUFF_OUT_OFF 7
// RCVBUFF_IN_OFF must be even
#define RCVBUFF_IN_OFF  8
#define RCVBUFF_OUT_OFF 9
#define XMTFRAMECNT_OFF 10
#define RCVFRAMECNT_OFF 11
// Contrived address to allow simultaneous copy of both chars
#define QUERCVCMD_OFF   12
#define QUEXMTCMD_OFF   13
#define RCVINDICATOR_OFF        14
#define REALTIME_ERR_OFF        16
#define FLUSH  11


// Unionized with rx di_buffer
typedef struct {
        UINT8     u3[(2*DOWN_DI_BUFSIZE/2+FLUSH)*8];   // output U3 array for Viterbi decoder
        UINT8     u3u2u1[2*DOWN_DI_BUFSIZE/2+FLUSH];   // output U3U2U1 array for Viterbi decoder
} U_TYPE;

typedef struct {
#if DS_LOOP_BACK
        byte          xmtbuff[NUM_DOWN_DI_BUF][DOWN_DI_BUFSIZE*2];
#else
        byte          xmtbuff[NUM_UP_DI_BUF][US_DI_BUFSIZE];
#endif
        complex_short rcvbuff[NUM_DOWN_DI_BUF][(2*DOWN_DI_BUFSIZE) + 1];
} ddi_type;

// structure for context store to re-enable interrupts in ISR
// CQ9583: Add AMR/IER/DP to int_save type for NONBIOS mode
typedef struct{
    SINT32 IRP;
    SINT32 CSR;
    SINT32 AMR;
    SINT32 IER;
    UINT32 DP;
} int_save_type;

#if DEBUG_DUMP
typedef struct{
   UINT32  baseAddress ;    // host address
   UINT32  lastBlockAddress ; // address where last block was started
   SINT32  currentIndex ;   // index of block to write
      } DBG_DUMP_control_t ;
#endif

// Function prototypes
#if DEBUG_DUMP
void DBG_DMP_memoryDump(PSINT32 source, SINT32 wordLength, SINT32 wrapCount) ;
#endif

void Display_Char_U(SINT8 inchar, SINT8 position);
void Set_LED_U(UINT16 led_num, SINT16 on_off);
void Toggle_LED_U(UINT16 led_num);
void Set_VCxo_U(SINT16 level);
void UTIL_FUNC_setRxPath(SINT16 element, SINT16 value);
void CTRL_MAIN_startAfeDma(void);
void CTRL_INIT_resetFpga() ;
void CTRL_MSM_resetTeqCoefs(void) ;

void CTRL_MSM_allocatePersistentMemory(void);
void CTRL_MSM_allocateTrainingMemory();

void CTRL_MSM_allocDSMMemory();
void CTRL_MSM_allocTEQMemory();
void CTRL_MSM_allocateECMemory();
void CTRL_MSM_allocateTEQ2Memory();
void CTRL_MSM_allocateQLNMemory();
void CTRL_MSM_freeNDELTQLNMemory();
void CTRL_MSM_allocDELTStringMemory();
void CTRL_MSM_freeDELTMemory();
void CTRL_MSM_reallocateTrainMemory();
void CTRL_MSM_reallocateDiagMemory();
void CTRL_MSM_freeMemory();
void CTRL_MSM_allocate2PathMemory();
void CTRL_MSM_allocAGCMemory();
void CTRL_MSM_freeAGCMemory();

#if MFGR_DIAG
void CTRL_MSM_AFE_myFail();
#endif

#if LOOP_BACK_DEBUG
void swtrap();
#endif
#endif
