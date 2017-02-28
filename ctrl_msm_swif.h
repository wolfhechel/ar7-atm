#ifndef __CTRL_MSM_SWIF_H__
#define __CTRL_MSM_SWIF_H__ 1

// Copyright Amati Communications Inc. 1997, 1998.  All rights reserved.
// ******************************************************************
// MSM_interface_U.h
// This file contains the interface constant definitions and
// function prototypes needed to use the modem state machine
// History
// 3/13/97  Mike Locke   Written
// 3/17/97  Mike Locke   Deleted reset parameter from transmitter, made chan
//                       into a short to save code space
// 3/24/97  Mike Locke   Added definition for XMT_TONE_HIGHLEVEL
// 4/7/97   Mike Locke   Added definition for DEFAULTSCALE
// 4/21/97  Mike Locke   Added TEQSHIFT
// 5/9/97   Mike Locke   Adjust echo canceler input scaling.
// 5/29/97  Mike Locke   Integrated TEQ training
// 5/30/97  Mike Locke   Added bandwidth constants
// 6/2/97   Mike Locke   Integrated CES
// 6/6/97   Mike Locke   Adjusted Reverb time scale
// 6/11/97  Mike Locke   Added RUNCES_SHIFT constant
// 6/13/97  Mike Locke   Changed TEQ_LENGTH to 31
// 6/18/97  Mike Locke   Added ChMeas code
// 6/26/97  Mike Locke   Fixed Medley transmit power
// 7/1/97   Mike Locke   Fixed CES scaling
// 7/10/97  Mike Locke   Fixed CES scaling to prevent overflow
// 7/16/97  Mike Locke   Added stuff for the ATUR
// 7/16/97  Mike Locke   Added stuff for assembly Rcv_Reverb_U
// 7/22/97  Mike Locke   Add CHID_SHIFT for ATUR
// 7/29/97  Mike Locke   Added PREC_BUF_SIZE
// 8/19/97  Mike Locke   Changed XMT_PRNS_TIMESCALE to fix echo cancelation
// 8/19/97  Mike Locke   Scaled CES down by a factor of 2 to avoid saturation
//                       on worst case echo
// 8/20/97  Mike Locke   Put in support for standard CP length
// 8/26/97  Mike Locke   Update GainType
// 8/28/97  Mike Locke   Change Xmt_DI_buffer declarations so that they are packed
// 10/7/97  Sujai Chari  Change RUNCES_SHIFT to accomodate change in FDEC
//                       coefficient scaling
// 2/5/98   Mike Locke   Add DOWNLASTBIN
// 11/19/97 Mike Locke   Add constants for randomized EC training
// 2/11/98  Mike Locke   Add DDI timing parameter to CTRL_FORE_controlDmtProcessing_Xmit_C and CTRL_FORE_controlDmtProcessing_Xmit_R
// 3/18/98  Mike Locke   Increase UP_LASTBIN to 31, remove ceslength hacks
// 3/19/98  Sujai Chari  Added in mods for ADSL over ISDN
// 3/24/98  Mike Locke   Change DOWN_FIRSTBIN to 20 for pseudo FDD
// 4/13/98  Sujai Chari  Changed REVERB_MINBIN, REVERB_MINPOWER for ADSL over ISDN
// 5/1/98   Vineet Ganju Change MAXCOMSTRINGS to 14 for rate adaptive mode
// 5/21/98  Mike Locke   Reduce transmit power to improve reliability
// 6/5/98   Mike Locke   Reduce ISDN ATUR xmit power, which together with a change
//                       to ssm_upsample_r.asm increases the actual xmit power by 4dB.
// 6/10/98  Mike Locke   Reduce ISDN ATUR xmit power to compensate for "improvements"
//                       to the ATUR AFE xmit gain.
// 7/24/98  Mike Locke   Adjust reverb detection thresholds for new algorithm
// 8/21/98  Jun Shen     Make ISDN downstream partially overlap upstream
// 8/21/98  Mike Locke   Decrease ATUR reverb shift from 11 to 10 to improve long line training
// 8/27/98  S. Bieser    Modified XMT_PRNS_FREQSCALE for AD12 codec.
// 3/1/99   Nirmal Warke Increased CES_LENGTH for AD11 pg 2.1 to account for the increased delay
//                       in the xmit path (ideally would like to "shift" ces_coeffs however we do
//                       have adequate MIPS at present for the increased ces_length)
// 09/14/99 Seidl        Changed DOWN_LASTBIN and CES_LENGTH back for the time being
// 10/13/99 Barnett      Merged in UNH Plugfest interop mod's.
// 10/19/99 Barnett      Adjusted CES_LENGTH for optimum NIC performance.
// 11/01/99 Seidl        Send UTC if profile num too great.
// 12/16/99 N. Warke     Convert to FDM codebase for ATUR by removing PREC, CES & FDEC
// 02/11/00 Nirmal Warke Changed UP_FIRSTBIN to 7.
// 02/11/00 Nirmal Warke Fixed XMT_TONE_LOWLEVEL to be 20 dB below XMT_TONE_HIGHLEVEL for ATUR.
// 01/21/00 Fred Wagner  derfmake changes
// 02/21/00 Barnett      Mod's in support of merging PCI-NIC T1.413 + G.lite.
// 11/28/00 Paul Hunt    new token RCVBINS is set appropriately according to GLITE token
// 01/11/01 U.Dasgupta   Removed GHS,GLITE tokens and added some multimode defines.
// 03/09/01 Nirmal Warke Added in constant definitions for line diagnostics
// 03/16/01 Nirmal Warke Added in constant definitions for CROSSTALK performance mods
// 04/06/01 J. Bergsagel Removed AD11_20 token (assumed always 1)
//                       Removed AI token (assumed always 1)
//                       Removed ATUC token (assumed always 0)
//                       Removed STD token (assumed always 1)
// 04/27/01 Jack Huang   Added G.hs second group tone list, 72, 88 and 96
// 05/10/01 M. Seidl     enabled 64-tone IFFT for AD11
// 05/10/01 M. Seidl     merged ANNEX B from 4000R
// 06/28/01 J. Bergsagel Removed FASTRETRAIN token (assumed always 0)
// 08/02/01 U.Dasgupta   changed DOWN_FIRSTBIN to 60 and UP_LASTBIN to 59 for ISDN AnnexB
// 11/05/01 U.Dasgupta   Changed XMT_PRNS_FREQSCALE to 1600 (same as POTS) for ISDN to get
//                       correct xmt power.
// 03/06/02 N. Warke     Moved time constants related to SNR calculation to this
//                       file to be consistent
// 01/21/03 MCB          Implemented Ax7 UNIT-MODULE modular software framework.
// 02/18/03 Mallesh      Defined UP_FIRSTBIN_UP according to changes in msm_interface_u.h
// 02/21/03 A. Redfern   Moved PHY_PATH1 and 2 constants for superloop.
// 03/21/03 F. Mujica    This headers uses ctrl_main.h
// 04/03/03 J. Bergsagel Moved enum for stringtable token values here instead of ctrl_msm_main.c
// 10/15/02 U. Iyer      Added SQRT2_XMT_PRNS_FREQSCALE and SQRT2_RCV_PRNS_FREQSCALE for DMT_BIS support
// 12/04/02 Shahed Molla Modified MAXCOMSTRINGS define to support more fields in globalvar.pString[].
// 12/13/02 Shahed Molla Modeified MAXCOMSTRINGS from 23 to 29 to have support for receiving C_MSGx_LDs
// 04/07/03 U.Dasgupta   Added coData_t structure for tuning CO-based parameters
// 04/23/03 Shahed Molla Modeified MAXCOMSTRINGS from 29 to 35 to have support for new ADSL2 messages.
// 06/05/03 U.Dasgupta   Added constant,UP_FIRSTBIN_TONE28,to support lower US tones for ISDN - Same as AR5
// 06/05/03 U.Dasgupta   Reduced DOWN_FIRSTBIN to 52 for ISDN.
// 06/18/03 A. Redfern   Reworked spectral shaping.
// 06/20/03 U. Iyer      Modified MAXCOMSTRINGS from 35 to 39 to have support for new ADSL2 messages (CMSG2 thru RPARAMS).
// 09/24/03 Z. Yang      Removed the constant REVERBSHIFT, instead use AX7_SHIFT_ADJUST as the scaler pass to the
//                       reverbestimation, so that the reverb_buff will have the same scale as fft output buffer.
// 09/24/03 J. Bergsagel Moved DI_STATE_latencyBuff_t to this file.
//                       Moved related "LATBUFSIZE" defines here also.
// 10/13/03 K. Nagaraj   Defined TEQ_LENGTH_ADSL2P and CREVERB_ENC_WORDS_PERFRAME_ADSL2P for ADSL2+ mode.
// 10/14/03 Sumeer B     Added Overhead Structure (DI_OVHD_Struct_t) in DI_STATE_latencyBuff_t.
// 02/20/04 Sumeer B     added pmd parameters to DI latency structure
// 05/04/04 Sameer V     Increased MAXCOMSTRINGS to 44.
// 06/16/04 S.Yim        Added support for S=1/4
// 10/06/04 Mallesh      Added definition for s as supported by the standard.
// 06/08/04 Jack Huang   Defined a different tone level for T1413 mode. It was found that our T1413 XMT tone level
//                       was below the defined PSD (-38 dBm/Hz). It is partially due to sharp filter characteristic around the
//                       T1413 tone. The level is now increased by 6 dB to compensate for that.
// 02/23/05 Yi Han       Removed token T1413_TONE_LEVEL_6DB_HIGHER
// 03/30/05 Venkat R     Removed DMT_BIS (Set to 1) Token
// 05/19/05 Venkat R     Add new defines for handling upstream bins in a consistent manner
// 06/19/05 Venkat R     CQ9600: Annex A/M Dynamic Configurability & set DOWN_PILOTBIN for COMB
// 07/12/05 Tim B        CQ9769: ADSL2+ DELT Support
// 07/20/05 Venkat R     CQ9600: Make changes according to review comments
// 05/12/05 Mike Locke   stringtable token enum moved to ctrl_msm_decl.h and generated
//                       by the state table compiler.  CQ9583
// 09/28/05 Kapil Gulati CQ10039: Moved some definitons from ctrl_init.h. Changed the max
//                       DS depth to 511 as per the new G.bis optional values.
// 10/20/05 Madhu H      CQ10008: Defined constants for Full bandwidth Reverb and Optional Medley signal for Annex B
//                       Also, Define constant for loop length upto which the optional medley stop bin
//                       is used against INFN.
// 01/18/06 Madhu H      CQ10186: Changed DOWN_FIRSTBIN values for Annex A and M. It is assumed we don't
//                       support pseudo-FDD mode currently.
// 02/08/06 Gerald Peter CQ10248: Removed ovhd_bis_pmd.h due to internal interface compile errors
// UR8_MERGE_START
// 06/06/06 KCCHEN       CQ10649: When the US K > 128bytes, the memory adjecent to LatencyBuff_U[] will be 
//                       overwritten/corrupted. To avoid this we need to increase the size of LatencyBuff_U[] 
//                       to accomodate K = 256 Bytes.
// UR8_MERGE_END
//UR8_MERGE_START CQ10781 Hao Ting
//07/22 Hao-Ting          Globalspan G.dmt mode will measure our transmitting power in GHS tones to decide power cutback
//                        value. However, our GHS tones is 1.5dB higher than standard defined. It will cause low DS rate
//                        at high noise environment like FB.            
//UR8_MERGE_END CQ10781 Hao Ting
// UR8_MERGE_START_END   CQ10566 MB
// 05/11/06 Mark Bryan   CQ10566 Added cabinet mode tone set A43c/B43c defintions for downstream.
// 02/28/06 Kapil Gulati CQ9977: Added a #define for maximum deinterleave memory size. // UR8_MERGE_START_END DEINTERLEAVER_CYCLE_OPTIMIZATION Craig Leeds
// UR8_MERGE_START CQ11021 Hao-Ting Lin
//                       Add define used in COMLAB fix
// UR8_MERGE_END CQ11021 Hao-Ting Lin
// UR8_MERGE_START monitored_tones MB
// 6/20/06 Mark Bryan      CQ11745: Added defintion's CONSTSHIFT, CONSTPOINT, and structure FeqStr used in the data and synch symbol FEQ's.
// UR8_MERGE_END monitored_tones
// *******************************************************************************************


