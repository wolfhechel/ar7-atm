/******************************************************************************
 *
 * FILE PURPOSE:    OS files for CPSAR
 ******************************************************************************
 * FILE NAME:       tn7sar.c
 *
 * DESCRIPTION:     This file contains source for required os files for CPSAR
 *
 * (C) Copyright 2002, Texas Instruments Inc
 *
 *
 * Revision History:
 * 0/11/02  Zhicheng Tang, created.
 *
 * 4/07/05   AV  Fixed the tn7sar_handle_interrupt() to do an EOI
 *               only if the interrupt was from a valid source.
 *
 * 04/22/05  AV   Changed the default value of the DaMask to 1 for all
 *                QoS modes, to fix the cell spacing violation.(SR 1-4426141).
 *
 * 08/19/05  JZ   Fix for auto PVC, add internal state for oam ping, and protect
 *                SAR from multiple oam pings (return error code EBUSY), set
 *                oamToHost flags.(SR 1-4767201), CQ # 9861.
 * 09/01/05  AV   Adding Instrumentation code for some of the key functions.(CQ 9907).
 * 09/06/05  AV   Adding support for VBR-rt and VBR-nrt modes.(CQ9808).
 * 09/21/05  AV   Renamed the overloaded memcmp and memcpy functions to
 *                tn7atm_memcmp and tn7atm_memcpy respectively to allow support
 *                for being compiled directly into the kernel.
 * 09/27/05  AV   Reverting CQ9861 as it causes a high traffic problem when no data
 *                channels are open. This was meant to get only OAM packets.
 * 10/11/05  JZ   Fix for auto PVC oam ping lost case, added a new environment
 *                variable autopvc_enable, when set to 1, to enable unmatched oam
 *                cells sent to the host (to enable auto PVC). - Waiting for PDSP
 *                firmware fix to send only OAM cells to the host, no data cells.
 * 11/3/05  CPH   (MR2784) Fix "channel0 closed" infinite loop when soft reboot in tn7sar_receive()
 *  01/25/06 JZ   CQ: 10273 Aztech/Singtel oam pvc management issue, new PDSP 0.54
 * UR8_MERGE_START CQ10402   Jack Zhang
 * 04/21/06  JZ   CQ: 10402 Incorrect initialization of the TxNumBuffers in tn7sar_activate_vc
 * UR8_MERGE_END CQ10402   Jack Zhang
 * UR8_MERGE_START CQ10700 Manjula K
 * 07/24/06 MK    CQ10700: Added counter for reporting packets dropped by ATM Driver/SAR
 * UR8_MERGE_END CQ10700
 *******************************************************************************/

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/atmdev.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/smp_lock.h>
#include <asm/io.h>
#include <asm/mips-boards/prom.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/ctype.h>


#define _CPHAL_AAL5
#define _CPHAL_SAR
#define _CPHAL_HAL
typedef void OS_PRIVATE;
typedef void OS_DEVICE;
typedef void OS_SENDINFO;
typedef void OS_RECEIVEINFO;
typedef void OS_SETUP;

#include "cpswhal_cpsar.h"
#include "tn7atm.h"
#include "tn7api.h"


/* PDSP Firmware files */
#include "tnetd7300_sar_firm.h"


enum
{
  PACKET_TYPE_AAL5,
  PACKET_TYPE_NULL,
  PACKET_TYPE_OAM,
  PACKET_TYPE_TRANS,
  PACKET_TYPE_AAL2
}PACKET_TYPE;

enum
{
  OAM_PING_FAILED,
  OAM_PING_SUCCESS,
  OAM_PING_PENDING,
  OAM_PING_NOT_STARTED,
  OAM_PING_PENDING_RECVD  //Add internal state for the unmatched oam ping result
}OAM_PING;

/* PDSP OAM General Purpose Registers (@todo: These need to be used in the HAL!) */

#define SAR_PDSP_HOST_OAM_CONFIG_REG_ADDR 0xa3000020
#define SAR_PDSP_OAM_CORR_REG_ADDR        0xa3000024
#define SAR_PDSP_OAM_LB_RESULT_REG_ADDR   0xa3000028
#define SAR_PDSP_OAM_F5LB_COUNT_REG_ADDR  0xa300002c
#define SAR_PDSP_OAM_F4LB_COUNT_REG_ADDR  0xa3000030

#define RESERVED_OAM_CHANNEL              15

#define AAL5_PARM "id=aal5, base = 0x03000000, offset = 0, int_line=15, ch0=[RxBufSize=1522; RxNumBuffers = 32; RxServiceMax = 50; TxServiceMax=50; TxNumBuffers=32; CpcsUU=0x5aa5; TxVc_CellRate=0x3000; TxVc_AtmHeader=0x00000640]"
#define SAR_PARM "id=sar,base = 0x03000000, reset_bit = 9, offset = 0; UniNni = 0, PdspEnable = 1"
#define RESET_PARM "id=ResetControl, base=0xA8611600"
#define CH0_PARM "RxBufSize=1522, RxNumBuffers = 32, RxServiceMax = 50, TxServiceMax=50, TxNumBuffers=32, CpcsUU=0x5aa5, TxVc_CellRate=0x3000, TxVc_AtmHeader=0x00000640"

#define MAX_PVC_TABLE_ENTRY 16

sar_stat_t sarStat;

unsigned int SAR_FREQUNCY=50000000;

typedef struct _channel_parm
{
  unsigned int     RxBufSize;
  unsigned int     RxNumBuffers;
  unsigned int     RxServiceMax;
  unsigned int     TxServiceMax;
  unsigned int     TxNumBuffers;
  unsigned int     CpcsUU;
  unsigned int     TxVc_CellRate;
  unsigned int     TxVc_AtmHeader;
}channel_parm_t;

typedef struct _aal5_parm
{
  unsigned int        base;
  unsigned int        offset;
  unsigned int        int_line;
  channel_parm_t      chan[8];
}aal5_parm_t;


typedef struct _sar_parm
{
  unsigned int        base;
  unsigned int        reset_bit;
  unsigned int        offset;
  unsigned int        UniNni;
}sar_parm_t;

typedef struct _pvc_table
{
  int bInUse;
  int vpi;
  int vci;
}pvc_table;

static aal5_parm_t aal5Parm;
static sar_parm_t  sarParm;
static char *pAal5, *pSar, *pReset;
static int oam_type;
static unsigned int oamPingStatus = OAM_PING_NOT_STARTED;
                                   //static variable initialization, jz
