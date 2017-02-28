#ifndef __DSL_HAL_API_H__
#define __DSL_HAL_API_H__ 1
/*******************************************************************************
* FILE PURPOSE:     DSL HAL to Application Interface for Sangam
*
********************************************************************************
* FILE NAME:        dsl_hal_api.h
*
* DESCRIPTION:
*       DSL-Application Interface Structures
*
*
* By: Ramakrishnan Parasuraman
*
* (C) Copyright 2003, Texas Instruments, Inc.
*    History
*    Data        Version    By               Notes
*    06Feb03     0.00       RamP            Initial Version Written
*    07Apr03     0.01       RamP            Commented out typedefs
*    09Apr03     0.02       RamP            Added deviceContext and extended
*                                           dspVer to include bugfixes
*    14Apr03     0.03       RamP            Added stateTransition to structure
*    16Apr03     0.04       RamP            Removed typedefs; changed dspVer
*    22Apr03     0.05       RamP            Moved acknowledgeInterrupt from
*               (alpha)                     support module into this
*    24Apr03     0.06       RamP            Moved the RSTATE enum from register
*
*    28Apr03     0.07       RamP            Added additional parameter to the
*               (alpha +)                   handleInterrupt function for intrSrc
*    14May03     0.08       RamP            Added hybrid switching APIs
*               (alpha ++)                  Added statistics fields in AppData
*                                           Added netService identifier
*    20May03     0.09       RamP            Added Inner/Outer pair API support.
*                                           Added dying gasp message.
*    29May03     0.10       RamP            Added coProfile structure
*    04Jun03     0.11       RamP            Added margin per tone statistics,
*                                           Added timing counters, added train
*                                           failure functions,added config flags
*    06Jun03     0.12       RamP            Added LED, STM Bert, dGasp LPR
*                                           config API functions
*    09Jun03     0.13       RamP            Added ATM Bert function, CO stats
*                                           Moved stateTransition to ITIDSLHW
*               (Beta)                      Moved configFlag to ITIDSLHW,
*                                           Cleaned up fifoInfo structure
*                                           Added US/DS R/S FEC parameters
*    21Aug03     0.14       RamP            Added g.hs message buffer, us/ds
*                                           bits n gains table, negoMsgs struct
*               (act)                       bitswap stucts/indices, trainstate,
*                                           Added functions for advanced config
*                                           Added gross gain and line length
*    29Sep03     0.15       RamP            Added tokens for advanced config
*                                           module api functions
*    12Oct03     0.16       RamP            Added adsl2Msgs structure with worst
*                                           case size for variable length msgs
*                                           Added API function prototypes
*    21Oct03     0.17       RamP            Added typedef for current modem
*                                           user settings
*    28Oct03     0.18       RamP            Added function to config blackout
*                                           bitmap in the RMSGPCB message
*    20Nov03     0.19       RamP            Added functions for generic and
*                                           host interface read - write
*    24Nov03     0.20       RamP            Added enum for detailed state track
*                                           Added element for state bit fields
*                                           Addded rState for encoded main state
*                                           Added blackout valid flag
*    26Dec03     0.21       RamP            Added defines for oamFeature masks
*    30Dec03     0.22       RamP            Increased sizes for cMsgPcb,RMsgPcb
*                                           to incorporate DELT mode messages
*    30Dec03     0.23       RamP            Added generic mailbox command fxn
*    02Mar04     0.24       RamP            Removed NUM_PAGES define and referenced
*                                           host interface define instead
*    03Mar04     0.25       RamP            Added downstream noise to statistics
*    20Apr04     0.26       RamP            Expanded CO Profile structure for scratch
*                                           memory & added a counter for numProfiles
*    29Apr04     0.27       RamP            Increased size of arrays for ADSL2+
*    14May04     0.28       RamP            Increased size of B&G tables for ADSL2+
*                                           added advanced statistics API function
*    26Aug04     0.29       Brian           Made changes to support switching DSP
*                                           frequency to 250Mhz
*    06Oct04     0.30       RamP            Added API functions to configure DSL PHY
*                                           features and to set DSP/MIPS as controller
*                                           for these features
*    29Dec04     0.31       DW              CQ 9356: Added definitions for Interop feature lists.
*    09Feb05     0.32       YH              CQ 9459: Added definitions for Interop feature lists.
*    10Mar05     0.33       CPH             Added IPE-Interop Patch Engine.
*    02Mar05     0.33       YH              Added additional definitions for Interop feature lists.
*    15Mar05     0.31       T.Leyrer        Added support for maxBits configuration
*                                           in each direction. Keep backward compatiility
*                                           with orig function.
*    31Mar05     ---        R. Pappu        No CQ.  Added definition for new interop
*                                           feature bit for QWEST_US_1M_RATE_FIX.
*    10May05                Arvind V        CQ 9605:Added a new variable useBitField
*                                           in the tagTIOIDINFO structure,
*                                           for allowing the user to select from
*                                           the dsl modes using bitfields.
*    16May05     ---        Madhu Hegde     CQ 9620. Added definition for new interop feature bit
*                                           to enable higher US rates for ADI by reducing quant noise.
*    23May05                H. Liu          CQ9500: Added OAMFEATURE_DISABLE_SAFEMODE_FOR_G992_3_5.
*    6June05                Peter Hou       Added AnnexB(ISDN) & AnnexM support
*    17June05               Peter Hou       Remove dslhal_advcfg_getSnrPerBin()
*    23June05               Peter Hou       - Change cMsgs1 length from 6 to 10 bytes to sync with
*                                           dev_host_interface.h
*                                           - Increase coProfiles[] from 4 to 5 to accomodate coProfile_ptr
*                                           & 4 scratchmem_ptrs.
*    30June05               Peter Hou       Change 2nd parameter of dslhal_advcfg_configDsTones() from unsigned int
*                                           to unsigned char.
*    13July05               Peter Hou       Added IOP-Feature Control bit definition
*                                           OAMFEATURE_ENABLE_BELGACOM_ANXB_FAST_TRAINING
*    10May05                Arvind V        CQ 9605:Added a new variable useBitField
*                                           in the tagTIOIDINFO structure,
*                                           for allowing the user to select from
*                                           the dsl modes using bitfields.
*    16May05     ---        Madhu Hegde     CQ 9620. Added definition for new interop feature bit
*                                           to enable higher US rates for ADI by reducing quant noise.
*    23May05                H. Liu          CQ9500: Added OAMFEATURE_DISABLE_SAFEMODE_FOR_G992_3_5.
*    18Jul05     ---        C. Urrutia      CQ9787: Added new interop bit for Westell RFI issue
*                                           Added new interop bit for OHIO hybrid 4
*    24July05               Peter Hou       -Change bCMsgs1 size to 12 based on MAX_CMSGPCB2_LENGTH from dev_host_interface.h
*                                           -Change OAMFEATURE_ENABLE_BELGACOM_ANXB_FAST_TRAINING to Bit14
*    29July05               Peter Hou       Add max_ds_tones & max_us_tones
*    1Sept05                Peter Hou       CQ9613: Change OAMFEATURE_ENABLE_OHIO_HYBRID4_REWIRE (removed)
*                                           to
*    5Sept05                Peter Hou       CQ9776: Added dslhal_api_getHLOGpsds(), dslhal_api_getQLNpsds(),
*                                           and dslhal_api_getSNRpsds().
*    21Sept05               Peter Hou       Change coProfiles[5] to coProfiles[6].
*    7 Oct05                AV/CPH          Changed dprintf to dgprintf and compile time
*                                           selection of dslhal_api_getQLNpsds() dslhal_api_getSNRpsds()
*                                           and for code size reduction.
*    31Oct05                Ram             CQ10012: Added new Interop bit for OAMFEATURE_ENABLE_REPORT_AVG_MARGIN
*                                                    recycled & reused the Belgacom AnnexB Fast training API bit
*    08Nov05                Peter Hou       Added LPR in TIOIDINFO.
*    30Nov05                KM/JZ           CQ10226: Added LOF errors and errored_seconds in TIOIDINFO.
*    18Jan06                CPH             CQ9885: Added OAMFEATURE_DISABLE_CNXT_OVHD_PATCH
*    7 Jan06                Hanyu Liu       CQ10173: Added API IOP bit17, OAMFEATURE_ENABLE_TELEFONICA_FIXED_MARGIN
*    27Jan06                Raghu M         CQ10045: Added API bit OAMFEATURE_ENABLE_MAXIMIZE_INP_SUPPORT
*                                           and OAMFEATURE_ENABLE_MINIMIZE_INTERLEAVER_DELAY
*    03Feb06                Madhu H         CQ10198: Added API IOP bit19 to extend AnnexB US to bin 28
*    03Feb06                Peter Hou       CQ10280: Add extended PHY feature bit support.
*    08Feb06                GPet            CQ10242: Added CLI redirect
*    10Feb06                Peter Hou       CQ10280: Change ENABLE_EXTENDED_FRAMING_SUPPORT & ENABLE_EXTENDED_INP_SUPPORT
*                                                    to DISABLE_.
*    UR8_MERGE_START Report_SES Manjula K
*    14Mar06                ManjulaK        CQ10369: Added severelyerrsecs in TIOIDINFO
*    UR8_MERGE_END  Report_SES
*    UR8_MERGE_START GERALD_CQ_REDIRECT_BUGFIX2_20060328 Gerald
*    28Mar06                GPet            CQ10411: fixed dhalapp crash with cliRedirect and reboot command
*    UR8_MERGE_END  GERALD_CQ_REDIRECT_BUGFIX2_20060328
// UR8_MERGE_START  API-Bits PeterHou
* OAMFEATURE_ENABLE_DETECT_MORE_COMB1TONES
*    3Apr06             PeterHou/KapilG CQ10375: Added OAMFEATURE_PHY_ENABLE_INP_DSCRC_IMPROVEMENT API bit
*                       PeterHou/HanyuL CQ10385: Added OAMFEATURE_ENABLE_DETECT_MORE_COMB1TONES API bit
*                       PeterHou/TimB   CQ10351: Added OAMFEATURE_PHY_ENABLE_SRA_SUPPORT API bit
// UR8_MERGE_END API-Bits
// UR8_MERGE_START  API-Bits PeterHou
*    6Apr06             PeterHou/TimB   CQ10471: Added OAMFEATURE_ENABLE_CNXT_US_FLAVOR_B API Bit
// UR8_MERGE_END API-Bits
//UR8_MERGE_START CQ10499   Jack Zhang
 * 5/19/06  JZ   CQ10499: ATM Driver Cleanup/Improvement Umbrella
//UR8_MERGE_END CQ10499   Jack Zhang
// UR8_MERGE_START API-Bit ManjulaK
*    24May06            ManjulaK/Nima   CQ10202: Added OAMFEATURE_PHY_TI_INTERNAL1 API bit.
// UR8_MERGE_END API-Bit
// UR8_MERGE_START CQ10600 ManjulaK
*    26May06            ManjulaK/HT     CQ10600: Added ENABLE_ETISALAT_US_CRC_N_LINEDROP_FIX API Bit
// UR8_MERGE_END CQ10600
// UR8_MERGE_START_END CQ10617 AdeelJ
*    05June06                AdeelJ     CQ10617: Added OAMFEATURE_PHY_ENABLE_INFN_MINMAR_PARSE_N_CHK API token
//    UR8_MERGE_START CQ10442 Manjula K
*    07Jun06                 ManjulaK   CQ10442: Added SRA in TIOIDINFO
//    UR8_MERGE_END   CQ10442
// UR8_MERGE_START API-Bit ManjulaK
*    07Jun06            ManjulaK        CQ10202: Changed OAMFEATURE_PHY_TI_INTERNAL1 API assignment to 30 from 31.
// UR8_MERGE_END API-Bit
// UR8_MERGE_START CQ10660 ManjulaK
*    06Jun06                Ram             CQ10495: Added ENABLE_T1413_ACTIVATION_FIRST API Bit
// UR8_MERGE_END CQ10660

//   6June06            MadhuH          CQ10516  Added OAMFEATURE_PHY_COOK_INP_TRAINING API Bits UR8_MERGE_START_END_CQ10516 MH
//   15June06           NimaF           CQ10665  Added OAMFEATURE_PHY_EXTENDED_PILOT_SEARCH API bits UR8_MERGE_START_END CQ10665 NF
// UR8_MERGE_START APIBits ManjulaK
*    26July06           ManjulaK/HT CQ10781: Added OAMFEATURE_PHY_ENABLE_LOWER_GHS_TONE_POWER API bit
*                                   CQ10784: Added OAMFEATURE_PHY_ENABLE_REDUCE_GHS_FALL_BACK_TIME API bit
*    08Aug06            Ram         CQ10774,84: Added OAMFEATURE_ENABLE_TELIA_SONERA_FIX and removed
*                                   OAMFEATURE_PHY_ENABLE_REDUCE_GHS_FALL_BACK_TIME bit to consolidate Telia fixes
*    11Aug06            Mark Bryan  CQ10566: Added OAMFEATURE_PHY_ENABLE_GHSCABMODE API bit
// UR8_MERGE_END APIBits
 * UR8_MERGE_START CQ10880   Jack Zhang
 *   30Aug06            JZ          CQ10880: Add DSL HAL API for sending mailbox message for L3
 * UR8_MERGE_END   CQ10880
// UR8_MERGE_START APIBits ManjulaK
*   20Oct06             Manjula/HT  CQ11023/CQ11032: Added OAMFEATURE_ENABLE_PCCW_CNXT_FIX API bit
                                    CQ11031 : Added OAMFEATURE_ENABLE_VERSION_NO_16BYTES API bit
                                    CQ11021/CQ11022: Added OAMFEATURE_ENABLE_NORWAY_COMLAB_FIX API bit
                        Manjula/YW  CQ10913: Added OAMFEATURE_PHY_DISABLE_ANNEXM_SUPPORT API bit
                        Manjula/KC  CQ11080: Added OAMFEATURE_ENABLE_CTLM_LOW_USRATE_FIX API bit
// UR8_MERGE_END APIBits
* UR8_MERGE_START_END CQ10819 Ram
* 10/23/06              Ram         CQ10819: Added OAMFEATURE_ENABLE_OPTUS_BTLOOP_FIX
// UR8_MERGE_START_END CQ10960 HZ
*    24Oct06            HZ          CQ10960: Added OAMFEATURE_PHY_ENABLE_TIME_ERROR_SCALE_CNXT
// UR8_MERGE_START CQ11075_API25 YW
* 10/23/06              Yan Wang    CQ11075: Added OAMFEATURE_ENABLE_LINE_STABILITY_2DBPAD_MARGIN bit
*                                   and OAMFEATURE_ENABLE_ECHO_BAND_DS_SNR_CUTBACK bit.
// UR8_MERGE_END CQ11075_API25 YW
*  UR8_MERGE_START CQ10978   Jack Zhang
*  10/4/06  JZ     CQ10978: Request for the DSL Power Management Status Report
*  UR8_MERGE_END   CQ10978*
*  UR8_MERGE_START CQ10979   Jack Zhang
*  10/4/06  JZ     CQ10979: Request for TR-069 Support for RP7.1
*  UR8_MERGE_END   CQ10979*
*  UR8_MERGE_START CQ11054   Jack Zhang
*  10/11/06  JZ     CQ11054: Data Precision and Range Changes for TR-069 Conformance
*  UR8_MERGE_END   CQ11054*
*  UR8_MERGE_START_END CQ11004
*    27OCT06    Nima   CQ11004: Added OAMFEATURE_PHY_TI_INTERNAL2 API bit.
// UR8_MERGE_START CQ11057 KCCHEN
// 10/12/06 Kuan-Chen Chen   CQ11057: Request US PMD test parameters from CO side
// UR8_MERGE_END CQ11057 KCCHEN
*  UR8_MERGE_START   CQ11228 HZ
*  12/08/06     Hao Zhou CQ11228: Modify the DS Margin report to 0.1dB precision.
*  UR8_MERGE_END   CQ11228 HZ
*  UR8_MERGE_START CQ11054   Jack Zhang
*  10/11/06  JZ     CQ11054: Data Precision and Range Changes for TR-069 Conformance
*  UR8_MERGE_END   CQ11054*
*  UR8_MERGE_START_END CQ11277 Ram
* 12/22/06              Ram         CQ11277: Added OAMFEATURE_ENABLE_ADSL2_2PLUS_HLIN
// UR8_MERGE_START_END CQ11247_TR69_DS_LATN_SATN  YW
// 12/18/06   Yan Wang      CQ11247: TR069 range and precision changes for LATNds, SATNds
*  UR8_MERGE_START CQ11230
*  12/27/06   Hao-Ting Lin
*  Add OAMFEATURE_FEATURE1 BIT0 for TR067 DSPCB test.
*  UR8_MERGE_END CQ11230
*  UR8_MERGE_START_END CQ11281 Ram
* 12/22/06              Ram         CQ11281: Added OAMFEATURE_ENABLE_GHS_SHORT_RESET
//UR8_MERGE_START_END CQ11306 AdeelJ
// 01/10/07  AdeelJ  CQ11306: Added OAMFEATURE_FEATURE1 BIT1 for Annex B improvement at OTE Greece
//UR8_MERGE_START CQ11425 KCCHEN
// 02/05/07  KCCHEN  CQ11425: Added OAMFEATURE_FEATURE1 BIT4 for Aztech Connexant's wrong read of near and far end
//                                  aggregate power of DS PMD.
//UR8_MERGE_END CQ11425 KCCHEN
// UR8_MERGE_START_END CQ11467 ANNEX_I_GHS  HL
// 02/07/07  Hanyu   CQ11467: Added OAMFEATURE_PHY_ENABLE_ANNEXI_SUPPORT Bit21.
// UR8_MERGE_START_END CQ11307 Ram
// 03/02/07 Ram      CQ11307: Added OAMFEATURE_FEATURE1_ENABLE_BRITISH_TELECOM_FIX API bit.
*  UR8_MERGE_START_END CQ11260 WT100_CTLM_B900 HL
* 01/15/07    Hanyu Liu CQ11260: Defined OAMFEATURE_FEATURE1_ENABLE_CTLM_B900_O2_FIX
*                                and OAMFEATURE_FEATURE1_ENABLE_FORCED_US_PCB_14DB_NULLLOOP.
* UR8_MERGE_START_END CQ11544 Tim
* 04/16/07    Tim B  CQ11544: Added variable for uncancelled echo
//UR8_MERGE_START_END CQ11709 Tim
* 05/01/07    Tim    CQ11709: Added support for AT&T priority 1 statistics
*  UR8_MERGE_START CQ11813 Hao-Ting
* 06/11/07 Hao-Ting CQ11813: CLI redirect support in linux
*                            Mark unused function by LINUX_CLI_SUPPORT flag
*  UR8_MERGE_END CQ11813
*  UR8_MERGE_START_END monitored_tones MB
*  05/25/2007 Mark Bryan   CQ11745: Added OAMFEATURE_PHY_DISABLE_MONITORED_TONES.
* UR8_MERGE_START CQ11803 KCCHEN/Ram
* 06/12/07  Ram    CQ11803: Added new API bit for US ATTNDR Update Fix
*                           Added new API function to update US ATTNDR
* UR8_MERGE_END CQ11803 KCCHEN/Ram
* UR8_MERGE_START CQ11724 Ram
*  06/11/07 Ram     CQ11724: New PHY_0 API bit 23 for CPE initiated DELT.
*                              ENABLE_CPE_INITIATED_DELT,
*                   CQ11781: Updated missing API bit declaration for Two Tone Timing
* UR8_MERGE_END CQ11724 Ram
* UR8_MERGE_START_END CQ11818 Ram
*  06/11/07 Ram     CQ11818: Added a new API bit for TDC fixes.
*  UR8_MERGE_START_START_END CQ11661 HL
*  06/19/07 Hanyu    CQ11661: Added OAMFEATURE_FEATURE1_ENABLE_ADSL2PLUS_FASTPATH_STABILITY.
*  07/02/07 Hao      CQ11781: Modify API bit declaration for Two Tone Timing and put reservation
*                             for BIT26, BIT27, BIT28.
*  09/11/07 Ram      CQ11931: Added new API bit for forced enable Monitored tones IOP1 bit9
*  09/12/07 EYin     CQ11929: Define NFEC/INP/Lp/Rp and usSNR in TIOIDINFO structure
*  UR8_MERGE_START_END CQ11922 Tim
*  09/04/07 Tim      CQ11922: Added on scratchram entry and a new API bit for PTM mode
*  09/19/07 CPH      CQ11466: Added EFM support
*******************************************************************************/