#include "ctrl_main.h"
#include "ovhd_bis_main.h"
// #include "ovhd_bis_pmd.h" (CQ:10248)

#if SERVICE_ISDN_ANNEXB

#define GHS_UPCARRIER0                       37
#define GHS_UPCARRIER1                       45
#define GHS_UPCARRIER2                       53

#define GHS_DOWNCARRIER0                     72
#define GHS_DOWNCARRIER1                     88
#define GHS_DOWNCARRIER2                     96

#else

// tone set A43 - upstream
#define GHS_UPCARRIER0                       9
#define GHS_UPCARRIER1                       17
#define GHS_UPCARRIER2                       25

// tone set A43 - downstream
#define GHS_DOWNCARRIER0                     40
#define GHS_DOWNCARRIER1                     56
#define GHS_DOWNCARRIER2                     64

#endif

// tone set B43 - downstream
#define GHS_DOWNCARRIER3                     72
#define GHS_DOWNCARRIER4                     88
#define GHS_DOWNCARRIER5                     96

//UR8_MERGE_START CQ10566 MB
// tone set A43c/B43c for cabinet mode - downstream //CQ10566
#define GHS_DOWNCARRIER6                     257
#define GHS_DOWNCARRIER7                     293
#define GHS_DOWNCARRIER8                     337
//UR8_MERGE_END CQ10566

