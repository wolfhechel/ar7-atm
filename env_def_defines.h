#ifndef __ENV_DEF_DEFINES_H__
#define __ENV_DEF_DEFINES_H__ 1

//********************************************************************
//*    DMT-BASE ADSL MODEM PROGRAM
//*    TEXAS INSTRUMENTS PROPRIETARTY INFORMATION
//*    AMATI CONFIDENTIAL PROPRIETARY
//*
//* (c) Copyright May 1999, Texas Instruments Incorporated.
//*     All Rights Reserved.
//*
//*   Property of Texas Instruments Incorporated and Amati Communications Corp.
//*
//* Restricted Rights - Use, duplication, or disclosure is subject to
//* restrictions set forth in TI's and Amati program license agreement and
//* associated documentation
//*
//*********************************************************************
//*
//* FILENAME: env_def_defines.h
//*
//* ABSTRACT: This file provides a mechanism for defining standard
//*           preprocessor flag sets for datapump.
//*
//* TARGET:  Non specific.
//*
//* TOOLSET:  Non specific.
//*
//* ACTIVATION:
//*
//* HISTORY:  DATE        AUTHOR         DESCRIPTION
//*           05/11/99    FLW            Created
//*           01/21/00    FLW            derfmake changes
//*           02/07/00    FLW/M.Seidl    added _debug targets
//*           02/23/00    FLW/M.Barnett  merged in from G.lite branch
//*           03/03/00    FLW/M.Barnett  added TRELLIS token
//*           05/11/00    Barnett        Added ap036btp & ap037btp support.
//*           02/07/00    M.Seidl        added HW_TEQ, HW_DEC and DC_BIAS tokens
//*           02/28/00    Derf/Seidl     Mod's in support of suppressing compiler warnings for empty source files
//*           02/28/00    M. Seidl       added SWTC token
//*           02/28/00    M. Seidl       added ap0501tp and its _debug target
//*           03/03/00    M. Seidl       added MODPILOT token, ap0307tp_debug has SWDI and FDD enabled
//*           03/08/00    Jack Huang     Added HWRSDEC token
//*           03/29/00    Derf/Seidl     Mod's in support Soft-TC and derfmake
//*           04/10/00    Jack Huang     Added PCIMASTER token
//*           04/11/00    Jack Huang     Added STBFIX token
//*           04/24/00    M. Seidl       ap0500tp has OLAYDP turned on, ap0500tp_debug has OLAYDP turned off now
//*           04/28/00    M.Seidl        added KAPIL token for ap0501tp targets
//*           06/02/00    P.Sexton       Added OAM_EOC token
//*           06/06/00    M. Seidl       added au0501tp target
//*           06/12/00    Michael Seidl  activated h/w TEQ for KAPIL targets, disabled STBFIX for all targets
//*           06/14/00    Derf           added ap0308tp, alphabetized token lists
//*           06/16/00    Jack Huang     added au0501tp_debug target
//*           06/22/00    M. Seidl       Enabled code overlays through EMIF from external memory. Now there are 3
//*                                      different code overlay schemes that are differentiated by the following tokens:
//*                                      OLAYDP_PCI:  uses PCI DMA in PCI Master Mode
//*                                      OLAYDP_HOST: Host downloads overlay code into DMEM upon request from DSP
//*                                      OLAYDP_EMIF: Host downloads overlay code into ext. mem at boot-time. DSP uses
//*                                                 DMA channel 0 to load code into on-chip PMEM
//*                                      OLAYDP:    turns overlays generally on or off
//*           07/15/00    B.Srinivasan   Cleaned up default tokens and tokens for au0501tp, au0501tp_debug,
//*                                      ap0501tp and ap0501tp_debug targets as well
//*           07/19/00    B.Srinivasan   Made changes w.r.t cleaning/fixing Rufus Interrupt related code
//*                                      (changed token TC_INTRPT_RUFUS to INTRPT_RUFUS
//*           06/14/00    Derf           added au0501cp
//*           08/12/00    Yinong Ding    Added DPLL_MODE token.
//*           08/16/00    F. Mujica      Moved DOUBLE_XMT_RATE, RX_HPF, and LEAKY_LMS tokens here.
//*           09/05/00    M. Seidl       Changed ap0501tp back to do overlay through PCI (OLAY_PCI = 1)
//*           09/11/00    M. Seidl       moved USB and OLAYDP_HOST definitions to be CHIPSET specific, not target specific
//*           09/21/00    U.Dasgupta/    Added a token for separate transmit and receive buffers.
//*                       F.Mujica       (SEPARATE_TX_RX_BUFFERS).
//*           10/02/00    U.Dasgupta/    Added DPLL and SEPARATE_TX_RX_BUFFER tokens for au0502 target.
//*                       F.Mujica
//*           10/02/00    M. Castellano  Added new tokens for new Host/EMIF DSP code overlay.
//*                                      OLAYDP_2STEP: Host downloads overlay code into ext. mem upon DSP request.  DSP
//*                                      DMA channel 0 to load code into on-chip PMEM.  It is a somewhat hybrid version
//*                                      of OLAYDP_HOST and OLAYDP_EMIF.  The test target is ap0502tp.
//*           10/25/00    Balaji         Added tokens for arv500tp target
//*           10/31/00    U. Iyer        Added TEQ_AVG token and set it one for Ax5
//*           11/02/00    Barnett        Added OAM_EOC=0 in defaults at bottom of file.
//*           11/21/00    M. Seidl       turned OLAYDP and AOC off for ap0307tp to make binary compatible w. whql'ed driver
//*           11/28/00    Paul Hunt      added FASTRETRAIN token for fast retrain specific code
//*           11/28/00    Paul Hunt      added PILOTDATA token to control channel estimation and transmission
//*                                      of data on the upstream pilot tone for ITU standard code loads
//*           12/20/00    Jack Huang     added EIGHTBITSRAM toekn for the targets that use 8-bit SRAM for
//*                                      interleaver/deinterleaver
//*           01/11/01    U.Dasgupta     Added mp targets and cleaned up GHS/PILOTDATA variables
//*           01/23/01    U.Dasgupta     Cleaned up code within GLITE tokens
//*           01/23/00    Barnett        Added in full EOC support for AP3/Ax5.  Got rid of UTC.  No longer needed.
//*           02/08/01    Barnett        Added DDC token.
//*           03/09/01    Nirmal Warke   Added in TOKEN definition for line diagnostics (LINE_DIAG): IMP DEPENDENCIES -
//*                                      TEQ_AVG must be off when LINE_DIAG is on (since they share a union buffer at the same time)
//*           03/13/01    M. Seidl       Recovered ap0500tp target. Added GHS token as PMEM saving option
//*           03/14/01    Barnett        Added ap0500mb target support.  Look-and-feel is similar to ap0500tp.
//*                                      Added seperate-but-equal def's wrt. ISDN Annex B, Annex C, and PROP.
//*           03/14/01    J. Bergsagel   Added EXTERNBERT token.
//*           03/15/01    Barnett/Balaji Merged AR5(H) -> 03.00.00 datapump.
//*           03/16/01    Nirmal Warke   Added in TOKEN definition for crosstalk performance mods (CROSSTALK): IMP DEPENDENCIES -
//*                                      TEQ_AVG must be off and LEAKY_LMS must be on when CROSSTALK is on
//*           03/21/01    Barnett        Added support for ar0500lp, ar0500mp, ar0500dp, arv500lp, arv500mp, and arv500dp.
//*           03/26/01    M. Seidl       enabled 64pt IFFT for ap0500mb (Raptor+AD11, FDM)
//*           03/28/01    J. Bergsagel   Removed EXTERNBERT token (now use host intf. var. instead)
//*           04/03/01    J. Bergsagel   Removed condition of DSPDP_CHIPSET_GEN==5 for default defines
//*                                      Removed LEAKY_LMS token (assumed always 1)
//*                                      Removed OLAYDP_HOST token (assumed always 0)
//*                                      Removed RX_HPF token (assumed always 1)
//*                                      Removed TRIBRID token (not used any more)
//*                                      Removed FDD token (assumed always 1)
//*                                      Removed HW_DEC token (assumed always 1)
//*                                      Removed HW_TEQ token (assumed always 1)
//*                                      Removed HWRSDEC token (assumed always 1)
//*                                      Removed ILEC_AD11_ALCATEL337 token (assumed always 0)
//*                                      Removed ILEC_AD11_HDSLNOISEFIX token (assumed always 0)
//*                                      Removed ILEC_AD11_MODULATEPILOT token (assumed always 0)
//*                                      Removed ILEC_AD11_NEWINTERPOLATE token (assumed always 0)
//*                                      Removed ILEC_AD11_USTXHPF token (assumed always 0)
//*                                      Removed SWDI token (assumed always 1)
//*                                      Removed TD_AGC token (assumed always 1)
//*                                      Removed DSPDP_LEGACY_TARGET token (assumed always 0)
//*                                      Removed AD11_20, AD11_20NL and AD11_20_NEWPREC token (always 1)
//*                                      Removed AI token (assumed always 1)
//*                                      Removed ATUC token (assumed always 0)
//*                                      Removed EU token (assumed always 0)
//*                                      Removed EVM2 token (assumed always 0)
//*                                      Removed INTRPT_RUFUS token (assumed always 0)
//*                                      Removed MODPILOT token (assumed always 0)
//*                                      Removed SL and SL_EVM tokens (assumed always 0)
//*                                      Removed STBIFX token (assumed always 0)
//*                                      Removed STD token (assumed always 1)
//*                                      Removed SWDI_LOOPBACK token (assumed always 0)
//*                                      Removed TID token (assumed always 0)
//*                                      Removed TII token (assumed always 1)
//*                                      Removed TIPCI token (assumed always 1)
//*                                      Removed UDI token (assumed always 1)
//*                                      Removed DC_BIAS token (assumed always 1)
//*           04/05/01    Barnett        Added DSPDP_ prefix to tokens that originate
//*                                      in the public interface.
//*           05/07/01    Jack Huang     Removed DOUBLE_XMT_RATE token.
//*           05/16/01    Barnett        Added back in EXTERNBERT token in support
//*                                      of saving PMEM.
//*           06/05/01    Jack Huang     Fixed the rules for ar0500mp_debug target
//*           04/23/01    M. Halleck     Merge Astro Wu's DDC enhancements
//*           06/05/01    M. Capps       Changed DSP_DEBUG to ENHANCED_SERIAL_DEBUG
//*           07/03/01    M. Capps       Replaced ESD token with !DDC, added DEV_DEBUG
//*           06/26/01    J. Bergsagel   Removed all the old ap03... stuff
//*                                      Removed OLAYDP_HOST token (again)
//*                                      Removed CROSSTALK token (assumed always 1)
//*                                      Removed TEQ_AVG token (assumed always 0)
//*                                      Removed DE token (assumed always 1)
//*                                      Removed PVAT token and au0501cp target
//*                                      Removed FASTRETRAIN token (assumed always 0)
//*           07/05/01    J. Bergsagel   Changed PCIMASTER token to TC_ATM_PCIMASTER
//*           07/20/01    Umesh Iyer     Added ATMBERT token. ATMBERT is conditional on SWTC definition. if SWTC is 0
//*                                      ATMBERT should be 0. Else it can be 0/1. Default 0.
//*           07/23/01    J. Bergsagel   Changed name from defines_u.h to dpsys_defines.h
//*           07/24/01    Barnett        Added support for build of $(TARGET)_diag mfgr'ing targets.
//*           08/02/01    Michael Seidl  renamed KAPIL token to !AD1X
//*           08/02/01    Michael Seidl  renamed GHS token to PMEMSAVE_GHS
//*           08/03/01    S.Yim          Added MFGR_DIAG token for afe diagnostic
//*                                      Added AFEDEV token for afe device driver
//*                                      Added DSPBIOSII token for dsp bios
//*           09/21/01    Sameer         Enable EXTERNBERT.  Disable ATMBERT.
//*           10/01/01    U.Dasgupta     Turned off SMART_MARGIN for ap0500mb because of FECs/CRCs;
//*           10/09/01    Barnett        Added support for ar0500db.
//*           10/12/01    Barnett        Disable EXTERNBERT.
//*           10/15/01    Barnett        Turn off SMART_MARGIN.
//*           11/07/01    Barnett        Def'ed ISDN_DEBUG for all targets to avoid compiler warnings.
//*                                      Assumed defaul value is zero.
//*           11/13/01    Barnett        Reworked ar0500db_debug to build JTAG-bootable load.
//*                                      The equivalent production target should only be flash-bootable.
//*           01/11/02    Yim            Added TOKEN JTAG to build JTAG load ar0500db_diag.
//*           01/23/02    U Iyer         Added DEBUG_LOG token. Default value 0
//*           01/31/02    Barnett        Added support for ar0700mp target.
//*           02/04/02    S.Yim          Added TOKEN JTAG to build JTAG load ar0500mp_diag
//*           02/11/02    U Iyer         Added MARGIN_DELTA_RETRAIN token. Default value 1
//*           05/15/02    Sameer V       Enabled EXTERNBERT token for AR5 and AU5 platforms. EXTERNBERT is
//*                                      not supported on AR5H.
//*           02/14/02    Barnett        Don't ref the SWTC feature token if mfgr'ing diag target.
//*           02/19/02    Yim            Added support to build au0502db_diag target.
//*           03/08/02    Nirmal Warke   Added feature token HYBRID_SWITCH
//*           03/15/02    U G Jani       Turned ON Bitswap support for AU5I (au0502db) targets.
//*           04/08/02    U G Jani       Enabled NLNOISEADJSNR token for AU5I targets.
//*           06/24/02    Seungmok Oh    Added PERTONE_EQ token support for those targets:
//*                                      (ar0500mp_debug, au0502mp_debug, ar0500mp, au0502mp)
//*           06/26/02    Mallesh        Added DS_PWR_CUTBACK token. Default value 1.
//*           06/27/02    Mallesh        Changed default value of DS_PWR_CUTBACK token to 0.
//*           06/29/02    U.Dasgupta     Token cleanup: Removed ISDN_DEBUG token
//*           04/29/02    Mannering      Remove EIGHTBITSRAM, Combined DOUBLEBUFFER with
//*                                      BITSWAP, added FPGA token
//*           05/03/02    Mannering      cleanup token changed by the new routine names
//*           05/06/02    Mannering      Add tokens OUTBAND and INBAND for codec commands
//*                                      If both OUTBAND and INBAND are 0 codec register are
//*                                      memory mapped.
//*           05/29/2002  S.Yim          Removed AD1X, AFEDEV
//*           08/31/2002  Paul Hunt      Added PERTONE_EQ and HYBRID_SWITCH for ar0700mp
//*           09/12/2002  Paul Hunt      Added support for ar0700db target.
//*           08/07/2002  U.Dasgupta     Turned off MARGIN_DELTA_RETRAIN feature for ISDN platforms
//*           11/14/2002  Paul Hunt      Merged AX5 MR6 PC modifications into AR7 codebase, specifically
//*                                      turned off MARGIN_DELTA_RETRAIN feature for ar0700db target
//*           08/26/2002  N. Warke       Added DUAL_TEQ token. Active only for Ax7 target
//*           09/26/2002  Mannering      Add token CODEC_EMU for codec emulator board
//*           10/15/2002  Iyer/Molla     Added DMT_BIS token for DELT support
//*           10/21/2002  A. Redfern     Added PHY_EC_ENABLE and PHY_PATH_ENABLE tokens
//*           10/23/2002  A. Redfern     Removed LINE_DIAG token
//*           10/28/2002  J. Bergsagel   Cleaned up old targets and cleaned up the token list
//*           10/30/2002  A. Redfern     Added PHY_TDW_ENABLE
//*           11/01/2002  A. Redfern     Removed SMART_MARGIN token
//*           11/01/2002  Mustafa        Turned on SPECTRAL_SHAPING features for Lucent AnyMedia O.69 Interop.
//*           11/15/2002  Yim/Mannering  Added CODEC_EMU token for analog emulation board specifics
//*           11/15/2002  Iyer/Molla     Added DEBUG_DELT and MEM_STR token to support DELT debug
//*           12/27/2002  Sameer V       Added ATM_TC_HW token for Sangam.
//*           01/06/2003  J. Bergsagel   Added default values for NLNOISEADJSNR, ARTT and DS_PWR_CUTBACK
//*           01/07/2003  S.Yim          Modified ar0700db_diag target to turn on ISDN token
//*           01/22/2003  J. Bergsagel   Added back in defines for the debug targets.
//*           01/21/2003  MCB            Implemented Ax7 UNIT-MODULE modular software framework.
//*           01/31/2003  J. Bergsagel   Made debug targets to be for the FPGA platform; non-debug for Sangam.
//*                                      Turned off DUAL_TEQ, PHY_EC_ENABLE and PHY_PATH_ENABLE by default
//*                                      for the Sangam (non-debug) targets.
//*                                      Turned off OLAYDP token by default.
//*                                      Turned off SWTC and turned on ATM_TC_HW by default for Sangam targets.
//*           01/29/2003  Sameer V       Added ATMBERT_HW token for Sangam.
//*           02/04/2003  D. Mannering   Added CO_PROFILE token
//*           02/19/2003  Sameer V       Added back EXTERNBERT token for ar0700mp_dp and ar0700db_dp targets.
//*                                      Disabled EXTERNBERT for debug target since it is not supported on the
//*                                      FPGA platform.
//*           02/21/2003  A. Redfern     Turned off OAM_EOC, AOC and BITSWAP (until memory issues are resolved).
//*                                      Turned on DUAL_TEQ, PHY_PATH_ENABLE and PHY_EC_ENABLE.
//*           02/21/2003  D. Mannering   Added DEBUG_DUMP.
//*           03/06/2003  J. Bergsagel   Cleaned up tokens for each target and switched diag targets
//*                                      over to the Sangam platform (instead of the FPGA platform).
//*           03/07/2003  J. Bergsagel   Cleaned up TC and BERT tokens
//*           03/11/2003  J. Bergsagel   Turned on AOC and BITSWAP for Sangam POTS and ISDN targets.
//*           03/20/2003  Mallesh        Added SHALF token.
//*           03/24/2003  F. Mujica      Enable hybrid selection
//*           03/26/2003  A. Redfern     Removed PMEMSAVE_GHS (always = 1).
//*           04/08/2003  F. Mujica      Renamed HYBRID_SWITCH token to PHY_HYB_ENABLE
//*           04/08/2003  J. Bergsagel   Turned off PHY_HYB_ENABLE for _debug targets (FPGA board builds)
//*           04/09/2003  J. Bergsagel   Turned on OLAYDP only for ar0700mp and ar0700db.
//*                                      Turned on AOC, BITSWAP, and OAM_EOC for ar0700mp and ar0700db.
//*           04/09/2003  J. Bergsagel   Corrected name "PHY_HYB_SELECT" to "PHY_HYB_ENABLE"
//*           04/15/2003  A. Redfern     Turned on ECI_PULSECOM_INTEROP because phase optimization was enabled.
//*           05/05/2003  D. Mannering   Review Comments - turn on AOC, EXTERNBERT, SHALF, for ar0700mp; amd
//*                                      turn on AOC for ar0700db
//*           05/11/2003  Prashant S     Added DMT_BIS token for AR7 Soft DI work
//*           05/13/2003  J. Bergsagel   Turned on IMPROVED_STAT_SUPPORT_06_03_00 by default for necessary statistics
//*           05/15/2003  J. Bergsagel   Turned off CO_PROFILE for diag targets.
//*           05/27/2003  U.Dasgupta     Added NLNOISEADJSNR_EC token for ISDN - to take care of non-linear noise
//*                                      (from ISDN splitter) compensation.
//*           06/02/2003  Z. Yang        Added PHY_NDIAG_ENABLE token.
//*           06/02/2003  Z. Yang        Added COMB_LINEDIAG_ENABLE token.
//*           06/05/2003  P. Hunt        Turned on ATUC_CLEARDOWN_CHANGE token for all targets.
//*           06/05/2003  Mallesh        Turned on CENTILLIUM_VENDORID_AND_TXRATE_CHNG to enable logging the vendor ID
//*                                      for centillium and litespan
//*           06/05/2003  U.Dasgupta     Turned on SHALF token for ISDN.
//*           06/06/2003  U.Dasgupta     Turned on G.hs nonstandard field token for ar0700db target.
//*           06/12/2003  J. Bergsagel   Changed *_debug targets to be for JTAG=1 instead of FPGA targets
//*                                      IMPORTANT: For non-JTAG cases, "JTAG" should be undefined.
//*                                      Therefore, "JTAG" should not have a default case at the bottom (JTAG 0)
//*           06/18/2003  A. Redfern     Turned on spectral shaping token for all targets.
//*           06/23/2003  J. Bergsagel   Turned off GHS_NON_STD_FIELD token for ar0700db until bugs are fixed.
//*           06/23/2003  U G Jani       Undid the above change since the bug is fixed.
//*           06/27/2003  Mallesh        Removed all the interop tokens which are no longer required.
//*           08/20/2003  J. Bergsagel   Added default value for OVHD_PMDTEST_PARA and put default section
//*                                      tokens in alphabetical order.
//*           10/09/2003  Hanyu Liu      Defined token ADSL2_1BIT_TONE to support Rx one bit constellation.
//*           10/12/2003  Hanyu Liu      Defined token ADSL2_BSWP for bitswap.
//*           10/20/2003  Xiaohui Li     Added READSL2_ENABLE token.
//*           11/10/2003  Venkatraman    Added ADSL2Plus changes from TI Bangalore.
//*           12/01/2003  Seungmok Oh    Added TXDF2B_PROFILING token, which is active only for POTS target.
//*           12/09/2003  Jack Huang     Turned on GHS_NON_STD_FIELD support for AR7 POTS targets
//*           12/16/2003  Mustafa T.     Added the necessary definitions for diag target.
//*           02/04/2004  Sumeer Bhatara Removed OVHD_PMDTEST_PARA token.
//*           02/04/2004  Mannering      Added token OHIO_SUPPORT.
//*           03/16/2004  Mannering      Added token CLI to support Ohio single serial port
//*           04/30/2004  Hanyu Liu      Added token ADSL2_GhsRA_DBG to enable different level of CLI for ADSL2/2+ RA.
//*           07/16/2004  Mallesh        Enabled ADSL2_1BIT_TONE token to enable support for 1 bit tones
//*           08/10/2004  Brian Z.       Enabled OLAYDP and DMT_Bis for AFE Diag
//*           11/01/2004  Sameer V       Enabled CO_PROFILES for ar0700mp_diag
//*           10/04/2004  Thomas Leyrer  Added token for rea-time trace (RTT) support
//*           11/19/2004  J. Bergsagel   Turned on the OHIO_SUPPORT token by default
//*           03/24/2005  Venkat R       Removed COMB_LINEDIAG_ENABLE as it was always set to 0
//*           03/24/2005  Venkat R       Removed DDC Token as it was always set to 0
//*                                      Removed DPLL_MODE token as it was always 0
//*                                      Removed PHY_EC_ENABLE (Set to 1) Token
//*                                      Removed PHY_HYB_ENABLE (Set to 1) Token
//*                                      Removed PHY_NDIAG_ENABLE (Set to 0) Token
//*                                      Removed PHY_PATH_ENABLE (Set to 1) Token
//*           03/30/2005  Venkat R       Removed DMT_BIS Enable (Set to 1) Token
//*                                      Removed DS_PWR_CUTBACK (Set to 0) Token
//*                                      Removed ARTT (Set to 0) Token
//*                                      Removed LINE_DIAG (Set to 1) Token
//*                                      Removed MANUFACTURING_TESTS (Set to 0) Token
//*                                      Removed GHS_NON_STD_FIELD (Set to 1) Not used anywhere Token\
//*                                      Removed NLNOISEADJSNR (Set to 0) Token
//*           04/01/2005  Venkat R       Removed CO_PROFILE (Set to 1) Token
//*                                      Removed SPECTRAL_SHAPING (Set to 1) Token
//*                                      Removed SEPARATE_TX_RX_BUFFERS (Set to 0) Token
//*                                      Removed TRELLIS (Set to 1) Token
//*           04/04/2005  Venkat R       Removed READSL2_ENABLE (Set to 1) Token
//*                                      Removed USB (Set to 0) Token
//*                                      Removed TC_ATM_PCIMASTER (Set to 0) Token
//*           04/07/2005  Venkat R       Removed CLI (Set to 1) Token
//*                                      Removed DUAL_TEQ (Set to 1) Token
//*           05/06/2005  Mike Locke     Remove DBG_CLI_MEMEORY_ACCESS token CQ9583
//*           05/23/2005  Venkat R       Synchronize Tokens from ar0700mp to ar0700db
//*           05/27/2005  Kapil Gulati   Disable ADSL2_BSWP token for ISDN diagnostics build similar
//*                                      similar to what is done for POTS diagnostics build.
//*           06/08/2005  Yi Han         Added token CQ_9618 to disable the change
//*           06/16/2005  Hanyu Liu      Removed CQ_9618 token. Enabled CQ9618 code.
//*           08/08/2005  Kapil Gulati   CQ9600: Enabled NLNOISEADJSNR_EC for ISDN build.
//*           09/15/2005  Venkat R       CQ9583: Remove DSPBIOSII Token
//*           12/09/2005  Venkat R       CQ9583: Explicitly declared TXDF2B_PROFILING to be zero for MFGR_DIAG
//*                                      earlier it was implicitly taking a value of zero
//*           12/12/2005  C Urrutia      CQ10073: Added RFI display token
//*           01/24/2006  Peter Hou      CQ9885: Added OVHD_L1CAPTURE for overhead channel raw data collection (default off)
//*           01/24/2006  Gerald Peter   CQ10142: Enabled RTT code and CLI code
//            UR8_MERGE_START_END CQ10202 Nima Ferdosi
//*           05/25/2006                 CQ10202: Added channel reporting token. 
//            UR8_MERGE_START CQ10786  KC
//                                       fix for Tonewi unable to link up to Globalspan DSLAM with 128K fix rate 
//            UR8_MERGE_END CQ10786  KC
// UR8_MERGE_START CQ11007  KCCHEN
// 09/26/06 KCCHEN       CQ11007 : US SNR margin update
// UR8_MERGE_END CQ11007 KCCHEN
//*************************************************************************************************************************
//*
//* The default flag settings are:
//*
//*   -dATUC=1 -dSTD=0 -dISDN=0 -dTIPCI=0 -dTID=0 -dTII=0 -dAI=0
//*   -dEVM2=0 -dEU=0 -dSL=0 -dSL_EVM=1 -dGLITE=0
//*
//* and are set after all the per-suffix options have had a chance to
//* set them.  Each flag is only set if it has not previously been set, so
//* per-suffix settings  can override defaults, and command-line defines can
//* override per-suffix settings.
//*
//*****************************************************************************


