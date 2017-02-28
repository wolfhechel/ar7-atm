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
*    31Mar04     0.01.12            RamP   Fixed length for RMSGPCB2L DELT message
*                                          Increased Max Sections to 400
*    20Apr04     0.01.12            RamP   Increased Max Sections for ADSL2Plus
*
*    29Apr04     0.01.13            RamP   Added indices for ADSL2+ DELT messages
*                                          Fixed length for RMSGPCB2L DELT message
*                                          Fixed index for RMSGPCB2PL message
*    30Jun04     0.02.00            RamP   Added register definitions for Ohio s/w
*    27Jan05     0.02.01            CPH    Added Ohio250 support & cleanup.
*    02Feb05     0.02.02            CPH    Remove DSLHAL_REG8() & DSLHAL_REG16() as
*                                          they are not endian neutral.
*    21Feb05     0.02.03            CPH    Pull included files out from this file so other
*                                          modules can use this file without catch
*    01Apr05     0.02.04            CPH    Added REG_SYSTEM_POSTDIV2 &
*    20Jul05     0.02.05            AV     Added Register definitions for the
*                                          PDCR:Peripheral Power Down Register and
*                                          the ADSLADR1, ADSLADR2 and ADSLADR3 Registers.
*    17June05    0.02.05            CPH    Resync datapump Msg Index (ctrl_msm_decl.h)
*                                          This fixed AnxdB ADSL2/2+ host crash problem.
*    18July05    0.02.05            CPH    CQ9600 Sync Training msg Index with ctrl_msm_decl.h.
*    29July05    0.02.06            CPH    Remove US_BNG_LENGTH & DS_BNG_LENGTH.
*    05Sept05    0.02.07            CPH    Added DSLHAL_ERROR_UNSUPPORTED_MODE & DSLHAL_ERROR_GENERAL_FAILURE.
*******************************************************************************/

//#include <env_def_typedefs.h>

//#ifdef INTERNAL_BUILD
//#include <dev_host_internalinterface.h>
//#endif
//#include <dev_host_interface.h>
//#include <dsl_hal_api.h>

#define ADSLSS_BASE                               0x01000000
#define ADSLSS2_BASE                              0x01800000
#define ADSLSS3_BASE                              0x01c00000


#define BBIF_BASE                                 0x02000000

#define ADSLSSADR                                 (BBIF_BASE+0x0000)
#define ADSLSSADR2                                (BBIF_BASE+0x0004)
#define ADSLSSADR3                                (BBIF_BASE+0x0008)

#define ADSLSSADRMASK                             0xff000000
#define DSP_PMEM_MASK                             0x00000000
#define DSP_DMEM_MASK                             0x80000000
#define WAKEUP_DSP                                0x00000001

enum CHIP_ID {
CHIP_AR7      =0x05,                  /* Sangam  Chip ID */
CHIP_AR7O_212 =0x18,                  /* Ohio212 Chip ID */
CHIP_AR7O_250_212 =0x2B               /* Ohio250/Ohio212(new) Chip ID */
};

//GPIO Registers
#define GPIO_BASE                                 0xA8610900
#define REG_GPIOENR                               (GPIO_BASE + 0x0C)  /* GPIO Enable Register */
#define REG_CVR                                   (GPIO_BASE + 0x14)  /* Chip Version Register */
#define REG_DIDR1                                 (GPIO_BASE + 0x18)  /* Die ID Register (low 32bit) */
#define REG_DIDR2                                 (GPIO_BASE + 0x1C)  /* Die ID Register (high 32bit) */

// PRCR: Peripheral Reset Control Register
#define PRCR_BASE                                 0xA8611600
#define REG_PRCR                                  (PRCR_BASE + 0x00)
// definitions for PRCR
#define PRCR_UART0                                0x00000001
#define PRCR_I2C                                  0x00000002
#define PRCR_GPIO                                 0x00000040
#define PRCR_ADSLSS                               0x00040080
#define PRCR_USB                                  0x00000100
#define PRCR_SAR                                  0x00000200
#define PRCR_DSP                                  0x00800000
#define PRCR_EMAC1                                0x00200000      /* EMAC1 reset */
#define PRCR_ADSLSS_ONLY                          0x00000080

#define ADSLSS_PRCR_BASE                          0xA1000600      /* DSP:0x02000600 */
#define REG_ADSLSS_PRCR                           (ADSLSS_PRCR_BASE + 0x00)
#define ADSLSS_PRCR_DSL_SPA                       0x40

