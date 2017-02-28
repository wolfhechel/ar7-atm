/* 
 *    Tnetd73xx ATM driver.
 *    by Zhicheng Tang, ztang@ti.com
 *    2000 (c) Texas Instruments Inc.
 *
 *
*/

#ifndef __TN7ATM_H
#define __TN7ATM_H

//#include  "mips_support.h"
#include  <linux/list.h>

#define ATM_REG_OK 1
#define ATM_REG_FAILED 0

#define TX_SERVICE_MAX    32
#define RX_SERVICE_MAX    20
#define TX_BUFFER_NUM     64
#define RX_BUFFER_NUM     28
#define TX_QUEUE_NUM      2
#define RX_BUFFER_SIZE    1582

#define TX_DMA_CHAN       16    /* number of tx dma channels available */
#define MAX_DMA_CHAN      16
#define ATM_NO_DMA_CHAN   MAX_DMA_CHAN + 1  /* no tx dma channels available */
#define ATM_SAR_INT       15
#define ATM_SAR_INT_PACING_BLOCK_NUM 2
#define ATM_DSL_INT       39

#define CONFIG_ATM_TN7ATM_DEBUG 0 /* Debug level (0=no mtn7s 5=verbose) */

#define TN7ATM_DEV(d)          ((struct tn7atm*)((d)->dev_data))


/* Avalanche SAR state information */

typedef enum tn7atm_state 
{
  TN7ATM_STATE_REGISTER             /* device registered */
}tn7atm_state;

typedef struct _sar_stat
{
  unsigned int txErrors;
  unsigned int rxErrors;
  unsigned int rxPktCnt;
  unsigned int txPktCnt;
  unsigned int rxBytes;
  unsigned int txBytes;
}sar_stat_t;

/* Host based look up table to xref Channel Id's, VPI/VCI, LC, CID, packet type */
typedef struct _tn7atm_tx_lut
{
  int         inuse;       /* is DMA channel available (1=y)  */
  int         chanid;      /*  DMA channel ID   (0-0x1f) This corresponds to the Channel ID
                              that is used in the connection config reg (TN7ATM_CONN_CONFIG) */
  int         vpi;         /* Virtual path identifier         */
  int         vci;         /* Virtual channel identifier      */
  void        *vcc;
  int         bClosing;
  int         ready;
  void        *net_device;
  int         tx_total_bufs;
  int         tx_used_bufs[2];
  int         netqueue_stop;
}tn7atm_lut_t;              

/* per device data */

typedef struct _tn7atm_private
{
  struct _tn7atm_private       *next;               /* next device */
  struct atm_dev              *dev;                /* ATM device */
  struct net_device_stats     stats;         /* Used to report Tx/Rx frames from ifconfig */
  tn7atm_lut_t                 lut[MAX_DMA_CHAN];  /* Tx DMA look up table (LUT) */
  int                         dsl_irq;            /* ATM SAR TransmitA interrupt number */
  int                         sar_irq;            /* ATM SAR ReceiveA interrupt number */
  char*                       name;                /* device name */
  char*                       proc_name;           /* board name under /proc/atm */
  unsigned int                available_cell_rate; /* cell rate */
  unsigned int                connection_cell_rate; /* cell rate */
  int                         lConnected;

  /* Tnetd73xx CPHAL */
  void                        *pSarHalDev;
  void                        *pSarHalFunc;
  void                        *pSarOsFunc;
  void                        *halIsr;
  int                         int_num;

  /* turbo dsl */
  int                          bTurboDsl;

  /* spin lock for netifqueue */
  spinlock_t                   netifqueueLock;
  int                          netifqueueLockFlag;
  int                          xmitStop; /* temp fix for SAR problem */
}tn7atm_private_t, Tn7AtmPrivate;



/* ATM adaptation layer id */
typedef enum tn7atm_aal {
    TN7ATM_AAL0  = 0,
    TN7ATM_AAL2  = 2,
    TN7ATM_AAL5  = 5,
} tn7atm_aal_t;




#endif