#ifdef INTERNAL_BUILD
#include <dsl_hal_api_pvt.h>
#endif


// Definitions for PARAM ID0
#define OAMFEATURE_AUTORETRAIN_MASK       0x00000001
#define OAMFEATURE_TC_SYNC_DETECT_MASK    0x00000002
#define OAMFEATURE_EOCAOC_INTERRUPT_MASK  0x00000004
#define OAMFEATURE_CONS_DISP_DISABLE_MASK 0x00000008
#define OAMFEATURE_GHSMSG_INTERRUPT_MASK  0x00000010

//#define dslhal_api_setMaxBitsPerCarrier dslhal_api_setMaxBitsPerCarrierUpstream


// DW CQ 9356: Definitions for PARAM ID2 (Interop features)
#define DSL_PHY_FEATURELIST0_PARAMID                        0x00000000  //CQ10280
#define DSL_PHY_FEATURELIST1_PARAMID                        0x00000001  //CQ10280
#define INTEROP_FEATURELIST0_PARAMID                        0x00000002
#define INTEROP_FEATURELIST1_PARAMID                        0x00000003

//UR8_MERGE_START_END monitored_tones MB
#define OAMFEATURE_PHY_DISABLE_MONITORED_TONES              0x00000040  // Bit6, CQ11745
#define OAMFEATURE_PHY_DISABLE_EXTENDED_FRAMING_SUPPORT     0x00000080  // Bit7, CQ10280
#define OAMFEATURE_PHY_ENABLE_EXTENDED_INP_SUPPORT          0x00000100  // Bit8, CQ10280
// UR8_MERGE_START API-Bits PeterHou
#define OAMFEATURE_PHY_ENABLE_SRA_SUPPORT                   0x00000200  // Bit9, CQ10351
#define OAMFEATURE_PHY_ENABLE_INP_DSCRC_IMPROVEMENT         0x00000400  // Bit10, CQ10375
#define OAMFEATURE_PHY_ENABLE_T1413_ACTIVATION_FIRST        0x00000800  // Bit11, CQ10495
// UR8_MERGE_END API-Bits
// UR8_MERGE_START_END CQ10617 AdeelJ
#define OAMFEATURE_PHY_ENABLE_INFN_MINMAR_PARSE_N_CHK       0x00001000  // Bit12, CQ10617

