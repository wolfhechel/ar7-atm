#ifndef __DEV_HOST_INTERFACE_H__
#define __DEV_HOST_INTERFACE_H__ 1

/*******************************************************************************
*  FILE PURPOSE: Public header file for the Host-to-DSP interface
********************************************************************************
*
*  TEXAS INSTRUMENTS PROPRIETARTY INFORMATION
*
*  (C) Copyright Texas Instruments Inc. 2002.  All rights reserved.
*
*  Property of Texas Instruments Incorporated
*
*  Restricted Rights - Use, duplication, or disclosure is subject to
*  restrictions set forth in TI's program license agreement and
*  associated documentation
*
*
*  FILE NAME: dev_host_interface.h
*
*  DESCRIPTION:
*       This header file defines the variables and parameters used between the
*       host processor and the DSP.  This file is included in both the DSP
*       software and the host software.
*
*  RULES FOR MODIFICATION AND USE OF THIS FILE:
*
*  --The main pointer to the struct of pointers will always be at the same fixed
*      location (0x80000000).
*
*  --Each pointer element in the struct of pointers (indicated by the main pointer)
*      will always point to a struct only.
*
*  --Any new structures added to the host interface in subsequent versions must
*      each have a corresponding new pointer element added to the END of the struct
*      of pointers.  Other than this, there will never be any moving or rearranging
*      of the pointer elements in the struct of pointers.
*
*  --Any new elements added to existing structures will be added at the END of the
*      structure.  Other than this, there will never be any moving or rearranging
*      of structure elements.
*
*  --A new structure will never be added as a new element in an old structure.
*      New structures must be added separately with a new entry in the struct of
*      pointers, as noted above.
*
*  --Also, the sizes of existing arrays within old structures will never be changed.
*
*  --The modem code in the DSP will never reference the struct of pointers in order
*      to avoid aliasing issues in the DSP code.  The modem code will only use the
*      specific structures directly.
*
*  --The host processor never accesses the DSP side of the ATM-TC hardware directly.
*      The DSP interfaces directly to the ATM-TC hardware and relays information to
*      the host processor through the host interface.
*
*  --The host processor can track the modem's transition through important states
*      by accessing the Modem State Bit Field in the host interface.  Each bit in
*      the bit field represents an important state to track in the modem.  As the
*      modem transitions through each important state, the corresponding bit will
*      change from a zero to a one.  Each bit in the bit field will only be reset to
*      zero if the modem retrains.  If new states need to be tracked and are added
*      in subsequent versions of the host interface, a corresponding bit will be
*      added at the END of the bit field to ensure backwards compatibility.  The
*      Modem State Bit Field is reset if the modem retrains or falls out of Showtime.
*
*  --An interrupt will be sent to the host processor when a change occurs in the
*      Modem State Bit Field.  There is an interrupt masking register which can mask
*      specific interrupts corresponding to the bits of the Modem State Bit Field.
*      This allows the host to keep an interrupt from being generated for those
*      states that are masked.
*
*  HISTORY:
*
*  11/20/02  J. Bergsagel    Written from the previous host interface file
*  11/27/02  J. Bergsagel    Added comments for mailbox control struct and
*                              fixed a couple items for overlay page stuff.
*                            Also, added temporary elements for SWTC code.
*  12/04/02  J. Bergsagel    Added extra dummy byte to DEV_HOST_eocVarDef_t
*                              for proper word alignment.
*  12/12/02  J. Bergsagel    Changed initial states in the modem state bit field
*                              and added more instructions for adding more states.
*  12/16/02  J. Bergsagel    Changed name "hostVersion_p" to "hostIntfcVersion_p".
*                            Removed dspAturState from DEV_HOST_modemStateBitField_t.
*                            Reorganized several struct elements to clean up the
*                              host interface.
*  12/27/02  Sameer V        Added missing channel 0 statistics for TC. Added
*                            ocd error information.
*  12/27/02  Sameer V        Added overlayState to OlayDP_Parms to indicate whether
*                              overlays are being executed in current state.
*  01/06/03  J. Bergsagel    Added maxAllowedMargin and minRequiredMargin to
*                              DEV_HOST_msg_t.
*                            Renamed TC chan 1 items to be chan 0 items to start out.
*  01/17/03  Sameer V        Moved delineationState to atmStats structure.
*  01/21/03  Barnett         Implemented Ax7 UNIT-MODULE modular software framework.
*  01/22/03  J. Bergsagel    Added warning comments for certain struct typedefs.
*  01/23/03  C. Perez-N.     Removed old AX5-only diags. command/response entries in the
*                            HOST and DSP ennumerations, and added the AX7 new ones
*                            Added pointer entries in the DEV_HOST_dspOamSharedInterface_t
*                            structure pointing to the analog diags. input/output/options
*                            structures.
*  01/29/03  Sameer V        Removed TC_IDLE in enum for delineation state. Hardware
*                            only reports TC_HUNT, TC_PRESYNC and TC_SYNC.
*  03/07/03  Sameer/Jonathan Put SWTC token around structs and elements only used by SWTC
*  03/12/03  Mannering       Add CO profile data structures
*  03/18/03  J. Bergsagel    Removed the obsolete DSP_CHECK_TC response message.
*  03/24/03  J. Bergsagel    Added DEV_HOST_hostInterruptMask_t for masking DSP interrupt sources
*  03/28/03  C. Perez-N      Changed the C-style comments and made them C++ sytle instead.
*                            Replaced the occurrences of "SINT32 *" pointer declarations with
*                            "PSINT32"
*  03/28/03  Mannering       Update CO profile data structures
*  04/04/03  S. Yim          Add host I/F hooks for switchable hybrid and RJ11
*                              inner/outer pair selection
*  04/11/03  J. Bergsagel    Changed modem state bit field struct types to enums instead and used
*                            a single integer variable for each "bitfield".
*                            Changed bit field for host interrupt masks to an integer value also.
*  04/14/03  J. Bergsagel    Changed name of table pointer "meanSquareTblDstrm_p" to "marginTblDstrm_p".
*  04/03/03  Umesh Iyer      CMsg1 and RMsg1 use the same storage as CMSGPCB and RMSGPCB.
*                            The string lengths for these have been adjusted to hold the longest
*                            message in each case. The PCB messages from ADSL2 are longer.
*  04/21/03  Sameeer V       Added new host mailbox message for shutting down the DSLSS peripherals.
*  04/23/03  J. Bergsagel    Fixed comments for overlay mailbox messages and for losErrors.
*  04/28/03  Mannering       Added skip phase op flag to  CO profile data structure
*  05/05/03  Mannering       Review Comments - Removed "#if CO_PROFILE" from around structure
*                            definitions and define the number of profiles (DEV_HOST_LIST_ENTRIES)
*  05/13/03  J. Bergsagel    Added new elements to DEV_HOST_phyPerf_t for host control of hybrid.
*  05/15/03  J. Bergsagel    Added "farEndLosErrors" and "farEndRdiErrors" to DEV_HOST_modemStatsDef_t.
*  05/16/03  Mannering       Updated CO profile structure to support updated bit allocation and
*                            interopability.
*  05/20/03  Sameer V        Added DSP message to inicate DYING GASP.
*  05/22/03  J. Bergsagel    Added a new struct typedef "DEV_HOST_hostInterruptSource_t".
*                            Added "atucGhsRevisionNum" to "DEV_HOST_dspWrNegoParaDef_t".
*                            Moved the following struct typedef's here to the public host interface:
*                              DEV_HOST_dspBitSwapDef_t
*                              DEV_HOST_atmDsBert_t
*  05/28/03  A. Redfern      Changed pointer type and location for margin reporting.
*  05/28/03  Mannering       Moved CO profile defines to dev_host_interface_pvt.h
*  05/28/03  J. Bergsagel    Moved subStateIndex and STM BERT controls into new struct "DEV_HOST_modemEnvPublic_t"
*  05/29/03  J. Bergsagel    Added elements to "DEV_HOST_modemEnvPublic_t" for host control of DSLSS LED's.
*  06/10/03  Umesh Iyer      Modified trainMode check to be compliant with the new host i/f mods.
*  06/05/03  J. Bergsagel    Added enum that will eventually replace the bitfield: DEV_HOST_diagAnlgOptionsVar_t.
*                            Added new element "currentHybridNumUsed" in the DEV_HOST_phyPerf_t typedef
*                            Added new host control flags for LPR signal detection on GPIO[0].
*  06/06/03  A. Redfern      Removed fine gain scale from the CO profile and added max downstream power cutback.
*                            Changed "test1" in CO profile struct to "phyEcDelayAdjustment".
*  06/26/03  J. Bergsagel    Added genericStructure typedef and two pointer elements of this type in the big table.
*  07/03/03  Jack Huang      Renamed test2 to bSwapThresholdUpdate
*  07/07/03  Mallesh         Changed phySigTxPowerCutback_f flag to a variable phySigTxGainReductionAt0kft which indicates the
*                            amount of gain reduction in linear scale.
*  07/15/03  Sameer V        Changed DEV_HOST_diagAnlgOptionsVar_t to be an enum instead of a bit field. Host code
*                            does not support setting bit fields.
*  07/22/03  Jack Huang      Added bitswap control flag in host i/f for API calls
*  08/06/03  Sameer V        Added missingToneDs_p to the DEV_HOST_oamWrNegoParaDef_t to enable host to switch off
*                            DS tones on specified bins
*  08/21/03  Jack Huang      Added pcbEnabled flag in the DEV_HOST_modemEnvPublic_t structure
*                            Added g.hs buffer definitions to DEV_HOST_dspOamSharedInterface_t
*                            Added DEV_HOST_consBufDef_t to the DEV_HOST_dspOamSharedInterface_t structure
*  08/26/03  J. Bergsagel    Fixed name of "missingToneDs_p" to be "missingToneDsAddr" instead (since it is
*                            not really used as a pointer).
*  09/11/03  Mallesh         Added a flag "usPilotInT1413ModeInMedley" to determine the need to send Upstream Pilot
*                            in medley in T1.413 mode.
*  09/12/03  J. Bergsagel    Changed "test3" to "phyBitaFastPathExcessFineGainBump" in CO profile struct.
*                            Changed "test4" to "phyBitaSkipGapAdjustment" in CO profile struct.
*  09/23/03  J. Bergsagel    Changed "T1413vendorRevisionNumber" to "vendorRevisionNumber" in DEV_HOST_msg_t.
*                            Added ADSL2 and ADSL2 diag. states to the modem state bit field.
*  10/01/03  J. Bergsagel    Changed define of "MULTI_MODE" to be 255 to indicate that all possible bits
*                            in the 8-bit bit field are turned on for any current and future training modes.
*  10/09/03  M. Turkboylari  Added DSP_TRAINING_MSGS and adsl2DeltMsgs_p, which is a pointer to a pointer,
*                            in order to pass the ADSL2 training and DELT messages to the host side. This is for ACT.
*  10/20/03  Mallesh         Added a GHS state enumerator for cleardown
*  10/20/03  Xiaohui Li      Add definition for READSL2_MODE and READSL2_DELT
*  11/07/03  J. Bergsagel    Removed all code for when SWTC==1, which therefore allows removal of include of
*                            "env_def_defines.h".  We shouldn't have any compile tokens used in this file.
*                            (the SWTC token is always off in any Ax7 code).
*  11/14/03  J. Bergsagel    Also removed READSL2_ENABLE token (no more compile tokens to be used in this .h file).
*  12/12/03  Sameer/Ram      Added DEV_HOST_EOCAOC_INTERRUPT_MASK to enable host to disable response code for AOC/EOC
*                            mailbox messages
*  12/09/03  Jack Huang      Changed G.hs txbuf size from 60 to 64 to fit the max segment size
*  12/15/03  Mallesh         Changed vendor ID type defenition from SINT16 to UINT16
*  12/23/03  Sameer V        Added ability to turn off constellation display reporting to host using oamFeature bit field.
*  12/24/03  Sameer V        Changed comment for Constellation Display Current Address to Host Write instead of DSP Write.
*  12/26/03  Sameer/Ram      Added DEV_HOST_GHSMSG_INTERRUPT_MASK to enable host to disable response code for GHS Messages
*  02/03/04  Mannering       Added token OHIO_SUPPORT
*  02/24/04  Mannering       Fixed comments on devCodecTxDf2aDen and devCodecTxDf2bDen.
*  03/08/04  Mallesh         Increased the number of overlay pages to 7.
*  04/01/04  Sameer          Increased the number of overlay pages to 8.
*  04/12/04  Sameer          Moved TXDF2B filter coeffs to SDRAM memory. Added mechanism to DMA data back to data memory
*                            based on the pointer allocated by the host processor.
*  04/15/04  Umesh Iyer      Added support for including 2 additional overlay pages for training.
*  04/20/04  U G Jani        Reused dummy2 for EOC selfTestResults results in DEV_HOST_eocVarDef_t typedef.
*  05/14/04  Mallesh         Added structures for ADSL2 bitswaps
*  06/11/04  Brian Z. & C.   Added structures for clear EOC HDLC host Rx and Tx buffers
*  07/09/04  Jack Huang      Added MSE settle time control in CO profile
*  07/22/04  SV/MH/SM        Added variable to indicate rcvMode.
*  08/09/04  Jack Huang      Added following control variables in CO profile a) thresholf for ratio of max/min MSE and
*                            b) threshold of max MSE power.
*  08/25/04  Sameer V        Added flag to indicate whether DSP runs at 200 v/s 250 Mhz.
*  08/26/04  Brian Z/S.Yim   Added DEV_HOST_diagAnlgOutputVar_t.rcvMode for Matlab offline data process
*  10/06/04  Sameer V        Added structure for feature control
*  03/17/05  T. Leyrer       Replaced reserved1 with maxbits_ds in structure DEV_HOST_msg_t. maxbits_ds is set to the
*                            minimum between host and CO setting and used for bit allocation and bitswap.
*  12/28/04  DW              CQ 9356: Modified definitions of phy feature control and enable bits.
*                            Added interop control and enable bits. Added bit mask definitions.
*  02/09/05  YH              CQ9459: add interop control and enable bits for SHORT_LOOP_US_ERR_REDUCTION.
*                            this is for fixing interop issue at CANTV
*  02/23/05  YH              added interop bit for QWEST_ALCATEL_US_LOW_RATE_FIX
*                            added interop bit for T1413_LONG_LOOP_ACTIVATION
                             added interop bit for QWEST_ALCATEL_US_LOW_RATE_FIX
                             added interop bit for ASB_ETSIB_HIGH_BER_FIX
                             added interop bit for BELL_CANADA_DOUBLE_BRIDGE_TAP_FIX
                             added interop bit for BELGACOM_DS_FIX
                             added interop bit for CINCINNATI_BELL_FIX
                             added interop bit for FRANCE_TELECOM_FIX
*  03/31/05  Ragu Pappu      added interop bit for QWEST_US_1M_RATE_FIX
*  05/20/05  Kapil           Added fields corresponding to AnnexB and AnnexM.
*                            Added two extra elements in DEV_HOST_dspWrNegoParaDef_t
*                            structure and one extra element in DEV_HOST_oamWrNegoParaDef_t
*                            structure. CQ9600.
*  05/20/05  Venkat R        CQ9600: Added bit field definitions for DEV_HOST_dspWrNegoParaDef_t
*                            psd_mask_qualifier field
*  06/14/05  Peter Hou       Added MULTI_MODE8 for backward compatibility
*                            Added ADSL2_MASKS & ADSL2PLUS_MASKS
*  07/06/05  Peter Hou       Added ENABLE_BELGACOM_ANXB_FAST_TRAINING as IOP Bit12
*  07/11/05  Kapil           CQ9600: Cleaned up usage of READSL as per new definitions of
*                            trainMode.
*  07/21/05  Kapil           CQ9600: Increase MAX_CMSGPCB2_LENGTH to ensure proper word alignment
*                            and also to take care of DELT.
*  07/21/05  Venkat R        CQ9600: Expanded some array sizes in DEV_HOST_olrDspRxDef_t to support
*                            64 upstream bins.
*  05/03/05  Hanyu Liu       CQ9500: added interop control and disable bits for SAFEMODE_FOR_G992_3_5
*  05/16/05  Madhu Hegde     CQ 9620. Added interop bit for ADI_UPSTREAM_RATES_FIX.
*                            When this bit is enabled, the quantization noise added to
*                            R-MEDLEY is reduced to get higher US data rates.
*  07/13/05  C. Urrutia      Added interop bit for ENABLE_OHIO_HYBRID4_REWIRE.
*                            When this bit is enabled hybrid 4 becomes part of the tx path
*  07/12/05  Shanti K        CQ9528 : Modified data structure DEV_HOST_modemEnvPublic_t
*  07/18/05  C. Urrutia      CQ9787 : Added interop bit for WESTELL_RFI_OPTION
*  08/31/05  Peter Hou       CQ9613 : Remove/recycle bit12, ENABLE_OHIO_HYBRID4_REWIRE,
*                            changed to ENABLE_NOKIA_D50_GDMT_RETRAIN_FIX
*  10/31/05  Ram             CQ10012: Added interop bit for REPORT_AVG_MARGIN; reused Bit14
*                                     Deleted unused bit for Belgacom Fast AnnexB training.
*  11/03/05  Manjula K       CQ10037:Added Inventory command support for ADSL2/2+
*  10/25/05  Ragu Pappu      CQ10004.  Added API control bit to disable extended*
*                            framing support.
*  1/4/06    Peter Hou       CQ5885, fixed ADSL2\2+ clearEoc CNXT IoP, added DISABLE_CNXT_CLREOC_PATCH.
*  01/18/06  Hanyu Liu       CQ10173: Added API bit17, ENABLE_TELEFONICA_FIXED_MARGIN
*  11/08/05  Peter Hou       Add DEV_HOST_BIS_MGMTCount_t in DEV_HOST_dspOamSharedInterface_t
*  01/27/06  Raghu M         CQ10045 Added API bit support for INP maximization
*  01/25/06  Madhu H         CQ10198: Added API bit to extend Annex B US to bin 28
*                            and delay minimization.
*  01/18/06  E Yin           CQ10222: Add API control bit to disable extended INP paramter in Ghs.
*  02/08/06  Wal/Ley/Peter   CQ10242: Added Cli2Linux buffer interface for CLI
*                                     redirection to Linux
*  02/09/06  Kapil Gulati    CQ10222: The API control bits for extended framing and INP>2 were
*                            not compliant with our traditional way of defining these bits.
*                            Hence fixed that issue.
*  02/14/06  Kapil Gulati    CQ10222: During SQI testing it was found that LS FS+ DSLAM does not
*                            handle INP>2 G.hs message correctly. Hence it was decided to disable
*                            this feature by default.
*  03/28/06  Tim Bornemisza  CQ10351: Add a bit to enable SRA and a message enum for the DHAL
*  UR8_MERGE_START CQ10415_10385 HL
*  03/27/06  Hanyu Liu       CQ10415: Changed infineon_CO_cabinet... to CO_cabinet...
*  03/30/06  Hanyu Liu       CQ10385: Added IOP API bit to enable C-COMB1 detector for 8 Comb tones.
*  UR8_MERGE_END CQ10415_10385
// UR8_MERGE_START API-Bits PeterHou
*  04/03/06  PeterHou/KapilG CQ10375: Added ENABLE_PHY_INP_DSCRC_IMPROVEMENT API bit
*            PeterHou/HanyuL CQ10385: Added ENABLE_DETECT_MORE_COMB1TONES API bit
*            PeterHou/TimB   CQ10351: Added ENABLE_PHY_SRA_SUPPORT API bit
// UR8_MERGE_END API-Bits PeterHou
// UR8_MERGE_START CQ10448 Ram
*  04/04/06  Ram             CQ10448: Combined two reserved byte fields into one SINT16 in atur_msg
*                            structure as attNdrBits field to compute DS Max Attainable NDR
*                            Removed trailing comma from IOP and PHY Feature Bit Structures
// UR8_MERGE_END CQ10448 Ram
// UR8_MERGE_START CQ10471 PeterHou
*  04/06/2006 PeterHou/TimB  CQ10471: Added ENABLE_CNXT_US_FLAVOR_B API Bit
// UR8_MERGE_END CQ10471
// UR8_MERGE_START API-Bit ManjulaK
*  05/24/06  ManjulaK/Nima   CQ10202: Added ENABLE_PHY_TI_INTERNAL1 API bit.
// UR8_MERGE_END API-Bit
// UR8_MERGE_START CQ10600 ManjulaK
// UR8_MERGE_START CQ10600 ManjulaK
*  05/26/2006 ManjulaK/HT  CQ10600: Added ENABLE_ETISALAT_US_CRC_N_LINEDROP_FIX API Bit
// UR8_MERGE_END CQ10600
// UR8_MERGE_END CQ10600
// UR8_MERGE_START_END CQ10617 AdeelJ
// 06/05/2006 AdeelJ       CQ10617: Added ENABLE_INFN_MINMAR_PARSE_N_CHK API Feature Bit
// UR8_MERGE_START_END API-Bit ManjulaK
*  06/07/06  ManjulaK      CQ10202: Changed ENABLE_PHY_TI_INTERNAL1 API bit assignment to 30 from 31.
*    UR8_MERGE_START CQ10654 Ram
*    06/06/06   Ram          CQ10654:  Added PHY Feature API bit 11 to activate in T1.413 mode.
*    UR8_MERGE_END   CQ10654 Ram
//UR8_MERGE_START_END CQ10516 MH
*  06/12/06  Madhu Hegde   CQ10516: Added ENABLE_PHY_COOK_INP_TRAINING API bit to bit 13
//UR8_MERGE_START_END CQ10665 NF
*  06/15/06  Nima Ferdosi  CQ10665: Added OAMFEATURE_PHY_EXTENDED_PILOT_SEARCH API bit to bit 14
// UR8_MERGE_START APIBits ManjulaK
*  07/26/06  ManjulaK/HT  CQ10781: Added ENABLE_LOWER_GHS_TONE_POWER API bit
*                         CQ10784: Added ENABLE_REDUCE_GHS_FALL_BACK_TIME API bit
// UR8_MERGE_END APIBits
//UR8_MERGE_START_END CQ10665 NF
*  06/29/06  Nima Ferdosi  CQ10665: fixed a naming mistake in API bit 14
//UR8_MERGE_START_END CQ10773 Ram
*  07/18/06  Ram           CQ10773: Added a #define for Loop Adaptive Timing Threshold to oamFeature
//UR8_MERGE_START_END CQ10758 Tim
*  07/11/06  Tim Bornemisza CQ10758: Updated DEV_HOST_olrDspRxDef_t structure for US SRA 
// UR8_MERGE_START_END CQ10774 Ram
*  07/19/2006 Ram          CQ10774: Added bit definition for oamFeature to disable Mailbox interrupt
*                          upon SRA event.
// UR8_MERGE_START CQ10774,784 Ram
*   08/08/06  Ram          CQ10774,84: Removed ENABLE_REDUCE_GHS_FALL_BACK_TIME & oamFeature definition
*                          for loop adaptive timing threshold and consolidated them into a new IOP bit
*                          ENABLE_TELIA_SONERA_FIX for all fixes applied to Telia.
// UR8_MERGE_START_END CQ10774,784 Ram
// UR8_MERGE_START_END     CQ10566 MB
// 08/11/2006 Mark Bryan   CQ10566 Added API_BIT ENABLE_PHY_GHSCABMODE
// UR8_MERGE_START_END CQ10899
*  08/30/2006 Madhu Hegde  CQ10899 Added API bits for ECHO_BAND_DS_SNR_CUTBACK
*  UR8_MERGE_START     CQ10927 HZ
*  09/11/06   Hao Zhou     CQ10927: Changed enumeration from ENABLE_RANDOM_TONE_ORDER to DISABLE_RANDOM_TONE_ORDER and removed CONTROL enum.
*  API bit affected:
*  ENABLE_RANDOM_TONE_ORDER; CONTROL_RANDOM_TONE_ORDER;
*  UR8_MERGE_END           CQ10927
*  UR8_MERGE_START CQ10880   Jack Zhang
*  8/30/06  JZ   CQ10880: Add DSL HAL API for sending mailbox message for L3
*  UR8_MERGE_END   CQ10880*
*  UR8_MERGE_START CQ11023 Hao-Ting Lin
*  Add API bit: ENABLE_PCCW_CNXT_FIX
*  Fix 2 issues: 1. unable to link up against CNXT DSLAM (U24,o6981,e38,e67)
*                2. margin drop to negative against to CNXT E38 G.DMT mode
*  UR8_MERGE_END CQ11023
*  UR8_MERGE_START CQ11031 Hao-Ting Lin
*  Add API bit: ENABLE_VERSION_NO_16BYTES
*  control EOC 16bytes for vendor version number at G.DMT mode.
*  UR8_MERGE_END
// UR8_MERGE_START CQ10913_AC7V30 YW
// 09/12/06   Yan Wang     CQ10913: AR7 fails to train against AC7 with version 3.0. This is because
//                         the old GHS in AC7 does not recognize new features. We should disable the 
//                         support of Annex M, extended frame parameters and large INP if AC7 with 
//                         firmware version older than 3.40 is encountered, and force the CPE to 
//                         retrain.
//                         We also create a new API bit to disable Annex M from the host.
// UR8_MERGE_END CQ10913_AC7V30 YW
*
// UR8_MERGE_START CQ11007  KCCHEN
// 09/26/06 KCCHEN       CQ11007 : US SNR margin update
// UR8_MERGE_END CQ11007 KCCHEN
* UR8_MERGE_START CQ11021 CQ11022 Hao-Ting Lin
*                                 Add COMLAB fix API bit
* UR8_MERGE_END CQ11021 CQ11022 Hao-Ting Lin
* UR8_MERGE_START_END CQ11007 Ram
// 10/04/06 Ram          CQ11007 : Added reserved bytes for 32-bit alignment in EocVar
// UR8_MERGE_START CQ10989 ManjulaK
* 10/20/06  Manjulak       CQ10989: Made changes for reporting selftestresults in ADSL2/2+ mode.
// UR8_MERGE_END CQ10989
* UR8_MERGE_START_END CQ10819 Ram
*  10/23/06 Ram          CQ10819 : Added API bit for OPTUS BT loop fix
* UR8_MERGE_START CQ10960 HZ
// 10/24/06 HZ        CQ10960: [7.0 SQI] Shorter reach vs LU FS+.  
//                    Add a PHY API bit (ENABLE_TIME_ERROR_SCALE_CNXT) for the PHY_TIME_ERROR_SCALE selection.
* UR8_MERGE_END   CQ10960 HZ
// UR8_MERGE_START CQ11075_API25 YW
// 10/23/06 Yan Wang      CQ11075: During live line tests at TDC and B2, DS CRC's and line drops were 
//                        observed with Ericsson/BRCM and Siemens/IFX DSLAM's. We need to reduce the 
//                        DS coding gain assumption by 2dB to leave enough noise margin, i.e. add 2-dB 
//                        pad for DS margin.
//                        A FEATURE API bit25 was defined  to provide an option for customers to control 
//                        when they need it.
// UR8_MERGE_END CQ11075_API25 YW
// UR8_MERGE_START_END CQ11080 Manjula/KC
*  10/25/06 Manjula       CQ11080 : Added ENABLE_CTLM_LOW_USRATE_FIX API bit                        
*  UR8_MERGE_START CQ10978   Jack Zhang
*  10/4/06  JZ     CQ10978: Request for the DSL Power Management Status Report
*  UR8_MERGE_END   CQ10978*
*  UR8_MERGE_START CQ10979   Jack Zhang
*  10/4/06  JZ     CQ10979: Request for TR-069 Support for RP7.1
*  UR8_MERGE_END   CQ10979*
*  UR8_MERGE_START_END CQ11004 Nima Ferdosi
*  10/27/06  Nima   CQ11004: Added ENABLE_PHY_TI_INTERNAL2 API bit.
*  UR8_MERGE_START_END CQ11004 Nima Ferdosi
*  11/02/06  Nima   CQ11004: Renamed ENABLE_PHY_TI_INTERNAL2 API bit to DISABLE_PHY_TI_INTERNAL2
*  UR8_MERGE_START_END CQ11161 Nima Ferdosi
*  11/15/06  Nima   CQ11161: Added a new field to DEV_HOST_eocVarDef_t to have the upstream margin with 0.1 dB granularity.
*  UR8_MERGE_START   CQ11194 HZ
*  12/05/06 Hao Zhou CQ11194: Modify the estimation of attainable DS data rate.
*                    Add a variable 'raOVRate' in the structure 'DSPWrNegoPara' in this file.    
*  UR8_MERGE_END CQ11194 HZ
// UR8_MERGE_START CQ11057 KCCHEN
// 10/12/06 Kuan-Chen Chen   CQ11057: Request US PMD test parameters from CO side
// UR8_MERGE_END CQ11057 KCCHEN
*  UR8_MERGE_START   CQ11228 HZ
*  12/08/06     Hao Zhou CQ11228: Modify the DS Margin report to 0.1dB precision.
*  UR8_MERGE_END   CQ11228 HZ
// UR8_MERGE_START_END CQ11277 Ram
// 12/22/06 Ram      CQ11277: Hlin Computation is protected with API bit ENABLE_ADSL2_2PLUS_HLIN
*  UR8_MERGE_START_END CQ11247_TR69_DS_LATN_SATN  YW
*  12/18/06   Yan Wang      CQ11247: TR069 range and precision changes for LATNds, SATNds  
*  UR8_MERGE_START CQ11230 Hao-Ting Lin
*  12/18/06 Hao-Ting Lin  Add API bit 31 TR067_DSPCB_FIX
*  UR8_MERGE_END CQ11230 Hao-Ting Lin
// 12/23/06 Ram      CQ11281: Added API bit for shorter g.hs Reset time ENABLE_GHS_SHORT_RESET
* (C) Copyright Texas Instruments Inc. 2002.  All rights reserved.
*******************************************************************************/