//PDCR: Peripheral Power Down Register
#define PDCR_BASE                                 0xA8610A00
#define REG_PDCR                                  (PDCR_BASE + 0x00)

//Selective Bit definitions for the PDCR Register
#define PDCR_SARP                                 0x00000100 /* SAR Power Down Bit */
#define PDCR_ADSLP                                0x00000200 /* ADSLSS Power Down Bit */
#define PDCR_ADSPP                                0x00001000 /* DSP Power Down Bit */

#define ADSLADR_BASE                              0xA2000000
#define ADSLADR1                                  (ADSLADR_BASE + 0)
#define ADSLADR2                                  (ADSLADR_BASE + 4)
#define ADSLADR3                                  (ADSLADR_BASE + 8)

#define DSP_PRPH_START_ADDR                       0xA1000000
#define DSP_PMEM_START_ADDR                       0xA1800000
#define DSP_DMEM_START_ADDR                       0xA1C00000

/* Ohio Registers */
#define MIPS_PLL_BASE                             0xa8610a80 /* MIPS PLL base address */
#define REG_MIPS_PLLCSR                           (MIPS_PLL_BASE)
#define REG_MIPS_PLLM                             (MIPS_PLL_BASE + 0x10)
#define REG_MIPS_PREDIV                           (MIPS_PLL_BASE + 0x14)
#define REG_MIPS_POSTDIV                          (MIPS_PLL_BASE + 0x18)
#define REG_MIPS_PLLCMD                           (MIPS_PLL_BASE + 0x38)
#define REG_MIPS_PLLSTAT                          (MIPS_PLL_BASE + 0x3c)
#define REG_MIPS_PLLCMDEN                         (MIPS_PLL_BASE + 0x40)

#define DIVEN                                     0x00008000
#define GOSTAT                                    0x00000001
#define PLLEN                                     0x00000001
#define GOSET                                     0x00000001
#define GOSET2                                    0x00000002  /* GOSET for REG_SYSTEM_POSTDIV2 */
#define GOSETEN                                   0x00000001
#define GOSETEN2                                  0x00000002  /* GOSETEN for REG_SYSTEM_POSTDIV2 */


#define SYSTEM_PLL_BASE                           0xa8610b00  /* System PLL base addr  */
#define REG_SYSTEM_PLLCSR                         (SYSTEM_PLL_BASE)
#define REG_SYSTEM_PLLM                           (SYSTEM_PLL_BASE + 0x10)
#define REG_SYSTEM_PREDIV                         (SYSTEM_PLL_BASE + 0x14)
#define REG_SYSTEM_POSTDIV                        (SYSTEM_PLL_BASE + 0x18)
#define REG_SYSTEM_POSTDIV2                       (SYSTEM_PLL_BASE + 0x1C)
#define REG_SYSTEM_PLLCMD                         (SYSTEM_PLL_BASE + 0x38)
#define REG_SYSTEM_PLLSTAT                        (SYSTEM_PLL_BASE + 0x3C)
#define REG_SYSTEM_PLLCMDEN                       (SYSTEM_PLL_BASE + 0x40)

//Clock multiplier to get 212 MHz from MIPS PLL
#define OHIO_MIPS_PLLM_RATIO                      5

// bit definition for REG_SYSTEM_POSTDIV2
#define OHIO_SYS_POSTDIV2_RATIO                   0

// bit definition for REG_SYSTEM_PLLM
#define OHIO_SYS_PLLM_RATIO                       9

#define REG_VSERCLKSELR                           0xa8611a10

// bit definition for REG_VSERCLKSELR
#define OHIO_250_MODE                             0x100

#define REG_PLL_TEST                              0xa8611a0c


/* Interrupt Controller Register */
#define INTC_BASE                                 0xa8612400          /* Interrupt Controller Base addr */
#define REG_INTC_INTSR1                           (INTC_BASE)         /* Interrupt Status/Mask Reg 1 */
#define REG_INTC_INTSR2                           (INTC_BASE + 0x04)  /* Interrupt Status/Mask Reg 2 */
#define REG_INTC_INTCR1                           (INTC_BASE + 0x10)  /* Interrupt Clear/Raw Reg 1*/
#define REG_INTC_INTCR2                           (INTC_BASE + 0x14)  /* Interrupt Clear/Raw Reg 2*/
#define REG_INTC_INTESR1                          (INTC_BASE + 0x20)  /* Interrupt Enable Set Register 1 */
#define REG_INTC_INTESR2                          (INTC_BASE + 0x24)  /* Interrupt Enable Set Register 2 */
#define REG_INTC_INTTYPR1                         (INTC_BASE + 0x60)  /* Interrupt Type Register1 */
#define REG_INTC_INTTYPR2                         (INTC_BASE + 0x64)  /* Interrupt Type Register2 */