//UR8_MERGE_START_END_CQ10516 MH
#define OAMFEATURE_PHY_COOK_INP_TRAINING                    0x00002000  // Bit13, CQ10516 MH

//UR8_MERGE_START_END CQ10665 Nima Ferdosi
#define OAMFEATURE_PHY_EXTENDED_PILOT_SEARCH                0x00004000  // Bit14, CQ10665 NF

// UR8_MERGE_START API-Bits ManjulaK
#define OAMFEATURE_PHY_ENABLE_LOWER_GHS_TONE_POWER          0x00008000  // Bit15, CQ10781
#define OAMFEATURE_PHY_ENABLE_GHSCABMODE                    0x00010000  // Bit16, CQ10566 MB
// UR8_MERGE_END API-Bits
// UR8_MERGE_START_END APIBits Manjulak
#define OAMFEATURE_PHY_DISABLE_ANNEXM_SUPPORT               0x00020000  //Bit17, CQ10913 YW
// UR8_MERGE_START_END CQ10960 HZ
#define OAMFEATURE_PHY_ENABLE_TIME_ERROR_SCALE_CNXT         0x00040000  // Bit18, CQ10960 HZ
// UR8_MERGE_START_END CQ11277 Ram
#define OAMFEATURE_ENABLE_ADSL2_2PLUS_HLIN                  0x00080000  // Bit19, CQ11277 Ram
// UR8_MERGE_START_END CQ11281 Ram
#define OAMFEATURE_ENABLE_GHS_SHORT_RESET                  0x00100000  // Bit20, CQ11281 Ram
// UR8_MERGE_START_END CQ11467 ANNEX_I_GHS  HL
#define OAMFEATURE_PHY_ENABLE_ANNEXI_SUPPORT               0x00200000  // Bit21, CQ11467 HL
// UR8_MERGE_START CQ11544 Tim
#define OAMFEATURE_PHY_UNCANCELLED_ECHO_METRIC             0x00400000  // Bit22, CQ11544 Tim
// UR8_MERGE_START_END CQ11724 Ram
#define OAMFEATURE_PHY_CPE_INITIATED_DELT                  0x00800000  // Bit23, CQ11724 Ram
// UR8_MERGE_START CQ11781 HAO
#define  OAMFEATURE_PHY_ENABLE_TWOTONE_ADSL1_TIMING         0x01000000  // Bit24, CQ11781 HAO
#define  OAMFEATURE_PHY_DISABLE_TWOTONE_A2PLUS_TIMING       0x02000000  // Bit25, CQ11781 HAO
// UR8_MERGE_START_END CQ11922 Tim
#define  OAMFEATURE_PHY_ENABLE_PTM_PREFERED                 0x04000000  // Bit26, CQ11922
// UR8_MERGE_START_END CQ11922 Tim
#define  OAMFEATURE_PHY_ENABLE_FORCE_PTM                    0x08000000  // Bit27, CQ11922
#define  OAMFEATURE_PHY_RESERVED_API3                   0x10000000  // Bit28, CQ11781 HAO
// UR8_MERGE_END CQ11781 HAO
// UR8_MERGE_START UR8_MERGE_START_END CQ11004 Nima
#define OAMFEATURE_PHY_TI_INTERNAL2                         0x20000000 // Bit29, CQ11004
// UR8_MERGE_START API-Bit ManjulaK
#define OAMFEATURE_PHY_TI_INTERNAL1                    0x40000000  // Bit30, CQ10202
// UR8_MERGE_END API-Bit


#define OAMFEATURE_DISABLE_GSPV_T1413_RETRAIN               0x00000001
#define OAMFEATURE_ENABLE_SHORT_LOOP_US_ERR_REDUCTION       0x00000002
#define OAMFEATURE_ENABLE_T1413_LONG_LOOP_ACTIVATION        0x00000004
#define OAMFEATURE_ENABLE_QWEST_ALCATEL_US_LOW_RATE_FIX     0x00000008
#define OAMFEATURE_ENABLE_ASB_ETSIB_HIGH_BER_FIX            0x00000010
#define OAMFEATURE_ENABLE_BELL_CANADA_DOUBLE_BRIDGE_TAP_FIX 0x00000020
#define OAMFEATURE_ENABLE_BELGACOM_DS_FIX                   0x00000040
#define OAMFEATURE_ENABLE_CINCINNATI_BELL_FIX               0x00000080
#define OAMFEATURE_ENABLE_FRANCE_TELECOM_FIX                0x00000100
#define OAMFEATURE_ENABLE_QWEST_US_1M_RATE_FIX              0x00000200
#define OAMFEATURE_DISABLE_SAFEMODE_FOR_G992_3_5            0x00000400
#define OAMFEATURE_ENABLE_ADI_UPSTREAM_RATES_FIX            0x00000800    //CQ 9620 MH 05/16/05
#define OAMFEATURE_ENABLE_NOKIA_D50_GDMT_RETRAIN_FIX        0x00001000    //CQ9613
#define OAMFEATURE_ENABLE_WESTELL_RFI_OPTION                0x00002000    // CQ9787 - Westell RFI option
#define OAMFEATURE_ENABLE_REPORT_AVG_MARGIN                 0x00004000    // CQ10012 - Avg Margin Reporting; removed unused AnnexB bit
#define OAMFEATURE_DISABLE_CNXT_OVHD_PATCH                  0x00008000    // CQ9885
#define OAMFEATURE_ENABLE_MAXIMIZE_INP_SUPPORT              0x00010000    // CQ10045 - Maximize INP
#define OAMFEATURE_ENABLE_TELEFONICA_FIXED_MARGIN           0x00020000    // CQ10173 - Force to drop line if  margin < targetmargin
#define OAMFEATURE_ENABLE_MINIMIZE_INTERLEAVER_DELAY        0x00040000    // CQ10045 - Minimize interleaver delay
#define OAMFEATURE_ENABLE_ANNEXB_US_STARTBIN                0x00080000    // CQ10198 - Extend Annex B US to bin 28
// UR8_MERGE_START API-Bits PeterHou
#define OAMFEATURE_ENABLE_DETECT_MORE_COMB1TONES            0x00100000    // CQ10385 - Detect more Comb1 tones to combat
                                                                          // Comb1-like crosstalk noise in ADSL2/2+ mode
#define OAMFEATURE_ENABLE_CNXT_US_FLAVOR_B                  0x00200000    // CQ10471 - Use CNXT Upstream flavor B in G.DMT & T1.413 mode
// UR8_MERGE_END API-Bits
// UR8_MERGE_START CQ10600 ManjulaK
#define OAMFEATURE_ENABLE_ETISALAT_US_CRC_N_LINEDROP_FIX    0x00400000    //CQ10600- add quantization noise to upstream signal from G2_MedleyA to G2_MedleyH state
// UR8_MERGE_END CQ10600
// UR8_MERGE_START_END CQ10774,84 Ram
#define OAMFEATURE_ENABLE_TELIA_SONERA_FIX                  0x00800000    //CQ10774,84 - for Telia specific fixes
// UR8_MERGE_START CQ11075_API25 YW
#define OAMFEATURE_ENABLE_ECHO_BAND_DS_SNR_CUTBACK          0x01000000    //CQ10899 SNR cutback for Showtime stability
#define OAMFEATURE_ENABLE_LINE_STABILITY_2DBPAD_MARGIN      0x02000000    //Enables 2dB additional noise margin to improve ong term line stability.
// UR8_MERGE_END CQ11075_API25 YW
// UR8_MERGE_START APIBits Manjulak
#define OAMFEATURE_ENABLE_PCCW_CNXT_FIX                     0x04000000    //CQ11023/CQ11032 Enables PCCW cnxt fixes.
#define OAMFEATURE_ENABLE_VERSION_NO_16BYTES                0x08000000    //CQ11031 Uses 16 bytes EOC vendor version number in G.DMT mode
#define OAMFEATURE_ENABLE_NORWAY_COMLAB_FIX                 0x10000000    //CQ11021/CQ11022 For COMLAB specific fixes
// UR8_MERGE_END APIBits
// UR8_MERGE_START_END CQ10819 Ram
#define OAMFEATURE_ENABLE_OPTUS_BTLOOP_FIX                  0x20000000    //CQ10819 - for Optus BT loop fixes
// UR8_MERGE_START_END CQ11080 Manjulak/KC
#define OAMFEATURE_ENABLE_CTLM_LOW_USRATE_FIX               0x40000000    //CQ11080 - Enables fix for low US rate against CTLM in ADSL2/2+ AnnexB mode
//UR8_MERGE_START CQ10499   Jack Zhang
#define MAX_US_TONES      64
//UR8_MERGE_END CQ10499   Jack Zhang