#include "dev_host_verdef.h"
// DW 12/28/04 Bit mask definitions, intended for general use
#define BIT0                 0x00000001
#define BIT1                 0x00000002
#define BIT2                 0x00000004
#define BIT3                 0x00000008
#define BIT4                 0x00000010
#define BIT5                 0x00000020
#define BIT6                 0x00000040
#define BIT7                 0x00000080
#define BIT8                 0x00000100
#define BIT9                 0x00000200
#define BIT10                0x00000400
#define BIT11                0x00000800
#define BIT12                0x00001000
#define BIT13                0x00002000
#define BIT14                0x00004000
#define BIT15                0x00008000
#define BIT16                0x00010000
#define BIT17                0x00020000
#define BIT18                0x00040000
#define BIT19                0x00080000
#define BIT20                0x00100000
#define BIT21                0x00200000
#define BIT22                0x00400000
#define BIT23                0x00800000
#define BIT24                0x01000000
#define BIT25                0x02000000
#define BIT26                0x04000000
#define BIT27                0x08000000
#define BIT28                0x10000000
#define BIT29                0x20000000
#define BIT30                0x40000000
#define BIT31                0x80000000

// ---------------------------------------------------------------------------------
// Address of the pointer to the DEV_HOST_dspOamSharedInterface_s struct of pointers
// This is where it all starts.
// ---------------------------------------------------------------------------------
#define DEV_HOST_DSP_OAM_POINTER_LOCATION 0x80000000

