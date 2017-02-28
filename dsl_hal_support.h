#ifndef DSL_HAL_SUPPORT_H__
#define DSL_HAL_SUPPORT_H__ 1

/*******************************************************************************
* FILE PURPOSE:     DSL Driver API functions for Sangam
*
********************************************************************************
* FILE NAME:        dsl_hal_functiondefines.c
*
* DESCRIPTION:
*       Contains basic DSL HAL API declarations for Sangam
*
*
* (C) Copyright 2001-02, Texas Instruments, Inc.
*    History
*    Date        Version            Notes
*    06Feb03     0.00.00            RamP   Created
*    21Mar03     0.00.01            RamP   Removed byteswap functions
*    21Mar03     0.00.02            RamP   Added extern osFreeVMemory declaration
*    10Apr03     0.00.03            RamP   Changed declaration for loadFWImage &
*                                          loadDebugFWImage to remove ptidsl param
*    12Apr03     0.00.04            RamP   Added function to set Interrupt Bit
*                                          Masks for bitfield & Mailboxes
*    14Apr03     0.00.05            RamP   Added modem state bit field processing
*    15Apr03     0.00.06            RamP   Added function osAllocateVMemory
*    21Apr03     0.01.00            RamP   Added function osAllocateDmaMemory
*                                          Added function osFreeDmaMemory
*                (Alpha)                   Added macro virtual2Physical,
*    22Apr03     0.01.01            RamP   Moved acknowledgeInterrupt to api.h
*    24Apr03     0.01.02            RamP   Added checkOvelayPage function
*    29May03     0.01.03            RamP   Added critical enter/exit function decl
*    06Jun03     0.01.04            RamP   Added Interrupt source parsing function
*    06Oct03     0.01.05            RamP   Added function abstraction switches
*    12Oct03     0.01.06            RamP   Added ADSL2 Message function prototypes
*    14Nov03     0.03.07            RamP   Added function to gather Rate Messages
*    29Apr04     0.03.08            RamP   Added function to determine dsNumTones
*    26Aug04     0.03.09            Brian  Made changes to support switching DSP
*                                          frequency to 250Mhz
*    14June05    0.03.10            CPH    Added dslhal_support_getTrainedModeEx()
*    20Jul05     0.03.11            CPH    Added dslhal_support_shortSwap32()
*    26Jul05     0.05.00            CPH    - Added dslhal_support_getTrainedMode(),
*                                            dslhal_support_readInternalOffset(),
*                                            dslhal_support_readOffset(),
*                                            dslhal_support_getNumUsTones()
*                                          - Rename dslhal_support_getNumTones() to dslhal_support_getNumDsTones()
*    7 Oct05     0.06.00            AV/CPH  Changed dprintf to dgprintf and compile time
*                                           selection of dslhal_api_getQLNpsds() dslhal_api_getSNRpsds()
*                                           and for code size reduction.
*    04Nov05     0.11.00            CPH    Fixed T1413 mode got Zero DS/US rate when DSL_BIT_TMODE is set.
*******************************************************************************/

#include <dsl_hal_api.h>

#define virtual2Physical(a)    (((int)a)&~0xe0000000)
/* External Function Prototype Declarations */

extern unsigned int shim_osGetCpuFrequency(void);
extern void shim_osClockWait(int val);
extern unsigned int shim_osClockTick(void);

extern int shim_osStringCmp(const char *s1, const char *s2);

extern void dprintf( int uDbgLevel, char * szFmt, ...);

/*
#ifdef DEBUG_BUILD
#define dgprintf(uDbgLevel, szFmt, ...) dprintf(uDbgLevel, szFmt, ...)
#else
#define dgprintf(uDbgLevel, szFmt, ...)
#endif
*/


#ifdef DEBUG_BUILD
#define dgprintf dprintf
#else
#define dgprintf(uDbgLevel, szFmt, args...)
#endif



