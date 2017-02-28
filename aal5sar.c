
/**
 *  
 *  aal5sar.c
 *
 *  TNETDxxxx Software Support\n
 *  Copyright (c) 2002 Texas Instruments Incorporated. All Rights Reserved.
 *
 *  version
 *      28Feb02 Greg       1.00  Original Version created.\n 
 *      06Mar02 Greg       1.01  Documentation (Doxygen-style) enhanced
 *      06May02 Greg       1.02  AAL2 added
 *      06Jun02 Greg       1.03  Multiple API and bug fixes from emulation
 *      12Jul02 Greg       1.04  API Update
 */

/**
@defgroup CPHAL_Functions CPHAL Functions

These are the CPHAL Functions.
*/

/**
@page CPHAL_Implementation_Details

@section cphal_intro Introduction

The CPHAL API described above is generally applicable to all modules.  Any
implementation differences will be described in the following module-specific
appendix sections.  

Included for your reference is a diagram showing the internal architecture
of the CPHAL:

@image html SangamSoftware.jpg "HAL Architecture"
@image latex SangamSoftware.jpg "HAL Architecture" height=2.8in

*/

/**
@defgroup AAL5_Functions Additional Functions for AAL5 Implementation

These functions are used only by the AAL5 module.
*/

/*
@defgroup CPMAC_Functions Additional Functions for CPMAC Implementation

No additional functions currently defined.
*/

/**
@page VDMA_Implementation_Details

@section vdma_intro Introduction

The VDMA-VT module facilitates efficient transfer of data (especially voice)
between two devices, as shown in the figure below.

@image html vdma.jpg "VDMA System Block Diagram"
@image latex vdma.jpg "VDMA System Block Diagram" height=1in

The VDMA-VT module supports two modes of operation: mirror mode and credit mode.
Mirror mode is intended for systems in which the remote device does not have a
VDMA-based module.  Credit mode is intended for highest performance when VDMA-based
modules exist on both ends of an interface.

For more detailed information on the operation of the VDMA module, please
reference the VDMA Module Guide.

@section vdma_channels VDMA Channels

The VDMA-VT module is a single channel, single transmit queue device.  Therefore,
when using the CHANNEL_INFO structure, the correct value for @c Ch is always 0.
Correspondingly, the correct value for the @c Ch parameter in @c ChannelTeardown() is
always 0.  Further, when calling @c Send(), the driver should always supply the value
of 0 for both the @c Ch and @c Queue parameters.

For the VDMA-VT, configuring the channel requires the configuration of either 2 FIFO 
elements (in credit mode) or 4 FIFO elements (in mirror mode).  For credit mode, the
driver must configure just the local Tx and Rx FIFOs.  For mirror mode, the driver must
configure the Tx and Rx FIFOs for both the remote and local ends of the interface.

This channel configuration is accomplished through multiple calls to @c ChannelSetup().
Each call configures a single FIFO, according to the parameters in the CHANNEL_INFO
structure.  The members of VDMA-VT's CHANNEL_INFO structure are defined below.


- int RemFifoAddr; Address of remote FIFO (mirror mode only).  Set to 0 for credit mode.
- int FifoAddr;    Address of the local FIFO.  If 0, the CPHAL will allocate the FIFO.
- int FifoSize;    Size of the FIFO.
- int PollInt;     Polling interval for the FIFO.
- int Endianness;  Endianness of the FIFO.  If 1, big endian.  If 0, little endian.
- int RemAddr;     Used only in credit mode.  This is the base address of the remote
                   remote VDMA-based device (VDMA-VT or AAL2)
- int RemDevID;    Used only in credit mode.  Identifies the type of remote VDMA-based device.
                   0=VDMAVT, 1=AAL2 Ch0, 2=AAL2 Ch1, 3=AAL2 Ch2, 4= AAL2 Ch3.       

For the VDMA-VT module, the driver must make all calls to @c ChannelSetup() prior to calling
@c Open().  This is because several of the channel specific parameters may not be changed
while the VDMA-VT module is operational.

@section vdma_params VDMA Parameters

Defined here are the set of parameters for the VDMA-VT module.  Default settings for
each parameter should be represented in the device configuration file (options.conf).
During @c Init(), the CPHAL will reference the device configuration file and load all
default settings.  The @c Control() interface gives the driver an opportunity to
modify any default settings before the module becomes operational during the @c Open()
call.

@param NoTxIndication  If 1, the CPHAL will not call @c SendComplete(). 0 is default.
@param NoRxIndication  If 1, the CPHAL will not call @c Receive(). 0 is default.
@param RemoteCPU       If 1, the CPHAL will not directly manipulate data in FIFO's, leaving
                       that task for a remote CPU. 0 is default.
@param RxIntEn         If 1, enables Rx interrupts.  0 is default.
@param TxIntEn         If 1, enables Tx interrupts.  0 is default.
@param Mirror          If 1, enables mirror mode.  0 selects credit mode (default).
@param RxIntCtl        Valid only in mirror mode.  If 1, interrupts will occur when the Rx FIFO
                       RdIndex is updated.  If 0, interrupts occur when the Rx FIFO WrIndex
                       is updated.
@param TxIntCtl        Valid only in mirror mode.  If 1, interrupts will occur when the Rx FIFO
                       RdIndex is updated.  If 0, interrupts occur when the Rx FIFO WrIndex
                       is updated.
@param RBigEn          Remote big endian mode.  If 1, remote is big endian.
@param LBigEn          Local big endian mode.  If 1, local is big endian.

@section vdma_polling Using VDMA-VT without interrupts

If your system configuration does not utilize VDMA interrupts, the ability to process the
Tx and Rx FIFOs is supported.  To process the Tx FIFO, call @c CheckTx().  If the CPHAL is
able to process any complete data transmissions, it will call @c SendComplete() as usual.  
To process the Rx FIFO, call @c CheckRx().  If the CPHAL has received any data, it will
call @c Receive() to pass the driver the data.  Please reference @ref VDMA_Functions for more
information on these interfaces.

@section vdma_details VDMA Implementation Details

The following functions are not defined for use with VDMA:  @c Status(), @c Tick(), @c StatsGet(),
and @c StatsClear().

*/

/**
@page AAL5_Implementation_Details

@section aal5_ver Version

@copydoc AAL5_Version

@section aal5_intro Introduction

The AAL5 implementation will support 16 channels for transmit and 16 channels for
receive.  Each of the transmit channels may have up to two transmit queues
associated with it.  If two queues are used, Queue 0 is the high priority queue,
and Queue 1 is the low priority queue.

@section aal5_params AAL5 Configuration Parameters

AAL5 requires two device entries to be available in the configuration repository, named
@p "aal5" and @p "sar".  The @p aal5 device entry must contain @p base (base address)
and @p int_line (interrupt number). The @p sar device entry must have both @p base 
(base address) and @p reset_bit (reset bit).

@par Device Level Configuration Parameters

The following parameters are device-level parameters, which apply across all
channels.  The value for these parameters may be modified by changing the value in the
configuration repository.  

- "UniNni":  
AAL5 network setting. 0 = UNI (default), 1 = NNI.

@par Channel Configuration Parameters

All AAL5 channel parameters may also be configured through the @c ChannelSetup() interface.
Following is the list of @p CHANNEL_INFO members that may be modified by the driver when 
calling @c ChannelSetup().  The driver may provide a value of 0xFFFFFFFF for any channel 
parameter to select a default value for the parameter.  The driver should at a minimum 
configure @p Vci and @p Vpi.  The usage of all parameters beginning with TxVc_,
TxVp_, RxVc_, RxVp_ is described in greater detail in the SAR Firmware Spec.
These parameters are mainly associated with QoS and OAM functionality.

- "RxNumBuffers":    
The number of Rx buffer descriptors to allocate for Ch.  
- "RxBufSize":         
Size (in bytes) for each Rx buffer.
- "RxBufferOffset":   
Number of bytes to offset rx data from start of buffer (must be less than buffer size).
- "RxServiceMax":    
Maximum number of packets to service at one time.
- "TxNumBuffers":      
The number of Tx buffer descriptors to allocate for Ch.
- "TxNumQueues":        
Number of Tx queues for this channel (1-2). Choosing 2 enables a low priority SAR queue. 
- "TxServiceMax":       
Maximum number of packets to service at one time. 
- "CpcsUU":         
The 2-byte CPCS UU and CPI information.
- "Gfc":    
Generic Flow Control.  Used in ATM header of Tx packets.
- "Clp":   
Cell Loss Priority.  Used in ATM header of Tx packets.
- "Pti":   
Payload Type Indication.  Used in ATM header of Tx packets.
- "DaMask":           
Specifies whether credit issuance is paused when Tx data not available.
- "Priority":   
Priority bin this channel will be scheduled within.
- "PktType":   
0=AAL5,1=Null AAL,2=OAM,3=Transparent,4=AAL2.
- "Vci":                
Virtual Channel Identifier.
- "Vpi":                 
Virtual Path Identifier.
- "TxVc_AtmHeader":
In firmware OAM mode, this
is the ATM header to be appended to front of firmware generated VC OAM cells for
this channel.  Note: To generate host OAM cells, call @c Send() with 
the appropriate mode.
- "TxVc_CellRate":       
Tx rate, set as clock ticks between transmissions (SCR for VBR, CBR for CBR).
- "TxVc_QosType":        
0=CBR,1=VBR,2=UBR,3=UBRmcr.
- "TxVc_Mbs":            
Min Burst Size in cells.
- "TxVc_Pcr":            
Peak Cell Rate for VBR in clock ticks between transmissions.
- "TxVc_OamTc":   
TC Path to transmit OAM cells for TX connections (0,1).
- "TxVc_VpOffset":   
Offset to the OAM VP state table for TX connections.  Channels with the same
VPI must have the same VpOffset value.  Channels with different VPIs
must have unique VpOffset values.
- "RxVc_OamCh":   
Channel to which to terminate received OAM cells to be forwarded to the Host
for either Host OAM mode, or when RxVc_OamToHost is enabled during Firmware
OAM mode.
- "RxVc_OamToHost":   
Indicates whether to pass received unmatched OAM loopback cells to the host;
0=do not pass, 1=pass.
- "RxVc_AtmHeader":   
ATM Header placed on firmware gen'd OAM cells for this channel on a Rx 
connection (must be big endian with 0 PTI).
- "RxVc_OamTc":   
TC Path to transmit OAM cells for RX connections (0,1).
- "RxVc_VpOffset":   
Offset to the OAM VP state table for RX connections.  Channels with the same
VPI must have the same VpOffset value.  Channels with different VPIs
must have unique VpOffset values.
- "TxVp_OamTc":   
TC Path to transmit OAM cells for TX VP connections (0,1).
- "TxVp_AtmHeader":   
ATM Header placed on firmware gen'd VP OAM cells for this channel on a Tx VP
connection (must be big endian with 0 VCI).
- "RxVp_OamCh":   
Channel to which to terminate received OAM cells to be forwarded to the Host
for either Host OAM mode, or when RxVc_OamToHost is enabled during Firmware
OAM mode.
- "RxVp_OamToHost":   
Indicates whether to pass received unmatched OAM loopback cells to the host;
0=do not pass, 1=pass.
- "RxVp_AtmHeader":
In firmware OAM mode, this
is the ATM header to be appended to front of firmware generated VP OAM cells for
this channel.  Note: To generate host OAM cells, call @c Send() with 
the appropriate mode.
- "RxVp_OamTc":   
TC Path to transmit OAM cells for RX VP connections (0,1).
- "RxVp_OamVcList":   
This 32-bit field is one-hot encoded to indicate all the VC channels that are
associated with this VP channel.  A value of 21 will indicate that VC
channels 0, 2, and 4 are associated with this VP channel.
- "FwdUnkVc":
Indicates whether or not to forward unknown VCI/VPI cells to the host.  This 
parameter only takes effect if the channel's PktType is Transparent(3).
1=forwarding enabled, 0=forwarding disabled.

@section aal5_details API Implementation Details

ATTENTION: Documentation given here supplements the documentation given in the general
CPHAL API section.  The following details are crucial to correct usage of the
AAL5 CPHAL.

@par Receive()
The least significant byte of @p Mode contains the channel number.  Bit 31
indicates whether or not the ATM header is present in the first fragment of
the packet.  If bit 31 is set, the 4 byte ATM header (minus HEC) will be provided 
in the first fragment, with the payload beginning in the second fragment.  Currently,
this is the default behavior for host OAM and transparent mode packets.
Bits 17-16 indicate the packet type that is being received.  
Mode Parameter Breakdown: <BR>
- 31     ATM Header In First Fragment (1=true, 0=false) <BR>
- 30-18  Unused. <BR>
- 17-16  Pkt Type. <BR>
  -   0=AAL5 <BR>
  -   1=PTI Based Null AAL <BR>
  -   2=OAM <BR>
  -   3=Transparent <BR>
- 15-08  Unused. <BR>
- 07-00  Channel Number.

@par Send()
The most significant 16 bits of the first fragment 'len' is used as the Offset 
to be added to the packet.  @c Send() will reserve this many bytes at the 
beginning of the transmit buffer prior to the first byte of valid data.
For the @p Mode parameter, Bit 31 must be set if the user has sent a packet with
the ATM Header (minus HEC) embedded in the first 4 bytes of the first fragment data buffer.
The OS has the option of using a 4 byte first fragment containing only ATM header,
or concatenating the ATM Header in front of the data payload.
If Bit 31 is set, the ATM Header in the buffer is preserved and sent with
each cell of the packet.  Otherwise, Send() will build the ATM header based on the
values of the Pti, Gfc, Clp, Vpi, and Vci parameters for the given channel.
Bits 17-16 are defined as the packet type.  Bits 15-08 may be used to specify the 
transmit queue to send the packet on.  Only values 0 (high priority) and 1 (low 
priority) are accepted.  Bits 07-00 should be used to indicate the channel number 
for the @c Send() operation.  Valid channel numbers are 0-15.
Mode Parameter Breakdown: <BR>
- 31     ATM Header In Packet (1=true, 0=false) <BR>
- 30-18  Unused. <BR>
- 17-16  Pkt Type. <BR>
  -   0=AAL5 <BR>
  -   1=PTI Based Null AAL <BR>
  -   2=OAM <BR>
  -   3=Transparent <BR>
- 15-08  Transmit Queue. <BR>
- 07-00  Channel Number.
  
@par ChannelSetup()
The AAL5 @c ChannelSetup() always configures both the Tx and Rx side of the channel
connection in the same call.

@par ChannelTeardown()
Regardless of the channel teardown direction selected, the AAL5 CPHAL will always
teardown both the Tx and Rx side of the channel connection.

@par TeardownComplete()
The value for the @p Direction parameter should be ignored for the AAL5 implementation,
since both directions (Tx and Rx) are always torndown in response to a @c ChannelTeardown()
command.

@par Control() (HAL version)
Defined keys and actions.  Unless otherwise stated, the data type
for Value is pointer to unsigned integer.  The list is broken into
three groups, one group which can be used anytime, one group that should
be used before halOpen(), and one group which can only be used after 
halOpen() (but before halClose()).  For channelized parameters, replace
'Ch' with the integer number of a channel (ex. "Gfc.4" can be used to set
Gfc for channel 4).

MAY USE ANYTIME AFTER INIT (after halInit() is called):

- "Gfc.Ch".  The OS may "Set" this value.  Changing this value causes
the Gfc in each Tx ATM header for this channel to take on the new Gfc value.

- "Clp.Ch".  The OS may "Set" this value.  Changing this value causes
the Clp in each Tx ATM header for this channel to take on the new Clp value.

- "Pti.Ch".  The OS may "Set" this value.  Changing this value causes
the Pti in each Tx ATM header for this channel to take on the new Pti value.

- "CpcsUU.Ch".  The OS may "Set" this value.  Changing this value causes
the CpcsUU in each Tx ATM header for this channel to take on the new CpcsUU value.

- "OamMode".  Specifies if host or firmware is performing OAM functions; 0 = Host OAM, 
1 = Firmware OAM.  When set, all SAR channels will be configured for
the selection, including AAL2 channels.

- "OamLbTimeout".  Specifies the firmware OAM loopback timeout, in milliseconds.

- "DeviceCPID".  The OS may "Set" this value.  This is the OAM connection 
point identifier.  The OS should provide a pointer to an array of 4 32-bit
integers.  Each word must be configured in big endian format.

- "FwdUnkVc.Ch".  Indicates whether or not to forward unknown VCI/VPI cells to the host.
This parameter only takes effect if the channel's PktType is Transparent(3).
1=forwarding enabled, 0=forwarding disabled.

MAY USE ONLY BEFORE HAL IS OPEN (before halOpen() call):
- "StrictPriority". The OS may "Set" this value.  Setting to 1 causes
a different interrupt processing routine to be used, which gives strict
priority to channels with lower numbers (channel 0 has highest priority).
The default handler gives equal priority to all channels.

- "MaxFrags".  The OS may "Set" or "Get" this value.  This defines the maximum
number of fragments that can be received by the AAL5 Rx port.  The default
value for AAL5 is 46.  This provides enough space to receive a maximum
length AAL5 packet (65,568 bytes) with the default buffer size of 1518 bytes, and
any amount of RxBufferOffset.  If the buffer size is configured to be smaller,
the OS *MUST* modify this parameter according to the following formula: 
((System Max AAL5 packet length)/(RxBufSize)) + 2.  (The extra two fragments in
the formula allow for RxBufferOffset and one fragment for the ATM Header, used
when receiving host OAM or transparent mode packets)

MAY USE ONLY AFTER HAL IS 'OPEN' (after halOpen() call):
- "Stats;Level;Ch;Queue".  The OS may "Get" Stats groups with this key, where 
'Level' is an integer from 0-4, Ch is an integer from 0-15, and Queue is
an integer from 0-1.  Note that Ch is not required for Level 4 stats, and Queue
is not required for Level 0, 3, and 4.  The statistics functionality and return 
value is described in the appendix entitled "Configuration and Control".

- "TxVc_CellRate.Ch".  The OS may "Set" this value.  Can be used to modify
CellRate for a channel on the fly.

- "TxVc_Mbs.Ch".  The OS may "Set" this value.  Can be used to modify
Mbs for a channel on the fly.

- "TxVc_Pcr.Ch".  The OS may "Set" this value.  Can be used to modify
Pcr for a channel on the fly.

- "PdspEnable".  The OS may "Set" this value.  Value 0 disables the PDSP.
Value 1 enables to PDSP.

- "DeviceCPID".  The OS may "Set" this value.  The Value should be an array
of 4 32-bit integers that comprise the CPID.  

- "RxVc_RDICount.Ch".  The OS may "Get" or "Set" this value.  Get returns
the current RDI count for the VC channel.  Set clears the counter, and the Value
is ignored.

- "RxVp_RDICount.Ch".  The OS may "Get" or "Set" this value.  Get returns
the current RDI count for the VP channel.  Set clears the counter, and the Value
is ignored.

- "RxVc_AISseg.Ch". The OS may "Get" this value.  This is an indication of
AIS segment error for the VC channel.

- "RxVp_AISseg.Ch". The OS may "Get" this value.  This is an indication of
AIS segment error for the VP channel.

- "RxVc_AISetoe.Ch". The OS may "Get" this value.  This is an indication of
AIS end-to-end error for the VC channel.

- "RxVp_AISetoe.Ch". The OS may "Get" this value.  This is an indication of
AIS end-to-end error for the VP channel.

- "RxVc_OamCh.Ch". The OS may "Set" this value.  Channel to which to terminate 
received OAM cells to be forwarded to the Host for either Host OAM mode, or when 
RxVc_OamToHost is enabled during Firmware OAM mode.

- "RxVp_OamCh.Ch". The OS may "Set" this value.  Channel to which to terminate 
received OAM cells to be forwarded to the Host for either Host OAM mode, or when 
RxVp_OamToHost is enabled during Firmware OAM mode.

- "F4_LB_Counter". The OS may "Get" this value.  This is a count of the number
  of near-end F4 loopbacks performed by the PDSP in firmware OAM mode.

- "F5_LB_Counter". The OS may "Get" this value.  This is a count of the number
  of near-end F5 loopbacks performed by the PDSP in firmware OAM mode.

- "TxVc_AtmHeader.Ch". The OS may "Set" this value.  In firmware OAM mode, this
is the ATM header to be appended to front of firmware generated VC OAM cells for
this channel.  In host OAM mode, this is used as the ATM header to be appended
to front of host generated VC OAM cells for this channel.  It must be configured
as big endian with PTI=0.  Note: To generate host OAM cells, call @c Send() with 
the appropriate mode.

- "TxVp_AtmHeader.Ch". The OS may "Set" this value.  In firmware OAM mode, this
is the ATM header to be appended to front of firmware generated VP OAM cells for
this channel.  In host OAM mode, this is used as the ATM header to be appended
to front of host generated VP OAM cells for this channel.  It must be configured
as big endian with VCI=0.  Note: To generate host OAM cells, call @c Send() with 
the appropriate mode.

- "PdspEnable".  The OS may "Set" this value. Controls whether or not the PDSP is 
allowed to fetch new instructions.  The PDSP is enabled by the CPHAL during Open(), 
and disabled during Close().  0 = disabled, 1 = enabled.

@par Control() (OS version)
Defined keys and actions:

- "Firmware".  The CPHAL will perform a "Get" action for the key "Firmware".  A pointer
to a pointer is passed in @p Value.  The OS must modify the referenced pointer to point
to the firmware.

- "FirmwareSize".  The CPHAL will perform a "Get" action for the key "FirmwareSize".
The OS must place the firmware size in the memory pointed at by @p Value.

- "OamLbResult".  When a channel that is in firmware OAM mode is commanded to perform
a loopback function, the result of the loopback generates an interrupt that is handled
by the OS like any other interrupt.  The CPHAL, upon servicing the interrupt, will call
osControl with this key, and an action of "Set".  The @p Value parameter will be a 
pointer to the integer result.  1 = pass, 0 = fail.

- "SarFreq". The CPHAL will perform a "Get" action for this key.  The OS should place
the SAR frequency (in Hz) in the memory pointed at by @p Value.

@section aal5_stats AAL5 Specific Statistics

Statistics level '0' contains all AAL5 specific statistics.  The following values will
be obtained when requesting stats level 0:

- "Crc Errors".  Number of CRC errors reported by SAR hardware.  Incremented for received
packets that contain CRC errors.  

- "Len Errors".  Number of length errors reported by SAR hardware.  Incremented for received
packets that are in excess of 1366 cells.

- "Abort Errors".  Number of abort errors reported by SAR hardware.

- "Starv Errors".  Number of buffer starvation errors reported by SAR hardware.  Incremented
when a part or all of a buffer cannot be received due to lack of RX buffer resources.  The SAR
drops all cells associated with the packet for each buffer starvation error that occurs.

*/ 