// This is exclusively used in ADSL1 and can be changed to US_NYQ_TONES_ANNEXA
//  Not doing this due to lack of time and need to change so many files
#define UPSTREAMBINS                         32

//CQ9600: New #defines for AnnexBM
// Here Nyquist Freq is the minimum required sampling frequency for Annex A/B
#define US_NYQ_TONES_ANNEXA                  32
#define US_NYQ_TONES_ANNEXB                  64
#define US_NYQ_TONES_ANNEXM                  64
#define US_NYQ_TONES_ANNEXI                  64 // Not used currently
#define US_NYQ_TONES_ANNEXJ                  64

#define MAX_US_NYQ_TONES                     US_NYQ_TONES_ANNEXM

// IFFT multiple of Nyquist required.
#define MAX_US_IFFT_NYQ_FACTOR               2

// Indicates that it is a 2X IFFT for whatever is the Nyquist frequency
// We could have the Max Factor changed to 4x, however in some places we need to use
// only 2X
#define US_2X_IFFT_NYQ_FACTOR                2

#define US_FIRSTBIN_ANNEXA                   7
#define US_FIRSTBIN_ANNEXM                   7
#define US_FIRSTBIN_ANNEXI                   0 // Not Used currently
#define US_FIRSTBIN_ANNEXB                   28
#define US_FIRSTBIN_ANNEXB_DTAGUR2           33

