#ifdef INTERNAL_BUILD
/*******************************************************************************
* FILE PURPOSE:     DSL HAL Internal ONLY API function Implementation for Sangam
*
********************************************************************************
* FILE NAME:        dsl_hal_internalapi.c
*
* DESCRIPTION:  
*       Contains DSL HAL API functions for Internal Consumption ONLY.
*  
*                    
* (C) Copyright 2003, Texas Instruments, Inc.
*    History
*    Date        File Version      Changed     Notes
*    05Feb03     RamP               RamP   Original Version
*    21Mar03     0.00.03            RamP   Changed byteswap function names
*    07Apr03     0.00.04            RamP   Implemented new error reporting scheme
*                                          Changed Commenting to C style only
*    18Jul03     0.00.05            RamP   Added internal use only diagnostic code
*                                          for testing preset modes 0,1,3,4,5
*                                          Moved ATM/STM BERT APIs to external 
*                                          API module;
*    29Sep03     0.00.06            RamP   Added advanced internal configuration 
*                                          API functions for the following:
*                                            - PGA Parameters 
*                                                % Pga1/2/3, Aeq Parameters 
*                                            - Advanced Phy Parameters
*                                                % TEQ, FEQ, EC Parameters
*                                                % Double Tx/Rx Rate Flags
*                                            - Crosstalk Parameters
*                                                % TEQ Starting pt & Leaky Flag 
*    08Oct03     0.00.07            RamP   Fixed byteswap problems with EC params
*******************************************************************************/
#include <dsl_hal_register.h>
#include <dsl_hal_support.h>
#include <dsl_hal_api.h>

