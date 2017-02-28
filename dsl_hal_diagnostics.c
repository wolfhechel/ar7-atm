#ifndef NO_DIAG
/*******************************************************************************
* FILE PURPOSE:     DSL Driver API functions for Avalanche
*
********************************************************************************
* FILE NAME:        dsl_hal_diagnostics.c
*
* DESCRIPTION:
*       Contains DSL HAL Analog and Digital Diagnostics implementation.
*
*
* (C) Copyright 2001-02, Texas Instruments, Inc.
*    History
*    Date        Version            Notes
*    06Feb03     0.00.00            RamP   Created
*    21Mar03     0.00.01            RamP   Changed byteswap function names
*    07Apr03     0.00.02            RamP   Implemented new error reporting scheme
*                                          Changed Commenting to C style only
*    10Apr03     0.00.04            RamP   Removed ptidsl from loaddebugFWImage
*                                          function;  moved size and fwimage
*                                          initialization to codeDownload functn
*    16Apr03     0.00.05            RamP   Fixed host interface struct changes
*    18Jul03     0.01.00            RamP   Implemented the API functions for
*                                          analog & digital diagnostics
*                                          TonesTestA/B, ecPowerTest, rxNoiseTest
*                                          getNoisePower, Diagnostic code download
*                                          setPgaParams, setMissingTones fns added
*    02Mar04     0.01.01            RamP   Changed overlay page number constant to
*                                          refer to the host interface constant
*    20Apr04     0.01.02            RamP   decoupled overlay page check from host
*                                          interface, added scratch memory support
+
*    29Apr04     0.01.03            RamP   Accomodated higher number of DS Tones
*                                          when training in ADSL2+ mode
*    02Feb05     0.01.04            CPH    Pull dev_host_interface.h out from dsl_hal_register.h
*    29Jul05     0.01.05            CPH    Change dslhal_support_getNumTones() to dslhal_support_getMaxDsTones()
*******************************************************************************/

#include <dev_host_interface.h>
#include <dsl_hal_register.h>
#include <dsl_hal_support.h>