#define DIM_PER_CONS                         2

#define US_OUTOFBAND_TONES_OFFSET            14
#define US_FIRST_OUTOFBAND_BIN_ANNEXA        (US_NYQ_TONES_ANNEXA + US_OUTOFBAND_TONES_OFFSET)
#define US_FIRST_OUTOFBAND_BIN_ANNEXM        (US_NYQ_TONES_ANNEXM + US_OUTOFBAND_TONES_OFFSET)
#define GAINSCALE_BUFSIZE                    DIM_PER_CONS*MAX_US_IFFT_NYQ_FACTOR*MAX_US_NYQ_TONES


#define MAX_US_NET_DATA_RATE                 3200000 // define it as multiple of 32 kbps as it is used for allocation

#if SERVICE_ISDN_ANNEXB
#define ADSL1_US_NYQ_TONES                   US_NYQ_TONES_ANNEXB
#else
#define ADSL1_US_NYQ_TONES                   US_NYQ_TONES_ANNEXA
#endif

#if SERVICE_ISDN_ANNEXB
#define SERVICE_UPSTREAMBINS                 US_NYQ_TONES_ANNEXB
#else
#define SERVICE_UPSTREAMBINS                 US_NYQ_TONES_ANNEXA
#endif

#define LOGUPSTREAMSAMPS                     6

