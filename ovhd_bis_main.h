#ifndef __OVHD_BIS_MAIN_H__
#define __OVHD_BIS_MAIN_H__ 1

// Copyright Texas Instruments Inc, 1999 All rights reserved
// ******************************************************************************************
// ovhd_bis_main.h
// This file contains the functions definations (for public use) related to the Overhead
// channel for G.992.3
//
// History
// 05/02/03     Sumeer Bhatara   written
// 10/08/03     Sumeer Bhatara   Added tx_framebuf in L1 Buffer stucture definition.
// 10/10/03     Sumeer Bhatara   Added L1 rx_framebuf read and write indices.
// 05/07/04     Mallesh          Added one buffer to transfer recieved byte after deleting escape
//                               character. Also added another token to indicate avaialbility of messages.
// 03/30/05     Venkat R         Removed DMT_BIS (Set to 1) Token
// 01/19/06     CPH              CQ9885, Added ovhd_cnxt_toggle_bit in DI_OVHD_xmtFrameDef_t
// UR8_MERGE_START_END SRA Tim Bornemisza
// 03/07/06     Tim Bornemisza   CQ10351: Add defines and variables for segmentation support
// UR8_MERGE_START_END L2PM Tim
// 08/31/06     Tim Bornemisza   CQ10756: Reduced our maximum segment size to correct for a bug on Infineon COs
// UR8_MERGE_START CQ11007  KCCHEN
// 09/26/06 KCCHEN       CQ11007 : US SNR margin update
// UR8_MERGE_END CQ11007 KCCHEN
// ******************************************************************************************

#include "env_def_typedefs.h"

//L1 buffers of size 32 octets. This needs to be atleast 16 keep this as power of 2 number.
#define OVHD_RXBUF_SIZE    32
#define OVHD_TXBUF_SIZE    32

#define TX_RDWR_LAG        16
#define RX_RDWR_LAG        0
#define RX_PROCESS_DELAY   4

//L2 Status Flag definitions
#define XMT_MSG_ABORT      0x01   //bit0
#define XMT_MSG_COMPLETE   0x02   //bit1
#define XMT_MSG_AVAILABLE  0x04   //bit2

#define RCV_L2BUF_OVERFLOW 0x10   //bit4
#define RCV_MSG_AVAILABLE  0x20   //bit5
#define RCV_FCS_ERROR      0x40   //bit6
#define RCV_VALID_FRAME    0x80   //bit7
/************Definition of bit7:6 of L2 Status Flag**************************/
//bit7:6
//   0 0      Rx in process, count for timeout
//   0 1      Rx FCS error , response invalid parameters
//   1 0      Rx valid frame, apply bitswap
//   1 1      Never
//L2 module is responsible for clearing the bit after using

//rcvStatusFlag definitions
#define RCV_STATUS_MSG_AVAILABLE_BIT 0x01
#define RCV_STATUS_MSG_WRITTEN_BIT   0x02
#define RCV_STATUS_MSG_PREVBYTE_FLAG 0x04

//xmtStatusFlag definitions
#define XMT_STATUS_FLAG_BIT    0x01
#define XMT_STATUS_ESC_BIT     0x02

#define ADDRESS_INDEX 0
#define CONTROL_INDEX 1
#define CMD_INDEX     2
#define SUBCMD_INDEX  3

#define START_TX_CORRUPT_CRC_FLAG 0x01
#define END_TX_CORRUPT_CRC_FLAG   0x02
#define START_RX_CORRUPT_CRC_FLAG 0x04
#define END_RX_CORRUPT_CRC_FLAG   0x08


// UR8_MERGE_START SRA Tim Bornemisza
//CQ10351: The maximum length of each segment in the message
//it may be a good idea to shrink this at some point
// UR8_MERGE_START L2PM Tim
//CQ10756: Infineon miscalculates the segment size so we have to reduce to 1020 for interop
#define MAX_OVHD_SEGMENT_LENGTH  1020
// UR8_MERGE_END L2PM Tim
#define LOG2_MAX_OVHD_SEGMENT_LENGTH  10

//CQ10351: Number of bytes to copy starting from message index 0 at the
//beginning of each segment
#define OVHD_SEGMENT_HEADER_LENGTH 4

//CQ10351: Amount of time to wait for a segment acknowledge message
#define OVHD_SEGMENT_RESPONSE_TIMEOUT 47    //in superframes, ~800 ms
// UR8_MERGE_END SRA Tim Bornemisza

enum {
  HIGH_PRIORITY,
  NORMAL_PRIORITY,
  LOW_PRIORITY
};

// ----------------------------
// Begin L1 Buffer definition
// ----------------------------
//
//L1 Buffer Struct Type
typedef struct {
  UINT16         TxBufRdInx;                       //read index of L1 tx buffer
  UINT16         TxBufWrInx;                       //write index of L1 tx buffer
  UINT16         RxBufRdInx;                       //read index of L1 rx buffer
  UINT16         RxBufWrInx;                       //write index of L1 rx buffer
  UINT16         deframebufWrIx;                   //write index of L1 rx buffer
  UINT16         deframebufRdIx;                   //write index of L1 rx buffer
  UINT8          tx_framebuf[OVHD_TXBUF_SIZE];     //transmit L1 buffer
  UINT8          rx_framebuf[OVHD_RXBUF_SIZE];     //receive L1 buffer
  UINT8          deframebuf[OVHD_RXBUF_SIZE];      //receive L1 buffer
} DI_L1_OVHDBufDef_t;

