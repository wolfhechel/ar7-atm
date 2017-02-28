/****************************************************************************************
 *   tn7atm.c
 *   Linux atm module implementation.
 *   Zhicheng Tang 01/08/2003
 *   2003 (c) Texas Instruments Inc.
 *
 *   01/17/05 AV      Fixed the clock selection based on the DHAL
 *                    version in tn7atm_autoDetectDspBoost().
 *   02/16/05 CPH     Added Ohio250 clock support.
 *   03/19/05 CPH     Added Inter-Op DSL phy Control.
 *   04/22/05 AV      Changed the default value of the SAR Interrupt pacing
 *                    to 4 from 2 for Ohio250 to reflect the change in the
 *                    IPACEP register in the BasePSP. (MR 2344).
 *   05/10/05 AV      Added support for the proc file for exporting the
 *                    different training modes.(CQ9605).
 *   05/23/05 AV      Fixing the reported ability for the chip to be able to
 *                    support ADSL2/ADSL2+ and Re-ADSL modes.
 *   06/29/05 SAN     Added a delay of 10 ms before resing the trimReg register.
 *   07/26/05 AV      Integrating the NSP changes for the packet detection. These
 *                    changes can be disabled by defining the __NO__VOICE_PATCH__ macro.
 *   08/10/05 CPH     CQ9776 (partial) Added proc avsar_SNRpsds & avsar_QLNpsds.
 *   08/15/05 AV      Added two functions to set and get the capability of the chip,
 *                    tn7atm_read_can_support_adsl2 and tn7atm_set_can_support_adsl2.
 *   08/24/05 AV      Removed disabling of the queue for upstream performance in Annex M.
 *   09/01/05 AV      Adding Instrumentation code for some of the key functions.(CQ 9907).
 *   09/06/05 AV      Adding support for VBR-rt and VBR-nrt modes.(CQ9808).
 *   09/13/05 AV      Relocating the turbodsl code here, as the source code has to be released.
 *   09/21/05 AV      Renamed the overloaded memcmp and memcpy functions to
 *                    tn7atm_memcmp and tn7atm_memcpy respectively to allow support
 *                    for being compiled directly into the kernel.
 *   9/22/05   AV     Adding support for the new LED driver in BasePSP 7.4. A new macro TN7DSL_LED_ACTION
 *                    has been used to replace the direct calls to the old LED handler.
 *   10/04/05 CPH     Added new (1.61v) 7100 support
 *   10/07/05 AV/CPH  Changed compile time selection of dslhal_api_getQLNpsds() dslhal_api_getSNRpsds()
 *                    and for code size reduction.
 *   11/03/05 CPH     Added ClearEoc ACK.
 *   11/07/05 CPH/ZChang Change 'sbk' to 'skb' in tn7atm_send() TIATM_INST_SUPP. (was typo)
 *   11/20/05 CT      CQ10076 - Added code to report chipset ID to avsar_ver
 *                             in the proc file system
 *
 *   12/07/05 AV      Added a check in the tn7atm_init() to verify that the dslhal_api_boostDspFrequency()
 *                    was able to boost the frequency of the DSP and SAR.
 *   01/25/06  JZ     CQ: 10273 Aztech/Singtel oam pvc management issue, new PDSP 0.54
 *   01/25/06  JZ     CQ: 10275 Add Remote Data Log code. Search ADV_DIAG_STATS in ATM  
 *                    driver code and manually turn on it for enabling the feature
 *   02/04/06 CPH     CQ10280: Add DSL_PHY_CNTL_0 and DSL_PHY_CNTL_1 support
 *    UR8_MERGE_START CQ10450   Jack Zhang
 *    4/04/06  JZ   CQ10450: Increase Interrupt pacing to 6 when downstream connection rate is higher than 20Mbps
 *    UR8_MERGE_END   CQ10450*
 *    UR8_MERGE_START CQ10682   Jack Zhang
 *    6/15/06  JZ   CQ10682: Display ADSL training messages in ATM driver proc file system
 *    UR8_MERGE_END   CQ10682*
 *   UR8_MERGE_START CQ10700 Manjula K
 *   07/24/06 MK      CQ10700: Added counters for reporting packets dropped by ATM Driver/SAR
 *   UR8_MERGE_END CQ10700
 *    UR8_MERGE_START CQ10979   Jack Zhang
 *    10/4/06  JZ     CQ10979: Request for TR-069 Support for RP7.1
 *    UR8_MERGE_END   CQ10979*
 *    UR8_MERGE_START CQ11057   Jack Zhang
 *    11/03/06 JZ     CQ11057: Request US PMD test parameters from CO side
 *    UR8_MERGE_END   CQ11057*
*********************************************************************************************/

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
#include "dsl_hal_api.h"
#include "tn7atm.h"
#include "tn7api.h"
#include "version.h"
#define _SIZE_T_
#include "dsl_hal_register.h"

#ifdef MODULE
MODULE_DESCRIPTION ("Tnetd73xx ATM Device Driver");
MODULE_AUTHOR ("Zhicheng Tang");
#endif

#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#define STOP_EMPTY_BUFF 2
#define START_EMPTY_BUFF 3
/* externs */

/*end of externs */

#ifndef TI_STATIC_ALLOCATIONS
#define TI_STATIC_ALLOCATIONS
#endif

#define tn7atm_kfree_skb(x)     dev_kfree_skb(x)

extern unsigned int SAR_FREQUNCY;

static int CanSuppADSL2 = TRUE;
static int EnableQoS = FALSE;

/* prototypes */
static int tn7atm_set_can_support_adsl2 (int can);

static int tn7atm_open (struct atm_vcc *vcc, short vpi, int vci);

static void tn7atm_close (struct atm_vcc *vcc);

static int tn7atm_ioctl (struct atm_dev *dev, unsigned int cmd, void *arg);

static int tn7atm_send (struct atm_vcc *vcc, struct sk_buff *skb);

static int tn7atm_change_qos (struct atm_vcc *vcc, struct atm_qos *qos,
                              int flags);

static int tn7atm_detect (void);
static int tn7atm_init (struct atm_dev *dev);
static int tn7atm_irq_request (struct atm_dev *dev);
static int tn7atm_proc_version (char *buf, char **start, off_t offset,
                                int count, int *eof, void *data);
static void tn7atm_exit (void);
static int tn7atm_proc_channels (char *buf, char **start, off_t offset,
                                 int count, int *eof, void *data);
static int tn7atm_proc_private (char *buf, char **start, off_t offset,
                                int count, int *eof, void *data);
inline static int tn7atm_queue_packet_to_sar (void *vcc1, void *skb1,
                                              int chan);

static int tn7atm_xlate_proc_name (const char *name,
                                   struct proc_dir_entry **ret,
                                   const char **residual);
static int tn7atm_proc_match (int len, const char *name,
                              struct proc_dir_entry *de);
static int tn7atm_proc_qos_read  (char *buf, char **start, off_t offset,
                                  int count, int *eof, void *data);
static int tn7atm_proc_qos_write (struct file *fp, const char *buf,
                                  unsigned long count, void *data);

//CT - Added function to return chipset Id
       void tn7atm_get_chipsetId (char *pVerId);
/*
 *
 *  Turbo DSL Implementaion
 *
 *  Zhicheng Tang   ztang@ti.com
 *
 *  2003 (c) Texas Instruments Inc.
 *
*/

/* defines and variables */
#define RFC2684_BRIDGED_HDR_SIZE 10
unsigned char LLC_BRIDGED_HEADER_2684[RFC2684_BRIDGED_HDR_SIZE] =
  {0xAA, 0xAA, 0x03, 0x00, 0x80, 0xC2, 0x00, 0x07, 0x00, 0x00};

#define RFC2684_ROUTED_HDR_SIZE 6
unsigned char LLC_ROUTED_HEADER_2684[6] ={0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00};

unsigned long PPP_LLC_HEADER  = 0xCF03FEFE;

/* struct definition */
enum
{
  AAL5_ENCAP_PPP_LLC,
  AAL5_ENCAP_PPP_VCMUX,
  AAL5_ENCAP_RFC2684_LLC_BRIDGED,
  AAL5_ENCAP_RFC2684_LLC_ROUTED
};

/* Etherent header */
typedef struct _turbodsl_ether_header
{
        unsigned char dst_mac_addr[6];
        unsigned char src_mac_addr[6];
        unsigned short ether_type;
} turbodsl_ether_header_t;


/* Ip header define */
typedef struct _turbodsl_ip_header
{

        unsigned short vit;
        unsigned short total_length;
        unsigned short ip_id;
        unsigned char  flag;            /* bit 0 = 0, bit1 = don't fragment, bit2=more frag */
  unsigned char  fragment_offset; /* offset include remaining 5 bits above, which make it 13 bits */
        unsigned char  time_to_live;
        unsigned char  protocol;
        unsigned short checksum;
        unsigned int  src_ip;
        unsigned int  dst_ip;
} turbodsl_ip_header_t;

/* Arp packet define */
typedef struct _turbodsl_arp_header
{
        unsigned short hardware_type;
        unsigned short protocol_type;
        unsigned char  h_len;
        unsigned char  p_len;
        unsigned short operation  ;
        unsigned char  snd_hw_address[6];
        unsigned char  snd_pt_address[4];
        unsigned char  dst_hw_address[6];
        unsigned char  dst_pt_address[4];
} turbodsl_arp_header_t;

#define FIN_FLAG 1
#define SYN_FLAG 1<<1
#define RST_FLAG 1<<2
#define PSH_FLAG 1<<3
#define ACK_FLAG 1<<4
#define URG_FLAG 1<<5

typedef struct _turbodsl_tcp_header
{
        unsigned short src_port;
        unsigned short dst_port;
        unsigned int seq_num;
        unsigned int ack_num;
  unsigned char  offset; /* only bits 4-7 are for offset */
  unsigned char  flags;  /* bits: 0-FIN, 1-SYN, 2-RST, 3-PSH, 4-ACK, 5-URG */
  unsigned short windows;
        unsigned short checksum;
        unsigned short urgent_ptr;
} turbodsl_tcp_header_t;

/* TurboDSL function declarations. */
__inline__ static int turbodsl_check_aal5_encap_type(unsigned char *pData);
static int turbodsl_check_priority_type(unsigned char *pData);

