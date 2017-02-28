#ifndef ___DSL_REGISTER_DEFINES_H___
#define ___DSL_REGISTER_DEFINES_H___ 1

/*******************************************************************************
* FILE PURPOSE:     DSL HAL H/W Registers and Constant Declarations for Sangam
*
********************************************************************************
* FILE NAME:        dsl_hal_register.h
*
* DESCRIPTION:  
*       Contains DSL HAL APIs for Adam2 OS functions
*  
*                    
* (C) Copyright 2001-02, Texas Instruments, Inc.
*    History
*    Date        Version            Notes
*    06Feb03     0.00.00            RamP   Created
*    21Mar03     0.00.01            RamP   Changed header files for Modular 
*                                          build framework
*    21Mar03     0.00.02            RamP   Introduced malloc size for DSP f/w
*    07Apr03     0.00.03            RamP   Implemented new error reporting scheme
*                                          Changed Commenting to C style only
*    12Apr03     0.00.04            RamP   Added Interrupt Mask defines
*    14Apr03     0.00.05            RamP   Renamed macros for REG8, REG16 & REG32
*    21Apr03     0.01.00            RamP   Added Interrupt source/clear registers
*                                          Changed enum RSTATE_SHOWTIME to 5
*    24Apr03     0.01.01            RamP   Moved the RSTATE enum to api.h
*                                          Added olay recovery error condition
*    14May03     0.01.02            RamP   Added defines for power computation
*                                          Added error condition for hybrids
*    04Jun03     0.01.03            RamP   Added enum for config flags, 
*                                          Cleaned up AR5 register defines
*                                          Added defines for higher data rate
*    06Jun03     0.01.04            RamP   Added error & interrupt defines
*    09Jun03     0.01.05            RamP   Modified enum for current config
*                                          Added additional C-Rates defines
*    18Jul03     0.01.06            RamP   Modified internal build flow
*    21Aug03     0.01.07            RamP   Added constellation buffer size
*    08Oct03     0.01.08            RamP   Added us/ds Bits n gains size
*    12Oct03     0.01.08            RamP   Added ADSL2 Message sizes, lengths 
*                                          and offsets for various formats
*    29Oct03     0.01.09            RamP   Added ADSL2 Delt offsets & sizes
*    24Nov03     0.01.10            RamP   Added bit field number, scan vector
*    26Dec03     0.01.11            RamP   Removed the oamFeature masks to api.h
*******************************************************************************/

#include "env_def_typedefs.h"
#ifdef INTERNAL_BUILD
#include "dev_host_internalinterface.h"
#endif
#include "dev_host_interface.h"
#include "dsl_hal_api.h"

#define ADSLSS_BASE                                                     0x01000000
#define BBIF_BASE                                                       0x02000000

#define ADSLSSADR                                                       (BBIF_BASE+0x0000)
#define ADSLSSADRMASK                                                   0xff000000
#define WAKEUP_DSP                                            0x00000001

/* Ax7 Reset Control */

#define RST_CNTRL_BASE                                                  0x8611600
#define RST_CNTRL_PRCR                                                  (RST_CNTRL_BASE + 0x00 )

#define    RST_CNTRL_PRCR_GPIO                                  0x00000040
#define    RST_CNTRL_PRCR_ADSLSS                                0x00000080
#define    RST_CNTRL_PRCR_USB                                   0x00000100
#define    RST_CNTRL_PRCR_SAR                                   0x00000200
#define    RST_CNTRL_PRCR_DSP                                   0x00800000
#define    RST_CNTRL_PRCR_EMAC1                                 0x00200000      /* EMAC1 reset */

#define    RST_CNTRL_SWRCR                                      (RST_CNTRL_BASE + 0x04 )
#define    RST_SWRCR_SWR0                                       0x00000001
#define    RST_SWRCR_SWR1                                       0x00000002

#define    RST_CNTRL_RSR                                        (TNETD53XX_RST_CNTRL_BASE + 0x08 )
#define    RST_RSR_RSCAUSE                                      0x00000003   /* Software Reset Caused by writing to SWR1 bit */