// Created a more representative name. This would mean for 64 tones ,
// it would return 6. 64 is the MAX_US_TONES used. If MAX_US_NYQ_TONES
// changes, we will have to change this number also
#define LOG_MAX_NSCUS_BASE2                  6

#define UPSTREAMSAMPS                        (UPSTREAMBINS*2)

// 1X IFFT Upstream Sample Length : Although both the values are same
// a cleanup of this might have to be done, when doubleTxRate is cleanedup
// and it might necessitate separate definitions for Annex A & M
#define GHS_UPSTREAMSAMPS                    (US_NYQ_TONES_ANNEXA*2)

#define UPSTREAMCP                           4 // Definition is for 32 upstream tones with 1X IFFT

#define UPDOWN_RATIO                         8

// Unlike DOWN_FIRSTBIN, this should only apply to MEDLEY


#if SERVICE_ISDN_ANNEXB
#define UP_FIRSTBIN                          32
#define UP_FIRSTBIN_TONE28                   28
#define UP_LASTBIN                           59

//CQ 10008 Defined constants for Full Bandwidth Reverb and Optional Medley signal.
//Full bandwidth reverb is sent in transceiver training and last bin of medley signal is
//changed to 56 in short loops to limit transmitter noise floor
#define UP_REVFULLFIRSTBIN                   18
#define UP_REVFULLLASTBIN                    63
#define UP_MEDOPTLASTBIN                     56

#elif SERVICE_ISDN_PROP
#define UP_FIRSTBIN                          1
#define UP_LASTBIN                           26


#elif SERVICE_POTS
#define UP_FIRSTBIN                          US_FIRSTBIN_ANNEXA
#define UP_LASTBIN                           31
#endif
#define UP_LASTBIN_ANNEXM                    59 // Also to be used for Annex J

#define DOWNSTREAMBINS                       (UPSTREAMBINS*UPDOWN_RATIO)
//UDG - Making RCVBINS=DOWNSTREAMBINS, even for GLITE
#define RCVBINS                              DOWNSTREAMBINS

#define MAX_RREVERB_ENC_WORDS_PERFRAME       (MAX_US_NYQ_TONES*2)/32

// All three definitions are for Annex A.
#define RREVERB_ENC_WORDS_PERFRAME           (US_NYQ_TONES_ANNEXA*2)/US_NYQ_TONES_ANNEXA
#define CREVERB_ENC_WORDS_PERFRAME_ADSL2P    ((DOWNSTREAMBINS*2)*2)/US_NYQ_TONES_ANNEXA
#define CREVERB_ENC_WORDS_PERFRAME           (DOWNSTREAMBINS*2)/US_NYQ_TONES_ANNEXA

#define LOGDOWNSTREAMBINS                    8
#define LOGDOWNSTREAMSAMPS                   9
#define DOWNSTREAMSAMPS                      (DOWNSTREAMBINS*2)
#define DOWNSTREAMCP                         (UPSTREAMCP*UPDOWN_RATIO)
#define DOWNSTREAMTOT                        (DOWNSTREAMSAMPS+DOWNSTREAMCP)


