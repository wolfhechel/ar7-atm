/******************************************************************************
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
 * 0/11/02         Zhicheng Tang, created.
 *
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
  OAM_PING_NOT_STARTED
}OAM_PING;

/* PDSP OAM General Purpose Registers (@todo: These need to be used in the HAL!) */

#define SAR_PDSP_HOST_OAM_CONFIG_REG_ADDR 0xa3000020
#define SAR_PDSP_OAM_CORR_REG_ADDR        0xa3000024
#define SAR_PDSP_OAM_LB_RESULT_REG_ADDR   0xa3000028
#define SAR_PDSP_OAM_F5LB_COUNT_REG_ADDR  0xa300002c
#define SAR_PDSP_OAM_F4LB_COUNT_REG_ADDR  0xa3000030

#define SAR_FREQUNCY 50000000

#define AAL5_PARM "id=aal5, base = 0x03000000, offset = 0, int_line=15, ch0=[RxBufSize=1522; RxNumBuffers = 32; RxServiceMax = 50; TxServiceMax=50; TxNumBuffers=32; CpcsUU=0x5aa5; TxVc_CellRate=0x3000; TxVc_AtmHeader=0x00000640]"
#define SAR_PARM "id=sar,base = 0x03000000, reset_bit = 9, offset = 0; UniNni = 0, PdspEnable = 1"
#define RESET_PARM "id=ResetControl, base=0xA8611600"
#define CH0_PARM "RxBufSize=1522, RxNumBuffers = 32, RxServiceMax = 50, TxServiceMax=50, TxNumBuffers=32, CpcsUU=0x5aa5, TxVc_CellRate=0x3000, TxVc_AtmHeader=0x00000640"

#define MAX_PVC_TABLE_ENTRY 16

sar_stat_t sarStat;
 
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
static unsigned int oamPingStatus;
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

static void tn7sar_data_invalidate(void *pmem, int size)
{
  unsigned int i,Size=(((unsigned int)pmem)&0xf)+size;

  for (i=0;i<Size;i+=16,pmem+=16)
  {
    __asm__(" .set mips3 ");
    __asm__("cache  17, (%0)" : : "r" (pmem));
    __asm__(" .set mips0 ");
  }

}

static void tn7sar_data_writeback(void *pmem, int size)
{
  unsigned int i,Size=(((unsigned int)pmem)&0xf)+size;

  for (i=0;i<Size;i+=16,pmem+=16)
  {
    __asm__(" .set mips3 ");
    __asm__(" cache  25, (%0)" : : "r" (pmem));
    __asm__(" .set mips0 ");
  }
}


static int 
tn7sar_find_device(int unit, const char *find_name, void *device_info)
{
  int ret_val = 0;
  char **ptr;

  ptr = (char **)device_info;
  dprintf(3,"tn7sar_find_device\n");
  if(strcmp(find_name, "aal5")==0)
  {
    //dprintf(4,"pAal5=%s\n", pAal5);
    *ptr = pAal5;
  }
  else if(strcmp(find_name, "sar")==0)
  {
    dprintf(3, "pSar=%s\n", pSar);
    *ptr = pSar;
  }  
  else if(strcmp(find_name, "reset")==0)
  {
    dprintf(3, "pReset=%s\n", pReset);
    *ptr = pReset;
  }

  device_info = NULL;

  return(ret_val);
}