/* Codec Registers */
#define CODEC_BASE                                0xa1040000
#define REG_CODEC_CTRL2                           (CODEC_BASE+0x10)   /* CTRL1,CTRL2,CTRL3,CTRL4: CTRL2 is byte1 */
#define REG_BUCKTRIM_PWD                          (CODEC_BASE+0x64)   /* BuckTrimPwd is byte 3 */
#define REG_BUCKTRIM_READ                         (CODEC_BASE+0x18)   /* BuckTrim Read is byte 1(offset 0x19), bit 2:0 */

/* ******************************************************
Interrupt sources on Ax7 interrupt controller.
The reserved sources are noted.
********************************************************* */

#define INTR_CNTRL_SRC_SECOND                     0
#define INTR_CNTRL_SRC_EXTERNAL0                  1
#define INTR_CNTRL_SRC_EXTERNAL1                  2
/* reserved sources ... */
#define INTR_CNTRL_SRC_TIMER0                     5
#define INTR_CNTRL_SRC_TIMER1                     6
#define INTR_CNTRL_SRC_UART0                      7
#define INTR_CNTRL_SRC_UART1                      8
#define INTR_CNTRL_SRC_DMA0                       9
#define INTR_CNTRL_SRC_DMA1                       10
/* reserved sources ... */
#define INTR_CNTRL_SRC_SAR                        15
/* reserved sources ... */
#define INTR_CNTRL_SRC_EMAC0                      19
/* reserved sources ... */
#define INTR_CNTRL_SRC_VLYNQ0                     21
#define INTR_CNTRL_SRC_CODEC_WAKE                 22
/* reserved sources ... */
#define INTR_CNTRL_SRC_USB                        24
#define INTR_CNTRL_SRC_VLYNQ1                     25
/* reserved sources ... */
#define INTR_CNTRL_SRC_EMAC1                      28
#define INTR_CNTRL_SRC_I2C                        29
#define INTR_CNTRL_SRC_DMA2                       30
#define INTR_CNTRL_SRC_DMA3                       31
/* reserved sources ... */
#define INTR_CNTRL_SRC_VDMA_RX                    37
#define INTR_CNTRL_SRC_VDMA_TX                    38
#define INTR_CNTRL_SRC_ADSLSS_SANGAM              39   /* for Sangam */
#define INTR_CNTRL_SRC_ADSLSS_OHIO                23   /* for Ohio   */


#ifndef K0BASE
#define K0BASE                                    0x80000000
#endif

#ifndef K1BASE
#define K1BASE                                    0xA0000000
#endif

#ifndef PHYS_ADDR
#define PHYS_ADDR(X)                              ((X) & 0X1FFFFFFF)
#endif

#ifndef PHYS_TO_K0
#define PHYS_TO_K0(X)                             (PHYS_ADDR(X)|K0BASE)
#endif

#ifndef PHYS_TO_K1
#define PHYS_TO_K1(X)                             (PHYS_ADDR(X)|K1BASE)
#endif

#ifndef DSLHAL_REG32
#define DSLHAL_REG32( addr )                      (*(volatile unsigned int *)PHYS_TO_K1(addr))
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
#ifndef _SIZE_T_
#define _SIZE_T_
typedef unsigned int size_t;
#endif

#define TIDSL_HW_CREATED                          0x00000001
#define TIDSL_HW_OPENED                           0x00000002
#define TIDSL_HW_STARTED                          0x00000004
#define TIDSL_OS_INITIALIZED                      0x00000008

/* Data Pump CRATES Table Defines */
#define SIZE_OF_CRATES1_TABLE                     120
#define CRATES1_BF_LS0                            7
#define CRATES1_BI_LS0                            17
#define CRATES1_BF_AS0                            0
#define CRATES1_BI_AS0                            10
#define CRATES1_BF_DSRS                           20
#define CRATES1_BI_DSRS                           21
#define CRATES1_BFI_DSS                           22
#define CRATES1_BFI_DSI                           23
#define CRATES1_BF_USRS                           25
#define CRATES1_BI_USRS                           26
#define CRATES1_BFI_USS                           27
#define CRATES1_BFI_USI                           28