#if SERVICE_ISDN_ANNEXB
#define DOWN_FIRSTBIN                        52
#else
// This should be 20 for pseudo FDD configurations
// 33 or more for FDD configurations, or 8 for full ec
// All values require modifying AGCTARGET_POWER in mi_rcv3_c.c

//CQ 10186 Enabling Annex B changes for Annex M. With DOWN_FIRSTBIN set to 20, we might have real time issues
//in Showtime. So changed to 32 as currently AR7 only supports FDD mode.
#define DOWN_FIRSTBIN                        32
#endif

#define DOWN_LASTBIN                         255
//CQ 10186 Changed DOWN_FIRSTBIN to 64 for Annex M
#define DOWN_FIRSTBIN_ANNEXM                 64 // Also to be used for Annex J


#define ALIGN_BUF_BKSIZE                     10
#define ALIGN_BUF_MASK                       0X3FF


#define IFFT_INBUFFERSIZE                    (MAX_US_IFFT_NYQ_FACTOR*MAX_US_NYQ_TONES)

#define MAXCOMSTRINGS                        53


#define CES_LENGTH                           512
// Must be (8*n)-1
#define TEQ_LENGTH                           31
#define TEQ_LENGTH_ADSL2P                    63
#define PREC_LENGTH                          32
#define PREC_BUF_SIZE                        (4*UPSTREAMSAMPS)
#define PREC_BUF_BKSIZE                      8
#define PREC_BUF_MASK                        (PREC_BUF_SIZE-1)

// Values to control the upstream upsampling operation on the ATUR
#define PREC_OFF                             -1
#define PREC_ONLY                            0
#define PREC_ON                              1


#if SERVICE_ISDN_ANNEXB
#define DOWN_PILOTBIN                        96

#elif SERVICE_ISDN_PROP
#define DOWN_PILOTBIN                        80

#elif SERVICE_POTS
#define DOWN_PILOTBIN                        64
#endif

#define DOWN_PILOTBIN_ANNEXM                 96 // Could be used only in ADSL2 Mode Annex B/M during G.hs

#define ADSL2_DOWN_PILOTBIN_COMB_ANNEXBM     95 // This is the pilot used during COMB in ADSL2 Annex B/M
                                                // as Annex B COMB signal does not contain 96

#if SERVICE_ISDN_ANNEXB
// The AnnexB standard does not support a pilot in the US. The way our code is written
// setting the pilot to bin 64 will give us the desired result with minimum changes to the
// code. There still are other changes made to the code elsewhere but they should be minimal.
#define UP_PILOTBIN                          63
#else
#define UP_PILOTBIN                          16
#endif

// Constants associated with signal scaling.  Defined here since both
// SSM and MI functions need these.

// Value adjusted until clipscale can be fixed.
// Should be 0x4000
// Use when no clipscaling is desired

#define DEFAULTSCALE                         0x4000
#define DEFAULTSHIFT                         3

// Clipscale constants for use with the rescaling of the FDEC data
// that is performed in CES.  The difference here is based on the
// different IFFT lengths.
#define DEFAULTCESSHIFT                      2

#define DEFAULTCESSCALE                      0x4000

// Clipscale constants for use with the UPSTREAM data
#define UPDEFAULTSCALE                       0x4000
#define UPDEFAULTSHIFT                       0



// Scale factor so that reverb goes into FDEC at a reasonable level.
// Since the factor for XMT_TONE_HIGHLEVEL is 3dB below the peak for
// some constellations, another 3dB could be added for fine gain adjust,
// and another 6 dB is added for sin(x)/x compensation; we need to be
// able to increase that value by 12dB and then shift it so that it
// fits into a 16bit value.  0x10585e4 is the peak expected constellation
// data, so a right shift of 10 bits should work for the FDEC input.
// Adjusted up by 3 db.  sin(x)/x not implemented.  3 dB error since
// stimulating re and im in bin gives 3dB more signal

