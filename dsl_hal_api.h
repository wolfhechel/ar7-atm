#ifndef __DSL_HAL_API_H__
#define __DSL_HAL_API_H__ 1
/*******************************************************************************
* FILE PURPOSE:     DSL HAL to Application Interface for Sangam
*
********************************************************************************
* FILE NAME:        dsl_application_interface.h
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
*******************************************************************************/

#ifdef INTERNAL_BUILD
#include <dsl_hal_api_pvt.h>
#endif

#define NUM_PAGES 4
#define OAMFEATURE_AUTORETRAIN_MASK       0x00000001
#define OAMFEATURE_TC_SYNC_DETECT_MASK    0x00000002
#define OAMFEATURE_EOCAOC_INTERRUPT_MASK  0x00000004
#define OAMFEATURE_CONS_DISP_DISABLE_MASK 0x00000008
#define OAMFEATURE_GHSMSG_INTERRUPT_MASK  0x00000010

typedef struct tagTIOIDINFO
{
  unsigned int   bState;           /* addr->bDSPATURState    */ 
  unsigned int   USConRate;        /* US Conection Rates */
  unsigned int   DSConRate;        /* DS Connection Rates */
  unsigned int   USPayload;        /* ennic_tx_pullup*/
  unsigned int   DSPayload;        /* ennic_indicate_receive_packet*/
  unsigned int   FrmMode;          /* addr->atur_msg.framing_mode*/
  unsigned int   MaxFrmMode;          
  unsigned int   TrainedPath;      /* Status of the Modem in which trained (Fast or Interleaved Path) */
  unsigned int   TrainedMode;      /* Status of the mode in which the modem is trained (G.dmt, T1.413, etc) */
  
  /* Superframe Count */
  unsigned int   usSuperFrmCnt;    /* Num of US Superframes */
  unsigned int   dsSuperFrmCnt;    /* Num of DS Superframes */

  /* LOS & SEF Stats */
  unsigned int   LOS_errors;       /* Num of ADSL frames where loss-of-sync */
  unsigned int   SEF_errors;       /* Num of severly errored ADSL frames - LOS > MAXBADSYNC ADSL frames */
  unsigned int   coLosErrors; /* CO LOS Defects */
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
  unsigned int   dsMargin;         /* Measured DS MArgin */

  unsigned int   usLineAttn;
  unsigned int   usMargin;

  unsigned char    bCMsgs1[6];
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

  unsigned char  BitAllocTblDstrm[256]; 
  unsigned char  BitAllocTblUstrm[32];  
    signed char  marginTblDstrm[256];
  unsigned char  rBng[512];
  unsigned char  cBng[126];
           int   usTxPower;
           int   dsTxPower;
         short   rxSnrPerBin0[256];
         short   rxSnrPerBin1[256];
         short   rxSnrPerBin2[256];

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
           int   rxNoisePower0[256];
           int   rxNoisePower1[256];
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

typedef struct _ITIDSLHW
{
  /* struct _TIDSL_IHwVtbl * pVtbl; */
  unsigned char*     fwimage;
  void*     pmainAddr;
  void*     pOsContext;
  unsigned int     ReferenceCount;
  unsigned int     netService;  

  int     InitFlag;

  int     imagesize;
  
  unsigned int     lConnected;
  unsigned int     bStatisticsInitialized;
  unsigned int     rState;
  unsigned int     bShutdown; 
  unsigned int     blackOutValid_f;
  unsigned char    blackOutBits[64];
  unsigned int     bAutoRetrain;
  volatile unsigned int     bOverlayPageLoaded;
  unsigned int     stateTransition;
  unsigned int     configFlag;
  unsigned int     dsBitSwapInx;
  unsigned int     usBitSwapInx;
  unsigned int     trainStateInx;
  unsigned int     usEocMsgInx;
  unsigned int     dsEocMsgInx;
  unsigned int     reasonForDrop;
  TIOIDINFO  AppData;
  dspVer     dspVer;

  OlayDP_Def olayDpPage[NUM_PAGES];
  OlayDP_Def coProfiles;
  OlayDP_Def constDisplay;
  dslBitSwapDef dsBitSwap[30];
  dslBitSwapDef usBitSwap[30];  
  trainStateInfo trainHistory[120];
  eocMessageDef usEocMsgBuf[30];
  eocMessageDef dsEocMsgBuf[30];
  adsl2Msgs     adsl2TrainingMessages;
  adsl2DeltMsgs adsl2DiagnosticMessages;
  unsigned int modemStateBitField[4];
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
* FUNCTION NAME:    dslhal_api_setMaxBitsPerCarrier(tidsl_t *ptidsl,unsigned int maxbits)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the Maximum bits per carrier value
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
 unsigned int *dsTones
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
* FUNCTION NAME: dslhal_advcfg_getSnrPerBin()
*
*********************************************************************************************
* DESCRIPTION:
*   Get SNR data per bin 
* Input
*        tidsl_t *ptidsl : Pointer to application structure
*        
*
* Return: 0  success
*         1  failed
* 
********************************************************************************************/
unsigned int dslhal_advcfg_getSnrPerBin
(tidsl_t * ptidsl,
 unsigned int snrBufferOpt
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

#ifdef INTERNAL_BUILD
#include <dsl_hal_internal_api.h>
#endif


#endif /* pairs #ifndef __DSL_APPLICATION_INTERFACE_H__ */