//* ===========================================================================
//*   Suffix codes
//*     The command-line can include -dOBJSFX_xxx to get a flag set
//*     instead of explicitly setting each flag on each CC/ASM command-line.
//*     and the object suffix will control the settings of "feature" constants.
//* ===========================================================================
//
//* ===========================================================================
//   Flag settings for new suffixes (in alphabetical order of suffix)
//     Each suffix has to specify only those flags which differ from the
//     default settings.
//* ===========================================================================
//  NOTE: Try to keep feature flags in alphabetical order to ease maintenance.
//* ===========================================================================
//*

#define CHIPSET_ID_UNKN '?'
#define CHIPSET_ID_AH   'H'
#define CHIPSET_ID_AP   'P'
#define CHIPSET_ID_AR   'R'
#define CHIPSET_ID_ARV  'R'
#define CHIPSET_ID_AT   'T'
#define CHIPSET_ID_AU   'U'

#define CHIPSET_ID2_GENERIC '0'
#define CHIPSET_ID2_ARV  'R'

  #define DSPDP_IMAGE_ID_STANDARD(code) ( \
      STANDARD_is_MULTIMODE(code) ? "M" : \
      STANDARD_is_GDMT(code)      ? "D" : \
      STANDARD_is_GLITE(code)     ? "L" : \
      STANDARD_is_T1413(code)     ? "T" : "_")

  #define DSPDP_IMAGE_ID_SERVICE(code)  ( \
      SERVICE_is_POTS(code)        ? "P" : \
      SERVICE_is_ISDN_ANNEXB(code) ? "B" : \
      SERVICE_is_ISDN_ANNEXC(code) ? "C" : \
      SERVICE_is_ISDN_PROP(code)   ? "I" : "")