extern int shim_osLoadFWImage(unsigned char *firmwareImage);
extern int shim_osLoadDebugFWImage(unsigned char *debugFirmwareImage);
extern unsigned int shim_read_overlay_page(void *ptr, unsigned int secOffset, unsigned int secLength);
extern void shim_osMoveMemory(char *dst, char *src, unsigned int numBytes);
extern void shim_osZeroMemory(char *dst, unsigned int numBytes);

extern void *shim_osAllocateMemory(unsigned int size);
extern void *shim_osAllocateVMemory(unsigned int size);
extern void *shim_osAllocateDmaMemory(unsigned int size);

extern void shim_osFreeMemory(void *ptr, unsigned int size);
extern void shim_osFreeVMemory(void *ptr, unsigned int size);
extern void shim_osFreeDmaMemory(void *ptr, unsigned int size);

extern void shim_osWriteBackCache(void *pMem, unsigned int size);
extern void shim_osCriticalEnter(void);
extern void shim_osCriticalExit(void);


/*******************************************************************************************
* FUNCTION NAME:    dslhal_support_writeHostMailbox
*
********************************************************************************************
* DESCRIPTION: Send a message to a mailbox
*
* ARGUMENTS:   int cmd             command to write
*              int tag             tag (currently unused)
*              int p1              parameter 1 (currently unused)
*              int p2              parameter 2 (currently unused)
*
* RETURNS:     0 if successful
*              NZ otherwise
*
*******************************************************************************************/

int dslhal_support_writeHostMailbox
(tidsl_t *ptidsl,
 int cmd,
 int tag,
 int p1,
 int p2);

/********************************************************************************************
* FUNCTION NAME:    dslhal_support_readDspMailbox
*
*********************************************************************************************
* DESCRIPTION: Reads a message from the mailbox
*
* ARGUMENTS:   int *pcmd           Pointer to command read
*
* RETURNS:     0 if successful
*              1 if no mail
*              NZ otherwise
*
*****************************************************************************************/

int dslhal_support_readDspMailbox
(tidsl_t *ptidsl,
 int *pcmd,
 int *ptag,
 int *pprm1,
 int *pprm2);

/********************************************************************************************
* FUNCTION NAME:    dslhal_support_readTextMailbox
*
*********************************************************************************************
* DESCRIPTION: Reads a message from the mailbox
*
* ARGUMENTS:   int *pcmd           Pointer to command read
*
* RETURNS:     0 if successful
*              1 if no mail
*              NZ otherwise
*
*****************************************************************************************/

int dslhal_support_readTextMailbox
(tidsl_t *ptidsl,
 int *pmsg1,
 int *pmsg2);

/******************************************************************************************
* FUNCTION NAME:    dslhal_support_blockRead
*
*********************************************************************************************
* DESCRIPTION: This rouin simulates DSP memory read as done in ax5 pci nic card
*
* INPUT:  void *addr,   memory address to be read
*         void *buffer, dat buffer to be filled with from memmory
*         size_t count, number of bytes to be written
*
* RETURN:  0 --succeeded
*          1 --Failed
*
*****************************************************************************************/

int dslhal_support_blockRead
(void *addr,
 void *buffer,
 size_t count);

/******************************************************************************************
* FUNCTION NAME:    dslhal_support_blockWrite
*
*******************************************************************************************
* DESCRIPTION: This rouin simulates DSP memory write as done in ax5 pci nic card
*
* INPUT:  void *buffer, data need to written
*         void *adde,   memory address to be written
*         size_t count, number of bytes to be written
*
* RETURN:  0 --succeeded
*          1 --Failed
*
*****************************************************************************************/

int dslhal_support_blockWrite
(void *buffer,
 void *addr,
 size_t count);

/******************************************************************************************
* FUNCTION NAME:    dslhal_support_hostDspAddressTranslate
*
*******************************************************************************************
* DESCRIPTION: This function moves the address window to translate physical address
*
* INPUT:  unsigned int addr : address that requires translation
*
* RETURN:  Translated address or error condition
*
*
*****************************************************************************************/

