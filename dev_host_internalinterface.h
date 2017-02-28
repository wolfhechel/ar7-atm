#ifndef _DEV_HOST_INTERNALINTERFACE_H_
#define _DEV_HOST_INTERNALINTERFACE_H_ 1

/************************************************************************************
*  FILE PURPOSE: Public header file for the Host-to-DSP "Internal-Use-Only" Interface
*************************************************************************************
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
*  FILE NAME: dev_host_internalinterface.h
*
*  DESCRIPTION:
*       This header file defines the variables and parameters used between the
*       host processor and the DSP for the "Internal-Use-Only" shared interface.
*       This file is included in both the DSP software and the host software
*       (if the host software is compiled for "Internal-Use-Only" operation.
*
*  RULES FOR MODIFICATION AND USE OF THIS FILE:
*
*  --The main pointer to the "Internal-Use-Only" struct of pointers will always
*      be at the same fixed location (0x80000004), for internal-use-only purposes.
*
*  --Each pointer element in the struct of pointers (indicated by the main pointer)
*      will always point to a struct only.
*
*  --Any new structures added to the internal host interface in subsequent versions
*      must each have a corresponding new pointer element added to the END of the
*      struct of pointers.  Other than this, there will never be any moving or
*      rearranging of the pointer elements in the struct of pointers.
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
*  HISTORY:
*
*  11/20/02  J. Bergsagel    Written from host_forinternaluseonly_r.h (the items
*                              that are for "Internal-Use-Only").
*  12/06/02  J. Bergsagel    Added dummy bytes to aocVar definition for 32-bit
*                              alignment.
*  12/16/02  J. Bergsagel    Resized and rearranged some struct members to save DMEM
*  01/06/03  J. Bergsagel    Removed obsolete MSE struct members.
*                            Added targetMarginAdjust to DEV_HOST_modemEnv_t.
*  01/21/03  Barnett         Implemented Ax7 UNIT-MODULE modular software framework.
*  01/28/03  Mallesh         removed DeintlvDMAErrCnt as DMA is no longer being used for deinterleaving
*  01/29/03  Sameer V        Added 64 bit variables to report ATMBERT bit count and bit error
*                            count to the host.
*  03/07/03  Sameer/Jonathan Put SWTC token around tcDmaErrCnt element in DEV_HOST_dspErrorCounts_t
*                            Put SWTC token around elements in DEV_HOST_atmDsBert_t that are for SWTC only
*  02/27/03  Mallesh         Pulled in debug_log changes and also put DEBUG_LOG token around them
*  03/05/03  Mallesh         Added AFE log for both the paths
*  03/12/03  Mannering       Changed size of atucId to 32 bits for Co Profiles
*  05/22/03  J. Bergsagel    Moved the following struct typedef's over to the public host interface:
*                              DEV_HOST_dspBitSwapDef_t
*                              DEV_HOST_atmDsBert_t
*  05/28/03  J. Bergsagel    Moved subStateIndex, externalBert and usBertPattern over to public interface.
*  06/09/03  U G Jani        Changed DEV_HOST_ghsDspTxBufDef_t and DEV_HOST_ghsDspRxBufDef_t sizes.
*  08/20/03  Jack Huang      Moved g.hs buffers into the public host interface
*                            DEV_HOST_ghsDspTxBufDef_t GHS_DSP_TX_Buf
*                            DEV_HOST_ghsDspRxBufDef_t GHS_DSP_RX_Buf
*  09/23/03  Mustafa T.      Added SNR and EOC enum's and DEV_HOST_teqFeqEc_t struct typedef.
*  09/23/03  J. Bergsagel    Changed doubleRxRate to doubleRxRate_p, since it is a pointer.
*  10/10/03  Mustafa T.      Added marginDelta_p and marginDeltaOffset_p pointers in DEV_HOST_teqFeqEc_t structure.
*  12/10/03  Jack Huang      Added array for G.hs VID. This is done for GSPN US rate work-around
*  10/04/04  Thomas Leyrer   Added real-time trace interface for DSLAGENT
*  07/01/05  Peter Hou       CQ9776 Added currCoProfCtrlAddr and currCoProfAddr in DEV_HOST_modemEnv_t
*  08/02/05  Peter Hou       CQ9776 (partial) TR69: Added ghsATUCVendorId_p
*  08/23/05  Peter Hou       CQ9885: Add clearEoc for ADSL2/2+ & ovhdControl.
*  11/03/05  Manjula K       CQ10037: Added bit definition for ovhdControl to support Inventory command.
*  1/4/06    Peter Hou       CQ9885: Fixed clearEoc CNXT interop problem.
*  01/24/06  Gerald Peter    Adapted RTT in terms of enabling by default (CQ10242)
* UR8_MERGE_START L2PM Tim
*  08/31/06  Tim Bornemisza  CQ10756: Added bit to turn off L2PM processing
* UR8_MERGE_END L2PM Tim
*  (C) Copyright Texas Instruments Inc. 2002.  All rights reserved.
*******************************************************************************/