/* ******************************************************
Interrupt sources on Ax7 interrupt controller.
The reserved sources are noted.
********************************************************* */

#define INTR_CNTRL_SRC_SECOND                                   0
#define INTR_CNTRL_SRC_EXTERNAL0                                1
#define INTR_CNTRL_SRC_EXTERNAL1                                2
/* reserved sources ... */
#define INTR_CNTRL_SRC_TIMER0                                   5
#define INTR_CNTRL_SRC_TIMER1                                   6
#define INTR_CNTRL_SRC_UART0                                    7
#define INTR_CNTRL_SRC_UART1                                    8
#define INTR_CNTRL_SRC_DMA0                                     9
#define INTR_CNTRL_SRC_DMA1                                     10
/* reserved sources ... */
#define INTR_CNTRL_SRC_SAR                                      15
/* reserved sources ... */
#define INTR_CNTRL_SRC_EMAC0                                    19
/* reserved sources ... */
#define INTR_CNTRL_SRC_VLYNQ0                                   21
#define INTR_CNTRL_SRC_CODEC_WAKE                               22
/* reserved sources ... */
#define INTR_CNTRL_SRC_USB                                      24
#define INTR_CNTRL_SRC_VLYNQ1                                   25
/* reserved sources ... */
#define INTR_CNTRL_SRC_EMAC1                                    28
#define INTR_CNTRL_SRC_I2C                                      29
#define INTR_CNTRL_SRC_DMA2                                     30
#define INTR_CNTRL_SRC_DMA3                                     31
/* reserved sources ... */
#define INTR_CNTRL_SRC_VDMA_RX                                  37
#define INTR_CNTRL_SRC_VDMA_TX                                  38
#define INTR_CNTRL_SRC_ADSLSS                                   39

#ifndef K0BASE 
#define K0BASE                                                          0x80000000
#endif

#ifndef K1BASE 
#define K1BASE                                                          0xA0000000
#endif

#ifndef PHYS_ADDR
#define PHYS_ADDR(X)                                                    ((X) & 0X1FFFFFFF)
#endif

#ifndef PHYS_TO_K0
#define PHYS_TO_K0(X)                                                   (PHYS_ADDR(X)|K0BASE)
#endif

#ifndef PHYS_TO_K1
#define PHYS_TO_K1(X)                                                   (PHYS_ADDR(X)|K1BASE)
#endif

#ifndef DSLHAL_REG8
#define DSLHAL_REG8( addr )                                          (*(volatile unsigned short *) PHYS_TO_K1(addr))
#endif

#ifndef DSLHAL_REG16
#define DSLHAL_REG16( addr )                                         (*(volatile unsigned short *)PHYS_TO_K1(addr))
#endif

#ifndef DSLHAL_REG32
#define DSLHAL_REG32( addr )                                         (*(volatile unsigned int *)PHYS_TO_K1(addr))
#endif

#ifndef NULL
#define NULL    0
#endif

#ifndef TRUE
#define TRUE    (1==1)
#endif

#ifndef FALSE
#define FALSE   (1==2)
#endif

/*******************************************************************************
* Type Defines for Library
********************************************************************************/
typedef unsigned int size_t;

#define TIDSL_HW_CREATED                                                0x00000001
#define TIDSL_HW_OPENED                                                 0x00000002
#define TIDSL_HW_STARTED                                                0x00000004
#define TIDSL_OS_INITIALIZED                                            0x00000008

/* Data Pump CRATES Table Defines */
#define SIZE_OF_CRATES1_TABLE                                           120
#define CRATES1_BF_LS0                                                  7
#define CRATES1_BI_LS0                                                  17
#define CRATES1_BF_AS0                                                  0
#define CRATES1_BI_AS0                                                  10
#define CRATES1_BF_DSRS                                                 20
#define CRATES1_BI_DSRS                                                 21
#define CRATES1_BFI_DSS                                                 22
#define CRATES1_BFI_DSI                                                 23
#define CRATES1_BF_USRS                                                 25
#define CRATES1_BI_USRS                                                 26
#define CRATES1_BFI_USS                                                 27
#define CRATES1_BFI_USI                                                 28