static int oamAtmHdr;
static int oamLbTimeout;
static char parm_data[1024];
static char aal5Data[1024];
static char sarData[1024];
static char resetData[256];
static pvc_table pvc_result[MAX_PVC_TABLE_ENTRY];

/* external function */
extern int __guDbgLevel;

/* gloabal function */
unsigned int oamFarLBCount[4];
/* end of gloabal function */

/* internal APIs */
static int tn7sar_atm_header(int vpi, int vci);
static void tn7sar_record_pvc(int atmheader);

/*end of internal APIs */
spinlock_t sar_lock;

/* HAL OS support functions */
unsigned long tn7sar_strtoul(const char *str, char **endptr, int base)
{
  unsigned long ret;

  ret= simple_strtoul(str, endptr, base);
  return ret;
}

/*** Viren: Change the function calls to Macros ***/
/* Macro Definition of function calls */
#define tn7sar_free_buffer \
  tn7atm_free_rx_skb

#define tn7sar_memset \
  memset

#define tn7sar_free \
    kfree

#define tn7sar_free_dev \
  kfree

#define tn7sar_free_dma_xfer \
  kfree

#define tn7sar_send_complete \
  tn7atm_send_complete

/***Viren: Macro definition ends here ***/

static void *tn7sar_malloc(unsigned int size)
{
  return(kmalloc(size, GFP_KERNEL));
}

static unsigned long lockflags;
static void tn7sar_critical_on(void)
{
  spin_lock_irqsave(&sar_lock,lockflags);
}

static void tn7sar_critical_off(void)
{
  spin_unlock_irqrestore(&sar_lock,lockflags);
}

static int tn7sar_find_device(int unit, const char *find_name, void *device_info)
{
  int ret_val = 0;
  char **ptr;

  ptr = (char **)device_info;
  dgprintf(3,"tn7sar_find_device\n");
  if(strcmp(find_name, "aal5")==0)
  {
    *ptr = pAal5;
  }
  else if(strcmp(find_name, "sar")==0)
  {
    dgprintf(3, "pSar=%s\n", pSar);
    *ptr = pSar;
  }
  else if(strcmp(find_name, "reset")==0)
  {
    dgprintf(3, "pReset=%s\n", pReset);
    *ptr = pReset;
  }

  device_info = NULL;

  return(ret_val);
}

static int tn7sar_get_device_parm_uint(void *dev_info, const char *param, unsigned int *value)
{
  char *dev_str;
  char *pMatch;
  int i=0, j=0;
  char val_str[64];
  unsigned int val;
  int base = 10;

  dgprintf(6, "tn7sar_get_device_parm_uint()\n");

  dev_str = (char *)dev_info;
  dgprintf(3, "parm=%s\n", param);
  pMatch = strstr(dev_str, param);

  if(pMatch)
  {
    //get "=" position
    while(pMatch[i] != '=')
    {
      i++;
    }
    i++;
    // get rid of spaces
    while(pMatch[i]== ' ')
    {
      i++;
    }
    //get rid of 0x
    if(pMatch[i]== '0')
    {
       if(toupper(pMatch[i+1]) == 'X')
      //if(pMatch[i+1] == 'x' || pMatch[i+1] == 'X')
      {
        i+=2;
        base = 16;
      }
    }

    // get next delineator
    while(pMatch[i] != ',' && pMatch[i] != 0x0)
    {
      val_str[j]=pMatch[i];
      j++;
      i++;
    }
    val_str[j]=0;

    val = simple_strtoul(val_str, (char **)NULL, base);
    dgprintf(4, "val =%d\n", val);
    *value = val;
    return 0;
  }


  dgprintf(3, "match not found.\n");
  if(strcmp(dev_str, "debug")==0)
  {
    dgprintf(6,"debug..\n");
    *value = 6;
    return 0;
  }
  return (1);
}

static int tn7sar_get_device_parm_value(void *dev_info, const char *param, void *value)
{
  char *dev_str;
  char *pMatch;
  int i=0, j=0;
  char *pVal;


  dgprintf(3, "tn7sar_get_device_parm_value().\n");

  pVal = (char *) parm_data;
  dev_str = (char *)dev_info;
  dgprintf(3, "dev_info: \n%s\n", dev_str);
  dgprintf(3, "param=%s\n", param);
  if(strcmp(param, "Ch0")==0)
  {
    *(char **)value = CH0_PARM;
    dgprintf(3, "value =%s\n", *(char **)value);
    return 0;
  }

  pMatch = strstr(dev_str, param);
  if(pMatch)
  {
    //get "=" position
    while(pMatch[i] != '=')
    {
      i++;
    }
    i++;
    // get rid of spaces
    while(pMatch[i]== ' ')
    {
      i++;
    }

    if(pMatch[i] != '[') //"["
    {
      // get next delineator
      while(pMatch[i] != ',' && pMatch[i] != 0x0)
      {
        pVal[j] = pMatch[i];
        j++;
        i++;
      }
      pVal[j]=0;

      *(char **)value = pVal;
      return 0;
    }
    else
    {
      i++; //skip "["
      while(pMatch[i] != ']')
      {
        if(pMatch[i] == ';') //";"
          pVal[j] = ',';
        else
          pVal[j] = pMatch[i];
        j++;
        i++;
      }
      pVal[j] = 0;
      *(char **)value = pVal;
      return 0;
    }

  }

  return (1);
}

static int tn7sar_control(void *dev_info, const char *key, const char *action, void *value)
{
        int ret_val = -1;

  if (strcmp(key, "Firmware") == 0)
  {
    if (strcmp(action, "Get") == 0)
    {
      *(int **)value = &SarPdspFirmware[0];
    }
    ret_val=0;
  }

  if (strcmp(key, "FirmwareSize") == 0)
  {
    if (strcmp(action, "Get") == 0)
    {
      *(int *)value = sizeof(SarPdspFirmware);
    }
    ret_val=0;
  }

  if (strcmp(key, "OamLbResult") == 0)
  {
    dgprintf(2, "get looback source call back\n");
    if (strcmp(action, "Set") == 0)
    {
      dgprintf(2, "oam result = %d\n", *(unsigned int *)value);
      oamFarLBCount[oam_type] = oamFarLBCount[oam_type] + *(unsigned int *)value;
#ifdef __AUTOPVC_DBG
      printk("tn7sar_control: pingStat %d, oamHdr %X result %d\n", oamPingStatus, oamAtmHdr, *(unsigned int *)value);
#endif// __AUTOPVC_DBG
      if( oamPingStatus == OAM_PING_PENDING_RECVD) //jz: CQ#9861, this state is only set in the Receive func.
      {                                            //jz: CQ#9861, it indiactes the unmatched oam ping success
        oamPingStatus = OAM_PING_SUCCESS;
      }
      else if(oamPingStatus == OAM_PING_PENDING)
      {
        oamPingStatus = *(unsigned int *)value;
        if(oamPingStatus == OAM_PING_SUCCESS)
        {
          /* record pvc */
          tn7sar_record_pvc(oamAtmHdr);
        }
      }

    }
    ret_val=0;
  }

  if (strcmp(key, "SarFreq") == 0)
  {
    if (strcmp(action, "Get") == 0)
    {
      *(int *)value = SAR_FREQUNCY;
    }
    ret_val=0;
  }
  return(ret_val);
}


