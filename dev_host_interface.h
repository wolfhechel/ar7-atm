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
*  (C) Copyright Texas Instruments Inc. 2002.  All rights reserved.
*******************************************************************************/

#include "dev_host_verdef.h"

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
  HOST_GHS_CLEARDOWN   // G.hs - FOR INTERNAL USE ONLY to start cleardown
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
  DSP_TRAINING_MSGS       // DSP Message to indicate that host has to copy the training message specified in the tag field.
};

// Define different ADSL training modes.
//Defintions as per new host interface.
#define NO_MODE             0
#define GDMT_MODE           2
#define GLITE_MODE          4
#define ADSL2_MODE          8
#define ADSL2_DELT          (ADSL2_MODE+1)
#define ADSL2PLUS_MODE      16
#define ADSL2PLUS_DELT      (ADSL2PLUS_MODE+1)
#define READSL2_MODE        32
#define READSL2_DELT        (READSL2_MODE+1)
#define T1413_MODE          128
#define MULTI_MODE          255 // all possible bits are set in the bit field

// Define the reason for dropping the connection

enum
{
  REASON_LOS         = 0x01,
  REASON_DYING_GASP  = 0x02,
  REASON_USCRCERR    = 0x04,
  REASON_MARGIN_DROP = 0x08
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

typedef struct
{
  UINT8  trainMode;          // Train mode selected.  See training modes defined above.
  UINT8  bDummy1;            // dummy byte for explicit 32-bit alignment
  UINT16 lineLength;         // Contains loop length estimate.  Accuracy w/i 500 ft.  LSbit = 1 for straight loop, = 0 for bridge tap
  UINT32 atucVendorId;       // Pass the vendor id of the CO to the host
  UINT8  cMsgs1[8];          // CMsgs1 and CMSGPCB
  UINT16 adsl2DSRate;        //
  UINT8  cRates2;            //
  UINT8  rRates2;            //
  UINT8  rRates1[4][11];     //
  UINT8  cMsgs2[4];          //
  UINT8  cRates1[4][30];     //
  UINT8  rMsgs2[4];          //
  UINT16 adsl2USRate;        //
  UINT8  atucGhsRevisionNum; // Pass the G.hs Revision number of the CO to the host
  UINT8  reserved1;          //
  PUINT8 *adsl2DeltMsgs_p;   // This pointer to a pointer passes the address of the globalvar.pString, which is also
                             // a pointer list of pointers. It will be used to pass all the new ADSL2 DELT messages to
                             // host side. This is for ACT.
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

typedef struct
{
  UINT8   stdMode;              // Desired train mode.  See training modes defined above.
  UINT8   ghsSequence;          // Selected G.hs session as shown in Appendix 1
  UINT8   usPilotFlag;          // Value of 1 indicates transmit an upstream pilot on bin 16
  UINT8   bDummy1;              // dummy byte for 32-bit alignment
  UINT8   rMsgs1[38];           // RMSG-1(6) and RMSG_PCB (38)
  UINT8   bDummy2[2];           // dummy bytes for 32-bit alignment
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
  UINT8   reserved1;            // Dummy bytes
  UINT8   reserved2;            // Dummy bytes
  UINT8   reserved3;            // Dummy bytes
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
  UINT8  reserved1;         // for 32-bit alignment
  UINT8  reserved2;         // for 32-bit alignment
  UINT8  reserved3;         // for 32-bit alignment
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

#define DEV_HOST_PAGE_NUM 4   // number of overlay pages

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
  UINT8        dummy[2];                  // dummy bytes for 32-bit alignment
  UINT32       eocModemStatusReg;         // Dsp Write, status bits to host
  UINT8        lineAtten;                 // Dsp Write, line attenuation in 0.5 db step
  SINT8        dsMargin;                  // DSP Write, measured DS margin
  UINT8        aturConfig[30];            // Dsp Write, also used by EOC for ATUR Configuration
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
  SINT16 devCodecTxdf2bCoeff[64] ;            // (BOTH) FIR filter coefficients
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
  DEV_HOST_coData_t *  hostProfileBase_p;     // base address of profile list
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
  SINT16   rsvd;
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
  UINT8  reserved1;            // These are for 32-bit alignment.
  UINT8  reserved2;            // These are for 32-bit alignment.
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
} DEV_HOST_dspOamSharedInterface_t;


// ---------------------------------------------------------------------------------
// Typedef to be used for the pointer to the DEV_HOST_dspOamSharedInterface_s struct
//  of pointers (this is used in dev_host_interface.c).
// ---------------------------------------------------------------------------------
typedef DEV_HOST_dspOamSharedInterface_t *DEV_HOST_dspOamSharedInterfacePtr_t;

#endif