/********************************************************************************************
* FUNCTION NAME: dslhal_diags_digi_memTestA()
*
*********************************************************************************************
* DESCRIPTION: This function is a combination of 8 different tests selected by an input array
*              The tests included in this function are Internal and external memory  data,
*             .address and data dependency tests besides the codec interconnection and the
*              DSP ID
*
* Input:       Test Array indicating which test to run.
*              Test 0 -> Basic RAM Data Test
*              Test 1 -> RAM Address Dependency Test
*              Test 2 -> RAM Data Dependency Test
*
*
*
*
* Return:      Status indicator, the bits are set depending upon pass/fail of test
*              Bit is set to 0 if pass and 1 if fail
*              Negative Value is Returned if there is a Hardware/Firmware error
********************************************************************************************/
unsigned int dslhal_diags_digi_memTestA(unsigned int* Test)
{
 ITIDSLHW_T *ptidsl;
  int  status=0, rc;
  unsigned int sts=0;
  int cmd;
  int tag;
  unsigned char missingTones[64];
  unsigned int  cpuFrequency=0;


  dprintf(0,"dslhal_diags_digi_testA()\n");

  cpuFrequency= shim_osGetCpuFrequency();

  ptidsl=(ITIDSLHW_T *) shim_osAllocateMemory(sizeof(ITIDSLHW_T));
  if(ptidsl==NULL)
    {
      dprintf(1, "unable to allocate memory for ptidsl\n");
    return DSLHAL_ERROR_MALLOC;
    }
   shim_osZeroMemory((char *) ptidsl, sizeof(ITIDSLHW_T));

     /* Download the Diagnostics Firmware to the DSP */

   rc = dslhal_diags_codeDownload(ptidsl,missingTones);
   if(rc!=0)
     {
       dprintf(0,"Error Downloading Code to DSP \n");
       return DSLHAL_ERROR_DIAGCODE_DOWNLOAD;
     }


 /* Dynamically Allocate Memory in SDRAM and Pass it on to Host Interface */
  if(Test[0]==1 || Test[1]==1 || Test[2]==1)
    {
      dprintf(0,"Allocating Memory \n");
      rc=dslhal_diags_digi_assignMemTestSpace(ptidsl);
      if(rc!=0)
        {
          dprintf(0,"Digital Diag Memory Allocate Error \n");
          return DSLHAL_ERROR_DIAG_MALLOC;
        }
    }

  if(Test[0]==1)
    {
    dprintf(0,"DSP External Memory Data Test");
    rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DIGITAL_MEM, 0, 0, 0);
    if (rc)
      return -1;
     shim_osClockWait(cpuFrequency);

    while (dslhal_support_readDspMailbox(ptidsl,&cmd, &tag, 0, 0) == 0)
      {
        dprintf(6,"mailbox message: 0x%x\n", cmd);

      if (cmd == DSP_TEST_PASSED)
        {
        dprintf(0,"... Passed\n");
        }
      if (cmd == DSP_TEST_FAILED)
        {
        dprintf(0, "... Failed\n");
        status |= 1<<0;
        rc = dslhal_diags_digi_readMemTestResult(ptidsl,sts);
        if(rc)
          {
            dprintf(0,"Error Reading Memory Test Results \n");
            return DSLHAL_ERROR_DIGIDIAG_FAILURE;
          }
        }
      }
    }
  if(Test[1]==1)
    {
    dprintf(0,"DSP External Memory Address Dependency Test");
    rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DIGITAL_MEM, 0, 1, 0);
    if (rc)
      return DSLHAL_ERROR_MAILBOX_WRITE;
     shim_osClockWait(cpuFrequency);

    while (dslhal_support_readDspMailbox(ptidsl,&cmd, &tag, 0, 0) == 0)
      {
        dprintf(6,"mailbox message: 0x%x\n", cmd);

      if (cmd == DSP_TEST_PASSED)
        {
        dprintf(0,"... Passed\n");
        }
      else if (cmd == DSP_TEST_FAILED)
        {
        dprintf(0, "... Failed\n");
        status |= 1<<1;
        rc = dslhal_diags_digi_readMemTestResult(ptidsl,sts);
        if(rc)
          {
            dprintf(0,"Error Reading Memory Test Results \n");
            return DSLHAL_ERROR_DIGIDIAG_FAILURE;
          }
        }
      }
    }
  if(Test[2]==1)
    {
    dprintf(0,"DSP External Memory Data Dependancy Test");
    rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DIGITAL_MEM, 0, 2, 0);
    if (rc)
      return DSLHAL_ERROR_MAILBOX_WRITE;
     shim_osClockWait(cpuFrequency);

    while (dslhal_support_readDspMailbox(ptidsl,&cmd, &tag, 0, 0) == 0)
      {
        dprintf(6,"mailbox message: 0x%x\n", cmd);

      if (cmd == DSP_TEST_PASSED)
        {
        dprintf(0,"... Passed\n");
        }
      else if (cmd == DSP_TEST_FAILED)
        {
        dprintf(0, "... Failed\n");
        status |= 1<<2;
        rc = dslhal_diags_digi_readMemTestResult(ptidsl,sts);
        if(rc)
          {
            dprintf(0,"Error Reading Memory Test Results \n");
            return DSLHAL_ERROR_DIGIDIAG_FAILURE;
          }
        }
      }
    }

  dprintf(0,"dslhal_diags_digi_testA() done\n");
  return status;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_diags_digi_memTestB()