// The define "MAX_NUM_UPBINS" is used in "DEV_HOST_diagAnlgInputVar_t" below.
// This value can never be changed (for host intf. backwards compatibility)
#define MAX_NUM_UPBINS 64

// -----------------------------------------------
// Begin common enumerations between DSP and host.
// -----------------------------------------------

// These Host-to-DSP commands are organized into two groups:
// immediate state change commands and status affecting commands.
// Do not add or remove commands except at the bottom since the DSP assumes this sequence.

enum
{
  HOST_ACTREQ,         // Send R-ACKREQ and monitor for C-ACKx
  HOST_QUIET,          // Sit quietly doing nothing for about 60 seconds, DEFAULT STATE; R_IDLE
  HOST_XMITBITSWAP,    // Perform upstream bitswap - FOR INTERNAL USE ONLY
  HOST_RCVBITSWAP,     // Perform downstream bitswap - FOR INTERNAL USE ONLY
  HOST_RTDLPKT,        // Send a remote download packet - FOR INTERNAL USE ONLY
  HOST_CHANGELED,      // Read the LED settings and change accordingly
  HOST_IDLE,           // Sit quiet
  HOST_REVERBTEST,     // Generate REVERB for manufacturing test
  HOST_CAGCTEST,       // Set coarse receive gain for manufacturing test
  HOST_DGASP,          // send Dying Gasp messages through EOC channel
  HOST_GHSREQ,         // G.hs - FOR INTERNAL USE ONLY
  HOST_GHSMSG,         // G.hs - FOR INTERNAL USE ONLY
  HOST_GHS_SENDGALF,   // G.hs - FOR INTERNAL USE ONLY
  HOST_GHSEXIT,        // G.hs - FOR INTERNAL USE ONLY
  HOST_GHSMSG1,        // G.hs - FOR INTERNAL USE ONLY
  HOST_HYBRID,         // Enable/Disable automatic hybrid switch
  HOST_RJ11SELECT,     // RJ11 inner/outer pair select
  HOST_DIGITAL_MEM,    // Digital Diags: run external memory tests
  HOST_TXREVERB,       // AFE Diags: TX path Reverb
  HOST_TXMEDLEY,       // AFE Diags: TX path Medley
  HOST_RXNOISEPOWER,   // AFE Diags: RX noise power
  HOST_ECPOWER,        // AFE Diags: RX eco power
  HOST_ALL_ADIAG,      // AFE Diags: all major analog diagnostic modes. Host is responsible to initiate each diagnostic sessions
  HOST_USER_ADIAG,     // AFE Diags: Host fills in analog diagnostic input data structure as specified and requests DSP to perform measurements as specified
  HOST_QUIT_ADIAG,     // AFE Diags: Host requests DSP to quit current diagnostic session. This is used for stopping the transmit REVERB/MEDLEY
  HOST_NO_CMD,         // All others - G.hs - FOR INTERNAL USE ONLY
  HOST_DSLSS_SHUTDOWN, // Host initiated DSLSS shutdown message
  HOST_SET_GENERIC,    // Set generic CO profile
  HOST_UNDO_GENERIC,   // Set profile previous to Generic
  HOST_GHS_CLEARDOWN,  // G.hs - FOR INTERNAL USE ONLY to start cleardown
// * UR8_MERGE_START CQ10880   Jack Zhang
  HOST_L3_MSG          // Send a message to DSP for L3
// * UR8_MERGE_END   CQ10880*  
};

// These DSP-to-Host responses are organized into two groups:
// responses to commands and requests for OAM services.

enum
{
  DSP_IDLE,               // R_IDLE state entered
  DSP_ACTMON,             // R_ACTMON state entered
  DSP_TRAIN,              // R_TRAIN state entered
  DSP_ACTIVE,             // R_ACTIVE state entered
  DSP_XMITBITSWAP,        // Upstream bitswap complete - FOR INTERNAL USE ONLY
  DSP_RCVBITSWAP,         // Downstream bitswap complete - FOR INTERNAL USE ONLY
  DSP_RTDL,               // R_RTDL state entered - FOR INTERNAL USE ONLY
  DSP_RRTDLPKT,           // RTDL packet received - FOR INTERNAL USE ONLY
  DSP_XRTDLPKT,           // RTDL packet transmitted - FOR INTERNAL USE ONLY
  DSP_ERROR,              // Command rejected, wrong state for this command
  DSP_REVERBTEST,         // Manufacturing REVERB test mode entered
  DSP_CAGCTEST,           // Manufacturing receive gain test done
  DSP_OVERLAY_START,      // Notify host that page overlay has started - overlay number indicated by "tag"
  DSP_OVERLAY_END,        // Notify host that page overlay has ended   - overlay number indicated by "tag"
  DSP_CRATES1,            // CRATES1 message is valid and should be copied to host memory now
  DSP_SNR,                // SNR calculations are ready and should be copied to host memory now
  DSP_GHSMSG,             // G.hs - FOR INTERNAL USE ONLY
  DSP_RCVBITSWAP_TIMEOUT, // Acknowledge Message was not received within ~500 msec (26 Superframes).
  DSP_ATM_TC_SYNC,        // Indicates true TC sync on both the upstream and downstream. Phy layer ready for data xfer.
  DSP_ATM_NO_TC_SYNC,     // Indicates loss of sync on phy layer on either US or DS.
  DSP_HYBRID,             // DSP completed hybrid switch
  DSP_RJ11SELECT,         // DSP completed RJ11 inner/outer pair select
  DSP_INVALID_CMD,        // Manufacturing (Digital and AFE) diags: CMD received not recognized
  DSP_TEST_PASSED,        // Manufacturing diags: test passed
  DSP_TEST_FAILED,        // Manufacturing diags: test failed
  DSP_TXREVERB,           // Manufacturing AFE diags: Response to HOST_TXREVERB
  DSP_TXMEDLEY,           // Manufacturing AFE diags: Response to HOST_TXMEDLEY
  DSP_RXNOISEPOWER,       // Manufacturing AFE diags: Response to HOST_RXNOISEPOWER
  DSP_ECPOWER,            // Manufacturing AFE diags: Response to HOST_ECPOWER
  DSP_ALL_ADIAG,          // Manufacturing AFE diags: Response to HOST_ALL_ADIAG
  DSP_USER_ADIAG,         // Manufacturing AFE diags: Response to HOST_USER_ADIAG
  DSP_QUIT_ADIAG,         // Manufacturing AFE diags: Response to HOST_QUIT_ADIAG
  DSP_DGASP,              // DSP Message to indicate dying gasp
  DSP_EOC,                // DSP Message to indicate that DSP sent an EOC message to CO
  DSP_TRAINING_MSGS,      // DSP Message to indicate that host has to copy the training message specified in the tag field.
  DSP_CLEAR_EOC,          // DSP Message to indicate that the Rx buffer is full and ready for host to reead
  // UR8_MERGE_START SRA Tim Bornemisza
  DSP_LOF,                // DSP Message to indicate an LOF alarm is being set or cleared
  DSP_SRA                 // Downstream SRA complete - FOR INTERNAL USE ONLY
  // UR8_MERGE_END SRA Tim Bornemisza
};

// Definitions used to indicate which modes are allowed by HAL. The datapump looks at
// these to determine the contents of transmitted handshake messages and to figure out
// what are the allowed modes in which the modem can potentially train up.
// Note that at this time ANNEX I and J are not supported and these bits are
// effectively ignored.
#define NO_MODE                       0x0000
#define DELT_ENABLE                   0x0001
#define GDMT_ANNEX_A_OR_B             0x0002
#define GLITE_ANNEX_A_AND_B           0x0004
#define ADSL2_ANNEX_A_OR_B            0x0008
#define ADSL2PLUS_ANNEX_A_OR_B        0x0010
#define ADSL2_ANNEX_L                 0x0020
#define T1413_ANSI                    0x0080
#define ADSL2_ANNEX_I                 0x0100
#define ADSL2_ANNEX_J                 0x0200
#define ADSL2_ANNEX_M                 0x0400
#define ADSL2PLUS_ANNEX_I             0x0800
#define ADSL2PLUS_ANNEX_J             0x1000
#define ADSL2PLUS_ANNEX_M             0x2000
#define ADSL2_ANNEX_A_OR_B_DELT       (ADSL2_ANNEX_A_OR_B     + DELT_ENABLE)
#define ADSL2PLUS_ANNEX_A_OR_B_DELT   (ADSL2PLUS_ANNEX_A_OR_B + DELT_ENABLE)
#define ADSL2_ANNEX_L_DELT            (ADSL2_ANNEX_L          + DELT_ENABLE)
#define MULTI_MODE                    0xffff
#define MULTI_MODE8                   0xff        // 8 bit Multi_Mode for backward compatibility

#define ADSL2_MASKS  (ADSL2_ANNEX_A_OR_B | ADSL2_ANNEX_I | ADSL2_ANNEX_J | ADSL2_ANNEX_L | ADSL2_ANNEX_M)
#define ADSL2PLUS_MASKS (ADSL2PLUS_ANNEX_A_OR_B | ADSL2PLUS_ANNEX_I | ADSL2PLUS_ANNEX_J | ADSL2PLUS_ANNEX_M)

// Defintions used to indicate the standard in which the modem trained up.
// Values for trainMode field of DEV_HOST_dspWrNegoParaDef_t - please note this refers to standard
// used. Please be careful while changing this. Changing the bit definitions of this byte can
// BREAK backward compatibility with HAL and ATM drivers versions earlier than D4.1
// Typically the bit field definitions for this field should be the same as Least Significant Byte of
// DEV_HOST_oamWrNegoParaDef_t.stdMode
// Please note that MULTI_MODE as defined in stdMode field of DEV_HOST_oamWrNegoParaDef_t is not
// required as this field refers to any one selected mode

#define GDMT_MODE                     0x02
#define GLITE_MODE                    0x04
#define ADSL2_MODE                    0x08
#define ADSL2PLUS_MODE                0x10
#define T1413_MODE                    0x80
#define ADSL2_DELT                    (ADSL2_MODE     + DELT_ENABLE)
#define ADSL2PLUS_DELT                (ADSL2PLUS_MODE + DELT_ENABLE)

#define BIS_STDS_MASK                 (ADSL2_MODE | ADSL2PLUS_MODE | DELT_ENABLE)

// Definitions used to indicate the Annex in which the modem trained up.
// Values for annex_selected field of DEV_HOST_dspWrNegoParaDef_t
enum
{
  ANNEXA                  = 0x1,
  ANNEXB                  = 0x2,
  ANNEXC                  = 0x4,
  ANNEXI                  = 0x8,
  ANNEXJ                  = 0x10,
  ANNEXL                  = 0x20,
  ANNEXM                  = 0x40
};

// Definitions for psd_mask_qualifier field of DEV_HOST_dspWrNegoParaDef_t

// Bit positions of the fields in DEV_HOST_dspWrNegoParaDef_t.psd_mask_qualifier
#define OVERLAPPED_SPEC_TYPE_BITPOS          0
#define ANNEXL_TYPE_BITPOS                   1 // Also called READSL
#define ANNEXMJ_SUBMODE_MASK_BITPOS          2
#define ANNEXMJ_US_INBAND_PSD_SHAPE_BITPOS   6
// Bit position 7 reserved for Annex J or M related use
#define ANNEXB_TONES1TO32_SUPP_BITPOS        8