// Bit-codes for feature byte in new version.
//
// 0000 0000
// |||| ||||
// |||| |||+ -- POTS
// |||| ||+---- ISDN_ANNEXB
// |||| |+----- ISDN_ANNEXC
// |||| +------ ISDN_PROP
// |||+--------
// ||+--------- GHS
// |+---------- GLITE
// +----------- T1413
//
//
#define FEATURE_BIT_T1413       0x80
#define FEATURE_BIT_GLITE       0x40
#define FEATURE_BIT_GHS         0x20
#define FEATURE_BIT_ISDN_PROP   0x08
#define FEATURE_BIT_ISDN_ANNEXC 0x04
#define FEATURE_BIT_ISDN_ANNEXB 0x02
#define FEATURE_BIT_POTS        0x01

#define STANDARD_BITS_MASK      (FEATURE_BIT_T1413 | FEATURE_BIT_GLITE | FEATURE_BIT_GHS)
#define STANDARD_BITS_T1413      FEATURE_BIT_T1413
#define STANDARD_BITS_GHS        FEATURE_BIT_GHS
#define STANDARD_BITS_GLITE     (FEATURE_BIT_GLITE | FEATURE_BIT_GHS)
#define STANDARD_BITS_GDMT      (STANDARD_BITS_T1413 | STANDARD_BITS_GHS)
#define STANDARD_BITS_MULTIMODE (STANDARD_BITS_T1413 | STANDARD_BITS_GLITE | STANDARD_BITS_GDMT)