/* register files */
#include "cp_sar_reg.h"  

#define _CPHAL_AAL5
#define _CPHAL
#define _CPPI_TEST /** @todo remove for release */
#define __CPHAL_CPPI_OFFSET /* support use of offset */

/*  OS Data Structure definitions  */

typedef void OS_PRIVATE;
typedef void OS_DEVICE;
typedef void OS_SENDINFO;
typedef void OS_RECEIVEINFO;
typedef void OS_SETUP;

/*  CPHAL Data Structure definitions  */

typedef struct hal_device  HAL_DEVICE;
typedef struct hal_private HAL_PRIVATE;
typedef struct hal_private HAL_RECEIVEINFO;

/* include CPHAL header files here */
#include "cpcommon_cpaal5.h"
#include "cpswhal_cpaal5.h"
#include "aal5sar.h"
#include "cpcommon_cpaal5.c"
    
#define CR_SERVICE            (170-1)
#define UTOPIA_PAUSE_REG      (*(volatile bit32u *)0xa4000000)

/* 
these masks are for the mode parameter used in halSend/OsReceive
(may move these elsewhere)
*/
#define CH_MASK            0xff
#define PRI_MASK           0x10000 
  
/* Rcb/Tcb Constants */
#define CB_SOF_BIT         (1<<31)
#define CB_EOF_BIT         (1<<30)
#define CB_SOF_AND_EOF_BIT (CB_SOF_BIT|CB_EOF_BIT)
#define CB_OWNERSHIP_BIT   (1<<29)
#define CB_EOQ_BIT         (1<<28)
#define CB_SIZE_MASK       0x0000ffff
#define CB_OFFSET_MASK     0xffff0000
#define RCB_ERRORS_MASK    0x03fe0000
#define RX_ERROR_MASK      0x000f0000
#define CRC_ERROR_MASK     0x00010000
#define LENGTH_ERROR_MASK  0x00020000
#define ABORT_ERROR_MASK   0x00040000
#define STARV_ERROR_MASK   0x00080000
#define TEARDOWN_VAL       0xfffffffc

/* interrupt vector masks */
#define TXH_PEND        0x01000000
#define TXL_PEND        0x02000000
#define RX_PEND         0x04000000
#define STS_PEND        0x08000000
#define AAL2_PEND       0x10000000
#define INT_PENDING     (TXH_PEND | TXL_PEND | RX_PEND | STS_PEND | AAL2_PEND)
#define STS_PEND_INVEC  0x0001F000
#define RX_PEND_INVEC   0x00000F00
#define TXL_PEND_INVEC  0x000000F0
#define TXH_PEND_INVEC  0x0000000F
#define AIS_SEG_MASK    0x1        /* +01.02.00 */
#define AIS_SEG_SHIFT   0          /* +01.02.00 */
#define AIS_ETOE_MASK   0x20000    /* +01.02.00 */
#define AIS_ETOE_SHIFT  17         /* +01.02.00 */
#define RDI_CNT_MASK    0xffff0000 /* +01.02.00 */
#define RDI_CNT_SHIFT   16         /* +01.02.00 */

/*
 *  This function takes a vpi/vci pair and computes the 4 byte atm header
 *  (minus the HEC).
 *
 *  @param  vpi  Virtual Path Identifier.
 *  @param  vci  Virtual Channel Identifier.
 *
 *  @return  A properly formatted ATM header, without the HEC.
 */ 
static int atmheader(int gfc, int vpi, int vci, int pti, int clp)
  {
   int itmp;

   itmp=0;
  
   /* UNI Mode uses the GFC field */
   itmp |= ((gfc & 0xF) << 28);
   itmp |= ((vpi & 0xFF) << 20);
    
   /* if NNI Mode, no gfc and larger VPI */
   /*itmp |= ((vpi & 0xFFF) << 20);*/

   itmp|=((vci & 0xFFFF) << 4);
   itmp|=((pti & 0x7) << 1);
   itmp|=(clp & 0x1);
   return(itmp);
  }

#include "cppi_cpaal5.c"

/*
 *   Re-entrancy Issues
 *   In order to ensure successful re-entrancy certain sections of the
 *   CPHAL code will be bracketed as Critical.
 *   The OS will provide the function Os.CriticalSection(BOOL), which
 *   will be passed a TRUE to enter the Critical Section and FALSE to exit.
 */

/*
 *  @ingroup CPHAL_Functions
 *  Clears the statistics information.
 *
 *  @param  HalDev   CPHAL module instance. (set by xxxInitModule())
 *
 *  @return 0 OK, Non-zero not OK
 */
static int StatsClear(HAL_DEVICE *HalDev)
  {
   int i;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]StatsClear(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   /* clear stats */
   for (i=0; i<NUM_AAL5_CHAN; i++)
     {
      HalDev->Stats.CrcErrors[i]=0;
      HalDev->Stats.LenErrors[i]=0;
      HalDev->Stats.DmaLenErrors[i]=0;
      HalDev->Stats.AbortErrors[i]=0;
      HalDev->Stats.StarvErrors[i]=0;
      HalDev->Stats.TxMisQCnt[i][0]=0;
      HalDev->Stats.TxMisQCnt[i][1]=0;
      HalDev->Stats.RxMisQCnt[i]=0;
      HalDev->Stats.RxEOQCnt[i]=0;
      HalDev->Stats.TxEOQCnt[i][0]=0;
      HalDev->Stats.TxEOQCnt[i][1]=0;
      HalDev->Stats.RxPacketsServiced[i]=0;
      HalDev->Stats.TxPacketsServiced[i][0]=0;
      HalDev->Stats.TxPacketsServiced[i][1]=0;
      HalDev->Stats.TxMaxServiced[i][0]=0;
      HalDev->Stats.TxMaxServiced[i][1]=0;
     }
   HalDev->Stats.RxTotal=0;
   HalDev->Stats.TxTotal=0;
   HalDev->Stats.RxMaxServiced=0;
   return (EC_NO_ERRORS);
  }

/*
 *  Returns statistics information.
 *
 *  @param  HalDev   CPHAL module instance. (set by xxxInitModule())  
 *
 *  @return 0
 */