// Overlapped/Non-overlapped Spectrum
// 1 - DS/US Overlapped spectrum, 0 - Non overlapped spectrum
#define OVERLAPPED_SPECTRUM_USED             (0x0001<<OVERLAPPED_SPEC_TYPE_BITPOS)

// READSL_TYPE Bit Field Definitions DEV_HOST_dspWrNegoParaDef_t.psd_qualifier
// 1-narrowband READSL / 0-wideband READSL
#define NB_ANNEXL                            (0x0001<<ANNEXL_TYPE_BITPOS)
#define WB_ANNEXL                            (0x0000<<ANNEXL_TYPE_BITPOS)

// Bit Masks to select the Sub Mode Mask Value of DEV_HOST_dspWrNegoParaDef_t.psd_qualifier
// and extract it
#define ANNEXM_US_SUBMODE_BITMASK            0x003c  // 4 bit field used for Qualifying the Annex J/M Upstream Mask

// values of the US_INBAND_PSD_SHAPE bits of the psd_qualifier field
#define ANNEXMJ_SUBMODE_MASK_32               0x00
#define ANNEXMJ_SUBMODE_MASK_36               0x01
#define ANNEXMJ_SUBMODE_MASK_40               0x02
#define ANNEXMJ_SUBMODE_MASK_44               0x03
#define ANNEXMJ_SUBMODE_MASK_48               0x04
#define ANNEXMJ_SUBMODE_MASK_52               0x05
#define ANNEXMJ_SUBMODE_MASK_56               0x06
#define ANNEXMJ_SUBMODE_MASK_60               0x07
#define ANNEXMJ_SUBMODE_MASK_64               0x08  // 0x9 to 0xA are reserved

// Inband Spectral Shaping Support
// PSD Shaping Suported -1, Not Supported = 0
#define ANNEXMJ_US_INBAND_SPECSHAPING_USED   (0x0001<<ANNEXMJ_US_INBAND_PSD_SHAPE_BITPOS)

// Annex B US Tones 1 to 32 Support
// US Tones 1 to 32 used -1, Not used = 0
#define ANNEXB_US_TONES_1TO32_USED           (0x0001<<ANNEXB_TONES1TO32_SUPP_BITPOS)

// Define the reason for dropping the connection
enum
{
  REASON_LOS         = 0x01,
  REASON_DYING_GASP  = 0x02,
  REASON_USCRCERR    = 0x04,
  REASON_MARGIN_DROP = 0x08
};

// Define bit fields for PMD layer feature enable/disable
enum
{
  ENABLE_ONE_BIT_SUPPORT       =  BIT0,
  ENABLE_S_ONE_BY_FOUR         =  BIT1,
  ENABLE_TONE_BLACKOUT         =  BIT2,
  //UR8_MERGE_START_END CQ10927 HZ
  DISABLE_RANDOM_TONE_ORDER     =  BIT3,
  ENABLE_2TONE_GHS_DETECTION   =  BIT4,
  ENABLE_CAPPED_RATE_SUPPORT   =  BIT5,
  DISABLE_EXTENDED_FRAMING_SUPPORT = BIT7,    // CQ10004
  ENABLE_EXTENDED_INP_SUPPORT      = BIT8,    // CQ10222
// UR8_MERGE_START API-Bits PeterHou
  ENABLE_PHY_SRA_SUPPORT           = BIT9,    // CQ10351
  ENABLE_PHY_INP_DSCRC_IMPROVEMENT = BIT10,   // CQ10375
  ENABLE_T1413_ACTIVATION_FIRST    = BIT11,    // CQ10495
// UR8_MERGE_END API-Bits
// UR8_MERGE_START_END CQ10617 AdeelJ
  ENABLE_INFN_MINMAR_PARSE_N_CHK   = BIT12,    // CQ10617

// UR8_MERGE_START_END CQ10516 MH
  ENABLE_PHY_COOK_INP_TRAINING     = BIT13,    // CQ10516 MH

// UR8_MERGE_START_END CQ10665 NF
  ENABLE_PHY_EXTENDED_PILOT_SEARCH  = BIT14,   //CQ10665 NF

// UR8_MERGE_START APIBits ManjulaK
  ENABLE_LOWER_GHS_TONE_POWER       = BIT15,    //CQ10781
// UR8_MERGE_END APIBits
// UR8_MERGE_START_END CQ10566 MB
  ENABLE_PHY_GHSCABMODE    =  BIT16,   //CQ10566
// UR8_MERGE_START_END CQ10913_AC7V30 YW  
  DISABLE_ANNEXM_SUPPORT       =  BIT17,       // CQ10913
// UR8_MERGE_START_END CQ10960 HZ  
  ENABLE_TIME_ERROR_SCALE_CNXT = BIT18,         //CQ10960 HZ
  // UR8_MERGE_START_END CQ11277 Ram
  ENABLE_ADSL2_2PLUS_HLIN = BIT19,    //CQ11277 Ram
  // UR8_MERGE_START_END CQ11281 Ram
  ENABLE_GHS_SHORT_RESET = BIT20,     //CQ11281 Ram
  // UR8_MERGE_START CQ11004 Nima
  // UR8_MERGE_START_END Renamed ENABLE_PHY_TI_INTERNAL2 to DISABLE_PHY_TI_INTERNAL2
  DISABLE_PHY_TI_INTERNAL2    = BIT29,    //CQ11004
// UR8_MERGE_END CQ11004
// UR8_MERGE_START API-Bit ManjulaK
  ENABLE_PHY_TI_INTERNAL1    = BIT30    //CQ10202
// UR8_MERGE_END API-Bit

};

// UR8_MERGE_START_END     CQ10927 HZ : Removed CONTROL enum here.

// Define bit fields for interop feature enable/disable
enum
{
  DISABLE_FORCED_T1413_GSPV_REV2           =  BIT0,
  ENABLE_SHORT_LOOP_US_ERR_REDUCTION       =  BIT1,  //CQ9459
  ENABLE_T1413_LONG_LOOP_ACTIVATION        =  BIT2,
  ENABLE_QWEST_ALCATEL_US_LOW_RATE_FIX     =  BIT3,
  ENABLE_ASB_ETSIB_HIGH_BER_FIX            =  BIT4,
  ENABLE_BELL_CANADA_DOUBLE_BRIDGE_TAP_FIX =  BIT5,
  ENABLE_BELGACOM_DS_FIX                   =  BIT6,
  ENABLE_CINCINNATI_BELL_FIX               =  BIT7,
  ENABLE_FRANCE_TELECOM_FIX                =  BIT8,
  ENABLE_QWEST_US_1M_RATE_FIX              =  BIT9,
  DISABLE_SAFEMODE_FOR_G992_3_5            =  BIT10,  // CQ9500
  ENABLE_ADI_UPSTREAM_RATES_FIX            =  BIT11,  // CQ9620 MH 05/16/
  ENABLE_NOKIA_D50_GDMT_RETRAIN_FIX        =  BIT12,  // CQ9613
  ENABLE_WESTELL_RFI_OPTION                =  BIT13,  // CQ9787 - Westell RFI option
  ENABLE_REPORT_AVG_MARGIN                 =  BIT14,  // CQ10012 Ram Avg Margin Reporting; recycled unused bit14
  DISABLE_CNXT_CLREOC_PATCH                =  BIT15,  // CQ9885 - clearEoc Fix
  ENABLE_MAXIMIZE_INP_SUPPORT              =  BIT16,  // CQ10045 - Maximize the INP support in fixed and capped rates
  ENABLE_TELEFONICA_FIXED_MARGIN           =  BIT17,  // CQ10173 - Force to drop line if  margin < targetmargin
  ENABLE_MINIMIZE_INTERLEAVER_DELAY        =  BIT18,  // CQ10045 - Minimize the interleaver dely in fixed and capped rates
  ENABLE_ANNEXB_US_STARTBIN                =  BIT19,  //CQ10198 Extend Annex B US to bin 28
// UR8_MERGE_START API-Bits PeterHou
  ENABLE_DETECT_MORE_COMB1TONES            =  BIT20,  // CQ10385 - Detect more Comb1 tones to combat Comb1-like crosstalk noise in ADSL2/2+ mode
  ENABLE_CNXT_US_FLAVOR_B                  =  BIT21,   // CQ10471 - Use CNXT Upstream flavor B in G.DMT & T1.413 mode
// UR8_MERGE_END API-Bits
// UR8_MERGE_START CQ10600 ManjulaK
  ENABLE_ETISALAT_US_CRC_N_LINEDROP_FIX    = BIT22,    //CQ10600- add quantization noise to upstream signal from G2_MedleyA to G2_MedleyH state
// UR8_MERGE_END CQ10600
  ENABLE_TELIA_SONERA_FIX                  = BIT23,    //UR8_MERGE_START_END CQ10773, CQ10784 Ram
// UR8_MERGE_START_END CQ10899 
  ENABLE_ECHO_BAND_DS_SNR_CUTBACK          = BIT24,     //SNR cutback for Showtime stability
// UR8_MERGE_START_END CQ11075_API25 YW
  ENABLE_LINE_STABILITY_2DBPAD_MARGIN      = BIT25,  
//UR8_MERGE_START CQ11023 Hao-Ting Lin
  ENABLE_PCCW_CNXT_FIX                     = BIT26,
  //UR8_MERGE_END CQ11023
  //UR8_MERGE_START CQ11031 Hao-Ting Lin
  ENABLE_VERSION_NO_16BYTES                = BIT27,
  //UR8_MERGE_END CQ11031
//UR8_MERGE_START CQ11021 CQ11022 Hao-Ting Lin
  ENABLE_NORWAY_COMLAB_FIX                 = BIT28,
//UR8_MERGE_END CQ11021 CQ11022 Hao-Ting Lin
  ENABLE_OPTUS_BTLOOP_FIX                  = BIT29,   // UR8_MERGE_START_END CQ10819 Ram
// UR8_MERGE_START_END CQ11080 Manjula/KC
  ENABLE_CTLM_LOW_USRATE_FIX               = BIT30
};

enum
{
  CONTROL_FORCED_T1413_GSPV_REV2             =  DISABLE_FORCED_T1413_GSPV_REV2,
  CONTROL_SHORT_LOOP_US_ERR_REDUCTION        =  ENABLE_SHORT_LOOP_US_ERR_REDUCTION,    //CQ9459
  CONTROL_T1413_LONG_LOOP_ACTIVATION         =  ENABLE_T1413_LONG_LOOP_ACTIVATION,
  CONTROL_QWEST_ALCATEL_US_LOW_RATE_FIX      =  ENABLE_QWEST_ALCATEL_US_LOW_RATE_FIX,
  CONTROL_ASB_ETSIB_HIGH_BER_FIX             =  ENABLE_ASB_ETSIB_HIGH_BER_FIX,
  CONTROL_BELL_CANADA_DOUBLE_BRIDGE_TAP_FIX  =  ENABLE_BELL_CANADA_DOUBLE_BRIDGE_TAP_FIX,
  CONTROL_BELGACOM_DS_FIX                    =  ENABLE_BELGACOM_DS_FIX,
  CONTROL_CINCINNATI_BELL_FIX                =  ENABLE_CINCINNATI_BELL_FIX,
  CONTROL_FRANCE_TELECOM_FIX                 =  ENABLE_FRANCE_TELECOM_FIX,
  CONTROL_QWEST_US_1M_RATE_FIX               =  ENABLE_QWEST_US_1M_RATE_FIX,
  CONTROL_SAFEMODE_FOR_G992_3_5              =  DISABLE_SAFEMODE_FOR_G992_3_5,         // CQ9500
  CONTROL_ADI_UPSTREAM_RATES_FIX             =  ENABLE_ADI_UPSTREAM_RATES_FIX,         // CQ 9620 MH 05/16/05
  CONTROL_NOKIA_D50_GDMT_RETRAIN_FIX         =  ENABLE_NOKIA_D50_GDMT_RETRAIN_FIX,     // CQ9613
  CONTROL_WESTELL_RFI_OPTION                 =  ENABLE_WESTELL_RFI_OPTION,             // CQ9787 - Westell RFI option
  CONTROL_REPORT_AVG_MARGIN                  =  ENABLE_REPORT_AVG_MARGIN,              // CQ10012 Ram Avg Margin Reporting; recycled unused bit14
  CONTROL_CNXT_CLREOC_PATCH                  =  DISABLE_CNXT_CLREOC_PATCH,             // CQ9885 clearEoc fix
  CONTROL_MAXIMIZE_INP_SUPPORT               =  ENABLE_MAXIMIZE_INP_SUPPORT,           // CQ10045 - Maximize the INP support in fixed and capped rates
  CONTROL_TELEFONICA_FIXED_MARGIN            =  ENABLE_TELEFONICA_FIXED_MARGIN,        // CQ10173
  CONTROL_MINIMIZE_INTERLEAVER_DELAY         =  ENABLE_MINIMIZE_INTERLEAVER_DELAY,     // CQ10045 - Minimize the interleaver delay in fixed and capped rates
  CONTROL_ANNEXB_US_STARTBIN                 =  ENABLE_ANNEXB_US_STARTBIN,             // CQ10198 Extend Annex B US to bin 28
// UR8_MERGE_START API-Bits PeterHou
  CONTROL_DETECT_MORE_COMB1TONES             =  ENABLE_DETECT_MORE_COMB1TONES,         // CQ10385 - Detect more Comb1 tones to combat Comb1-like crosstalk noise in ADSL2/2+ mode.
  CONTROL_CNXT_US_FLAVOR_B                   =  ENABLE_CNXT_US_FLAVOR_B,               // CQ10471 - Use CNXT Upstream flavor B in G.DMT & T1.413 mode
// UR8_MERGE_END API-Bits
  // UR8_MERGE_START CQ10600 ManjulaK
  CONTROL_ETISALAT_US_CRC_N_LINEDROP_FIX     =  ENABLE_ETISALAT_US_CRC_N_LINEDROP_FIX,  //CQ10600- add quantization noise to upstream signal from G2_MedleyA to G2_MedleyH state
  // UR8_MERGE_END CQ10600
  CONTROL_TELIA_SONERA_FIX                   =  ENABLE_TELIA_SONERA_FIX,
  // UR8_MERGE_START_END_CQ10899 MH
  CONTROL_ECHO_BAND_DS_SNR_CUTBACK           =  ENABLE_ECHO_BAND_DS_SNR_CUTBACK,
  // UR8_MERGE_START_END CQ11075_API25 YW
  CONTROL_LINE_STABILITY_2DBPAD_MARGIN       =  ENABLE_LINE_STABILITY_2DBPAD_MARGIN,
  //UR8_MERGE_START CQ11023 Hao-Ting Lin
  CONTROL_PCCW_CNXT_FIX                      =  ENABLE_PCCW_CNXT_FIX,
  //UR8_MERGE_END CQ11023
  //UR8_MERGE_START CQ11031 Hao-Ting Lin
  CONTROL_VERSION_NO_16BYTES                 =  ENABLE_VERSION_NO_16BYTES,
  //UR8_MERGE_END CQ11031
  //UR8_MERGE_START CQ11021 CQ11022 Hao-Ting Lin
  CONTROL_NORWAY_COMLAB_FIX                  =  ENABLE_NORWAY_COMLAB_FIX,
  //UR8_MERGE_END CQ11021 CQ11022 Hao-Ting Lin
  // UR8_MERGE_START_END CQ10819 Ram
  CONTROL_OPTUS_BTLOOP_FIX                   =  ENABLE_OPTUS_BTLOOP_FIX,
  // UR8_MERGE_START_END CQ11080 Manjula/KC
  CONTROL_CTLM_LOW_USRATE_FIX                =  ENABLE_CTLM_LOW_USRATE_FIX 
};