*
*********************************************************************************************
* DESCRIPTION: This function is a combination of 8 different tests selected by an input array
*              The tests included in this function are Internal and external memory  data,
*             .address and data dependency tests besides the codec interconnection and the
*              DSP ID
*
* Input:       Test Array which indicates what tests are to be run
*
* Return:      The Status array or an error condition
*
********************************************************************************************/
unsigned int dslhal_diags_digi_memTestB(unsigned int   Test, unsigned int* Status)
{
 ITIDSLHW_T *ptidsl;
  int  status=0,rc;
  unsigned int sts=0;
  int cmd;
  int tag;
  unsigned char missingTones[64];
  unsigned int  cpuFrequency=0;
  int param1;

  dprintf(0,"dslhal_diags_digi_testB()\n");

  cpuFrequency= shim_osGetCpuFrequency();

  ptidsl=(ITIDSLHW_T *) shim_osAllocateMemory(sizeof(ITIDSLHW_T));
  if(ptidsl==NULL)
    {
    dprintf(1, "unable to allocate memory for ptidsl\n");
    return DSLHAL_ERROR_MALLOC;
    }
   shim_osZeroMemory((char *) ptidsl, sizeof(ITIDSLHW_T));
   /* Download the Diagnostics Firmware to the DSP */

   rc = dslhal_diags_codeDownload(ptidsl,missingTones);
   if(rc!=0)
     {
       dprintf(0,"Error Downloading Code to DSP \n");
       return DSLHAL_ERROR_DIAGCODE_DOWNLOAD;
     }

 /* Dynamically Allocate Memory in SDRAM and Pass it on to Host Interface */
  if(Test==0 || Test==1 || Test==2)
    {
      dprintf(0,"Allocating Memory \n");
      rc=dslhal_diags_digi_assignMemTestSpace(ptidsl);
      if(rc!=0)
        {
          dprintf(0,"Digital Diag Memory Allocate Error \n");
          return DSLHAL_ERROR_DIAG_MALLOC;
        }
    }

  /*********** Start the actual test **********************/

  if(Test==0)
    {
    dprintf(0,"DSP External Memory Data Test\n");
    rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DIGITAL_MEM, 0, 0, 0);
    if (rc)
      return DSLHAL_ERROR_MAILBOX_WRITE;
     shim_osClockWait(cpuFrequency);

    while (dslhal_support_readDspMailbox(ptidsl,&cmd, &tag, &param1, 0) == 0)
      {
      dprintf(6,"mailbox message: 0x%x\n", cmd);

      if (cmd == DSP_TEST_PASSED)
        {
        dprintf(0,"DSP_TEST_PASSED\n");
        *Status = param1;
        return DSLHAL_ERROR_NO_ERRORS;
        }
      if (cmd == DSP_TEST_FAILED)
        {
        dprintf(0,"DSP_TEST_FAILED\n");
        *Status = param1;
        rc = dslhal_diags_digi_readMemTestResult(ptidsl,sts);
        if(rc)
          {
            dprintf(0,"Error Reading Memory Test Results \n");
            return rc;
          }
        return DSLHAL_ERROR_DIGIDIAG_FAILURE;
        }
      }
    }
  if(Test==1)
    {
    dprintf(0,"DSP External Memory Address Test\n");
    rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DIGITAL_MEM, 0, 1, 0);
    if (rc)
      return DSLHAL_ERROR_MAILBOX_WRITE;
     shim_osClockWait(cpuFrequency);

    while (dslhal_support_readDspMailbox(ptidsl,&cmd, &tag, &param1, 0) == 0)
      {
      dprintf(6,"mailbox message: 0x%x\n", cmd);

      if (cmd == DSP_TEST_PASSED)
        {
        dprintf(0,"DSP_TEST_PASSED\n");
        *Status = param1;
        return DSLHAL_ERROR_NO_ERRORS;
        }
      if (cmd == DSP_TEST_FAILED)
        {
        dprintf(0,"DSP_TEST_FAILED\n");
        *Status = param1;
        rc = dslhal_diags_digi_readMemTestResult(ptidsl,sts);
        if(rc)
          {
            dprintf(0,"Error Reading Memory Test Results \n");
            return DSLHAL_ERROR_DIGIDIAG_FAILURE;
          }
        return DSLHAL_ERROR_DIGIDIAG_FAILURE;
        }
      }
    }
  if(Test==2)
    {
    dprintf(0,"DSP External Memory Data Dependancy Test\n");
    rc = dslhal_support_writeHostMailbox(ptidsl, HOST_DIGITAL_MEM, 0, 2, 0);
    if (rc)
      return DSLHAL_ERROR_MAILBOX_WRITE;
     shim_osClockWait(cpuFrequency);

    while (dslhal_support_readDspMailbox(ptidsl,&cmd, &tag, &param1, 0) == 0)
      {
      dprintf(6,"mailbox message: 0x%x\n", cmd);

      if (cmd == DSP_TEST_PASSED)
        {
        dprintf(0,"DSP_TEST_PASSED\n");
        *Status = param1;
        return DSLHAL_ERROR_NO_ERRORS;
        }
      if (cmd == DSP_TEST_FAILED)
        {
        dprintf(0,"DSP_TEST_FAILED\n");
        *Status = param1;
        rc = dslhal_diags_digi_readMemTestResult(ptidsl,sts);
        if(rc)
          {
            dprintf(0,"Error Reading Memory Test Results \n");
            return DSLHAL_ERROR_DIGIDIAG_FAILURE;
          }
        return DSLHAL_ERROR_DIGIDIAG_FAILURE;
        }
      }
    }
  dprintf(0,"dslhal_diags_digi_testB() done\n");
  return status;
}

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