#define SERVICE_BIT_ISDN_ANNEXB  FEATURE_BIT_ISDN_ANNEXB
#define SERVICE_BIT_ISDN_ANNEXC  FEATURE_BIT_ISDN_ANNEXC
#define SERVICE_BIT_ISDN_PROP    FEATURE_BIT_ISDN_PROP
#define SERVICE_BIT_POTS         FEATURE_BIT_POTS

//
//  Debug new-style suffixes
//
//

#ifndef OVHD_L1CAPTURE  // used fro OVHD L1 raw data collectiong (for debugging)
#define OVHD_L1CAPTURE 0
#endif
#if defined(OBJSFX_ar0700db_debugobj)
#ifndef OBJSFX_ar0700dbobj
#define OBJSFX_ar0700dbobj 1
#endif
// Here, in alphabetic order, are the feature tokens that
// distinguish this suffix from its non-_debug partner:
// (All other tokens from the non-_debug partner that are non-conflicting will also be picked up)

#ifndef JTAG
#define JTAG 1
#endif

#elif defined(OBJSFX_ar0700mp_debugobj)
#ifndef OBJSFX_ar0700mpobj
#define OBJSFX_ar0700mpobj 1
#endif
// Here, in alphabetic order, are the feature tokens that
// distinguish this suffix from its non-_debug partner:
// (All other tokens from the non-_debug partner that are non-conflicting will also be picked up)