//UR8_MERGE_START_END CQ11230 Hao-Ting Lin
#define OAMFEATURE_FEATURE1_ENABLE_TR067_DSPCB_FIX                   0x00000001    //CQ11230 - Enable TR067 DSPCB fix
//UR8_MERGE_START_END CQ11306 AdeelJ
#define OAMFEATURE_FEATURE1_ENABLE_GREECE_OTE_ANNEXB_FIX             0x00000002    //CQ11306 - Enable AnnexB fixes for OTE Greece
//UR8_MERGE_START_END CQ11260 WT100_CTLM_B900 HL
#define OAMFEATURE_FEATURE1_ENABLE_CTLM_B900_O2_FIX                  0x00000004    //CQ11260 - Enable CTLM O2 fix
//UR8_MERGE_START_END CQ11260 WT100_CTLM_B900 HL
#define OAMFEATURE_FEATURE1_ENABLE_FORCED_US_PCB_14DB_NULLLOOP       0x00000008    //CQ11260 - Enable forced US PCB
//UR8_MERGE_START CQ11307 Ram
//UR8_MERGE_START CQ11425 KCCHEN
#define OAMFEATURE_FEATURE1_ENABLE_CNXT_PMD_FIX                      0x00000010    //CQ11425 - ENABLE CNXT's PMD FIX
//UR8_MERGE_START_END CQ11307 Ram
#define OAMFEATURE_FEATURE1_ENABLE_BRITISH_TELECOM_FIX               0x00000020    //CQ11307
// UR8_MERGE_START_START_END CQ11661 HL
#define OAMFEATURE_FEATURE1_ENABLE_ADSL2PLUS_FASTPATH_STABILITY      0x00000040    //CQ11661 HL
//UR8_MERGE_START_END CQ11818 Ram
#define OAMFEATURE_FEATURE1_ENABLE_TDC_FIX                           0x00000080    //CQ11818
//UR8_MERGE_START_END CQ11803 KCCHEN/Ram
#define OAMFEATURE_FEATURE1_ENABLE_USATTNDR_UPDATE_FIX               0x00000100    //CQ11803
//UR8_MERGE_START_END CQ11931 Ram
#define OAMFEATURE_FEATURE1_ENABLE_FORCED_MONITORED_TONES             0x00000200    //CQ11931

typedef struct tagT1413Info
{
  unsigned char  VendorId[2];
  unsigned char  t1413Revision;
  unsigned char  VendorRevision;
} T1413INFO;


typedef struct tagTIOIDINFO
{
  unsigned int   bState;           /* addr->bDSPATURState    */
#ifdef AR7_EFM
  unsigned int   CurrDslState;
#endif
  unsigned int   clear_eoc;        /* 1: clearEOC msg indicator; 0: otherwise */
  unsigned int   USConRate;        /* US Conection Rates */
  unsigned int   DSConRate;        /* DS Connection Rates */
  unsigned int   USPayload;        /* ennic_tx_pullup*/
  unsigned int   DSPayload;        /* ennic_indicate_receive_packet*/
  unsigned int   FrmMode;          /* addr->atur_msg.framing_mode*/
  unsigned int   MaxFrmMode;
  unsigned int   TrainedPath;      /* Status of the Modem in which trained (Fast or Interleaved Path) */
  unsigned int   TrainedMode;      /* Status of the mode in which the modem is trained (G.dmt, T1.413, etc) */
  //UR8_MERGE_START CQ10442 Manjula K
  unsigned int   SRA;              /* 1: SRA indicator; 0: otherwise */
  //UR8_MERGE_END CQ10442

  /* Superframe Count */
  unsigned int   usSuperFrmCnt;    /* Num of US Superframes */
  unsigned int   dsSuperFrmCnt;    /* Num of DS Superframes */

  /* LOS & SEF Stats */
  unsigned int   LOS_errors;       /* Num of ADSL frames where loss-of-sync */
  unsigned int   coLosErrors;      /* CO LOS Defects */
#if 1 // LOF  CQ10226
  unsigned int   LOF_errors;       /* Number of times Loss Of Framing happened */
  unsigned int   coLofErrors;      /* Number of times Loss Of Framing happened in CO */
  unsigned int   LOF_f;              /* Loss Of Framing flag. 1=set, 0=clear */
  unsigned int   coLOF_f;            /* CO Loss Of Framing flag. 1=set, 0=clear */
  unsigned int   erroredSeconds;   /* Downstream errored seconds. Currently support only downstream and ADSL2/ADSL2+. 11/2005 */
  //UR8_MERGE_START Report_SES Manjula K
  //CQ10369
  unsigned int   severelyerrsecs;   /* Downstream severely errored seconds. Currently support only downstream and ADSL2/ADSL2+.*/
  //UR8_MERGE_END Report_SES

//  unsigned int   coErroredSeconds;   /* CO errored seconds */
#endif
  unsigned int   SEF_errors;       /* Num of severly errored ADSL frames - LOS > MAXBADSYNC ADSL frames */
  unsigned int   coRdiErrors; /* CO RDI defects */
  /* CRC Stats */
  unsigned int   usICRC_errors;    /* Num of Upstream CRC errored ADSL frames on Interleaved Path */
  unsigned int   usFCRC_errors;    /* Num of Upstream CRC errored ADSL frames on Fast Path */
  unsigned int   dsICRC_errors;    /* Num of Downstream CRC errored ADSL frames on Interleaved Path */
  unsigned int   dsFCRC_errors;    /* Num of Downstream CRC errored ADSL frames on Fast Path */

  /* FEC Stats */
  unsigned int   usIFEC_errors;    /* Num of Upstream FEC errored (corrected) ADSL frames on Interleaved Path */
  unsigned int   usFFEC_errors;    /* Num of Upstream FEC errored (corrected) ADSL frames on Fast Path */
  unsigned int   dsIFEC_errors;    /* Num of Downstream FEC errored (corrected) ADSL frames on Interleaved Path */
  unsigned int   dsFFEC_errors;    /* Num of Downstream FEC errored (corrected) ADSL frames on Fast Path */

  /* NCD Stats */
  unsigned int   usINCD_error;     /* UpStream No Cell Delineation on Interleaved Path */
  unsigned int   usFNCD_error;     /* UpStream No Cell Delineation on Fast Path */
  unsigned int   dsINCD_error;     /* Downstream No Cell Delineation on Interleaved Path */
  unsigned int   dsFNCD_error;     /* Downstream No Cell Delineation on Fast Path */

  /* LCD Stats */
  unsigned int   usILCD_errors;    /* UpStream Loss of Cell Delineation (within the same connection) on Interleaved Path */
  unsigned int   usFLCD_errors;    /* UpStream Loss of Cell Delineation (within the same connection) on Fast Path */
  unsigned int   dsILCD_errors;    /* Downstream Loss of Cell Delineation (within the same connection) on Interleaved Path */
  unsigned int   dsFLCD_errors;    /* Downstream Loss of Cell Delineation (within the same connection) on Fast Path */

  /* HEC Stats */
  unsigned int   usIHEC_errors;    /* Num of Upstream HEC errored ADSL frames on Interleaved Path */
  unsigned int   usFHEC_errors;    /* Num of Upstream HEC errored ADSL frames on Fast Path */
  unsigned int   dsIHEC_errors;    /* Num of Downstream HEC errored ADSL frames on Interleaved Path */
  unsigned int   dsFHEC_errors;    /* Num of Downstream HEC errored ADSL frames on Fast Path */

  /* Upstream ATM Stats */
  unsigned int   usAtm_count[2];      /* Upstream Good Cell Count */
  unsigned int   usIdle_count[2];     /* Upstream Idle Cell Count */
  unsigned int   usPdu_count[2];      /* UpStream PDU Count */

  /* Downstream ATM Stats */
  unsigned int   dsGood_count[2];     /* Downstream Good Cell Count */
  unsigned int   dsIdle_count[2];     /* Downstream Idle Cell Count */
  unsigned int   dsBadHec_count[2];   /* Downstream Bad Hec Cell Count */
  unsigned int   dsOVFDrop_count[2];  /* Downstream Overflow Dropped Cell Count */
  unsigned int   dsPdu_count[2];      /* Downstream PDU Count */
                             /* (only looks for end of pdu on good atm cells received, */
                             /* not on Bad_Hec or Overflow cell) */

  unsigned int   dsLineAttn;       /* DS Line Attenuation */
//  UR8_MERGE_START_END   CQ11228 HZ
  signed int     dsMargin;         /* Measured DS MArgin */

  unsigned int   usLineAttn;
  // UR8_MERGE_START_END CQ11247_TR69_DS_LATN_SATN  YW
  int   usMargin;

  unsigned char    bCMsgs1[12];  // (was 6) used by both cMsgs1 & cMsg-PCB now, sync from dev_host_interface.h
  unsigned char    bRMsgs1[6];
  unsigned char    bCRates2;
  unsigned char    bRRates2;
  unsigned char    bRRates1[4][11];
  unsigned char    bCMsgs2[4];
  unsigned char    bCRates1[4][30];
  unsigned char    bRMsgs2[4];

  unsigned int   USPeakCellRate;

  unsigned int   dsl_status;
  unsigned int   dsl_modulation;
  unsigned char  dsl_ghsRxBuf[10][64];
  unsigned char  dsl_GHS_msg_type[2];

  int     TxVCs[12];
  int     RxVCs[12];

  unsigned int   vci_vpi_val;

  unsigned char  BitAllocTblDstrm[512];
//UR8_MERGE_START CQ10499   Jack Zhang
  unsigned char  BitAllocTblUstrm[MAX_US_TONES];
//UR8_MERGE_END CQ10499   Jack Zhang
    signed char  marginTblDstrm[512];
  unsigned char  rBng[1024];
  unsigned char  cBng[128];
           int   usTxPower;
           int   dsTxPower;
         short   rxSnrPerBin0[512];
         short   rxSnrPerBin1[512];
         short   rxSnrPerBin2[512];

  unsigned int   StdMode;
  unsigned int   atucVendorId;
  unsigned char  currentHybridNum;
  unsigned char  atucRevisionNum;
  unsigned int   trainFails;
  unsigned int   trainFailStates[30];
  unsigned int   idleTick;
  unsigned int   initTick;
  unsigned int   showtimeTick;
  unsigned char  dsFastParityBytesPerSymbol;
  unsigned char  dsIntlvParityBytesPerSymbol;
  unsigned char  dsSymbolsPerCodeWord;
  unsigned int   dsInterleaverDepth;
  unsigned char  usFastParityBytesPerSymbol;
  unsigned char  usIntlvParityBytesPerSymbol;
  unsigned char  usSymbolsPerCodeWord;
  unsigned int   usInterleaverDepth;
  unsigned int   atmBertBitCountLow;
  unsigned int   atmBertBitCountHigh;
  unsigned int   atmBertBitErrorCountLow;
  unsigned int   atmBertBitErrorCountHigh;
  unsigned int   lineLength;
  unsigned int   grossGain;
           int   rxNoisePower0[512];
           int   rxNoisePower1[512];
           int   showtimeCount;
           int   trellis;
           int   dsNoise[512];
  unsigned int   useBitField;   /* Use of bitfields without translation for backward compatibility. */
  unsigned short annex_selected;
  unsigned short psd_mask_qualifier;
  unsigned int   max_ds_tones;
  unsigned int   max_us_tones;
  unsigned int   LPR;    /* Loss of Power- dying gasp occurred */

#if 1
  // added for TR69
  unsigned char  ghsATUCVendorId[8];  /* country code(2) + VID(4) + Vendor Specific(2) */
  unsigned char  ghsATURVendorId[8];  /* country code(2) + VID(4) + Vendor Specific(2) */
  T1413INFO      t1413ATUC;
  T1413INFO      t1413ATUR;
#endif
//  UR8_MERGE_START CQ10979   Jack Zhang
  unsigned int   totalInitErrs;    // Total number of Initialization Errors.
  unsigned int   totalInitTOs;     // Total number of Initialization Timeout Errors.
  unsigned int   showtimeInitErrs; // Number of Init. Errs since the most recent showtime.
  unsigned int   showtimeInitTOs;  // Number of Init. Timeout Errs. since the most recent showtime.
  unsigned int   lastshowInitErrs; // Number of Init. Errs since the 2nd most recent showtime.
  unsigned int   lastshowInitTOs;  // Number of Init. Timeout Errs. since the 2nd most recent showtime.
  int            dsACTPSD;    // Downstream actual power spectral density.
  int            usACTPSD;    // Upstream actual power spectral density.
  int            dsHLINSC;    // Downstream linear representation scale.
//UR8_MERGE_START CQ11709 Tim
  unsigned short dsHlogMT;    //Downstream Hlog measurement time
  unsigned short dsSNRMT;     //Downstream SNR measurement time
  unsigned short dsQLNMT;     //Downstream QLN measurement time
//UR8_MERGE_END CQ11709 Tim
//UR8_MERGE_START_END CQ11544 Tim
  int            uncancelledEcho;
//  UR8_MERGE_END   CQ10979*
//  UR8_MERGE_START CQ10978   Jack Zhang
  unsigned char  pwrStatus;   // DSL Power Management Status.
//  UINT8                 pad[3];      // pading to 32 bits.
//  UR8_MERGE_END   CQ10978*

#ifdef CLI2HOST_SUPPORT
  unsigned int   cliBuffsize;    /* This value does not contain vars after raw clidata (CQ10242)*/
  unsigned char  *p_cliBuffAddr; /* This is in MIPS context */
  unsigned int   cliRedirect;    /* The redirect mode: McBSP(default), redirect, both */
  unsigned int   readPointer;    /* Readindex of the circular buffer */
#endif //CLI2HOST_SUPPORT

  // Add Nfec/Rp/Lp
  unsigned int   usNFEC;           // codeword size
  unsigned int   usINP;
  unsigned short usLp;
  unsigned short usRp;

  unsigned int   dsNFEC;           // codeword size
  unsigned int   dsINP;
  unsigned short dsLp;
  unsigned short dsRp;

}TIOIDINFO,*PTIOIDINFO;