// --------------------------------------------------------------------------------
// Address of the pointer to the DEV_HOST_dspOamInternalSharedInterface_s struct of
//   pointers.  This is where it all starts.
// --------------------------------------------------------------------------------
#define DEV_HOST_DSP_OAM_INTERNAL_POINTER_LOCATION 0x80000004
#define TOTAL_DEBUGLOG_MEM  0x040000

#include "phy_ec.h"

// -----------------------------
// Begin AOC Buffer definitions.
// -----------------------------

enum
{
  SNR_SINGLE_PATH,   // This message tag signifies to the host that there is only one SNR and its address is ready in DEV_HOST_snrBuffer_s.buffer1_p.
  SNR_DUAL_PATH,     // This message tag says that SNR for paht 1 and path 2 are ready in DEV_HOST_snrBuffer_s.buffer1_p and DEV_HOST_snrBuffer_s.buffer2_p, respectively.
  SNR_DUAL_COMBINED  // This message tag says that the address for combined SNR for path 1 and 2 are ready in DEV_HOST_snrBuffer_s.buffer1_p.
                     // Note that DEV_HOST_snrBuffer_t is defined in dev_host_interface.h
};

enum
{
  EOC_CPE,           // This message tag indicates that the EOC message that we are reporting is from CPE to CO.
  EOC_CO             // This message tag indicates that the EOC message that we are reporting is from CO to CPE.
};

// The value below can never be changed in order to keep the host intf. backwards compatible
#define DSPDP_AOCBUFSIZE 128

// This structure has been ordered to eliminate writes to odd bytes.
// All elements are DSP write.
typedef struct
{
  UINT8 aocUBufInInx;
  UINT8 aocUBufOutInx;
  UINT8 aocDBufOutInx;
  UINT8 aocDBufInInx;
  UINT8 aocUstrmBuf[DSPDP_AOCBUFSIZE];
  UINT8 aocDstrmBuf[DSPDP_AOCBUFSIZE];
} DEV_HOST_aocBufDef_t;


// ---------------------------
// Begin AOC Var. definitions.
// ---------------------------

// Struct to group the AOC var's together.
typedef struct
{
  volatile SINT32 bitSwapMsgCntUstrm[2]; // statistic for received bitswap messages BitSwapMsgCntUstrm[0]: normal bitswap
                                         //                                         BitSwapMsgCntUstrm[1]: extended bitswap
  volatile SINT32 bitSwapCntUstrm;       // statistic for performed bitswaps in upstream
  volatile SINT32 bitSwapMsgCntDstrm[2]; // statistic for sent bitswap messages BitSwapMsgCntDstrm[0]: normal bitswap
                                         //                                     BitSwapMsgCntDstrm[1]: extended bitswap
  volatile SINT32 bitSwapCntDstrm;       // statistic for performed bitswaps in downstream
  SINT16          settleCnt;             // superframe countdown to assure settle time between bitswaps
  SINT16          waitForAck;            // superframe counter used for timeout of sent bitswap requests
  SINT16          waitForPmAck;          // superframe counter can be used for timeout of sent profile management requests
  SINT16          aocDBufNewBytes;       // used in hostless AOC
  SINT16          freeSpace;             // number of free bytes in Upstreambuffer
  UINT8           aocDBufNewOutInx;      // used in hostless AOC
  UINT8           sendUtcFlag;           // indicates whether a UTC message has to be sent
  UINT8           CoProfileNumber;       // this variable contains the current profile number in CO context
  UINT8           sendPmAckFlag;         // after a profile request message has been rcv'ed, this flag will be set as a
                                         // reminder to send an ACK message
  UINT8           usBitSwapReqFlag;      // flag gets set to bitswap or ext bitswap when mse table indicates bitswap is necessary
  UINT8           dsBitSwapReqFlag;      // flag gets set after bitswap request message received
  UINT8           aocReset;              // this flag will indicate a retrain took place
  UINT8           dummy[3];              // dummy bytes for 32-bit alignment
} DEV_HOST_aocVarDef_t;