#ifndef ADSL2_BSWP
#define ADSL2_BSWP 1
#endif
#ifndef AOC
#define AOC 1
#endif
#ifndef BITSWAP
#define BITSWAP 1
#endif
#ifndef DEBUG_ADSL2
#define DEBUG_ADSL2  0
#endif
#ifndef DEBUG_LOG
#define DEBUG_LOG 0
#endif
#ifndef JTAG
#define JTAG 1
#endif
#endif  // end of the series of "#elif defined" for debug targets


//
//  Standard new-style suffixes for operational and mfgr'ing diag.
//

#if defined(OBJSFX_ar0700dbobj)
#define CHIPSET_AR07    1
#define PLATFORM_AR0700 1
#define DSPDP_CHIPSET_ID      CHIPSET_ID_AR
#define DSPDP_CHIPSET_ID2     CHIPSET_ID2_GENERIC
#define DSPDP_CHIPSET_GEN     7
#define DSPDP_HARDWARE_REV1   '0'
#define DSPDP_HARDWARE_REV2   '0'
#define DSPDP_FEATURE_CODE   (STANDARD_BITS_GDMT|SERVICE_BIT_ISDN_ANNEXB)
#ifndef AOC
#define AOC 1
#endif
// ATM_TC_HW and SWTC are mutually exclusive
#ifndef ATM_TC_HW
#define ATM_TC_HW 1
#endif
#ifndef SWTC
#define SWTC 0
#endif
#ifndef BITSWAP
#define BITSWAP 1
#endif
#ifndef EXTERNBERT
#define EXTERNBERT 0
#endif
#ifndef MARGIN_DELTA_RETRAIN
#define MARGIN_DELTA_RETRAIN 0
#endif
#ifndef NLNOISEADJSNR_EC
#define NLNOISEADJSNR_EC 1
#endif
#ifndef OLAYDP
#define OLAYDP 1
#endif
#ifndef SHALF
#define SHALF 1
#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Copy tokens from SERVICE_POTS
// ATM_TC_HW and SWTC are mutually exclusive
#ifndef ADSL2_1BIT_TONE
#define ADSL2_1BIT_TONE 1
#endif
#ifndef ADSL2_BSWP
#define ADSL2_BSWP 1
#endif
#ifndef DS_RX_CODEWORD
#define DS_RX_CODEWORD 1
#endif