/* ATM device operations */
struct atm_dev *mydev;
#ifdef CPATM_TASKLET_MODE
void tn7atm_handle_tasklet (unsigned long data);
struct tasklet_struct tn7atm_tasklet;
#endif

/* *INDENT-OFF* */
static const struct atmdev_ops tn7atm_ops = {
        open:           tn7atm_open,
        close:          tn7atm_close,
        ioctl:          tn7atm_ioctl,
        getsockopt:     NULL,
        setsockopt:     NULL,
        send:           tn7atm_send,
        sg_send:        NULL,
        phy_put:        NULL,
        phy_get:        NULL,
        change_qos:     tn7atm_change_qos,
};

const char drv_proc_root_folder[] = "avalanche/";
static struct proc_dir_entry *root_proc_dir_entry = NULL;
#define DRV_PROC_MODE 0644
static int proc_root_already_exists = TRUE;
static struct
{
    const unsigned char name[32];
    int (*read_func) (char* , char **, off_t , int ,int *, void *);
    int (*write_func) (struct file *, const char * , unsigned long , void *);

} proc_if[] = {
    {"avsar_ver",                   tn7atm_proc_version,           NULL},
    {"avsar_channels",              tn7atm_proc_channels,          NULL},
    {"avsar_sarhal_stats",          tn7sar_proc_sar_stat,          NULL},
    {"avsar_oam_ping",              tn7sar_proc_oam_ping,          NULL},
    {"avsar_pvc_table",             tn7sar_proc_pvc_table,         NULL},
    {"avsar_rxsnr0",                tn7dsl_proc_snr0,              NULL},
    {"avsar_rxsnr1",                tn7dsl_proc_snr1,              NULL},
    {"avsar_rxsnr2",                tn7dsl_proc_snr2,              NULL},
    {"clear_eoc_stats",             tn7dsl_proc_eoc,               NULL},
    {"avsar_bit_allocation_table",  tn7dsl_proc_bit_allocation,    NULL},
    {"avsar_dsl_modulation_schemes",tn7dsl_proc_train_mode_export, NULL},
#ifndef NO_ADV_STATS
    {"avsar_SNRpsds",               tn7dsl_proc_SNRpsds,           NULL},
    {"avsar_QLNpsds",               tn7dsl_proc_QLNpsds,           NULL},
// *    UR8_MERGE_START CQ10979   Jack Zhang
#ifdef TR69_HLIN_IN
//    {"avsar_HLINpsds",              tn7dsl_proc_HLINpsds,          NULL},
    {"avsar_HLINpsds1",             tn7dsl_proc_HLINpsds1,         NULL},
    {"avsar_HLINpsds2",             tn7dsl_proc_HLINpsds2,         NULL},
    {"avsar_HLINpsds3",             tn7dsl_proc_HLINpsds3,         NULL},
    {"avsar_HLINpsds4",             tn7dsl_proc_HLINpsds4,         NULL},
#endif //TR69_HLIN_IN
// *    UR8_MERGE_END   CQ10979*
// *    UR8_MERGE_START CQ11057   Jack Zhang
#define TR69_PMD_IN
#ifdef TR69_PMD_IN
    {"avsar_PMDTestus",             tn7dsl_proc_PMDus,            NULL},
//    {"avsar_PMDTestus1",          tn7dsl_proc_PMDus1,            NULL},
#endif  //TR69_PMD_IN
// *    UR8_MERGE_END   CQ11057 *
#endif
    {"avsar_private",               tn7atm_proc_private,           NULL},
    {"avsar_modem_training",        tn7dsl_proc_modem,             NULL},
    {"avsar_modem_stats",           tn7dsl_proc_stats,             tn7dsl_proc_write_stats},

	/* junzhao add to show dsl status at 2007.4.12 */
	{"avsar_dsl_ticks", 			tn7dsl_state_ticks, 		   NULL},    
	/* Junzhao add end */

    
#ifdef ADV_DIAG_STATS //CQ10275
//for 2.6    {"avsar_modem_adv_stats",       tn7dsl_proc_adv_stats,         NULL},
//For 2.4 kernel, due to proc file system size limitation
    {"avsar_modem_adv_stats1",      tn7dsl_proc_adv_stats1,        NULL},
    {"avsar_modem_adv_stats2",      tn7dsl_proc_adv_stats2,        NULL},
    {"avsar_modem_adv_stats3",      tn7dsl_proc_adv_stats3,        NULL},
//UR8_MERGE_START CQ10682   Jack Zhang
    {"avsar_modem_dbg_cmsgs",       tn7dsl_proc_dbg_cmsgs,         NULL},
    {"avsar_modem_dbg_rmsgs1",      tn7dsl_proc_dbg_rmsgs1,        NULL},
    {"avsar_modem_dbg_rmsgs2",      tn7dsl_proc_dbg_rmsgs2,        NULL},
    {"avsar_modem_dbg_rmsgs3",      tn7dsl_proc_dbg_rmsgs3,        NULL},
    {"avsar_modem_dbg_rmsgs4",      tn7dsl_proc_dbg_rmsgs4,        NULL},
// UR8_MERGE_END   CQ10682*
#endif //ADV_DIAG_STATS
    {"avsar_qos_enable",            tn7atm_proc_qos_read,          tn7atm_proc_qos_write}
};

/* *INDENT-ON* */

static unsigned int ATM_DSL_INT;
unsigned int def_sar_inter_pace;

int __guDbgLevel = 0;

//static spinlock_t chan_init_lock;

#ifdef DEBUG_BUILD
void xadump (unsigned char *buff, int len, int debugLev)
{
  int i, j;
  if (__guDbgLevel < debugLev)
    return;

  j = 0;
  for (i = 0; i < len; i++)
  {
    printk ("%02x ", buff[i]);
    j++;
    if (j == 8)
    {
      j = 0;
      printk ("\n");
    }
  }
  printk ("\n");
}
#endif

/* Optimized memcpy for 32-bit processors. */
void * tn7atm_memcpy(void * dst, void const * src, size_t len)
{
    long * plDst = (long *) dst;
    long const * plSrc = (long const *) src;
    char * pcDst;
    char const * pcSrc;

    if (!((int)src & 0xFFFFFFFC) && !((int)dst & 0xFFFFFFFC))
    {
        while (len >= 4)
        {
            *plDst++ = *plSrc++;
            len -= 4;
        }
    }

    pcDst = (char *) plDst;
    pcSrc = (char const *) plSrc;

    while (len--)
    {
        *pcDst++ = *pcSrc++;
    }

    return (dst);
}

/* Optimized memcmp for 32-bit processors. */
int tn7atm_memcmp(void * dst, void const * src, size_t len)
{
    long * plDst = (long *) dst;
    long const * plSrc = (long const *) src;
    char * pcDst;
    char const * pcSrc;

    if (!((int)src & 0xFFFFFFFC) && !((int)dst & 0xFFFFFFFC))
    {
        while (len >= 4)
        {
            if(*plDst++ != *plSrc++)
                return -1;  /* Not equal*/
            len -= 4;
        }
    }

    pcDst = (char *) plDst;
    pcSrc = (char const *) plSrc;

    while (len--)
    {
        if(*pcDst++ != *pcSrc++)
            return -1; /* Not equal*/
    }

    /* They are equal */
    return 0;
}

/* Moved these definitions here for all modules to use. */
__inline__ void tn7atm_data_invalidate(void *pmem, int size)
{
  unsigned int i,Size=(((unsigned int)pmem)&0xf)+size;
  if(!pmem)
    return;
  for (i=0;i<Size;i+=16,pmem+=16)
  {
    __asm__(" .set mips3 ");
    __asm__("cache  17, (%0)" : : "r" (pmem));
    __asm__(" .set mips0 ");
  }

}

__inline__ void tn7atm_data_writeback(void *pmem, int size)
{
  unsigned int i,Size=(((unsigned int)pmem)&0xf)+size;
  if(!pmem)
    return;
  for (i=0;i<Size;i+=16,pmem+=16)
  {
    __asm__(" .set mips3 ");
    __asm__(" cache  25, (%0)" : : "r" (pmem));
    __asm__(" .set mips0 ");
  }
}

/***************************************************************************
 *  Turbo DSL Implementaion
 *
 *  Zhicheng Tang   ztang@ti.com
 *
 *  2003 (c) Texas Instruments Inc.
 *
 * Function: turbodsl_check_aal5_encap_type
 * Descripation: Determine AAL5 Encapsulation type
 * Input:
 *        unsigned char *pData, AAL5 Packet buffer pointer
 ****************************************************************************/
__inline__ static int turbodsl_check_aal5_encap_type(unsigned char *pData)
  {

  if(!tn7atm_memcmp(pData, LLC_BRIDGED_HEADER_2684, 6))
    return AAL5_ENCAP_RFC2684_LLC_BRIDGED;
  if(!tn7atm_memcmp(pData, LLC_ROUTED_HEADER_2684, 6))
    return AAL5_ENCAP_RFC2684_LLC_ROUTED;
  if(!tn7atm_memcmp(pData, (unsigned char *)&PPP_LLC_HEADER, sizeof(PPP_LLC_HEADER)))
    return AAL5_ENCAP_PPP_LLC;

  return AAL5_ENCAP_PPP_VCMUX;
  }

/***************************************************************************
 *  Turbo DSL Implementaion
 *
 *  Zhicheng Tang   ztang@ti.com
 *
 *  2003 (c) Texas Instruments Inc.
 *
 * Function: turbodsl_check_priority_type
 * Descripation: Determine AAL5 Encapsulation type
 * Input:
 *        unsigned char *pData, AAL5 Packet buffer pointer.
 *        short vpi, VPI.
 *        int vci,  VCI
 ****************************************************************************/