/*
static STAT_INFO* StatsGet(HAL_DEVICE *HalDev)
  {
   STAT_INFO* MyStats = &HalDev->Stats;
#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]StatsGet(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   dbgPrintf("HAL Stats:\n");
   DispStat(HalDev, "Rx Total",MyStats->RxTotal);
   DispStat(HalDev, "Tx Total",MyStats->TxTotal);
   DispStat(HalDev, "Rx Peak",MyStats->RxMaxServiced);
   DispStat(HalDev, "TxH Peak",MyStats->TxMaxServiced[0][0]);
   DispStat(HalDev, "TxL Peak",MyStats->TxMaxServiced[0][1]);
   DispChStat(HalDev, "CrcErr",&MyStats->CrcErrors[0],1);
   DispChStat(HalDev, "LenErr",&MyStats->LenErrors[0],1);
   DispChStat(HalDev, "DmaLenErr",&MyStats->DmaLenErrors[0],1);
   DispChStat(HalDev, "AbortErr",&MyStats->AbortErrors[0],1);
   DispChStat(HalDev, "StarvErr",&MyStats->StarvErrors[0],1);
   DispChStat(HalDev, "TxH MisQ Cnt",&MyStats->TxMisQCnt[0][0],2);
   DispChStat(HalDev, "TxL MisQ Cnt",&MyStats->TxMisQCnt[0][1],2);
   DispChStat(HalDev, "Rx MisQ Cnt",&MyStats->RxMisQCnt[0],1);
   DispChStat(HalDev, "Rx EOQ Cnt",&MyStats->RxEOQCnt[0],1); 
   DispChStat(HalDev, "TxH EOQ Cnt",&MyStats->TxEOQCnt[0][0],2);
   DispChStat(HalDev, "TxL EOQ Cnt",&MyStats->TxEOQCnt[0][1],2);
   DispChStat(HalDev, "Rx Pkts",&MyStats->RxPacketsServiced[0],1);
   DispChStat(HalDev, "TxH Pkts",&MyStats->TxPacketsServiced[0][0],2);
   DispChStat(HalDev, "TxL Pkts",&MyStats->TxPacketsServiced[0][1],2); 

   return (&HalDev->Stats);
  }
*/

#ifdef __CPHAL_DEBUG
void dbgChannelConfigDump(HAL_DEVICE *HalDev, int Ch)
  {
   CHANNEL_INFO *HalCh = &HalDev->ChData[Ch];
   dbgPrintf("  [aal5 Inst %d, Ch %d] Config Dump:\n", HalDev->Inst, Ch);
   dbgPrintf("    TxNumBuffers  :%08d, TxNumQueues :%08d\n", 
             HalCh->TxNumBuffers, HalCh->TxNumQueues);
   dbgPrintf("    RxNumBuffers  :%08d, RxBufSize   :%08d\n", 
             HalCh->RxNumBuffers, HalCh->RxBufSize);
   dbgPrintf("    TxServiceMax  :%08d, RxServiceMax:%08d\n", 
             HalCh->TxServiceMax, HalCh->RxServiceMax);
   dbgPrintf("    RxBufferOffset:%08d, DaMask      :%08d\n", 
             HalCh->RxBufferOffset, HalCh->DaMask);
   dbgPrintf("    CpcsUU        :%08d, Gfc         :%08d\n", 
             HalCh->CpcsUU, HalCh->Gfc);
   dbgPrintf("    Clp           :%08d, Pti         :%08d\n", 
             HalCh->Clp, HalCh->Pti);
   dbgPrintf("    Priority      :%08d, PktType     :%08d\n", 
             HalCh->Priority, HalCh->PktType);
   dbgPrintf("    Vci           :%08d, Vpi         :%08d\n", 
             HalCh->Vci, HalCh->Vpi);
   dbgPrintf("    TxVc_CellRate :%08d, TxVc_QosType:%08d\n", 
             HalCh->TxVc_CellRate, HalCh->TxVc_QosType);
   dbgPrintf("    TxVc_Mbs      :%08d, TxVc_Pcr    :%08d\n", 
             HalCh->TxVc_Mbs, HalCh->TxVc_Pcr);
   dbgPrintf("    TxVc_AtmHeader:%08d\n", 
             HalCh->TxVc_AtmHeader);
   osfuncSioFlush();
  }
#endif

/*
 * Retrieves channel parameters from configuration file.  Any parameters
 * which are not found are ignored, and the HAL default value will apply,
 * unless a new value is given through the channel structure in the call
 * to ChannelSetup.
 */
static int ChannelConfigGet(HAL_DEVICE *HalDev, CHANNEL_INFO *HalChn)
  {
   unsigned int Ret, Value, Ch = HalChn->Channel;
   OS_FUNCTIONS *OsFunc = HalDev->OsFunc;
   void *ChInfo;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]ChannelConfigGet(HalDev:%08x, HalChn:%08x)\n", (bit32u)HalDev,
                (bit32u)HalChn);
      osfuncSioFlush();
     }
#endif

   Ret=OsFunc->DeviceFindParmValue(HalDev->DeviceInfo, channel_names[Ch], &ChInfo);
   if (Ret) return (EC_AAL5|EC_FUNC_CHSETUP|EC_VAL_CH_INFO_NOT_FOUND);

   /* i don't care if a value is not found because they are optional */
   Ret=OsFunc->DeviceFindParmUint(ChInfo, "TxNumBuffers", &Value);
   if (!Ret) HalDev->ChData[Ch].TxNumBuffers = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "TxNumQueues", &Value);
   if (!Ret) HalDev->ChData[Ch].TxNumQueues = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "TxServiceMax", &Value);
   if (!Ret) HalDev->ChData[Ch].TxServiceMax = Value;
   
   Ret=OsFunc->DeviceFindParmUint(ChInfo, "RxNumBuffers", &Value);
   if (!Ret) HalDev->ChData[Ch].RxNumBuffers = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "RxBufferOffset", &Value);
   if (!Ret) HalDev->ChData[Ch].RxBufferOffset = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "RxBufSize", &Value);
   if (!Ret) HalDev->ChData[Ch].RxBufSize = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "RxServiceMax", &Value);
   if (!Ret) HalDev->ChData[Ch].RxServiceMax = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "CpcsUU", &Value);
   if (!Ret) HalDev->ChData[Ch].CpcsUU = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "Gfc", &Value);
   if (!Ret) HalDev->ChData[Ch].Gfc = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "Clp", &Value);
   if (!Ret) HalDev->ChData[Ch].Clp = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "Pti", &Value);
   if (!Ret) HalDev->ChData[Ch].Pti = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "DaMask", &Value);
   if (!Ret) HalDev->ChData[Ch].DaMask = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "Priority", &Value);
   if (!Ret) HalDev->ChData[Ch].Priority = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "PktType", &Value);
   if (!Ret) HalDev->ChData[Ch].PktType = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "Vci", &Value);
   if (!Ret) HalDev->ChData[Ch].Vci = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "Vpi", &Value);
   if (!Ret) HalDev->ChData[Ch].Vpi = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "TxVc_CellRate", &Value);
   if (!Ret) HalDev->ChData[Ch].TxVc_CellRate = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "TxVc_QosType", &Value);
   if (!Ret) HalDev->ChData[Ch].TxVc_QosType = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "TxVc_Mbs", &Value);
   if (!Ret) HalDev->ChData[Ch].TxVc_Mbs = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "TxVc_Pcr", &Value);
   if (!Ret) HalDev->ChData[Ch].TxVc_Pcr = Value;

   Ret=OsFunc->DeviceFindParmUint(ChInfo, "TxVc_AtmHeader", &Value);
   if (!Ret) HalDev->ChData[Ch].TxVc_AtmHeader = Value;

   return (EC_NO_ERRORS);
  }

/*
 * Sets up channel parameters in the hardware, and initializes the CPPI
 * TX and RX buffer descriptors and buffers.
 */
static int ChannelConfigApply(HAL_DEVICE *HalDev, CHANNEL_INFO *HalChn)
  {
   int j, Ch = HalChn->Channel;
   volatile bit32u *pTmp;
   int Ret; /* +GSG 030410 */

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]ChannelConfigApply(HalDev:%08x, HalChn:%08x)\n", (bit32u)HalDev,
                (bit32u)HalChn);
      osfuncSioFlush();
     }
#endif

   if ((HalDev->ChIsOpen[Ch][DIRECTION_TX] == TRUE) || (HalDev->ChIsOpen[Ch][DIRECTION_RX] == TRUE))
     {
      return(EC_AAL5|EC_FUNC_CHSETUP|EC_VAL_CH_ALREADY_OPEN);
     }

   HalDev->InRxInt[Ch]=FALSE;
  
   /* Initialize Queue Data */
   HalDev->RxActQueueHead[Ch]=0;
   HalDev->RxActQueueCount[Ch]=0;
   HalDev->TxActQueueHead[Ch][0]=0;
   HalDev->TxActQueueHead[Ch][1]=0;
   HalDev->TxActQueueCount[Ch][0]=0;
   HalDev->TxActQueueCount[Ch][1]=0;
   HalDev->RxActive[Ch] = FALSE;
   HalDev->TxActive[Ch][0] = FALSE;
   HalDev->TxActive[Ch][1] = FALSE;

   /* Clear Rx State RAM */
   pTmp = pRX_DMA_STATE_WORD_0(HalDev->dev_base) + (Ch*64);
   for (j=0; j<NUM_RX_STATE_WORDS; j++)
     *pTmp++ = 0; 
     
   /* Check that Rx DMA State RAM was cleared */
   pTmp -= NUM_RX_STATE_WORDS;
   for (j=0; j<NUM_RX_STATE_WORDS; j++)
     {
      if (*pTmp++ != 0)
        {
         return(EC_AAL5|EC_FUNC_CHSETUP|EC_VAL_RX_STATE_RAM_NOT_CLEARED);
        }    
     } 
 
   /* Clear Tx State RAM */
   pTmp = pTX_DMA_STATE_WORD_0(HalDev->dev_base) + (Ch*64);
   for (j=0; j<NUM_TX_STATE_WORDS; j++)
     *pTmp++ = 0; 
     
   /* Check that Tx DMA State RAM was cleared */
   pTmp -= NUM_TX_STATE_WORDS;
   for (j=0; j<NUM_TX_STATE_WORDS; j++)
     {
      if (*pTmp++ != 0)
        {
         return(EC_AAL5|EC_FUNC_CHSETUP|EC_VAL_TX_STATE_RAM_NOT_CLEARED);
        }
     }
  
   /* Initialize Tx State RAM (Nothing to do) */
   
   /* Initialize Rx State RAM */
   /* Configure the Rx buffer offset */
   pTmp=(pRX_DMA_STATE_WORD_0(HalDev->dev_base) + (Ch*64));
   *pTmp |= (HalDev->ChData[Ch].RxBufferOffset & 0xFF);

   /* Initialize buffer memory for the channel */
   Ret = InitTcb(HalDev, Ch);
   if (Ret) return (Ret);

   Ret = InitRcb(HalDev, Ch);
   if (Ret) return (Ret);

   /* setup interrupt mask/enable for the channel */
   SAR_TX_MASK_SET(HalDev->dev_base) = (1<<Ch);

   /* if using the low priority queue, set up mask for it */
   if (HalDev->ChData[Ch].TxNumQueues == 2)            /* +GSG 030421 */
     SAR_TX_MASK_SET(HalDev->dev_base) = (1<<Ch)<<16;  /* +GSG 030421 */

   SAR_RX_MASK_SET(HalDev->dev_base) = (1<<Ch);

   /* call SAR layer to complete the channel setup - hardware configuration of ch */
   Ret = HalDev->SarFunc->ChannelSetup(HalDev->SarDev, &HalDev->ChData[Ch]); /* ~GSG 030410 */
   if (Ret)                                                                  /* +GSG 030410 */
     return (Ret);                                                           /* +GSG 030410 */

   /* channel officially open for business */
   HalDev->ChIsOpen[Ch][DIRECTION_TX] = TRUE;
   HalDev->ChIsOpen[Ch][DIRECTION_RX] = TRUE;

   return (EC_NO_ERRORS);
  }

/*
 * Sets up HAL default channel configuration parameter values.
 */
static void ChannelConfigInit(HAL_DEVICE *HalDev, CHANNEL_INFO *HalChn)
  {
   int Ch = HalChn->Channel;
   
#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]ChannelConfigInit(HalDev:%08x, HalChn:%08x)\n", (bit32u)HalDev,
                (bit32u)HalChn);
      osfuncSioFlush();
     }
#endif

   HalDev->ChData[Ch].Channel        = Ch;
   HalDev->ChData[Ch].TxNumBuffers   = cfg_tx_num_bufs[Ch];
   HalDev->ChData[Ch].RxNumBuffers   = cfg_rx_num_bufs[Ch];
   HalDev->ChData[Ch].RxBufSize      = cfg_rx_buf_size[Ch];
   HalDev->ChData[Ch].RxBufferOffset = cfg_rx_buf_offset[Ch];
   HalDev->ChData[Ch].TxNumQueues    = cfg_tx_num_queues[Ch]; 
   HalDev->ChData[Ch].CpcsUU         = cfg_cpcs_uu[Ch];
   HalDev->ChData[Ch].DaMask         = cfg_da_mask[Ch];
   HalDev->ChData[Ch].Priority       = cfg_priority[Ch];
   HalDev->ChData[Ch].PktType        = cfg_pkt_type[Ch];
   HalDev->ChData[Ch].Vci            = cfg_vci[Ch];
   HalDev->ChData[Ch].Vpi            = cfg_vpi[Ch];
   HalDev->ChData[Ch].TxVc_CellRate  = cfg_cell_rate[Ch];
   HalDev->ChData[Ch].TxVc_QosType   = cfg_qos_type[Ch];
   HalDev->ChData[Ch].TxVc_Mbs       = cfg_mbs[Ch];
   HalDev->ChData[Ch].TxVc_Pcr       = cfg_pcr[Ch];
   HalDev->ChData[Ch].Gfc            = cfg_gfc[Ch];
   HalDev->ChData[Ch].Clp            = cfg_clp[Ch];
   HalDev->ChData[Ch].Pti            = cfg_pti[Ch];
   HalDev->ChData[Ch].RxServiceMax   = cfg_rx_max_service[Ch];
   HalDev->ChData[Ch].TxServiceMax   = cfg_tx_max_service[Ch];
  }

/* 
 * Update per channel data in the HalDev based channel structure.
 * If a certain channel parameter has been passed with the HAL_DEFAULT
 * value (0xFFFFFFFF), then do not copy it. 
 */
static void ChannelConfigUpdate(HAL_DEVICE *HalDev, CHANNEL_INFO *HalChn)
  {
   int Ch = HalChn->Channel;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]ChannelConfigUpdate(HalDev:%08x, HalChn:%08x)\n", (bit32u)HalDev,
                (bit32u)HalChn);
      osfuncSioFlush();
     }