static inline void tn7sar_sarhal_isr_register(OS_DEVICE *os_dev, int(*hal_isr)(HAL_DEVICE *, int *), int interrupt_num)
{
  tn7atm_sarhal_isr_register(os_dev, hal_isr, interrupt_num);
}

static void tn7sar_isr_unregister(OS_DEVICE *os_dev, int interrupt_num)
{
    /* TODO */
}


/*** Viren: We should be able to get rid of this wrapper */
static inline void *tn7sar_malloc_rxbuffer(unsigned int size, void *mem_base, unsigned int mem_range, void *osSetup,
                       HAL_RECEIVEINFO *hal_info, OS_RECEIVEINFO **os_receive_info, OS_DEVICE *os_dev)
{

   return tn7atm_allocate_rx_skb(os_dev, os_receive_info, size, osSetup);
}

static inline void *tn7sar_malloc_dev(unsigned int size)
{
  return(kmalloc(size, GFP_KERNEL));
}

static inline void *tn7sar_malloc_dma_xfer(unsigned int size, void *mem_base, unsigned int mem_range)
{
  dgprintf(4, "tn7sar_malloc_dma_xfer, size =%d\n", size);

  return (kmalloc(size, GFP_DMA |GFP_KERNEL));

}

static void tn7sar_record_pvc(int atmheader)
{
  int vci,vpi;
  int i;

  vci = 0xffff & (atmheader >> 4);
  vpi = 0xff & (atmheader >> 20);
  for(i = 0; i < MAX_PVC_TABLE_ENTRY; i++)
  {
    if(pvc_result[i].bInUse)
    {
      if(pvc_result[i].vpi == vpi && pvc_result[i].vci == vci)
      {
        return;
      }
    }
  }
  for(i = 0; i < MAX_PVC_TABLE_ENTRY; i++)
  {
    if(pvc_result[i].bInUse == 0)
    {
      pvc_result[i].bInUse = 1;
      pvc_result[i].vpi = vpi;
      pvc_result[i].vci = vci;
      return;
    }
  }
  return;
}

static void tn7sar_clear_pvc_table(void)
{
  int i;

  for(i = 0; i < MAX_PVC_TABLE_ENTRY; i++)
  {
    pvc_result[i].bInUse = 0;
    pvc_result[i].vpi = 0;
    pvc_result[i].vci = 0;
  }
}

//#define PRTK_UNWANTED_CELLS_2_HOST  1

int tn7sar_process_unmatched_oam(FRAGLIST *frag_list, unsigned int frag_count, unsigned int packet_size, unsigned int mode)
{

  FRAGLIST *local_list;
  int i;
  unsigned int atmHdr;

  local_list = frag_list;

  for(i=0;i<(int)frag_count;i++)
  {
    tn7atm_data_invalidate(local_list->data, (int)local_list->len);
    local_list ++;
  }
  local_list = frag_list;
#ifdef PRTK_UNWANTED_CELLS_2_HOST
  printk("tn7sar_process_unmatched_oam - (ALL)cells: pingStat %d, atmHdr %X, oamHdr %X \n", oamPingStatus, *(unsigned int *)frag_list->data, oamAtmHdr);
#endif  //PRTK_UNWANTED_CELLS_2_HOST
  if((mode>>31)) /*vci, vpi is attached */
  {
    atmHdr = *(unsigned int *)frag_list->data;
    //printk("tn7sar_process_unmatched_oam: pingStat %d, atmHdr %X, oamHdr %X \n", oamPingStatus, atmHdr, oamAtmHdr);
    //tn7sar_record_pvc(atmHdr); jz: moved down for only receiving the expected oam pings, CQ#9861
    if(atmHdr & 0x8) //oam cell
    {
#ifdef PRTK_UNWANTED_CELLS_2_HOST
      printk("tn7sar_process_unmatched_oam - (OAM)cells: pingStat %d, atmHdr %X, oamHdr %X \n", oamPingStatus, *(unsigned int *)frag_list->data, oamAtmHdr);
#endif  //PRTK_UNWANTED_CELLS_2_HOST
      atmHdr &= 0xfffffff0;
      if(atmHdr == oamAtmHdr)
      {
#ifdef PRTK_UNWANTED_CELLS_2_HOST
        printk("tn7sar_process_unmatched_oam - (Response OAM)cells: pingStat %d, atmHdr %X, oamHdr %X \n", oamPingStatus, atmHdr, oamAtmHdr);
#endif  //PRTK_UNWANTED_CELLS_2_HOST
        tn7sar_record_pvc(atmHdr); //jz: moved here for CQ#9861
        if(oamPingStatus == OAM_PING_PENDING)
        {
          // oamPingStatus = OAM_PING_SUCCESS;
          oamPingStatus = OAM_PING_PENDING_RECVD; //jz: CQ#9861: received the unconfigured VCC oam pings
          oamFarLBCount[oam_type] = oamFarLBCount[oam_type] + 1;
        }
        return 0;
      }
    }
#ifdef PRTK_UNWANTED_CELLS_2_HOST
    else{
      printk("---------DBG-ERR: tn7sar_process_unmatched_oam - NOT AN UNMATCHED OAM Cell!!!------------\n");
    }
#endif  //PRTK_UNWANTED_CELLS_2_HOST
  }

  return 0;
}