//interop_1
enum{
   // UR8_MERGE_START_START_END CQ11230
   ENABLE_TR067_DSPCB_FIX                   = BIT0
};

enum{
  // UR8_MERGE_START_START_END CQ11230
  CONTROL_TR067_DSPCB_FIX                    =  ENABLE_TR067_DSPCB_FIX  
};

enum  //(CQ10242)
{
    CLI2MCBSP,       // send CLI buffer to McBSP only
    CLI2HOST,        // send CLI buffer to host
    CLI2BOTH         // send CLI biffer to McBSP AND Host
};

// ----------------------------------------------------
// Begin modem state bit field definitions - DSP write.
// ----------------------------------------------------

// BitField1 for initial states and G.hs states.
// If more values need to be added, they will be added at the end (up to 32 total entries).  However, if this causes
// the state transitions to tick off out of normal bit order, then the C code will have to be re-written
// that causes the proper values to be entered into the modem state bit fields.
typedef enum
{
  ZERO_STATE1 = 0,
  RTEST       = 0x1,
  RIDLE       = 0x2,
  RINIT       = 0x4,
  RRESET      = 0x8,
  GNSFLR      = 0x10,
  GTONE       = 0x20,
  GSILENT     = 0x40,
  GNEGO       = 0x80,
  GFAIL       = 0x100,
  GACKX       = 0x200,
  GQUIET2     = 0x400
} DEV_HOST_stateBitField1_t; // this enum should only have 32 bit entries in it.  Add another enum if you need more.

// BitField2 for T1.413 states and for the rest of the modem states (so far)
// If more values need to be added, they will be added at the end (up to 32 total entries).  However, if this causes
// the state transitions to tick off out of normal bit order, then the C code will have to be re-written
// that causes the proper values to be entered into the modem state bit fields.
typedef enum
{
  ZERO_STATE2 = 0,
  TNSFLR      = 0x1,
  TACTREQ     = 0x2,
  TACTMON     = 0x4,
  TFAIL       = 0x8,
  TACKX       = 0x10,
  TQUIET2     = 0x20,
  RQUIET2     = 0x40,
  RREVERB1    = 0x80,
  RQUIET3     = 0x100,
  RECT        = 0x200,
  RREVERB2    = 0x400,
  RSEGUE1     = 0x800,
  RREVERB3    = 0x1000,
  RSEGUE2     = 0x2000,
  RRATES1     = 0x4000,
  RMSGS1      = 0x8000,
  RMEDLEY     = 0x10000,
  RREVERB4    = 0x20000,
  RSEGUE3     = 0x40000,
  RMSGSRA     = 0x80000,
  RRATESRA    = 0x100000,
  RREVERBRA   = 0x200000,
  RSEGUERA    = 0x400000,
  RMSGS2      = 0x800000,
  RRATES2     = 0x1000000,
  RREVERB5    = 0x2000000,
  RSEGUE4     = 0x4000000,
  RBNG        = 0x8000000,
  RREVERB6    = 0x10000000,
  RSHOWTIME   = 0x20000000
} DEV_HOST_stateBitField2_t;  // this enum should only have 32 bit entries in it.  Add another enum if you need more.

// BitField3 for ADSL2 states
// If more values need to be added, they will be added at the end (up to 32 total entries).  However, if this causes
// the state transitions to tick off out of normal bit order, then the C code will have to be re-written
// that causes the proper values to be entered into the modem state bit fields.
typedef enum
{
  ZERO_STATE3 = 0,
  G2QUIET1    = 0x1,
  G2COMB1     = 0x2,
  G2QUIET2    = 0x4,
  G2COMB2     = 0x8,
  G2ICOMB1    = 0x10,
  G2LINEPROBE = 0x20,
  G2QUIET3    = 0x40,
  G2COMB3     = 0x80,
  G2ICOMB2    = 0x100,
  G2RMSGFMT   = 0x200,
  G2RMSGPCB   = 0x400,
  G2REVERB1   = 0x800,
  G2QUIET4    = 0x1000,
  G2REVERB2   = 0x2000,
  G2QUIET5    = 0x4000,
  G2REVERB3   = 0x8000,
  G2ECT       = 0x10000,
  G2REVERB4   = 0x20000,
  G2SEGUE1    = 0x40000,
  G2REVERB5   = 0x80000,
  G2SEGUE2    = 0x100000,
  G2RMSG1     = 0x200000,
  G2MEDLEY    = 0x400000,
  G2EXCHANGE  = 0x800000,
  G2RMSG2     = 0x1000000,
  G2REVERB6   = 0x2000000,
  G2SEGUE3    = 0x4000000,
  G2RPARAMS   = 0x8000000,
  G2REVERB7   = 0x10000000,
  G2SEGUE4    = 0x20000000
} DEV_HOST_stateBitField3_t;  // this enum should only have 32 bit entries in it.  Add another enum if you need more.

// BitField4 for ADSL2 diag. states
// If more values need to be added, they will be added at the end (up to 32 total entries).  However, if this causes
// the state transitions to tick off out of normal bit order, then the C code will have to be re-written
// that causes the proper values to be entered into the modem state bit fields.
typedef enum
{
  ZERO_STATE4 = 0,
  GDSEGUE1    = 0x1,
  GDREVERB5   = 0x2,
  GDSEGUE2    = 0x4,
  GDEXCHANGE  = 0x8,
  GDSEGUELD   = 0x10,
  GDRMSGLD    = 0x20,
  GDQUIET1LD  = 0x40,
  GDQUIET2LD  = 0x80,
  GDRACK1     = 0x100,
  GDRNACK1    = 0x200,
  GDQUIETLAST = 0x400
} DEV_HOST_stateBitField4_t;  // this enum should only have 32 bit entries in it.  Add another enum if you need more.

// This struct collects all of the bitfield types listed above for the modem state bit field(s)
typedef struct
{
  DEV_HOST_stateBitField1_t bitField1; // this is the first modem state bit field (mostly init. and G.hs)
  DEV_HOST_stateBitField2_t bitField2; // this is the second modem state bit field (T1.413 and G.dmt)
  DEV_HOST_stateBitField3_t bitField3; // this is the third modem state bit field (ADSL2)
  DEV_HOST_stateBitField4_t bitField4; // this is the fourth modem state bit field (ADSL2 diag.)
} DEV_HOST_modemStateBitField_t;


// -----------------------------------------------
// Begin NegoPara message definitions - DSP write.
// -----------------------------------------------
// CQ9600: Maximym CMSGPCB2_length is Fixed CMSG_PCB length (4) +
//         MAX_NSCus (64)/8 = 2 + 8 =12
//         Note that in DELT mode the length is 2 bytes more than in non DELT mode.
//         The constant below takes DELT into account.
#define MAX_CMSGPCB2_LENGTH              12

// CQ9600: Definition of CMSGPCB2 Length with Blackouts. The cMsgs1 field
//         DEV_HOST_dspWrNegoParaDef_t structure below has to have the maximum
//         of CMSGPCB length in ADSl2/2+ and CMSGS1 in ADSL1/T1.413

typedef struct
{
  UINT8  trainMode;          // Train mode selected.  See training modes defined above.
  UINT8  bDummy1;            // dummy byte for explicit 32-bit alignment
  UINT16 lineLength;         // Contains loop length estimate.  Accuracy w/i 500 ft.  LSbit = 1 for straight loop, = 0 for bridge tap
  UINT32 atucVendorId;       // Pass the vendor id of the CO to the host
  UINT8  cMsgs1[MAX_CMSGPCB2_LENGTH];    // Array is used for both cMsgs1 & cMsg-PCB
  UINT16 adsl2DSRate;        //
  UINT8  cRates2;            //
  UINT8  rRates2;            //
  UINT8  rRates1[4][11];     //
  UINT8  cMsgs2[4];          //
  UINT8  cRates1[4][30];     //
  UINT8  rMsgs2[4];          // This always needs to be aligned on 4 byte boundary.
  UINT16 adsl2USRate;        //
  UINT8  atucGhsRevisionNum; // Pass the G.hs Revision number of the CO to the host
  UINT8  reserved1;          //
  PUINT8 *adsl2DeltMsgs_p;   // This pointer to a pointer passes the address of the globalvar.pString, which is also
                             // a pointer list of pointers. It will be used to pass all the new ADSL2 DELT messages to
                             // host side. This is for ACT.
  UINT8  rcvMode;            // rcvMode for physical layer transactions. Trainmode will be used for all messaging
// UR8_MERGE_START_END CQ11194 HZ  
  UINT8  bDummy2;            // for integer alignment
  UINT16 annex_selected;     // Annex that the modem trained up with.
  UINT16 psd_mask_qualifier; // Bit definitions given above in the files.
// UR8_MERGE_START_END CQ11194 HZ
  SINT16 raOVRate;           // Store the overhead data rate calculated at training time. 
} DEV_HOST_dspWrNegoParaDef_t;
 

// ----------------------------------------------------
// Begin OAM NegoPara message definitions - Host write.
// ----------------------------------------------------

// OAM Feature bit fields.
//
// Bit    0 - Enable auto retrain of modem
// Bit    1 - Detect and report TC sync to host
// Bit 2-31 - Reserved

#define DEV_HOST_AUTORETRAIN_ON       0x00000001
#define DEV_HOST_TC_SYNC_DETECT_ON    0x00000002

#define DEV_HOST_AUTORETRAIN_MASK       0x00000001
#define DEV_HOST_TC_SYNC_DETECT_MASK    0x00000002
#define DEV_HOST_EOCAOC_INTERRUPT_MASK  0x00000004
#define DEV_HOST_CONS_DISP_DISABLE_MASK 0x00000008
#define DEV_HOST_GHSMSG_INTERRUPT_MASK  0x00000010
#define DEV_HOST_SRA_INTERRUPT_MASK     0x00000020 //UR8_MERGE_START_END CQ10774 Ram

typedef struct
{
  UINT8   stdMode;              // Desired train mode.  See training modes defined above.
  UINT8   ghsSequence;          // Selected G.hs session as shown in Appendix 1
  UINT8   usPilotFlag;          // Value of 1 indicates transmit an upstream pilot on bin 16
  UINT8   stdMode_byte2;        // 2nd byte of the desired train mode. This is to be used in conjunction with stdMode.
  UINT8   rMsgs1[38];           // RMSG-1(6) and RMSG_PCB (38)
  UINT16  stdMode_bytes34;      // Reserved for future use
  UINT32  oamFeature;           // 32 bit wide bit field to set OAM-specific features.
  SINT8   marginThreshold;      // Threshold for margin reporting
  UINT8   hostFixAgc;           // flag to force datapump to bypass AGC training and use the following values
  UINT8   hostFixEqualizer;     // forced analog equalizer value used during AGC training when hostfix_agc is on
  UINT8   hostFixPga1;          // forced pga1 value used during AGC training when hostFixAgc is on
  UINT8   hostFixPga2;          // forced pga2 value used during AGC training when hostFixAgc is on
  UINT8   hostFixPga3;          // forced pga3 value used during AGC training when hostFixAgc is on
  UINT8   marginMonitorShwtme;  // margin monitoring flag (during showtime)
  UINT8   marginMonitorTrning;  // margin monitoring flag (during training)
  UINT8   disableLosAlarm;      // flag to disable training based on los
  UINT8   usCrcRetrain;         // flag to disable retrain due to excessive USCRC
  UINT8   t1413VendorId[2];     // Vendor ID used for T1.413 trainings
  UINT8   gdmtVendorId[8];      // Vendor ID used for G.dmt trainings (ITU VendorID)
  UINT8   missingTones[64];     // 64 element array to define missing tones for TX_MEDLEY and TX REVERB tests
  UINT32  missingToneDsAddr;    // Address given to DSP for tones to be switched off in DS direction
  UINT8   dsToneTurnoff_f;      // This flag controls the DS tone turn off logic
  UINT8   mhzFlag;              // Indicates whether DSP runs at 200 v/s 250 Mhz
  UINT8   reserved2;            // Dummy bytes
  UINT8   reserved3;            // Dummy bytes
  UINT32  mode_config;          // This flag tells the DSP the modes which are enabled by OAM.
                                // Due to backward compatibility reasons this variable is not OAM write but is in
                                // fact written by the datapump. This variable is formed by packing stdMode_bytes34,
                                // stdMode_byte2, and stdMode together.                              
} DEV_HOST_oamWrNegoParaDef_t;


// ----------------------------------------
// Begin Rate-adaptive message definitions.
// ----------------------------------------

// The four values below can never be changed (for host intf. backwards compatibility)
#define DEV_HOST_RMSGSRA_LENGTH  10
#define DEV_HOST_RRATESRA_LENGTH 1
#define DEV_HOST_CRATESRA_LENGTH 120
#define DEV_HOST_CMSGSRA_LENGTH  6