typedef struct{
  unsigned char    bCMsgs1[6];
  unsigned char    bCRates2;
  unsigned char    bRRates2;
  unsigned char    bRRates1[4][11];
  unsigned char    bCMsgs2[4];
  unsigned char    bCRates1[4][30];
  unsigned char    bCRatesRA[4][30];
  unsigned char    bRMsgs2[4];
  unsigned char    bRRatesRA[4];
  unsigned char    bRMsgsRA[12];
  unsigned char    bCMsgsRA[6];
}negoMsgs;

typedef struct{
  unsigned char    cMsgFmt[2];
  unsigned char    rMsgFmt[2];
  unsigned char    cMsgPcb[12];
  unsigned char    rMsgPcb[70];
  unsigned char    dummy1[2];
  unsigned char    cMsg1[40];
  unsigned char    rMsg1[4];
  unsigned char    cMsg2[8];
  unsigned char    rMsg2[64];
  unsigned char    cParams[264];
  unsigned char    rParams[2088];
  unsigned short   cMsgPcbLen;
  unsigned short   rMsgPcbLen;
  unsigned short   cMsg1Len;
  unsigned short   rMsg1Len;
  unsigned short   cMsg2Len;
  unsigned short   rMsg2Len;
  unsigned short   cParamsLen;
  unsigned short   rParamsLen;
}adsl2Msgs;

typedef struct{
  unsigned char    rMsg1Ld[16];
  unsigned char    rMsg2Ld[260];
  unsigned char    rMsg3Ld[260];
  unsigned char    rMsg4Ld[260];
  unsigned char    rMsg5Ld[260];
  unsigned char    rMsg6Ld[260];
  unsigned char    rMsg7Ld[260];
  unsigned char    rMsg8Ld[260];
  unsigned char    rMsg9Ld[260];
  unsigned char    rMsg10Ld[260];
  unsigned char    rMsg11Ld[260];
  unsigned char    rMsg12Ld[260];
  unsigned char    rMsg13Ld[260];
  unsigned char    rMsg14Ld[260];
  unsigned char    rMsg15Ld[260];
  unsigned char    rMsg16Ld[260];
  unsigned char    rMsg17Ld[260];
  unsigned char    cMsg1Ld[16];
  unsigned char    cMsg2Ld[260];
  unsigned char    cMsg3Ld[132];
  unsigned char    cMsg4Ld[68];
  unsigned char    cMsg5Ld[68];
  unsigned short   rMsg1LdLen;
  unsigned short   rMsgxLdLen;
  unsigned short   cMsg1LdLen;
  unsigned short   cMsg2LdLen;
  unsigned short   cMsg3LdLen;
  unsigned short   cMsg4LdLen;
  unsigned short   cMsg5LdLen;
  unsigned short   dummy8;
}adsl2DeltMsgs;

typedef struct{
  unsigned char    trellisFlag;
  unsigned char    rateAdaptFlag;
  unsigned char    marginMonitorTraining;
  unsigned char    marginMonitorShowtime;
    signed char    marginThreshold;
  unsigned char    disableLosFlag;
  unsigned char    aturConfig[30];
  unsigned char    eocVendorId[8];
  unsigned char    eocSerialNumber[32];
  unsigned char    eocRevisionNumber[4];
}currentPhySettings;


typedef struct
{
  unsigned int  PmemStartWtAddr;              /* source address in host memory */
  unsigned int  OverlayXferCount;             /* number of 32bit words to be transfered */
  unsigned int  BinAddr;                      /* destination address in dsp's pmem */
  unsigned int  overlayHostAddr;
  unsigned int  olayPageCrc32;
  unsigned int  SecOffset;
} OlayDP_Def;

typedef struct
{
  unsigned int    timeStamp; /* TimeStp revision  */
  unsigned char   major;     /* Major revision    */
  unsigned char   minor;     /* Minor revision    */
  unsigned char   bugFix;    /* BugFix revision   */
  unsigned char   buildNum;  /* BuildNum revision */
  unsigned char   reserved;  /* for future use    */
}dspVer;

typedef struct{
  unsigned char major;
  unsigned char minor;
  unsigned char bugfix;
  unsigned char buildNum;
  unsigned int  timeStamp;
}dslVer;

typedef struct{
  unsigned char bitSwapCommand[6];
  unsigned char bitSwapBinNum[6];
  unsigned char bitSwapSFrmCnt;
}dslBitSwapDef;

typedef struct{
  unsigned int aturState;
  unsigned int subStateIndex;
  unsigned int timeStamp;
}trainStateInfo;

typedef struct{
  unsigned char ctrlBits;
  unsigned char infoBits;
}eocMessageDef;

typedef struct{
  unsigned int phyControlWord;
  unsigned int phyEnableDisableWord;
}phySettings;

enum
{
  RSTATE_TEST,
  RSTATE_IDLE,
  RSTATE_INIT,
  RSTATE_HS,
  RSTATE_RTDL,
  RSTATE_SHOWTIME,
};

typedef enum
{
  ATU_RZERO1      = 100,
  ATU_RTEST       = 101,
  ATU_RIDLE       = 102,
  ATU_RINIT       = 103,
  ATU_RRESET      = 104,
  GDMT_NSFLR      = 105,
  GDMT_TONE       = 106,
  GDMT_SILENT     = 107,
  GDMT_NEGO       = 108,
  GDMT_FAIL       = 109,
  GDMT_ACKX       = 110,
  GDMT_QUIET2     = 111,
  ATU_RZERO2      = 200,
  T1413_NSFLR     = 201,
  T1413_ACTREQ    = 202,
  T1413_ACTMON    = 203,
  T1413_FAIL      = 204,
  T1413_ACKX      = 205,
  T1413_QUIET2    = 206,
  ATU_RQUIET2     = 207,
  ATU_RREVERB1    = 208,
  ATU_RQUIET3     = 209,
  ATU_RECT        = 210,
  ATU_RREVERB2    = 211,
  ATU_RSEGUE1     = 212,
  ATU_RREVERB3    = 213,
  ATU_RSEGUE2     = 214,
  ATU_RRATES1     = 215,
  ATU_RMSGS1      = 216,
  ATU_RMEDLEY     = 217,
  ATU_RREVERB4    = 218,
  ATU_RSEGUE3     = 219,
  ATU_RMSGSRA     = 220,
  ATU_RRATESRA    = 221,
  ATU_RREVERBRA   = 222,
  ATU_RSEGUERA    = 223,
  ATU_RMSGS2      = 224,
  ATU_RRATES2     = 225,
  ATU_RREVERB5    = 226,
  ATU_RSEGUE4     = 227,
  ATU_RBNG        = 228,
  ATU_RREVERB6    = 229,
  ATU_RSHOWTIME   = 230,
  ATU_RZERO3      = 300,
  ADSL2_QUIET1    = 301,
  ADSL2_COMB1     = 302,
  ADSL2_QUIET2    = 303,
  ADSL2_COMB2     = 304,
  ADSL2_ICOMB1    = 305,
  ADSL2_LINEPROBE = 306,
  ADSL2_QUIET3    = 307,
  ADSL2_COMB3     = 308,
  ADSL2_ICOMB2    = 309,
  ADSL2_RMSGFMT   = 310,
  ADSL2_RMSGPCB   = 311,
  ADSL2_REVERB1   = 312,
  ADSL2_QUIET4    = 313,
  ADSL2_REVERB2   = 314,
  ADSL2_QUIET5    = 315,
  ADSL2_REVERB3   = 316,
  ADSL2_ECT       = 317,
  ADSL2_REVERB4   = 318,
  ADSL2_SEGUE1    = 319,
  ADSL2_REVERB5   = 320,
  ADSL2_SEGUE2    = 321,
  ADSL2_RMSG1     = 322,
  ADSL2_MEDLEY    = 323,
  ADSL2_EXCHANGE  = 324,
  ADSL2_RMSG2     = 325,
  ADSL2_REVERB6   = 326,
  ADSL2_SEGUE3    = 327,
  ADSL2_RPARAMS   = 328,
  ADSL2_REVERB7   = 329,
  ADSL2_SEGUE4    = 330,
  ATU_RZERO4      = 400,
  DELT_SEGUE1     = 401,
  DELT_REVERB5    = 402,
  DELT_SEGUE2     = 403,
  DELT_EXCHANGE   = 404,
  DELT_SEGUELD    = 405,
  DELT_RMSGLD     = 406,
  DELT_QUIET1LD   = 407,
  DELT_QUIET2LD   = 408,
  DELT_RACK1      = 409,
  DELT_RNACK1     = 410,
  DELT_QUIETLAST  = 411
} modemStates_t;