static int tn7sar_receive(OS_DEVICE *os_dev,FRAGLIST *frag_list, unsigned int frag_count, unsigned int packet_size,
                 HAL_RECEIVEINFO *hal_receive_info, unsigned int mode)
{
  int ch;
  struct atm_dev *dev;
  Tn7AtmPrivate *priv;
  HAL_FUNCTIONS *pHalFunc;
  int bRet;
  static int cnt = 0;

  dgprintf(4, "tn7sar_receive\n");

  #ifdef TIATM_INST_SUPP
  psp_trace(ATM_DRV_RX_ENTER);
  #endif

  dev = (struct atm_dev *)os_dev;
  priv= (Tn7AtmPrivate *)dev->dev_data;
  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;

  /* Mode contains channel info */
  ch = (mode & 0xFF);

  if(ch == RESERVED_OAM_CHANNEL) //15
  {
    tn7sar_process_unmatched_oam(frag_list, frag_count, packet_size, mode);
    bRet = pHalFunc->RxReturn(hal_receive_info, 0);
    #ifdef TIATM_INST_SUPP
    psp_trace_par(ATM_DRV_RX_EXIT, bRet);
    #endif

    return 0;
  }

  /*** Should we remove this check ***/
  if(frag_count > 1 || frag_list->len == 0)
  {
    printk("Packet fragment count > 1, not handle.\n");
    bRet = pHalFunc->RxReturn(hal_receive_info, 0);
    priv->stats.rx_errors++; //UR8_MERGE_START_END CQ10700 Manjula K
    #ifdef TIATM_INST_SUPP
    psp_trace_par(ATM_DRV_RX_EXIT, bRet);
    #endif

    return 1;
  }

  /* Added check to see if the channel is closed */
  if(!priv->lut[ch].ready)
  {
    // printk("Channel %d is closed.\n", ch);
    if( cnt == 0) printk("Channel %d is closed.\n", ch);
    cnt++;
    if( (cnt %100) == 0) printk("Channel %d is closed. cnt=%d\n", ch, cnt);
    pHalFunc->RxReturn(hal_receive_info, 0);
    return 1;
  }

  /** Can this invalidation be removed from here **/
  tn7atm_data_invalidate(frag_list->data, (int)frag_list->len);
  bRet=tn7atm_receive(os_dev, ch, packet_size, frag_list->OsInfo, frag_list->data);

  if(bRet==0)
  {
    sarStat.rxPktCnt++;
    sarStat.rxBytes += packet_size;
    pHalFunc->RxReturn(hal_receive_info, 1);
  }
  else
  {
    pHalFunc->RxReturn(hal_receive_info, 0);
  }

  #ifdef TIATM_INST_SUPP
  psp_trace_par(ATM_DRV_RX_EXIT, bRet);
  #endif

  return bRet;
}

void tn7sar_teardown_complete(OS_DEVICE *OsDev, int ch, int Dir)
{
  //AV: just for debugging.
  printk("%s called for channel %d\n", __FUNCTION__, ch);
  return;
}

int tn7sar_init_module(OS_FUNCTIONS *os_funcs)
{
  dgprintf(4, "tn7sar_init_module\n");
  if( os_funcs == 0 )
  {
    return(-1);
  }
  os_funcs->Control                  = tn7sar_control;
  os_funcs->CriticalOn               = tn7sar_critical_on;
  os_funcs->CriticalOff              = tn7sar_critical_off;
  os_funcs->DataCacheHitInvalidate   = tn7atm_data_invalidate;
  os_funcs->DataCacheHitWriteback    = tn7atm_data_writeback;
  os_funcs->DeviceFindInfo           = tn7sar_find_device;
  os_funcs->DeviceFindParmUint       = tn7sar_get_device_parm_uint;
  os_funcs->DeviceFindParmValue      = tn7sar_get_device_parm_value;
  os_funcs->Free                     = tn7sar_free;
  os_funcs->FreeRxBuffer             = tn7sar_free_buffer;
  os_funcs->FreeDev                  = tn7sar_free_dev;
  os_funcs->FreeDmaXfer              = tn7sar_free_dma_xfer;
  os_funcs->IsrRegister              = tn7sar_sarhal_isr_register;
  os_funcs->IsrUnRegister            = tn7sar_isr_unregister;
  os_funcs->Malloc                   = tn7sar_malloc;
  os_funcs->MallocRxBuffer           = tn7sar_malloc_rxbuffer;
  os_funcs->MallocDev                = tn7sar_malloc_dev;
  os_funcs->MallocDmaXfer            = tn7sar_malloc_dma_xfer;
  os_funcs->Memset                   = tn7sar_memset;
  os_funcs->Printf                   = printk;
  os_funcs->Receive                  = tn7sar_receive;
  os_funcs->SendComplete             = tn7sar_send_complete;
  os_funcs->Strcmpi                  = strcmp;
  os_funcs->Sprintf                  = sprintf;
  os_funcs->Strlen                   = strlen;
  os_funcs->Strstr                   = strstr;
  os_funcs->Strtoul                  = tn7sar_strtoul;
  os_funcs->TeardownComplete         = tn7sar_teardown_complete;

  return(0);
}


static void tn7sar_init_dev_parm(void)
{
  int i;


  /* aal5 */
  //strcpy(aal5Parm.id, "aal5");
  aal5Parm.base = 0x03000000;
  aal5Parm.offset = 0;
  aal5Parm.int_line=15;
  aal5Parm.chan[0].RxBufSize=1600;
  aal5Parm.chan[0].RxNumBuffers = 32;
  aal5Parm.chan[0].RxServiceMax = 50;
  aal5Parm.chan[0].TxServiceMax=50;
  aal5Parm.chan[0].TxNumBuffers=32;
  aal5Parm.chan[0].CpcsUU=0x5aa5;
  aal5Parm.chan[0].TxVc_CellRate=0x3000;
  aal5Parm.chan[0].TxVc_AtmHeader=0x00000640;
  for(i=1;i<8;i++)
  {
    tn7atm_memcpy(&aal5Parm.chan[i], &aal5Parm.chan[0], sizeof(aal5Parm.chan[0]));
  }


  /* sar */
  //strcpy(sarParm.id, "sar");
  sarParm.base = 0x03000000;
  sarParm.reset_bit = 9;
  sarParm.offset = 0;
  sarParm.UniNni = 0;

  pAal5 = aal5Data;
  pSar  = sarData;
  pReset = resetData;
  strcpy(pAal5, AAL5_PARM);
  strcpy(pSar, SAR_PARM);
  strcpy(pReset, RESET_PARM);

}