typedef struct
{
  UINT8 rRatesRaString[DEV_HOST_RRATESRA_LENGTH+3];
  UINT8 rMsgsRaString[DEV_HOST_RMSGSRA_LENGTH+2];
  UINT8 cMsgsRaString[DEV_HOST_CMSGSRA_LENGTH+2];
} DEV_HOST_raMsgsDef_t;


// ----------------------------------------------
// Begin superframe cnts definitions - DSP write.
// ----------------------------------------------

#define DEV_HOST_FRAMES_PER_SUPER   68
#define DEV_HOST_SUPERFRAMECNTDSTRM 0
#define DEV_HOST_SUPERFRAMECNTUSTRM 4

// Although only the least significant 8 bits should be used as an
// unsigned char for computing the bitswap superframe number, a
// full 32 bit counter is provided here in order to have an
// accurate indicator of the length of time that the modem has
// been connected.  This counter will overflow after 2.35 years
// of connect time.

typedef struct
{
  UINT32 wSuperFrameCntDstrm;
  UINT32 wSuperFrameCntUstrm;
} DEV_HOST_dspWrSuperFrameCntDef_t;


// --------------------------------
// Begin ATUR/ATUC msg definitions.
// --------------------------------

// Grouping used by the DSP to simplify parameter passing.
// All of these are written by the DSP.

typedef struct
{
  UINT16 vendorId;          // TI's vendor ID = 0x0004; Amati's vendor ID = 0x0006
  UINT8  versionNum;        // T1.413 issue number
  UINT8  rateAdapt;         // 0 = fix rate (Default); 1= adaptive rate
  UINT8  trellis;           // 0 = disable trellis(default); 1 = enable trellis
  UINT8  echoCancelling;    // 0 = disable echo cancelling; 1 = enable echo cancelling(default)
  UINT8  maxBits;           // value range: 0-15; default = 15
  UINT8  maxPsd;            //
  UINT8  actualPsd;         //
  UINT8  maxIntlvDepth;     // 0, 1, 2, or 3 for 64, 128, 256, or 512 max depth
  UINT8  framingMode;       // 0 for asynchronous, 1 for synchronous full overhead
                            // 2 for reduced overhead, 3 for merged reduced overhead DSP write.
  UINT8  maxFrameMode;      // maximum framing mode desired.  Nor 0 or 3.
  SINT16 targetMargin;      //
  SINT16 maxAllowedMargin;  //
  SINT16 minRequiredMargin; //
  SINT16 maxTotBits;        //
  UINT8  grossGain;         //
  UINT8  ntr;               // Enable/disable NTR support
  SINT16 loopAttn;          // Loop Attenuation
  UINT8  vendorRevisionNumber;  // Reported Vendor Revision Number
  UINT8  maxbits_ds;        // will be set to the minimum between host and CO setting for maxBits.
// UR8_MERGE_START CQ10448 Ram
  SINT16 attNdrBits;        // Number of bits minus overhead for max ATTNDR computation
// UR8_MERGE_END CQ10448 Ram
} DEV_HOST_msg_t;


// --------------------------------------
// Begin bits and gains table definitions
// --------------------------------------

typedef struct
{
  PUINT8  aturBng_p;            // pointer to ATU-R bits and gains table
  PUINT8  atucBng_p;            // pointer to ATU-C bits and gains table
  PUINT8  bitAllocTblDstrm_p;   // pointer to Downstream Bit Allocation table
  PUINT8  bitAllocTblUstrm_p;   // pointer to Upstream Bit Allocation table
  PSINT8  marginTblDstrm_p;     // pointer to Downstream Margin table
} DEV_HOST_dspWrSharedTables_t;


// ----------------------------------------
// Begin datapump code overlay definitions.
// ----------------------------------------

#define DEV_HOST_PAGE_NUM 11   // number of overlay pages (page 0 + number of overlay pages)

#define MAX_NUM_INIT_STATES 255 // number of states in the DSP state machine

// Never access a struct of this typedef directly.  Always go through the DEV_HOST_olayDpDef_t struct
typedef struct
{
  UINT32 overlayHostAddr;     // source address in host memory
  UINT32 overlayXferCount;    // number of 32bit words to be transfered
  UINT32 overlayDspAddr;      // destination address in DSP's PMEM
} DEV_HOST_olayDpPageDef_t;


typedef struct
{
  UINT32 overlayStatus;                      // Status of current overlay to DSP PMEM
  UINT32 overlayNumber;                      // DSP PMEM overlay page number
  UINT32 overlayState;                       // Indicates whether current state is an overlay state
  DEV_HOST_olayDpPageDef_t *olayDpPage_p[DEV_HOST_PAGE_NUM]; // Def's for the Pages
} DEV_HOST_olayDpDef_t;


// -------------------------
// Begin ATM-TC definitions.
// -------------------------

// TC cell states.
typedef enum
{
  TC_HUNT,
  TC_PRESYNC,
  TC_SYNC
} DEV_HOST_cellDelinState_t;


// --------------------------------------------
// Begin datapump error/statistics definitions.
// --------------------------------------------

// Never access a struct of this typedef directly.  Always go through the DEV_HOST_modemStatsDef_t struct.
typedef struct
{
  UINT32 crcErrors;   // Num of CRC errored ADSL frames
  UINT32 fecErrors;   // Num of FEC errored (corrected) ADSL frames
  UINT32 ocdErrors;   // Out of Cell Delineation
  UINT32 ncdError;    // No Cell Delineation
  UINT32 lcdErrors;   // Loss of Cell Delineation (within the same connection)
  UINT32 hecErrors;   // Num of HEC errored ADSL frames
} DEV_HOST_errorStats_t;


typedef struct
{
  DEV_HOST_errorStats_t *usErrorStatsIntlv_p; // us error stats - interleave path
  DEV_HOST_errorStats_t *dsErrorStatsIntlv_p; // ds error stats - interleave path
  DEV_HOST_errorStats_t *usErrorStatsFast_p;  // us error stats - fast path
  DEV_HOST_errorStats_t *dsErrorStatsFast_p;  // ds error stats - fast path
  UINT32                losErrors;   // Num of ADSL frames where loss-of-signal
  UINT32                sefErrors;   // Num of severly errored ADSL frames - LOS > MAXBADSYNC ADSL frames
  UINT32                farEndLosErrors; // Number of reported LOS defects by the CO.
  UINT32                farEndRdiErrors; // Number of reported RDI defects by the CO.
//  UR8_MERGE_START CQ10979   Jack Zhang
  SINT32                dsACTPSD;    // Downstream actual power spectral density.
  SINT32                usACTPSD;    // Upstream actual power spectral density.
  SINT32                dsHLINSC;    // Downstream linear representation scale.
  PUINT32               dsHLINps_p;  // Downstream linear channel characteristics per subcarrier.
//  UR8_MERGE_END   CQ10979*
//  UR8_MERGE_START CQ10978   Jack Zhang
  UINT8                 pwrStatus;   // DSL Power Management Status.
  UINT8                 pad[3];      // pading to 32 bits.
//  UR8_MERGE_END   CQ10978*
} DEV_HOST_modemStatsDef_t;

// Never access a struct of this typedef directly.  Always go through the DEV_HOST_atmStats_t struct.
typedef struct
{
  UINT32 goodCount;   // Upstream Good Cell Count
  UINT32 idleCount;   // Upstream Idle Cell Count
} DEV_HOST_usAtmStats_t;

// Never access a struct of this typedef directly.  Always go through the DEV_HOST_atmStats_t struct.
typedef struct
{
  UINT32 goodCount;      // Downstream Good Cell Count
  UINT32 idleCount;      // Downstream Idle Cell Count
  UINT32 badHecCount;    // Downstream Bad Hec Cell Count
  UINT32 ovflwDropCount; // Downstream Overflow Dropped Cell Count
  DEV_HOST_cellDelinState_t delineationState; // Indicates current delineation state
} DEV_HOST_dsAtmStats_t;


typedef struct
{
  DEV_HOST_usAtmStats_t *us0_p; // US ATM stats for TC channel 0
  DEV_HOST_dsAtmStats_t *ds0_p; // DS ATM stats for TC channel 0
  DEV_HOST_usAtmStats_t *us1_p; // US ATM stats for TC channel 1
  DEV_HOST_dsAtmStats_t *ds1_p; // DS ATM stats for TC channel 1
} DEV_HOST_atmStats_t;


// ----------------------
// Begin EOC definitions.
// ----------------------

// The two values below can never change (for backwards compatibility of host intf.)
#define DEV_HOST_EOCREG4LENGTH   32
#define DEV_HOST_EOCREG5LENGTH   32

typedef struct
{
  UINT8        eocReg4[DEV_HOST_EOCREG4LENGTH];    // Host/Dsp Write, vendor specific EOC Register 4
  UINT8        eocReg5[DEV_HOST_EOCREG5LENGTH];    // Host/Dsp Write, vendor specific EOC Register 5
  UINT8        vendorId[8];               // Host write
  UINT8        revNumber[4];              // Host, ATU-R Revision Number
  UINT8        serialNumber[32];          // Host write
  UINT8        eocReg4Length;             // Host Write, valid length for EOC register 4
  UINT8        eocReg5Length;             // Host Write, valid length for EOC register 5
  UINT8        selfTestResults[2];        // EOC selftestResults place holder
  UINT32       eocModemStatusReg;         // Dsp Write, status bits to host
  // UR8_MERGE_START CQ11247_TR69_DS_LATN_SATN  YW
  UINT16       lineAtten;               // Dsp Write, line attenuation in 0.1 db step
  //   UR8_MERGE_START   CQ11228 HZ
  SINT16       dsMargin;                  // DSP Write, measured DS margin in 0.1 db precision
 //   UR8_MERGE_END   CQ11228 HZ
  // UR8_MERGE_END CQ11247_TR69_DS_LATN_SATN  YW
  UINT8        aturConfig[30];            // Dsp Write, also used by EOC for ATUR Configuration
  // UR8_MERGE_START_END CQ11247_TR69_DS_LATN_SATN  YW
  SINT8        dummy[2];                  // 32-bit alignment
  UINT8        revNumber_2p[16];          // CQ10037- Host, ATU-R Version Number, used only in ADSL2, ADSL2plus mode.
  // UR8_MERGE_START_END CQ11057 KCCHEN
  // Removed unused variables.
//UR8_MERGE_START_END CQ10989 ManjulaK
  UINT8        selfTestResults_2p[4];     // CQ10989-INVTRY selftestResults place holder for ADSL2, ADSL2plus
} DEV_HOST_eocVarDef_t;

typedef struct
{
  UINT16       endEocThresh;              // Host Write, end of Clear EOC stream threshold
  UINT16       dummy;                     // dummy value to fill gap
  UINT32       dropEocCount;              // Dsp Write, counter of dropped Clear EOC bytes
  UINT16       eocRxLength;               // Host/DSP write, number of valid Rx Clear EOC bytes
  UINT16       eocTxLength;               // Host/DSP write, number of valid Tx Clear EOC bytes
  UINT8        eocRxBuf[64];              // Dsp Write, Buffer for receiving Rx Clear EOC bytes
  UINT8        eocTxBuf[64];              // Host Write, Buffer for writing Tx Clear EOC bytes
} DEV_HOST_clearEocVarDef_t;


// -----------------------------------
// Begin CO profile Definitions.
// -----------------------------------

/* struct size must be a word size            */
typedef struct
{

  SINT16 devCodecRxdf4Coeff[12] ;             // (BOTH) IIR Coefficients
  SINT16 devCodecTxdf2aCoeff[64] ;            // (BOTH) FIR filter coefficients
#if OHIO_SUPPORT
  SINT16 devCodecTxdf2bCoeff[84] ;            // (BOTH) FIR filter coefficients
#else
  SINT16 devCodecTxdf2bCoeff[64] ;            // (BOTH) FIR filter coefficients
#endif
  SINT16 devCodecTxdf1Coeff[12] ;             // (BOTH) IIR filter coefficients
  UINT16 devCodecTxDf2aDen;                   // (BOTH) denominator for IIR filter
  UINT16 devCodecTxDf2bDen;                   // (BOTH) denominator for IIR filter
  SINT16 ctrlMsmSpecGain[32];                 // (BOTH)

  SINT16 phyBitaRateNegIntNoTrellis ;         // (BOTH) value to set
  SINT16 phyBitaRateNegIntTrellis ;           // (BOTH) value to set
  SINT16 phyBitaRateNegFastNoTrellis ;        // (BOTH) value to set
  SINT16 phyBitaRateNegFastTrellis ;          // (BOTH) value to set
  SINT16 phyBitaRsFlag ;                      // (BOTH)
  SINT16 phyBitaFirstSubChannel ;             // (BOTH)
  SINT16 phyBitaMaxFineGainBump;              // max fine gain bump
  SINT16 phyBitaFineGainReduction;            // fine gain reduction
  SINT16 phyBitaMaxDownstreamPowerCutback;    // max downstream power cutback

  SINT16 phySigTxGainReductionAt0kft;         // upstream power reduction at 0 kft.

  SINT16 phyAgcPgaTarget ;                    // (BOTH) compare value

  UINT16 imsg413TxRate ;                      // (BOTH) Tx rate
  SINT16 imsg413RsBytesAdjust ;               // (BOTH) subtract value
  UINT16 imsg413PstringMask ;                 // (POTS) Or'ed into pString[RMSGS1_INDEX][1]
  SINT16 imsg413UsPilot ;                     // (BOTH)??
  UINT16 imsg413SkipPhaseOp ;                 // (POTS)

  UINT16 ctrlMsmSensitivity1 ;                // (BOTH) value to set
  UINT16 ctrlMsmTxPsdShape_f;                 // (BOTH) upstream spectral shaping flag

  UINT16 ovhdAocUsBswapReq_f ;                // (BOTH)value to set
  UINT16 ovhdAocScanMse_f  ;                  // (BOTH)value to set
  UINT16 firstMSESettleTime  ;                // First bitswap starts
  UINT16 mseSettleTime ;                      // Sequential swaps spacing

  UINT16 powerSwapThreshold  ;                // power swap threshold
  UINT32 maxMSEThreshold ;                    // MSE threshold

  SINT16 phyRevFullFirstBin ;                 //
  SINT16 phyRevFullLastBin ;                  //
  SINT16 phyRevFirstBin ;                     //
  SINT16 phyRevLastBin ;                      //
  SINT16 phyMedFirstBin ;                     //
  SINT16 phyMedLastBin ;                      //
  SINT16 phyMedOptionalLastBin;               // Medley last bin - optional

  SINT16 phyEcDelayAdjustment;                // Echo delay adjustment
  SINT16 bSwapThresholdUpdate;                // bSwapThresholdUpdate
  SINT16 phyBitaFastPathExcessFineGainBump;   // Used in phy_bita.c
  SINT16 phyBitaSkipGapAdjustment;            // Used in phy_bita.c
  SINT16 usPilotInT1413ModeInMedley;          // To send Upstream Pilot in medley in T1.413 mode.

  UINT32 profileVendorId ;                    // vendor id

} DEV_HOST_coData_t ;