// ------------------------------
// Begin MSE results definitions.
// ------------------------------

// Struct to group the mse results together.
typedef struct
{
  UINT16 iMax;
  UINT8  bitSwapFlags;
  UINT8  dummy;
} DEV_HOST_mseResultDef_t;


// -----------------------------
// Begin EOC Buffer definitions.
// -----------------------------

// The two values below can never be changed in order to keep the host intf. backwards compatible
#define MAX_EOCTX_BUF  64
#define MAX_EOCRX_BUF  64

// This structure has been ordered to eliminate writes to odd bytes.
// All elements are DSP write.
typedef struct
{
  UINT8 eocUBufInInx;
  UINT8 eocUBufOutInx;
  UINT8 eocDBufOutInx;
  UINT8 eocDBufInInx;
  UINT8 eocUstrmBuf[MAX_EOCTX_BUF];
  UINT8 eocDstrmBuf[MAX_EOCRX_BUF];
} DEV_HOST_eocBufDef_t;


// -------------------------------
// Begin AGC settings definitions.
// -------------------------------

// Structure used to present AGC settings to host.
typedef struct
{
  UINT8 rxPga1;
  UINT8 rxPga2;
  UINT8 rxPga3;
  UINT8 anlgEq;
} DEV_HOST_agcSetting_t;


// -----------------------------
// Begin Cross Talk definitions.
// -----------------------------

// Structure used to present crosstalk parameters to host.
typedef struct
{
  UINT8 teqStartingPoint;
  UINT8 leakyTeqFlag;
  UINT8 rxHpfFlag;
  UINT8 dummy;
} DEV_HOST_crossTalkPara_t;


// -------------------------------
// Begin Error Counts definitions.
// -------------------------------

typedef struct
{
#if SWTC
  UINT32  tcDmaErrCnt;         // if (tc_dma_errcnt > 0), TC DMA did not complete in time
#endif
  PUINT32 realTimeErrors_p;    // if (realTimeErrors > 0), DSP has blown real-time
  PUINT32 rsNotDoneErrCnt_p;   // if (rs_not_done_error > 0), RS decoding did not complete in time
  PUINT32 fecUncorrectErr_p;   // RS uncorrectable error cnt
  PUINT32 fecUnderflowErr_p;   // RS DMA underflow error cnt
  PUINT32 allOptionsFailCnt_p; // if (Down_option == 0x00) || (Up_option == 0x00), all options failed
  PUINT32 swdiTxErr_p;         // SWDI Tx parameter errors, SWDIOK=0x0, BAD_K=0x1, BAD_R=0x2, BAD_S=0x4
  PUINT32 swdiRxErr_p;         // SWDI Rx parameter errors, SWDIOK=0x0, BAD_K=0x1, BAD_R=0x2, BAD_S=0x4
} DEV_HOST_dspErrorCounts_t;


