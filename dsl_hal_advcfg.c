//#ifndef NO_ACT
/*******************************************************************************
* FILE PURPOSE:     DSL Driver API functions for Advanced Configurations
*
********************************************************************************
* FILE NAME:        dsl_hal_advcfg.c
*
* DESCRIPTION:
*       Contains DSL HAL Advanced Configurations & Statistics APIs
*
*
* (C) Copyright 2003-04, Texas Instruments, Inc.
*    History
*    Date        Version            Notes
*    20Aug03     0.00.00            RamP   Original Version Written
*    29Sep03     0.00.00            RamP   Added advanced config APIs for:
*                                             - Snr Per Bin
*                                             - Reason For Connection Drop
*                                             - Constellation Data per bin
*                                             - Maximum Average Fine Gains
*                                             - Logging EOC Messages
*    15Oct03     0.00.01            RamP   Modified negoMsgs block writes
*                                          to fix pointer de-referencing
*    28Oct03     0.00.02            RamP   Added function to config blackout
*                                          bitmap in the RMSGPCB message
*    24Nov03     0.00.03            RamP   Added valid flag logic before
*                                          writing blackout bits
*    01Apr04     0.00.04            RamP   Fixed Fine Gains setting
*    14May04     0.00.05            RamP   Incorporated dsNumTones logic where
*                                          necessary to switch number of bins
*                                          Moved SNR function to support module
*    14May04     0.00.06            RamP   Added logic to set additional flags
*                                          for US/DS AOC bitswap
*    02Aug04     0.00.07            RamP   Fixed bug with bitswap flag setting
*    02Feb05     0.00.08            CPH    Pull dev_host_interface.h out from dsl_hal_register.h
*    05Jul05     0.00.09            CPH    CQ9775: Change dslhal_advcfg_configDsTones input parameters & support for ADSL2+
*    24Jul05     0.00.10            CPH    Fixed comments in dslhal_advcfg_configDsTones function header
*******************************************************************************/
#include <dev_host_interface.h>
#include <dsl_hal_register.h>
#include <dsl_hal_support.h>