#elif defined(OBJSFX_ar0700db_diagobj)
#define CHIPSET_AR07    1
#define PLATFORM_AR0700 1
#define DSPDP_CHIPSET_ID      CHIPSET_ID_AR
#define DSPDP_CHIPSET_ID2     CHIPSET_ID2_GENERIC
#define DSPDP_CHIPSET_GEN     7
#define DSPDP_HARDWARE_REV1   '0'
#define DSPDP_HARDWARE_REV2   '0'
#define DSPDP_FEATURE_CODE   (STANDARD_BITS_GDMT|SERVICE_BIT_ISDN_ANNEXB)
#ifndef AOC
#define AOC 0
#endif
// ATM_TC_HW and SWTC are mutually exclusive (or both must be off)
#ifndef ATM_TC_HW
#define ATM_TC_HW 0
#endif
#ifndef SWTC
#define SWTC 0
#endif
#ifndef BITSWAP
#define BITSWAP 0
#endif
#ifndef MARGIN_DELTA_RETRAIN
#define MARGIN_DELTA_RETRAIN 0
#endif
#ifndef MFGR_DIAG
#define MFGR_DIAG 1
#endif
#ifndef OAM_EOC
#define OAM_EOC 0
#endif
#ifndef OLAYDP
#define OLAYDP 1
#endif
#ifndef SNR_UPDATE
#define SNR_UPDATE 0
#endif
#ifndef NLNOISEADJSNR_EC
#define NLNOISEADJSNR_EC 1
#endif

#ifndef US_CRC_RETRAIN
#define US_CRC_RETRAIN 0
#endif
#ifndef ADSL2_BSWP
#define ADSL2_BSWP 0
#endif
#ifndef DS_RX_CODEWORD
#define DS_RX_CODEWORD 0
#endif

#elif defined(OBJSFX_ar0700mpobj)
#define CHIPSET_AR07    1
#define PLATFORM_AR0700 1
#define DSPDP_CHIPSET_ID      CHIPSET_ID_AR
#define DSPDP_CHIPSET_ID2     CHIPSET_ID2_GENERIC
#define DSPDP_CHIPSET_GEN     7
#define DSPDP_HARDWARE_REV1   '0'
#define DSPDP_HARDWARE_REV2   '0'
#define DSPDP_FEATURE_CODE   (STANDARD_BITS_MULTIMODE|SERVICE_BIT_POTS)
#ifndef AOC
#define AOC 1
#endif
// ATM_TC_HW and SWTC are mutually exclusive
#ifndef ADSL2_1BIT_TONE
#define ADSL2_1BIT_TONE 1
#endif
#ifndef ADSL2_BSWP
#define ADSL2_BSWP 1
#endif
#ifndef ATM_TC_HW
#define ATM_TC_HW 1
#endif
#ifndef SWTC
#define SWTC 0
#endif
#ifndef BITSWAP
#define BITSWAP 1
#endif
#ifndef EXTERNBERT
#define EXTERNBERT 0
#endif
#ifndef OLAYDP
#define OLAYDP 1
#endif
#ifndef SHALF
#define SHALF 1
#endif
#ifndef MEM_STR
#define MEM_STR 0
#endif
#ifndef DS_LOOP_BACK
#define DS_LOOP_BACK 0
#endif
#ifndef LOOP_BACK_DEBUG
#define LOOP_BACK_DEBUG 0
#endif
#ifndef US_LOOP_BACK
#define US_LOOP_BACK 0
#endif
#ifndef DS_RX_CODEWORD
#define DS_RX_CODEWORD 1
#endif