enum
{
  DSLTRAIN_NO_MODE,
  DSLTRAIN_MULTI_MODE,
  DSLTRAIN_T1413_MODE,
  DSLTRAIN_GDMT_MODE,
  DSLTRAIN_GLITE_MODE
};

enum
{
  ID_RESTORE_DEFAULT_LED,
  ID_DSL_LINK_LED,
  ID_DSL_ACT_LED
};


#ifndef TC_MODE
#define TC_MODE
// This is also defined in tn7efm.h
// Send with DSP_GHS_TCMODE in parameter 0 to host.
enum {
  TC_MODE_STM,
  TC_MODE_ATM,
  TC_MODE_PTM,  // EFM mode
};
#endif

typedef struct _ITIDSLHW
{
  /* struct _TIDSL_IHwVtbl * pVtbl; */
  unsigned char*    fwimage;
  void*             pmainAddr;
  void*             pOsContext;
  unsigned int      ReferenceCount;
  unsigned int      netService;

  int               InitFlag;

  int               imagesize;

  unsigned int      lConnected;
  unsigned int      bStatisticsInitialized;
  unsigned int      rState;
  unsigned int      bShutdown;
  unsigned int      blackOutValid_f;
  unsigned char     blackOutBits[64];
  unsigned int      bAutoRetrain;
  volatile unsigned int     bOverlayPageLoaded;
  unsigned int      stateTransition;
  unsigned int      configFlag;
  unsigned int      dsBitSwapInx;
  unsigned int      usBitSwapInx;
  unsigned int      trainStateInx;
  unsigned int      usEocMsgInx;
  unsigned int      dsEocMsgInx;
  unsigned int      reasonForDrop;
  unsigned int      numProfiles;
  unsigned int      numOlayPages;
  TIOIDINFO         AppData;
  dspVer            dspVer;

  OlayDP_Def        olayDpPage[32];
  // UR8_MERGE_START_END CQ11922 Tim
  OlayDP_Def        coProfiles[6];
  OlayDP_Def        constDisplay;
  dslBitSwapDef     dsBitSwap[30];
  dslBitSwapDef     usBitSwap[30];
  trainStateInfo    trainHistory[120];
  eocMessageDef     usEocMsgBuf[30];
  eocMessageDef     dsEocMsgBuf[30];
  adsl2Msgs         adsl2TrainingMessages;
  adsl2DeltMsgs     adsl2DiagnosticMessages;
  unsigned int      modemStateBitField[4];

// UR8_MERGE_START CQ11466
#ifdef AR7_EFM
  int ghs_TCmode;                 /* TC_MODE_ATM or TC_MODE_PTM (EFM) */
#endif  
// UR8_MERGE_END CQ11466

#ifdef INTERNAL_BUILD
  internalParameters internalVars;
#endif
} ITIDSLHW_T, *PITIDSLHW_T, tidsl_t;


/**********************************************************************************
* API proto type defines
**********************************************************************************/

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
* Notes: external function osAllocateMemory(), osZeroMemory(), osLoadFWImage() are required
*****************************************************************************************/
int dslhal_api_dslStartup
(
 PITIDSLHW_T *ppIHw
);


/********************************************************************************************
* FUNCTION NAME: dslhal_api_gatherStatistics
*
*********************************************************************************************
* DESCRIPTION: Read statistical infromation from ax5 modem daugter card.
* Input: tidsl_t *ptidsl
*
* Return: 0    success
*         1    failed
*
********************************************************************************************/

void dslhal_api_gatherStatistics
(
 tidsl_t * ptidsl
);


/********************************************************************************************
* FUNCTION NAME: dslhal_api_initStatistics
*
*********************************************************************************************
* DESCRIPTION: init statistical infromation of ax5 modem daugter card.
*
* Input: tidsl_t *ptidsl
*
* Return: NULL
*
********************************************************************************************/

void dslhal_api_initStatistics
(
 tidsl_t * ptidsl
);



/******************************************************************************************
* FUNCTION NAME:    dslhal_api_getDslDriverVersion
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

void dslhal_api_getDslHalVersion
(
 void  *pVer
);

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
* Notes: external function osFreeMemory() is required.
*****************************************************************************************/

int dslhal_api_dslShutdown
(
 tidsl_t *ptidsl
);


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
*
*****************************************************************************************/

int dslhal_api_getDspVersion
(
 tidsl_t *ptidsl,
 void  *pVer
);


/********************************************************************************************
* FUNCTION NAME: dslhal_diags_digi_memTestA()
*
*********************************************************************************************
* DESCRIPTION: This function does the digital tests on the DSP. It does the DSP ID test,
*              memory tests on the external and internal memories of DSP, Codec Interconnect
*              test and Interrupt Test.
*
* Input:       Test selects the test to be performed based on the elements set of 9 element
*              array passed the the parameter.
*
* Return:      Status of the Tests Failed
*
********************************************************************************************/

unsigned int dslhal_diags_digi_memTestA
(
unsigned int*   Test
);


/********************************************************************************************
* FUNCTION NAME: dslhal_diags_digi_memTestB()
*
*********************************************************************************************
* DESCRIPTION: This function does the digital tests on the DSP. It does the DSP ID test,
*              memory tests on the external and internal memories of DSP, Codec Interconnect
*              test and Interrupt Test.
*
* Input:       Test selects the digital test to be performed.
*
* Return:      Status of the Tests Failed
*
********************************************************************************************/

unsigned int dslhal_diags_digi_memTestB
(
unsigned int   Test,
unsigned int   *Status
);


/********************************************************************************************
* FUNCTION NAME: dslhal_diags_anlg_tonesTestA()
*
*********************************************************************************************
* DESCRIPTION: This function instructs the Transciever to transmit the Reverb with missing
*              tones and Medley's with missing tones. These signals are defined in ITU
*              G.992.1 ADSL Standards.
*
* Input: Test selects between the Reverb or Medley tests. 0 - Reverb, 1 - Medley
*        Array is a 64 element unsigned integer type array. The element of this array
*              describe which tones are to be generated by selecting the element of
*              the array to be non zero.
* Return: NULL
*
********************************************************************************************/

void dslhal_diags_anlg_tonesTestA
(
unsigned int   Test,
unsigned int* Array
);


/********************************************************************************************
* FUNCTION NAME: dslhal_diags_anlg_tonesTestB()
*
*********************************************************************************************
* DESCRIPTION: This function instructs the Transciever to transmit the Reverb with missing
*              tones and Medley's with missing tones. These signals are defined in ITU
*              G.992.1 ADSL Standards.
*
* Input: Test selects between the Reverb or Medley tests. 0 - Reverb, 1 - Medley
*        Array is a 64 element unsigned integer type array. The element of this array
*              describe which tones are to be generated by selecting the element of
*              the array to be non zero.
* Return: NULL
*
********************************************************************************************/

void dslhal_diags_anlg_tonesTestB
(
unsigned int   Test,
unsigned int  Tones
);

/********************************************************************************************
* FUNCTION NAME: dslhal_diags_anlg_rxNoiseTest()
*
*********************************************************************************************
* DESCRIPTION: This function instructs the Transciever to transmit the Reverb with missing
*              tones and Medley's with missing tones. These signals are defined in ITU
*              G.992.1 ADSL Standards.
*
* Input: Test selects between the Reverb or Medley tests. 0 - Reverb, 1 - Medley
*        Tones selects the .
* Return: NULL
*
********************************************************************************************/

void dslhal_diags_anlg_rxNoiseTest
(int agcFlag,
short pga1,
short pga2,
short pga3,
short aeq
);

/********************************************************************************************
* FUNCTION NAME: dslhal_diags_anlg_ecNoiseTest()
*
*********************************************************************************************
* DESCRIPTION: This function instructs the Transciever to transmit the Reverb with missing
*              tones and Medley's with missing tones. These signals are defined in ITU
*              G.992.1 ADSL Standards.
*
* Input: Test selects between the Reverb or Medley tests. 0 - Reverb, 1 - Medley
*        Tones selects the .
* Return: NULL
*
********************************************************************************************/

void dslhal_diags_anlg_ecNoiseTest
(int agcFlag,
short pga1,
short pga2,
short pga3,
short aeq
);


/********************************************************************************************
* FUNCTION NAME: dslhal_api_pollTrainingStatus()
*
*********************************************************************************************
* DESCRIPTION: code to decode modem status and to start modem training
*
* Input: tidsl_t *ptidsl
*
* Return: 0-? status mode training
*         -1    failed
*
********************************************************************************************/
int dslhal_api_pollTrainingStatus
(
 tidsl_t *ptidsl
);



#ifdef CLI2HOST_SUPPORT
/********************************************************************************************
* FUNCTION NAME: dslhal_api_redirectInit() //(CQ10242)
*
*********************************************************************************************
* DESCRIPTION:
*   Initializes the redirect buffer on the DSP
*
* Input: pointer ptidsl
*
* Return:
*
* NOTE:
*
********************************************************************************************/
void dslhal_api_redirectInit(ITIDSLHW_T *ptidsl);