#define FAST_PATH                                 0
#define INTERLEAVED_PATH                          1

#define LINE_NOT_CONNECTED                        0
#define LINE_CONNECTED                            1
#define LINE_DISCONNECTED                         2
#define LINE_NOT_TO_CONNECT                       3

#define MAXSECTIONS                               400

/*****************************************************************************************
 * Localstructure declarations
 *
 ****************************************************************************************/
enum
{
  DSLHAL_ERROR_NO_ERRORS,                         /* 00 */
  DSLHAL_ERROR_UNRESET_ADSLSS,                    /* 01 */
  DSLHAL_ERROR_RESET_ADSLSS,                      /* 02 */
  DSLHAL_ERROR_UNRESET_DSP,                       /* 03 */
  DSLHAL_ERROR_RESET_DSP,                         /* 04 */
  DSLHAL_ERROR_NO_FIRMWARE_IMAGE,                 /* 05 */
  DSLHAL_ERROR_MALLOC,                            /* 06 */
  DSLHAL_ERROR_FIRMWARE_MALLOC,                   /* 07 */
  DSLHAL_ERROR_DIAG_MALLOC,                       /* 08 */
  DSLHAL_ERROR_OVERLAY_MALLOC,                    /* 09 */
  DSLHAL_ERROR_CODE_DOWNLOAD,                     /* 10 */
  DSLHAL_ERROR_DIAGCODE_DOWNLOAD,                 /* 11 */
  DSLHAL_ERROR_BLOCK_READ,                        /* 12 */
  DSLHAL_ERROR_BLOCK_WRITE,                       /* 13 */
  DSLHAL_ERROR_MAILBOX_READ,                      /* 14 */
  DSLHAL_ERROR_MAILBOX_WRITE,                     /* 15 */
  DSLHAL_ERROR_MAILBOX_NOMAIL,                    /* 16 */
  DSLHAL_ERROR_MAILBOX_OVERFLOW,                  /* 17 */
  DSLHAL_ERROR_INVALID_PARAM,                     /* 18 */
  DSLHAL_ERROR_ADDRESS_TRANSLATE,                 /* 19 */
  DSLHAL_ERROR_FIRMWARE_CRC,                      /* 20 */
  DSLHAL_ERROR_FIRMWARE_OFFSET,                   /* 21 */
  DSLHAL_ERROR_CONFIG_API_FAILURE,                /* 22 */
  DSLHAL_ERROR_EOCREG_API_FAILURE,                /* 23 */
  DSLHAL_ERROR_VERSION_API_FAILURE,               /* 24 */
  DSLHAL_ERROR_STATS_API_FAILURE,                 /* 25 */
  DSLHAL_ERROR_MARGIN_API_FAILURE,                /* 26 */
  DSLHAL_ERROR_CTRL_API_FAILURE,                  /* 27 */
  DSLHAL_ERROR_HYBRID_API_FAILURE,                /* 28 */
  DSLHAL_ERROR_MODEMENV_API_FAILURE,              /* 29 */
  DSLHAL_ERROR_INTERRUPT_FAILURE,                 /* 30 */
  DSLHAL_ERROR_INTERNAL_API_FAILURE,              /* 31 */
  DSLHAL_ERROR_DIGIDIAG_FAILURE,                  /* 32 */
  DSLHAL_ERROR_TONETEST_FAILURE,                  /* 33 */
  DSLHAL_ERROR_NOISETEST_FAILURE,                 /* 34 */
  DSLHAL_ERROR_MODEMSTATE,                        /* 35 */
  DSLHAL_ERROR_OVERLAY_CORRUPTED,                 /* 36 */
  DSLHAL_ERROR_UNSUPPORTED_MODE,                  /* 37 */
  DSLHAL_ERROR_GENERAL_FAILURE,                   /* 38 */
};

enum
{
  CONFIG_FLAG_NOFLAG,                             /* 00 */
  CONFIG_FLAG_TRELLIS,                            /* 01 */
  CONFIG_FLAG_EC,                                 /* 02 */
  CONFIG_FLAG_RS                                  /* 03 */
};