/*****************************************************************************/
/* ACT API functions -- To be moved into their own independent module --RamP */
/*****************************************************************************/

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_onOffPcb()
*
*********************************************************************************************
* DESCRIPTION:
*   Turns on / off the power cutback feature; 0 = OFF and 1 = ON
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_onOffPcb(tidsl_t * ptidsl,unsigned int onOff)
{
  int rc;
  DEV_HOST_modemEnvPublic_t modemEnv;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  if(onOff <0 || onOff >1)
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }

  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   dspOamSharedInterface.modemEnvPublic_p = (DEV_HOST_modemEnvPublic_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemEnvPublic_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemEnvPublic_p,
                                      &modemEnv, sizeof(DEV_HOST_modemEnvPublic_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   modemEnv.pcbEnabled = onOff;
   rc=dslhal_support_blockWrite(&modemEnv,dspOamSharedInterface.modemEnvPublic_p,sizeof(DEV_HOST_modemEnvPublic_t));
   if(rc)
     return DSLHAL_ERROR_MODEMENV_API_FAILURE;
   else
     return DSLHAL_ERROR_NO_ERRORS;
}

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
unsigned int dslhal_advcfg_onOffBitSwap(tidsl_t * ptidsl,unsigned int usDs, unsigned int onOff)
{
  int rc, offset[2]={33,0},flag=0;
  DEV_HOST_dspBitSwapDef_t  bitSwapDef;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  if((onOff <0 || onOff >1)||(usDs <0 || usDs>1))
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }

  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   if(usDs==0)
     {
       dspOamSharedInterface.dspBitSwapUstrm_p = (DEV_HOST_dspBitSwapDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspBitSwapUstrm_p);
       rc += dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspBitSwapUstrm_p,
                                      &bitSwapDef, sizeof(DEV_HOST_dspBitSwapDef_t));
       offset[0]=33;
       rc += dslhal_api_dspInterfaceRead(ptidsl,(unsigned int)ptidsl->pmainAddr,2,(unsigned int *)&offset, (unsigned char *)&flag,4);
       if(rc)
        return DSLHAL_ERROR_BLOCK_READ;
       flag &= dslhal_support_byteSwap32(0xffffff00);
       flag |= dslhal_support_byteSwap32(onOff & 0x1);
       bitSwapDef.bitSwapEnabled = onOff;
       rc += dslhal_api_dspInterfaceWrite(ptidsl,(unsigned int)ptidsl->pmainAddr,2,(unsigned int *)&offset, (unsigned char *)&flag,4);
       rc +=dslhal_support_blockWrite(&bitSwapDef,dspOamSharedInterface.dspBitSwapUstrm_p,sizeof(DEV_HOST_dspBitSwapDef_t));
       if(rc)
        return DSLHAL_ERROR_BLOCK_WRITE;
     }
   else
    {
       dspOamSharedInterface.dspBitSwapDstrm_p = (DEV_HOST_dspBitSwapDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspBitSwapDstrm_p);
       rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspBitSwapDstrm_p,
                                      &bitSwapDef, sizeof(DEV_HOST_dspBitSwapDef_t));
       offset[0]=32;
       rc += dslhal_api_dspInterfaceRead(ptidsl,(unsigned int)ptidsl->pmainAddr,2,(unsigned int *)&offset, (unsigned char *)&flag,4);
       if(rc)
        return DSLHAL_ERROR_BLOCK_READ;
       flag &= dslhal_support_byteSwap32(0xffffff00);
       flag |= dslhal_support_byteSwap32(onOff & 0x1);
       bitSwapDef.bitSwapEnabled = onOff;
       rc += dslhal_api_dspInterfaceWrite(ptidsl,(unsigned int)ptidsl->pmainAddr,2,(unsigned int *)&offset, (unsigned char *)&flag,4);
       rc += dslhal_support_blockWrite(&bitSwapDef,dspOamSharedInterface.dspBitSwapDstrm_p,sizeof(DEV_HOST_dspBitSwapDef_t));
       if(rc)
        return DSLHAL_ERROR_BLOCK_WRITE;
     }

    return DSLHAL_ERROR_NO_ERRORS;
}

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
* The dsTones[] is a short array in external SDRAM which
* specify the tone number that will be turned off.
* - First element specify the number of tone entries follows.
* - Short (16bit) for each tone so that it can support ADSL2+.
* - For the tone entry most Significant 2 bits is used to indicate the
*   Tone is a range start/end.
*   If MSB 2 (bit[15:14]) bits are '01', then the tone numer is the range_start
*   If MSB 2 (bit[15:14]) bits are '10', then the tone numer is the range_end
* - For example, if the following tones are to be turned off:
*   Tone 64, 80-90, 151
*   Then the array's content will become:
*   0x0004, // 4 entries follows
*   0x0040, // tone 64
*   0x4050, // tone range start from 80 (0x0050 | 0x4000)
*   0x805A, // tone range end   at   90 (0x005A | 0x8000)
*   0x0097  // tone 151
********************************************************************************************/
#define DS_MISSINGTONE_BUFSIZE   32
//static unsigned short dsToneConfig[DS_MISSINGTONE_BUFSIZE]; // allow knock out 31 tones
unsigned int dslhal_advcfg_configDsTones(tidsl_t * ptidsl,unsigned short *dsTones)
{
  static unsigned short *p_dsToneConfig=0;
  unsigned int i, rc;
  unsigned int numTones;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  DEV_HOST_oamWrNegoParaDef_t NegoPara;
  unsigned int *pToneConfig32;
  unsigned int num_swap32;

  dprintf(5, "dslhal_advcfg_configDsTones\n");
  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }


  //
  // The first Time this function is called, it allocate DS_MISSINGTONE_BUFSIZE*2 bytes
  //  (ie DS_MISSINGTONE_BUFSIZE shorts) of buffer from kernel
  //
  if (!p_dsToneConfig)
  {
    p_dsToneConfig = (unsigned short *) shim_osAllocateDmaMemory(DS_MISSINGTONE_BUFSIZE*2);
    if (!p_dsToneConfig)
    {
      dprintf(1,"shim_osAllocateDmaMemory allocate %d bytes failed!\n", DS_MISSINGTONE_BUFSIZE);
      return DSLHAL_ERROR_MALLOC;
    }
  }

  dprintf(2,"p_dsToneConfig=%08x\n", (unsigned int) p_dsToneConfig);

  //CQ9775
  for (numTones=0; *dsTones!= 0; numTones++)
  {
    if (numTones>= DS_MISSINGTONE_BUFSIZE)
    {
      dprintf(1,"Too many input dsTones!\n");
      return DSLHAL_ERROR_INVALID_PARAM;
    }

    p_dsToneConfig[numTones+1]= *dsTones++;
    dprintf(5, "missTone[%d]=%04x\n", numTones+1, p_dsToneConfig[numTones+1]);
  }

  // num of entries does not include the element 0 itself.
  p_dsToneConfig[0]= numTones;

  dprintf(5,"missTone[0]=%04x\n", p_dsToneConfig[0]);

  // adjust endians
  pToneConfig32= (unsigned int *) p_dsToneConfig;

  //
  // numEntries = numTones+1
  // each entry in array is a short (2 bytes)
  // -> total # 32bit to swap = ((numEntries*2+3)/4
  //
  num_swap32= ((numTones+1)*2+3)/4;
  for(i=0; i< num_swap32; i++, pToneConfig32++)
    *pToneConfig32 = dslhal_support_shortSwap32(*pToneConfig32);

  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID) dspOamSharedInterface.oamWriteNegoParams_p,&NegoPara, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  NegoPara.dsToneTurnoff_f = 1;
  NegoPara.missingToneDsAddr
    = dslhal_support_byteSwap32(virtual2Physical((unsigned int) p_dsToneConfig));
  rc = dslhal_support_blockWrite(&NegoPara,(PVOID)dspOamSharedInterface.oamWriteNegoParams_p, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;

  return DSLHAL_ERROR_NO_ERRORS;
}

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
unsigned int dslhal_advcfg_getAocBitswapBuffer(tidsl_t * ptidsl,unsigned int usDs)
{
  int rc=0;
  DEV_HOST_dspBitSwapDef_t  bitSwapDef;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  if((usDs <0 || usDs>1))
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }

  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   if(usDs==0)
     {
       dspOamSharedInterface.dspBitSwapUstrm_p = (DEV_HOST_dspBitSwapDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspBitSwapUstrm_p);
       rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspBitSwapUstrm_p,
                                      &bitSwapDef, sizeof(DEV_HOST_dspBitSwapDef_t));
       if (rc)
        {
          dprintf(1,"dslhal_support_blockRead failed\n");
          return DSLHAL_ERROR_BLOCK_READ;
         }
       else
         {
          shim_osMoveMemory((void *)ptidsl->usBitSwap[ptidsl->usBitSwapInx].bitSwapCommand, (void *)bitSwapDef.bitSwapCmd, 6);
          shim_osMoveMemory((void *)ptidsl->usBitSwap[ptidsl->usBitSwapInx].bitSwapBinNum, (void *)bitSwapDef.bitSwapBin, 6);
          ptidsl->usBitSwap[ptidsl->usBitSwapInx].bitSwapSFrmCnt = (unsigned char)bitSwapDef.bitSwapSCnt;
         }
     }
   else
    {
       dspOamSharedInterface.dspBitSwapDstrm_p = (DEV_HOST_dspBitSwapDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspBitSwapDstrm_p);
       rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspBitSwapDstrm_p,
                                      &bitSwapDef, sizeof(DEV_HOST_dspBitSwapDef_t));
       if (rc)
        {
          dprintf(1,"dslhal_support_blockRead failed\n");
          return DSLHAL_ERROR_BLOCK_READ;
        }
       else
         {
          shim_osMoveMemory((void *)ptidsl->dsBitSwap[ptidsl->dsBitSwapInx].bitSwapCommand, (void *)bitSwapDef.bitSwapCmd, 6);
          shim_osMoveMemory((void *)ptidsl->dsBitSwap[ptidsl->dsBitSwapInx].bitSwapBinNum, (void *)bitSwapDef.bitSwapBin, 6);
          ptidsl->dsBitSwap[ptidsl->dsBitSwapInx].bitSwapSFrmCnt = (unsigned char)bitSwapDef.bitSwapSCnt;
         }
    }
   /* Add bitswap fetch code here */
  return DSLHAL_ERROR_NO_ERRORS;
}

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
unsigned int dslhal_advcfg_readTrainingMessages(tidsl_t * ptidsl,void *msgPtr)
{
  int rc;
  DEV_HOST_dspWrNegoParaDef_t  negoParms;
  DEV_HOST_raMsgsDef_t         raMsgParms;
  DEV_HOST_dspOamSharedInterface_t  dspOamSharedInterface;
  negoMsgs *msgStruct;
  msgStruct = (negoMsgs *)msgPtr;

  dprintf(1, "dslhal_advcfg_readTrainingMessages\n");

  rc = dslhal_support_blockRead(ptidsl->pmainAddr, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  dspOamSharedInterface.dspWriteNegoParams_p = (DEV_HOST_dspWrNegoParaDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.dspWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.dspWriteNegoParams_p, &negoParms, sizeof(DEV_HOST_dspWrNegoParaDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  else
    {
    if(ptidsl->rState < ATU_RSEGUERA)
      shim_osMoveMemory((void *)msgStruct->bCRates1, (void *)negoParms.cRates1, 120);
    else
      shim_osMoveMemory((void *)msgStruct->bCRatesRA, (void *)negoParms.cRates1, 120);
    msgStruct->bCRates2 = negoParms.cRates2;
    shim_osMoveMemory((void *)msgStruct->bRRates1, (void *)negoParms.rRates1, 44);
    msgStruct->bRRates2 = negoParms.rRates2;
    shim_osMoveMemory((void *)msgStruct->bCMsgs1, (void *)negoParms.cMsgs1, 6);
    shim_osMoveMemory((void *)msgStruct->bCMsgs2, (void *)negoParms.cMsgs2, 4);
    shim_osMoveMemory((void *)msgStruct->bRMsgs2, (void *)negoParms.rMsgs2, 4);
    }

    dspOamSharedInterface.raMsgs_p = (DEV_HOST_raMsgsDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.raMsgs_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.raMsgs_p,
                               &raMsgParms, sizeof(DEV_HOST_raMsgsDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  else
    {
    shim_osMoveMemory((void *)msgStruct->bRRatesRA, (void *)raMsgParms.rRatesRaString,4);
    shim_osMoveMemory((void *)msgStruct->bRMsgsRA, (void *)raMsgParms.rMsgsRaString,12);
    shim_osMoveMemory((void *)msgStruct->bCMsgsRA, (void *)raMsgParms.cMsgsRaString,6);
    }
return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_getTrainingState()
*
*********************************************************************************************
* DESCRIPTION:
*   Reads the training state, substate on demand with a timestamp;
* Input
*        tidsl_t *ptidsl : Pointer to application structure
*        void *msgStruct : Pointer to training state struct
*
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_getTrainingState(tidsl_t * ptidsl,void *trainPtr)
{
  int rc;
  DEV_HOST_dspOamSharedInterface_t  dspOamSharedInterface;
  DEV_HOST_modemEnvPublic_t modemEnv;

  unsigned int trainState, trainSubState;
  trainStateInfo *trainStruct;
  trainStruct = (trainStateInfo *)trainPtr;

  dprintf(5,"dslhal_advcfg_getTrainingState\n");

  rc = dslhal_support_blockRead(ptidsl->pmainAddr, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  dspOamSharedInterface.modemEnvPublic_p = (DEV_HOST_modemEnvPublic_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemEnvPublic_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemEnvPublic_p, &modemEnv, sizeof(DEV_HOST_modemEnvPublic_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  dslhal_support_processModemStateBitField(ptidsl);
  trainState=ptidsl->AppData.bState;
  trainStruct->aturState = trainState;
  trainSubState = dslhal_support_byteSwap16(modemEnv.subStateIndex);

  if (rc)
    {
    dprintf(1,"Error reading trainSubState\n");
    return DSLHAL_ERROR_CONFIG_API_FAILURE;
    }

  dprintf(5,"trainSubState=%04X\n", trainSubState);

  trainSubState=trainSubState & 0xffff;
  trainStruct->subStateIndex=trainSubState;
  trainStruct->timeStamp = shim_osClockTick();

  dprintf(0,"StateIndex:%d | SubstateIndex:%d | TimeStamp:%d \n",trainState,trainSubState,trainStruct->timeStamp);
  return DSLHAL_ERROR_NO_ERRORS;
}


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
unsigned int dslhal_advcfg_resetBitSwapMessageLog(tidsl_t * ptidsl, unsigned int usDs)
{
  if(usDs>1 || usDs<0)
    return DSLHAL_ERROR_INVALID_PARAM;
  if(usDs==0)
    shim_osZeroMemory((char *)ptidsl->usBitSwap, (sizeof(dslBitSwapDef)*30));
  else
    shim_osZeroMemory((char *)ptidsl->dsBitSwap, (sizeof(dslBitSwapDef)*30));
  return DSLHAL_ERROR_NO_ERRORS;
    }

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
unsigned int dslhal_advcfg_setConstellationBinNumber(tidsl_t * ptidsl, unsigned int binNum)
{
  DEV_HOST_consBufDef_t           constDisp;
  DEV_HOST_dspOamSharedInterface_t dspOamSharedInterface;
  int rc;
  rc = dslhal_support_blockRead(ptidsl->pmainAddr, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  dspOamSharedInterface.consDispVar_p = (DEV_HOST_consBufDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.consDispVar_p);
  /* Access the profileList Structure */
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.consDispVar_p,&constDisp, sizeof(DEV_HOST_consBufDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  constDisp.consDispBin = dslhal_support_byteSwap32(binNum);
  rc = dslhal_support_blockWrite(&constDisp,(PVOID)dspOamSharedInterface.consDispVar_p,sizeof(DEV_HOST_consBufDef_t));
  if (rc)
    return DSLHAL_ERROR_BLOCK_WRITE;
  return DSLHAL_ERROR_NO_ERRORS;
}

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
unsigned int dslhal_advcfg_resetTrainStateHistory(tidsl_t * ptidsl)
{

  shim_osZeroMemory((char *)ptidsl->trainHistory, (sizeof(trainStateInfo)*120));
  ptidsl->trainStateInx=0;
  return DSLHAL_ERROR_NO_ERRORS;
}

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
unsigned int dslhal_advcfg_logEocMessages(tidsl_t * ptidsl, unsigned int usDs, unsigned int eocLowerBytes, unsigned int eocUpperBytes)
{
  if(usDs==0)
    {
      ptidsl->usEocMsgInx++;
      if(ptidsl->usEocMsgInx>29)
         ptidsl->usEocMsgInx=0;
      ptidsl->usEocMsgBuf[ptidsl->usEocMsgInx].infoBits = (unsigned char)(eocUpperBytes & 0xff);
      ptidsl->usEocMsgBuf[ptidsl->usEocMsgInx].ctrlBits = (unsigned char)(eocLowerBytes & 0xff);
    }
  else
    {
      ptidsl->dsEocMsgInx++;
      if(ptidsl->dsEocMsgInx>29)
         ptidsl->dsEocMsgInx=0;
      ptidsl->dsEocMsgBuf[ptidsl->dsEocMsgInx].infoBits = (unsigned char)(eocUpperBytes & 0xff);
      ptidsl->dsEocMsgBuf[ptidsl->dsEocMsgInx].ctrlBits = (unsigned char)(eocLowerBytes & 0xff);
    }
  return DSLHAL_ERROR_NO_ERRORS;
    }

/********************************************************************************************
* FUNCTION NAME: dslhal_advcfg_getReasonForDrop()
*
*********************************************************************************************
* DESCRIPTION:
*   Reads the reason for the dropping the previous DSL connection;
* Input
*        tidsl_t *ptidsl : Pointer to application structure
*        void *msgStruct : Pointer to training state struct
*
*
* Return: 0  success
*         1  failed
*
* NOTE:
*   DSP image is based on LITTLE endian
*
********************************************************************************************/
unsigned int dslhal_advcfg_getReasonForDrop(tidsl_t * ptidsl)
{
  int rc;
  DEV_HOST_dspOamSharedInterface_t  dspOamSharedInterface;
  DEV_HOST_modemEnvPublic_t modemEnv;

  dprintf(5,"dslhal_advcfg_getReasonForDrop\n");

  rc = dslhal_support_blockRead(ptidsl->pmainAddr, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  dspOamSharedInterface.modemEnvPublic_p = (DEV_HOST_modemEnvPublic_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemEnvPublic_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemEnvPublic_p, &modemEnv, sizeof(DEV_HOST_modemEnvPublic_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }

  ptidsl->reasonForDrop = dslhal_support_byteSwap32(modemEnv.reasonForDrop);
  dprintf(6,"Reason For Last Connection Drop: %d\n",ptidsl->reasonForDrop);
  modemEnv.reasonForDrop = 0;
  rc=dslhal_support_blockWrite(&modemEnv,dspOamSharedInterface.modemEnvPublic_p,sizeof(DEV_HOST_modemEnvPublic_t));
  if(rc)
    return DSLHAL_ERROR_MODEMENV_API_FAILURE;
  else
    return DSLHAL_ERROR_NO_ERRORS;
}

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
unsigned int dslhal_advcfg_ctrlMaxAvgFineGains(tidsl_t * ptidsl,unsigned int onOff)
{
  int rc;
  DEV_HOST_modemEnvPublic_t modemEnv;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
  if(onOff <0 || onOff >1)
   {
     dprintf(3,"Invalid input parameter \n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }

  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   dspOamSharedInterface.modemEnvPublic_p = (DEV_HOST_modemEnvPublic_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemEnvPublic_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemEnvPublic_p,
                                      &modemEnv, sizeof(DEV_HOST_modemEnvPublic_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   modemEnv.maxAvgFineGainCtrl_f = onOff;
   rc=dslhal_support_blockWrite(&modemEnv,dspOamSharedInterface.modemEnvPublic_p,sizeof(DEV_HOST_modemEnvPublic_t));
   if(rc)
     return DSLHAL_ERROR_MODEMENV_API_FAILURE;
   else
     return DSLHAL_ERROR_NO_ERRORS;
}

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
unsigned int dslhal_advcfg_setMaxAvgFineGain(tidsl_t * ptidsl,short fineGain)
{
  int rc;
  DEV_HOST_modemEnvPublic_t modemEnv;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;

  if(!ptidsl)
   {
     dprintf(3, "Error: PTIDSL pointer invalid\n");
     return DSLHAL_ERROR_INVALID_PARAM;
   }
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;

   rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   dspOamSharedInterface.modemEnvPublic_p = (DEV_HOST_modemEnvPublic_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.modemEnvPublic_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.modemEnvPublic_p,
                                      &modemEnv, sizeof(DEV_HOST_modemEnvPublic_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   modemEnv.maxAverageFineGain = dslhal_support_byteSwap16(fineGain);
   rc=dslhal_support_blockWrite(&modemEnv,dspOamSharedInterface.modemEnvPublic_p,sizeof(DEV_HOST_modemEnvPublic_t));
   if(rc)
     return DSLHAL_ERROR_MODEMENV_API_FAILURE;
   else
     return DSLHAL_ERROR_NO_ERRORS;
}

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
unsigned int dslhal_advcfg_readPhySettings(tidsl_t * ptidsl,void *cfgPtr)
{
  int rc;
  DEV_HOST_oamWrNegoParaDef_t  negoParms;
  DEV_HOST_eocVarDef_t         eocParms;
  DEV_HOST_msg_t               aturMsg;
  DEV_HOST_dspOamSharedInterface_t  dspOamSharedInterface;
  currentPhySettings *cfgStruct;
  cfgStruct = (currentPhySettings *)cfgPtr;

  dprintf(1, "dslhal_advcfg_readPhySettings\n");

  rc = dslhal_support_blockRead(ptidsl->pmainAddr, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  dspOamSharedInterface.oamWriteNegoParams_p = (DEV_HOST_oamWrNegoParaDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.oamWriteNegoParams_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.oamWriteNegoParams_p, &negoParms, sizeof(DEV_HOST_oamWrNegoParaDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  else
    {
    cfgStruct->disableLosFlag = negoParms.disableLosAlarm;
    cfgStruct->marginThreshold = negoParms.marginThreshold;
    cfgStruct->marginMonitorTraining = negoParms.marginMonitorTrning;
    cfgStruct->marginMonitorShowtime = negoParms.marginMonitorShwtme;
    if(negoParms.stdMode == T1413_MODE)
      shim_osMoveMemory((void *)cfgStruct->eocVendorId, (void *)negoParms.t1413VendorId,2);
    if(negoParms.stdMode == GDMT_MODE)
      shim_osMoveMemory((void *)cfgStruct->eocVendorId, (void *)negoParms.gdmtVendorId,8);
    }

    dspOamSharedInterface.aturMsg_p = (DEV_HOST_msg_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.aturMsg_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.aturMsg_p,
                               &aturMsg, sizeof(DEV_HOST_msg_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  else
    {
    cfgStruct->trellisFlag = aturMsg.trellis;
    cfgStruct->rateAdaptFlag = aturMsg.rateAdapt;
    }

    dspOamSharedInterface.eocVar_p = (DEV_HOST_eocVarDef_t *) dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.eocVar_p);

  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.eocVar_p,
                               &eocParms, sizeof(DEV_HOST_eocVarDef_t));
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  else
    {
    shim_osMoveMemory((void *)cfgStruct->eocSerialNumber, (void *)eocParms.serialNumber,32);
    shim_osMoveMemory((void *)cfgStruct->aturConfig, (void *)eocParms.aturConfig,30);
    shim_osMoveMemory((void *)cfgStruct->eocRevisionNumber, (void *)eocParms.revNumber,4);
    }
return DSLHAL_ERROR_NO_ERRORS;
}

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
unsigned int dslhal_advcfg_setBlackOutBits(tidsl_t * ptidsl)
{
  unsigned char rMsgPcbBuf[68], rMsgFmtBuf[2];
  unsigned int i=0,rc=0,adsl2MsgLoc=0;
  if(!ptidsl->blackOutValid_f)
    return DSLHAL_ERROR_NO_ERRORS;
  adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                (ptidsl, RMSGFMT_INDEX);
  rc+= dslhal_support_blockRead((PVOID)adsl2MsgLoc,
                                &rMsgFmtBuf,RMSGFMT_SIZE);
  rMsgFmtBuf[0] |= 0x80;
  rc+= dslhal_support_blockWrite(&rMsgFmtBuf,
                                (PVOID)adsl2MsgLoc,RMSGFMT_SIZE);
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;

  adsl2MsgLoc = dslhal_support_getAdsl2MessageLocation
                (ptidsl, RMSGPCB_INDEX);
  rc+= dslhal_support_blockRead((PVOID)adsl2MsgLoc,
       &rMsgPcbBuf,RMSGPCB_SIZE);

  for(i=0;i<32;i++)
    rMsgPcbBuf[4+i] = ptidsl->blackOutBits[i];
  for(i=0;i<36;i+=4)
    dprintf(4,"Byte:%d = 0x%x  0x%x  0x%x  0x%x \n",i,rMsgPcbBuf[i], rMsgPcbBuf[i+1],
             rMsgPcbBuf[i+2], rMsgPcbBuf[i+3]);
  rc+= dslhal_support_blockWrite(&rMsgPcbBuf,
       (PVOID)adsl2MsgLoc,RMSGPCB_SIZE);
  if(rc)
    return DSLHAL_ERROR_CONFIG_API_FAILURE;

  return DSLHAL_ERROR_NO_ERRORS;
}

//#endif