int tn7sar_get_stats(void *priv1)
{
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  Tn7AtmPrivate *priv;
  int i, j;
  unsigned int *pSarStat, *pStateBase;
  char statString[64];
  int len;

  dgprintf(2, "tn7sar_get_stats\n");

  priv = (Tn7AtmPrivate *)priv1;
  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  //memset(&sarStat, 0, sizeof(sarStat));
  sarStat.txErrors = 0;
  sarStat.rxErrors = 0;
  for(i=0;i<MAX_DMA_CHAN;i++)
  {
    if(priv->lut[i].inuse)
    {
      for(j=0;j<1;j++)
      {
        len=sprintf(statString, "Stats;0;%d", priv->lut[i].chanid);
        statString[len]=0;
        dgprintf(2, "statString=%s\n",statString);
        pHalFunc->Control(pHalDev, statString, "Get", &pSarStat);
        pStateBase = pSarStat;
        while(pSarStat)
        {
          if((char *)*pSarStat == NULL)
            break;
          dgprintf(2, "%s\n", (char *) *pSarStat);
          pSarStat++;
          dgprintf(2, "%s\n", (char *) *pSarStat);
          sarStat.rxErrors += os_atoul((char *) *pSarStat);
          pSarStat++;
        }

        kfree(pStateBase);
      }
    }
  }
  return 0;
}

#define USE_PDSP_054  //CQ10273
int tn7sar_setup_oam_channel(Tn7AtmPrivate *priv)
{

  CHANNEL_INFO chInfo;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  int chan=RESERVED_OAM_CHANNEL; //15;
  char *pauto_pvc = NULL;
  int  auto_pvc = 1;
  unsigned int oam2host_ch;
  char   oam_str[20];

  dgprintf(4, "tn7sar_setup_oam_channel\n");

  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  pauto_pvc = prom_getenv("autopvc_enable");
  if(pauto_pvc)  //CQ10273
  {
    auto_pvc =tn7sar_strtoul(pauto_pvc, NULL, 10);
  }

  memset(&chInfo, 0xff, sizeof(chInfo));

  /* channel specific */
  chInfo.Channel = RESERVED_OAM_CHANNEL; //15; hardcoded for last channel
  chInfo.Direction = 0;
  chInfo.Vci = 30; /* just need below 32 */
  chInfo.Vpi = 0;
  chInfo.TxVc_QosType = 2;

  /*default */
  chInfo.PktType      = PACKET_TYPE_TRANS; //PACKET_TYPE_OAM; To correct Data Cell to Host Issues! 10/07/05
  chInfo.TxServiceMax = 2;
  chInfo.RxServiceMax = 2;
  chInfo.TxNumQueues  = 1;
  chInfo.TxNumBuffers = 4;
  chInfo.RxNumBuffers = 4;
  chInfo.RxBufSize    = 256;

  if( auto_pvc == 0)
  {
    chInfo.RxVc_OamToHost = 0; //jz: CQ#9861, Set the unmatched oam ping to the host flags
    chInfo.RxVp_OamToHost = 0; //jz: CQ#9861, Set the unmatched oam ping to the host flags
    chInfo.FwdUnkVc       = 0;
  }
  else
  {
    chInfo.RxVc_OamToHost = 1; //jz: CQ#9861, Set the unmatched oam ping to the host flags
    chInfo.RxVp_OamToHost = 1; //jz: CQ#9861, Set the unmatched oam ping to the host flags
    chInfo.RxVc_OamCh     = RESERVED_OAM_CHANNEL;//jz: CQ#9861, Set the unmatched oam ping to the host flags
    chInfo.RxVp_OamCh     = RESERVED_OAM_CHANNEL;//jz: CQ#9861, Set the unmatched oam ping to the host flags
    
    if(auto_pvc<=2)
      chInfo.FwdUnkVc       = 1; 
    else  //3 or more
      chInfo.FwdUnkVc       = 0; // was 1; test fix for not forwarding data cells!enable forwarding of unknown vc

#ifdef __AUTOPVC_DBG
      printk("DBG----: autopvc_enable=%d(1: autoPVC ok, 2: unK=1 and 54chg, 3: unK=0 Greg's suggestion) chInfo.FwdUnkVc set to %d.\n", auto_pvc, chInfo.FwdUnkVc);
#endif// __AUTOPVC_DBG
  }
  //chInfo.FwdUnkVc       = 1; //priginal was 1
  chInfo.TxVc_AtmHeader = tn7sar_atm_header((int)chInfo.Vpi, chInfo.Vci);
  chInfo.RxVc_AtmHeader = tn7sar_atm_header((int)chInfo.Vpi, chInfo.Vci);
  chInfo.TxVp_AtmHeader = tn7sar_atm_header((int)chInfo.Vpi, 0);
  chInfo.RxVp_AtmHeader = tn7sar_atm_header((int)chInfo.Vpi, 0);

  dgprintf(4, "TxVc_AtmHeader=0x%x\n", chInfo.TxVc_AtmHeader);


  if(pHalFunc->ChannelSetup(pHalDev, &chInfo, NULL))
  {
    printk("failed to setup channel =%d.\n", chan);
    return -1;
  }

#ifdef USE_PDSP_054  //CQ10273
    //Test new firmware fix  Norayda/Greg 12/15/05 v0.54bis
    // chInfo.FwdUnkVc       = 0; // was 1; test fix for not forwarding data cells!enable forwarding of unknown vc
    if(auto_pvc >= 2)
    {
#ifdef __AUTOPVC_DBG
      printk("DBG----: PDSP54 autopvc_enable=%d(1: autoPVC ok, 2: unK=1 and 54chg, 3: unK=0 Greg's suggestion)\n", auto_pvc);
#endif// __AUTOPVC_DBG

      oam2host_ch = 0x4000000f;  //Use channel 15 (0xf) for unmatched oam cells, bit 31 cleared, bit 30 set, chnal=15 
      sprintf(oam_str, "a3009194%0X", oam2host_ch);
#ifdef __AUTOPVC_DBG
      printk("DBG----: unmatched OAM option %d: cmd = %s.\n", oam2host_ch);
#endif// __AUTOPVC_DBG
      tn7dsl_write_memory( oam_str);  // This one writes 0x4000000f to addr 0xa3009194
    }
#endif // USE_PDSP_054

  // claiming the channel
  priv->lut[chan].vpi = 0;
  priv->lut[chan].vci = 30;
  priv->lut[chan].chanid = chan;
  priv->lut[chan].inuse = 1;
  return 0;
}