// ----------------------------
// Begin L2 Buffer definition
// ----------------------------
//
//L2 Buffer Struct Type
typedef struct {
  UINT16  L2UBufInInx;              //L2 module write, OVHD module read only
  UINT16  L2UBufOutInx;             //OVHD module write, L2 module read only
  UINT16  L2DBufInInx;              //OVHD module write, L2 module read only
  UINT16  L2DBufOutInx;             //L2 module write, OVHD module read only
  UINT8   L2StatusFlag;             //rcv L2 status Flag
  UINT8  *L2UBuf;                   //L2 upstream buffer
  UINT8  *L2DBuf;                   //L2 downstream buffer
  UINT16  L2TxBufSize;              //L2  transmit buffer size
  UINT16  L2RxBufSize;              //L2  receive buffer size
} DI_L2_BufDef_t;

// L2StatusFlag definition
// | Bit 7     | Bit 6   | Bit 5       | Bit 4     | Bit 3 | Bit 2 | Bit 1      | Bit 0      |
// | Rcv Valid | Rcv FCS | New Message | L2 buffer |   0   |   0   | Tx Message | Tx Message |
// | Frame     | error   | available   | overflow  |       |       | complete   | aborted    |

typedef struct {
  UINT8             xmtstate;             //OVHD transmitter state
  UINT8             priority;             //priority of the message to be transmitted
  UINT8             xmtStatusFlag;        //xmt status flag
  UINT8             prevL2_moduleInx;     //index of last transmitted L2 module
  UINT16            xmtCRC;               //CRC of the transmitted message
  UINT16            xmtbytecount;         //index of the message that is to be transmitted
  // UR8_MERGE_START SRA Tim Bornemisza
  DI_L2_BufDef_t    *xmtL2_Buf_p;         //L2 transmit buffer
  UINT32             xmtsegmentcount;     //CQ10351 - number of bytes transmitted in current segment
  UINT16            xmtmsgsize;           //length of the message that is to be transmitted
  UINT8             ovhd_cnxt_toggle_bit;  //CQ9885, clearEoc fix
  UINT8             xmtsegment;           //CQ10351 - current segment number
  UINT8             xmttotalsegments;     //CQ10351 - total number of segments in the message
  UINT8             dummy[3];             //32 bit aligned
  // UR8_MERGE_END SRA Tim Bornemisza
} DI_OVHD_xmtFrameDef_t;

// xmtStatusFlag Definitions
// |Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1   | Bit 0    |
// |  0   |   0   |   0   |   0   |   0   |   0   | HDLC_ESC| HDLC_FLAG|
//

typedef struct {
  UINT8            escape_flag;          //escape flag, used in HDLC de-framing
  UINT8            rcvStatusFlag;        //rcv status flag
  UINT8            rcvstate;             //HDLC receiver state
  UINT8            rcvwritestate;        //write state fo teh overhead function
  UINT16           rcvCRC;               //CRC of the received message
  UINT16           rcvbytecount;         //number of bytes of the received message
  DI_L2_BufDef_t   *rcvL2_Buf_p;         //L2 receive buffer
} DI_OVHD_rcvFrameDef_t;

// rcvStatusFlag Definitions
// |Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2    | Bit 1      | Bit 0        |
// |  0   |   0   |   0   |   0   |   0   | Prev byte| Msg_Written| Msg_available|
// |      |       |       |       |       | was a    |            |              |
// |      |       |       |       |       | flag     |            |              |

typedef struct {
  DI_L1_OVHDBufDef_t    L1_OVHD_Buf_s;
  DI_OVHD_xmtFrameDef_t xmtFrameStatus_s;
  DI_OVHD_rcvFrameDef_t rcvFrameStatus_s;
} DI_OVHD_Struct_t;


void OVHD_BIS_XMT_Processing(void);

void OVHD_BIS_RCV_Processing(void);

void OVHD_init ();
// UR8_MERGE_START CQ11007 KCCHEN
// Address field value
// Message designator
enum
{
  //High Priority
  DESIG_OLR                               = 0x01,
  //Normal Priority
  DESIG_EOC                               = 0x41,
  DESIG_TIME                              = 0x42,
  DESIG_INVENTORY                         = 0x43,
  DESIG_CTRL_PARAMETER_READ               = 0x04,
  DESIG_MANAGEMENT_COUNTER_READ           = 0x05,
  DESIG_POWER_MANAGEMENT                  = 0x07,
  DESIG_CLEAR_EOC                         = 0x08,
  DESIG_NONSTANDARD_FACILITY              = 0x3f,
  //Low Priority
  DESIG_PMD_TEST_PARAMETER_READ           = 0x81,
  DESIG_NONSTANDARD_FACILITY_LOW_PRIORITY = 0x7f
};
#define US_SNRM_UPDATE_DBG 0 // Token for the US snr margin update debug
// UR8_MERGE_END CQ11007 KCCHEN
#endif // _OVHD_BIS_MAIN_H_