#endif

   HalDev->ChData[Ch].Channel = Ch;

   /* ChannelUpdate is a macro defined in cpcommon.h.  It requires
      the presence of the variables named 'Ch' and 'HalChn'.*/
   ChannelUpdate(DaMask);
   ChannelUpdate(Priority);
   ChannelUpdate(PktType);
   ChannelUpdate(Vci);
   ChannelUpdate(Vpi);
   ChannelUpdate(CpcsUU);
   ChannelUpdate(Gfc);
   ChannelUpdate(Clp);
   ChannelUpdate(Pti);
   /* AAL5 Stuff */
   ChannelUpdate(TxNumBuffers);
   ChannelUpdate(RxNumBuffers);
   ChannelUpdate(RxBufSize);
   ChannelUpdate(RxBufferOffset);
   ChannelUpdate(TxNumQueues);
   ChannelUpdate(TxServiceMax);
   ChannelUpdate(RxServiceMax);
   /* PDSP STATE RAM */
   ChannelUpdate(TxVc_CellRate);
   ChannelUpdate(TxVc_QosType);
   ChannelUpdate(TxVc_Mbs);
   ChannelUpdate(TxVc_Pcr);
   /* OAM */
   ChannelUpdate(TxVc_AtmHeader);
   ChannelUpdate(TxVc_OamTc);
   ChannelUpdate(TxVc_VpOffset);
   ChannelUpdate(RxVc_OamCh);
   ChannelUpdate(RxVc_OamToHost);
   ChannelUpdate(RxVc_AtmHeader);
   ChannelUpdate(RxVc_VpOffset);
   ChannelUpdate(RxVc_OamTc);
   ChannelUpdate(TxVp_AtmHeader);
   ChannelUpdate(TxVp_OamTc);
   ChannelUpdate(RxVp_AtmHeader);
   ChannelUpdate(RxVp_OamCh);
   ChannelUpdate(RxVp_OamTc);
   ChannelUpdate(RxVp_OamToHost);
   ChannelUpdate(RxVp_OamVcList);
   ChannelUpdate(FwdUnkVc);
  }

/**
 * @ingroup CPHAL_Functions
 * This function opens the specified channel.  The caller must populate
 * the @p HalCh structure.  CPHAL default values may be requested for any or all
 * members of the @p HalCh structure by supplying a value of 0xFFFFFFFF for the
 * given member.  The @p OsSetup parameter is a pointer to an OS defined
 * data structure.  If the CPHAL later calls @c MallocRxBuffer(), this pointer
 * is returned in that call.
 *  
 * @param   HalDev  CPHAL module instance. (set by xxxInitModule())   
 * @param   HalCh   Per channel information structure.  Implementation specific.
 * @param   OsSetup Pointer to an OS-defined data structure.
 *
 * @return  EC_NO_ERRORS (ok). <BR>
 *          Possible Error Codes:<BR>
 *          @ref EC_VAL_INVALID_STATE "EC_VAL_INVALID_STATE"<BR>
 *          @ref EC_VAL_NULL_CH_STRUCT "EC_VAL_NULL_CH_STRUCT"<BR>
 *          @ref EC_VAL_INVALID_CH "EC_VAL_INVALID_CH"<BR>
 *          @ref EC_VAL_CH_ALREADY_OPEN "EC_VAL_CH_ALREADY_OPEN"<BR>
 *          @ref EC_VAL_RX_STATE_RAM_NOT_CLEARED "EC_VAL_RX_STATE_RAM_NOT_CLEARED"<BR>
 *          @ref EC_VAL_TX_STATE_RAM_NOT_CLEARED "EC_VAL_TX_STATE_RAM_NOT_CLEARED"<BR>
 *          @ref EC_VAL_TCB_MALLOC_FAILED "EC_VAL_TCB_MALLOC_FAILED"<BR>
 *          @ref EC_VAL_RCB_MALLOC_FAILED "EC_VAL_RCB_MALLOC_FAILED"<BR>
 *          @ref EC_VAL_RX_BUFFER_MALLOC_FAILED "EC_VAL_RX_BUFFER_MALLOC_FAILED"<BR>
 *          @ref EC_VAL_LUT_NOT_READY "EC_VAL_LUT_NOT_READY"<BR>
 */
static int halChannelSetup(HAL_DEVICE *HalDev, CHANNEL_INFO *HalCh, OS_SETUP *OsSetup)
  {
  int Ch, Ret;
   
#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]halChannelSetup(HalDev:%08x, HalCh:%08x, OsSetup:%08x)\n", (bit32u)HalDev,
                (bit32u)HalCh, (bit32u)OsSetup);
      osfuncSioFlush();
     }
#endif

  /* Verify proper device state */
  if (HalDev->State < enInitialized)
    return (EC_AAL5|EC_FUNC_CHSETUP|EC_VAL_INVALID_STATE);

  /* We require the channel structure to be passed, even if it only contains
     the channel number */
  if (HalCh == NULL)
    {
     return(EC_AAL5|EC_FUNC_CHSETUP|EC_VAL_NULL_CH_STRUCT);
    }

  Ch = HalCh->Channel;

  if ((Ch < 0) || (Ch > MAX_AAL5_CHAN))
    {
     return(EC_AAL5|EC_FUNC_CHSETUP|EC_VAL_INVALID_CH);
    }

  /* if channel is already open, this call is invalid */
  if ((HalDev->ChIsOpen[Ch][DIRECTION_TX] == TRUE) || (HalDev->ChIsOpen[Ch][DIRECTION_RX] == TRUE))
    {
     return(EC_AAL5|EC_FUNC_CHSETUP|EC_VAL_CH_ALREADY_OPEN);
    }

  /* channel is closed, but might be setup.  If so, reopen the hardware channel. */
  if ((HalDev->ChIsSetup[Ch][DIRECTION_TX] == FALSE) && (HalDev->ChIsSetup[Ch][DIRECTION_RX] == FALSE))
    {
      /* Setup channel configuration */
      /* Store OS_SETUP */
      HalDev->ChData[Ch].OsSetup = OsSetup;                                         /* ~GSG 030508 */

      /* setup HAL default values for this channel first */
      ChannelConfigInit(HalDev, HalCh);
 
      /* retrieve options.conf channel parameters */
      /* currently ignoring return value, making the choice that it's okay if
         the user does not supply channel configuration in the data store */
      ChannelConfigGet(HalDev, HalCh);
      
      /* update HalDev with data given in HalCh */  
      ChannelConfigUpdate(HalDev, HalCh);
      
#ifdef __CPHAL_DEBUG
      if (DBG(8))
        {
         dbgChannelConfigDump(HalDev, Ch);
        }
#endif

      /* HalDev->ChIsSetup[Ch][0] = TRUE; */
      HalDev->ChIsSetup[Ch][DIRECTION_TX] = TRUE;
      HalDev->ChIsSetup[Ch][DIRECTION_RX] = TRUE;

      /* I don't initialize RcbStart or TcbStart here because their values may be
         reused across several Setup/Teardown calls */
    }

  /* If the hardware has been opened (is out of reset), then configure the channel
     in the hardware. NOTE: ChannelConfigApply calls the CPSAR ChannelSetup()! */
  if (HalDev->State == enOpened)
    {
     Ret = ChannelConfigApply(HalDev, HalCh);
     if (Ret) return (Ret);
    }

  return (EC_NO_ERRORS);
  }
   
/*
 *  This function configures the rate at which the OAM timer scheduler
 *  channels will be scheduled.  The value of OamRate is the number of
 *  clock ticks between cell transmissions (if OAM function is sourcing
 *  cells), or the number of clock ticks between events or absence of events
 *  (if OAM function is sinking cells).  The value of i indicates
 *  which OAM function to apply the rate to.  A list is given below.
 *
 *  @par Oam Function Values
 *  - 0  : Loopback source
 *  - 1  : F4 CC source
 *  - 2  : F5 CC source
 *  - 3  : F4 CC sink
 *  - 4  : F5 CC sink
 *  - 5  : F4 TX AIS source
 *  - 6  : F5 TX AIS source
 *  - 7  : F4 RX RDI source
 *  - 8  : F5 RX RDI source
 *  - 9  : F4 AIS monitor
 *  - 10 : F5 AIS monitor
 *
 *  The following is information on how to calculate the OAM rate.  There
 *  is only one OAM timer that is shared among all channels.  Therefore, if
 *  you wanted an OAM source function (ex. F4 CC source) to generate 1 cell/sec 
 *  across 8 channels, you would need to configure the OAM timer to schedule 8 
 *  cells/sec.  In addition, the credits are shared between segment and end-to-end
 *  type OAM cells, so if you were sending both types of cells, you would
 *  need to configure the OAM timer for 16 cells/sec.  However, the clock
 *  rate must be specified in clock ticks between events.  Using an example
 *  clock rate of 125 MHz, the rate in clock ticks can be calculated by 
 *  dividing 125 Mhz by 16 cells/sec.  The results is 7812500 ticks.  Thus,
 *  every 7812500 clock cycles, an OAM cell will be generated for the F4 CC
 *  Source function.
 */
static void OamRateConfig(HAL_DEVICE *HalDev)
  {
   int i;
   bit32u OamRate, Freq = HalDev->SarFrequency;

   /* Configure OAM Timer State Block */
   for (i=0; i<NUM_OAM_RATES; i++)
     {
      switch(i)
        {
         case 0:  OamRate = ((Freq/1000)*HalDev->OamLbTimeout); 
                  break;
         case 1: 
         case 2:
         case 5:
         case 6: 
         case 7:
         case 8:  OamRate = (Freq/38);
                  break;
         case 3:
         case 4:  OamRate = ((Freq*3) + (Freq/2))/38;
                  break;
         case 9:
         case 10: OamRate = ((Freq*2) + (Freq/2))/38;
                  break;
         default: OamRate = (Freq*5);
                  break;
        }
  
      *(pOAM_TIMER_STATE_WORD_0(HalDev->dev_base) + (i*64) + 1) = OamRate;
     }
  }

/**
 *  @ingroup AAL5_Functions
 *  This function is used to enable OAM functions (other than loopback) for a 
 *  particular channel.  The channel (embedded within OamConfig - see below) must 
 *  have been configured for firmware OAM (not host OAM) for these configurations
 *  to take effect.  More than one function may be enabled at one time. 
 *  If more than one function is enabled, they must all be of the same level, all
 *  F4(VP) or all F5(VC).   
 * 
 *  The usage of the OamConfig parameter is described through the table below.  To
 *  initiate firmware OAM, set one or more bits in OamConfig corresponding to the
 *  various OAM functions.  To disable firmware OAM functions, set bit 30 along 
 *  with any other combination of bits to shutdown various OAM functions at once.
 *
 *  Acronyms:
 *  e2e - end to end, seg - segment, CC - continuity check, 
 *  AIS - Alarm Indication Signal
 *
 *  @par Bit:   Function:               Description
 *  - 31:       Reserved:
 *  - 30:       Setup/Teardown:         0 - enable, 1 - disable (Note 1)
 *  - 29:       F4  CC  Source seg:     0 - no action, 1 - configure 
 *  - 28:       F4  CC  Source e2e:     0 - no action, 1 - configure
 *  - 27:       F4  AIS Source seg:     0 - no action, 1 - configure
 *  - 26:       F4  AIS Source e2e:     0 - no action, 1 - configure
 *  - 25:       F5  CC  Source seg:     0 - no action, 1 - configure
 *  - 24:       F5  CC  Source e2e:     0 - no action, 1 - configure
 *  - 23:       F5  AIS Source seg:     0 - no action, 1 - configure
 *  - 22:       F5  AIS Source e2e:     0 - no action, 1 - configure
 *  - 21:       F4  CC  Sink seg:       0 - no action, 1 - configure
 *  - 20:       F4  CC  Sink e2e:       0 - no action, 1 - configure
 *  - 19:       F5  CC  Sink seg:       0 - no action, 1 - configure
 *  - 18:       F5  CC  Sink e2e:       0 - no action, 1 - configure
 *  - 17:8:     Reserved:
 *  - 7:0:      Channel:                AAL5/AAL2 VC/VP channel (Note 2)
 * 
 *
 *  Note 1:  This bit must be clear to enable the specified OAM function.
 *  Note 2:  This must specify the VC channel for F5 functions, and the VP
 *           channel for F4 functions.
 *
 *  @param HalDev      CPHAL module instance. (set by xxxInitModule())
 *  @param OamConfig   A 32-bit integer field defined as follows: 
 */
static void halOamFuncConfig(HAL_DEVICE *HalDev, unsigned int OamConfig)
  {
   /* GPR 0 */
   SAR_PDSP_HOST_OAM_CONFIG_REG(HalDev->dev_base) = OamConfig;
  }

/**
 *  @ingroup AAL5_Functions
 *  This function is used to enable OAM loopback functions for a particular 
 *  channel.  The channel (embedded within OamConfig - see below) must have been 
 *  configured for firmware OAM (not host OAM) for these configurations to take 
 *  effect.  Only one loopback function can be enabled at a time.  
 *
 *  The LLID is inserted into to the OAM cell's LLID field, and it specifies the 
 *  LLID of the connection point in the network where the generated loopback cell
 *  should be turned around.  The LLID is composed of 4 32-bit words, and this 
 *  function expects the caller to pass an array of 4 words in the LLID field.  
 *  The CorrelationTag is a 32-bit word that the PDSP uses to correlate loopback 
 *  commands with loopback responses.  It should simply be changed for each 
 *  call, and there is no restriction on the value used for CorrelationTag.
 * 
 *  The usage of the OamConfig parameter is described through the table below.  To
 *  initiate firmware OAM, set one of the bits corresponding to the
 *  various loopback OAM functions.  Note that only one loopback source may be 
 *  commanded at a time.
 *
 *  Acronyms:
 *  e2e - end to end, seg - segment, LB - loopback
 * 
 *  @par Bit:   Function:               Description
 *  - 31:16:    Reserved:
 *  - 15:       F4  LB Source seg:      0 - no action, 1 - configure (Note 1)
 *  - 14:       F4  LB Source seg:      0 - no action, 1 - configure (Note 1)
 *  - 13:       F4  LB Source e2e:      0 - no action, 1 - configure (Note 1)
 *  - 12:       F4  LB Source e2e:      0 - no action, 1 - configure (Note 1) 
 *  - 11:8:     Reserved:
 *  - 7:0:      Channel:                AAL5/AAL2 VC/VP channel (Note 2)
 * 
 *
 *  Note 1:  Only one LB function may be enabled at one time.  Once enabled, 
 *           the PDSP will time out after 5 seconds.  The host must wait until it 
 *           has received the result of the current LB request before initiating 
 *           a new request. <BR>
 *  Note 2:  This must specify the VC channel for F5 functions, and the VP
 *           channel for F4 functions.
 *
 *  @param HalDev      CPHAL module instance. (set by xxxInitModule())
 *  @param OamConfig   A 32-bit integer field defined as follows: 
 *  @param LLID        Loopback Location Identifier (passed as 4 word array).
 *                     Must be configured in big endian format.
 *  @param CorrelationTag 32-bit tag correlates loopback commands with loopback 
 *                        responses.  Must be configured in big endian format.
 *
 */