#define FAST_PATH                                                       0
#define INTERLEAVED_PATH                                                1

#define LINE_NOT_CONNECTED                                              0
#define LINE_CONNECTED                                                  1
#define LINE_DISCONNECTED                                               2 
#define LINE_NOT_TO_CONNECT                                             3 

#define MAXSECTIONS                                                     125

/*****************************************************************************************
 * Localstructure declarations
 *
 ****************************************************************************************/
enum
{
  DSLHAL_ERROR_NO_ERRORS,                /* 00 */
  DSLHAL_ERROR_UNRESET_ADSLSS,           /* 01 */
  DSLHAL_ERROR_RESET_ADSLSS,             /* 02 */
  DSLHAL_ERROR_UNRESET_DSP,              /* 03 */ 
  DSLHAL_ERROR_RESET_DSP,                /* 04 */    
  DSLHAL_ERROR_NO_FIRMWARE_IMAGE,        /* 05 */
  DSLHAL_ERROR_MALLOC,                   /* 06 */
  DSLHAL_ERROR_FIRMWARE_MALLOC,          /* 07 */
  DSLHAL_ERROR_DIAG_MALLOC,              /* 08 */
  DSLHAL_ERROR_OVERLAY_MALLOC,           /* 09 */
  DSLHAL_ERROR_CODE_DOWNLOAD,            /* 10 */
  DSLHAL_ERROR_DIAGCODE_DOWNLOAD,        /* 11 */
  DSLHAL_ERROR_BLOCK_READ,               /* 12 */  
  DSLHAL_ERROR_BLOCK_WRITE,              /* 13 */  
  DSLHAL_ERROR_MAILBOX_READ,             /* 14 */  
  DSLHAL_ERROR_MAILBOX_WRITE,            /* 15 */
  DSLHAL_ERROR_MAILBOX_NOMAIL,           /* 16 */
  DSLHAL_ERROR_MAILBOX_OVERFLOW,         /* 17 */
  DSLHAL_ERROR_INVALID_PARAM,            /* 18 */
  DSLHAL_ERROR_ADDRESS_TRANSLATE,        /* 19 */
  DSLHAL_ERROR_FIRMWARE_CRC,             /* 20 */
  DSLHAL_ERROR_FIRMWARE_OFFSET,          /* 21 */
  DSLHAL_ERROR_CONFIG_API_FAILURE,       /* 22 */ 
  DSLHAL_ERROR_EOCREG_API_FAILURE,       /* 23 */
  DSLHAL_ERROR_VERSION_API_FAILURE,      /* 24 */
  DSLHAL_ERROR_STATS_API_FAILURE,        /* 25 */
  DSLHAL_ERROR_MARGIN_API_FAILURE,       /* 26 */
  DSLHAL_ERROR_CTRL_API_FAILURE,         /* 27 */
  DSLHAL_ERROR_HYBRID_API_FAILURE,       /* 28 */
  DSLHAL_ERROR_MODEMENV_API_FAILURE,     /* 29 */
  DSLHAL_ERROR_INTERRUPT_FAILURE,        /* 30 */
  DSLHAL_ERROR_INTERNAL_API_FAILURE,     /* 31 */
  DSLHAL_ERROR_DIGIDIAG_FAILURE,         /* 32 */
  DSLHAL_ERROR_TONETEST_FAILURE,         /* 33 */
  DSLHAL_ERROR_NOISETEST_FAILURE,        /* 34 */
  DSLHAL_ERROR_MODEMSTATE,               /* 35 */
  DSLHAL_ERROR_OVERLAY_CORRUPTED         /* 36 */
};
 
enum 
{
  CONFIG_FLAG_NOFLAG,                    /* 00 */
  CONFIG_FLAG_TRELLIS,                   /* 01 */
  CONFIG_FLAG_EC,                        /* 02 */
  CONFIG_FLAG_RS                         /* 03 */
};