typedef struct
{
  DEV_HOST_coData_t             * hostProfileBase_p;                    // base address of profile list
  UINT32                        * dspScratchMem_p0;                     // base address of DSP scratch memory
  UINT32                        * dspScratchMem_p1;                     // base address of DSP scratch memory
  UINT32                        * dspScratchMem_p2;                     // base address of DSP scratch memory
  UINT32                        * dspScratchMem_p3;                     // base address of DSP scratch memory
} DEV_HOST_profileBase_t ;


// -----------------------------------
// Begin DSP/Host Mailbox Definitions.
// -----------------------------------

// The 3 values below can never be decreased, only increased.
// If you increase one of the values, you must add more to the
//   initializers in "dev_host_interface.c".
#define DEV_HOST_HOSTQUEUE_LENGTH  8
#define DEV_HOST_DSPQUEUE_LENGTH   8
#define DEV_HOST_TEXTQUEUE_LENGTH  8

// Never access a struct of this typedef directly.  Always go through the DEV_HOST_mailboxControl_t struct.
typedef struct
{
  UINT8 cmd;
  UINT8 tag;
  UINT8 param1;
  UINT8 param2;
} DEV_HOST_dspHostMsg_t;

// Never access a struct of this typedef directly.  Always go through the DEV_HOST_mailboxControl_t struct.
typedef struct
{
  UINT32 msgPart1;
  UINT32 msgPart2;
} DEV_HOST_textMsg_t;

// The structure below has been ordered so that the Host need only write to
// even byte locations to update the indices.

// The Buffer pointers in the struct below each point to a different
//   struct array that has an array size of one of the matching Queue Length
//   values defined above (DEV_HOST_HOSTQUEUE_LENGTH, DEV_HOST_DSPQUEUE_LENGTH,
//   and DEV_HOST_TEXTQUEUE_LENGTH).

typedef struct
{
  UINT8 hostInInx;  // Host write, DSP must never write except for init
  UINT8 bDummy0[3]; //   dummy bytes for explicit 32-bit alignment
  UINT8 hostOutInx; // DSP write, Host must never write
  UINT8 bDummy1[3]; //   dummy bytes for explicit 32-bit alignment
  UINT8 dspOutInx;  // Host write, DSP must never write except for init
  UINT8 bDummy2[3]; //   dummy bytes for explicit 32-bit alignment
  UINT8 dspInInx;   // DSP write, Host must never write
  UINT8 bDummy3[3]; //   dummy bytes for explicit 32-bit alignment
  UINT8 textInInx;  // DSP write, Host must never write
  UINT8 bDummy4[3]; //   dummy bytes for explicit 32-bit alignment
  UINT8 textOutInx; // Host write, DSP must never write except for init
  UINT8 bDummy5[3]; //   dummy bytes for explicit 32-bit alignment
  DEV_HOST_dspHostMsg_t *hostMsgBuf_p;  // pointer to Host Mailbox Buffer (Host writes the buffer)
  DEV_HOST_dspHostMsg_t *dspMsgBuf_p;   // pointer to DSP Mailbox Buffer (DSP writes the buffer)
  DEV_HOST_textMsg_t *textMsgBuf_p;     // pointer to Text Mailbox Buffer (DSP writes the buffer)
} DEV_HOST_mailboxControl_t;


//-----------------------------------------
// Physical layer performance parameter
//-----------------------------------------
typedef struct
{
  SINT32 hybridCost[5];             // Cost functions for hybrids (0: none, 1-4 hybrid options)
  SINT32 usAvgGain;                 // upstream average gain in 20log10 (Q8)
  SINT32 dsAvgGain;                 // downstream average gain in 20log10 (Q8)
  UINT8  disableDspHybridSelect_f;  // Allows host to disable the automatic hybrid selection by the DSP
  UINT8  hostSelectHybridNum;       // DSP will use this hybrid number only if DSP Select is disabled (values: 1-4)
  UINT8  currentHybridNumUsed;      // DSP indicates to the host the current hybrid number in use
  UINT8  reserved1;                 // reserved for future use
} DEV_HOST_phyPerf_t;


/***********************************************************
 * The 3 structures below are used only for analog
 * diagnostic functions originally defined in diag.h
 * Moved here by Carlos A. Perez under J. Bergsagel request
 ***********************************************************/

/****************************************************************************/
/* Options for the Analog Diagnostic user input data structure              */
/* (MUST be word aligned)                                                   */
/****************************************************************************/
typedef enum
{
  ZERO_DIAG_OPT   = 0,       // dummy value for zero place-holder
  NOISE_ONLY      = 0x1,     // diagnostic in noise only mode (on=1, off=0), disregard diagMode 0-4
  EXTERNAL_CO     = 0x2,     // operates against external CO (external=1, internal=0)
  DIAG_AGC        = 0x4,     // agc selects gains control (agc=1, manual=0)
  CROSSTALK_TEQ   = 0x8,     // crosstalk selects teq (crosstalk=1, manual=0)
  LEAKY_TEQ       = 0x10,    // use leaky teq (on=1, off=0)
  AUX_AMPS        = 0x20,    // auxamps (on=1, off=0)
  BW_SELECT       = 0x40,    // change rxhpf/txlpf fc (modify=1, default=0)
  DIAG_HYB_SELECT = 0x80,    // change hybrid (modify=1, default=0)
  POWER_DOWN_CDC  = 0x100,   // power down codec (power down=1, no power down=0)
  ISDN_OP_MODE    = 0x200,   // operation mode (pots=0, isdn=1)
  BYPASS_RXAF2    = 0x400,   // Bypass except RXAF2 (on=1, off = 0)
  TX_TEST_CONT    = 0x800,   // Continuous tx test (on=1, off=0)
  TX_SCALE_MTT    = 0x1000   // Scale tx signal for Mtt test (on=1, off=0)
} DEV_HOST_diagAnlgOptionsVar_t;

/****************************************************************************/
/* Analog Diagnostic user input data structure (MUST be word align)         */
/****************************************************************************/

typedef struct
{
  DEV_HOST_diagAnlgOptionsVar_t   diagOption; // Other diagnostic optional settings

  UINT8                     diagMode;     // Performance diagnostic mode
  UINT8                     txMode;       // transmit mode
  UINT8                     rxMode;       // receive mode
  UINT8                     teqSp;        // Select teq starting pt
  UINT8                     txDf1;        // see dev_codec_filters.c and
  UINT8                     txDf2a;       // dev_codec.h for filter coefficients
  UINT8                     txDf2b;
  UINT8                     rxDf4;

  UINT16                    codingGain256Log2;  // 256*Log2(coding gain)
  UINT16                    noiseMargin256Log2; // 256*Log2(noise margin)

  UINT16                    rxPga1;       // PGA1
  UINT16                    rxPga2;       // PGA2
  UINT16                    rxPga3;       // PGA3
  UINT16                    anlgEq;       // AEQ settings (dB/MHz)

  SINT8                     pilotBin;     // Select  pilot subchannel
  SINT8                     txSwGain;     // manual set for bridge tap loop
  SINT8                     tdw1Len;      // TDW1 length - 0,2,4,8,16
  SINT8                     tdw2Len;      // TDW2 length - 0,2,4,8,16

  UINT8                     teqEcMode;    // TEQ/EC mode
  UINT8                     hybrid;
  UINT8                     txAttn;       // Codec Tx attenuation
  UINT8                     txGain;       // Codec Tx gain (Sangam only)

  SINT16                    txPda;        //Codec Tx Digital gain/attn
  UINT8                     txTone[MAX_NUM_UPBINS];  // Turning tones on/off
                                                     // Still govern by lastbin
  UINT16                   rsvd;          //for 32 bits alignment
}DEV_HOST_diagAnlgInputVar_t;

/****************************************************************************/
/* Analog diagnostic output data structure                                  */
/****************************************************************************/
typedef struct
{
  PSINT32  rxSnr_p[2];                  // Pointer to estimated snr
  PSINT32  rxSubChannelCapacity_p[2];   // Pointer to estimated subchan capacity
  PSINT32  rxSignalPower_p[2];          // Pointer to estimated signal power
  PSINT32  rxNoisePower_p[2];           // Pointer to estimated noise power
  PSINT32  rxAvg_p;                     // Pointer to average of rcvd signal
  SINT32   chanCapacity[2] ;            // Channel total capacity
  SINT32   dataRate[2];                 // Modem data rate (SNR)
  SINT32   avgNoiseFloor;               // Average noise floor
  SINT16   snrGap256Log2;               // 256*Log2(snr gap)
  SINT16   rxPga1;                      // PGA1
  SINT16   rxPga2;                      // PGA2
  SINT16   rxPga3;                      // PGA3
  SINT16   anlgEq;                      // AEQ settings (dB/MHz)
  SINT8    rcvMode;                     // CPE rcv mode for Matlab data processing
  SINT8    rsvd;
}DEV_HOST_diagAnlgOutputVar_t;


// Bit field structure that allows the host to mask off interrupt sources for possible DSP-to-Host interrupts.
// Each bit represents a possible source of interrupts in the DSP code that might cause a DSP-to-Host
//   interrupt to occur.
// This mask structure is not intended to show how interrupt sources in the DSP code correspond to the actual
//   DSP-to-Host interrupts.  There could be multiple ways to cause an interrupt in the DSP code, but they all
//   eventually tie into one of the three possible DSP-to-Host interrupts.
// The host should write a "1" to an individual bit when it wants to mask possible interrupts from that source.

// enum that represents individual bits in maskBitField1
typedef enum
{
  ZERO_MASK1      = 0,   // dummy value for zero place-holder
  DSP_MSG_BUF     = 0x1, // mask interrupts due to DSP-to-Host message mailbox updates
  STATE_BIT_FIELD = 0x2, // mask interrupts due to changes in the modem state bit fields
  DSP_HEARTBEAT   = 0x4  // mask interrupts for the DSP hearbeat
} DEV_HOST_intMask1_t; // this enum should only have 32 values in it (maximum).

// Add more "mask bit fields" at the end of this struct if you need more mask values
typedef struct
{
  DEV_HOST_intMask1_t maskBitField1;
} DEV_HOST_hostInterruptMask_t;  // this struct should only have 32 bits in it.

// Bit field structure that allows the host to determine the source(s) of DSP-to-Host interrupts in case
//   several of the interrupt sources get combined onto a single DSP-to-Host interrupt.
// DSP will set each bit to a "1"as an interrupt occurs.
// Host has the reponsibility to clear each bit to a "0" after it has determined the source(s) of interrupts.
// Each source bit field in this struct will use the same enum typedef that matches the corresponding mask
//   bit field in "DEV_HOST_hostInterruptMask_t"
typedef struct
{
  DEV_HOST_intMask1_t sourceBitField1;
} DEV_HOST_hostInterruptSource_t;


// --------------------------
// Begin bitswap definitions.
// --------------------------

// bitSwapSCnt contains the superframe to perform bit swap
// The entries must be ordered so that the first group only contains bit change commands
// The other entries may contain power adjustment instructions and must be
// written with something.  NOP (0) is an available instruction.
typedef struct
{
  PUINT8 fineGains_p;          // pointer to bng string, needed to check fine gains for powerswap
  UINT8  bitSwapNewIndices[6]; // Bin before bitSwapBin to process
  UINT8  bitSwapCmd[6];        // Bitswap command for bitSwapBin
  UINT8  bitSwapBin[6];        // bin to modify
  UINT8  bitSwapSCnt;          // Superframe count on which to perform bitswap
  UINT8  bitSwapEnabled;       // bitSwapEnabled
} DEV_HOST_dspBitSwapDef_t;

// -------------------------------------------
// Begin OLR DSP Rx definitions for ADSL2 and ADSL2 plus.
// -------------------------------------------
// OLR (On-line reconfig) contains the variables and arrays to perform US bitswap
// DRR and SRA variables can be added in this struct later
// All fields are DSP write/read with OVHD level2 Rx buffer.

typedef struct
{
  UINT8   bitSwapEnabled;               // Enable bitswap
  UINT8   Index;                        // index to parse msg
  UINT8   ctrl_field;                   // OLR msg control field
  UINT8   MsgType;                      // OLR msg type
  UINT8   NakReason;                    // Negtive Ack and reason
  UINT8   send_msg_action;              // Transmit/re-transmit msg flag
  UINT8   resp_msg_action;              // Negative resp msg value
  UINT8   ss_invert_phase;              // Sending response of inverted phase for sync symbol  
  UINT16  Nf;                           // number of subcarriers to be modified
//UR8_MERGE_START CQ10758 Tim  
  UINT16  Lp;                           // CQ10758: the new total number of bits from SRA   
  UINT8   Bp;                           // CQ10758: the new Bp from an SRA or DRR
//UR8_MERGE_END CQ10758 Tim   
  UINT16  msg_i;                        // Msg index to store bit swap msg
  UINT8   binnum[MAX_NUM_UPBINS];       // Subcarrier numbers OLR_BSWP_MAX_LEN for US
  UINT16  gainnbits[MAX_NUM_UPBINS];    // 12-bit gains and 4-bit bits for the subcarrier
//UR8_MERGE_START_END CQ10758 Tim       
  UINT8   rxState;
} DEV_HOST_olrDspRxDef_t;               // CQ10758: struct def for parsing US req bitswap msg

// -------------------------------------------
// Begin OLR DSP Tx definitions.
// -------------------------------------------
// OLR (On-line reconfig) contains the variables to perform DS bitswap
// DRR and SRA variables can be added in this struct later
// All fields are DSP write/read with OVHD level2 Tx buffer.