// ------------------------------------
// Begin Modem Environment definitions.
// ------------------------------------
//
// L1 Capture is used to capture L1 Overhead channel TX/RX raw data.
// The captured result are stored in external SDRAM and can be
// retrieved by DHALAPP dbg command @l1cap.
// Do not reuse this structure for other purpose.
//
#if  OVHD_L1CAPTURE
typedef struct
{
  UINT8           *L1CapRxBuf;         // L1 Rx Capture buffer address, set by DHALAPP.
  UINT32          L1CapBufSize;        // L1 Catpture Buffer Size, set by DHALAPP.
  UINT32          L1CapRxBufIdx;       // Current Rx capture buffer index
  UINT32          L1RxLagMax;          // Record the Rx Lag Max value so far

  UINT8           *L1CapTxBuf;         // L1 Tx Capture buffer Address, set by DHALAPP
  UINT32          L1CapTxBufIdx;       // Current Tx capture buffer index
  UINT32          L1TxLagMax;          // Record the Tx Lag Max value so far

} DEV_HOST_L1Capture_t;
#endif


typedef struct
{
  volatile UINT32 debugHalt;          // Only used if concurrent debugging between CCS and atmdiag -j(tag)
  PSINT16         snr_p;              // pointer to snr table
  PTR_complex_int channelId_p;        // pointer to channel id
  PSINT32         doubleXmtRate_p;    // Pointer to modem[0].double_xmt_rate
  UINT32          atucId;             // bit 0: TI/bit 1: Acaltel/bit 2: GPSN/bit 3: ADI
  UINT8           testMode;           // DSP in test mode, DDI data will be bad
  UINT8           txSwitchLogic;      // Set to "1" to enable 32/64 auto detect
  UINT8           targetMarginAdjust; // DS target margin adjustment flag
  UINT8           reserved1;          // dummy byte for 32-bit alignment
  PSINT32         doubleRxRate_p;     // This flag indicates how many tones are used in the DS direction.
  UINT8           ghsVendorId[8];     // G.hs Vendor ID
  UINT32          currCoProfCtrlAddr; // Address of Co Profile Control currently in use in DSP
  UINT32          currCoProfAddr;     // Address of Co Profile currently in use in DSP
  UINT32          ghsATUCVendorIdAddr;  // Address of IMSG_HS_rxIdField_s.vendorID
  UINT32          ovhdControl;        // enable/control overhead function
  UINT8           ProgCode[4];          // Progress code
  UINT32          TestParmHlogfMsgAddr; // Address of TestParmHlogfMsg[]
  UINT32          TestParmQLNfMsgAddr;  // Address of TestParmQLNfMsg[]
  UINT32          TestParmSNRfMsgAddr;  // Address of TestParmSNRfMsg[]
  UINT32          recvMsgAddr;          // Address of recv_msg, type of DDI_RX_MSG
  UINT32          xmtMsgAddr;           // Address of xmt_msg, type of DDI_TX_MSG
  UINT32          L1CapCtrlAddr;
} DEV_HOST_modemEnv_t;

// bit definition for ovhdControl
#define OVHDCNTL_DISABLE_AOC        0x0001
#define OVHDCNTL_DISABLE_EOC        0x0002
#define OVHDCNTL_DISABLE_BISXMT     0x0004
#define OVHDCNTL_DISABLE_BISRCV     0x0008
#define OVHDCNTL_DISABLE_BISOLR     0x0010
#define OVHDCNTL_DISABLE_BISCLREOC  0x0020
#define OVHDCNTL_DISABLE_BISEOC     0x0040
#define OVHDCNTL_DISABLE_BISMGMT    0x0080
#define OVHDCNTL_DISABLE_BISTIME    0x0100
#define OVHDCNTL_DISABLE_BISPMD     0x0200
#define OVHDCNTL_DISABLE_BISINVNTRY 0x0400  //CQ10037
#define OVHDCNTL_DISABLE_SNRUPDATE  0x0800
// UR8_MERGE_START_END L2PM Tim
#define OVHDCNTL_DISABLE_BISL2PM    0x1000  //CQ10756




/* DEBUG_LOG token is enabled for debugging purposes only and it will be disabled for production code.
 * To prevent allocation of big chunk of memory ( which is allocated in SDRAM and we may not have excess
 * memory in production hardware) with production code load,
 * a small fake memory  is declared, to keep the host interface backword compatible. */