/********************************************************************************************
* FUNCTION NAME: dslhal_internalDiags_anlg_allDiagnosticTests()
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

void dslhal_internalDiags_anlg_allDiagnosticTests(int diagMode, int agcFlag, short pga1, short pga2, short pga3, short aeq)
{
 ITIDSLHW_T *ptidsl;
 int  rc=0;
 unsigned char missingTones[64];
 int cmd;
 int tag;
 unsigned int  cpuFrequency=0;
 dprintf(4,"dslhal_diags_anlg_ecNoiseTest()\n");

 cpuFrequency= shim_osGetCpuFrequency();

 ptidsl=(ITIDSLHW_T *) shim_osAllocateMemory(sizeof(ITIDSLHW_T));
 if(ptidsl==NULL)
   {
   dprintf(1, "unable to allocate memory for ptidsl\n");
   return;
   }
  shim_osZeroMemory((char *) ptidsl, sizeof(ITIDSLHW_T));
 
   /* Download the Diagnostics Firmware to the DSP */
   for(rc=0;rc<64;rc++)
      missingTones[rc]=1;
   rc = dslhal_diags_codeDownload(ptidsl, missingTones);
   if(rc!=0)
     {
       dprintf(0,"Error Downloading Code to DSP \n");
       return;
     }

   /* Set the Parameters in the Shared Interface Structure */
   rc = dslhal_internalDiags_anlg_setDiagMode(ptidsl,diagMode);
   if(rc!=0)
     {
       dprintf(0,"Error Setting Diag Option\n");
       return;
      }
   rc = dslhal_diags_anlg_setPgaParams(ptidsl,agcFlag,pga1,pga2,pga3,aeq);
   if(rc!=0)
     {
       dprintf(0,"Error Setting PGA-AEQ parameters \n");
       return;
     }
 /*********** Start the actual test **********************/

   dprintf(0,"Rx Noise Test\n");
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_ALL_ADIAG, 0, 0, 0);
   if (rc)
     return;
    while(1)
     {
     while (dslhal_support_readDspMailbox(ptidsl,&cmd, &tag, 0, 0) == 0)
       {
       dprintf(6,"mailbox message: 0x%x\n", cmd);
       if (cmd == DSP_IDLE) 
         {
         dprintf(5,"DSP_IDLE\n");
      
         /* command DSP to ACTREQ */
         rc = dslhal_support_writeHostMailbox(ptidsl, HOST_ACTREQ, 0, 0, 0);
         if (rc)
           return;
         }
       if (cmd == DSP_QUIT_ADIAG) 
         {
         dprintf(0,"DSP_ALL_ADIAG\n");
         dslhal_internalDiags_anlg_getDiagnosticTestResults(ptidsl);
         return;
         } 
       }
     }

 dprintf(4,"dslhal_internalDiags_anlg_ecNoiseTest() done\n");
 return;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_internalDiags_setDiagMode(tidsl_t *ptidsl,int diagMode)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the PGA and AEQ parameters
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int flag; //if flag = TRUE set rateadapt flag else reset it
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_internalDiags_anlg_setDiagMode(tidsl_t *ptidsl,int diagMode)
{
   DEV_HOST_diagAnlgInputVar_t analogInput;
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc;
   dprintf(5," dslhal_diags_anlg_setPgaParams()\n");
   
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
   if(diagMode==2 || diagMode >5)
     {
       dprintf(3, "Error: Valid Diag Modes are 0,1,3,4,5\n");
       return DSLHAL_ERROR_INVALID_PARAM;
     }     

   dspOamSharedInterface.analogInputVar_p = (DEV_HOST_diagAnlgInputVar_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.analogInputVar_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.analogInputVar_p,
                                      &analogInput, sizeof(DEV_HOST_diagAnlgInputVar_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   analogInput.diagMode = diagMode; 
     //dslhal_support_byteSwap32((unsigned int)(diagMode));

   dslhal_support_blockWrite(&analogInput,(PVOID)dspOamSharedInterface.analogInputVar_p,sizeof(DEV_HOST_diagAnlgInputVar_t)); 
   
   dprintf(5," dslhal_diags_anlg_setPgaParams() Done\n");
   return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_diags_getDiagnosticTestResults(tidsl_t *ptidsl)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the PGA and AEQ parameters
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int flag; //if flag = TRUE set rateadapt flag else reset it
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_internalDiags_anlg_getDiagnosticTestResults(tidsl_t *ptidsl)
{
   DEV_HOST_diagAnlgOutputVar_t analogOutput;
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc;
   dprintf(5," dslhal_diags_anlg_setPgaParams()\n");
   
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
   
   dspOamSharedInterface.analogOutputVar_p = (DEV_HOST_diagAnlgOutputVar_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.analogOutputVar_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.analogOutputVar_p,
                                      &analogOutput, sizeof(DEV_HOST_diagAnlgOutputVar_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   /* Dump Rx Noise Power */
   analogOutput.rxNoisePower_p[0] = (int *)dslhal_support_byteSwap32((unsigned int)analogOutput.rxNoisePower_p[0]);
   analogOutput.rxNoisePower_p[1] = (int *)dslhal_support_byteSwap32((unsigned int)analogOutput.rxNoisePower_p[1]);
   rc = dslhal_support_blockRead((PVOID)analogOutput.rxNoisePower_p[0],&ptidsl->AppData.rxNoisePower0, 256*sizeof(int));
   rc = dslhal_support_blockRead((PVOID)analogOutput.rxNoisePower_p[1],&ptidsl->AppData.rxNoisePower1, 256*sizeof(int));
   dprintf(-5,"Rx Noise Power 0 \n");
   dprintf(-5,"2131 1 80000000 1 100\n");
   for(rc=0;rc<256;)
     {
      dprintf(-5,"0x%08x\n",dslhal_support_byteSwap32(ptidsl->AppData.rxNoisePower0[rc++]));
      if((rc%8==0)&&(rc!=0))
         dprintf(5,"\n");
     }
   dprintf(-5,"Rx Noise Power 1 \n");
   dprintf(-5,"2131 1 81000000 1 100\n");
   for(rc=0;rc<256;)
     {
      dprintf(-5,"0x%08x\n",dslhal_support_byteSwap32(ptidsl->AppData.rxNoisePower1[rc++]));
      if((rc%8==0)&&(rc!=0))
         dprintf(5,"\n");
     }
   /* End of Rx Noise Power Dump */  

   /* Dump Rx Signal Power */
   analogOutput.rxSignalPower_p[0] = (int *)dslhal_support_byteSwap32((unsigned int)analogOutput.rxSignalPower_p[0]);
   analogOutput.rxSignalPower_p[1] = (int *)dslhal_support_byteSwap32((unsigned int)analogOutput.rxSignalPower_p[1]);
   rc = dslhal_support_blockRead((PVOID)analogOutput.rxSignalPower_p[0],&ptidsl->internalVars.rxSignalPower0, 256*sizeof(int));
   rc = dslhal_support_blockRead((PVOID)analogOutput.rxSignalPower_p[1],&ptidsl->internalVars.rxSignalPower1, 256*sizeof(int));
   dprintf(-5,"Rx Signal Power 0 \n");
   dprintf(-5,"2131 1 82000000 1 100\n");
   for(rc=0;rc<256;)
     {
      dprintf(-5,"0x%08x\n",dslhal_support_byteSwap32(ptidsl->internalVars.rxSignalPower0[rc++]));
      if((rc%8==0)&&(rc!=0))
         dprintf(5,"\n");
     }
   dprintf(-5,"Rx Signal Power 1 \n");
   dprintf(-5,"2131 1 83000000 1 100\n");
   for(rc=0;rc<256;)
     {
      dprintf(-5,"0x%08x\n",dslhal_support_byteSwap32(ptidsl->internalVars.rxSignalPower1[rc++]));
      if((rc%8==0)&&(rc!=0))
         dprintf(5,"\n");
     }
   /* End of Rx Signal Power Dump */  

   /* Dump Rx SNR */
   analogOutput.rxSnr_p[0] = (int *)dslhal_support_byteSwap32((unsigned int)analogOutput.rxSnr_p[0]);
   analogOutput.rxSnr_p[1] = (int *)dslhal_support_byteSwap32((unsigned int)analogOutput.rxSnr_p[1]);
   rc = dslhal_support_blockRead((PVOID)analogOutput.rxSnr_p[0],&ptidsl->internalVars.rxSnr0, 256*sizeof(int));
   rc = dslhal_support_blockRead((PVOID)analogOutput.rxSnr_p[1],&ptidsl->internalVars.rxSnr1, 256*sizeof(int));
   dprintf(-5,"Rx SNR 0 \n");
   dprintf(-5,"2131 1 84000000 1 100\n");
   for(rc=0;rc<256;)
     {
      dprintf(-5,"0x%08x\n",dslhal_support_byteSwap32(ptidsl->internalVars.rxSnr0[rc++]));
      if((rc%8==0)&&(rc!=0))
         dprintf(5,"\n");
     }
   dprintf(-5,"Rx SNR 1 \n");
   dprintf(-5,"2131 1 85000000 1 100\n");
   for(rc=0;rc<256;)
     {
      dprintf(-5,"0x%08x\n",dslhal_support_byteSwap32(ptidsl->internalVars.rxSnr1[rc++]));
      if((rc%8==0)&&(rc!=0))
         dprintf(5,"\n");
     }
   /* End of Rx SNR Dump */  

   /* Dump Rx Subchannel Capacity */
   analogOutput.rxSubChannelCapacity_p[0] = (int *)dslhal_support_byteSwap32((unsigned int)analogOutput.rxSubChannelCapacity_p[0]);
   analogOutput.rxSubChannelCapacity_p[1] = (int *)dslhal_support_byteSwap32((unsigned int)analogOutput.rxSubChannelCapacity_p[1]);
   rc = dslhal_support_blockRead((PVOID)analogOutput.rxSubChannelCapacity_p[0],&ptidsl->internalVars.rxSubChannelCapacity0, 256*sizeof(int));
   rc = dslhal_support_blockRead((PVOID)analogOutput.rxSubChannelCapacity_p[1],&ptidsl->internalVars.rxSubChannelCapacity1, 256*sizeof(int));
   dprintf(-5,"Rx SubChannelCapacity 0 \n");
   dprintf(-5,"2131 1 86000000 1 100\n");
   for(rc=0;rc<256;)
     {
      dprintf(-5,"0x%08x\n",dslhal_support_byteSwap32(ptidsl->internalVars.rxSubChannelCapacity0[rc++]));
      if((rc%8==0)&&(rc!=0))
         dprintf(5,"\n");
     }
   dprintf(-5,"Rx SubChannelCapacity 1 \n");
   dprintf(-5,"2131 1 87000000 1 100\n");
   for(rc=0;rc<256;)
     {
      dprintf(-5,"0x%08x\n",dslhal_support_byteSwap32(ptidsl->internalVars.rxSubChannelCapacity1[rc++]));
      if((rc%8==0)&&(rc!=0))
         dprintf(5,"\n");
     }
   /* End of Rx SubChannelCapacity Dump */  

   dprintf(5," dslhal_diags_anlg_getRxNoisePower() Done\n");
   return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_internalcfg_setPgaParams(tidsl_t *ptidsl,int agcFlag,short pga1, short pga2, short pga3, short aeq)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the PGA and AEQ parameters
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int flag; //if flag = TRUE set rateadapt flag else reset it
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_internalcfg_setPgaParams(tidsl_t *ptidsl,short pga1, short pga2, short pga3, short aeq)
{
   DEV_HOST_agcSetting_t agcSettings;
   DEV_HOST_dspOamInternalSharedInterface_t *pdspOamInternalInterface, dspOamInternalInterface;
   int rc;
   char *tmp = (char *) DEV_HOST_DSP_OAM_INTERNAL_POINTER_LOCATION;
 
   dprintf(5," dslhal_diags_anlg_setPgaParams()\n");
   
   if(!ptidsl)
     {
       dprintf(3, "Error: PTIDSL pointer invalid\n");
       return DSLHAL_ERROR_INVALID_PARAM;
     }

  rc = dslhal_support_blockRead(tmp, &pdspOamInternalInterface, sizeof(unsigned int));
  dprintf(5, "tmp=0x%X, addr=0x%X\n", (unsigned int)tmp, (unsigned int)pdspOamInternalInterface);
  pdspOamInternalInterface= (DEV_HOST_dspOamInternalSharedInterface_t *)dslhal_support_byteSwap32((unsigned int)pdspOamInternalInterface);
  dprintf(5, "tmp=0x%X, addr=0x%X\n", (unsigned int)tmp, (unsigned int)pdspOamInternalInterface);
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  
  if(!pdspOamInternalInterface)
    {
    dprintf(1,"Couldn't read main pointer\n");
    return DSLHAL_ERROR_INVALID_PARAM;
    }
  
   rc = dslhal_support_blockRead(pdspOamInternalInterface, &dspOamInternalInterface,sizeof(DEV_HOST_dspOamInternalSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   dspOamInternalInterface.agcSetting_p = (DEV_HOST_agcSetting_t *)dslhal_support_byteSwap32((unsigned int)dspOamInternalInterface.agcSetting_p);
   rc = dslhal_support_blockRead((PVOID)dspOamInternalInterface.agcSetting_p,
                                      &agcSettings, sizeof(DEV_HOST_agcSetting_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   rc = 0;
   if(pga1 < 0 || pga1 > 15)
     rc++;
   if(pga2 < 0 || pga2 > 7)
     rc++;
   if(pga3 < 0 || pga3 > 3)
     rc++;
   if(aeq < 0 || aeq > 5)
     rc++;
   if(rc)
     {
       dprintf(1,"Unable to configure PGA Settings: Invalid Input !\n");
       rc = 0;
       return DSLHAL_ERROR_INVALID_PARAM;
      }
   else
     {
       agcSettings.rxPga1 = dslhal_support_byteSwap16((unsigned short)pga1);
       agcSettings.rxPga2 = dslhal_support_byteSwap16((unsigned short)pga2);
       agcSettings.rxPga3 = dslhal_support_byteSwap16((unsigned short)pga3);
       agcSettings.anlgEq = dslhal_support_byteSwap16((unsigned short)aeq);
     }
   dslhal_support_blockWrite(&agcSettings,(PVOID)dspOamInternalInterface.agcSetting_p,sizeof(DEV_HOST_agcSetting_t)); 
   
   dprintf(5," dslhal_internalcfg_setPgaParams() Done\n");
   return DSLHAL_ERROR_NO_ERRORS;
}


/******************************************************************************************
* FUNCTION NAME:    dslhal_internalcfg_getCrossTalkParams(tidsl_t *ptidsl)
*
*******************************************************************************************
* DESCRIPTION: This fuction gets the Crosstalk parameters into app interface
*
* INPUT:  PITIDSLHW_T *ptidsl
*        
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_internalcfg_getCrossTalkParams(tidsl_t *ptidsl)
{
   DEV_HOST_crossTalkPara_t crossTalkParams;
   DEV_HOST_dspOamInternalSharedInterface_t *pdspOamInternalInterface, dspOamInternalInterface;
   int rc;
   char *tmp = (char *) DEV_HOST_DSP_OAM_INTERNAL_POINTER_LOCATION;
 
   dprintf(5," Get Crosstalk Parameters\n");
   
   if(!ptidsl)
     {
       dprintf(3, "Error: PTIDSL pointer invalid\n");
       return DSLHAL_ERROR_INVALID_PARAM;
     }

  rc = dslhal_support_blockRead(tmp, &pdspOamInternalInterface, sizeof(unsigned int));
  dprintf(5, "tmp=0x%X, addr=0x%X\n", (unsigned int)tmp, (unsigned int)pdspOamInternalInterface);
  pdspOamInternalInterface= (DEV_HOST_dspOamInternalSharedInterface_t *)dslhal_support_byteSwap32((unsigned int)pdspOamInternalInterface);
  dprintf(5, "tmp=0x%X, addr=0x%X\n", (unsigned int)tmp, (unsigned int)pdspOamInternalInterface);
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  
  if(!pdspOamInternalInterface)
    {
    dprintf(1,"Couldn't read main pointer\n");
    return DSLHAL_ERROR_INVALID_PARAM;
    }
  
   rc = dslhal_support_blockRead(pdspOamInternalInterface, &dspOamInternalInterface,sizeof(DEV_HOST_dspOamInternalSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   dspOamInternalInterface.crossTalkPara_p = (DEV_HOST_crossTalkPara_t *)dslhal_support_byteSwap32((unsigned int)dspOamInternalInterface.crossTalkPara_p);
   rc = dslhal_support_blockRead((PVOID)dspOamInternalInterface.crossTalkPara_p,
                            &crossTalkParams, sizeof(DEV_HOST_crossTalkPara_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   ptidsl->internalVars.teqStartingPoint = crossTalkParams.teqStartingPoint;   
   ptidsl->internalVars.teqLeakyFlag = crossTalkParams.leakyTeqFlag;   
   ptidsl->internalVars.rxHpfFlag = crossTalkParams.rxHpfFlag;   
   /* For Debug */
   dprintf(5,"Teq St Pt: %d  Teq Leaky Flag %d Rx Hpf:%d \n",
           ptidsl->internalVars.teqStartingPoint,
           ptidsl->internalVars.teqLeakyFlag,
           ptidsl->internalVars.rxHpfFlag);
   /*End of Debug */
   dprintf(5," dslhal_internalcfg_getCrossTalkParams() Done\n");
   return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_internalcfg_getPhyParams(tidsl_t *ptidsl)
*
*******************************************************************************************
* DESCRIPTION: This fuction gets the FEQ, TEQ, EC Paramteres
*
* INPUT:  PITIDSLHW_T *ptidsl
*        
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_internalcfg_getPhyParams(tidsl_t *ptidsl)
{
   PHY_EC_ecData_t ecParameters;
   DEV_HOST_modemEnv_t internalModemEnv;
   DEV_HOST_teqFeqEc_t teqFeqEcParams;
   DEV_HOST_dspOamInternalSharedInterface_t *pdspOamInternalInterface, dspOamInternalInterface;
   int rc;
   char *tmp = (char *) DEV_HOST_DSP_OAM_INTERNAL_POINTER_LOCATION;
 
   dprintf(5," Get Internal Phy Parameters\n");
   
   if(!ptidsl)
     {
       dprintf(3, "Error: PTIDSL pointer invalid\n");
       return DSLHAL_ERROR_INVALID_PARAM;
     }

  rc = dslhal_support_blockRead(tmp, &pdspOamInternalInterface, sizeof(unsigned int));
  dprintf(5, "tmp=0x%X, addr=0x%X\n", (unsigned int)tmp, (unsigned int)pdspOamInternalInterface);
  pdspOamInternalInterface= (DEV_HOST_dspOamInternalSharedInterface_t *)dslhal_support_byteSwap32((unsigned int)pdspOamInternalInterface);
  dprintf(5, "tmp=0x%X, addr=0x%X\n", (unsigned int)tmp, (unsigned int)pdspOamInternalInterface);
  if (rc)
    {
    dprintf(1,"dslhal_support_blockRead failed\n");
    return DSLHAL_ERROR_BLOCK_READ;
    }
  
  if(!pdspOamInternalInterface)
    {
    dprintf(1,"Couldn't read main pointer\n");
    return DSLHAL_ERROR_INVALID_PARAM;
    }
  
   rc = dslhal_support_blockRead(pdspOamInternalInterface, &dspOamInternalInterface,sizeof(DEV_HOST_dspOamInternalSharedInterface_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   dspOamInternalInterface.teqFeqEc_p = (DEV_HOST_teqFeqEc_t *)dslhal_support_byteSwap32((unsigned int)dspOamInternalInterface.teqFeqEc_p);
   rc = dslhal_support_blockRead((PVOID)dspOamInternalInterface.teqFeqEc_p,
                            &teqFeqEcParams, sizeof(DEV_HOST_teqFeqEc_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   
   teqFeqEcParams.looptype_p = (int *)dslhal_support_byteSwap32((unsigned int)teqFeqEcParams.looptype_p);
   rc = dslhal_support_blockRead((PVOID)teqFeqEcParams.looptype_p,
                            &ptidsl->internalVars.loopType, sizeof(int));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   teqFeqEcParams.pilotSubChannel_p = (int *)dslhal_support_byteSwap32((unsigned int)teqFeqEcParams.pilotSubChannel_p);
   rc = dslhal_support_blockRead((PVOID)teqFeqEcParams.pilotSubChannel_p,
                            &ptidsl->internalVars.pilotSubChannel, sizeof(int));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   ptidsl->internalVars.pilotSubChannel = dslhal_support_byteSwap32((unsigned int)ptidsl->internalVars.pilotSubChannel);
   ptidsl->internalVars.loopType = dslhal_support_byteSwap32((unsigned int)ptidsl->internalVars.loopType);
   /* For Debug */
   dprintf(5,"Pilot Subchannel: %x  Looptype %x \n",
           ptidsl->internalVars.pilotSubChannel,
           ptidsl->internalVars.loopType);
   /*End of Debug */

   teqFeqEcParams.teqCoeff_p = (short *)dslhal_support_byteSwap32((unsigned int)teqFeqEcParams.teqCoeff_p);
   rc = dslhal_support_blockRead((PVOID)teqFeqEcParams.teqCoeff_p,
                            &ptidsl->internalVars.teqCoeffs, (NUM_TEQ_COEFFICIENTS*sizeof(short)));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   teqFeqEcParams.feqCoeff_p = (complex_int *)dslhal_support_byteSwap32((unsigned int)teqFeqEcParams.feqCoeff_p);
   rc = dslhal_support_blockRead((PVOID)teqFeqEcParams.feqCoeff_p,
                            &ptidsl->internalVars.feqCoeffs, (NUM_FEQ_COEFFICIENTS*sizeof(complex_int)));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   teqFeqEcParams.ecData_p = (PHY_EC_ecData_t *)dslhal_support_byteSwap32((unsigned int)teqFeqEcParams.ecData_p);
   rc = dslhal_support_blockRead((PVOID)teqFeqEcParams.ecData_p,
                            &ecParameters, sizeof(PHY_EC_ecData_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   shim_osMoveMemory((void *)ptidsl->internalVars.ecFdEstimate, (void *)ecParameters.fdEcho,(2*(NUM_EC_SAMPLESIZE*sizeof(complex_int))));
   shim_osMoveMemory((void *)ptidsl->internalVars.ecTdEstimate, (void *)ecParameters.tdEcho,(NUM_EC_SAMPLESIZE*sizeof(short)));
   shim_osMoveMemory((void *)ptidsl->internalVars.ecTdCoeffs, (void *)ecParameters.filter, (NUM_EC_COEFFICIENTS*sizeof(short)));
   
   ptidsl->internalVars.ecPathFrameDelay = (short)dslhal_support_byteSwap16((unsigned short)ecParameters.frameDelay);
   ptidsl->internalVars.ecPathSampleDelay = (short)dslhal_support_byteSwap16((unsigned short)ecParameters.sampleDelay);
   ptidsl->internalVars.ecDelay = (short)dslhal_support_byteSwap16((unsigned short)ecParameters.delay);
   ptidsl->internalVars.ecScale = (short)dslhal_support_byteSwap16((unsigned short)ecParameters.scale);

   /* For Debug */
   dprintf(5,"Ec Delay: %x  Ec Scale %x \n 
              Ec Frame Delay: %x  Ec Sample Delay: %x\n",
           ptidsl->internalVars.ecDelay,
           ptidsl->internalVars.ecScale,
           ptidsl->internalVars.ecPathFrameDelay,
           ptidsl->internalVars.ecPathSampleDelay);
   /*End of Debug */
   dspOamInternalInterface.modemEnv_p = (DEV_HOST_modemEnv_t *)dslhal_support_byteSwap32((unsigned int)dspOamInternalInterface.modemEnv_p);
   rc = dslhal_support_blockRead((PVOID)dspOamInternalInterface.modemEnv_p,
                            &internalModemEnv, sizeof(DEV_HOST_modemEnv_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   

   internalModemEnv.doubleXmtRate_p = (int *)dslhal_support_byteSwap32((unsigned int)internalModemEnv.doubleXmtRate_p);
   rc = dslhal_support_blockRead((PVOID)internalModemEnv.doubleXmtRate_p,
                            &ptidsl->internalVars.doubleXmtRate, sizeof(int));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }

   internalModemEnv.doubleRxRate_p = (int *)dslhal_support_byteSwap32((unsigned int)internalModemEnv.doubleRxRate_p);
   rc = dslhal_support_blockRead((PVOID)internalModemEnv.doubleRxRate_p,
                            &ptidsl->internalVars.doubleRxRate, sizeof(int));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   dprintf(5," dslhal_internalcfg_getPhyParams() Done\n");
   return DSLHAL_ERROR_NO_ERRORS;
}

#endif