/********************************************************************************************
* FUNCTION NAME: dslhal_api_redirectMode() //(CQ10242)
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
void dslhal_api_redirectMode(ITIDSLHW_T *ptidsl, unsigned char mode);



/********************************************************************************************
* FUNCTION NAME: dslhal_api_redirectPrint() //(CQ10242)
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
//UR8_MERGE_START CQ11813 Hao-Ting
#ifndef LINUX_CLI_SUPPORT
void dslhal_api_redirectPrint(ITIDSLHW_T *ptidsl);
#endif
//UR8_MERGE_END CQ11813


//UR8_MERGE_START GERALD_CQ_REDIRECT_BUGFIX2_20060328 Gerald
/********************************************************************************************
* FUNCTION NAME: dslhal_api_redirectFree() //(CQ10242)
*
*********************************************************************************************
* DESCRIPTION:
*   frees the memory and cleans variables up in terms of cliRedirect
*
* Input: pointer ptidsl
*
* Return:
*
* NOTE:
*
********************************************************************************************/
void dslhal_api_redirectFree(ITIDSLHW_T *ptidsl);
#endif //CLI2HOST_SUPPORT
//UR8_MERGE_END  GERALD_CQ_REDIRECT_BUGFIX2_20060328


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
int dslhal_api_handleTrainingInterrupt
(
 tidsl_t *ptidsl,
 int intrSource
);

/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_setEocSerialNumber(tidsl_t *ptidsl,char *SerialNumber)
 *
 *******************************************************************************************
 * DESCRIPTION: This fuction Sets the EOC Serial Number
 *
 * INPUT:  PITIDSLHW_T *ptidsl
 *         char *SerialNumber : Input EOC Serial Number
 *
 * RETURN: 0 SUCCESS
 *         1 FAILED
 *
 *****************************************************************************************/
unsigned int dslhal_api_setEocSerialNumber
(
tidsl_t *ptidsl,
char *SerialNumber
);

/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_setEocVendorId(tidsl_t *ptidsl,char *VendorID)
 *
 *******************************************************************************************
 * DESCRIPTION: This fuction Sets the EOC Serial Number
 *
 * INPUT:  PITIDSLHW_T *ptidsl
 *         char *VendorID : EOC Vendor ID
 *
 * RETURN: 0 SUCCESS
 *         1 FAILED
 *
 *****************************************************************************************/
unsigned int dslhal_api_setEocVendorId
(
tidsl_t *ptidsl,
char *VendorID
);

/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_setEocRevisionNumber(tidsl_t *ptidsl,char *RevNum)
 *
 *******************************************************************************************
 * DESCRIPTION: This fuction Sets the EOC Revision Number
 *
 * INPUT:  PITIDSLHW_T *ptidsl
 *         char *RevNum : Input EOC Revision Number
 *
 * RETURN: 0 SUCCESS
 *         1 FAILED
 *
 *****************************************************************************************/
unsigned int dslhal_api_setEocRevisionNumber
(
tidsl_t *ptidsl,
char *RevNumber
);

/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_boostDspFrequency(void)
 *
 *******************************************************************************************
 * DESCRIPTION: This fuction will boost DSP frequency to 250Mhz
 *
 * INPUT:  none
 *
 * RETURN: 0 SUCCESS
 *         1 FAILED
 *
 *****************************************************************************************/
unsigned int dslhal_api_boostDspFrequency(void);

/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_setAturConfig(tidsl_t *ptidsl,char *ATURConfig)
 *
 *******************************************************************************************
 * DESCRIPTION: This fuction Sets the EOC ATUR Config Register
 *
 * INPUT:  PITIDSLHW_T *ptidsl
 *         char *RevNum : Input EOC ATUR Config Register
 *
 * RETURN: 0 SUCCESS
 *         1 FAILED
 *
 *****************************************************************************************/
unsigned int dslhal_api_setAturConfig
(
tidsl_t *ptidsl,
char *ATURConfig
);

/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_disableLosAlarm(tidsl_t *ptidsl, unsigned int set)
 *
 *******************************************************************************************
 * DESCRIPTION: This fuction disables all the LOS alarms
 *
 * INPUT:  PITIDSLHW_T *ptidsl
 *         unsigned int set // if set == TRUE : Disable LOS alarms, else enable
 *
 * RETURN: 0 SUCCESS
 *         1 FAILED
 * NOTES:  Currently not supported in any version other than MR4 Patch release..
 *****************************************************************************************/
unsigned int dslhal_api_disableLosAlarm
(
tidsl_t *ptidsl,
unsigned int
);

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
unsigned int dslhal_api_sendIdle
(
tidsl_t *ptidsl
);

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
unsigned int dslhal_api_sendQuiet
(
tidsl_t *ptidsl
);

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
unsigned int dslhal_api_sendDgasp
(
tidsl_t *ptidsl
);

/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_setMarginThreshold(tidsl_t *ptidsl, int threshold)
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
unsigned int dslhal_api_setMarginThreshold
(
tidsl_t *ptidsl,
int threshold
);