static void halOamLoopbackConfig(HAL_DEVICE *HalDev, unsigned int OamConfig, unsigned int *LLID, unsigned int CorrelationTag)
  {
   volatile bit32u *tmp;

   /* test to see if this is a loopback command */
   if (OamConfig & 0xf000)
     {
      /* write the OAM correlation tag (GPR 1) */
      SAR_PDSP_OAM_CORR_REG(HalDev->dev_base) = CorrelationTag;

      /* write the LLID */
      tmp = pOAM_CONFIG_BLOCK_WORD_0(HalDev->dev_base);

      /* advance past the CPID */
      tmp += 4;

      *tmp++ = LLID[0];
      *tmp++ = LLID[1];
      *tmp++ = LLID[2];
      *tmp   = LLID[3];

      /* GPR 0 */
      SAR_PDSP_HOST_OAM_CONFIG_REG(HalDev->dev_base) = OamConfig;
     }
  }

/*
 *  This function allows the host software to access any register directly.
 *  Primarily used for debug.
 *  
 *  @param   HalDev      CPHAL module instance. (set by xxxInitModule())  
 *  @param   RegOffset   Hexadecimal offset to desired register (from device base addr)
 *
 *  @return  Volatile pointer to desired register.
 */
static volatile bit32u* halRegAccess(HAL_DEVICE *HalDev, bit32u RegOffset)
  {
   /* compute the register address */
   return ((volatile bit32u *)(HalDev->dev_base + RegOffset)); 
  }

#ifdef __CPHAL_DEBUG
static void dbgConfigDump(HAL_DEVICE *HalDev)
  {
   dbgPrintf("  AAL5 Inst %d Config Dump:\n", HalDev->Inst);
   dbgPrintf("    Base     :%08x, offset:%08d\n", 
             HalDev->dev_base, HalDev->offset);
   dbgPrintf("    Interrupt:%08d, debug :%08d\n", 
             HalDev->interrupt, HalDev->debug);
   osfuncSioFlush();
  }
#endif

/**
 *  @ingroup CPHAL_Functions
 *  Performs a variety of control functions on the CPHAL module.  It is used to
 *  modify/read configuration parameters and to initiate internal functions.
 *  The @p Key indicates the function to perform or the parameter to access (note 
 *  that these Keys are identical to those used in accessing the configuration data
 *  store).  @p Action is applicable to parameters only, and indicates what the
 *  CPHAL should do with the parameter (i.e. "Set", "Get", etc..).  The actions
 *  for each parameter are defined in the module specific documentation.
 *  
 *  @param   HalDev      CPHAL module instance. (set by xxxInitModule())
 *  @param   Key         Key specifying the parameter to change or internal function to initiate.  See module specific documentation for available keys.
 *  @param   Action      Specifies the action to take.  See module specific documentation for available actions.
 *  @param   Value       Pointer to new value for given @p Key parameter ("Set"), or returned value of Key ("Get").
 *  
 *  @return  EC_NO_ERRORS (ok).<BR>
 *           Possible Error Codes:<BR>
 *           @ref EC_VAL_INVALID_STATE "EC_VAL_INVALID_STATE"<BR>
 *           @ref EC_VAL_KEY_NOT_FOUND "EC_VAL_KEY_NOT_FOUND"<BR>
 *           @ref EC_VAL_ACTION_NOT_FOUND "EC_VAL_ACTION_NOT_FOUND"<BR>     
 */
static int halControl(HAL_DEVICE *HalDev, const char *Key, const char *Action, void *Value)
  {
   int Level, Ch, KeyFound=0, ActionFound=0, rc=EC_NO_ERRORS, Queue;
   char *TmpKey = (char *)Key;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]halControl(HalDev:%08x, Key:%s, Action:%s, Value:%08x)\n", (bit32u)HalDev,
                Key, Action, (bit32u)Value);
      osfuncSioFlush();
     }