// The ATUR echo canceler requires that this be larger than 0x121a

#define XMT_PRNS_FREQSCALE                   0x1600

#define SQRT2_XMT_PRNS_FREQSCALE             0x1F1D //sqrt(2)*XMT_PRNS_FREQSCALE

// 0x1600 0x171d

// Max FFT output corresponds to +53dBm.  Tone high level power is
// supposed to be -4dBm average, or -1dBm peak.  54dB below 0x7fffffff
// is 4284793 = 0x416179
// There is a gain of 2 in the FFT


#define XMT_TONE_HIGHLEVEL                   (362*XMT_PRNS_FREQSCALE)
#define XMT_TONE_HIGHLEVEL_T1413             (XMT_TONE_HIGHLEVEL)
//UR8_MERGE_START CQ10781 Hao Ting
#define LOWER_XMT_TONE_HIGHLEVEL             (305*XMT_PRNS_FREQSCALE)//-1.5dB to XMT_TONE_HIGHLEVEL
//UR8_MERGE_END CQ10781 Hao Ting


// Low tone level is 20 dB lower
#define XMT_TONE_LOWLEVEL                    (XMT_TONE_HIGHLEVEL/10)
#define XMT_TONE_LOWLEVEL_T1413              (XMT_TONE_HIGHLEVEL_T1413/10)

//UR8_MERGE_END CQ11021 Hao-Ting Lin
//                       We got long sync time problem against Nokia D50e DMT8B3 DSLAM at 3500m 
//                       loop with TR-067 B2.4.2.1 FB noise applied to the CO side.
//                       issue: If we increase our GHS tone power by 0.7dB. The link up sucessful rate
//                              would increase to 80%.
//                       solution: Work around. Use API bit to control fix. If API bit is on, increase 
//                                 GHS tone power.
#define XMT_TONE_HIGHLEVEL_400               (400*XMT_PRNS_FREQSCALE)
//UR8_MERGE_END CQ11021 Hao-Ting Lin

// Adjusted down by 1 so that the above adjustment is effectively
// -3dB
#define XMT_FDEC2IFFT_SHIFT                  8

// A power of 1 was applied to each bin and there are 256 bins, so
// that the IDFT rms is 16 times that, or 16.  We want the rms to be
// 3dB below 0x4161790, which corresponds to the peak amplitude of
// a +20dBm sinusoid.  3dB below 0x4161790 is 0x2D3B2E2 and 1/16th
// of that is 0x2D3B2E.  Since the par is limited to 15dB, we will
// assume that it is not more than a factor of 8 at this point.
// This means that we need a shift of 10 bits to avoid overflow in
// a 16 bit representation.  This first number is only used by
// the table generation program

// Changed to XMT_PRNS_FREQSCALE/32
//#define XMT_PRNS_TIMESCALE ((XMT_PRNS_FREQSCALE)/32) // 184.90625

#define XMT_PRNS_TIMESHIFT                   13


// Right value for TEQ training
#define RCV_PRNS_FREQSCALE                   0x4000
#define SQRT2_RCV_PRNS_FREQSCALE             0x5A82 //sqrt(2)*RCV_PRNS_FREQSCALE

// Number of final right shifts in TEQ
#define TEQSHIFT                             9

// Minimum number of shifts to prevent overflow inchid
#define CHID_SHIFT                           11
#define LOG_SNR_ITERS                        4
#define SNR_ITERS                            16
#define SNRCALC_TIME                         16  // DOWNSTREAMBINS/SNR_ITERS
// Training time for ChMeas, must be a power of 2
#define CHMEAS_TIME                          4096
#define LOG_CHMEAS_TIME                      12

// Stuff to control Rcv_Reverb_U

#define REVERB_THRESH                        1

#define REVERB_SKIPBIN                       4
//#define REVERBSHIFT 10

// path labels
#define PHY_PATH_1                           0
#define PHY_PATH_2                           1