unsigned int dslhal_diags_digi_assignMemTestSpace(tidsl_t *ptidsl)
{
  DEV_HOST_dspOamSharedInterface_t dspOamSharedInterface, *pdspOamSharedInterface;
  DEV_HOST_olayDpDef_t            olayDpParms;
  DEV_HOST_olayDpPageDef_t        olayDpPageDef[32];
  int i,rc;
  ptidsl->olayDpPage[0].PmemStartWtAddr  = (unsigned int) shim_osAllocateMemory(DIGITAL_DIAG_MEMSIZE);
  if(ptidsl->olayDpPage[0].PmemStartWtAddr == NULL)
    {
      dprintf(1, "Memory allocate error\n");
      return DSLHAL_ERROR_MALLOC;
    }
#ifdef PRE_SILICON
  ptidsl->olayDpPage[0].overlayHostAddr = ((((ptidsl->olayDpPage[0].PmemStartWtAddr)-0x84000000)-0x10000000)+0x030b0000);
#else
  ptidsl->olayDpPage[0].overlayHostAddr = ((unsigned int)(ptidsl->olayDpPage[0].PmemStartWtAddr)&~0xe0000000);
#endif
  dprintf(0,"Allocated Addr: 0x%x \t Xlated Addr: 0x%x \n",ptidsl->olayDpPage[0].PmemStartWtAddr,ptidsl->olayDpPage[0].overlayHostAddr);

  ptidsl->olayDpPage[0].overlayHostAddr = (unsigned int)dslhal_support_byteSwap32(ptidsl->olayDpPage[0].overlayHostAddr);
  ptidsl->olayDpPage[0].OverlayXferCount = DIGITAL_DIAG_MEMSIZE;

  /* Communicate the Allocated Memory Address to DSP to do External Memory Test */

  /* Access the DSP-OAM Shared Interface */
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return DSLHAL_ERROR_BLOCK_READ;
    }

  /* Change the Endianness of the olayDpDef pointer */
  dspOamSharedInterface.olayDpParms_p = (DEV_HOST_olayDpDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.olayDpParms_p);
  /* Access the olayDpDef Structure */
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.olayDpParms_p,&olayDpParms, sizeof(DEV_HOST_olayDpDef_t));

  if (rc)
    {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return DSLHAL_ERROR_BLOCK_READ;
    }

  for(i=0;i<1;i++)
    {
      /* Change the endianness of the olayDpPageDef Pointer */
      olayDpParms.olayDpPage_p[i] = (DEV_HOST_olayDpPageDef_t *) dslhal_support_byteSwap32((unsigned int)olayDpParms.olayDpPage_p[i]);
      /* Access the olayDpPageDef Structure */
      rc = dslhal_support_blockRead((PVOID)olayDpParms.olayDpPage_p[i],&olayDpPageDef[i],sizeof(DEV_HOST_olayDpPageDef_t));
      if (rc)
        return rc;
      dprintf(5,"Current HostAddr:0x%x\n",olayDpPageDef[i].overlayHostAddr);
      olayDpPageDef[i].overlayHostAddr = ptidsl->olayDpPage[i].overlayHostAddr;
      rc = dslhal_support_blockWrite(&olayDpPageDef[i],(PVOID)olayDpParms.olayDpPage_p[i],sizeof(DEV_HOST_olayDpPageDef_t));
      if (rc)
        return DSLHAL_ERROR_BLOCK_WRITE;
    }
  return DSLHAL_ERROR_NO_ERRORS;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_diags_digi_readMemTestResult()
*
*********************************************************************************************
* DESCRIPTION: Assigns Memory Space in SDRAM for External Memory Test
* Input: tidsl_t *ptidsl
*
* Return: 0    success
*         1    failed
*
********************************************************************************************/

unsigned int dslhal_diags_digi_readMemTestResult(tidsl_t *ptidsl,unsigned int testResult)
{
  DEV_HOST_dspOamSharedInterface_t dspOamSharedInterface, *pdspOamSharedInterface;
  DEV_HOST_olayDpDef_t            olayDpParms;
  DEV_HOST_olayDpPageDef_t        olayDpPageDef;
  int rc;

  /* Access the DSP-OAM Shared Interface */
  pdspOamSharedInterface = (DEV_HOST_dspOamSharedInterfacePtr_t) ptidsl->pmainAddr;
  rc = dslhal_support_blockRead(pdspOamSharedInterface, &dspOamSharedInterface,sizeof(DEV_HOST_dspOamSharedInterface_t));
  if (rc)
    {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return DSLHAL_ERROR_BLOCK_READ;
    }

  /* Change the Endianness of the olayDpDef pointer */
  dspOamSharedInterface.olayDpParms_p = (DEV_HOST_olayDpDef_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.olayDpParms_p);
  /* Access the olayDpDef Structure */
  rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.olayDpParms_p,&olayDpParms, sizeof(DEV_HOST_olayDpDef_t));

  if (rc)
    {
      dprintf(1,"dslhal_support_blockRead failed\n");
      return DSLHAL_ERROR_BLOCK_READ;
    }

      /* Change the endianness of the olayDpPageDef Pointer */
      olayDpParms.olayDpPage_p[0] = (DEV_HOST_olayDpPageDef_t *) dslhal_support_byteSwap32((unsigned int)olayDpParms.olayDpPage_p[0]);
      /* Access the olayDpPageDef Structure */
      rc = dslhal_support_blockRead((PVOID)olayDpParms.olayDpPage_p[0],&olayDpPageDef,sizeof(DEV_HOST_olayDpPageDef_t));
      if (rc)
        return DSLHAL_ERROR_BLOCK_READ;
      dprintf(5,"Current HostAddr:0x%x\n",olayDpPageDef.overlayHostAddr);
      testResult = olayDpPageDef.overlayHostAddr;

  return DSLHAL_ERROR_NO_ERRORS;
}


