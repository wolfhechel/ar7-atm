/*********************************************************************************************
 *    Tnetd73xx ATM driver.
 *    by Zhicheng Tang, ztang@ti.com
 *    2000 (c) Texas Instruments Inc.
 *
 *   09/01/05 AV      Adding Instrumentation code for some of the key functions.
 *                    This can be enabled/disabled with the defining of the
 *                    TIATM_INST_SUPP macro.(CQ 9907).
 *  9/22/05 AV   Adding support for the new LED driver in BasePSP 7.4. A new macro TN7DSL_LED_ACTION
 *               has been used to replace the direct calls to the old LED handler.
 * UR8_MERGE_START CQ10450   Jack Zhang
 * 4/04/06  JZ   CQ10450: Increase Interrupt pacing to 6 when downstream connection rate is higher than 20Mbps
 * UR8_MERGE_END   CQ10450*
 * 09/18/07 CPH    CQ11466   Added EFM Support
 *************************************************************************************************/

#ifndef __TN7ATM_H
#define __TN7ATM_H

//#include  "mips_support.h"
#include  <linux/list.h>

#include <linux/config.h>

#ifdef CONFIG_MODVERSIONS
#include <linux/modversions.h>
#endif

#ifdef AR7_EFM
#include "tn7efm.h"
#endif
#define ATM_REG_OK 1
#define ATM_REG_FAILED 0

#define TX_SERVICE_MAX    32
#define RX_SERVICE_MAX    16
#define TX_BUFFER_NUM     64
#define RX_BUFFER_NUM     28
#define TX_QUEUE_NUM      2
#define RX_BUFFER_SIZE    1582

#define TX_DMA_CHAN       16    /* number of tx dma channels available */
#define MAX_DMA_CHAN      16
#define EOC_DMA_CHAN      MAX_DMA_CHAN      /* reserve this chan for clear eoc */
#define ATM_NO_DMA_CHAN   MAX_DMA_CHAN + 1  /* no tx dma channels available */
#define ATM_SAR_INT       15
#define ATM_SAR_INT_PACING_BLOCK_NUM 2

#define ATM_DSL_INT_SANGAM  39   /* for Sangam */
#define ATM_DSL_INT_OHIO    23   /* for Ohio */
#define SANGAM_DEFAULT_IPACEMAX_VAL 3
#define OHIO_DEFAULT_IPACEMAX_VAL 4
#define ANNEX_M_2PLUS_PACEMAX_VAL 6

//UR8_MERGE_START CQ10450   Jack Zhang
#define HIGH_DS_CONN_RATE_THRESHOLD       20000  //20 Mbps=20,000kbps
#define HIGH_DS_CONN_RATE_PACEMAX_VAL     6
//UR8_MERGE_END   CQ10450*

/* Temporary fix till the Base PSP doesn't export the following in a header file. */
extern int avalanche_request_pacing(int irq_nr, unsigned int blk_num, unsigned int pace_val);

/* Check if the release.h is mapped into the include folder. */
#ifdef CONFIG_HAS_RELEASE_H_FILE
#include <linux/release.h>
#endif /* CONFIG_HAS_RELEASE_H_FILE */

/* Base PSP 7.4 support */
#if ((PSP_VERSION_MAJOR == 7) && (PSP_VERSION_MINOR == 4))
#define TIATM_INST_SUPP     /* Enable Instrumentation code. */
#define __NO__VOICE_PATCH__ /* Not required anymore. */

#if defined (CONFIG_MIPS_AVALANCHE_COLORED_LED)
#include <asm/avalanche/generic/led_manager.h>

/* LED handles */
extern void *hnd_LED_0;

#define MOD_ADSL          1
#define DEF_ADSL_IDLE     1
#define DEF_ADSL_TRAINING 2
#define DEF_ADSL_SYNC     3
#define DEF_ADSL_ACTIVITY 4

#define LED_NUM_1 0
#define LED_NUM_2 1

#endif /*defined (CONFIG_MIPS_AVALANCHE_COLORED_LED)*/

/* So as to not cause any confusion. */
#ifdef BASE_PSP_7X
#undef BASE_PSP_7X
#endif /*BASE_PSP_7X*/

#define TN7DSL_LED_ACTION(module_handle, module_name, state_id) led_manager_led_action(module_handle, state_id)

#endif /*((PSP_VERSION_MAJOR == 7) && (PSP_VERSION_MINOR == 4)) */

#ifdef CONFIG_LED_MODULE
#ifndef BASE_PSP_7X
#include <asm/avalanche/ledapp.h>
#define MOD_ADSL            1
#define DEF_ADSL_IDLE       1
#define DEF_ADSL_TRAINING   2
#define DEF_ADSL_SYNC       3
#define DEF_ADSL_TXACTIVITY 6
#define DEF_ADSL_RXACTIVITY 7

#else
/* BASE_PSP_7X is defined */
#define MOD_ADSL 1
#define DEF_ADSL_IDLE     1
#define DEF_ADSL_TRAINING 2
#define DEF_ADSL_SYNC     3
#define DEF_ADSL_ACTIVITY 4