int tn7sar_init(struct atm_dev *dev, Tn7AtmPrivate *priv)
{
  int retCode;
  int hal_funcs_size;

  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  OS_FUNCTIONS  *pOsFunc;
  int oamMod;
  char *pLbTimeout;
  int lbTimeout;


  dgprintf(4, "tn7sar_init\n");

  pOsFunc = (OS_FUNCTIONS  *)kmalloc(sizeof(OS_FUNCTIONS), GFP_KERNEL);


  priv->pSarOsFunc = ( void *)pOsFunc;

  /* init boot parms */
  tn7sar_init_dev_parm();

  /* init sar os call back functions */
  retCode = tn7sar_init_module(pOsFunc);
  if (retCode != 0) /* error */
  {
    printk("Failed to init SAR OS Functions\n");
    return (1);
  }

        /* Init sar hal */
  retCode = cpaal5InitModule(&pHalDev, (OS_DEVICE*) dev, &pHalFunc,
                               pOsFunc, sizeof(OS_FUNCTIONS), &hal_funcs_size, 0);
  if (retCode != 0) /* error */
  {
    printk("Failed to init SAR HAL\n");
    return (1);
  }

  /* sanity check */
  if (pHalDev == NULL || pHalFunc == NULL || hal_funcs_size != sizeof(HAL_FUNCTIONS) )
  {
    printk("Invalid SAR hal and/or functions.\n");
    return (1);
  }

  /* remeber HAL pointers */
  priv->pSarHalDev = (void *)pHalDev;
  priv->pSarHalFunc = (void *)pHalFunc;

        /* Probe for the Device to get hardware info from driver */
  retCode = pHalFunc->Probe(pHalDev);
  if (retCode !=0)
  {
    printk("SAR hal probing error.\n");
    return (1);
  }

  /* init sar hal */
  retCode = pHalFunc->Init(pHalDev);
  if (retCode != 0) /* error */
  {

    printk("pHalFunc->Init failed. err code =%d\n", retCode);
    return (1);
  }

  /* open hal module */
  retCode = pHalFunc->Open(pHalDev);
  if (retCode != 0) /* error */
  {
     printk("pHalFunc->open failed, err code: %d\n",retCode );
     return (1);
  }

  /* init sar for firmware oam */
  oamMod= 1;
  pHalFunc->Control(pHalDev,"OamMode", "Set", &oamMod);

  /* read in oam lb timeout value */
  pLbTimeout = prom_getenv("oam_lb_timeout");
  if(pLbTimeout)
  {
    lbTimeout =tn7sar_strtoul(pLbTimeout, NULL, 10);
    oamLbTimeout = lbTimeout;
    pHalFunc->Control(pHalDev,"OamLbTimeout", "Set", &lbTimeout);
  }
  else
  {
    oamLbTimeout = 5000;
  }

  oamFarLBCount[0]=0;
  oamFarLBCount[1]=0;
  oamFarLBCount[2]=0;
  oamFarLBCount[3]=0;

  memset(&sarStat, 0 , sizeof(sarStat));

  /* setup channel 15 for oam operation */
  tn7sar_setup_oam_channel(priv);
  dgprintf(4, "tn7sar_init done");
  return 0;
}

static int tn7sar_atm_header(int vpi, int vci)
{
  union
  {
    unsigned char byte[4];
    unsigned int  dword;
  }atm_h;
  int itmp = 0;

  //vci
  itmp = vci &0xf;
  atm_h.byte[0] = 0;
  atm_h.byte[0] |= (itmp << 4);
  atm_h.byte[1] = ((vci & 0xff0) >> 4);
  atm_h.byte[2] = 0;
  atm_h.byte[2] |= ((vci & 0xf000) >>12);;
  atm_h.byte[2] |= ((vpi & 0xf) << 4);
  atm_h.byte[3] = 0;
  atm_h.byte[3] = ((vpi & 0xff0) >> 4);
  return atm_h.dword;
}

int tn7sar_activate_vc (tn7atm_activate_vc_parm_t * tn7atm_activate_vc_parm_p)
{
  CHANNEL_INFO chInfo;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE *pHalDev;
  int rc;

  dgprintf (4, "tn7sar_activate_vc\n");

  pHalFunc = (HAL_FUNCTIONS *) tn7atm_activate_vc_parm_p->priv->pSarHalFunc;
  pHalDev = (HAL_DEVICE *) tn7atm_activate_vc_parm_p->priv->pSarHalDev;

  memset (&chInfo, 0xff, sizeof (chInfo));

  /*
   * channel specific
   */
  chInfo.Channel = tn7atm_activate_vc_parm_p->chan;
  chInfo.Direction = 0;
  chInfo.Vci = tn7atm_activate_vc_parm_p->vci;
  chInfo.Vpi = tn7atm_activate_vc_parm_p->vpi;
  chInfo.TxVc_QosType = tn7atm_activate_vc_parm_p->qos;
  chInfo.Priority = tn7atm_activate_vc_parm_p->priority;

  /*
   * SR 1-4426141: The mask is now being defaulted for all QoS modes. This
   * fixes the cell spacing violation at low rates.
   */
  chInfo.DaMask = 1;

  chInfo.TxVc_Mbs = tn7atm_activate_vc_parm_p->mbs;     /* use pcr as MBS */
  chInfo.TxVc_CellRate = SAR_FREQUNCY / (tn7atm_activate_vc_parm_p->scr);
  chInfo.TxVc_Pcr = SAR_FREQUNCY / (tn7atm_activate_vc_parm_p->pcr);

  /*
   * default
   */
  chInfo.PktType = PACKET_TYPE_AAL5;
  /* @change to make it configurable 05/15/2004 */
  chInfo.TxServiceMax = tn7atm_activate_vc_parm_p->priv->sarTxMax;

  /* @change to make it configurable 05/15/2004 */
  chInfo.RxServiceMax = tn7atm_activate_vc_parm_p->priv->sarRxMax;

  chInfo.TxNumQueues = TX_QUEUE_NUM;

  if (tn7atm_activate_vc_parm_p->vpi == REMOTE_MGMT_VPI
      && tn7atm_activate_vc_parm_p->vci == REMOTE_MGMT_VCI)
  {
    chInfo.TxNumBuffers = 4;
    chInfo.RxNumBuffers = 4;
    chInfo.RxBufSize = MAX_CLEAR_EOC_BUF_SIZE;
  }
  else
  {
    /* @change to make it configurable 05/15/2004 */
// UR8_MERGE_START CQ10402   Jack Zhang
    chInfo.TxNumBuffers = tn7atm_activate_vc_parm_p->priv->sarTxBuf; //CQ10402
// UR8_MERGE_END CQ10402   Jack Zhang

    /* @change to make it configurable 05/15/2004 */
    chInfo.RxNumBuffers = tn7atm_activate_vc_parm_p->priv->sarRxBuf;

    chInfo.RxBufSize = RX_BUFFER_SIZE;
  }

  chInfo.RxVc_OamToHost = 0;
  chInfo.RxVp_OamToHost = 0;

  /* Cleaned up two unnecessary function calls. */
  chInfo.TxVc_AtmHeader = tn7sar_atm_header (tn7atm_activate_vc_parm_p->vpi, tn7atm_activate_vc_parm_p->vci);
  chInfo.RxVc_AtmHeader = chInfo.TxVc_AtmHeader;
  chInfo.TxVp_AtmHeader = tn7sar_atm_header (tn7atm_activate_vc_parm_p->vpi, 0);
  chInfo.RxVp_AtmHeader = chInfo.TxVp_AtmHeader;
  chInfo.CpcsUU = 0;

  dgprintf (4, "TxVc_AtmHeader=0x%x\n", chInfo.TxVc_AtmHeader);

  rc = pHalFunc->ChannelSetup(pHalDev, &chInfo, tn7atm_activate_vc_parm_p->priv->lut[tn7atm_activate_vc_parm_p->chan].vcc);
  if (rc)
  {
    printk ("failed to setup channel =%d with return code %d \n",
            tn7atm_activate_vc_parm_p->chan, rc);
    return -1;
  }

  return 0;
}