typedef struct
{
  UINT8    bitSwapEnabled;               // Enable Bitswap Flag
  UINT8    ctrl_field;                   // OLR msg control field
  UINT8    MsgType;                      // OLR msg type
  UINT8    waitforresp;                  // wait for response flag
  UINT8    resend_count;                 // retransmit counter to limit resend
  UINT8    prev_ss_phase;                // Previously detected sync symbol phase
  UINT16   prev_txmsg_len;               // Previous Tx msg length for resending
  SINT16   timeoutcnt;                   // wait for response timeout count
} DEV_HOST_olrDspTxDef_t;                // struct def for generating DS bitswap req


// ---------------------------
// Begin ATM BERT definitions.
// ---------------------------

// Structure used for ATM Idle Cells based bit error rate computation.
typedef struct
{
  UINT8  atmBertFlag;      // Feature enable/disable flag (Host write)
  UINT8  dummy1;
  UINT8  dummy[2];         // Dummy bytes for 32-bit alignment
  UINT32 bitCountLow;      // Low part of 64-bit BERT bit count (DSP write)
  UINT32 bitCountHigh;     // High part of 64-bit BERT bit count (DSP write)
  UINT32 bitErrorCountLow; // Low part of 64-bit BERT bit count (DSP write)
  UINT32 bitErrorCountHigh;// High part of 64-bit BERT bit count (DSP write)
} DEV_HOST_atmDsBert_t;


// ------------------------------------
// Misc. modem environment definitions.
// ------------------------------------


typedef struct
{
  SINT16 subStateIndex;        // Index that signifies datapump substate. (DSP write)
  UINT8  externalBert;         // Turn on/off external BERT interface. 0 = OFF; 1 = ON. (Host write)
  UINT8  usBertPattern;        // BERT pattern for US TX data. 0 = 2^15-1; 1 = 2^23-1. (Host write)
  UINT8  overrideDslLinkLed_f; // Overrides DSP operation of the DSL_LINK LED. (Host write)
                               //   0 = DSP is in control; 1 = Host is in control.
  UINT8  dslLinkLedState_f;    // DSL_LINK LED state when override flag has been set. (Host write)
                               //   DSL_LINK LED will be updated with this value once per frame.
                               //   LED is active-low: 0 = ON, 1 = OFF.
  UINT8  overrideDslActLed_f;  // Overrides DSP operation of the DSL_ACT LED. (Host write)
                               //   0 = DSP is in control; 1 = Host is in control.
  UINT8  dslActLedState_f;     // DSL_ACT LED state when override flag has been set. (Host write)
                               //   DSL_ACT LED will be updated with this value once per frame.
                               //   LED is active-low: 0 = ON, 1 = OFF.
  UINT8  dGaspLprIndicator_f;  // How LPR signal (GPIO[0]) is to be interpreted. (Host write)
                               //   0 = LPR is active-low; 1 = LPR is active-high.
  UINT8  overrideDspLprGasp_f; // Overrides DSP detection of LPR signal to send out DGASP. (Host write)
                               //   0 = DSP detects LPR; 1 = Host detects LPR and sends "HOST_DGASP" to DSP.
  UINT8  pcbEnabled;           // DS power cut back
  UINT8  maxAvgFineGainCtrl_f; // If maxAvgFineGainCtrl_f == 0, then the datapump controls the maximum average fine gain value.
                               // If maxAvgFineGainCtrl_f == 1, then the host controls the maximum average fine gain value.
  UINT32 reasonForDrop;        // This field will tell the host what might be the reason for a dropped connection.
  SINT16 maxAverageFineGain;   // When maxAvgFineGainCtrl_f == 1, the value in maxAverageFineGain is the maximum average fine gain level in 256log2 units.
// UR8_MERGE_START_END CQ10415_CABMODE HL
  UINT8  CO_cabinet_mode_enabled; // 0 = Default
                                           // 1 = (Mode is G.992.5 mode) && (CO is Infineon) && (Cabinet mode with DS start tone # >= 100)
  UINT8  enable_cnxt_ovhd_patch_f;
} DEV_HOST_modemEnvPublic_t;


// -----------------------------
// Generic structure definition.
// -----------------------------

typedef struct
{
  PSINT8  parameter1_p;
  PSINT16 parameter2_p;
  PSINT32 parameter3_p;
  PUINT8  parameter4_p;
  PUINT16 parameter5_p;
  PUINT32 parameter6_p;
} DEV_HOST_genericStructure_t;


// ------------------------------
// Begin G.hs buffer definitions.
// ------------------------------

typedef struct
{
  UINT8 txBuf[64]; // G.hs xmt buffer
} DEV_HOST_ghsDspTxBufDef_t;


typedef struct
{
  UINT8 rxBuf[80]; // G.hs rcv buffer
} DEV_HOST_ghsDspRxBufDef_t;

// -----------------------------------------
// Begin Constellation Display definitions.
// -----------------------------------------

typedef struct
{
  UINT32  consDispStartAddr;     // Host write
  UINT32  consDispCurrentAddr;   // Host write
  UINT32  consDispBufLen;        // Constellation Buffer Length
  UINT32  consDispBin;           // Host write, DS band only
} DEV_HOST_consBufDef_t;

typedef struct
{
  PSINT16 buffer1_p;          //DSP write
  PSINT16 buffer2_p;          //DSP write
} DEV_HOST_snrBuffer_t;

// ---------------------------------------------------------------------------------
// Typedef to be used for the clear EOC HDLC frame exchange between host and DSP,
// used in ovhd_eoc.c
// ---------------------------------------------------------------------------------

#define OVHD_clearEOC_hostBufSize 600       // overhead clear EOC HDLC host buffer size
#define OVHD_clearEOC_hostBuffers 4         // number of overhead clear EOC HDLC host buffers in Rx or Tx

typedef struct
{
  UINT32 len;                               // packet length
  UINT8  data[OVHD_clearEOC_hostBufSize];   // data field
} clearEocBufDesc_t;

typedef struct
{
  UINT32 clearEocEnabled;                   // 0 -- disabled; 1 -- enabled.
  clearEocBufDesc_t  *pTxBufDesc[OVHD_clearEOC_hostBuffers];   // Tx buffer desc pointer array
  clearEocBufDesc_t  *pRxBufDesc[OVHD_clearEOC_hostBuffers];   // Rx buffer desc pointer array
  UINT32 txRdIndex;                          // DSP read
  UINT32 txWrIndex;                          // Host write
  UINT32 rxRdIndex;                          // Host read
  UINT32 rxWrIndex;                          // DSP write
} DEV_HOST_clearEocParm_t;


typedef struct
{
  UINT32 phyFeature;                       // Feature bits
  UINT32 phyControl;                       // Control bits
} DEV_HOST_phyFeatureConfiguration_t;

typedef struct
{
  DEV_HOST_phyFeatureConfiguration_t   * phyFeatureList0_p; // Pointer to Feature List 0
  DEV_HOST_phyFeatureConfiguration_t   * phyFeatureList1_p; // Pointer to Feature List 1
  DEV_HOST_phyFeatureConfiguration_t   * phyInteropList0_p; // Pointer to Interop feature list 0
  DEV_HOST_phyFeatureConfiguration_t   * phyInteropList1_p; // Pointer to Interop feature list 1
} DEV_HOST_phyControlStructure_t;

//Management counters (moved from ovhd_bis_mgmt.h)

//WARNING!
//The order of this structure is critical as it mirrors the ADSL2
//management overhead message and we take advantage of this as part of an
//optimization in ovvhd_bis_mgmt.c.  Do not change the order or remove
//anything and if you need to make an addition, it must be an integer
//and it must be added to the end
typedef struct {
  //PMD and PMS-TC counters
  UINT32 fec_errors;              //fec anomalies
  UINT32 crc_errors;              //crc anomalies
  UINT32 fec_errored_seconds;     //FEC errored seconds counter
  UINT32 errored_seconds;         //errored seconds counter
  UINT32 severly_errored_seconds; //severly errored seconds counter
  UINT32 los_errored_seconds;     //los errored seconds counter
  UINT32 unavailable_seconds;     //unavailable errored seconds counter
  //TPS-TC counters
  UINT32 hec_errors;              //HEC anomalies counters
  UINT32 total_cells_thro_HEC;    //counter of total cells passed through HEC function
  UINT32 total_cells_to_ATM;      //counter of total cells passed to the upper ATM function
  UINT32 bit_errors_idle_cells;   //counter of total bit errors detected in ATM idle cells payload
}BIS_MGMT_CountersDef_t;

typedef struct
{
  BIS_MGMT_CountersDef_t *bis_Mgmt_Count_US_p;
  BIS_MGMT_CountersDef_t *bis_Mgmt_Count_DS_p;
//  UR8_MERGE_START CQ10979   Jack Zhang
  UINT32                totalInitErrs;    // Total number of Initialization Errors.
  UINT32                totalInitTOs;     // Total number of Initialization Timeout Errors.
  UINT32                showtimeInitErrs; // Number of Init. Errs since the most recent showtime.
  UINT32                showtimeInitTOs;  // Number of Init. Timeout Errs. since the most recent showtime.
  UINT32                lastshowInitErrs; // Number of Init. Errs since the 2nd most recent showtime.
  UINT32                lastshowInitTOs;  // Number of Init. Timeout Errs. since the 2nd most recent showtime.
//  UR8_MERGE_END   CQ10979*
} DEV_HOST_MGMTCount_t;

typedef struct //(CQ10242)
{
 UINT8          redirect;              // set by NSP
 UINT8          ack;                   // set by datapump to acknowledge redirect
 UINT16         buffsize;              // size of debug buffer in chars
 PUINT8         buffaddr;              // address of debug buffer (an array of char)
} DEV_HOST_Cli2lctl_t;

// UR8_MERGE_START CQ11057 KCCHEN
#define MAX_US_TONES 64
typedef struct {
  unsigned short TestParmCOHlogfMsg[MAX_US_TONES];
  unsigned char TestParmCOQLNfMsg[MAX_US_TONES];
  unsigned char TestParmCOSNRfMsg[MAX_US_TONES];
  UINT16 co_latn;
  UINT16 co_satn;
  signed short usMargin;                  // DSP Write, measured US margin
  signed short dummy;
  UINT32 co_attndr;
  signed short co_near_actatp;
  signed short co_far_actatp;
}DEV_HOST_BIS_PMD_TEST_PARAMETERS_FROM_CO_Def_t;
// UR8_MERGE_END CQ11057 KCCHEN

// --------------------------------------------------------------------------------------
// Typedef to be used for the DEV_HOST_dspOamSharedInterface_s struct of pointers
//  (this is used in dev_host_interface.c).
// NOTE: This struct of pointers is NEVER to be referenced anywhere else in the DSP code.
// IMPORTANT: Only pointers to other structs go into this struct !!
// --------------------------------------------------------------------------------------
typedef struct
{
  DEV_HOST_hostIntfcVersionDef_t   *hostIntfcVersion_p;
  DEV_HOST_dspVersionDef_t         *datapumpVersion_p;
  DEV_HOST_modemStateBitField_t    *modemStateBitField_p;
  DEV_HOST_dspWrNegoParaDef_t      *dspWriteNegoParams_p;
  DEV_HOST_oamWrNegoParaDef_t      *oamWriteNegoParams_p;
  DEV_HOST_raMsgsDef_t             *raMsgs_p;
  DEV_HOST_dspWrSuperFrameCntDef_t *dspWriteSuperFrameCnt_p;
  DEV_HOST_msg_t                   *atucMsg_p;
  DEV_HOST_msg_t                   *aturMsg_p;
  DEV_HOST_dspWrSharedTables_t     *dspWrSharedTables_p;
  DEV_HOST_olayDpDef_t             *olayDpParms_p;
  DEV_HOST_eocVarDef_t             *eocVar_p;
  DEV_HOST_clearEocVarDef_t        *clearEocVar_p;
  DEV_HOST_modemStatsDef_t         *modemStats_p;
  DEV_HOST_atmStats_t              *atmStats_p;
  DEV_HOST_mailboxControl_t        *dspHostMailboxControl_p;
  DEV_HOST_phyPerf_t               *phyPerf_p;
  DEV_HOST_diagAnlgInputVar_t      *analogInputVar_p;
  DEV_HOST_diagAnlgOutputVar_t     *analogOutputVar_p;
  DEV_HOST_hostInterruptMask_t     *hostInterruptMask_p;
  DEV_HOST_profileBase_t           *profileList_p;
  DEV_HOST_hostInterruptSource_t   *hostInterruptSource_p;
  DEV_HOST_dspBitSwapDef_t         *dspBitSwapDstrm_p;
  DEV_HOST_dspBitSwapDef_t         *dspBitSwapUstrm_p;
  DEV_HOST_atmDsBert_t             *atmDsBert_p;
  DEV_HOST_modemEnvPublic_t        *modemEnvPublic_p;
  DEV_HOST_genericStructure_t      *genericStructure1_p;
  DEV_HOST_genericStructure_t      *genericStructure2_p;
  DEV_HOST_ghsDspTxBufDef_t        *ghsDspTxBuf_p;
  DEV_HOST_ghsDspRxBufDef_t        *ghsDspRxBuf_p;
  DEV_HOST_consBufDef_t            *consDispVar_p;
  DEV_HOST_snrBuffer_t             *snrBuffer_p;
  DEV_HOST_olrDspRxDef_t           *olrDspRx_p;
  DEV_HOST_olrDspTxDef_t           *olrDspTx_p;
  DEV_HOST_clearEocParm_t          *pClrEOC_p;
  DEV_HOST_phyControlStructure_t   *phyControl_p;
  DEV_HOST_MGMTCount_t             *mgmt_Count_p;
  DEV_HOST_Cli2lctl_t              *cli_p;  //(CQ10242)
} DEV_HOST_dspOamSharedInterface_t;


// ---------------------------------------------------------------------------------
// Typedef to be used for the pointer to the DEV_HOST_dspOamSharedInterface_s struct
//  of pointers (this is used in dev_host_interface.c).
// ---------------------------------------------------------------------------------
typedef DEV_HOST_dspOamSharedInterface_t *DEV_HOST_dspOamSharedInterfacePtr_t;

#endif