unsigned int dslhal_support_hostDspAddressTranslate
( unsigned int addr
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_support_unresetDslSubsystem
*
*******************************************************************************************
* DESCRIPTION: This function unreset Dsl Subsystem
*
* INPUT:  None
*
* RETURN: 0 if Pass; 1 if Fail
*
*****************************************************************************************/
int dslhal_support_unresetDslSubsystem
(void
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_support_unresetDsp()
*
*******************************************************************************************
* DESCRIPTION: This fuction takes ax5 daugter board out of reset.
*
* INPUT:  None
*
* RETURN: 0  --successful.
*         1  --failed
*
*****************************************************************************************/
int dslhal_support_unresetDsp
(void
);


/******************************************************************************************
* FUNCTION NAME:    dslhal_support_resetDslSubsystem
*
*******************************************************************************************
* DESCRIPTION: This function unreset Dsl Subsystem
*
* INPUT:  None
*
* RETURN: 0 if Pass; 1 if Fail
*
*****************************************************************************************/
int dslhal_support_resetDslSubsystem
(void
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_support_resetDsp()
*
*******************************************************************************************
* DESCRIPTION: This fuction takes ax5 daugter board out of reset.
*
* INPUT:  None
*
* RETURN: 0  --successful.
*         1  --failed
*
*****************************************************************************************/
int dslhal_support_resetDsp
(void
);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_setDsp250MHzTrigger(void)
*
*********************************************************************************************
* DESCRIPTION:
*   Set the trigger to run DSP at 250Mhz.
*
* Input:  none
*
* Return: >=0, success
*         -1,  failure
*
*
********************************************************************************************/
int dslhal_support_setDsp250MHzTrigger(tidsl_t *ptidsl);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_hostDspCodeDownload()
*
*********************************************************************************************
* DESCRIPTION:
*   download DSP image from host memory to dsp memory
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/

int dslhal_support_hostDspCodeDownload
(tidsl_t * ptidsl
);

/********************************************************************************************
* FUNCTION NAME: dslhal_diags_digi_assignMemTestSpace()
*
*********************************************************************************************
* DESCRIPTION: Assigns Memory Space in SDRAM for External Memory Test
* Input: tidsl_t *ptidsl
*
* Return: 0    success
*         1    failed
*
********************************************************************************************/

unsigned int dslhal_diags_digi_assignMemTestSpace
(tidsl_t *ptidsl
);

/********************************************************************************************
* FUNCTION NAME: dslhal_diags_digi_readMemTestResult()
*
*********************************************************************************************
* DESCRIPTION: Reads Results of External Memory Test
* Input: tidsl_t *ptidsl
*
* Return: 0    success
*         1    failed
*
********************************************************************************************/

unsigned int dslhal_diags_digi_readMemTestResult
(tidsl_t *ptidsl,
unsigned int testResult
);


/********************************************************************************************
* FUNCTION NAME: dslhal_diags_codeDownload()
*
*********************************************************************************************
* DESCRIPTION: Brings DSLSS out of Reset, Downloads Diag Firmware,
*              brings DSP out of Reset
* Input: tidsl_t *ptidsl
*
* Return: 0    success
*         1    failed
*
********************************************************************************************/

unsigned int dslhal_diags_codeDownload
(tidsl_t *ptidsl,
unsigned char* missingTones
);

/********************************************************************************************
* FUNCTION NAME: dslhal_diags_anlg_setPgaParams()
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

unsigned int  dslhal_diags_anlg_setPgaParams
(tidsl_t *ptidsl,
int agcFlag,
short pga1,
short pga2,
short pga3,
short aeq
);

/********************************************************************************************
* FUNCTION NAME: dslhal_diags_anlg_getRxNoisePower()
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

unsigned int dslhal_diags_anlg_getRxNoisePower
(tidsl_t *ptidsl
);

/********************************************************************************************
* FUNCTION NAME: dslhal_diags_anlg_setMissingTones()
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

unsigned int dslhal_diags_anlg_setMissingTones
(tidsl_t *ptidsl,
unsigned char* missingTones
);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_readDelineationState()
*
*********************************************************************************************
* DESCRIPTION:
*   download DSP image from host memory to dsp memory
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_readDelineationState
(tidsl_t *ptidsl
);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_processModemStateBitField()
*
*********************************************************************************************
* DESCRIPTION:
*   download DSP image from host memory to dsp memory
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_processModemStateBitField
(tidsl_t *ptidsl
);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_setInterruptMask()
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

unsigned int dslhal_support_setInterruptMask
(tidsl_t * ptidsl,
unsigned int inputMask
);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_computeCrc32()
*
*********************************************************************************************
* DESCRIPTION:
*   Computes the CRC-32 for the input data
*
* Return: 32 bit CRC of the input data
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_computeCrc32
(unsigned char *data,
int len
);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_checkOverlayPage()
*
*********************************************************************************************
* DESCRIPTION:
*   Computes the CRC-32 for the input data and compares it with reference
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_checkOverlayPage
(tidsl_t *ptidsl,
unsigned int tag
);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_restoreTrainingInfo()
*
*********************************************************************************************
* DESCRIPTION:
*   Computes the CRC-32 for the input data and compares it with reference
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/

int dslhal_support_restoreTrainingInfo(tidsl_t * ptidsl);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_reloadTrainingInfo()
*
*********************************************************************************************
* DESCRIPTION:
*   Computes the CRC-32 for the input data and compares it with reference
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/

int dslhal_support_reloadTrainingInfo(tidsl_t * ptidsl);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_clearTrainingInfo()
*
*********************************************************************************************
* DESCRIPTION:
*   Computes the CRC-32 for the input data and compares it with reference
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/

int dslhal_support_clearTrainingInfo(tidsl_t * ptidsl);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_parseInterruptSource()
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

unsigned int dslhal_support_parseInterruptSource(tidsl_t * ptidsl);
/********************************************************************************************
* FUNCTION NAME: dslhal_support_advancedIdleProcessing()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced Idle State Processing Functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_advancedIdleProcessing(tidsl_t *ptidsl);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_aocBitSwapProcessing()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced Bitswap buffer Processing Functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_aocBitSwapProcessing(tidsl_t *ptidsl,unsigned int usDs);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_gatherEocMessages()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced EOC Buffering functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_gatherEocMessages(tidsl_t *ptidsl,int usDs, int msgPart1, int msgPart2);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_gatherSnrPerBin()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced Snr per bin buffering Functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_gatherSnrPerBin(tidsl_t *ptidsl,unsigned int snrParm);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_processTrainingState()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced Training State Processing Functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_processTrainingState(tidsl_t *ptidsl);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_gatherAdsl2Messages()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced EOC Buffering functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_gatherAdsl2Messages(tidsl_t *ptidsl,int msgTag, int param1, int param2);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_getAdsl2MsgLocation()
*
*********************************************************************************************
* DESCRIPTION:
*   Gets the address to the ADSL2 Message being looked up
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_getAdsl2MessageLocation(tidsl_t *ptidsl,int msgOffset);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_getCMsgsRa()
*
*********************************************************************************************
* DESCRIPTION:
*   Calls Advanced Training Message functions
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_getCMsgsRa(tidsl_t *ptidsl,void *cMsg);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_getMaxDsTones()
*
*********************************************************************************************
* DESCRIPTION:
*   Gets Number of Down Stream tones
* Return: 256 or 512
*
********************************************************************************************/
unsigned int dslhal_support_getMaxDsTones(tidsl_t *ptidsl);

unsigned int dslhal_support_getNumTones(tidsl_t *ptidsl); // Kept for backward compatible

/********************************************************************************************
* FUNCTION NAME: dslhal_support_getMaxUsTones()
*
*********************************************************************************************
* DESCRIPTION:
*   Gets Number of Up Stream tones
* Return: 256 or 512
*
********************************************************************************************/
unsigned int dslhal_support_getMaxUsTones(tidsl_t *ptidsl);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_gatherRateMessages()
*
*********************************************************************************************
* DESCRIPTION:
*   Gathers Advanced Training Messages
*
* Return: Error Condition (if any)
*
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_support_gatherRateMessages(tidsl_t *ptidsl);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_byteSwap16()
*
*********************************************************************************************
* DESCRIPTION:
* byteswap a short
*
* INPUT:
* Return: NULL
*
********************************************************************************************/

unsigned short dslhal_support_byteSwap16(unsigned short in16Bits);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_byteSwap32()
*
*********************************************************************************************
* DESCRIPTION:
*   byteswap within 32 bit unsigned integer
*
* INPUT: unsigned 32 bit integer
*
* Return: Byte Swapped 32 bit integer
*      B0|B1|B2|B3 -> B3|B2|B1|B0  (where B0..B3 is byte(8bit) data within unsigned int32)
*
* Note: The function will do the swap when MIPS code is compiled as Big Endian
*       For little endian, the result is the same as input.
********************************************************************************************/
unsigned int dslhal_support_byteSwap32_BE (unsigned int in32Bits);

#ifndef EB
#define dslhal_support_byteSwap32(x) ((unsigned int) (x))
#else
#define dslhal_support_byteSwap32 dslhal_support_byteSwap32_BE
#endif

/********************************************************************************************
* FUNCTION NAME: dslhal_support_shortSwap32()
*
*********************************************************************************************
* DESCRIPTION:
*   shortswap within 32 bit unsigned integer
*
* INPUT: unsigned 32 bit integer
*
* Return: Short Swapped 32 bit integer
*   S0|S1 -> S1|S0 (where S0,S1 is unsigned short (16bit)data within unsigned int32)
*
* Note: The function will do the swap when MIPS code is compiled as Big Endian
*       For little endian, the result is the same as input.
********************************************************************************************/

unsigned int dslhal_support_shortSwap32(unsigned int in32Bits);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_getTrainedModeEx()
*
*********************************************************************************************
* DESCRIPTION:
*  Construced the Extended (32bits) TrainedMode
*
* INPUT:
* Return: NULL
*
********************************************************************************************/
unsigned int dslhal_support_getTrainedMode (tidsl_t *ptidsl);
unsigned int dslhal_support_getTrainedModeEx(unsigned int trainMode, unsigned int annex_sel);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_readOffset
*
*********************************************************************************************
* DESCRIPTION:
*   Read 32 bit endian converted value from Host-DSP Interface based on offset array.
*  (Internal Used Only)
*
* INPUT: offsetnum: # entries in *offset array follows.
*        *offset: offset table.
* Return: 32 bit endian converted value.
*
********************************************************************************************/
unsigned int dslhal_support_readOffset(tidsl_t *ptidsl, int offsetnum, int *offset);

/********************************************************************************************
* FUNCTION NAME: dslhal_support_readInternalOffset
*
*********************************************************************************************
* DESCRIPTION:
*   Read 32 bit endian converted value from Internal Host-DSP Interface based on offset array.
*  (Internal Used Only)
*
* INPUT: offsetnum: # entries in *offset array follows.
*        *offset: offset table.
* Return: 32 bit endian converted value.
*
********************************************************************************************/
unsigned int dslhal_support_readInternalOffset(tidsl_t *ptidsl, int offsetnum, int *offset);

unsigned int dslhal_support_addrFromOffset(tidsl_t *ptidsl, int offsetnum, int *offset, int type);
unsigned int dslhal_support_writeFromOffset(tidsl_t *ptidsl, int offsetnum, int *offset, unsigned int dat, int type);

/******************************************************************************************
* FUNCTION NAME:     dslhal_support_IsADSL1Mode
*
*******************************************************************************************
* DESCRIPTION: This function check current train mode see if it's GDMT, T1413, or GLITE.
*              Detect & take care both old train mode scheme & new train mode scheme.
*              It is expected the ptidsl->AppData.dsl_modulation is been set before calling
*              this function.
*
* INPUT:  PITIDSLHW_T *ptidsl
*
* RETURN: TRUE: It's one of ADSL1 (GDMT, T1413, or GLITE) mode.
*         FALSE: It's not ADSL1 mode.
*****************************************************************************************/
int dslhal_support_IsADSL1Mode(tidsl_t *ptidsl);

#endif /* Pairs #ifndef DSL_HAL_FUNCTIONDEFINES_H__ */