#define USE_EMIF_REV    0
#define USE_CVR_REV     1
#define TNETD53XX_MAXLOOP       10000
#define REVERB          0
#define MEDLEY          1
#define NONINTENSE      0
#define slavespace0 0xa1000000

#define MASK_MAILBOX_INTERRUPTS 0x00000001
#define MASK_BITFIELD_INTERRUPTS 0x00000002
#define MASK_HEARTBEAT_INTERRUPTS 0x00000004
#define DSP_INTERRUPT_SOURCE_REGISTER  0x020007A0
#define DSP_INTERRUPT_CLEAR_REGISTER   0x020007A4

#define DIGITAL_DIAG_MEMSIZE 1048576
#define CRC32_QUOTIENT 0x04c11db7
#define DSP_FIRMWARE_MALLOC_SIZE 0x7ffff
#define DSP_CONSTELLATION_BUFFER_SIZE 1024*4
#define LOG43125 9303
#define US_NOMINAL_POWER (-38)
#define US_BNG_LENGTH 32
#define DS_BNG_LENGTH 256
#define NUMBER_OF_BITFIELDS 4
#define BITFIELD_SCAN 0x80000000

/* ADSL Message offsets from Host Interface Pointer */

/* ADSL2 Messages Index and Length defines */

#define CMSGFMT_INDEX      0
#define CMSGPCB_INDEX      1
#define RMSGFMT_INDEX      2
#define RMSGPCB_INDEX      3
#define RMSG1LD_INDEX     13
#define RMSG2LD_INDEX     14
#define RMSG3LD_INDEX     15
#define RMSG4LD_INDEX     16
#define RMSG5LD_INDEX     17
#define RMSG6LD_INDEX     18
#define RMSG7LD_INDEX     19
#define RMSG8LD_INDEX     20
#define RMSG9LD_INDEX     21
#define CMSG1LD_INDEX     22
#define CMSG2LD_INDEX     23
#define CMSG3LD_INDEX     24
#define CMSG4LD_INDEX     25
#define CMSG5LD_INDEX     26
#define CMSGPCB2_INDEX    28
#define CMSGPCB2L_INDEX   29
#define RMSGFMT2_INDEX    30
#define RMSGPCB2L_INDEX   31
#define CMSG1ADSL2_INDEX  32
#define RMSG1ADSL2_INDEX  33
#define CMSG2ADSL2_INDEX  34
#define RMSG2ADSL2_INDEX  35
#define CPARAMS_INDEX     36
#define RPARAMS_INDEX     37

/* ADSL2 Message Sizes */

#define CMSGFMT_SIZE       2
#define RMSGFMT_SIZE       2
#define CMSGPCB_SIZE       2
#define CMSGPCB2_SIZE      6    /* Annex A with Blackout */
#define CMSGPCB2L_SIZE    10    /* Annex B with Blackout */
#define RMSGPCB_SIZE      36
#define RMSG1LD_SIZE      16
#define RMSGxLD_SIZE     258
#define CMSG1LD_SIZE      16
#define CMSG2LD_SIZE     130
#define CMSG3LD_SIZE      66
#define CMSG4LD_SIZE      34
#define CMSG5LD_SIZE      34
#define CMSG1ADSL2_SIZE   24   
#define RMSG1ADSL2_SIZE    4
#define CMSG2ADSL2_SIZE    4
#define RMSG2ADSL2_SIZE   32
#define CPARAMS_SIZE     136 
#define RPARAMS_SIZE     808

/* ADSL2 Plus Message Sizes (if Different from ADSL2) */

#define RMSGPCB_P_SIZE     68
#define CMSG1ADSL2P_SIZE   40 /* With Blackout */
#define CPARAMS_PA_SIZE   168 
#define RPARAMS_PA_SIZE  2088
#define CPARAMS_PB_SIZE   296
#define RPARAMS_PB_SIZE  2088

#endif /* pairs #ifndef ___DSL_REGISTER_DEFINES_H___ */