/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_setMarginMonitorFlags(tidsl_t *ptidsl, unsigned int trainflag,unsigned int shwtflag)
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
unsigned int dslhal_api_setMarginMonitorFlags
(
tidsl_t *ptidsl,
unsigned int trainflag,
unsigned int shwtflag
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setRateAdaptFlag(tidsl_t *ptidsl)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Rate Adapt Enable Flag
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int flag //if flag = TRUE set flag else reset
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_api_setRateAdaptFlag
(
tidsl_t *ptidsl,
unsigned int flag
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setTrellisFlag(tidsl_t *ptidsl, unsigned int flag)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Trellis Coding Enable Flag
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int flag //if flag = TRUE set flag else reset
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_api_setTrellisFlag
(
tidsl_t *ptidsl,
unsigned int flag
);


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
unsigned int dslhal_api_setMaxBitsPerCarrierDownstream
(
tidsl_t *ptidsl,
unsigned int maxbits
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setMaxBitsPerCarrier(tidsl_t *ptidsl,unsigned int maxbits)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Maximum bits per carrier value in upstream band
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int maxbits : should be a value between 0-15
*
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_api_setMaxBitsPerCarrier
(
tidsl_t *ptidsl,
unsigned int maxbits
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setMaxBitsPerCarrierUpstream(tidsl_t *ptidsl,unsigned int maxbits)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Maximum bits per carrier value in upstream band
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int maxbits : should be a value between 0-15
*
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_api_setMaxBitsPerCarrierUpstream
(
tidsl_t *ptidsl,
unsigned int maxbits
);

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
unsigned int dslhal_api_setMaxInterleaverDepth
(
tidsl_t *ptidsl,
unsigned int maxdepth
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_api_setTrainingMode(tidsl_t *ptidsl,unsigned int trainmode)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the desired training mode(none/T1.413/G.dmt/G.lite)
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int trainmode :Should be between 0 and 4; 0:No Mode 1:Multimode
*                                 2: T1.413, 3:G.dmt, 4: G.lite
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/

unsigned int dslhal_api_setTrainingMode
(
tidsl_t *ptidsl,
unsigned int trainmode
);

/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_dslRetrain(tidsl_t *ptidsl)
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
unsigned int dslhal_api_dslRetrain
(
tidsl_t *ptidsl
);

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
unsigned int dslhal_api_acknowledgeInterrupt
(tidsl_t * ptidsl
);

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
unsigned int dslhal_api_disableDspHybridSelect
(tidsl_t * ptidsl,
 unsigned int disable
);

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
unsigned int dslhal_api_selectHybrid
(tidsl_t * ptidsl,
 unsigned int hybridNum
);

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
unsigned int dslhal_api_reportHybridMetrics
(tidsl_t * ptidsl,
 int     *metric
);

/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_selectInnerOuterPair(tidsl_t *ptidsl, unsigned int pairSelect)
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

unsigned int dslhal_api_selectInnerOuterPair
(tidsl_t *ptidsl,
unsigned int pairSelect
);

/******************************************************************************************
 * FUNCTION NAME:    dslhal_api_resetTrainFailureLog(tidsl_t *ptidsl, unsigned int pairSelect)
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

unsigned int dslhal_api_resetTrainFailureLog
(tidsl_t *ptidsl
);

/********************************************************************************************
* FUNCTION NAME: dslhal_api_controlLed()
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
unsigned int dslhal_api_configureLed
(tidsl_t * ptidsl,
unsigned int idLed,
unsigned int onOff
);

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
unsigned int dslhal_api_configureExternBert
(tidsl_t * ptidsl,
unsigned int configParm,
unsigned int parmVal
);

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
unsigned int dslhal_api_configureAtmBert
(tidsl_t * ptidsl,
unsigned int configParm,
unsigned int parmVal
);

/********************************************************************************************
* FUNCTION NAME: dslhal_api_configureDgaspLpr()
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
unsigned int dslhal_api_configureDgaspLpr
(tidsl_t * ptidsl,
unsigned int configParm,
unsigned int parmVal
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_onOffPcb()
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
unsigned int dslhal_advcfg_onOffPcb
(tidsl_t * ptidsl,
unsigned int onOff
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_onOffBitSwap()
*
*********************************************************************************************
* DESCRIPTION:
*   Turns on / off the power cutback feature;
* Input
*         usDs;  0 = us  and 1 = ds;
*         onOff; 0 = OFF and 1 = ON
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_onOffBitSwap
(tidsl_t * ptidsl,
 unsigned int usDs,
 unsigned int onOff
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_configDsTones()
*
*********************************************************************************************
* DESCRIPTION:
*   Turns on / off specific tones in the downstream direction;
* Input
*        pointer to the array specifying the tones to be turned on/off
*
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_configDsTones
(tidsl_t * ptidsl,
 unsigned short *dsTones
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_getAocBitSwapBuffer()
*
*********************************************************************************************
* DESCRIPTION:
*   Fetches the Tx/Rx AOC bitswap Buffer;
* Input
*        Transmit / Receive buffer to be fetched
*
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_getAocBitswapBuffer
(tidsl_t * ptidsl,
unsigned int usDs
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_readTrainingMessages()
*
*********************************************************************************************
* DESCRIPTION:
*   Reads all the training messages on demand;
* Input
*        tidsl_t *ptidsl : Pointer to application structure
*        void *msgStruct : Pointer to Message Structure
*
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_readTrainingMessages
(tidsl_t * ptidsl,
void *msgPtr
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_getTrainingState()
*
*********************************************************************************************
* DESCRIPTION:
*   Reads all the training messages on demand;
* Input
*        tidsl_t *ptidsl : Pointer to application structure
*        void *msgStruct : Pointer to training state structure
*
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_getTrainingState
(tidsl_t * ptidsl,
void *statePtr
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_resetBitSwapMessageLog()
*
*********************************************************************************************
* DESCRIPTION:
*   Clears the Aoc Bitswap Message Log
* Input
*        tidsl_t *ptidsl : Pointer to application structure
*        unsigned int usDs ; Upstream=0, Downstream=1
*
* Return: 0  success
*         1  failed
*
********************************************************************************************/
unsigned int dslhal_advcfg_resetBitSwapMessageLog
(tidsl_t * ptidsl,
 unsigned int usDs
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_setConstellationBinNumber()
*
*********************************************************************************************
* DESCRIPTION:
*   Specifies the bin number for which constellation data should be fetched
* Input
*        tidsl_t *ptidsl : Pointer to application structure
*        unsigned int binNum : constellation bin number whose data is required
*
* Return: 0  success
*         1  failed
*
********************************************************************************************/
unsigned int dslhal_advcfg_setConstellationBinNumber
(tidsl_t * ptidsl,
 unsigned int binNum
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_resetTrainStateHistory()
*
*********************************************************************************************
* DESCRIPTION:
*   Clears the Training State History Log
* Input
*        tidsl_t *ptidsl : Pointer to application structure
*
*
* Return: 0  success
*         1  failed
*
********************************************************************************************/
unsigned int dslhal_advcfg_resetTrainStateHistory
(tidsl_t * ptidsl
);


/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_logEocMessages()
*
*********************************************************************************************
* DESCRIPTION:
*   Logs EOC messages sent by the Modem to the CO
* Input
*        tidsl_t *ptidsl : Pointer to application structure
*        unsigned int eocLowerBytes : Lower [1-5] bits of EOC Message
*        unsigned int eocUpperBytes : Upper [6-13] bits of EOC Message
*
* Return: 0  success
*         1  failed
*
********************************************************************************************/
unsigned int dslhal_advcfg_logEocMessages
(tidsl_t * ptidsl,
 unsigned int usDs,
 unsigned int eocLowerBytes,
 unsigned int eocUpperBytes
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_getReasonForDrop()
*
*********************************************************************************************
* DESCRIPTION:
*   Reads the reason for dropping DSL connection;
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
unsigned int dslhal_advcfg_getReasonForDrop
(tidsl_t * ptidsl
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_ctrlMaxAvgFineGains()
*
*********************************************************************************************
* DESCRIPTION:
*   Turns on / off the host control for Max Avg Fine Gains; 0 = OFF and 1 = ON
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_ctrlMaxAvgFineGains
(tidsl_t * ptidsl,
unsigned int onOff
);


/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_setMaxAvgFineGain()
*
*********************************************************************************************
* DESCRIPTION:
*   Set the Maximum Average Fine Gain Value
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_setMaxAvgFineGain
(tidsl_t * ptidsl,
 short fineGain
);
/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_readPhySettings()
*
*********************************************************************************************
* DESCRIPTION:
*   Reads the advanced Phy layer settings on demand;
* Input
*        tidsl_t *ptidsl : Pointer to application structure
*        void *cfgStruct : Pointer to Phy Config Structure
*
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_readPhySettings
(tidsl_t * ptidsl,
void *cfgPtr
);

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_setBlackOutBits()
*
*********************************************************************************************
* DESCRIPTION:
*   Sets the Blackout Bits in the RMSGPCB message
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_setBlackOutBits
(tidsl_t * ptidsl
);

/********************************************************************************************
* FUNCTION NAME: dslhal_api_getAdvancedStats()
*
*********************************************************************************************
* DESCRIPTION: Read statistical infromation from ax5 modem daugter card.
* Input: tidsl_t *ptidsl
*
* Return: 0    success
*         1    failed
*
********************************************************************************************/
unsigned int dslhal_api_getAdvancedStats
(tidsl_t * ptidsl);

/********************************************************************************************
* FUNCTION NAME: dslhal_api_genericDspRead()
*
*********************************************************************************************
* DESCRIPTION:
*  Reads from a generic location in the host interface
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_genericDspRead
(tidsl_t * ptidsl,
 unsigned int offset1,
 unsigned int offset2,
 unsigned int offset3,
 unsigned char* localBuffer,
 unsigned int numBytes
);

/********************************************************************************************
* FUNCTION NAME: dslhal_api_genericDspWrite()
*
*********************************************************************************************
* DESCRIPTION:
*  Writes to a generic location in the host interface
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_genericDspWrite
(tidsl_t * ptidsl,
 unsigned int offset1,
 unsigned int offset2,
 unsigned int offset3,
 unsigned char* localBuffer,
 unsigned int numBytes
);

/********************************************************************************************
* FUNCTION NAME: dslhal_api_dspInterfaceRead()
*
*********************************************************************************************
* DESCRIPTION:
*  Reads from a generic location in the host interface
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_dspInterfaceRead
(tidsl_t * ptidsl,
 unsigned int baseAddr,
 unsigned int numOffsets,
 unsigned int *offsets,
 unsigned char* localBuffer,
 unsigned int numBytes
);

/********************************************************************************************
* FUNCTION NAME: dslhal_api_dspInterfaceWrite()
*
*********************************************************************************************
* DESCRIPTION:
*  Writes to a generic location in the host interface
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_api_dspInterfaceWrite
(tidsl_t * ptidsl,
 unsigned int baseAddr,
 unsigned int numOffsets,
 unsigned int *offsets,
 unsigned char* localBuffer,
 unsigned int numBytes
);

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
unsigned int dslhal_api_sendMailboxCommand
(tidsl_t *ptidsl,
unsigned int cmd
);

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
unsigned int dslhal_api_sendL3Command(tidsl_t *ptidsl);
// * UR8_MERGE_END   CQ10880*

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
unsigned int dslhal_api_setPhyFeatureController
(tidsl_t * ptidsl,
 unsigned int paramId,
 unsigned int phyCtrlWord
);


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
unsigned int dslhal_api_enableDisablePhyFeatures
(tidsl_t * ptidsl,
 unsigned int paramId,
 unsigned int phyFeatureSet
);

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
unsigned int dslhal_api_readPhyFeatureSettings
(tidsl_t * ptidsl,
unsigned int paramId,
void *phyFeature
);
// UR8_MERGE_START CQ11057   Jack Zhang

//from #include "dev_host_interface.h"

// Duplicate DEV_HOST_BIS_PMD_TEST_PARAMETERS_FROM_CO_Def_t

typedef struct tagCoPMDTestParams{

  unsigned short TestParmCOHlogfMsg[MAX_US_TONES];

  unsigned char TestParmCOQLNfMsg[MAX_US_TONES];

  unsigned char TestParmCOSNRfMsg[MAX_US_TONES];

  unsigned short co_latn;

  unsigned short co_satn;

  unsigned long co_attndr;

  signed short usMargin;                  // DSP Write, measured US margin

  signed short dummy;

  signed short co_near_actatp;

  signed short co_far_actatp;

//UR8_MERGE_START CQ11709 Tim
  unsigned short usHLOGMT;

  unsigned short usSNRMT;

  unsigned short usQLNMT;

  unsigned short dummy2;
//UR8_MERGE_END CQ11709 Tim

}CoPMDTestParams_t;
// UR8_MERGE_END CQ11057   Jack Zhang
// UR8_MERGE_START CQ11057 KCCHEN
/********************************************************************************************
* FUNCTION NAME: dslhal_api_getPMDTestus()
*
*********************************************************************************************
* DESCRIPTION:
*   Get US PMD Test parameters in ADSL2/2+ mode
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
unsigned int dslhal_api_getPMDTestus(tidsl_t * ptidsl, CoPMDTestParams_t * co_pmdtest_params_p, int flag);
#endif
// UR8_MERGE_END CQ11057 KCCHEN
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
*         int flag: 0: training HLOGpsds. 1: showtime HLOGpsds.
*         **Note: Currently only training HLOGpsds is supported.
*
* Return: DSLHAL_ERROR_NO_ERRORS  success
*         otherwise               failed
*
********************************************************************************************/

unsigned int dslhal_api_getHLOGpsds(tidsl_t * ptidsl, unsigned short *outbuf, int flag);

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
unsigned int dslhal_api_getQLNpsds(tidsl_t * ptidsl, unsigned char *outbuf, int flag);
#endif

/********************************************************************************************
* FUNCTION NAME: dslhal_api_getSNRpsds()
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
*
* Return: DSLHAL_ERROR_NO_ERRORS  success
*         otherwise               failed
*
********************************************************************************************/
#ifndef NO_ADV_STATS
unsigned int dslhal_api_getSNRpsds(tidsl_t * ptidsl, unsigned char *outbuf, int flag);
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
*
********************************************************************************************/
#ifndef NO_ADV_STATS
unsigned int dslhal_api_getHLINpsds(tidsl_t * ptidsl, unsigned char *outbuf, int flag);
#endif
//*  UR8_MERGE_END   CQ10979*

//  UR8_MERGE_START CQ11054   Jack Zhang
/********************************************************************************************
* FUNCTION NAME: dslhal_api_getHighPrecision
*
*********************************************************************************************
* DESCRIPTION: Get High Precision setting for TR-69 Support;
* Input: None
*
* Return: 0    Low -precision for backward compat.
*         1    High precision for TR-69 Support mode is on
*
********************************************************************************************/

unsigned int dslhal_api_getHighPrecision(void);

/********************************************************************************************
* FUNCTION NAME: void dslhal_api_setHighPrecision
*
*********************************************************************************************
* DESCRIPTION: Set High Precision for TR-69 Support
* Input: None
* Return: None
********************************************************************************************/

void dslhal_api_setHighPrecision(void);
//  UR8_MERGE_END   CQ11054*

//  UR8_MERGE_START CQ11803   Ram
/********************************************************************************************
* FUNCTION NAME: dslhal_api_deltaUSMaxAttainableNDR
*
*********************************************************************************************
* DESCRIPTION: Delta between current and initial Upstream Max Attainable Net Data Rate
* Input:  PITIDSLHW_T *ptidsl,
* Return: Delta US Max Attainable NDR Estimate
*
********************************************************************************************/
#ifndef NO_ADV_STATS
int dslhal_api_deltaUSMaxAttainableNDR(tidsl_t * ptidsl);
#endif
//  UR8_MERGE_END CQ11803   Ram

#ifdef INTERNAL_BUILD
#include <dsl_hal_internal_api.h>
#endif


#endif /* pairs #ifndef __DSL_APPLICATION_INTERFACE_H__ */