/********************************************************************************************
* FUNCTION NAME: dslhal_diags_codeDownload()
*
*********************************************************************************************
* DESCRIPTION: Brings DSLSS out of Reset, Downloads Diag Firmware,
*              brings DSP out of Reset
* Input: tidsl_t *ptidsl
* Return: 0    success
*         1    failed
*
********************************************************************************************/

unsigned int dslhal_diags_codeDownload(tidsl_t *ptidsl, unsigned char* missingTones)
{
  int i,rc;
  unsigned char *dbgFirmwareImage;
 /* reset DSL Subsystem  */
  rc=dslhal_support_unresetDslSubsystem();
  if(rc)
    {
      dprintf(1, "unable to reset Dsl Subsystem!! \n");
     shim_osFreeMemory((char *) ptidsl, sizeof(ITIDSLHW_T));
    return DSLHAL_ERROR_UNRESET_ADSLSS;
    }

  ptidsl->fwimage = shim_osAllocateMemory(DSP_FIRMWARE_MALLOC_SIZE);
  if(!ptidsl->fwimage)
    {
      dprintf(1,"Failed to Allocate Memory for DSP firmware binary \n");
      return DSLHAL_ERROR_FIRMWARE_MALLOC;
    }
  /* read firmware file from flash   */
  dbgFirmwareImage=(unsigned char *)ptidsl->fwimage;
  rc=shim_osLoadDebugFWImage(dbgFirmwareImage);
  if(rc<0)
    {
    dprintf(1, "unable to get fw image\n");
    shim_osFreeVMemory((char *)ptidsl->fwimage,DSP_FIRMWARE_MALLOC_SIZE);
    shim_osFreeMemory((char *) ptidsl, sizeof(ITIDSLHW_T));
    return DSLHAL_ERROR_NO_FIRMWARE_IMAGE;
    }
  else
    {
      ptidsl->imagesize = rc;
     }

  /* load fw to DSP  */
  if(dslhal_support_hostDspCodeDownload(ptidsl))
    {
      dprintf(1,"dsp download error\n");
    for(i=0; i<ptidsl->numOlayPages; i++)
      {
      if(ptidsl->olayDpPage[i].PmemStartWtAddr !=NULL)
        {
         shim_osFreeMemory((char *) ptidsl->olayDpPage[i].PmemStartWtAddr,
                     ptidsl->olayDpPage[i].OverlayXferCount);
        }
      }
     shim_osFreeMemory((char *) ptidsl, sizeof(ITIDSLHW_T));
    return DSLHAL_ERROR_DIAGCODE_DOWNLOAD;
    }
  rc = dslhal_diags_anlg_setMissingTones(ptidsl,missingTones);
  if(rc)
    {
      dprintf(1,"Error Setting Missing Tones Array \n");
      return DSLHAL_ERROR_INVALID_PARAM;
    }
  /* unreset DSP */
  rc=dslhal_support_unresetDsp();
  if (rc)
    {
      dprintf(1,"unable to wakeup DSP \n");
    for(i=0; i<ptidsl->numOlayPages; i++)
      {
      if(ptidsl->olayDpPage[i].PmemStartWtAddr !=NULL)
        {
         shim_osFreeMemory((char *) ptidsl->olayDpPage[i].PmemStartWtAddr,
                     ptidsl->olayDpPage[i].OverlayXferCount);
        }
      }
     shim_osFreeMemory((char *) ptidsl, sizeof(ITIDSLHW_T));
    return DSLHAL_ERROR_UNRESET_DSP;
    }
  return DSLHAL_ERROR_NO_ERRORS;
}