#elif defined(OBJSFX_ar0700mp_diagobj)
#define CHIPSET_AR07    1
#define PLATFORM_AR0700 1
#define DSPDP_CHIPSET_ID      CHIPSET_ID_AR
#define DSPDP_CHIPSET_ID2     CHIPSET_ID2_GENERIC
#define DSPDP_CHIPSET_GEN     7
#define DSPDP_HARDWARE_REV1   '0'
#define DSPDP_HARDWARE_REV2   '0'
#define DSPDP_FEATURE_CODE   (STANDARD_BITS_MULTIMODE|SERVICE_BIT_POTS)
#ifndef ADSL2_BSWP
#define ADSL2_BSWP 0
#endif
#ifndef AOC
#define AOC 0
#endif
// ATM_TC_HW and SWTC are mutually exclusive (or both must be off)
#ifndef ATM_TC_HW
#define ATM_TC_HW 0
#endif
#ifndef SWTC
#define SWTC 0
#endif
#ifndef BITSWAP
#define BITSWAP 0
#endif
#ifndef MARGIN_DELTA_RETRAIN
#define MARGIN_DELTA_RETRAIN 0
#endif
#ifndef MFGR_DIAG
#define MFGR_DIAG 1
#endif
#ifndef OAM_EOC
#define OAM_EOC 0
#endif
#ifndef OLAYDP
#define OLAYDP 1
#endif
#ifndef SNR_UPDATE
#define SNR_UPDATE 0
#endif
#ifndef US_CRC_RETRAIN
#define US_CRC_RETRAIN 0
#endif
#ifndef ADSL2_BSWP
#define ADSL2_BSWP 0
#endif
#ifndef DS_RX_CODEWORD
#define DS_RX_CODEWORD 0
#endif

#else
#define DSPDP_CHIPSET_ID      CHIPSET_ID_UNKN
#define DSPDP_CHIPSET_ID2     CHIPSET_ID2_GENERIC
#define DSPDP_CHIPSET_GEN     0
#define DSPDP_HARDWARE_REV1   '0'
#define DSPDP_HARDWARE_REV2   '0'
#define DSPDP_FEATURE_CODE    0
#endif

// For use in checking the code in drivers -- indented to avoid .h->.ah
 #define STANDARD_is_T1413(code)      (!(((code) & STANDARD_BITS_MASK) ^  STANDARD_BITS_T1413))
 #define STANDARD_is_GLITE(code)      (!(((code) & STANDARD_BITS_MASK) ^  STANDARD_BITS_GLITE))
 #define STANDARD_is_GHS(code)          (((code) & STANDARD_BITS_MASK) &  STANDARD_BITS_GHS)
 #define STANDARD_is_GDMT(code)       (!(((code) & STANDARD_BITS_MASK) ^ (STANDARD_BITS_T1413 | STANDARD_BITS_GHS)))
 #define STANDARD_is_MULTIMODE(code)  (!(((code) & STANDARD_BITS_MASK) ^ (STANDARD_BITS_T1413 | STANDARD_BITS_GLITE | STANDARD_BITS_GDMT)))
 #define SERVICE_is_POTS(code)           ((code) & SERVICE_BIT_POTS)
 #define SERVICE_is_ISDN_ANNEXB(code)    ((code) & SERVICE_BIT_ISDN_ANNEXB)
 #define SERVICE_is_ISDN_ANNEXC(code)    ((code) & SERVICE_BIT_ISDN_ANNEXC)
 #define SERVICE_is_ISDN_PROP(code)      ((code) & SERVICE_BIT_ISDN_PROP)

#define STANDARD_T1413     (!((DSPDP_FEATURE_CODE & STANDARD_BITS_MASK) ^  STANDARD_BITS_T1413))
#define STANDARD_GLITE     (!((DSPDP_FEATURE_CODE & STANDARD_BITS_MASK) ^  STANDARD_BITS_GLITE))
#define STANDARD_GHS         ((DSPDP_FEATURE_CODE & STANDARD_BITS_MASK) &  STANDARD_BITS_GHS)
#define STANDARD_GDMT      (!((DSPDP_FEATURE_CODE & STANDARD_BITS_MASK) ^ (STANDARD_BITS_T1413 | STANDARD_BITS_GHS)))
#define STANDARD_MULTIMODE (!((DSPDP_FEATURE_CODE & STANDARD_BITS_MASK) ^ (STANDARD_BITS_T1413 | STANDARD_BITS_GLITE | STANDARD_BITS_GDMT)))

#define SERVICE_POTS        (DSPDP_FEATURE_CODE & SERVICE_BIT_POTS)
#define SERVICE_ISDN_ANNEXB (DSPDP_FEATURE_CODE & SERVICE_BIT_ISDN_ANNEXB)
#define SERVICE_ISDN_ANNEXC (DSPDP_FEATURE_CODE & SERVICE_BIT_ISDN_ANNEXC)
#define SERVICE_ISDN_PROP   (DSPDP_FEATURE_CODE & SERVICE_BIT_ISDN_PROP)
#define SERVICE_ISDN        (SERVICE_ISDN_ANNEXB | SERVICE_ISDN_ANNEXC | SERVICE_ISDN_PROP)


//
//  Backwards compatibility with old tokens
//

#if (SERVICE_POTS)
#ifndef ISDN
#define ISDN 0
#endif
#endif

#if (SERVICE_ISDN_ANNEXB | SERVICE_ISDN_PROP)
#ifndef ISDN
#define ISDN 1
#endif
#endif


//
//* ===========================================================================
//  More Default settings
//* ===========================================================================
//

//
//  BEGIN Could automatically generate showdefs code
//
#ifndef AOC
#define AOC 1
#endif
#ifndef ATMBERT
#define ATMBERT 0
#endif
// ATM_TC_HW and SWTC are mutually exclusive
#ifndef ATM_TC_HW
#define ATM_TC_HW 1
#endif
#if ATM_TC_HW
#ifndef ATMBERT_HW
#define ATMBERT_HW 1
#endif
#ifndef SWTC
#define SWTC 0
#endif
#else // else case for #if ATM_TC_HW
#ifndef ATMBERT_HW
#define ATMBERT_HW 0
#endif
#ifndef SWTC
#define SWTC 1
#endif
#endif // end of #if ATM_TC_HW
#ifndef ATM_TC_PATH1_ON
#define ATM_TC_PATH1_ON 0
#endif
#ifndef BITSWAP
#define BITSWAP 1
#endif
#ifndef TRELLIS
#define TRELLIS 1
#endif
#ifndef DEBUG_ADSL2
#define DEBUG_ADSL2 0
#endif
#ifndef DEBUG_DUMP
#define DEBUG_DUMP 0
#endif
#ifndef DEBUG_LOG
#define DEBUG_LOG 0
#endif
#ifndef DEV_DEBUG
#define DEV_DEBUG 0
#endif
#ifndef DS_LOOP_BACK
#define DS_LOOP_BACK 0
#endif
#ifndef DS_RX_CODEWORD
#define DS_RX_CODEWORD 1
#endif
#ifndef LOOP_BACK_DEBUG
#define LOOP_BACK_DEBUG 0
#endif