int tn7sar_send_packet(Tn7AtmPrivate *priv, int chan, void *new_skb, void *data,unsigned int len, int priority)
{
  FRAGLIST fragList;
  unsigned int mode;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;

  dgprintf(4, "tn7sar_send_packet\n");
  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  fragList.len = len;
  fragList.data = (void *)data;

  xdump((char *)fragList.data , fragList.len, 6);

  /*mode bit
  31-19 unused
  18    oam cell, 1 = true, 0=false
  17-16 oam type, 0=F4 seg, 1=F4 End, 2=F5 seg, 3=F5 end
  15-08 transimit queue, current, 0=priority queue, 1=normal queue
  07-00 channel number
  */
  mode = 0;
  mode |= (0xff & chan);
  mode |= ((0xff & priority) << 8);

  dgprintf(4, "mode = %d\n", mode);

  tn7atm_data_writeback(fragList.data, len);
  if(pHalFunc->Send(pHalDev, &fragList, 1, len, new_skb, mode) != 0)
  {
    dgprintf(1, "SAR hal failed to send packet.\n");
    return 1;
  }
  //tn7sar_get_stats(priv);
  sarStat.txPktCnt++;
  sarStat.txBytes +=len;
  return 0;
}



int tn7sar_handle_interrupt(struct atm_dev *dev, Tn7AtmPrivate *priv, int *more)
{
  int rc=0;
  int (*halIsr)(HAL_DEVICE *halDev, int *work);

  halIsr = priv->halIsr;

  rc = halIsr((HAL_DEVICE *)priv->pSarHalDev, more);

  /* Do EOI if the interrupt was from a valid source. */
  if(rc != EC_VAL_INTERRUPT_NOT_FOUND)
  {
#ifdef CPATM_TASKLET_MODE
    if(*more == 0)
    {
        ((HAL_FUNCTIONS *)priv->pSarHalFunc)->PacketProcessEnd((HAL_DEVICE *)priv->pSarHalDev);
    }
#else
        ((HAL_FUNCTIONS *)priv->pSarHalFunc)->PacketProcessEnd((HAL_DEVICE *)priv->pSarHalDev);
#endif
  }

  return rc;
}

static spinlock_t chan_close_lock;
//AV:Temporary hack
#ifndef pRX_CPPI_COMP_PTR
#define pRX_CPPI_COMP_PTR(base)             ((volatile bit32u *)(base+0x8048))
#define RX_CPPI_COMP_PTR(base)              (*pRX_CPPI_COMP_PTR(base))
#define TEARDOWN_VAL                        0xfffffffc
#define pTXH_CPPI_COMP_PTR(base)            ((volatile bit32u *)(base+0x8040))
#define TXH_CPPI_COMP_PTR(base)             (*pTXH_CPPI_COMP_PTR(base))
#define pTXL_CPPI_COMP_PTR(base)            ((volatile bit32u *)(base+0x8044))
#define TXL_CPPI_COMP_PTR(base)             (*pTXL_CPPI_COMP_PTR(base))
#endif

int tn7sar_deactivate_vc(Tn7AtmPrivate *priv, int chan)
{
  unsigned int mode;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  int rc;
  int flags;

  //dgprintf(4, "tn7sar_deactivate_vc\n");
  //printk("tn7sar_deactivate_vc entered\n");
  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  //AV: testing for disabling interrupts.
  //mode = 0xf; //tear down everything, wait for return;
  mode = (0xf & ~0x8); //tear down everything, don't wait for return;

  spin_lock_irqsave(&chan_close_lock, flags);

  rc = pHalFunc->ChannelTeardown(pHalDev, chan, mode);

#if 0
  //AV:Wait for the interrupt to happen.
  while (((*(pRX_CPPI_COMP_PTR( pHalDev->dev_base )+( chan *64)))  & TEARDOWN_VAL) != TEARDOWN_VAL)
  {
     RxTeardownInt(HalDev->dev_base, chan);

     //Assuming that both are set at the same time
     TxTeardownInt(HalDev->dev_base, chan, 0);
     TxTeardownInt(HalDev->dev_base, chan, 1);
  }
#endif
  spin_unlock_irqrestore(&chan_close_lock, flags);

  printk("ChannelTeardown returned rc = %d\n", rc);
  //return rc;
  return 0;
}

void tn7sar_exit(struct atm_dev *dev, Tn7AtmPrivate *priv)
{
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;

  dgprintf(4, "tn7sar_exit()\n");

  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  tn7sar_deactivate_vc(priv, RESERVED_OAM_CHANNEL);//15);  de-activate oam channel

  pHalFunc->Close(pHalDev, 2);
  pHalFunc->Shutdown(pHalDev);

  kfree(priv->pSarOsFunc);

}

void tn7sar_get_sar_version(Tn7AtmPrivate *priv, char **pVer)
{
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;

  dgprintf(4, "tn7sar_get_sar_version()\n");

  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;
  pHalFunc->Control(pHalDev, "Version", "Get", pVer);
}