typedef struct led_reg{
    unsigned int param;
    void (*init)(unsigned long param);
    void (*onfunc)(unsigned long param);
    void (*offfunc)(unsigned long param);
}led_reg_t;

#endif /* BASE_PSP_7X */

/* For LED wrapper functions */
extern void register_led_drv(int led_num,led_reg_t *led);
extern void deregister_led_drv( int led_num);
extern void led_operation(int mod,int state_id);

/* LED handles */
extern void *hnd_LED_0;

#define TN7DSL_LED_ACTION(module_handle, module_name, state_id) led_operation(module_name, state_id)

#define LED_NUM_1 3
#define LED_NUM_2 4
#endif /* CONFIG_LED_MODULE */

/* No LED support */
#if !defined (CONFIG_MIPS_AVALANCHE_COLORED_LED) && !defined (CONFIG_LED_MODULE)
#define TN7DSL_LED_ACTION(module_handle, module_name, state_id)
#endif

#ifdef TIATM_INST_SUPP
#include <linux/psp_trace.h>
#endif

#define CONFIG_ATM_TN7ATM_DEBUG 0 /* Debug level (0=no mtn7s 5=verbose) */

#define TN7ATM_DEV(d)          ((struct tn7atm*)((d)->dev_data))

/*** Viren: Addition of TAsklet Mode ***/
/*struct tasklet_struct tasklet;*/

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
  tn7atm_lut_t                lut[MAX_DMA_CHAN+1];  /* Tx DMA look up table (LUT) */
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

  /* turbo dsl & Sar Queue */
  int                          bTurboDsl;
  int                          sarRxBuf;//@Added to make Rx buffer configurable.
  int                          sarRxMax;//@Added to make Rx service max configurable
  int                          sarTxBuf;//@Added to make Tx buffer configurable.
  int                          sarTxMax;//@Added to make Tx service max configurable
  /* spin lock for netifqueue */
  spinlock_t                   netifqueueLock;
  int                          netifqueueLockFlag;
  int                          xmitStop; /* temp fix for SAR problem */

//UR8_MERGE_START CQ10450   Jack Zhang
  unsigned int                 chip_id;
//UR8_MERGE_END   CQ10450*

#ifdef AR7_EFM
  struct net_device           *efm_dev;                 /* EFM device */
  char*                        efm_name;                 /* device name */
  char*                        efm_proc_name;            /* board name under /proc/atm */

  // The followings are used to set the appropriate Tx/Rx buf size during mode switching
  int                          efm_sarRxBuf;//@Added to make Rx buffer configurable.
  int                          efm_sarRxMax;//@Added to make Rx service max configurable
  int                          efm_sarTxBuf;//@Added to make Tx buffer configurable.
  int                          efm_sarTxMax;//@Added to make Tx service max configurable

  int                          available_tx_bufs[NUM_EFM_CHAN];    /* Available tx buffs */
  unsigned int                 efm_ctl;        // read from env var EFM_CTL
  int                          curr_TC_mode;        // 1=ATM, 2=PTM(EFM)
  int                          EFM_mode;       // 0= ATM, 1=EFM, 4=EFM_LB,
  int                          target_TC_mode;
  struct tasklet_struct        tn7efm_tasklet;
  unsigned char                efm_mac_addr[6];
  
  // efm_initialized is used to check if EFM has been initialzed (once) or not. 
  // During mode switching, not all of the structure are teardown & re-create,
  // this flag is used to initialize these structure. 
  // This info is used during auto-switch to determine whether tn7efm_init()
  // or tn7efm_detect() need to be run.
  int                          efm_initialized; // 0=tn7efm_detect has not been run, 1= tn7efm_detect has been run
  int                          mode_switching; // 0=Not switching. 1=In the middle of switching.
  void                         *pIhw;
#endif
}tn7atm_private_t, Tn7AtmPrivate;


#define RESERVED_OAM_CHANNEL              15


/* ATM adaptation layer id */
typedef enum tn7atm_aal {
    TN7ATM_AAL0  = 0,
    TN7ATM_AAL2  = 2,
    TN7ATM_AAL5  = 5,
} tn7atm_aal_t;


/* TX flush struct */
typedef struct tx_flush {
  struct atm_vcc *vcc;
  int            queue;
  int            skip_num;
}tx_flush_t;

/* DSL generic read / write */
typedef struct dsl_read_write {
  int action;
  int offsetnum;
  int offset[8];
  unsigned int data;
}dsl_read_write_t;

/* Params for the activate vc function */
typedef struct
{
    Tn7AtmPrivate *priv;
    int vpi;
    int vci;
    int pcr;
    int scr;
    int mbs;
    int cdvt;
    int chan;
    int qos;
    int priority;
}tn7atm_activate_vc_parm_t;


#ifndef PHYS_ADDR
#define PHYS_ADDR(X)                              ((X) & 0X1FFFFFFF)
#endif

#ifndef K1BASE
#define K1BASE                                    0xA0000000
#endif

#ifndef PHYS_TO_K1
#define PHYS_TO_K1(X)                             (PHYS_ADDR(X)|K1BASE)
#endif

#endif  // __TN7ATM_H