// Prashant S : For buffer size calculations, please refer AR7 TX-RX chain design document.
// UR8_MERGE_START CQ10649 KCCHEN
// CQ10649- When the US K > 128bytes, the memory adjecent to LatencyBuff_U[] will be 
//          overwritten/corrupted. To avoid this we need to increase the size of LatencyBuff_U[] 
//          to accomodate K = 256 Bytes.
#define TX_LATBUFSIZEPLUS                    516
// UR8_MERGE_END CQ10649 KCCHEN

#define TX_LATBUFSIZE                        TX_LATBUFSIZEPLUS
#if DS_LOOP_BACK
#define RX_LATBUFSIZE                        6024
#else
#define MAX_SP_FACTOR                        4
#define MAX_SP_FACTOR_STD                    3
#define MAX_CODEWORD_SIZE                    256
#define RX_LATBUFSIZE                        ((MAX_SP_FACTOR+1)*MAX_CODEWORD_SIZE)
#endif


// Interleave and deinterleave memory constants
#define N_MAX                                255

#if US_LOOP_BACK
#define D_MAX_DS                             16
#elif DS_LOOP_BACK
#define D_MAX_DS                             16
#else
#define D_MAX_DS                             511
#endif
#define D_MAX_US                             16

#define D_MAX_DS_MANDATORY                   64

// Number of Dp values supported in DS. Currently the standard defines
// 21 values.
#define NUM_Dp_MANDATORY                      7
#define NUM_Dp_OPTIONAL                      14
#define NUM_Dp_SUPPORTED                     (NUM_Dp_MANDATORY+NUM_Dp_OPTIONAL)

// Max value of (NFECp-1)(Dp-1) allowed by standard.
#define MAX_NFECp_Dp_PRODUCT                 16002


// UR8_MERGE_START DEINTERLEAVER_CYCLE_OPTIMIZATION Craig Leeds
// 02/28/06 Kapil Gulati CQ9977: Added a #define for maximum deinterleave memory size. 
// Memory used by the deinterleaver. As of now G.bis standard specifies
// that (NFECp-1)*(Dp-1) <= 16002. Po's algorithm theoritically uses
// ((NFECp-1)*(Dp-1)+2)/2 = 16004/2 = 8002. However the current
// implementation needs upto Dp extra bytes. Hence add that and
// round it up to the next multiple of 4 bytes.
#define MAX_DINTLV_MEM (((8002+D_MAX_DS+4)/4)*4)
// UR8_MERGE_END DEINTERLEAVER_CYCLE_OPTIMIZATION Craig Leeds


//CQ 10008. It is found that Annex B DS performance gets degraded if medley end tone is set beyond
// 56 in short loops, AWGN/no noise conditions against INFN. Define the constant that
// decides the loop length upto which optional medley stop bin (56) is used for above
// mentioned cases.
// The US Medley stop bin cannot be reduced to 56 against BRCM as we do not meet WT-100
// spec for US performance
#define LOOP_LENGTH_5800_OPTIONAL_MEDLEY 5800
#define LOOP_LENGTH_3800_OPTIONAL_MEDLEY 3800

typedef struct {
        unsigned char   shift_amount;
        unsigned char   nbits;
        short           multiplier;
} GainType;

// Structure used to hold the timing recovery scaling factor
// It goes here because both SSM and MI use it

typedef struct {
  int exp;
  int mant;
  } scaleType;

// ML CQ9583 remove stringtable enum

// UR8_MERGE_START monitored_tones MB
//CQ11745
#define CONSTSHIFT 14
#define CONSTPOINT (1<<CONSTSHIFT)

#pragma STRUCT_ALIGN (FeqStr, 4)
typedef struct {
  UINT8 synchmu1;
  UINT8 synchmu2;
  UINT8 alpha_shift1;
  UINT8 alpha_shift2;
} FeqStr;
// UR8_MERGE_END monitored_tones



#endif //__CTRL_MSM_SWIF_H__