int tn7sar_get_near_end_loopback_count(unsigned int *pF4count, unsigned int *pF5count)
{
  unsigned int f4c, f5c;

  f4c = *(volatile unsigned int *)SAR_PDSP_OAM_F4LB_COUNT_REG_ADDR;
  f5c = *(volatile unsigned int *)SAR_PDSP_OAM_F5LB_COUNT_REG_ADDR;
  *pF4count = f4c;
  *pF5count = f5c;

  return 0;
}


int tn7sar_tx_flush(void *privContext, int chan, int queue, int skip)
{
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  Tn7AtmPrivate *priv;
  int value=0;

  dgprintf(4, "tn7sar_tx_flush()\n");

  //printk("tn7sar_tx_flush\n");
  priv = (Tn7AtmPrivate *)privContext;
  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;
  value = (skip << 16) | (queue << 8) | (chan);
  pHalFunc->Control(pHalDev, "TxFlush", "Set", &value);

  return 0;

}


int tn7sar_oam_generation(void *privContext, int chan, int type, int vpi, int vci, int timeout)
{
  unsigned int regv = 0;
  static unsigned int tag;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  Tn7AtmPrivate *priv;
  unsigned int llid[4]={0xffffffff,0xffffffff,0xffffffff,0xffffffff};
  char           cphal_cmd[40];

  dgprintf(2, "tn7sar_oam_generation()\n");

  priv = (Tn7AtmPrivate *)privContext;
  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  if(timeout >= 5000)
  {
    if(timeout == 6000)
    {
      tn7sar_clear_pvc_table();
      return 0;
    }
    timeout = oamLbTimeout;
  }

    //jz  CQ9861: Protect the SAR from multiple oam pings, returning EBUSY error code
  if( (oamPingStatus == OAM_PING_PENDING) || (oamPingStatus == OAM_PING_PENDING_RECVD))
  {
#ifdef __AUTOPVC_DBG
    printk("tn7sar_oam_generation: OAM Ping Dropped! pingStat %d, oamHdr %X vpi-vci: (%d,%d)\n",
        oamPingStatus, oamAtmHdr, vpi, vci);
#endif// __AUTOPVC_DBG
    return EBUSY;
  }

  pHalFunc->Control(pHalDev,"OamLbTimeout", "Set", &timeout);

  /* calculate atm header */
  oamAtmHdr = tn7sar_atm_header(vpi,vci);

  if(chan == ATM_NO_DMA_CHAN)
  {
    //tn7sar_unmatched_oam_generation(priv, vpi, vci, type);
    //return 0;
    if(vci==0)
    {
      oamPingStatus = OAM_PING_FAILED;
      return EINVAL; //jz: Now returning error code instead of success code 0;
    }

    chan = RESERVED_OAM_CHANNEL; //15;

    /* config the atm header */
    //pHalFunc->Control(pHalDev,"TxVc_AtmHeader.15", "Set", &oamAtmHdr);
    sprintf( cphal_cmd, "TxVc_AtmHeader.%d", RESERVED_OAM_CHANNEL);
    pHalFunc->Control(pHalDev, cphal_cmd, "Set", &oamAtmHdr);

  }

  oam_type = type;

  regv = (0xff & chan);
  switch(type)
  {
    case 0:
      regv |= (1<<12); //f5 end
      dgprintf(2, "f5 loop back\n");
      break;
    case 1:
      regv |= (1<<13); // f4 end
      break;
    case 2:
      regv |= (1<<14); //f5 seg
      break;
    case 3:
      regv |= (1<<15); //f4 seg
      break;
    default:
      break;
  }
  oamPingStatus = OAM_PING_PENDING;
  pHalFunc->OamLoopbackConfig(pHalDev, regv, llid, tag);
  tag++;

  return 0;
}

int tn7sar_proc_oam_ping(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len = 0;
  unsigned int oam_ps = oamPingStatus;

  if( oam_ps == OAM_PING_PENDING_RECVD )
    oam_ps = OAM_PING_PENDING;  //jz  CQ9861: Only export the PENDING status, not internal state

  len += sprintf(buf+len, "%d\n", oam_ps); //oamPingStatus);

  return len;
}

int tn7sar_proc_pvc_table(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len = 0;
  int i;

  for(i=0;i<16;i++)
  {
    if(pvc_result[i].bInUse)
    {
      len += sprintf(buf+len, "%d,%d\n", pvc_result[i].vpi,pvc_result[i].vci);
    }
    else
    {
      len += sprintf(buf+len, "0,0\n");
    }
  }
  return len;
}



int tn7sar_proc_sar_stat(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len = 0;
  int limit = count - 80;
  struct atm_dev *dev;
  Tn7AtmPrivate *priv;
  int i, j, k;
  int stat_len;
  char statString[32];
  unsigned int *pStateBase, *pSarStat;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  int dBytes;

  dev = (struct atm_dev *)data;
  priv = (Tn7AtmPrivate *)dev->dev_data;

  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  len += sprintf(buf+len, "SAR HAL Statistics");
  for(i=0;i<MAX_DMA_CHAN;i++)
  {
    if(priv->lut[i].inuse)
    {
      if(len<=limit)
      len += sprintf(buf+len, "\nChannel %d:\n",priv->lut[i].chanid);
      k=0;
      for(j=0;j<4;j++)
      {
        stat_len =sprintf(statString, "Stats;%d;%d", j,priv->lut[i].chanid);
        statString[stat_len]=0;
        pHalFunc->Control(pHalDev, statString, "Get", &pSarStat);
        pStateBase = pSarStat;

        while(pSarStat)
        {
          if((char *)*pSarStat == NULL)
            break;
          if(len<=limit)
          {
            dBytes = sprintf(buf+len, "%s: ",(char *) *pSarStat);
            len += dBytes;
            k += dBytes;
          }
          pSarStat++;
          if(len<=limit)
          {
            dBytes = sprintf(buf+len, "%s;  \n",(char *) *pSarStat);
            len += dBytes;
            k += dBytes;
          }
          pSarStat++;

          if(k > 60)
          {
            k=0;
            if(len<=limit)
            len += sprintf(buf+len, "\n");
          }
        }

        kfree(pStateBase);
      }
    }
  }

  return len;
}

void tn7sar_get_sar_firmware_version(unsigned int *pdsp_version_ms, unsigned int *pdsp_version_ls)
{

  *pdsp_version_ms = (SarPdspFirmware[9]>>20) & 0xF;
  *pdsp_version_ls = (SarPdspFirmware[9]>>12) & 0xFF;
  return;
}