/********************************************************************************************
* FUNCTION NAME: dslhal_diags_anlg_testA()
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

void dslhal_diags_anlg_tonesTestA(unsigned int Test, unsigned int* Array)
{
 ITIDSLHW_T *ptidsl;
 int  i, rc=0;
 unsigned char missingTones[64];
 int cmd;
 int tag;

 dprintf(0,"dslhal_diags_anlg_testA()\n");
 ptidsl=(ITIDSLHW_T *)shim_osAllocateMemory(sizeof(ITIDSLHW_T));
 if(ptidsl==NULL)
   {
   dprintf(1, "unable to allocate memory for ptidsl\n");
   return;
   }
  shim_osZeroMemory((char *) ptidsl, sizeof(ITIDSLHW_T));

 for(i=0;i<64;i++)
   {
   missingTones[i] = Array[i];
   }
  /* Unreset the ADSL Subsystem  */
  rc=dslhal_support_unresetDslSubsystem();
  if(rc)
    {
    dprintf(1, "unable to reset ADSL Subsystem \n");
    shim_osFreeMemory((void *) ptidsl, sizeof(ITIDSLHW_T));
    /* return DSLHAL_ERROR_UNRESET_ADSLSS; */
    }


   /* Download the Diagnostics Firmware to the DSP */

   rc = dslhal_diags_codeDownload(ptidsl,missingTones);
   if(rc!=0)
     {
       dprintf(0,"Error Downloading Code to DSP \n");
       return;
     }
   rc=dslhal_support_unresetDsp();
   if (rc)
    {
     dprintf(0,"unable to bring DSP out of Reset\n");
     for(i=0; i<ptidsl->numOlayPages; i++)
       {
        if(ptidsl->olayDpPage[i].PmemStartWtAddr !=NULL)
          {
           shim_osFreeDmaMemory((void *) ptidsl->olayDpPage[i].PmemStartWtAddr,
                     ptidsl->olayDpPage[i].OverlayXferCount);
          }
        }
    }
 /*********** Start the actual test **********************/

 if(Test==REVERB)
   {
   dprintf(0,"TX REVERB Test\n");
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_TXREVERB, 0, 0, 0);
   if (rc)
     return;
   while(1)
     {
       while (dslhal_support_readDspMailbox(ptidsl,&cmd, &tag, 0, 0) == 0)
         {
           dprintf(6,"mailbox message: 0x%x\n", cmd);
           if (cmd == DSP_IDLE)
             {
               dprintf(0,"DSP_IDLE\n");

               /* command DSP to ACTREQ */
               rc = dslhal_support_writeHostMailbox(ptidsl, HOST_ACTREQ, 0, 0, 0);
               if (rc)
                 return;
             }
           if (cmd == DSP_TXREVERB)
             {
               dprintf(0,"DSP_TXREVERB\n");
               return;
             }

         }
     }
   }
 if(Test==MEDLEY)
   {
     dprintf(0,"TX MEDLEY Test\n");
     rc = dslhal_support_writeHostMailbox(ptidsl, HOST_TXMEDLEY, 0, 0, 0);
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
             if (cmd == DSP_TXMEDLEY)
               {
                 dprintf(0,"DSP_TXMEDLEY\n");
                 return;
               }

           }
       }
   }
 dprintf(4,"dslhal_diags_anlg_testA() done\n");
 return;
}