#ifndef US_LOOP_BACK
#define US_LOOP_BACK 0
#endif
#ifndef ADSL2_1BIT_TONE
#define ADSL2_1BIT_TONE 1
#endif
#ifndef ADSL2_BSWP
#define ADSL2_BSWP 1
#endif
#ifndef MEM_STR
#define MEM_STR 0
#endif
#ifndef EXTERNBERT
#define EXTERNBERT 0
#endif
#ifndef FPGA
#define FPGA 0
#endif
#ifndef INBAND
#define INBAND 0
#endif
#ifndef ISDN
#define ISDN 0
#endif
#ifndef ISDN_DEBUG
#define ISDN_DEBUG 0
#endif
#ifndef LOOP_BACK_DEBUG
#define LOOP_BACK_DEBUG 0
#endif
#ifndef MARGIN_DELTA_RETRAIN
#define MARGIN_DELTA_RETRAIN 1
#endif
#ifndef MEM_STR
#define MEM_STR 0
#endif
#ifndef MFGR_DIAG
#define MFGR_DIAG 0
#endif
#ifndef NLNOISEADJSNR_EC
#define NLNOISEADJSNR_EC 0
#endif
#ifndef NTR_MODE
#define NTR_MODE 0
#endif
#ifndef OAM_EOC
#define OAM_EOC 1
#endif
#ifndef OHIO_SUPPORT
#define OHIO_SUPPORT 1
#endif
#ifndef OLAYDP
#define OLAYDP 0
#endif
#ifndef OUTBAND
#define OUTBAND 0
#endif
#ifndef PERTONE_EQ
#define PERTONE_EQ 0
#endif
#ifndef PHY_TDW_ENABLE
#define PHY_TDW_ENABLE 0
#endif
#ifndef SHALF
#define SHALF 0
#endif
#ifndef SNR_UPDATE
#define SNR_UPDATE 1
#endif
#ifndef TC_DEBUG
#define TC_DEBUG 0
#endif
#ifndef TC_LOOPBACK
#define TC_LOOPBACK 0
#endif
#ifndef TESTMODE
#define TESTMODE 0
#endif

#if (!MFGR_DIAG)   //(CQ:10242)
#define DBG_RTT 1
#else // (#if MFGR_DIAG)
#define DBG_RTT 0
#endif // (#if MFGR_DIAG)

#ifndef RTT_DATA_MEMORY_ENABLE
#define RTT_DATA_MEMORY_ENABLE 1
#endif
#ifndef RTT_AFE_SAMPLES_PATH0_ENABLE
#define RTT_AFE_SAMPLES_PATH0_ENABLE 1
#endif
#ifndef RTT_FFT_DOWNSTREAM_PATH0_ENABLE
#define RTT_FFT_DOWNSTREAM_PATH0_ENABLE 1
#endif
#ifndef RTT_CONSTELLATION_US_ENABLE
#define RTT_CONSTELLATION_US_ENABLE 0
#endif
#ifndef RTT_CONSTELLATION_DS_ENABLE
#define RTT_CONSTELLATION_DS_ENABLE 0
#endif
#ifndef RTT_VCXO_IN_HEADER
#define RTT_VCXO_IN_HEADER 1
#endif

#ifndef TXDF2B_PROFILING

#if (SERVICE_POTS)

#if (!MFGR_DIAG)
#define TXDF2B_PROFILING 1
#else // (!MFGR_DIAG)
#define TXDF2B_PROFILING 0
#endif // (#if !MFGR_DIAG)

#else // (#if SERVICE_POTS)
#define TXDF2B_PROFILING 0
#endif // (#if SERVICE_POTS)

#endif // (#ifndef TXDF2B_PROFILING)

#ifndef US_CRC_RETRAIN
#define US_CRC_RETRAIN 1
#endif
#ifndef US_LOOP_BACK
#define US_LOOP_BACK 0
#endif
#ifndef ATM_TC_DBG
#define ATM_TC_DBG  1
#endif

#ifndef RFI_DISPLAY
#define RFI_DISPLAY  0
#endif

#ifndef ADSL2_GhsRA_DBG
#define ADSL2_GhsRA_DBG  3  // Debug level 1, 2, 3
#endif

//UR8_MERGE_START CQ10202 Nima Ferdosi
#ifndef TI_INTERNAL1
#define TI_INTERNAL1 0 
#endif
//UR8_MERGE_END CQ10202


// Control inclusion of CLI code to handle memory read/write
// Diag module will not build with this flag set 4/28/05 ML
#ifndef DBG_CLI_MEMORY_ACCESS
#define DBG_CLI_MEMORY_ACCESS 0
#endif

#ifndef DBG_CLI_REDIRECT  //(CQ:10242)
#define DBG_CLI_REDIRECT 1
#endif


//
//  END Could automatically generate showdefs code
//
#if DSPDP_FEATURE_CODE
#else
// Unrecognized_suffix____check_spelling
#endif
//
//  LNK_CMD is set when running CPP to generate lnk_cpe.cmd file
//  -- the linker is not happy when it sees C code show up in the
//  result!
//
#ifndef LNK_CMD
extern int compile_happy;  // Keep the compiler from complaining about an empty file
#endif
//UR8_MERGE_START CQ10786  KC
#define CNXT_ADSL1_128K_FAIL_FIX 1
#ifdef CNXT_ADSL1_128K_FAIL_FIX
  #define CNXT_ADSL1_128K_FAIL_FIX_BIT_ALLOCATED_START_TONE_47 47
  #define CNXT_ADSL1_128K_FAIL_FIX_RATE_160K 40//4*40=160= 128+32kbps
  #define ATUR_RELINK 0
#endif
//UR8_MERGE_END CQ10786  KC
#endif // _ENV_DEF_DEFINES_H_