#define USE_EMIF_REV                              0
#define USE_CVR_REV                               1
#define TNETD53XX_MAXLOOP                         10000
#define REVERB                                    0
#define MEDLEY                                    1
#define NONINTENSE                                0
#define slavespace0                               0xa1000000

#define MASK_MAILBOX_INTERRUPTS                   0x00000001
#define MASK_BITFIELD_INTERRUPTS                  0x00000002
#define MASK_HEARTBEAT_INTERRUPTS                 0x00000004
#define DSP_INTERRUPT_SOURCE_REGISTER             0x020007A0
#define DSP_INTERRUPT_CLEAR_REGISTER              0x020007A4

#define DIGITAL_DIAG_MEMSIZE                      1048576
#define CRC32_QUOTIENT                            0x04c11db7
#define DSP_FIRMWARE_MALLOC_SIZE                  0x7ffff
#define DSP_CONSTELLATION_BUFFER_SIZE             (1024*4)
#define LOG43125                                  9303
#define US_NOMINAL_POWER                          (-38)
//#define US_BNG_LENGTH                             32
//#define DS_BNG_LENGTH                             256
#define NUMBER_OF_BITFIELDS                       4
#define BITFIELD_SCAN                             0x80000000

/* ADSL Message offsets from Host Interface Pointer */

/* ADSL2 Messages Index and Length defines */

/*
 * !!Important:
 *      These definition must match the same definition of
 *      ctrl_msm_decl.h in datapump.
*/
#if 1 //cph, sync for RP5 changes
#define CRATES1_INDEX      0
#define CMSGS1_INDEX       1
#define RRATES1_INDEX      2
#define RMSGS1_INDEX       3
#define RMSGS2_INDEX       4
#define RRATES2_INDEX      5
#define CMSGS2_INDEX       6
#define CRATES2_INDEX      7
#define CBNG_INDEX         8
#define RBNG_INDEX         9
#define RMSGSRA_INDEX     10
#define RRATESRA_INDEX    11
#define CMSGSRA_INDEX     12
#define CRATESRA_INDEX     0

#define CMSGFMT_INDEX      0 // Same as CRATES1
#define CMSGPCB_INDEX      1 // Same as CMSGS1
#define RMSGFMT_INDEX      2 // Same as RRATES1
#define RMSGPCB_INDEX      3 // Same as RMSGS1

#define RMSG1LD_INDEX     13
#define RMSG2LD_INDEX     14
#define RMSG3LD_INDEX     15
#define RMSG4LD_INDEX     16
#define RMSG5LD_INDEX     17
#define RMSG6LD_INDEX     18
#define RMSG7LD_INDEX     19
#define RMSG8LD_INDEX     20
#define RMSG9LD_INDEX     21
#define RMSG10LD_INDEX    22
#define RMSG11LD_INDEX    23
#define RMSG12LD_INDEX    24
#define RMSG13LD_INDEX    25
#define RMSG14LD_INDEX    26
#define RMSG15LD_INDEX    27
#define RMSG16LD_INDEX    28
#define RMSG17LD_INDEX    29
#define CMSG1LD_INDEX     30
#define CMSG2LD_INDEX     31
#define CMSG3LD_INDEX     32
#define CMSG4LD_INDEX     33
#define CMSG5LD_INDEX     34
#define RMSGACK_INDEX     35
#define CMSGPCB2_INDEX    36 // Same as CMSGS1
#define CMSGPCB2L_INDEX   37 // Same as CMSGS1
#define RMSGFMT2_INDEX    38 // Same as RMSGS1
#define RMSGPCB2L_INDEX   39 // Same as RRATES1
#define CMSG1ADSL2_INDEX  40
#define RMSG1ADSL2_INDEX  41
#define CMSG2ADSL2_INDEX  42
#define RMSG2ADSL2_INDEX  43
#define CPARAMS_INDEX     44
#define RPARAMS_INDEX     45

#define CMSG1ADSL2P_INDEX 46
#define RMSG2ADSL2P_INDEX 47
#define CPARAMSPL_INDEX   48
#define RPARAMSPL_INDEX   49
#define RMSGPCB2PL_INDEX  50 // Same as CRATES1