static int turbodsl_check_priority_type(unsigned char *pData)
  {
/*  int encap;*/
  unsigned char *pP;
  unsigned short etherType;
  turbodsl_ip_header_t *pIp;
  turbodsl_tcp_header_t *pTcp;
/*  unsigned short ip_length;*/

  dgprintf(2, "turbodsl_check_priority_type ==>\n");

  /*** Viren: Eliminated local VAriable */
 /* encap = turbodsl_check_aal5_encap_type(pData); */
  pP = pData;

  switch(turbodsl_check_aal5_encap_type(pData))
    {
    case AAL5_ENCAP_RFC2684_LLC_BRIDGED:
      pP += RFC2684_BRIDGED_HDR_SIZE; //skip off aal5 encap
      pP += 12;               //skip of mac address
      etherType = *(unsigned short *)pP;
      if((etherType != 0x6488) && (etherType != 0x0008))
        {
        //Not an IP packet
        return 1;
        }

      pP +=2;                 //skip ether type
      if(etherType == 0x6488)
        {
        pP += 6;
        }
      break;
    case AAL5_ENCAP_RFC2684_LLC_ROUTED:
      pP += RFC2684_ROUTED_HDR_SIZE; //skip of encap
      pP += 2; //skip ether type
      break;
    case AAL5_ENCAP_PPP_LLC:
      pP += sizeof(PPP_LLC_HEADER);
      if((*pP == 0xff) && (*(pP+1) == 0x03)) //ppp hdlc header
        pP += 2;
      break;
    case AAL5_ENCAP_PPP_VCMUX:
      if((*pP == 0xff) && (*(pP+1) == 0x03)) //ppp hdlc header
        pP += 2;
      break;
    default:
      return 1;
    }

  pIp = (turbodsl_ip_header_t *)pP;
  if(pIp->vit != 0x0045)
    {
    //Not a IP packet
    return 1;
    }

  if(pIp->protocol != 0x06)
    {
    //not tcp packet
    return 1;
    }

  pTcp = (turbodsl_tcp_header_t *)(pP + sizeof(turbodsl_ip_header_t));

  /*** Viren: not required ***/
  /*ip_length = ((pIp->total_length>>8) + (pIp->total_length<<8));*/

  if((pTcp->flags & ACK_FLAG) && (((pIp->total_length>>8) + (pIp->total_length<<8)) <=40))
    return 0;

  return 1;
  }

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_walk_vccs(struct atm_dev *dev, short *vcc, int *vci)
 *
 *  Description: retrieve VPI/VCI for connection
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static int tn7atm_walk_vccs (struct atm_vcc *vcc, short *vpi, int *vci)
{
  struct atm_vcc *walk;

  /*
   * find a free VPI
   */
  if (*vpi == ATM_VPI_ANY)
  {

    for (*vpi = 0, walk = vcc->dev->vccs; walk; walk = walk->next)
    {

      if ((walk->vci == *vci) && (walk->vpi == *vpi))
      {
        (*vpi)++;
        walk = vcc->dev->vccs;
      }
    }
  }

  /*
   * find a free VCI
   */
  if (*vci == ATM_VCI_ANY)
  {

    for (*vci = ATM_NOT_RSV_VCI, walk = vcc->dev->vccs; walk;
         walk = walk->next)
    {

      if ((walk->vpi = *vpi) && (walk->vci == *vci))
      {
        *vci = walk->vci + 1;
        walk = vcc->dev->vccs;
      }
    }
  }

  return 0;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_sar_irq(void)
 *
 *  Description: tnetd73xx SAR interrupt.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void tn7atm_sar_irq (int irq, void *voiddev, struct pt_regs *regs)
{
  struct atm_dev *atmdev;
  Tn7AtmPrivate *priv;
  int more;
  int retval;

  dgprintf (6, "tn7atm_sar_irq\n");

#ifdef TIATM_INST_SUPP
  psp_trace (ATM_DRV_SAR_ISR_ENTER);
#endif

  atmdev = (struct atm_dev *) voiddev;
  priv = (Tn7AtmPrivate *) atmdev->dev_data;

#ifdef CPATM_TASKLET_MODE
  tasklet_schedule (&tn7atm_tasklet);
#else
  retval = tn7sar_handle_interrupt (atmdev, priv, &more);
#endif

  dgprintf (6, "Leaving tn7atm_sar_irq\n");

#ifdef TIATM_INST_SUPP
  psp_trace_par (ATM_DRV_SAR_ISR_EXIT, retval);
#endif
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_dsl_irq(void)
 *
 *  Description: tnetd73xx DSL interrupt.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void tn7atm_dsl_irq (int irq, void *voiddev, struct pt_regs *regs)
{
  struct atm_dev *atmdev;
  Tn7AtmPrivate *priv;
  int retval;

  dgprintf (4, "tn7atm_dsl_irq\n");

#ifdef TIATM_INST_SUPP
  psp_trace (ATM_DRV_DSL_ISR_ENTER);
#endif

  atmdev = (struct atm_dev *) voiddev;
  priv = (Tn7AtmPrivate *) atmdev->dev_data;

  retval = tn7dsl_handle_interrupt ();

  dgprintf (4, "Leaving tn7atm_dsl_irq\n");

#ifdef TIATM_INST_SUPP
  psp_trace_par (ATM_DRV_DSL_ISR_EXIT, retval);
#endif
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_irq_request(struct atm_dev *dev)
 *
 *  Description: Initialize Interrupt handler
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static int __init tn7atm_irq_request (struct atm_dev *dev)
{
  Tn7AtmPrivate *priv;
  char *ptr;

  dgprintf (4, "tn7atm_irq_request()\n");
  priv = (Tn7AtmPrivate *) dev->dev_data;

  /*
   * Register SAR interrupt
   */
  priv->sar_irq = LNXINTNUM (ATM_SAR_INT);      /* Interrupt line # */
  if (request_irq (priv->sar_irq, tn7atm_sar_irq, SA_INTERRUPT, "SAR ", dev))
    printk ("Could not register tn7atm_sar_irq\n");

  /*
   * interrupt pacing
   */
  ptr = prom_getenv ("sar_ipacemax");
  if (ptr)
  {
    def_sar_inter_pace = os_atoi (ptr);
  }
  avalanche_request_pacing (priv->sar_irq, ATM_SAR_INT_PACING_BLOCK_NUM,
                            def_sar_inter_pace);

  /*
   * Reigster Receive interrupt A
   */
  priv->dsl_irq = LNXINTNUM (ATM_DSL_INT);      /* Interrupt line # */
  if (request_irq (priv->dsl_irq, tn7atm_dsl_irq, SA_INTERRUPT, "DSL ", dev))
    printk ("Could not register tn7atm_dsl_irq\n");

/***** VRB Tasklet Mode ****/
#ifdef CPATM_TASKLET_MODE
  tasklet_init (&tn7atm_tasklet, tn7atm_handle_tasklet, (unsigned long) dev);
#endif
/***************/


  return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_lut_find(struct atm_vcc *vcc)
 *
 *  Description: find an TX DMA channel
 *               that matches a vpi/vci pair
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int tn7atm_lut_find (short vpi, int vci)
{
  int i;
  Tn7AtmPrivate *priv;

  priv = (Tn7AtmPrivate *) mydev->dev_data;

  if (vci == 0)                 // find first vpi channel
  {
    for (i = 0; i < MAX_DMA_CHAN; i++)
    {
      if ((priv->lut[i].vpi == vpi))
        return i;
    }
  }

  dgprintf (4, "vpi=%d, vci=%d\n", vpi, vci);
  for (i = 0; i < MAX_DMA_CHAN; i++)
  {
    if ((priv->lut[i].vpi == vpi) && (priv->lut[i].vci == vci))
      return i;
  }

  return ATM_NO_DMA_CHAN;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_lut_clear(struct atm_vcc *vcc,int chan)
 *
 *  Description: find an TX DMA channel
 *               that matches a vpi/vci pair
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static int tn7atm_lut_clear (struct atm_vcc *vcc, int chan)
{
  Tn7AtmPrivate *priv;

  priv = (Tn7AtmPrivate *) vcc->dev->dev_data;

  memset (&priv->lut[chan], 0, sizeof (priv->lut[chan]));

  return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_walk_lut(void)
 *
 *  Description: find an available TX DMA channel
 *               and initialize LUT
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static int tn7atm_walk_lut (Tn7AtmPrivate * priv)
{
  int i;

  for (i = 0; i < MAX_DMA_CHAN; i++)
  {
    if (!priv->lut[i].inuse)
    {
      return i;                 /* return available dma channel number */
    }
  }
  return ATM_NO_DMA_CHAN;       /* no tx dma channels available */
}

static int tn7atm_set_lut (Tn7AtmPrivate * priv, struct atm_vcc *vcc,
                           int chan)
{
  int retval;

  if (!priv->lut[chan].inuse)
  {
    priv->lut[chan].vpi = (int) vcc->vpi;
    priv->lut[chan].vci = vcc->vci;
    priv->lut[chan].chanid = chan;
    priv->lut[chan].inuse = 1;  /* claim the channel */
    priv->lut[chan].vcc = (void *) vcc;
    priv->lut[chan].bClosing = 0;
    priv->lut[chan].ready = 0;
    priv->lut[chan].tx_total_bufs = TX_BUFFER_NUM;
    priv->lut[chan].tx_used_bufs[0] = 0;
    priv->lut[chan].tx_used_bufs[1] = 0;
    retval = 0;
  }
  else
      retval = -1;  /* no tx dma channels available */

  return retval;
}

static void str2eaddr (char *pMac, char *pStr)
{
  char tmp[3];
  int i;

  for (i = 0; i < 6; i++)
  {
    tmp[0] = pStr[i * 3];
    tmp[1] = pStr[i * 3 + 1];
    tmp[2] = 0;
    pMac[i] = os_atoh (tmp);
  }
}

static int __init tn7atm_get_ESI (struct atm_dev *dev)
{
  int i;
  char esi_addr[ESI_LEN] = { 0x00, 0x00, 0x11, 0x22, 0x33, 0x44 };
  char *esiaddr_str = NULL;

  esiaddr_str = prom_getenv ("macc");

  if (!esiaddr_str)
  {
    // printk("Using default macc address = 00:01:02:03:04:05\n");
    esiaddr_str = "00:00:02:03:04:05";
  }
  str2eaddr (esi_addr, esiaddr_str);

  for (i = 0; i < ESI_LEN; i++)
    dev->esi[i] = esi_addr[i];

  return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_open(struct atm_vcc *vcc, short vpi, int vci)
 *
 *  Description: Device operation: open
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

 /* AV: Temporary fix, till this is imported in the atm.h header file. */
 /* Adding support for VBR-rt and VBR-nrt, default VBR is treated as VBR-nrt */
#ifndef ATM_VBR_NRT
#define ATM_VBR_NRT    ATM_VBR
#endif

#ifndef ATM_VBR_RT
#define ATM_VBR_RT     5
#endif

int tn7atm_open (struct atm_vcc *vcc, short vpi, int vci)
{
  tn7atm_activate_vc_parm_t tn7atm_activate_vc_parm;
  int rc;
  //int flags;

  dgprintf(1, "tn7atm_open()\n");


  tn7atm_activate_vc_parm.priv = (Tn7AtmPrivate *)vcc->dev->dev_data;
  if(tn7atm_activate_vc_parm.priv==NULL)
  {
    printk("null priv\n");
    return -1;
  }

  MOD_INC_USE_COUNT;

  /* find a free VPI/VCI */
  tn7atm_walk_vccs(vcc, &vpi, &vci);

  vcc->vpi = vpi;
  vcc->vci = vci;

  if ((vci == ATM_VCI_UNSPEC) || (vpi == ATM_VCI_UNSPEC))
  {
    MOD_DEC_USE_COUNT;
    return -EBUSY;
  }

  tn7atm_activate_vc_parm.vpi = vpi;
  tn7atm_activate_vc_parm.vci = vci;

  if ((vpi == CLEAR_EOC_VPI) && (vci == CLEAR_EOC_VCI))
  {
    /* always use (max_dma_chan+1) for clear eoc */
    tn7atm_activate_vc_parm.chan = EOC_DMA_CHAN;

    /* check to see whether clear eoc is opened or not */
    if (tn7atm_activate_vc_parm.priv->lut[tn7atm_activate_vc_parm.chan].inuse)
    {
      MOD_DEC_USE_COUNT;
      printk("tn7atm_open: Clear EOC channel (dmachan=%d) already in use.\n", tn7atm_activate_vc_parm.chan);
      return -EBUSY;
    }

    rc = tn7dsl_clear_eoc_setup();
    if (rc)
    {
      printk("tn7atm_open: failed to setup clear_eoc\n");
      MOD_DEC_USE_COUNT;
      return -EBUSY;
    }
    tn7atm_set_lut(tn7atm_activate_vc_parm.priv,vcc, tn7atm_activate_vc_parm.chan);
    set_bit(ATM_VF_ADDR, &vcc->flags);  /* claim address    */
    vcc->itf = vcc->dev->number;        /* interface number */
  }
  else  /* PVC channel setup */
  {
    if ((vpi==REMOTE_MGMT_VPI) && (vci==REMOTE_MGMT_VCI))
    {
      tn7atm_activate_vc_parm.chan = 14;   /* always use chan 14 for MII PVC-base romote mgmt */
    }
    else
    {
       rc = tn7atm_lut_find(vpi, vci);
      /* check to see whether PVC is opened or not */
      if(ATM_NO_DMA_CHAN != rc)
      {
        MOD_DEC_USE_COUNT;
        printk("PVC already opened. dmachan = %d\n", rc);
        return -EBUSY;
      }
      /*check for available channel */

      if((tn7atm_activate_vc_parm.chan = tn7atm_walk_lut(tn7atm_activate_vc_parm.priv)) == ATM_NO_DMA_CHAN)
      {
        printk("No TX DMA channels available\n");
        return -EBUSY;
      }
    }

   set_bit(ATM_VF_ADDR, &vcc->flags);  /* claim address    */

   vcc->itf = vcc->dev->number;        /* interface number */

   switch(vcc->qos.txtp.traffic_class)
   {
     case ATM_CBR: /* Constant Bit Rate */
       tn7atm_activate_vc_parm.qos = 0;
       tn7atm_activate_vc_parm.priority = 0;
       tn7atm_activate_vc_parm.pcr = vcc->qos.txtp.pcr;
       tn7atm_activate_vc_parm.scr = vcc->qos.txtp.pcr;
       tn7atm_activate_vc_parm.cdvt = vcc->qos.txtp.max_cdv;
       break;

     case ATM_UBR: /* Unspecified Bit Rate */
       tn7atm_activate_vc_parm.qos = 2;
       tn7atm_activate_vc_parm.priority = 2;
       break;

     case ATM_VBR: /* Variable Bit Rate-Non RealTime*/
       tn7atm_activate_vc_parm.qos = 1;
       tn7atm_activate_vc_parm.priority = 1;

       tn7atm_activate_vc_parm.pcr = vcc->qos.txtp.pcr;
       tn7atm_activate_vc_parm.scr = vcc->qos.txtp.scr;
       if(vcc->qos.txtp.max_pcr >= 0)
           tn7atm_activate_vc_parm.mbs = vcc->qos.txtp.max_pcr;
       tn7atm_activate_vc_parm.cdvt = vcc->qos.txtp.max_cdv;
       break;

     case ATM_VBR_RT:    /* Variable Bit Rate-RealTime */
       tn7atm_activate_vc_parm.qos = 1;
       tn7atm_activate_vc_parm.priority = 0;

       tn7atm_activate_vc_parm.pcr = vcc->qos.txtp.pcr;
       tn7atm_activate_vc_parm.scr = vcc->qos.txtp.scr;
       if(vcc->qos.txtp.max_pcr >= 0)
           tn7atm_activate_vc_parm.mbs = vcc->qos.txtp.max_pcr;
       tn7atm_activate_vc_parm.cdvt = vcc->qos.txtp.max_cdv;
       break;

     default:
         tn7atm_activate_vc_parm.qos = 2;
         tn7atm_activate_vc_parm.priority = 2;
         break;
   }

   //spin_lock_irqsave(&chan_init_lock, flags);
   tn7atm_set_lut(tn7atm_activate_vc_parm.priv,vcc, tn7atm_activate_vc_parm.chan);

  #if 0
     {
       extern int testflag1;
       if(testflag1==1)
       {
         printk("set pppoa copy on\n");
         vcc->qos.txtp.icr=1;
       }
     }
  #endif

    /* Activate SAR channel */
   //spin_lock_irqsave(&chan_init_lock, flags);
   rc = tn7sar_activate_vc(&tn7atm_activate_vc_parm);
   if (rc < 0)
   {
      printk("failed to activate hw channel\n");
      MOD_DEC_USE_COUNT;
      tn7atm_lut_clear(vcc, tn7atm_activate_vc_parm.chan);
      //spin_unlock_irqrestore(&chan_init_lock, flags);
      return -EBUSY;
   }

   //vcc->dev_data = (void *)&tn7atm_activate_vc_parm.priv->lut[tn7atm_activate_vc_parm.chan];
   //set_bit(ATM_VF_READY, &vcc->flags);

   //tn7atm_activate_vc_parm.priv->lut[tn7atm_activate_vc_parm.chan].ready = 1;

   //spin_unlock_irqrestore(&chan_init_lock, flags);
   //return 0;
  }

  /* insure that the the vcc struct points to the correct entry
       in the lookup table */

  vcc->dev_data = (void *)&tn7atm_activate_vc_parm.priv->lut[tn7atm_activate_vc_parm.chan];
  set_bit(ATM_VF_READY, &vcc->flags);

  //mdelay(100);
  tn7atm_activate_vc_parm.priv->lut[tn7atm_activate_vc_parm.chan].ready = 1;
  dgprintf (1, "Leave tn7atm_open\n");
  return 0;
}

void tn7atm_close (struct atm_vcc *vcc)
{
  Tn7AtmPrivate *priv;
  int dmachan;
  spinlock_t closeLock;
  unsigned int closeFlag;
  int rc;

  priv = (Tn7AtmPrivate *) vcc->dev->dev_data;
  printk ("%s: closing %d.%d.%d.%d\n", __FUNCTION__, vcc->itf, vcc->vpi, vcc->vci,
            vcc->qos.aal);

  spin_lock_irqsave (&closeLock, closeFlag);
  clear_bit (ATM_VF_READY, &vcc->flags);        /* ATM_VF_READY: channel is
                                                 * ready to transfer data */
  //spin_unlock_irqrestore (&closeLock, closeFlag);

  if ((vcc->vpi == CLEAR_EOC_VPI) && (vcc->vci == CLEAR_EOC_VCI))
  {
    dmachan = EOC_DMA_CHAN;
  }
  else
  {
    dmachan = tn7atm_lut_find (vcc->vpi, vcc->vci);
    // printk("closing channel: %d\n", dmachan);
    if (dmachan == ATM_NO_DMA_CHAN)
    {
      printk ("Closing channel not found.\n");
      spin_unlock_irqrestore (&closeLock, closeFlag);
      return;
    }
  }

  priv->lut[dmachan].bClosing = 1;
  priv->lut[dmachan].ready = 0;

  spin_unlock_irqrestore (&closeLock, closeFlag);

  if (vcc->vpi == CLEAR_EOC_VPI && vcc->vci == CLEAR_EOC_VCI)
  {
    tn7dsl_clear_eoc_close ();
  }
  else
  {
    rc = tn7sar_deactivate_vc (priv, dmachan);
    if (rc)   /* tear down channel */
    {
      printk ("failed to close channel %d.\n", dmachan);
      //spin_unlock_irqrestore (&closeLock, closeFlag);
      //return;
    }
    //printk("ChannelTeardown returned rc = %d for chan %d \n", rc, dmachan);
  }

  /*
   * ATM_VF_READY: channel is ready to transfer data
   */
  clear_bit (ATM_VF_READY, &vcc->flags);

  //spin_lock_irqsave (&closeLock, closeFlag);
  tn7atm_lut_clear (vcc, dmachan);
  //spin_unlock_irqrestore (&closeLock, closeFlag);

  MOD_DEC_USE_COUNT;

  dgprintf (1, "Leave tn7atm_close\n");
}

/* AV: These may already be defined in the atmdev.h header file */
#ifndef ATM_TXSTOP
#define ATM_TXSTOP 0x800c61f4
#endif

#ifndef ATM_TXFLUSH
#define ATM_TXFLUSH 0x800c61f5
#endif

#ifndef ATM_DSL_READ_WRITE
#define ATM_DSL_READ_WRITE 0x800c61f6
#endif

static int tn7atm_ioctl (struct atm_dev *dev, unsigned int cmd, void *arg)
{
  Tn7AtmPrivate *priv;

  priv = (Tn7AtmPrivate *) dev->dev_data;

  // printk("tn7atm_ioctl cmd =%x\n", cmd);
  switch (cmd)
  {

  case ATM_TXSTOP:             /* temp fix for SAR tear down problem */
//        printk("ioctl cmd = 0x%x (%u), arg = 0x%p (%lu)\n", cmd, cmd, arg, (unsigned long)arg);
//        printk("arg = %d\n", *(int*)arg);
    priv->xmitStop = *(int *) arg;
    // printk("Executing ATM_SETLOOP for tn7 \n");
    // printk("Stop variable = :%d: \n",priv->xmitStop);
    return 0;

    // case SAR_DSL_RESET_SOFTBOOT:
    // return tn7atm_dsl_clean_reboot();
  case ATM_TXFLUSH:
    {
      int dmachan;
      tx_flush_t *pflush;
      struct atm_vcc *vcc;

      pflush = (tx_flush_t *) arg;
      vcc = pflush->vcc;
      dmachan = tn7atm_lut_find (vcc->vpi, vcc->vci);
      if (pflush->queue == 2)
      {
        tn7sar_tx_flush ((void *) priv, dmachan, 0, pflush->skip_num);
        tn7sar_tx_flush ((void *) priv, dmachan, 1, pflush->skip_num);
      }
      tn7sar_tx_flush ((void *) priv, dmachan, pflush->queue,
                       pflush->skip_num);
      return 0;
    }
  case ATM_DSL_READ_WRITE:
    {
      dsl_read_write_t *pGeneric;
      pGeneric = (dsl_read_write_t *) arg;
      if (pGeneric->action == 0)
      {
        return tn7dsl_generic_read (pGeneric->offsetnum, pGeneric->offset);
      }
      else
      {
        return tn7dsl_generic_write (pGeneric->offsetnum, pGeneric->offset,
                                     pGeneric->data);
      }
    }

  case 0:
    return 0;
  }

  return -ENOSYS;

}

static int tn7atm_change_qos (struct atm_vcc *vcc, struct atm_qos *qos,
                              int flags)
{
  dgprintf (1, "Enter tn7atm_change_qos\n");
  dgprintf (1, "Leave tn7atm_change_qos\n");
  return 0;
}


int tn7atm_send (struct atm_vcc *vcc, struct sk_buff *skb)
{

  Tn7AtmPrivate *priv;
  int bret;
  int chan;

  dgprintf (4, "tn7atm_send()\n");

#ifdef TIATM_INST_SUPP
  psp_trace_par (ATM_DRV_PKT_TX_ENTER, skb->len);
#endif

  priv = (Tn7AtmPrivate *) vcc->dev->dev_data;

  /*
   * add vcc field in skb for clip inATMARP fix
   */
  ATM_SKB (skb)->vcc = vcc;

  /*
   * check for dsl line connection
   */
  if (priv->lConnected != 1 || priv->xmitStop == 1)
  {
    dgprintf (4, "dsl line down\n");
    if (vcc->pop)
      vcc->pop (vcc, skb);
    else
      tn7atm_kfree_skb (skb);
    priv->stats.tx_errors++; //UR8_MERGE_START_END CQ10700 Manjula K
    return 1;
  }

  /*
   * send over clear eoc
   */
  if (vcc->vpi == CLEAR_EOC_VPI && vcc->vci == CLEAR_EOC_VCI)
  {
    bret = tn7dsl_clear_eoc_send ((unsigned char *) skb->data, skb->len, 0);
    if (vcc->pop)
      vcc->pop (vcc, skb);
    else
      tn7atm_kfree_skb (skb);
  }
  else                          // send over atm channel
  {
    /*
     * check whether PVC is closing
     */
    chan = tn7atm_lut_find (vcc->vpi, vcc->vci);

    /* set br2684 dev pointer */
    priv->lut[chan].net_device = skb->dev;
    if (chan == ATM_NO_DMA_CHAN || priv->lut[chan].bClosing == 1)
    {
      dgprintf (4, "can find sar channel\n");
      if (vcc->pop)
        vcc->pop (vcc, skb);
      else
        tn7atm_kfree_skb (skb);
      priv->stats.tx_errors++; //UR8_MERGE_START_END CQ10700 Manjula K
      return 1;
    }

    /*** Viren: Added the 3rd parameter to the function to avoid calling the function again ***/
    bret = tn7atm_queue_packet_to_sar (vcc, skb, chan);
  }

#ifdef TIATM_INST_SUPP
  psp_trace_par (ATM_DRV_PKT_TX_EXIT, bret);
#endif

  return bret;
}


inline static int tn7atm_queue_packet_to_sar (void *vcc1, void *skb1,
                                              int dmachan)
{
  struct atm_vcc *vcc;
  struct sk_buff *skb;
  int priority = 1;
  Tn7AtmPrivate *priv;

  vcc = (struct atm_vcc *) vcc1;
  skb = (struct sk_buff *) skb1;

  priv = (Tn7AtmPrivate *) vcc->dev->dev_data;

  dgprintf (4, "vcc->vci=%d\n", vcc->vci);

  // turbo dsl TCP ack check
  if (priv->bTurboDsl)
    priority = turbodsl_check_priority_type (skb->data);

  // skb priority check
  if (priority != 0)
  {
    if ((skb->cb[47]) >> 1)
      priority = 1;
    else
      priority = 0;
  }

  /*
   * PANKAJ: Once a packet has been passed down to the HAL layers we need to
   * request the upper layers to stop pushing data to us. This is to ensure
   * that higher priority packets are not being held back by the lower
   * priority ones.
   */
  if ((skb->dev != NULL) && (EnableQoS))
        netif_stop_queue(skb->dev);


  if (tn7sar_send_packet (priv, dmachan, skb, skb->data, skb->len, priority))
  {
    dgprintf (1, "failed to send packet\n");
    if (vcc->pop)
      vcc->pop (vcc, skb);
    else
      tn7atm_kfree_skb (skb);


    /*
     * PANKAJ: In case of error we need to wakeup the queue again.
     */
    if ((skb->dev != NULL) && (EnableQoS))
         netif_wake_queue(skb->dev);
         
    priv->stats.tx_dropped++; //UR8_MERGE_START_END CQ10700 Manjula K

    return 1;
  }

  return 0;
}

/* functions needed by SAR HAL */

int tn7atm_send_complete (void *osSendInfo)
{
  Tn7AtmPrivate *priv;
  struct sk_buff *skb;
  struct atm_vcc *vcc;
  static unsigned int ledticks;

  dgprintf (4, "tn7atm_send_complete()\n");
#ifdef TIATM_INST_SUPP
  psp_trace (ATM_DRV_PKT_TX_COMPLETE_ENTER);
#endif

  skb = (struct sk_buff *) osSendInfo;
  priv = (Tn7AtmPrivate *) mydev->dev_data;
  vcc = ATM_SKB (skb)->vcc;
  if (vcc)
  {
    dgprintf (4, "vcc->vci=%d\n", vcc->vci);

    if (vcc->pop)
    {
      dgprintf (5, "free packet\n");
      vcc->pop (vcc, skb);
    }

  }

  /*
   * Update Stats: There may be a better place to do this, but this is a
   * start
   */
  priv->stats.tx_packets++;

#if defined (CONFIG_LED_MODULE) || defined (CONFIG_MIPS_AVALANCHE_COLORED_LED)
  if ((ledticks != jiffies) && (hnd_LED_0))
  {
    ledticks = jiffies;
    TN7DSL_LED_ACTION(hnd_LED_0, MOD_ADSL, DEF_ADSL_ACTIVITY);
  }
#endif

  /*
   * track number of buffer used
   */

  dgprintf (4, "tn7atm_send_complete() done\n");

  /*
   * PANKAJ: Once the packet has been transmitted and the interrupt
   * acknowledged the SAR driver is free to take another packet for
   * transmission.
   */
  if (skb->dev != NULL)
    netif_wake_queue (skb->dev);

#ifdef TIATM_INST_SUPP
  psp_trace_par (ATM_DRV_PKT_TX_COMPLETE_EXIT, skb->len);
#endif

  dgprintf (4, "tn7atm_send_complete() done\n");

  return 0;
}

void *tn7atm_allocate_rx_skb (void *os_dev, void **os_receive_info,
                              unsigned int size, void *vcc)
{
  struct sk_buff *skb;
  struct atm_vcc *vcc1;

  dgprintf (4, "tn7atm_allocate_rx_skb size=%d\n", size);
  size = ((size + 3) & 0xfffffffc);

#ifdef TI_STATIC_ALLOCATIONS
  skb = ti_alloc_skb (size, GFP_ATOMIC);
#else
  skb = dev_alloc_skb (size);
#endif

  if (skb == NULL)
  {
    dgprintf (4, "rx allocate skb failed\n");
    return NULL;
  }
  *os_receive_info = (void *) skb;

  vcc1 = (struct atm_vcc *) vcc;
  ATM_SKB (skb)->vcc = vcc1;

  if (vcc1 != NULL)
  {
    if (vcc1->qos.txtp.icr == 1)        // hack to have this field indicating
                                        //
      // it is pppoa channel.
    {
      skb_reserve (skb, 2);     // reserve 2 bytes for pppoa no copy
    }
  }

 /*** VRB: Experimenting with invalidation ***/
  /*
   * tn7sar_data_invalidate(skb->data, (int) size);
   */
  return (skb->data);
}

void tn7atm_free_rx_skb (void *skb, void *pmem)
{
  dgprintf (4, "tn7atm_free_rx_skb\n");
  tn7atm_kfree_skb ((struct sk_buff *) skb);
}


int tn7atm_receive (void *os_dev, int ch, unsigned int packet_size,
                    void *os_receive_info, void *data)
{
  Tn7AtmPrivate *priv;
  struct atm_dev *dev;
  struct sk_buff *skb;
  struct atm_vcc *vcc;
  static unsigned int ledticks;

  dgprintf (4, "tn7atm_receive()\n");

#ifdef TIATM_INST_SUPP
  psp_trace_par (ATM_DRV_PKT_RX_ENTER, packet_size);
#endif

  dev = (struct atm_dev *) os_dev;

  priv = (Tn7AtmPrivate *) dev->dev_data;

  if (priv->lut[ch].ready == 0)
  {
    printk ("%s: channel not ready %d\n", __FUNCTION__, ch);
    priv->stats.rx_errors++; //UR8_MERGE_START_END CQ10700 Manjula K
    return 1;
  }

  vcc = (struct atm_vcc *) priv->lut[ch].vcc;
  if (vcc == NULL)
  {
    printk ("vcc=Null");
    priv->stats.rx_errors++; //UR8_MERGE_START_END CQ10700 Manjula K
#ifdef TIATM_INST_SUPP
    psp_trace_2_par (ATM_DRV_PKT_RX_EXIT, -1, -1);
#endif
    return 1;
  }

  /*
   * assume no fragment packet for now
   */
  skb = (struct sk_buff *) os_receive_info;

  if (skb == NULL)
  {
    dgprintf (1, "received empty skb.\n");
    priv->stats.rx_errors++; //UR8_MERGE_START_END CQ10700 Manjula K
#ifdef TIATM_INST_SUPP
    psp_trace_2_par (ATM_DRV_PKT_RX_EXIT, -1, -2);
#endif
    return 1;
  }
  /*
   * see skbuff->cb definition in include/linux/skbuff.h
   */
  ATM_SKB (skb)->vcc = vcc;

  skb->len = packet_size;

  dgprintf (3, "skb:[0x%p]:0x%x pdu_len: 0x%04x\n", skb, skb->len,
            packet_size);
  dgprintf (3, "data location: 0x%x, 0x%x\n", (unsigned int) skb->data,
            (unsigned int) data);

  /*
   * skb_trim(skb,skb->len);
   */
  /*
   * skb size is incorrect for large packets > 1428 bytes ??
   */
  __skb_trim (skb, skb->len);   /* change to correct > 1500 ping when
                                 * firewall is on */

  dgprintf (3, "pushing the skb...\n");

  skb->stamp = vcc->timestamp = xtime;

  xdump ((unsigned char *) skb->data, skb->len, 5);

#ifndef __NO__VOICE_PATCH__
  /*
   * PANKAJ: Debugging voice-data coexistence.... Pass the packet to the
   * upper layers registered protocol inspector for prioritizing the received
   * packet.
   */
  if (vcc->protocol_inspector != NULL)
  {
    /*
     * Did the protocol inspector pass the packet.
     */
    if (vcc->protocol_inspector (skb) == 0)
    {
      /*
       * NO. The packet was supposed to be dropped. Eat the packet and return
       * SUCCESS..
       */
      kfree_skb (skb);
#ifdef TIATM_INST_SUPP
      psp_trace_2_par (ATM_DRV_PKT_RX_EXIT, 0, 0);
#endif
      return 0;
    }
  }
#endif

  if (atm_charge (vcc, skb->truesize) == 0)
  {
    dgprintf (1, "Receive buffers saturated for %d.%d.%d - PDU dropped\n",
              vcc->itf, vcc->vci, vcc->vpi);
    priv->stats.rx_dropped++; //UR8_MERGE_START_END CQ10700 Manjula K
#ifdef TIATM_INST_SUPP
    psp_trace_2_par (ATM_DRV_PKT_RX_EXIT, -1, -3);
#endif
    return 1;
  }

  /*
   * pass it up to kernel networking layer and update stats
   */
  vcc->push (vcc, skb);

  /*
   * Update receive packet stats
   */
  priv->stats.rx_packets++;
  atomic_inc (&vcc->stats->rx);

#if defined (CONFIG_LED_MODULE) || defined (CONFIG_MIPS_AVALANCHE_COLORED_LED)
  if ((ledticks != jiffies) && (hnd_LED_0))
  {
    ledticks = jiffies;
    TN7DSL_LED_ACTION(hnd_LED_0, MOD_ADSL, DEF_ADSL_ACTIVITY);
  }
#endif

  dgprintf (3,
            "(a) Receiving:vpi/vci[%d/%d] chan_id: %d  skb len:0x%x  skb truesize:0x%x\n",
            vcc->vpi, vcc->vci, ch, skb->len, skb->truesize);

#ifdef TIATM_INST_SUPP
  psp_trace_2_par (ATM_DRV_PKT_RX_EXIT, 0, skb->truesize);
#endif

  return 0;
}

static int tn7atm_proc_channels (char *buf, char **start, off_t offset,
                                 int count, int *eof, void *data)
{
  int len = 0;
  int limit = count - 80;
  int i;

  struct atm_dev *dev;
  Tn7AtmPrivate *priv;

  dev = (struct atm_dev *) data;
  priv = (Tn7AtmPrivate *) dev->dev_data;

  if (len <= limit)
    len += sprintf (buf + len, "Chan  Inuse   ChanID   VPI     VCI \n");
  if (len <= limit)
    len +=
      sprintf (buf + len,
               "------------------------------------------------------------------\n");

  for (i = 0; i <= MAX_DMA_CHAN; i++)
  {
    if (len <= limit)
    {
      len += sprintf (buf + len,
                      " %02d    %05d   %05d   %05d   %05d \n",
                      i, priv->lut[i].inuse, priv->lut[i].chanid,
                      priv->lut[i].vpi, priv->lut[i].vci);
    }
  }

  if (len <= limit)
    len +=
      sprintf (buf + len,
               "------------------------------------------------------------------\n");

  return len;
}

static int tn7atm_proc_private (char *buf, char **start, off_t offset,
                                int count, int *eof, void *data)
{
  int len = 0;
  int limit = count - 80;
  struct atm_dev *dev;
  Tn7AtmPrivate *priv;

  dev = (struct atm_dev *) data;
  priv = (Tn7AtmPrivate *) dev->dev_data;

  if (len <= limit)
    len += sprintf (buf + len, "\nPrivate Data Structure(%s):\n", priv->name);
  if (len <= limit)
    len += sprintf (buf + len, "----------------------------------------\n");
  if (len <= limit)
    len += sprintf (buf + len, "priv:  0x%p\n", priv);
  if (len <= limit)
    len += sprintf (buf + len, "next:  0x%p", priv->next);
  if (len <= limit)
    len += sprintf (buf + len, "\tdev:   0x%p\n", priv->dev);

  if (len <= limit)
    len += sprintf (buf + len, "tx_irq: %02d", priv->sar_irq);
  if (len <= limit)
    len += sprintf (buf + len, "rx_irq: %02d", priv->dsl_irq);

  return len;
}

void tn7atm_sarhal_isr_register (void *os_dev, void *hal_isr,
                                 int interrupt_num)
{
  struct atm_dev *dev;
  Tn7AtmPrivate *priv;

  dgprintf (4, "tn7atm_sarhal_isr_register()\n");

  dev = (struct atm_dev *) os_dev;
  priv = (Tn7AtmPrivate *) dev->dev_data;
  priv->halIsr = (void *) hal_isr;
  priv->int_num = interrupt_num;


}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_exit(void)
 *
 *  Description: Avalanche SAR exit function
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void tn7atm_exit (void)
{

  struct atm_dev *dev;

  Tn7AtmPrivate *priv;
  int ctr;

  dgprintf (4, "tn7atm_exit()\n");

  dev = mydev;
  priv = (Tn7AtmPrivate *) dev->dev_data;
  priv->lConnected = 0;

/***** VRB ****/
#ifdef CPATM_TASKLET_MODE
  tasklet_kill (&tn7atm_tasklet);
#endif
/******    ****/
  tn7dsl_exit ();

  tn7sar_exit (dev, priv);

  /*
   * freeup irq's
   */
  free_irq (priv->dsl_irq, priv->dev);
  free_irq (priv->sar_irq, priv->dev);

  kfree (dev->dev_data);

  // atm_dev_deregister (dev);
  shutdown_atm_dev (dev);

  /*
   * remove proc entries
   *
   * AV: Clean-up. Remove all of them from the data structure.
   */
  for (ctr = 0; ctr < (NUM_ELEMS (proc_if)); ctr++)
  {
    remove_proc_entry (proc_if[ctr].name, root_proc_dir_entry);
  }

  /*
   * Remove the root folder only if we created it.
   */
  if (!proc_root_already_exists)
    remove_proc_entry (drv_proc_root_folder, NULL);

  tn7dsl_dslmod_sysctl_unregister ();

  printk ("AVSAR: Module Removed\n");

}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_registration(struct tn7* tn7)
 *
 *  Description: ATM driver registration
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static int __init tn7atm_register (Tn7AtmPrivate * priv)
{
  /*
   * allocate memory for the device
   */

  dgprintf (4, "device %s being registered\n", priv->name);

  mydev = atm_dev_register (priv->proc_name, &tn7atm_ops, -1, NULL);

  if (mydev == NULL)
  {
    printk ("atm_dev_register returning NULL\n");
    return ATM_REG_FAILED;
  }

  printk ("registered device %s\n", priv->name);

  mydev->dev_data = priv;       /* setup device data in atm_dev struct */
  priv->dev = mydev;            /* setup atm_device in avalanche sar struct */

  mydev->ci_range.vpi_bits = ATM_CI_MAX;        /* atm supports 11 bits */
  mydev->ci_range.vci_bits = 16;        /* atm VCI max = 16 bits */


  return ATM_REG_OK;
}

static int tn7atm_proc_version (char *buf, char **start, off_t offset,
                                int count, int *eof, void *data)
{
  int len = 0;
  char dslVer[8];
  char dspVer[10];
  char chipsetID[32]; //CT CQ10076 - Added temporary buffer to store chipset Id
  char *pSarVer;
  Tn7AtmPrivate *priv;
  int i;
  unsigned int pdspV1, pdspV2;

  priv = mydev->dev_data;

  len +=
    sprintf (buf + len, "ATM Driver version:[%d.%02d.%02d.%02d]\n",
             LINUXATM_VERSION_MAJOR, LINUXATM_VERSION_MINOR,
             LINUXATM_VERSION_BUGFIX, LINUXATM_VERSION_BUILDNUM);

  tn7dsl_get_dslhal_version (dslVer);

  len +=
    sprintf (buf + len, "DSL HAL version: [%d.%02d.%02d.%02d]\n", dslVer[0],
             dslVer[1], dslVer[2], dslVer[3]);
  tn7dsl_get_dsp_version (dspVer);

  len +=
    sprintf (buf + len, "DSP Datapump version: [%d.%02d.%02d.%02d] ",
             dspVer[4], dspVer[5], dspVer[6], dspVer[7]);
  if (dspVer[8] == 2)           // annex B
    len += sprintf (buf + len, "Annex B\n");
  else if (dspVer[8] == 3)      // annex c
    len += sprintf (buf + len, "Annex c\n");
  else
    len += sprintf (buf + len, "Annex A\n");

  tn7sar_get_sar_version (priv, &pSarVer);

  len += sprintf (buf + len, "SAR HAL version: [");
  for (i = 0; i < 8; i++)
  {
    len += sprintf (buf + len, "%c", pSarVer[i + 7]);
  }
  len += sprintf (buf + len, "]\n");

  tn7sar_get_sar_firmware_version (&pdspV1, &pdspV2);
  len += sprintf (buf + len, "PDSP Firmware version:[%01x.%02x]\n",
                  pdspV1, pdspV2);

  //CT CQ10076 - Added code to report chipset ID using proc file system
  tn7atm_get_chipsetId(chipsetID);
  len += sprintf (buf + len, "Chipset ID: [%s]\n",chipsetID);

  return len;
}


/* Device detection */

static int __init tn7atm_detect (void)
{
  Tn7AtmPrivate *priv;
  struct proc_dir_entry *dsl_wr_file = NULL; /* Only for ones with a write
                                                 * function. */
  int ctr;
  const char *residual;

  dgprintf (4, "tn7atm_detect().\n");
  /*
   * Device allocated as a global static structure at top of code "mydev"
   */

  /*
   * Alloc priv struct
   */
  priv = kmalloc (sizeof (Tn7AtmPrivate), GFP_KERNEL);
  if (!priv)
  {
    printk ("unable to kmalloc priv structure. Killing autoprobe.\n");
    return -ENODEV;
  }
  memset (priv, 0, sizeof (Tn7AtmPrivate));
  priv->name = "TI Avalanche SAR";
  priv->proc_name = "avsar";

  if ((tn7atm_register (priv)) == ATM_REG_FAILED)
    return -ENODEV;

  if (tn7atm_init (mydev))
  {
    printk ("Error : Failed to Initialize the DSL subsystem !!.\n");
    return -ENODEV;
  }

  /*
   * Set up proc entry for atm stats
   */
  if (tn7atm_xlate_proc_name
      (drv_proc_root_folder, &root_proc_dir_entry, &residual))
  {
    printk ("Creating new root folder %s in the proc for the driver stats \n",
            drv_proc_root_folder);
    root_proc_dir_entry = proc_mkdir (drv_proc_root_folder, NULL);
    if (!root_proc_dir_entry)
    {
      printk ("Error : Failed to Initialize the proc subsystem !!.\n");
      return -ENOMEM;
    }
    proc_root_already_exists = FALSE;
  }

  /*
   * AV: Clean-up. Moved all the definitions to the data structure.
   */
  for (ctr = 0; ctr < (NUM_ELEMS (proc_if)); ctr++)
  {
      /* Only if we have a write function, we create a normal proc file. */
      if(proc_if[ctr].write_func)
      {
          dsl_wr_file = create_proc_entry (proc_if[ctr].name, DRV_PROC_MODE, root_proc_dir_entry);
          if (dsl_wr_file)
          {
            dsl_wr_file->read_proc  = proc_if[ctr].read_func;
            dsl_wr_file->write_proc = proc_if[ctr].write_func;
            dsl_wr_file->data = (void *)mydev; //UR8_MERGE_START_END CQ10700 Manjula K
          }
          dsl_wr_file = NULL;
      }
      else
      {
          /* Create a read-only entry. */
          create_proc_read_entry (proc_if[ctr].name, 0, root_proc_dir_entry,
                             proc_if[ctr].read_func, mydev);
      }
  }

  tn7dsl_dslmod_sysctl_register ();

  printk ("Texas Instruments ATM driver: version:[%d.%02d.%02d.%02d]\n",
          LINUXATM_VERSION_MAJOR, LINUXATM_VERSION_MINOR,
          LINUXATM_VERSION_BUGFIX, LINUXATM_VERSION_BUILDNUM);
  return 0;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_probe(void)
 *
 *  Description: Avalanche SAR driver probe (see net/atm/pvc.c)
 *               this is utilized when the SAR driver is built
 *               into the kernel and needs to be configured.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int __init tn7atm_probe (void)
{
  tn7atm_detect ();
  return -ENODEV;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: static int tn7atm_autoDetectDspBoost(void)
 *
 *  Description: Detect whether Dsp frequency should be boosted or not
 *
 *  Input:   none
 *  Output:  none
 *  Return:
 *
 *       0   -- do not boost
 *       1   -- Boost
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static int tn7atm_autoDetectDspBoost (void)
{
  char *cp;
  unsigned int dspfreq;
  unsigned int reg_cvr;
  unsigned int reg_DIDR1;
  unsigned int reg_DIDR2;
//  unsigned int  reg_codec_CTRL2;
  unsigned int PG_num;
  unsigned int chip_id;
  unsigned int timecode;
  unsigned int TrimRegVal;
  int boostDsp = 0;
  int dsp_noboost = 0;
//UR8_MERGE_START CQ10450   Jack Zhang
  Tn7AtmPrivate *priv = (tn7atm_private_t *)mydev->dev_data;
//UR8_MERGE_END   CQ10450*

  cp = prom_getenv ("dsp_noboost");
  if (cp)
  {
    dsp_noboost = os_atoi (cp);
  }

  cp = (char *) prom_getenv ("dsp_freq");
  if (cp)
  {
    dspfreq = os_atoi (cp);
    if (dspfreq == 250)
    {
      boostDsp = 1;
    }
  }
  else if (!dsp_noboost)        /* make sure dsp_noboost flag was not set
                                 * before boost */
  {
    /*
     * auto detect whether boost dsp clock or not
     */
    reg_cvr = DSLHAL_REG32 (REG_CVR);
    PG_num = (reg_cvr >> 16) & 0xFF;
    reg_DIDR1 = DSLHAL_REG32 (REG_DIDR1);
    reg_DIDR2 = DSLHAL_REG32 (REG_DIDR2);

//UR8_MERGE_START CQ10450   Jack Zhang
  //  chip_id = reg_cvr & 0xFFFF;
  chip_id = priv->chip_id;
//UR8_MERGE_END   CQ10450*

    switch (chip_id)
    {
    case CHIP_AR7:             /* 7300, 7300A or 7300C */
//      printk("Sangam detected\n");

      if (!(reg_DIDR2 & BIT23))
      {                         /* 7300 or 7300A */
        if ((PG_num != 0x57) && (PG_num >= 0x23))
        {
          /*
           * Rev 2.3 or larger but not 5.7, can boost
           */
          boostDsp = 1;
          tn7atm_set_can_support_adsl2 (TRUE);
        }
        else
        {
          /*
           * timecode gereatre than 4208000 can boost
           */
          timecode =
            ((reg_DIDR2 & 0x1FFF) << 10) | ((reg_DIDR1 & 0xFFC00000) >> 22);
          dgprintf (4, "timecode=%d\n", timecode);
          if (timecode > 4208000)
            boostDsp = 1;
        }
        tn7atm_set_can_support_adsl2 (TRUE);
      }
      else
      {
        printk ("7300C detected\n");    /* else 7300C: don't boost */
        tn7atm_set_can_support_adsl2 (FALSE);
      }
      break;

    case CHIP_AR7O_212:        /* Ohio212: don't boost */
      printk ("Ohio212 detected\n");
      tn7atm_set_can_support_adsl2 (FALSE);
      break;

    case CHIP_AR7O_250_212:    /* Ohio250 or Ohio212(new) */
      /*
       * Check Buck Trim bit see if it's Ohio250 or Ohio 212
       */
      DSLHAL_REG32 (REG_PRCR) &= ~PRCR_DSP;     /* put DSP in reset */
      DSLHAL_REG32 (REG_PRCR) |= (PRCR_ADSLSS); /* reset ADSLSS */
      DSLHAL_REG32 (REG_ADSLSS_PRCR) |= ADSLSS_PRCR_DSL_SPA;    /* reset
                                                                 * ADSLSS
                                                                 * DSL_SPA */
      mdelay (10);              /* Sanjay : Delay the read from the register
                                 * as sometimes this causes incorrect value
                                 * to be read from the register */
      TrimRegVal = (DSLHAL_REG32 (REG_BUCKTRIM_READ) >> 8) & 0x07;
      if (TrimRegVal == 0x07)
      {
        printk ("Ohio250(7200/7100A2) detected\n");
        boostDsp = 1;
        tn7atm_set_can_support_adsl2 (TRUE);
      }
      else
      {
        if (TrimRegVal==0x06)
        {
          boostDsp = 1;
          printk ("Ohio250(7100A1) detected\n");
        }
        else
          printk ("Ohio212(7100Trim=%d) detected\n", TrimRegVal);

        tn7atm_set_can_support_adsl2 (FALSE);
      }
      break;

    default:
      dgprintf (4, "Unknown chip id, cvr=%08x\n", reg_cvr);
      tn7atm_set_can_support_adsl2 (FALSE);
      break;
    }                           /* switch */
  }

  if (boostDsp)
    dgprintf (4, "set dspfreq 250Mhz\n");

  return (boostDsp);
}

//CT CQ10076 - Added function to get chipset Id

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: static int tn7atm_get_chipsetId(char *pVerId)
 *
 *  Description: Detect whether Dsp frequency should be boosted or not
 *
 *  Input:   char * buffer to populate with chipset ID string
 *                  max 31 characters
 *  Output:  none
 *  Return:  none
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void tn7atm_get_chipsetId (char *pVerId)
{
  unsigned int reg_DIDR2;
  unsigned int chip_id;
  unsigned int TrimRegVal;
  
//UR8_MERGE_START CQ10450   Jack Zhang
  Tn7AtmPrivate *priv = (tn7atm_private_t *)mydev->dev_data;
//UR8_MERGE_END   CQ10450*

  /*
   * get chipset hw info
   */
//UR8_MERGE_START CQ10450   Jack Zhang
  //chip_id = (DSLHAL_REG32 (REG_CVR)) & 0xFFFF;
  chip_id = priv->chip_id;
//UR8_MERGE_END   CQ10450*
  reg_DIDR2 = DSLHAL_REG32 (REG_DIDR2);

  switch (chip_id)
  {
  case CHIP_AR7:             /* 7300, 7300A or 7300C */

    if (!(reg_DIDR2 & BIT23))
      sprintf(pVerId, "7300/7300A");
    else
      sprintf(pVerId, "7300C");
    break;

  case CHIP_AR7O_212:        /* Ohio212 */
    sprintf(pVerId, "Ohio212");
    break;

  case CHIP_AR7O_250_212:    /* Ohio250 or Ohio212(new) */
    /*
     * Check Buck Trim bit see if it's Ohio250 or Ohio 212
     */
    TrimRegVal = (DSLHAL_REG32 (REG_BUCKTRIM_READ) >> 8) & 0x07;
    if (TrimRegVal == 0x07)
    {
      sprintf(pVerId, "Ohio250(7200/7100A2)");
    }
    else
    {
      if (TrimRegVal==0x06)
        sprintf(pVerId, "Ohio250(7100A1)");
      else
        sprintf(pVerId, "Ohio212(7100Trim=%01d)", TrimRegVal);
    }
    break;

  default:
    sprintf(pVerId, "Unknown");
    break;
  }                           /* switch */

  return;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Function: int tn7atm_init(struct atm_dev *dev)
 *
 *  Description: Device Initialization
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static int __init tn7atm_init (struct atm_dev *dev)
{
  Tn7AtmPrivate *priv;
  unsigned int chip_id;
  extern unsigned int _dsl_Feature_0;
  extern unsigned int _dsl_Feature_1;
  extern unsigned int _dsl_Feature_0_defined;
  extern unsigned int _dsl_Feature_1_defined;
  extern unsigned int _dsl_PhyControl_0;  //CQ10280
  extern unsigned int _dsl_PhyControl_1;
  extern unsigned int _dsl_PhyControl_0_defined;
  extern unsigned int _dsl_PhyControl_1_defined;
  char *ptr;

  dgprintf (4, "tn7atm_init()\n");
  priv = (Tn7AtmPrivate *) dev->dev_data;

  chip_id = DSLHAL_REG32 (REG_CVR) & 0xFFFF;
//UR8_MERGE_START CQ10450   Jack Zhang
  priv->chip_id = chip_id;
//UR8_MERGE_END   CQ10450*
  switch (chip_id & 0xFFFF)
  {
  case CHIP_AR7:               /* Sangam */
    printk ("Sangam detected\n");
    ATM_DSL_INT = ATM_DSL_INT_SANGAM;
    def_sar_inter_pace = SANGAM_DEFAULT_IPACEMAX_VAL;   /* MR 2344 */
    SAR_FREQUNCY = 50000000;    /* 200Mhz/4 */
    break;

  case CHIP_AR7O_212:          /* Ohio212 */
  case CHIP_AR7O_250_212:      /* Ohio250 */
    ATM_DSL_INT = ATM_DSL_INT_OHIO;
    def_sar_inter_pace = OHIO_DEFAULT_IPACEMAX_VAL;     /* MR 2344 */
    SAR_FREQUNCY = 53000000;    /* 212Mhz/4 */
    break;

  default:
    return -ENODEV;             /* Unknown chip */
  };

  if (tn7atm_autoDetectDspBoost ())
  {
    if(!dslhal_api_boostDspFrequency ())
    {
        /* Boosted the SAR frequency as the boost was successful. */
    SAR_FREQUNCY = 62500000;    /* 250Mhz/4 */
    }
  }

  if (tn7sar_init (dev, priv) != 0)
  {
    printk ("Failed to init SAR.\n");
    return -ENODEV;
  }

  // Inter-Op DSL phy Control
  // Note the setting of _dsl_Feature_0 and _dsl_Feature_1 must before
  // dslhal_api_dslStartup (in tn7dsl_init()).
  if ((ptr = prom_getenv ("DSL_FEATURE_CNTL_0")) != NULL)
  {
    if ((ptr[0] == '0') && (ptr[1] == 'x'))     // skip 0x before pass to
      // os_atoh
      ptr += 2;
    _dsl_Feature_0 = os_atoh (ptr);
    _dsl_Feature_0_defined = 1;
  }

  if ((ptr = prom_getenv ("DSL_FEATURE_CNTL_1")) != NULL)
  {
    if ((ptr[0] == '0') && (ptr[1] == 'x'))     // skip 0x before pass to
      // os_atoh
      ptr += 2;
    _dsl_Feature_1 = os_atoh (ptr);
    _dsl_Feature_1_defined = 1;
  }

//CQ10280
  // DSL phy Feature Control
  // Note the setting of _dsl_PhyControl_0 and _dsl_PhyControl_1 must before
  // dslhal_api_dslStartup (in tn7dsl_init()).
  if ((ptr = prom_getenv ("DSL_PHY_CNTL_0")) != NULL)
  {
    if ((ptr[0] == '0') && (ptr[1] == 'x'))     // skip 0x before pass to
      // os_atoh
      ptr += 2;
    _dsl_PhyControl_0 = os_atoh (ptr);
    _dsl_PhyControl_0_defined = 1;
  }

  if ((ptr = prom_getenv ("DSL_PHY_CNTL_1")) != NULL)
  {
    if ((ptr[0] == '0') && (ptr[1] == 'x'))     // skip 0x before pass to
      // os_atoh
      ptr += 2;
    _dsl_PhyControl_1 = os_atoh (ptr);
    _dsl_PhyControl_1_defined = 1;
  }


  if (tn7dsl_init (priv) < 0)
  {
    printk ("Failed to init DSL.\n");
    return -ENODEV;
  }

//cph99
//printk("ptr=%s\n", ptr);


  if (tn7atm_get_ESI (dev) < 0) /* set ESI */
    return -ENODEV;

  if (tn7atm_irq_request (dev) < 0)
    return -EBUSY;

  priv->bTurboDsl = 1;
  // read config for turbo dsl
  ptr = prom_getenv ("TurboDSL");
  if (ptr)
  {
    priv->bTurboDsl = os_atoi (ptr);
  }

  // @Added to make Rx buffer number & Service max configurable through
  // environment variable.
  priv->sarRxBuf = RX_BUFFER_NUM;
  ptr = NULL;
  ptr = prom_getenv ("SarRxBuf");
  if (ptr)
  {
    priv->sarRxBuf = os_atoi (ptr);
  }
  priv->sarRxMax = RX_SERVICE_MAX;
  ptr = NULL;
  ptr = prom_getenv ("SarRxMax");
  if (ptr)
  {
    priv->sarRxMax = os_atoi (ptr);
  }
  priv->sarTxBuf = TX_BUFFER_NUM;
  ptr = NULL;
  ptr = prom_getenv ("SarTxBuf");
  if (ptr)
  {
    priv->sarTxBuf = os_atoi (ptr);
  }
  priv->sarTxMax = TX_SERVICE_MAX;
  ptr = NULL;
  ptr = prom_getenv ("SarTxMax");
  if (ptr)
  {
    priv->sarTxMax = os_atoi (ptr);
  }

  return 0;
}

int tn7atm_device_connect_status (void *priv, int state)
{
  Tn7AtmPrivate *priv1;

  dgprintf (5, "tn7atm_device_connect_status()\n");
  priv1 = (Tn7AtmPrivate *) priv;

  priv1->lConnected = state;
  dgprintf (5, "priv1->lConnected=%d\n", priv1->lConnected);
  return 0;
}

#ifdef CPATM_TASKLET_MODE
void tn7atm_handle_tasklet (unsigned long data)
{
  struct atm_dev *atmdev = (struct atm_dev *) data;
  Tn7AtmPrivate *priv;
  int more = 0;

  dgprintf (6, "tn7atm_sar_irq\n");
  priv = (Tn7AtmPrivate *) atmdev->dev_data;

  tn7sar_handle_interrupt (atmdev, priv, &more);

  if (more)
  {
    tasklet_schedule (&tn7atm_tasklet);
  }
}
#endif

/*
* This function returns a boolean value to indicate whether the chip supports ADSL2 and ADSL2+.
*/
int tn7atm_read_can_support_adsl2 (void)
{
  return (CanSuppADSL2);
}

/*
* This function sets a boolean value to indicate whether the chip supports ADSL2 and ADSL2+.
*/
static int tn7atm_set_can_support_adsl2 (int can)
{
  if ((!can == FALSE) || (can == TRUE))
    CanSuppADSL2 = can;
  else
    return FALSE;

  return TRUE;
}

/*
 * This function matches a name such as "serial", and that specified by the
 * proc_dir_entry
 */
static int tn7atm_proc_match (int len, const char *name,
                              struct proc_dir_entry *de)
{
  if (!de || !de->low_ino)
    return 0;
  if (de->namelen != len)
    return 0;
  return !strncmp (name, de->name, len);
}

/*
 * This function parses a name such as "tty/driver/serial", and
 * returns the struct proc_dir_entry for "/proc/tty/driver", and
 * returns "serial" in residual.
 */
static int tn7atm_xlate_proc_name (const char *name,
                                   struct proc_dir_entry **ret,
                                   const char **residual)
{
  const char *cp = name, *next;
  struct proc_dir_entry *de;
  int len;
  extern struct proc_dir_entry proc_root;

  de = &proc_root;
  while (1)
  {
    next = strchr (cp, '/');
    if (!next)
      break;

    len = next - cp;
    for (de = de->subdir; de; de = de->next)
    {
      if (tn7atm_proc_match (len, cp, de))
        break;
    }
    if (!de)
      return -ENOENT;
    cp += len + 1;
  }
  *residual = cp;
  *ret = de;

  return 0;
}

static int tn7atm_proc_qos_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf (buf + len, "\nEnableQoS = %d\n", EnableQoS);
    return len;

}
static int tn7atm_proc_qos_write(struct file *fp, const char *buf, unsigned long count, void *data)
{
    char local_buf[32];

    if(count > 30)
    {
        printk("Error : Buffer Overflow\n");
        printk("Use \"echo 1> avsar_qos_enable \" to enable the QoS \n");
        return -1;
     }

    copy_from_user(local_buf,buf,count);

    if(local_buf[count - 1] =='\n')
        local_buf[count - 1] ='\0'; /* Ignoring last \n char */


     if(!strcmp("0",local_buf))
        EnableQoS = FALSE;
     else if(!strcmp("1",local_buf))
        EnableQoS = TRUE;
    else
    {
            printk("\nError: Unknown Command sent.\n");
            printk("Use \"echo 1 > avsar_qos_enable\" to enable the QoS\n");
            printk("\nOR\nUse \"echo 0 > avsar_qos_enable\" to disable the QoS\n");
            return -1;
    }
    return count;
}

#ifdef MODULE
module_init (tn7atm_detect);
module_exit (tn7atm_exit);
#endif /* MODULE */