typedef struct
{
#if DEBUG_LOG
  UINT8  glbDebugLogArray[TOTAL_DEBUGLOG_MEM];
#else
  UINT32 glb_debuglog_fake_mem;         // fake memory for preserving pointer to debuglog in the host
#endif
} DEV_HOST_debugLog_t;



// Any data that needs to be dumped to MIPS memory space for debug should be defined as a
// struct as shown above. The pointer pointing to this struct needs to get included in DEV_HOST_debugDump_t.
typedef struct
{
  DEV_HOST_debugLog_t *debugLog_p;
} DEV_HOST_debugDump_t;

typedef struct
{
  PSINT16             teqCoeff_p;           // pointer to TEQ coeff's
  complex_int        *feqCoeff_p;           // pointer to FEQ coeff's
  PHY_EC_ecData_t    *ecData_p;             // Echo canceller data scructure.
  PSINT32             pilotSubChannel_p;    // pointer to pilot subchannel
  PSINT32             looptype_p;           // pointer to loop type
  PSINT16             marginDelta_p;        // pointer to retrain margin delta
  PSINT16             marginDeltaOffset_p;  // pointer to retrain margin delta offset
} DEV_HOST_teqFeqEc_t;

// All RTT information in LE for the DSP
typedef struct
{
 UINT16         supported;             // set by DSP
 UINT16         selected;              // set by DMS and agent
 UINT32         source_address;        // to be used with RTT_DATA_MEMEORY
 UINT32         dest_address;          // start address of host buffer set by host
 UINT32         start_time;            // when to start DMA within DMT cycle
 UINT16         size;                  // length of trace in integers, bins are samples
 UINT16         ntrace;                // number of traces before wrap around
 UINT16         trace_size;            // step size for next buffer
 UINT16         dma_error;             // dma did not complete before next one
 UINT16         current_buffer;        // current write buffer
 UINT16         dummy;
} DEV_HOST_RttType_t;

typedef DI_L2_BufDef_t DEV_HOST_L2clearEocDef_t;
typedef DI_L1_OVHDBufDef_t DEV_HOST_L1_OVHDBufDef_t;
// --------------------------------------------------------------------------------------
// Typedef to be used for the DEV_HOST_dspOamInternalSharedInterface_s struct of pointers
//  (this is used in dev_host_internalinterface.c).
// NOTE: This struct of pointers is NEVER to be referenced anywhere else in the DSP code.
// IMPORTANT: Only pointers to other structs go into this struct !!
// --------------------------------------------------------------------------------------
typedef struct
{
  DEV_HOST_aocVarDef_t             *aocVar_p;
  DEV_HOST_aocBufDef_t             *aocBuf_p;
  DEV_HOST_mseResultDef_t          *mseResult_p;
  DEV_HOST_eocBufDef_t             *eocBuf_p;
  DEV_HOST_agcSetting_t            *agcSetting_p;
  DEV_HOST_crossTalkPara_t         *crossTalkPara_p;
  DEV_HOST_dspErrorCounts_t        *dspErrorCounts_p;
  DEV_HOST_modemEnv_t              *modemEnv_p;
  DEV_HOST_debugDump_t             *debugDump_p;
  DEV_HOST_teqFeqEc_t              *teqFeqEc_p;
  DEV_HOST_RttType_t               *trace_p;
  DEV_HOST_L2clearEocDef_t         *L2clrEoc_p; //cph
  DEV_HOST_L1_OVHDBufDef_t         *L1Ovhd_p;   //cph
} DEV_HOST_dspOamInternalSharedInterface_t;


// -----------------------------------------------------------------------------------------
// Typedef to be used for the pointer to the DEV_HOST_dspOamInternalSharedInterface_s struct
//  of pointers (this is used in dev_host_internalinterface.c).
// -----------------------------------------------------------------------------------------
typedef DEV_HOST_dspOamInternalSharedInterface_t *DEV_HOST_dspOamInternalSharedInterfacePtr_t;

#endif