#else
#define CMSGFMT_INDEX                             0
#define CMSGPCB_INDEX                             1
#define RMSGFMT_INDEX                             2
#define RMSGPCB_INDEX                             3
#define RMSG1LD_INDEX                             13
#define RMSG2LD_INDEX                             14
#define RMSG3LD_INDEX                             15
#define RMSG4LD_INDEX                             16
#define RMSG5LD_INDEX                             17
#define RMSG6LD_INDEX                             18
#define RMSG7LD_INDEX                             19
#define RMSG8LD_INDEX                             20
#define RMSG9LD_INDEX                             21
#define CMSG1LD_INDEX                             22
#define CMSG2LD_INDEX                             23
#define CMSG3LD_INDEX                             24
#define CMSG4LD_INDEX                             25
#define CMSG5LD_INDEX                             26
#define CMSGPCB2_INDEX                            28
#define CMSGPCB2L_INDEX                           29
#define RMSGFMT2_INDEX                            30
#define RMSGPCB2L_INDEX                           31
#define CMSG1ADSL2_INDEX                          32
#define RMSG1ADSL2_INDEX                          33
#define CMSG2ADSL2_INDEX                          34
#define RMSG2ADSL2_INDEX                          35
#define CPARAMS_INDEX                             36
#define RPARAMS_INDEX                             37
#define CMSG1ADSL2P_INDEX                         38
#define RMSG2ADSL2P_INDEX                         39
#define CPARAMSPL_INDEX                           40
#define RPARAMSPL_INDEX                           41
#define RMSGPCB2PL_INDEX                          42
#define RMSG10LD_INDEX                            43
#define RMSG11LD_INDEX                            44
#define RMSG12LD_INDEX                            45
#define RMSG13LD_INDEX                            46
#define RMSG14LD_INDEX                            47
#define RMSG15LD_INDEX                            48
#define RMSG16LD_INDEX                            49
#define RMSG17LD_INDEX                            50

#endif

/* ADSL2 Message Sizes */

#define CMSGFMT_SIZE                                2
#define RMSGFMT_SIZE                                2
#define CMSGPCB_SIZE                                2
#define CMSGPCB2_SIZE                               6    /* Annex A with Blackout */
#define CMSGPCB2L_SIZE                             10    /* Annex B with Blackout */
#define RMSGPCB_SIZE                               38
#define RMSGPCB2L_SIZE                             36
#define RMSG1LD_SIZE                               16
#define RMSGxLD_SIZE                              258
#define CMSG1LD_SIZE                               16
#define CMSG2LD_SIZE                              130
#define CMSG3LD_SIZE                               66
#define CMSG4LD_SIZE                               34
#define CMSG5LD_SIZE                               34
#define CMSG1ADSL2_SIZE                            24
#define RMSG1ADSL2_SIZE                             4
#define CMSG2ADSL2_SIZE                             4
#define RMSG2ADSL2_SIZE                            32
#define CPARAMS_SIZE                              136
#define RPARAMS_SIZE                              808

/* ADSL2 Plus Message Sizes (if Different from ADSL2) */
#define RMSGPCB2PL_SIZE                            68  /* Missing an Index */
#define CMSG1ADSL2P_SIZE                           40  /* With Blackout */
#define RMSG2ADSL2P_SIZE                           64
#define CPARAMSPL_SIZE                             168
#define RPARAMSPL_SIZE                            2088

/*
#define CPARAMS_PB_SIZE   296
#define RPARAMS_PB_SIZE  2088
*/

/*
 * Define Bit Masks
 */
#define BIT0    0x00000001            // Bit masks
#define BIT1    0x00000002
#define BIT2    0x00000004
#define BIT3    0x00000008
#define BIT4    0x00000010
#define BIT5    0x00000020
#define BIT6    0x00000040
#define BIT7    0x00000080
#define BIT8    0x00000100
#define BIT9    0x00000200
#define BIT10   0x00000400
#define BIT11   0x00000800
#define BIT12   0x00001000
#define BIT13   0x00002000
#define BIT14   0x00004000
#define BIT15   0x00008000
#define BIT16   0x00010000
#define BIT17   0x00020000
#define BIT18   0x00040000
#define BIT19   0x00080000
#define BIT20   0x00100000
#define BIT21   0x00200000
#define BIT22   0x00400000
#define BIT23   0x00800000
#define BIT24   0x01000000
#define BIT25   0x02000000
#define BIT26   0x04000000
#define BIT27   0x08000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000


#endif /* pairs #ifndef ___DSL_REGISTER_DEFINES_H___ */






