/********************************************************************************************
* FUNCTION NAME: dslhal_diags_anlg_testB()
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
void dslhal_diags_anlg_tonesTestB(unsigned int   Test, unsigned int Tones)
{
 ITIDSLHW_T *ptidsl;
 int  i, rc=0;
 unsigned char missingTones[64];
 int cmd;
 int tag;
 unsigned int  cpuFrequency=0;

 dprintf(4,"dslhal_diags_anlg_testB() NEW 1\n");

 cpuFrequency= shim_osGetCpuFrequency();

 ptidsl=(ITIDSLHW_T *) shim_osAllocateMemory(sizeof(ITIDSLHW_T));
 if(ptidsl==NULL)
   {
   dprintf(1, "unable to allocate memory for ptidsl\n");
   return;
   }
  shim_osZeroMemory((char *) ptidsl, sizeof(ITIDSLHW_T));

   /* Download the Diagnostics Firmware to the DSP */

 for(i=0;i<64;i++)
   {
   if(Tones)
     {
      if(!(i%Tones)&&(i!=0))
        missingTones[i] = 0;
      else
        missingTones[i] = 1;
     }
   else
        missingTones[i] = 1;
   }

  /* Unreset the ADSL Subsystem  */
  rc=dslhal_support_unresetDslSubsystem();
  if(rc)
    {
    dprintf(1, "unable to reset ADSL Subsystem \n");
    shim_osFreeMemory((void *) ptidsl, sizeof(ITIDSLHW_T));
    /* return DSLHAL_ERROR_UNRESET_ADSLSS; */
    }


   rc = dslhal_diags_codeDownload(ptidsl, missingTones);
   if(rc!=0)
     {
       dprintf(0,"Error Downloading Code to DSP \n");
       return;
     }
   rc=dslhal_support_unresetDsp();
   if (rc)
     {
       dprintf(0,"unable to bring DSP out of Reset\n");
       for(i=0; i<ptidsl->numOlayPages; i++)
        {
          if(ptidsl->olayDpPage[i].PmemStartWtAddr !=NULL)
           {
             shim_osFreeDmaMemory((void *) ptidsl->olayDpPage[i].PmemStartWtAddr,
                     ptidsl->olayDpPage[i].OverlayXferCount);
            }
         }
      }

 /*********** Start the actual test **********************/

 if(Test==REVERB)
   {
   dprintf(0,"TX REVERB Test\n");
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_TXREVERB, 0, 0, 0);
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
       if (cmd == DSP_TXREVERB)
         {
         dprintf(0,"DSP_TXREVERB\n");
         return;
         }

       }
     }
   }
 if(Test==MEDLEY)
   {
   dprintf(0,"TX MEDLEY Test\n");
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_TXMEDLEY, 0, 0, 0);
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
       if (cmd == DSP_TXMEDLEY)
         {
         dprintf(0,"DSP_TXMEDLEY\n");
         return;
         }
       }
     }
   }
 dprintf(4,"dslhal_diags_anlg_testB() done\n");
 return;
}



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
void dslhal_diags_anlg_rxNoiseTest(int agcFlag, short pga1, short pga2, short pga3, short aeq)
{
 ITIDSLHW_T *ptidsl;
 int  rc=0;
 unsigned char missingTones[64];
 int cmd;
 int tag;
 unsigned int  cpuFrequency=0;
 dprintf(4,"dslhal_diags_anlg_rxNoiseTest() \n");

 cpuFrequency= shim_osGetCpuFrequency();

 ptidsl=(ITIDSLHW_T *) shim_osAllocateMemory(sizeof(ITIDSLHW_T));
 if(ptidsl==NULL)
   {
   dprintf(1, "unable to allocate memory for ptidsl\n");
   return;
   }
  shim_osZeroMemory((char *) ptidsl, sizeof(ITIDSLHW_T));

   /* Download the Diagnostics Firmware to the DSP */

   rc = dslhal_diags_codeDownload(ptidsl, missingTones);
   if(rc!=0)
     {
       dprintf(0,"Error Downloading Code to DSP \n");
       return;
     }

   /* Set the Parameters in the Shared Interface Structure */

   rc = dslhal_diags_anlg_setPgaParams(ptidsl,agcFlag,pga1,pga2,pga3,aeq);
   if(rc!=0)
     {
       dprintf(0,"Error Setting PGA-AEQ parameters \n");
       return;
     }
 /*********** Start the actual test **********************/

   dprintf(0,"Rx Noise Test\n");
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_RXNOISEPOWER, 0, 0, 0);
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
         dprintf(0,"DSP_QUIT_ADIAG\n");
         dslhal_diags_anlg_getRxNoisePower(ptidsl);
         return;
         }

       }
     }

 dprintf(4,"dslhal_diags_anlg_rxNoiseTest() done\n");
 return;
}

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

void dslhal_diags_anlg_ecNoiseTest(int agcFlag, short pga1, short pga2, short pga3, short aeq)
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
   rc = dslhal_diags_anlg_setPgaParams(ptidsl,agcFlag,pga1,pga2,pga3,aeq);
   if(rc!=0)
     {
       dprintf(0,"Error Setting PGA-AEQ parameters \n");
       return;
      }
 /*********** Start the actual test **********************/

   dprintf(0,"Rx Noise Test\n");
   rc = dslhal_support_writeHostMailbox(ptidsl, HOST_ECPOWER, 0, 0, 0);
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
         dprintf(0,"DSP_ECPOWER\n");
         dslhal_diags_anlg_getRxNoisePower(ptidsl);
         return;
         }
       }
     }

 dprintf(4,"dslhal_diags_anlg_ecNoiseTest() done\n");
 return;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_diags_setPgaParams(tidsl_t *ptidsl,int agcFlag,short pga1, short pga2, short pga3, short aeq)
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
unsigned int dslhal_diags_anlg_setPgaParams(tidsl_t *ptidsl,int agcFlag, short pga1, short pga2, short pga3, short aeq)
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

   dspOamSharedInterface.analogInputVar_p = (DEV_HOST_diagAnlgInputVar_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.analogInputVar_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.analogInputVar_p,
                                      &analogInput, sizeof(DEV_HOST_diagAnlgInputVar_t));
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
   if(agcFlag)
     rc++;
   if(rc)
     {
       dprintf(1,"Using AGC settings!\n");
       rc = 0;
       analogInput.diagOption |= dslhal_support_byteSwap32((unsigned int)((DEV_HOST_diagAnlgOptionsVar_t)DIAG_AGC));
      }
   else
     {
       analogInput.rxPga1 = dslhal_support_byteSwap16((unsigned short)pga1);
       analogInput.rxPga2 = dslhal_support_byteSwap16((unsigned short)pga2);
       analogInput.rxPga3 = dslhal_support_byteSwap16((unsigned short)pga3);
       analogInput.anlgEq = dslhal_support_byteSwap16((unsigned short)aeq);
     }
   dslhal_support_blockWrite(&analogInput,(PVOID)dspOamSharedInterface.analogInputVar_p,sizeof(DEV_HOST_diagAnlgInputVar_t));

   dprintf(5," dslhal_diags_anlg_setPgaParams() Done\n");
   return DSLHAL_ERROR_NO_ERRORS;
}