static int 
tn7sar_get_device_parm_uint(void *dev_info, const char *param, unsigned int *value)
{
  char *dev_str;
  char *pMatch;
  int i=0, j=0;
  char val_str[64];
  unsigned int val;
  int base = 10;

  dprintf(6, "tn7sar_get_device_parm_uint()\n");

  dev_str = (char *)dev_info;
  dprintf(3, "parm=%s\n", param);
  pMatch = strstr(dev_str, param);
  //dprintf(4, "pMatch=%s\n", pMatch);
  if(pMatch)
  {
    //get "=" position
    while(pMatch[i] != 0x3d)
    {
      i++;
    }
    i++;
    // get rid of spaces
    while(pMatch[i]==0x20)
    {
      i++;
    }
    //get rid of 0x
    if(pMatch[i]==0x30)
    {
      if(pMatch[i+1] == 0x58 || pMatch[i+1] == 0x78)
      {
        i+=2;
        base = 16;
      }
    }

    // get next delineator
    while(pMatch[i] != 0x2c && pMatch[i] != 0x0)
    {
      val_str[j]=pMatch[i];
      j++;
      i++;
    }
    val_str[j]=0;
    //dprintf(4, "val_str=\n%s\n", val_str);
    //xdump(val_str, strlen(val_str) + 1, 4);
    val = simple_strtoul(val_str, (char **)NULL, base);
    dprintf(4, "val =%d\n", val);
    *value = val;
    return 0;
  }


  dprintf(3, "match not found.\n");
  if(strcmp(dev_str, "debug")==0)
  {
    dprintf(6,"debug..\n");
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

  
  dprintf(3, "tn7sar_get_device_parm_value().\n");
  
  pVal = (char *) parm_data;
  dev_str = (char *)dev_info;
  dprintf(3, "dev_info: \n%s\n", dev_str);
  dprintf(3, "param=%s\n", param);
  if(strcmp(param, "Ch0")==0)
  {
    *(char **)value = CH0_PARM;
    dprintf(3, "value =%s\n", *(char **)value);
    return 0;
  }

  pMatch = strstr(dev_str, param);
  if(pMatch)
  {
    //get "=" position
    while(pMatch[i] != 0x3d)
    {
      i++;
    }
    i++;
    // get rid of spaces
    while(pMatch[i]==0x20)
    {
      i++;
    }

    if(pMatch[i] != 0x5b) //"["
    {
      // get next delineator
      while(pMatch[i] != 0x2c && pMatch[i] != 0x0)
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
      while(pMatch[i] != 0x5d)
      {
        if(pMatch[i] == 0x3b) //";"
          pVal[j] = 0x2c;
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

static void tn7sar_free(void *pmem)
{
    kfree(pmem);
}

static void 
tn7sar_free_buffer(OS_RECEIVEINFO *os_receive_info, void *pmem)
{
  tn7atm_free_rx_skb(os_receive_info);
}

static void tn7sar_free_dev(void *pmem)
{
  kfree(pmem);
}

static void tn7sar_free_dma_xfer(void *pmem)
{
  kfree(pmem);
}


static int
tn7sar_control(void *dev_info, const char *key, const char *action, void *value)
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
    dprintf(2, "get looback source call back\n");
    if (strcmp(action, "Set") == 0)
    {       
      dprintf(2, "oam result = %d\n", *(unsigned int *)value);
      oamFarLBCount[oam_type] = oamFarLBCount[oam_type] + *(unsigned int *)value;
      if(oamPingStatus == OAM_PING_PENDING)
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


static void
tn7sar_sarhal_isr_register(OS_DEVICE *os_dev, int(*hal_isr)(HAL_DEVICE *, int *), int interrupt_num)
{
  tn7atm_sarhal_isr_register(os_dev, hal_isr, interrupt_num);
}

static void 
tn7sar_isr_unregister(OS_DEVICE *os_dev, int interrupt_num)
{
    /* TODO */
}


static void *
tn7sar_malloc_rxbuffer(unsigned int size, void *mem_base, unsigned int mem_range, HAL_DEVICE *hal_dev, 
                       HAL_RECEIVEINFO *hal_info, OS_RECEIVEINFO **os_receive_info, OS_DEVICE *os_dev)
{
   return tn7atm_allocate_rx_skb(os_dev, os_receive_info, size);
}

static void *
tn7sar_malloc_dev(unsigned int size)
{
  return(kmalloc(size, GFP_KERNEL));
}

static void *
tn7sar_malloc_dma_xfer(unsigned int size, void *mem_base, unsigned int mem_range)
{
  dprintf(4, "tn7sar_malloc_dma_xfer, size =%d\n", size);

  return (kmalloc(size, GFP_DMA |GFP_KERNEL));
  
}

static void * 
tn7sar_memset(void *dst, int set_char, size_t count)
{
    return (memset(dst, set_char, count));
}

static int tn7sar_printf(const char *format, ...)
{
  /* TODO: add debug levels */
  static char buff[256];
  va_list  ap; 

  va_start( ap, format); 
  vsprintf((char *)buff, format, ap); 
  va_end(ap); 

  printk("SAR HAL: %s\n", buff);
  return(0);
}

static void tn7sar_record_pvc(int atmheader)
{
  int vci,vpi;
  int i;

  vci = 0xffff & (atmheader >> 4);
  vpi = 0xff & (atmheader >> 20);
  for(i=0;i<MAX_PVC_TABLE_ENTRY;i++)
  {
    if(pvc_result[i].bInUse)
    {
      if(pvc_result[i].vpi == vpi && pvc_result[i].vci == vci)
      {
        return;
      }
    }
  }
  for(i=0;i<MAX_PVC_TABLE_ENTRY;i++)
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
 
  for(i=0;i<MAX_PVC_TABLE_ENTRY; i++)
  {
    pvc_result[i].bInUse = 0;
    pvc_result[i].vpi = 0;
    pvc_result[i].vci = 0;
  }
}

int tn7sar_process_unmatched_oam(FRAGLIST *frag_list, unsigned int frag_count, unsigned int packet_size, unsigned int mode)
{

  FRAGLIST *local_list;
  int i;
  unsigned int atmHdr;

  local_list = frag_list;

  for(i=0;i<(int)frag_count;i++)
  {
    tn7sar_data_invalidate(local_list->data, (int)local_list->len);
    local_list ++;
  }
  local_list = frag_list;
  if((mode>>31)) /*vci, vpi is attached */
  {
    atmHdr = *(unsigned int *)frag_list->data;
    tn7sar_record_pvc(atmHdr);
    if(atmHdr & 0x8) //oam cell
    {
      atmHdr &= 0xfffffff0;
      if(atmHdr == oamAtmHdr)
      {
        if(oamPingStatus == OAM_PING_PENDING)
        {
          oamPingStatus = OAM_PING_SUCCESS;
          oamFarLBCount[oam_type] = oamFarLBCount[oam_type] + 1;
        }
        return 0;
      }
    }
  }
 
  return 0;
}


static int 
tn7sar_receive(OS_DEVICE *os_dev,FRAGLIST *frag_list, unsigned int frag_count, unsigned int packet_size, 
                 HAL_RECEIVEINFO *hal_receive_info, unsigned int mode)
{   
  int ch;
  struct atm_dev *dev;
  Tn7AtmPrivate *priv;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  int bRet;


  dprintf(4, "tn7sar_receive\n");

  dev = (struct atm_dev *)os_dev;
  priv= (Tn7AtmPrivate *)dev->dev_data;
  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  
  /* Mode contains channel info */
  ch = (mode & 0xFF);
 
  if(ch == 15)
  {
    tn7sar_process_unmatched_oam(frag_list, frag_count, packet_size, mode);
    pHalFunc->RxReturn(hal_receive_info, 0);
    return 0;
  }

  if(frag_count > 1 || frag_list->len == 0)
  {
    printk("Packet fragment count > 1, not handdle.\n");
    return 1;
  }
 
  tn7sar_data_invalidate(frag_list->data, (int)frag_list->len);
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

  return bRet;
}

static int 
tn7sar_send_complete(OS_SENDINFO *osSendInfo)
{  
  return (tn7atm_send_complete(osSendInfo));
}

void
tn7sar_teardown_complete(OS_DEVICE *OsDev, int ch, int Dir)
{
  return;
}
  

/*
unsigned int tn7sar_virt(unsigned int address)
{
  return phys_to_virt(address);
}
*/

int tn7sar_init_module(OS_FUNCTIONS *os_funcs)
{
  dprintf(4, "tn7sar_init_module\n");
  if( os_funcs == 0 )
  {
    return(-1);   
  }
  os_funcs->Control                  = tn7sar_control;
  os_funcs->CriticalOn               = tn7sar_critical_on;
  os_funcs->CriticalOff              = tn7sar_critical_off;
  os_funcs->DataCacheHitInvalidate   = tn7sar_data_invalidate;
  os_funcs->DataCacheHitWriteback    = tn7sar_data_writeback;
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
  os_funcs->Printf                   = tn7sar_printf;
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
    memcpy(&aal5Parm.chan[i], &aal5Parm.chan[0], sizeof(aal5Parm.chan[0]));
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
  
  dprintf(2, "tn7sar_get_stats\n");

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
        dprintf(2, "statString=%s\n",statString);
        pHalFunc->Control(pHalDev, statString, "Get", &pSarStat);
        pStateBase = pSarStat;
        while(pSarStat)
        {
          if((char *)*pSarStat == NULL)
            break;
          dprintf(2, "%s\n", (char *) *pSarStat);
          pSarStat++;
          dprintf(2, "%s\n", (char *) *pSarStat);
          sarStat.rxErrors += os_atoul((char *) *pSarStat);
          pSarStat++;
        }
          
        kfree(pStateBase);
      }
    }
  }
  return 0;
}
          
int tn7sar_setup_oam_channel(Tn7AtmPrivate *priv)
{

  CHANNEL_INFO chInfo;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  int chan=15;  
  dprintf(4, "tn7sar_setup_oam_channel\n");

  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  memset(&chInfo, 0xff, sizeof(chInfo));

  /* channel specific */
  chInfo.Channel = 15; /* hardcoded for last channel */
  chInfo.Direction = 0;
  chInfo.Vci = 30; /* just need below 32 */
  chInfo.Vpi = 0;
  chInfo.TxVc_QosType = 2;

  /*default */
  chInfo.PktType      = PACKET_TYPE_TRANS;
  chInfo.TxServiceMax = 2;
  chInfo.RxServiceMax = 2;
  chInfo.TxNumQueues  = 1;
  chInfo.TxNumBuffers = 4;
  chInfo.RxNumBuffers = 4;
  chInfo.RxBufSize    = 256;
  chInfo.RxVc_OamToHost = 0;
  chInfo.RxVp_OamToHost = 0;
  chInfo.FwdUnkVc       = 1; //enable forwarding of unknown vc
  chInfo.TxVc_AtmHeader = tn7sar_atm_header((int)chInfo.Vpi, chInfo.Vci);
  chInfo.RxVc_AtmHeader = tn7sar_atm_header((int)chInfo.Vpi, chInfo.Vci);
  chInfo.TxVp_AtmHeader = tn7sar_atm_header((int)chInfo.Vpi, 0);
  chInfo.RxVp_AtmHeader = tn7sar_atm_header((int)chInfo.Vpi, 0);

  dprintf(4, "TxVc_AtmHeader=0x%x\n", chInfo.TxVc_AtmHeader);

  if(pHalFunc->ChannelSetup(pHalDev, &chInfo, NULL))
  {
    printk("failed to setup channel =%d.\n", chan);
    return -1;
  }
 
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


  dprintf(4, "tn7sar_init\n");

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
  dprintf(4, "tn7sar_init done");
  return 0;
}

static int
tn7sar_atm_header(int vpi, int vci)
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

int tn7sar_activate_vc(Tn7AtmPrivate *priv, short vpi, int vci, int pcr, int scr, int mbs, int cdvt, int chan, int qos)
{
  CHANNEL_INFO chInfo;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  
  dprintf(4, "tn7sar_activate_vc\n");

  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  memset(&chInfo, 0xff, sizeof(chInfo));

  /* channel specific */
  chInfo.Channel = chan;
  chInfo.Direction = 0;
  chInfo.Vci = vci;
  chInfo.Vpi = vpi;
  chInfo.TxVc_QosType = qos;
  chInfo.Priority = qos;
  
  if(chInfo.TxVc_QosType == 1) /* if the connection is VBR than set the DaMask value to tell the schedular to accumalte the credit */
  {
    chInfo.DaMask = 1;
  }  
  chInfo.TxVc_Mbs = mbs; /* use pcr as MBS */
  pcr = SAR_FREQUNCY/pcr;
  scr = SAR_FREQUNCY/scr;
  chInfo.TxVc_CellRate = scr;
  chInfo.TxVc_Pcr = pcr;

  /*default */
  chInfo.PktType      = PACKET_TYPE_AAL5;
  chInfo.TxServiceMax = TX_SERVICE_MAX;
  chInfo.RxServiceMax = RX_SERVICE_MAX;
  chInfo.TxNumQueues  = TX_QUEUE_NUM;
  chInfo.TxNumBuffers = TX_BUFFER_NUM;
  chInfo.RxNumBuffers = RX_BUFFER_NUM;
  chInfo.RxBufSize    = RX_BUFFER_SIZE;
  chInfo.RxVc_OamToHost = 0;
  chInfo.RxVp_OamToHost = 0;
  chInfo.TxVc_AtmHeader = tn7sar_atm_header((int)vpi, vci);
  chInfo.RxVc_AtmHeader = tn7sar_atm_header((int)vpi, vci);
  chInfo.TxVp_AtmHeader = tn7sar_atm_header((int)vpi, 0);
  chInfo.RxVp_AtmHeader = tn7sar_atm_header((int)vpi, 0);
  chInfo.CpcsUU = 0;

  dprintf(4, "TxVc_AtmHeader=0x%x\n", chInfo.TxVc_AtmHeader);

  if(pHalFunc->ChannelSetup(pHalDev, &chInfo, NULL))
  {
    printk("failed to setup channel =%d.\n", chan);
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

  dprintf(4, "tn7sar_send_packet\n");
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
  
  dprintf(4, "mode = %d\n", mode);

  tn7sar_data_writeback(fragList.data, len);
  if(pHalFunc->Send(pHalDev, &fragList, 1, len, new_skb, mode) != 0)
  {
    dprintf(1, "SAR hal failed to send packet.\n");
    return 1;
  }
  //tn7sar_get_stats(priv);
  sarStat.txPktCnt++;
  sarStat.txBytes +=len;
  return 0;
}



int tn7sar_handle_interrupt(struct atm_dev *dev, Tn7AtmPrivate *priv)
{
  int more;
  int rc;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  int (*halIsr)(HAL_DEVICE *halDev, int *work);

  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;
  halIsr = priv->halIsr;

  rc = halIsr(pHalDev, &more);
  
  pHalFunc->PacketProcessEnd(pHalDev);

  return rc;
}


int tn7sar_deactivate_vc(Tn7AtmPrivate *priv, int chan)
{
  unsigned int mode;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;

  dprintf(4, "tn7sar_deactivate_vc\n");
  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  mode = 0xf; //tear down everything, wait for return;

  pHalFunc->ChannelTeardown(pHalDev, chan, mode);
  return 0;
}

void tn7sar_exit(struct atm_dev *dev, Tn7AtmPrivate *priv)
{
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;

  dprintf(4, "tn7sar_exit()\n");

  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;
  
  tn7sar_deactivate_vc(priv, 15); // de-activate oam channel

  pHalFunc->Close(pHalDev, 2);
  pHalFunc->Shutdown(pHalDev);

  kfree(priv->pSarOsFunc);
  
}

void tn7sar_get_sar_version(Tn7AtmPrivate *priv, char **pVer)
{
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;

  dprintf(4, "tn7sar_get_sar_version()\n");

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


int tn7sar_unmatched_oam_generation(void *privContext, int vpi, int vci, int type)
{

  unsigned int regv = 0;
  int chan=15;
  static unsigned int tag;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  Tn7AtmPrivate *priv;
  unsigned int llid[4]={0xffffffff,0xffffffff,0xffffffff,0xffffffff};

  dprintf(4, "tn7sar_unknow_oam_generation()\n");
 
  priv = (Tn7AtmPrivate *)privContext;
  pHalFunc = (HAL_FUNCTIONS *)priv->pSarHalFunc;
  pHalDev  = (HAL_DEVICE *)priv->pSarHalDev;

  if(vci==0)
  {
    oamPingStatus = OAM_PING_FAILED;
    return 0;
  }
  /* calculate atm header */
  oamAtmHdr = tn7sar_atm_header(vpi,vci);

  /* config the atm header */
  pHalFunc->Control(pHalDev,"TxVc_AtmHeader.15", "Set", &oamAtmHdr);

  /*record oam type */
  oam_type = type;

  regv = (0xff & chan);
  
  switch(type)
  {
    case 0:
      regv |= (1<<12); //f5 end 
      dprintf(2, "f5 loop back\n");
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

int tn7sar_oam_generation(void *privContext, int chan, int type, int vpi, int vci, int timeout)
{
  unsigned int regv = 0;
  static unsigned int tag;
  HAL_FUNCTIONS *pHalFunc;
  HAL_DEVICE    *pHalDev;
  Tn7AtmPrivate *priv;
  unsigned int llid[4]={0xffffffff,0xffffffff,0xffffffff,0xffffffff};

  dprintf(2, "tn7sar_oam_generation()\n");

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


  pHalFunc->Control(pHalDev,"OamLbTimeout", "Set", &timeout);

  if(chan == ATM_NO_DMA_CHAN)
  {
    tn7sar_unmatched_oam_generation(priv, vpi, vci, type);
    return 0;
  }

  /* calculate atm header */
  oamAtmHdr = tn7sar_atm_header(vpi,vci);
  
  oam_type = type;

  regv = (0xff & chan);
  switch(type)
  {
    case 0:
      regv |= (1<<12); //f5 end 
      dprintf(2, "f5 loop back\n");
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

  len += sprintf(buf+len, "%d\n", oamPingStatus);

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

  len += sprintf(buf+len, "SAR HAL Statistics\n");
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
            dBytes = sprintf(buf+len, "%s;  ",(char *) *pSarStat);
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