#endif

   /* Verify proper device state */
   if (HalDev->State < enInitialized)
     return (EC_AAL5|EC_FUNC_CONTROL|EC_VAL_INVALID_STATE);   

   if (HalDev->OsFunc->Strcmpi(Key, "Debug") == 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         HalDev->debug = *(int *)Value;
         /* also setup debug variable in CPSAR module */
         rc = HalDev->SarFunc->Control(HalDev->SarDev, "Debug", "Set", Value);
        }
     }

   if (HalDev->OsFunc->Strstr(Key, "FwdUnkVc.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
  
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("FwdUnkVc.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         HalDev->ChData[Ch].FwdUnkVc = *(int *)Value;

         if ((HalDev->State == enOpened) && (HalDev->ChData[Ch].PktType == 3))
           rc = HalDev->SarFunc->Control(HalDev->SarDev, Key, Action, Value);         
        }
     }

   /* +GSG 030407 */
   if (HalDev->OsFunc->Strcmpi(Key, "OamMode") == 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         rc = HalDev->SarFunc->Control(HalDev->SarDev, Key, Action, Value);         
        }

      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
         rc = HalDev->SarFunc->Control(HalDev->SarDev, Key, Action, Value);
        }
     }

   /* +GSG 030307 */
   if (HalDev->OsFunc->Strcmpi(Key, "Version") == 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
         *(const char **)Value = pszVersion_CPAAL5;
        }
     }

   /* +GSG 030529 */
   if (HalDev->OsFunc->Strcmpi(Key, "TurboDslErrors") == 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
         *(int *)Value = HalDev->TurboDslErrors;
        }
     }
   
   /* +GSG 030416 */
   if (HalDev->OsFunc->Strcmpi(Key, "F4_LB_Counter") == 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
         *(int *)Value = SAR_PDSP_OAM_F4_LB_COUNT_REG(HalDev->dev_base);
        }
     }

   /* +GSG 030416 */
   if (HalDev->OsFunc->Strcmpi(Key, "F5_LB_Counter") == 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
         *(int *)Value = SAR_PDSP_OAM_F5_LB_COUNT_REG(HalDev->dev_base);
        }
     }
   
   if (HalDev->OsFunc->Strstr(Key, "Stats;") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
         TmpKey += HalDev->OsFunc->Strlen("Stats;");
         Level = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);
         TmpKey++;
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);
         TmpKey++;
         Queue = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);
         TmpKey++;
         StatsGet(HalDev, (void **)Value, Level, Ch, Queue);
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #100 */
   if (HalDev->OsFunc->Strstr(Key, "Gfc.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("Gfc.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* first, store new value in our channel structure */
         HalDev->ChData[Ch].Gfc = *(int *)Value;
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #100 */
   if (HalDev->OsFunc->Strstr(Key, "Clp.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("Clp.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* first, store new value in our channel structure */
         HalDev->ChData[Ch].Clp = *(int *)Value;
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #100 */
   if (HalDev->OsFunc->Strstr(Key, "Pti.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("Pti.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* first, store new value in our channel structure */
         HalDev->ChData[Ch].Pti = *(int *)Value;
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #100 */
   if (HalDev->OsFunc->Strstr(Key, "CpcsUU.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("CpcsUU.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* first, store new value in our channel structure */
         HalDev->ChData[Ch].CpcsUU = *(int *)Value;
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #100 */
   if (HalDev->OsFunc->Strstr(Key, "TxVc_CellRate.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("TxVc_CellRate.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* first, store new value in our channel structure */
         HalDev->ChData[Ch].TxVc_CellRate = *(int *)Value;

         /* now, apply to PDSP state RAM */
         if (HalDev->State == enOpened)
           *(pPDSP_AAL5_TX_STATE_WORD_0(HalDev->dev_base)+(Ch*64))= HalDev->ChData[Ch].TxVc_CellRate;
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #100 */
   if (HalDev->OsFunc->Strstr(Key, "TxVc_Mbs.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("TxVc_Mbs.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* first, store new value in our channel structure */
         HalDev->ChData[Ch].TxVc_Mbs = *(int *)Value;

         /* now, apply to PDSP state RAM */
         if (HalDev->State == enOpened)
           *(pPDSP_AAL5_TX_STATE_WORD_0(HalDev->dev_base)+(Ch*64)+2)= HalDev->ChData[Ch].TxVc_Mbs;
        }
     }

   if (HalDev->OsFunc->Strstr(Key, "TxVc_AtmHeader.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("TxVc_AtmHeader.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* first, store new value in our channel structure */
         HalDev->ChData[Ch].TxVc_AtmHeader = *(int *)Value;

         /* now, apply to PDSP state RAM */
         if (HalDev->State == enOpened)
           *(pPDSP_AAL5_TX_STATE_WORD_0(HalDev->dev_base)+(Ch*64)+6)= HalDev->ChData[Ch].TxVc_AtmHeader;
        }
     }

   if (HalDev->OsFunc->Strstr(Key, "TxVp_AtmHeader.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("TxVp_AtmHeader.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* first, store new value in our channel structure */
         HalDev->ChData[Ch].TxVp_AtmHeader = *(int *)Value;

         /* now, apply to PDSP state RAM */
         if (HalDev->State == enOpened)
           *(pPDSP_AAL5_TX_VP_STATE_WORD_0(HalDev->dev_base)+(Ch*64))= HalDev->ChData[Ch].TxVp_AtmHeader;
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #100 */
   if (HalDev->OsFunc->Strstr(Key, "TxVc_Pcr.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("TxVc_Pcr.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* first, store new value in our channel structure */
         HalDev->ChData[Ch].TxVc_Pcr = *(int *)Value;

         /* now, apply to PDSP state RAM */
         if (HalDev->State == enOpened)
           *(pPDSP_AAL5_TX_STATE_WORD_0(HalDev->dev_base)+(Ch*64)+4)= HalDev->ChData[Ch].TxVc_Pcr;
        }
     }

   /* +GSG 030428 */
   if (HalDev->OsFunc->Strstr(Key, "RxVc_OamCh.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("RxVc_OamCh.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* first, store new value in our channel structure */
         HalDev->ChData[Ch].RxVc_OamCh = (*(int *)Value) & 0xff;

         /* now, apply to PDSP state RAM */
         if (HalDev->State == enOpened)
           *(pPDSP_AAL5_RX_STATE_WORD_0(HalDev->dev_base)+(Ch*64)) |= HalDev->ChData[Ch].RxVc_OamCh;
        }
     }

   /* +GSG 030428 */
   if (HalDev->OsFunc->Strstr(Key, "RxVp_OamCh.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("RxVp_OamCh.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* first, store new value in our channel structure */
         HalDev->ChData[Ch].RxVp_OamCh = (*(int *)Value) & 0xff;

         /* now, apply to PDSP state RAM */
         if (HalDev->State == enOpened)
           *(pPDSP_AAL5_RX_VP_STATE_WORD_0(HalDev->dev_base)+(Ch*64)+1) |= HalDev->ChData[Ch].RxVp_OamCh;
        }
     }

   /* +GSG 030304 */
   /* Fixes PITS #98 */
   if (HalDev->OsFunc->Strstr(Key, "PdspEnable") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         
         /* this variable is controlled by the CPSAR module */
         if (HalDev->State == enOpened)
           {
            rc=HalDev->SarFunc->Control(HalDev->SarDev, "PdspEnable", "Set", Value);
           }
        }
     }

   if (HalDev->OsFunc->Strstr(Key, "OamLbTimeout") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
         
         HalDev->OamLbTimeout = *(int *)Value;
         /* this variable is controlled by the CPSAR module */
         if (HalDev->State == enOpened)
           {
            *(pOAM_TIMER_STATE_WORD_0(HalDev->dev_base) + 1) = 
              ((HalDev->SarFrequency/1000) * HalDev->OamLbTimeout);
           }
        }
     }

   /* +GSG 030306 (PITS #114) */
   if (HalDev->OsFunc->Strstr(Key, "DeviceCPID") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         unsigned int* local = (unsigned int *)Value;
         ActionFound=1;
         /* first, store new value in our hal structure */
         HalDev->DeviceCPID[0] = local[0];
         HalDev->DeviceCPID[1] = local[1];
         HalDev->DeviceCPID[2] = local[2];
         HalDev->DeviceCPID[3] = local[3];

         /* now, apply to PDSP state RAM */
         if (HalDev->State == enOpened)
           {
            *(bit32u *)(pOAM_CONFIG_BLOCK_WORD_0(HalDev->dev_base) + 0) = HalDev->DeviceCPID[0];
            *(bit32u *)(pOAM_CONFIG_BLOCK_WORD_0(HalDev->dev_base) + 1) = HalDev->DeviceCPID[1];
            *(bit32u *)(pOAM_CONFIG_BLOCK_WORD_0(HalDev->dev_base) + 2) = HalDev->DeviceCPID[2];
            *(bit32u *)(pOAM_CONFIG_BLOCK_WORD_0(HalDev->dev_base) + 3) = HalDev->DeviceCPID[3];
           }
        }
     }

   /* +GSG 030304 */
   /* Fixes PITS #99 */
   if (HalDev->OsFunc->Strstr(Key, "StrictPriority") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
        
         /* used in halOpen to decide which interrupt handler to use */
         HalDev->StrictPriority = *(int *)Value;
        }
     }

   if (HalDev->OsFunc->Strstr(Key, hcMaxFrags) != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
        
         if ((*(int *)Value) > 0)
           HalDev->MaxFrags = *(int *)Value;
          else
           rc = (EC_AAL5|EC_FUNC_CONTROL|EC_VAL_INVALID_VALUE);
        }

      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;

         *(int *)Value = HalDev->MaxFrags;
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #103 */
   if (HalDev->OsFunc->Strstr(Key, "RxVc_RDICount.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
        
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("RxVc_RDICount.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* PDSP's Rx VC State word 3 contains the value */
         if (HalDev->State == enOpened)
           {
            *(int *)Value = (((*(pPDSP_AAL5_RX_STATE_WORD_0(HalDev->dev_base)+(Ch*64))) & RDI_CNT_MASK)>>RDI_CNT_SHIFT);
           }
        }
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
        
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("RxVc_RDICount.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* All sets write 0, this action is a clear only */
         if (HalDev->State == enOpened)
           {
            (*(pPDSP_AAL5_RX_STATE_WORD_0(HalDev->dev_base)+(Ch*64))) &=~ RDI_CNT_MASK;
           }
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #103 */
   if (HalDev->OsFunc->Strstr(Key, "RxVc_AISseg.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
        
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("RxVc_AISseg.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* PDSP's Rx VC State word 3 contains the value */
         if (HalDev->State == enOpened)
           {
            *(int *)Value = (((*(pPDSP_AAL5_RX_STATE_WORD_0(HalDev->dev_base)+(Ch*64)+3)) & AIS_SEG_MASK)>>AIS_SEG_SHIFT);
           }
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #103 */
   if (HalDev->OsFunc->Strstr(Key, "RxVc_AISetoe.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
        
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("RxVc_AISetoe.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* PDSP's Rx VC State word 3 contains the value */
         if (HalDev->State == enOpened)
           {
            *(int *)Value = (((*(pPDSP_AAL5_RX_STATE_WORD_0(HalDev->dev_base)+(Ch*64)+3)) & AIS_ETOE_MASK)>>AIS_ETOE_SHIFT);
           }
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #103 */
   if (HalDev->OsFunc->Strstr(Key, "RxVp_RDICount.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
        
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("RxVp_RDICount.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* PDSP's Rx VC State word 3 contains the value */
         if (HalDev->State == enOpened)
           {
            *(int *)Value = (((*(pPDSP_AAL5_RX_VP_STATE_WORD_0(HalDev->dev_base)+(Ch*64)+1)) & RDI_CNT_MASK)>>RDI_CNT_SHIFT);
           }
        }
      if (HalDev->OsFunc->Strcmpi(Action, "Set") == 0)
        {
         ActionFound=1;
        
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("RxVp_RDICount.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* All sets write 0, this action is a clear only */
         if (HalDev->State == enOpened)
           {
            (*(pPDSP_AAL5_RX_VP_STATE_WORD_0(HalDev->dev_base)+(Ch*64)+1)) &=~ RDI_CNT_MASK;
           }
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #103 */
   if (HalDev->OsFunc->Strstr(Key, "RxVp_AISseg.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
        
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("RxVp_AISseg.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* PDSP's Rx VC State word 3 contains the value */
         if (HalDev->State == enOpened)
           {
            *(int *)Value = (((*(pPDSP_AAL5_RX_VP_STATE_WORD_0(HalDev->dev_base)+(Ch*64)+2)) & AIS_SEG_MASK)>>AIS_SEG_SHIFT);
           }
        }
     }

   /* +GSG 030306 */
   /* Fixes PITS #103 */
   if (HalDev->OsFunc->Strstr(Key, "RxVp_AISetoe.") != 0)
     {
      KeyFound=1;
      if (HalDev->OsFunc->Strcmpi(Action, "Get") == 0)
        {
         ActionFound=1;
        
         /* extract channel number */
         TmpKey += HalDev->OsFunc->Strlen("RxVp_AISetoe.");
         Ch = HalDev->OsFunc->Strtoul(TmpKey, &TmpKey, 10);

         /* PDSP's Rx VC State word 3 contains the value */
         if (HalDev->State == enOpened)
           {
            *(int *)Value = (((*(pPDSP_AAL5_RX_VP_STATE_WORD_0(HalDev->dev_base)+(Ch*64)+2)) & AIS_ETOE_MASK)>>AIS_ETOE_SHIFT);
           }
        }
     }

   if (KeyFound == 0)
     rc = (EC_AAL5|EC_FUNC_CONTROL|EC_VAL_KEY_NOT_FOUND);

   if (ActionFound == 0)
     rc = (EC_AAL5|EC_FUNC_CONTROL|EC_VAL_ACTION_NOT_FOUND);

   return(rc);    
  }

/*
 * Sets up HAL default configuration parameter values.
 */
static void ConfigInit(HAL_DEVICE *HalDev)
  {
#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]ConfigInit(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   /* configure some defaults with tnetx7300 values */
   HalDev->dev_base = 0xa3000000;
   HalDev->offset = 0;
   HalDev->interrupt = 15;
   HalDev->debug = 0;
   HalDev->MaxFrags = 46;
   HalDev->OamLbTimeout = 5000;
  }

/*
 * Retrieve HAL configuration parameter values.
 */
static bit32u ConfigGet(HAL_DEVICE *HalDev)
  {
   bit32u Ret;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]ConfigGet(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   /* get the configuration parameters common to all modules */
   Ret = ConfigGetCommon(HalDev);
   if (Ret) return (EC_AAL5|Ret);

   /* get AAL5 specific configuration parameters here */
   Ret = HalDev->OsFunc->Control(HalDev->OsDev, hcSarFrequency, pszGET, &HalDev->SarFrequency); /* GSG +030416*/
   if (Ret)                                                                                /* GSG +030416*/
     HalDev->SarFrequency = 200000000;  /* 200 Mhz default */                              /* GSG +030416*/

   return (EC_NO_ERRORS);
  }

/** 
 *  @ingroup CPHAL_Functions
 *  This function initializes the CPHAL module. It gathers all
 *  necessary global configuration info from the configuration file, and
 *  performs initialization and configuration of the device.  Note that
 *  the device operation is not started until the OS calls @c Open().
 *
 *  @param   HalDev   CPHAL module instance. (set by xxxInitModule())
 *
 *  @return  EC_NO_ERRORS (ok). <BR>
 *           Possible Error Codes:<BR>
 *           @ref EC_VAL_INVALID_STATE "EC_VAL_INVALID_STATE"<BR>
 *           @ref EC_VAL_BASE_ADDR_NOT_FOUND "EC_VAL_BASE_ADDR_NOT_FOUND"<BR>
 *           @ref EC_VAL_RESET_BIT_NOT_FOUND "EC_VAL_RESET_BIT_NOT_FOUND"<BR>
 *           @ref EC_VAL_INTERRUPT_NOT_FOUND "EC_VAL_INTERRUPT_NOT_FOUND"<BR>
 *           @ref EC_VAL_OFFSET_NOT_FOUND "EC_VAL_OFFSET_NOT_FOUND"<BR>
 */
static int halInit(HAL_DEVICE *HalDev)
  {
  int i;
  bit32u error_code;

#ifdef __CPHAL_DEBUG
  if (DBG(0))
    {
     dbgPrintf("[aal5]halInit(HalDev:%08x)\n", (bit32u)HalDev);
     osfuncSioFlush();
    }
#endif

  /* Verify proper device state */
  if (HalDev->State != enDevFound)
    return(EC_AAL5|EC_FUNC_HAL_INIT|EC_VAL_INVALID_STATE);

  /* Configure HAL defaults */
  ConfigInit(HalDev);

  /* Retrieve HAL configuration parameters from data store */
  error_code = ConfigGet(HalDev);
  if (error_code) return (error_code);
 
  /* Other items (OAM related) that need to be passed in somehow */
  HalDev->DeviceCPID[0] = 0xffffffff;
  HalDev->DeviceCPID[1] = 0xffffffff;
  HalDev->DeviceCPID[2] = 0xffffffff;
  HalDev->DeviceCPID[3] = 0xffffffff;
  HalDev->LBSourceLLID[0] = 0xffffffff;
  HalDev->LBSourceLLID[1] = 0xffffffff;
  HalDev->LBSourceLLID[2] = 0xffffffff;
  HalDev->LBSourceLLID[3] = 0xffffffff;

  /* Initialize SAR layer*/
  error_code = HalDev->SarFunc->Init(HalDev->SarDev);
  if (error_code) return (error_code);

  /* Initialize various HalDev members.  This is probably overkill, since these
     are initialized in ChannelSetup() and HalDev is cleared in InitModule(). */
  for (i=0; i<NUM_AAL5_CHAN; i++)
    { 
     HalDev->InRxInt[i]=FALSE;
     HalDev->ChIsOpen[i][DIRECTION_TX] = FALSE;
     HalDev->ChIsOpen[i][DIRECTION_RX] = FALSE;
     HalDev->TcbStart[i][0] = 0;
     HalDev->TcbStart[i][1] = 0;
     HalDev->RcbStart[i]  = 0;
    }

  /* initialize SAR stats */
  StatsClear(HalDev);

  /* init Stat pointers */
 
  /* even though these statistics may be for multiple channels/queues, i need 
     only configure the pointer to the beginning of the array, and I can index 
     from there if necessary */
  StatsTable0[0].StatPtr  = &HalDev->Stats.CrcErrors[0];
  StatsTable0[1].StatPtr  = &HalDev->Stats.LenErrors[0];
  StatsTable0[2].StatPtr  = &HalDev->Stats.AbortErrors[0];
  StatsTable0[3].StatPtr  = &HalDev->Stats.StarvErrors[0];

  StatsTable1[0].StatPtr  = &HalDev->Stats.DmaLenErrors[0];
  StatsTable1[1].StatPtr  = &HalDev->Stats.TxMisQCnt[0][0];
  StatsTable1[2].StatPtr  = &HalDev->Stats.RxMisQCnt[0];
  StatsTable1[3].StatPtr  = &HalDev->Stats.TxEOQCnt[0][0];
  StatsTable1[4].StatPtr  = &HalDev->Stats.RxEOQCnt[0];
  StatsTable1[5].StatPtr  = &HalDev->Stats.RxPacketsServiced[0];
  StatsTable1[6].StatPtr  = &HalDev->Stats.TxPacketsServiced[0][0];
  StatsTable1[7].StatPtr  = &HalDev->Stats.RxMaxServiced;
  StatsTable1[8].StatPtr  = &HalDev->Stats.TxMaxServiced[0][0];
  StatsTable1[9].StatPtr  = &HalDev->Stats.RxTotal;
  StatsTable1[10].StatPtr = &HalDev->Stats.TxTotal;

  StatsTable2[0].StatPtr  = (bit32u *)&HalDev->RcbPool[0];
  StatsTable2[1].StatPtr  = &HalDev->RxActQueueCount[0];
  StatsTable2[2].StatPtr  = (bit32u *)&HalDev->RxActQueueHead[0];
  StatsTable2[3].StatPtr  = (bit32u *)&HalDev->RxActQueueTail[0];
  StatsTable2[4].StatPtr  = &HalDev->RxActive[0];
  StatsTable2[5].StatPtr  = (bit32u *)&HalDev->RcbStart[0];
  StatsTable2[6].StatPtr  = &HalDev->RxTeardownPending[0];
  StatsTable2[7].StatPtr  = (bit32u *)&HalDev->TcbPool[0][0];
  StatsTable2[8].StatPtr  = &HalDev->TxActQueueCount[0][0];
  StatsTable2[9].StatPtr  = (bit32u *)&HalDev->TxActQueueHead[0][0];
  StatsTable2[10].StatPtr = (bit32u *)&HalDev->TxActQueueTail[0][0];
  StatsTable2[11].StatPtr = &HalDev->TxActive[0][0];
  StatsTable2[12].StatPtr = (bit32u *)&HalDev->TcbStart[0][0];
  StatsTable2[13].StatPtr = &HalDev->TxTeardownPending[0];

  StatsTable4[0].StatPtr  = &HalDev->dev_base;
  StatsTable4[1].StatPtr  = &HalDev->offset;
  StatsTable4[2].StatPtr  = &HalDev->interrupt;
  StatsTable4[3].StatPtr  = &HalDev->debug;
  StatsTable4[4].StatPtr  = &HalDev->Inst;
 
  StatsTable3[0].StatPtr  = &HalDev->ChData[0].RxBufSize;
  StatsTable3[1].StatPtr  = &HalDev->ChData[0].RxBufferOffset;
  StatsTable3[2].StatPtr  = &HalDev->ChData[0].RxNumBuffers;
  StatsTable3[3].StatPtr  = &HalDev->ChData[0].RxServiceMax;
  StatsTable3[4].StatPtr  = &HalDev->ChData[0].TxNumBuffers;
  StatsTable3[5].StatPtr  = &HalDev->ChData[0].TxNumQueues;
  StatsTable3[6].StatPtr  = &HalDev->ChData[0].TxServiceMax;
  StatsTable3[7].StatPtr  = &HalDev->ChData[0].CpcsUU;
  StatsTable3[8].StatPtr  = &HalDev->ChData[0].Gfc;
  StatsTable3[9].StatPtr  = &HalDev->ChData[0].Clp;
  StatsTable3[10].StatPtr = &HalDev->ChData[0].Pti;
  StatsTable3[11].StatPtr = &HalDev->ChData[0].DaMask;
  StatsTable3[12].StatPtr = &HalDev->ChData[0].Priority;
  StatsTable3[13].StatPtr = &HalDev->ChData[0].PktType;
  StatsTable3[14].StatPtr = &HalDev->ChData[0].Vci;
  StatsTable3[15].StatPtr = &HalDev->ChData[0].Vpi;
  StatsTable3[16].StatPtr = &HalDev->ChData[0].TxVc_CellRate;
  StatsTable3[17].StatPtr = &HalDev->ChData[0].TxVc_QosType;
  StatsTable3[18].StatPtr = &HalDev->ChData[0].TxVc_Mbs;
  StatsTable3[19].StatPtr = &HalDev->ChData[0].TxVc_Pcr;

  /* update device state */
  HalDev->State = enInitialized;

#ifdef __CPHAL_DEBUG
  if (DBG(9))
    dbgConfigDump(HalDev);
#endif

  return(EC_NO_ERRORS);
  }

/*
 *  Use this function to actually send after queuing multiple packets using 
 *  Send().  This is a debug only function that should be removed - it was 
 *  necessary to properly implement my loopback tests. 
 *
 *  @param   HalDev      CPHAL module instance. (set by xxxInitModule()) 
 *  @param   Queue       Queue number to kick. 
 *
 *  @return  0 OK, Non-Zero Not OK
 */
static int halKick(HAL_DEVICE *HalDev, int Queue)
  {
   int Ch;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]halKick(HalDev:%08x. Queue:%d)\n", (bit32u)HalDev, Queue);
      osfuncSioFlush();
     }
#endif

   for (Ch = 0; Ch < 16; Ch ++)
     {
      if ((!HalDev->TxActive[Ch][Queue]) && (HalDev->TxActQueueHead[Ch][Queue] != 0))
        {
         *(pTX_DMA_STATE_WORD_0(HalDev->dev_base)+(Ch*64)+Queue)=
           VirtToPhys(HalDev->TxActQueueHead[Ch][Queue]);
         HalDev->TxActive[Ch][Queue]=TRUE;
        }
     } 
 
   return (EC_NO_ERRORS);   
  }

/*  +GSG 030305  For PITS #99
 *  Alternate interrupt handler that uses the INT_VECTOR in order to 
 *  provide strict priority handling among channels, beginning with Ch 0.
 *
 *  @param   HalDev   CPHAL module instance. (set by xxxInitModule())
 *  @param   MoreWork (Output) When set to 1, indicates that there is more work to do.
 *                    Caller should ensure that the value pointed at is set to 0
 *                    prior to the call.
 *  @return  0 OK, non-zero error.
 */
static int DeviceIntAlt(HAL_DEVICE *HalDev, int *MoreWork)
  {
   int tmp, Ch, WorkFlag;
   bit32u rc;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]DeviceIntAlt(HalDev:%08x, MoreWork:%08x)\n", (bit32u)HalDev, (bit32u)MoreWork);
      osfuncSioFlush();
     }
#endif

   /* Verify proper device state - important because a call prior to Open would
      result in a lockup */
   if (HalDev->State != enOpened)
     return(EC_AAL5|EC_FUNC_DEVICE_INT_ALT|EC_VAL_INVALID_STATE);

   if ((tmp=SAR_INTR_VECTOR(HalDev->dev_base))&INT_PENDING)
     {
       /*printf("\015 %d RxQ",HalDev->RxActQueueCount[0]);
         HalDev->OsFunc->Control(HalDev->OsDev, enSIO_FLUSH, enNULL, 0); */

      if (tmp&TXH_PEND) 
        {
         /* decide which channel to service */
         Ch = (SAR_INTR_VECTOR(HalDev->dev_base) & TXH_PEND_INVEC);

         rc = TxInt(HalDev,Ch,0,&WorkFlag);
         if (rc) return (rc);

         if (WorkFlag == 1)
           *MoreWork = 1;
        }

      if (tmp&TXL_PEND) 
        {
         /* decide which channel to service */
         Ch = ((SAR_INTR_VECTOR(HalDev->dev_base) & TXL_PEND_INVEC) >> 4);

         rc = TxInt(HalDev,Ch,1,&WorkFlag);
         if (rc) return (rc);

         if (WorkFlag == 1)
           *MoreWork = 1;
        }

      if (tmp&RX_PEND)
        {
         /* decide which channel to service */
         Ch = ((SAR_INTR_VECTOR(HalDev->dev_base) & RX_PEND_INVEC) >> 8);

         rc = RxInt(HalDev,Ch,&WorkFlag);
         if (rc) return (rc);

         if (WorkFlag == 1)
           *MoreWork = 1;
        }
       
      if (tmp&STS_PEND)
        {
         /* GPR 2 code added for PITS 103 */
         /* determine interrupt source */
         Ch = ((SAR_INTR_VECTOR(HalDev->dev_base) & STS_PEND_INVEC) >> 12);

         /* only if this is GPR 2 interrupt do we take action */
         if (Ch == 26)
           {
            /* pass loopback result back to OS */
            HalDev->OsFunc->Control(HalDev->OsDev, "OamLbResult", "Set",
              (bit32u *)pSAR_PDSP_OAM_LB_RESULT_REG(HalDev->dev_base)); 
           }

         /* clear the interrupt */
         SAR_STATUS_CLR_REG(HalDev->dev_base) |= 0x04000000;
        }

      if (tmp&AAL2_PEND)
        { 
          /* no action defined */
        }
    
      SAR_INTR_VECTOR(HalDev->dev_base) = 0;
     }

   return (EC_NO_ERRORS);
  }

/*
 *  Called to service a module interrupt.  This function determines
 *  what type of interrupt occurred and dispatches the correct handler.
 *
 *  @param   HalDev   CPHAL module instance. (set by xxxInitModule())
 *  @param   MoreWork (Output) When set to 1, indicates that there is more work to do.
 *                    Caller should ensure that the value pointed at is set to 0
 *                    prior to the call.
 *  @return  0 OK, non-zero error.
 */
static int DeviceInt(HAL_DEVICE *HalDev, int *MoreWork)
  {
    /*static int NextRxCh=0;
      static int NextTxCh[2]={0,0};*/

   int tmp, Ch, FirstCh, WorkFlag;
   int NextTxLCh, NextTxHCh, NextRxCh;
   bit32u rc;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]DeviceInt(HalDev:%08x, MoreWork:%08x)\n", (bit32u)HalDev, (bit32u)MoreWork);
      osfuncSioFlush();
     }
#endif

   /* Verify proper device state - important because a call prior to Open would
      result in a lockup */
   if (HalDev->State != enOpened)
     return(EC_AAL5|EC_FUNC_DEVICE_INT|EC_VAL_INVALID_STATE);

   NextTxHCh = HalDev->NextTxCh[0];
   NextTxLCh = HalDev->NextTxCh[1];
   NextRxCh =  HalDev->NextRxCh;

   /* service interrupts while there is more work to do */
   /*while (((tmp=SAR_INTR_VECTOR(HalDev->dev_base))&INT_PENDING) && (TotalPkts < 500))*/
   if ((tmp=SAR_INTR_VECTOR(HalDev->dev_base))&INT_PENDING)
     {
       /*printf("\015 %d RxQ",HalDev->RxActQueueCount[0]);
         HalDev->OsFunc->Control(HalDev->OsDev, enSIO_FLUSH, enNULL, 0); */

      if (tmp&TXH_PEND) 
        {
         /* decide which channel to service */
         FirstCh = NextTxHCh;
         while (1)
           {
            Ch = NextTxHCh++;
            if (NextTxHCh == 16)
              NextTxHCh = 0;
            if (SAR_TX_MASKED_STATUS(HalDev->dev_base) & (1<<Ch))
              break;
            if (FirstCh == NextTxHCh)
              {
               /* we checked every channel and still haven't found anything to do */
               return (EC_AAL5|EC_FUNC_DEVICE_INT|EC_VAL_NO_TXH_WORK_TO_DO);
              }
           }
 
         rc = TxInt(HalDev,Ch,0,&WorkFlag);
         if (rc) return (rc);

         if (WorkFlag == 1)
           *MoreWork = 1;
        }

      if (tmp&TXL_PEND)
        {
         /* decide which channel to service */
         FirstCh = NextTxLCh;
         while (1)
           {
            Ch = NextTxLCh++;
            if (NextTxLCh == 16)
              NextTxLCh = 0;
            if (SAR_TX_MASKED_STATUS(HalDev->dev_base) & (1<<(Ch+16)))
              break;
            if (FirstCh == NextTxLCh)
              {
               /* we checked every channel and still haven't found anything to do */
               return (EC_AAL5|EC_FUNC_DEVICE_INT|EC_VAL_NO_TXL_WORK_TO_DO);
              }
           }

         rc = TxInt(HalDev,Ch,1,&WorkFlag);
         if (rc) return (rc);

         if (WorkFlag == 1)
           *MoreWork = 1;       
        }
         
      if (tmp&RX_PEND)
        {
         FirstCh = NextRxCh;
         while (1)
           {
            Ch = NextRxCh++;   
            if (NextRxCh == 16)
              NextRxCh = 0;
            if (SAR_RX_MASKED_STATUS(HalDev->dev_base) & (1 << Ch))
              break;  /* found a channel to service */
            if (FirstCh == NextRxCh)
              {
               /* we checked every channel and still haven't found anything to do */
               return (EC_AAL5|EC_FUNC_DEVICE_INT|EC_VAL_NO_RX_WORK_TO_DO);
              }
           }

         rc = RxInt(HalDev,Ch, &WorkFlag);
         if (rc) return (rc);

         if (WorkFlag == 1)
           *MoreWork = 1;
        }
       
      if (tmp&STS_PEND)
        {
         /* +GSG 030305 */
         /* GPR 2 code added for PITS 103 */
         /* determine interrupt source */
         Ch = ((SAR_INTR_VECTOR(HalDev->dev_base) & STS_PEND_INVEC) >> 12);

         /* only if this is GPR 2 interrupt do we take action */
         if (Ch == 26)
           {
            /* pass loopback result back to OS */
            HalDev->OsFunc->Control(HalDev->OsDev, "OamLbResult", "Set",
              (bit32u *)pSAR_PDSP_OAM_LB_RESULT_REG(HalDev->dev_base)); 
           }

         /* clear the interrupt */
         SAR_STATUS_CLR_REG(HalDev->dev_base) |= 0x04000000;
        }

      if (tmp&AAL2_PEND)
        { 
          /* no action defined */
        }
    
      SAR_INTR_VECTOR(HalDev->dev_base) = 0;
     }

   HalDev->NextTxCh[0] = NextTxHCh;
   HalDev->NextTxCh[1] = NextTxLCh;
   HalDev->NextRxCh = NextRxCh;

   /* This must be done by the upper layer */
   /* SAR_EOI(HalDev->dev_base) = 0; */

   return (EC_NO_ERRORS);
  }

/** 
 *  @ingroup CPHAL_Functions
 *  This function starts the operation of the CPHAL device.  It takes the device
 *  out of reset, and calls @c IsrRegister().  This function should be called after 
 *  calling the @c Init() function.
 *
 *  @param  HalDev   CPHAL module instance. (set by xxxInitModule())
 *
 *  @return EC_NO_ERRORS (ok).<BR>
 *           Possible Error Codes:<BR>
 *           @ref EC_VAL_INVALID_STATE "EC_VAL_INVALID_STATE"<BR>
 *           @ref EC_VAL_KEY_NOT_FOUND "EC_VAL_KEY_NOT_FOUND"<BR>
 *           @ref EC_VAL_FIRMWARE_TOO_LARGE "EC_VAL_FIRMWARE_TOO_LARGE"<BR>
 *           @ref EC_VAL_PDSP_LOAD_FAIL "EC_VAL_PDSP_LOAD_FAIL"<BR>
 *           @ref EC_VAL_RX_STATE_RAM_NOT_CLEARED "EC_VAL_RX_STATE_RAM_NOT_CLEARED"<BR>
 *           @ref EC_VAL_TX_STATE_RAM_NOT_CLEARED "EC_VAL_TX_STATE_RAM_NOT_CLEARED"<BR>
 *           @ref EC_VAL_TCB_MALLOC_FAILED "EC_VAL_TCB_MALLOC_FAILED"<BR>
 *           @ref EC_VAL_RCB_MALLOC_FAILED "EC_VAL_RCB_MALLOC_FAILED"<BR>
 *           @ref EC_VAL_RX_BUFFER_MALLOC_FAILED "EC_VAL_RX_BUFFER_MALLOC_FAILED"<BR>
 *           @ref EC_VAL_LUT_NOT_READY "EC_VAL_LUT_NOT_READY"<BR>
 */
static int halOpen(HAL_DEVICE *HalDev)
  {
   int i,Ret;
   bit32 SarBase = HalDev->dev_base;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]halOpen(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   /* Verify proper device state */
   if (HalDev->State < enInitialized)
     return (EC_AAL5|EC_FUNC_OPEN|EC_VAL_INVALID_STATE);

   /* Open the SAR (this brings the whole device out of reset */
   Ret = HalDev->SarFunc->Open(HalDev->SarDev); /* ~GSG 030410 */
   if (Ret)                                     /* +GSG 030410 */
     return (Ret);                              /* +GSG 030410 */

   /* Change device state */
   HalDev->State = enOpened;


#ifdef __CPHAL_DEBUG
   /* print out the version information */
   if (DBG(7))
     {
      dbgPrintf("[aal5 halOpen()]Module ID(AAL5-CPSAR):%d, Version:%2d.%02d\n",
                (SAR_ID_REG(SarBase)&0xffff0000)>>16,
                (SAR_ID_REG(SarBase)&0xff00)>>8,
                SAR_ID_REG(SarBase)&0xff);
      osfuncSioFlush();
     }
#endif

   /* GREG 11/1/02:  The State RAM clearing code was previously in cpsar.c,
      directly after device reset.  I moved it here because I believe it is
      AAL5 specific code.  Also the MAX_CHAN was set to 19 in cpsar.c, which
      would have caused this code to clear too much memory! */

   /* NOTE:  State RAM must be cleared prior to initializing the PDSP!! */

   /* GSG 030416:  Removed all of this.  All PDSP State RAM is cleared
      in CPSAR Open().  On Close(), all channels are torndown, thus all
      AAL5 channel state RAM is cleared.  */

   /* Clear Rx State RAM  */
   /*for (i=0; i<NUM_AAL5_CHAN; i++)
     for (j=0; j<NUM_RX_STATE_WORDS; j++)
     *(pRX_DMA_STATE_WORD_0(SarBase) + (i*64) + j) = 0; */

   /* Check that Rx DMA State RAM was cleared */
   /*for (i=0; i<NUM_AAL5_CHAN; i++)
     for (j=0; j<NUM_RX_STATE_WORDS; j++)
       {
        if (*(pRX_DMA_STATE_WORD_0(SarBase) + (i*64) + j) != 0)
         {
          return(EC_AAL5|EC_FUNC_HAL_INIT|EC_VAL_RX_STATE_RAM_NOT_CLEARED);
         }
         }*/

   /* Clear Tx State RAM  */
   /*for (i=0; i<NUM_AAL5_CHAN; i++)
     for (j=0; j<NUM_TX_STATE_WORDS; j++)
       {
        *(pTX_DMA_STATE_WORD_0(SarBase) + (i*64) + j) = 0; 
        }*/

   /* Check that Tx DMA State RAM was cleared */
   /*for (i=0; i<NUM_AAL5_CHAN; i++)
     for (j=0; j<NUM_TX_STATE_WORDS; j++)
       {
        if (*(pTX_DMA_STATE_WORD_0(SarBase) + (i*64) + j) != 0)
          {
           return(EC_AAL5|EC_FUNC_HAL_INIT|EC_VAL_TX_STATE_RAM_NOT_CLEARED);
          }
          }*/

   /* GSG +030523 Malloc space for the Rx fraglist */
   HalDev->fraglist = HalDev->OsFunc->Malloc(HalDev->MaxFrags * sizeof(FRAGLIST));

   /* For any channels that have been pre-initialized, set them up now */
   for (i=0; i<NUM_AAL5_CHAN; i++)
     {
      if ((HalDev->ChIsSetup[i][0]==TRUE) && (HalDev->ChIsOpen[i][0]==FALSE))
        {
         CHANNEL_INFO HalChn;
         HalChn.Channel = i;
         Ret = ChannelConfigApply(HalDev, &HalChn);
         if (Ret) return (Ret);
        }
     }

   /* OAM code would be a candidate to go into ConfigApply */

   /* Configure OAM Timer State Block */
   OamRateConfig(HalDev); /* +GSG 030416 */

   /* Setup OAM Configuration Block */
   for (i=0; i<8; i++) /* ~GSG 030603 4->8 */
     {
      if (i < 4)
        *(pOAM_CONFIG_BLOCK_WORD_0(SarBase) + i) = HalDev->DeviceCPID[i];
      else
        *(pOAM_CONFIG_BLOCK_WORD_0(SarBase) + i) = HalDev->LBSourceLLID[i-4];
     }

   /* Setup OAM Padding Block */
   for (i=0; i<12; i++)
     {
      *(pOAM_PADDING_BLOCK_WORD_0(SarBase) + i) = ((i==11)?0x6a6a0000:0x6a6a6a6a);    
     }

   /* Enable Tx CPPI DMA */
   TX_CPPI_CTL_REG(HalDev->dev_base) = 1;
   
   /* Enable Rx CPPI DMA */
   RX_CPPI_CTL_REG(HalDev->dev_base) = 1;

   /* +GSG 030306 */
   /* Fix for PITS 103 */
   /* Enable Host Interrupt for GPR 2 (OAM LB result register) */
   SAR_HOST_INT_EN_SET_REG(HalDev->dev_base) |= 0x04000000;
   
   /* +GSG 030304 to fix PITS 99 (if block is new)*/
   if (HalDev->StrictPriority == 1)
     {
#ifdef __CPHAL_DEBUG
      if (DBG(1))
        {
         dbgPrintf("[aal5->os]IsrRegister(OsDev:%08x, halIsr:%08x, Interrupt:%d)\n",
                   (bit32u)HalDev->OsDev, (bit32u)DeviceIntAlt, HalDev->interrupt);
         osfuncSioFlush();
        }
#endif

      /* "register" the interrupt handler */
      HalDev->OsFunc->IsrRegister(HalDev->OsDev, DeviceIntAlt, HalDev->interrupt);
     }
    else /* +GSG 030306 */
     { /* +GSG 030306 */
#ifdef __CPHAL_DEBUG
      if (DBG(1))
        {
         dbgPrintf("[aal5->os]IsrRegister(OsDev:%08x, halIsr:%08x, Interrupt:%d)\n",
                   (bit32u)HalDev->OsDev, (bit32u)DeviceInt, HalDev->interrupt);
         osfuncSioFlush();
        }
#endif

      /* "register" the interrupt handler */
      HalDev->OsFunc->IsrRegister(HalDev->OsDev, DeviceInt, HalDev->interrupt);
     } /* +GSG 030306 */

   return(EC_NO_ERRORS);
  }

/**
 *  @ingroup CPHAL_Functions
 *  Called to retrigger the interrupt mechanism after packets have been
 *  processed.  Call this function when the HalISR function indicates that
 *  there is no more work to do.  Proper use of this function will guarantee 
 *  that interrupts are never missed.  
 *
 *  @param  HalDev   CPHAL module instance. (set by xxxInitModule())  
 *
 *  @return EC_NO_ERRORS (ok). <BR>
 */
static int halPacketProcessEnd(HAL_DEVICE *HalDev)
  {
#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]halPacketProcessEnd(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   SAR_EOI(HalDev->dev_base) = 0;
   return (EC_NO_ERRORS);
  }

/**
 *  @ingroup CPHAL_Functions
 *  This function probes for the instance of the CPHAL module.  It will call
 *  the OS function @c DeviceFindInfo() to get the information required.
 *  
 *  @param   HalDev      CPHAL module instance. (set by xxxInitModule())
 *
 *  @return  EC_NO_ERRORS (ok). <BR>
 *           Possible Error Codes:<BR>
 *           @ref EC_VAL_INVALID_STATE "EC_VAL_INVALID_STATE"<BR>
 *           @ref EC_VAL_DEVICE_NOT_FOUND "EC_VAL_DEVICE_NOT_FOUND"<BR>
 */
static int halProbe(HAL_DEVICE *HalDev)
  {
   int Ret;

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]halProbe(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   /* Verify hardware state is "enConnected */
   if (HalDev->State != enConnected)
     return (EC_AAL5|EC_FUNC_PROBE|EC_VAL_INVALID_STATE);

#ifdef __CPHAL_DEBUG
   if (DBG(1))
     {
      dbgPrintf("[aal5->os]DeviceFindInfo(Inst:%d, DeviceName:%s, DeviceInfo:%08x)\n",
                HalDev->Inst, "aal5", (bit32u)&HalDev->DeviceInfo);
      osfuncSioFlush();
     }
#endif

   /* Attempt to find the device information */
   Ret = HalDev->OsFunc->DeviceFindInfo(HalDev->Inst, "aal5", &HalDev->DeviceInfo);
   if (Ret)
     return(EC_AAL5|EC_FUNC_PROBE|EC_VAL_DEVICE_NOT_FOUND);

   /* Call Probe for supporting CPSAR layer */
   Ret = HalDev->SarFunc->Probe(HalDev->SarDev);
   if (Ret)
     return(Ret);

   /* Set device state to DevFound */
   HalDev->State = enDevFound;

   return(EC_NO_ERRORS);    
  }

/** 
 *  @ingroup CPHAL_Functions
 *  This function shuts down the CPHAL module completely.  The caller must call
 *  Close() to put the device in reset prior shutting down.  This call will free
 *  the HalDev and the HAL function pointer structure, effectively ending
 *  communications between the driver and the CPHAL.  Further use of the module
 *  must be initiated by a call to xxxInitModule(), which starts the entire process
 *  over again.
 *
 *  @param   HalDev   CPHAL module instance. (set by xxxInitModule())  
 *
 *  @return  EC_NO_ERRORS (ok). <BR>
 *           Possible Error Codes:<BR>
 *           Any error code from halClose().<BR>
 */
static int halShutdown(HAL_DEVICE *HalDev)
  {
   int Ch, Queue;                                                  /*GSG+030514*/

#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]halShutdown(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   /* Verify proper device state */
   if (HalDev->State == enOpened)
     halClose(HalDev, 3);                                          /*GSG+030429*/

   /* Buffer/descriptor resources may still need to be freed if a Close
      Mode 1 was performed prior to Shutdown - clean up here */    /*GSG+030514*/
   for (Ch=0; Ch<NUM_AAL5_CHAN; Ch++)                                
     {                                                             
      if (HalDev->RcbStart[Ch] != 0)                           
        FreeRx(HalDev,Ch);

      for(Queue=0; Queue<MAX_QUEUE; Queue++)
        {
         if (HalDev->TcbStart[Ch][Queue] != 0)
           FreeTx(HalDev,Ch,Queue);
        }
     }

   /* shutdown the CPSAR layer */
   HalDev->SarFunc->Shutdown(HalDev->SarDev);

#ifdef __CPHAL_DEBUG
   if (DBG(6))
     {
      dbgPrintf("  [aal5 halShutdown()]Free AAL5 function pointers\n");
      osfuncSioFlush();
     }
   if (DBG(1)||DBG(3))
     {
      dbgPrintf("[aal5->os]Free(MemPtr:%08x)\n", (bit32u)HalDev->HalFuncPtr);
      osfuncSioFlush();
     }
#endif
   /* free the HalFunc */
   HalDev->OsFunc->Free(HalDev->HalFuncPtr);

#ifdef __CPHAL_DEBUG
   if (DBG(6))
     {
      dbgPrintf("  [aal5 halShutdown]Free HalDev\n");
      osfuncSioFlush();
     }
   if (DBG(1)||DBG(3))
     {
      dbgPrintf("[aal5->os]Free(MemPtr:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif
   /* free the HAL device */
   HalDev->OsFunc->FreeDev(HalDev);

   return(EC_NO_ERRORS);
  }

/**
 *  @ingroup CPHAL_Functions
 *  Used to perform regular checks on the device.  This function should be 
 *  called at a regular interval specified by the @c Tick parameter. 
 *  
 *  @param   HalDev      CPHAL module instance. (set by xxxInitModule())
 *
 *  @return  EC_NO_ERRORS (ok).<BR>
 *           Possible Error Codes:<BR>
 *           @ref EC_VAL_INVALID_STATE "EC_VAL_INVALID_STATE"<BR>
 */
static int halTick(HAL_DEVICE *HalDev)
  {
#ifdef __CPHAL_DEBUG
   if (DBG(0))
     {
      dbgPrintf("[aal5]halTick(HalDev:%08x)\n", (bit32u)HalDev);
      osfuncSioFlush();
     }
#endif

   if (HalDev->State != enOpened)
     return(EC_AAL5|EC_FUNC_TICK|EC_VAL_INVALID_STATE);

   return(EC_NO_ERRORS);
  }

/** 
 *  @ingroup CPHAL_Functions
 *
 *  This function will:
 *  -# allocate a HalDev that will be used by the OS for future communications with the device
 *  -# save OsDev for use when calling OS functions
 *  -# allocate and populate HalFunc with the addresses of CPHAL functions.
 *  -# check OsFuncSize to see if it meets the minimum requirement.
 *  -# return the size of the HAL_FUNCTIONS structure through the HalFuncSize pointer.  The OS
 *     should check this value to ensure that the HAL meets its minimum requirement.
 *
 *  Version checking between the OS and the CPHAL is done using the OsFuncSize and 
 *  HalFuncSize.  Future versions of the CPHAL may add new functions to either
 *  HAL_FUNCTIONS or OS_FUNCTIONS, but will never remove functionality.  This enables
 *  both the HAL and OS to check the size of the function structure to ensure that
 *  the current OS and CPHAL are compatible.
 *
 *  Note:  This is the only function exported by a CPHAL module. 
 *
 *  Please refer to the section "@ref hal_init" for example code.
 *
 *  @param   HalDev  Pointer to pointer to CPHAL module information.  This will 
 *                   be used by the OS when communicating to this module via 
 *                   CPHAL. Allocated during the call.
 *  @param   OsDev   Pointer to OS device information.  This will be saved by
 *                   the CPHAL and returned to the OS when required.
 *  @param   HalFunc Pointer to pointer to structure containing function pointers for all CPHAL 
 *                   interfaces.  Allocated during the call.
 *  @param   OsFunc  Pointer to structure containing function pointers for all OS
 *                   provided interfaces.  Must be allocated by OS prior to call.
 *  @param   OsFuncSize  Size of OS_FUNCTIONS structure.
 *  @param   HalFuncSize    Pointer to the size of the HAL_FUNCTIONS structure.
 *  @param   Inst    The instance number of the module to initialize. (start at
 *                   0).
 * 
 *  @return  EC_NO_ERRORS (ok). <BR>
 *           Possible Error Codes:<BR>
 *           @ref EC_VAL_OS_VERSION_NOT_SUPPORTED "EC_VAL_OS_VERSION_NOT_SUPPORTED"<BR>
 *           @ref EC_VAL_MALLOC_DEV_FAILED "EC_VAL_MALLOC_DEV_FAILED"<BR>
 *           @ref EC_VAL_MALLOC_FAILED "EC_VAL_MALLOC_FAILED"<BR>
 */
int xxxInitModule(HAL_DEVICE **HalDev, 
                 OS_DEVICE *OsDev, 
                 HAL_FUNCTIONS **HalFunc, 
                 OS_FUNCTIONS *OsFunc, 
                 int OsFuncSize,
                 int *HalFuncSize,
                 int Inst);

int cpaal5InitModule(HAL_DEVICE **HalDev, 
                 OS_DEVICE *OsDev, 
                 HAL_FUNCTIONS **HalFunc, 
                 OS_FUNCTIONS *OsFunc, 
                 int OsFuncSize,
                 int *HalFuncSize,
                 int Inst)
  {
   int rc, SarFuncSize;
   HAL_DEVICE *HalPtr;
   HAL_FUNCTIONS *HalFuncPtr;

   /* NEW CODE */
   if (OsFuncSize < sizeof(OS_FUNCTIONS))
     return (EC_AAL5|EC_FUNC_HAL_INIT|EC_VAL_OS_VERSION_NOT_SUPPORTED);

   HalPtr = (HAL_DEVICE *) OsFunc->MallocDev(sizeof(HAL_DEVICE));
   if (!HalPtr)
     return (EC_AAL5|EC_FUNC_HAL_INIT|EC_VAL_MALLOC_DEV_FAILED);

   HalFuncPtr = (HAL_FUNCTIONS *) OsFunc->Malloc(sizeof(HAL_FUNCTIONS));
   if (!HalFuncPtr)
     return (EC_AAL5|EC_FUNC_HAL_INIT|EC_VAL_MALLOC_FAILED);

   /* Initialize the size of hal functions */
   *HalFuncSize = sizeof (HAL_FUNCTIONS);

   /* clear the device structure */
   OsFunc->Memset(HalPtr, 0, sizeof(HAL_DEVICE));

   /* clear the function pointers */
   OsFunc->Memset(HalFuncPtr, 0, sizeof(HAL_FUNCTIONS));

   /* initialize the HAL_DEVICE structure */
   HalPtr->OsDev  = OsDev;
   /*HalPtr->OsOpen = OsDev;*/
   HalPtr->Inst   = Inst; 
   HalPtr->OsFunc = OsFunc;
  
   /* Supply pointers for the CPHAL API functions */
   HalFuncPtr->RxReturn        = halRxReturn;
   HalFuncPtr->Init            = halInit;
   HalFuncPtr->Close           = halClose;
   HalFuncPtr->Send            = halSend;
   HalFuncPtr->ChannelSetup    = halChannelSetup;
   HalFuncPtr->ChannelTeardown = halChannelTeardown;
   HalFuncPtr->Open            = halOpen;
   HalFuncPtr->Kick            = halKick;
   HalFuncPtr->RegAccess       = halRegAccess; 
   HalFuncPtr->Probe           = halProbe;
   HalFuncPtr->Control         = halControl;
   HalFuncPtr->Tick            = halTick;
   HalFuncPtr->Shutdown        = halShutdown;
   HalFuncPtr->OamFuncConfig   = halOamFuncConfig; /* +GSG 030306 */
   HalFuncPtr->OamLoopbackConfig   = halOamLoopbackConfig; /* ~GSG 030416 */

   /* Temporary */
   /*HalFuncPtr->StatsGetOld = StatsGet;*/
   HalFuncPtr->PacketProcessEnd = halPacketProcessEnd;

   /* Now, AAL5 must connect to the CPSAR layer */

   /* Attach to SAR HAL Functions */
   /*
   cpsarInitModule(NULL, NULL, 0, NULL, &SarFuncSize, Inst);

   if (SarFuncSize!=sizeof(CPSAR_FUNCTIONS))
    return(EC_AAL5|EC_FUNC_HAL_INIT|EC_VAL_CPSAR_VERSION_NOT_SUPPORTED); 

   HalPtr->SarFunc = (CPSAR_FUNCTIONS *) OsFunc->Malloc(SarFuncSize);
   */

   rc = cpsarInitModule(&HalPtr->SarDev, OsDev, &HalPtr->SarFunc, OsFunc, sizeof(OS_FUNCTIONS), &SarFuncSize, Inst);

   /* pass back the error value from the CPSAR layer if necessary */
   if (rc)
     return(rc);

   /*
   if (!HalPtr->SarDev)
     return(EC_AAL5|EC_FUNC_HAL_INIT|EC_VAL_NULL_CPSAR_DEV);
   */

   /* Initialize the hardware state */
   HalPtr->State = enConnected;

   /* keep a reference to HalFuncPtr so I can free it later */
   HalPtr->HalFuncPtr = HalFuncPtr;

   /* pass the HalPtr back to the caller */
   *HalDev = HalPtr;
   *HalFunc = HalFuncPtr;

   return(EC_NO_ERRORS);
  }