/******************************************************************************************
* FUNCTION NAME:    dslhal_diags_getRxNoisePower(tidsl_t *ptidsl)
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
unsigned int dslhal_diags_anlg_getRxNoisePower(tidsl_t *ptidsl)
{
   DEV_HOST_diagAnlgOutputVar_t analogOutput;
   DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int rc, max_ds_tones;
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
   max_ds_tones = dslhal_support_getMaxDsTones(ptidsl);
   dspOamSharedInterface.analogOutputVar_p = (DEV_HOST_diagAnlgOutputVar_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.analogOutputVar_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.analogOutputVar_p,
                                      &analogOutput, sizeof(DEV_HOST_diagAnlgOutputVar_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
   analogOutput.rxNoisePower_p[0] = (int *)dslhal_support_byteSwap32((unsigned int)analogOutput.rxNoisePower_p[0]);
   analogOutput.rxNoisePower_p[1] = (int *)dslhal_support_byteSwap32((unsigned int)analogOutput.rxNoisePower_p[1]);
   rc = dslhal_support_blockRead((PVOID)analogOutput.rxNoisePower_p[0],&ptidsl->AppData.rxNoisePower0, max_ds_tones*sizeof(int));
   rc = dslhal_support_blockRead((PVOID)analogOutput.rxNoisePower_p[1],&ptidsl->AppData.rxNoisePower1, max_ds_tones*sizeof(int));
   dprintf(5,"Rx Noise Power 0 \n");
   dprintf(-5,"2131 1 80000000 1 100\n");
   for(rc=0;rc<max_ds_tones;)
     {
      dprintf(-5,"0x%08x\n",dslhal_support_byteSwap32(ptidsl->AppData.rxNoisePower0[rc++]));
      if((rc%8==0)&&(rc!=0))
         dprintf(5,"\n");
     }
   dprintf(5,"Rx Noise Power 1 \n");
   dprintf(-5,"2131 1 80000000 1 100\n");
   for(rc=0;rc<max_ds_tones;)
     {
      dprintf(-5,"0x%08x\n",dslhal_support_byteSwap32(ptidsl->AppData.rxNoisePower1[rc++]));
      if((rc%8==0)&&(rc!=0))
         dprintf(5,"\n");
     }

   dprintf(5," dslhal_diags_anlg_getRxNoisePower() Done\n");
   return DSLHAL_ERROR_NO_ERRORS;
}

/******************************************************************************************
* FUNCTION NAME:    dslhal_diags_anlg_setMissingTones(tidsl_t *ptidsl,unsigned char* missingTones)
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
unsigned int dslhal_diags_anlg_setMissingTones(tidsl_t *ptidsl,unsigned char* missingTones)
{
  DEV_HOST_diagAnlgInputVar_t analogInput;
  DEV_HOST_dspOamSharedInterface_t *pdspOamSharedInterface, dspOamSharedInterface;
   int i, rc;
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

   dspOamSharedInterface.analogInputVar_p = (DEV_HOST_diagAnlgInputVar_t *)dslhal_support_byteSwap32((unsigned int)dspOamSharedInterface.analogInputVar_p);
   rc = dslhal_support_blockRead((PVOID)dspOamSharedInterface.analogInputVar_p,
                                      &analogInput, sizeof(DEV_HOST_diagAnlgInputVar_t));
   if (rc)
     {
       dprintf(1,"dslhal_support_blockRead failed\n");
       return DSLHAL_ERROR_BLOCK_READ;
     }
  if (missingTones)
    {
      for(i=0;i<64;i++)
        analogInput.txTone[i] = missingTones[i];
    }
     dslhal_support_blockWrite(&analogInput,(PVOID)dspOamSharedInterface.analogInputVar_p,sizeof(DEV_HOST_diagAnlgInputVar_t));
  dprintf(5," dslhal_api_setMonitorFlags() Done\n");
  return DSLHAL_ERROR_NO_ERRORS;
}

#endif
