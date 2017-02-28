/* 
 *   tn7.c
 *   Linux atm module implementation.
 *   Zhicheng Tang 01/08/2003
 *   2003 (c) Texas Instruments Inc.
 *
 *
*/

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
#include "tn7atm.h"
#include "tn7api.h"
#include "syssw_version.h"

#ifdef CONFIG_LED_MODULE
#include <asm/ar7/ledapp.h>
#endif
#include <asm/ar7/avalanche_intc.h>

#ifdef MODULE
MODULE_DESCRIPTION ("Tnetd73xx ATM Device Driver");
MODULE_AUTHOR ("Zhicheng Tang");
#endif

/* Version Information */
//static char atm_version[] ="1.0.0.1";

#define TRUE   1
#define FALSE  0

#define STOP_EMPTY_BUFF 2
#define START_EMPTY_BUFF 3
/* externs */

/*end of externs */

#define tn7atm_kfree_skb(x)     dev_kfree_skb(x)

/* prototypes */
int tn7atm_open (struct atm_vcc *vcc, short vpi, int vci);

void tn7atm_close (struct atm_vcc *vcc);

static int tn7atm_ioctl (struct atm_dev *dev, unsigned int cmd, void *arg);

int tn7atm_send (struct atm_vcc *vcc, struct sk_buff *skb);

static int tn7atm_change_qos (struct atm_vcc *vcc, struct atm_qos *qos,int flags);

static int tn7atm_detect(void);
static int tn7atm_init(struct atm_dev* dev);
//static int tn7atm_reset(void);
static int tn7atm_irq_request(struct atm_dev* dev);
static int tn7atm_proc_version(char* buf, char **start, off_t offset, int count,int *eof, void *data);
static void tn7atm_exit(void);
static int tn7atm_proc_channels(char* buf, char **start, off_t offset, int count,int *eof, void *data);
static int tn7atm_proc_private(char* buf, char **start, off_t offset, int count,int *eof,void *data);
//static void tn7atm_free_packet(void *vcc1, void *priv, void *skb1);
static int tn7atm_queue_packet_to_sar(void *vcc1, void *skb1);

#include "turbodsl.c"

/* ATM device operations */

struct atm_dev *mydev;

static const struct atmdev_ops tn7atm_ops = {
        open:           tn7atm_open,
        close:          tn7atm_close,
        ioctl:          tn7atm_ioctl,
        getsockopt:     NULL,
        setsockopt:     NULL,
        send:           tn7atm_send,
        sg_send:          NULL,
        phy_put:          NULL,
        phy_get:          NULL,
        change_qos:     tn7atm_change_qos,
};


int __guDbgLevel = 1;


void xdump(unsigned char *buff, int len, int debugLev)
{
#ifdef DEBUG_BUILD
  int i, j;
  if( __guDbgLevel < debugLev)
    return;

  j=0;
  for(i=0;i<len;i++)
  {
    printk("%02x ", buff[i]);
    j++;
    if(j==8)
    {
      j=0;
      printk("\n");
    }
  }
  printk("\n");
#endif
}


#if 0 /* by nbd */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int tn7atm_walk_vccs(struct atm_dev *dev, short *vcc, int *vci)
 *
 *  Description: retrieve VPI/VCI for connection
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static int
tn7atm_walk_vccs(struct atm_vcc *vcc, short *vpi, int *vci)
{
  struct atm_vcc* walk;

  //    printk(tn7 "tn7atm_walk_vccs\n");
  /* find a free VPI */
  if (*vpi == ATM_VPI_ANY) {

    for (*vpi = 0, walk = vcc->dev->vccs; walk; walk = walk->next) {

      if ((walk->vci == *vci) && (walk->vpi == *vpi)) {
        (*vpi)++;
        walk = vcc->dev->vccs;
      }
    }
  }

  /* find a free VCI */
  if (*vci == ATM_VCI_ANY) {
        
    for (*vci = ATM_NOT_RSV_VCI, walk = vcc->dev->vccs; walk; walk = walk->next) {

      if ((walk->vpi = *vpi) && (walk->vci == *vci)) {
        *vci = walk->vci + 1;
        walk = vcc->dev->vccs;
      }
    }
  }

  return 0;
}
#endif


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int tn7atm_sar_irq(void)
 *
 *  Description: tnetd73xx SAR interrupt.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void
tn7atm_sar_irq(int irq , void *voiddev , struct pt_regs *regs)
{
  struct atm_dev *atmdev;
  Tn7AtmPrivate *priv;

  dprintf(6, "tn7atm_sar_irq\n");
  atmdev = (struct atm_dev *) voiddev;
  priv = (Tn7AtmPrivate *)atmdev->dev_data;

  tn7sar_handle_interrupt(atmdev, priv);
 
  dprintf(6, "Leaving tn7atm_sar_irq\n");
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int tn7atm_dsl_irq(void)
 *
 *  Description: tnetd73xx DSL interrupt.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void
tn7atm_dsl_irq(int irq , void *voiddev , struct pt_regs *regs)
{
  struct atm_dev *atmdev;
  Tn7AtmPrivate *priv;

  dprintf(4, "tn7atm_dsl_irq\n");
  atmdev = (struct atm_dev *) voiddev;
  priv = (Tn7AtmPrivate *)atmdev->dev_data;

  tn7dsl_handle_interrupt();
 
  dprintf(4, "Leaving tn7atm_dsl_irq\n");
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int tn7atm_Inittxcomp(struct tn7* tn7)
 *
 *  Description: Initialize Interrupt handler
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static int __init
tn7atm_irq_request (struct atm_dev *dev)
{
  Tn7AtmPrivate *priv;
  char *ptr;
  int ipace=2;

  dprintf(4, "tn7atm_irq_request()\n");
  priv = (Tn7AtmPrivate *) dev->dev_data;

  /* Register SAR interrupt */
  priv->sar_irq = LNXINTNUM(ATM_SAR_INT);       /* Interrupt line # */
  if (request_irq(priv->sar_irq, tn7atm_sar_irq, SA_INTERRUPT, "SAR ", dev))
    printk ("Could not register tn7atm_sar_irq\n");

  /* interrupt pacing */
  ptr= prom_getenv("sar_ipacemax");
  if(ptr)
  {
    ipace=os_atoi(ptr);
  }
  avalanche_request_pacing(priv->sar_irq, ATM_SAR_INT_PACING_BLOCK_NUM, ipace);

  /* Reigster Receive interrupt A */
  priv->dsl_irq = LNXINTNUM(ATM_DSL_INT);       /* Interrupt line # */
  if (request_irq(priv->dsl_irq, tn7atm_dsl_irq, SA_INTERRUPT, "DSL ", dev))
    printk ("Could not register tn7atm_dsl_irq\n");

  return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int tn7atm_lut_find(struct atm_vcc *vcc)
 *
 *  Description: find an TX DMA channel
 *               that matches a vpi/vci pair
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
int
tn7atm_lut_find(short vpi, int vci)
{
  int      i;
  Tn7AtmPrivate *priv;
  
  priv = (Tn7AtmPrivate *)mydev->dev_data;

  if(vci==0) // find first vpi channel
  {
    for(i=0; i< MAX_DMA_CHAN; i++)
    {  
      if((priv->lut[i].vpi == vpi))
      return i;
    }
  }

  dprintf(4, "vpi=%d, vci=%d\n", vpi, vci);
  for(i=0; i< MAX_DMA_CHAN; i++)
  {  
    if((priv->lut[i].vpi == vpi) && (priv->lut[i].vci == vci))
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
static int
tn7atm_lut_clear(struct atm_vcc *vcc, int chan)
{
  Tn7AtmPrivate *priv;
  
  priv = (Tn7AtmPrivate *)vcc->dev->dev_data;

  memset(&priv->lut[chan], 0, sizeof(priv->lut[chan]));

  return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int tn7atm_walk_lut(void)
 *
 *  Description: find an available TX DMA channel
 *               and initialize LUT
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static int 
tn7atm_walk_lut(Tn7AtmPrivate *priv)
{
  int i;

  for(i=0; i< MAX_DMA_CHAN; i++){ 
    if(!priv->lut[i].inuse)
    {    
      return i;                            /* return available dma channel number */
    }
  }
  return ATM_NO_DMA_CHAN;       /* no tx dma channels available */
}

static int 
tn7atm_set_lut(Tn7AtmPrivate *priv, struct atm_vcc *vcc, int chan)
{
  
  if(!priv->lut[chan].inuse)
  {
    priv->lut[chan].vpi = (int)vcc->vpi;
    priv->lut[chan].vci = vcc->vci;
    priv->lut[chan].chanid = chan;
    priv->lut[chan].inuse = 1;         /* claim the channel */
    priv->lut[chan].vcc = (void *)vcc;
    priv->lut[chan].bClosing = 0;
    priv->lut[chan].ready = 0;
    priv->lut[chan].tx_total_bufs = TX_BUFFER_NUM;
    priv->lut[chan].tx_used_bufs[0] = 0;
    priv->lut[chan].tx_used_bufs[1] = 0;
    return 0;
  }
  return -1;       /* no tx dma channels available */
}

#if 0
static void tn7atm_free_packet(void *pVc, void *pDev, void *pPacket)
  {
  Tn7AtmPrivate *priv;
  struct atm_vcc *vcc;
  struct sk_buff *skb;

  vcc = (struct atm_vcc *)pVc;
  priv = (Tn7AtmPrivate *)pDev;
  skb = (struct sk_buff *) pPacket;

  if(vcc->pop)
    vcc->pop(vcc, skb);   
  else
    tn7atm_kfree_skb(skb);
  }
#endif

static void str2eaddr(char *pMac, char *pStr)
{
  char tmp[3];
  int i;

  for(i=0;i<6;i++)
  {
    tmp[0]=pStr[i*3];
    tmp[1]=pStr[i*3+1];
    tmp[2]=0;
    pMac[i]=os_atoh(tmp);
  }
}

static int __init
tn7atm_get_ESI(struct atm_dev *dev)
{
  int  i;
  char  esi_addr[ESI_LEN]={0x00,0x00,0x11,0x22,0x33,0x44};
  char  *esiaddr_str = NULL;

  esiaddr_str = prom_getenv("macc");

  if (!esiaddr_str) {
    //printk("macc address not set in adam2 environment space\n");
    //printk("Using default macc address = 00:01:02:03:04:05\n");
    esiaddr_str = "00:00:02:03:04:05";
  }
  str2eaddr(esi_addr, esiaddr_str);

  for(i=0; i < ESI_LEN; i++)
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
//static int
int tn7atm_open (struct atm_vcc *vcc, short vpi, int vci)
{
  Tn7AtmPrivate *priv;
  int dmachan;
  int rc;
  int traffic_type;
  int pcr = 0x20000;
  int scr = 0x20000;
  int mbs = 0x20000;
  int cdvt = 10000;
  int err;

  dprintf(1, "tn7atm_open()\n");

  priv = (Tn7AtmPrivate *)vcc->dev->dev_data; 
  if(priv==NULL)
  {
    printk("null priv\n");
    return -1;
  }

  MOD_INC_USE_COUNT;

#if 0 /* by nbd */
  /* find a free VPI/VCI */
  tn7atm_walk_vccs(vcc, &vpi, &vci);
#else
  if ((err = atm_find_ci(vcc, &vpi, &vci))) {
    printk("atm_find_ci err = %d\n", err);
    return err;
  }
  
#endif
        
  vcc->vpi = vpi;
  vcc->vci = vci;

  if (vci == ATM_VCI_UNSPEC || vpi == ATM_VCI_UNSPEC)
  {
    MOD_DEC_USE_COUNT;
    return -EBUSY;
  }
  
  /* check to see whether PVC is opened or not */
  if((dmachan = tn7atm_lut_find(vcc->vpi, vcc->vci)) != ATM_NO_DMA_CHAN) 
  {
    MOD_DEC_USE_COUNT;  
    printk("PVC already opened. dmachan = %d\n", dmachan);
    return -EBUSY;
  }
  /*check for available channel */
  if((dmachan = tn7atm_walk_lut(priv)) == ATM_NO_DMA_CHAN)
  {
    printk("No TX DMA channels available\n");
    return -EBUSY;
  }

  set_bit(ATM_VF_ADDR, &vcc->flags);  /* claim address    */
        
  vcc->itf = vcc->dev->number;        /* interface number */

  switch(vcc->qos.txtp.traffic_class)
  {
    case ATM_CBR: /* Constant Bit Rate */
      traffic_type = 0;
      pcr = vcc->qos.txtp.pcr;
      scr = vcc->qos.txtp.pcr;
      cdvt = vcc->qos.txtp.max_cdv;
      printk("cdvt=%d\n", cdvt);
      break;
    case ATM_UBR: /* Unspecified Bit Rate */
      traffic_type = 2;
      break;

	/* Disable ATM_VBR until pppd ppoatm plugin supports it.
         * NOTE: Support ATM_VBR requires the addition of a scr
         * field to the atm_trafprm structure which will cause
	 * a change in the SO_ATMQOS ioctl. Make sure that the
	 * revised header file becomes visible to the pppd
	 * pppoatm plugin source, or the SO_ATMQOS ioctl will fail.
	 */
#if 0
    case ATM_VBR: /* Variable Bit Rate */
      traffic_type = 1;
      pcr = vcc->qos.txtp.pcr;
      scr = vcc->qos.txtp.scr;
      if(vcc->qos.txtp.max_pcr >= 0)
        mbs = vcc->qos.txtp.max_pcr;
      cdvt = vcc->qos.txtp.max_cdv;
      printk("cdvt=%d\n", cdvt);
      printk("mbs=%d\n", mbs);
      break;
#endif
    default:
        traffic_type = 2;
  }

  dprintf(4, "vpi=%d, vci=%d, pcr=%d, dmachan=%d, qos=%d\n", vpi,vci,pcr,dmachan,traffic_type);
  /* Activate SAR channel */
  rc = tn7sar_activate_vc(priv, vpi, vci, pcr, scr, mbs, cdvt, dmachan, traffic_type);
  if(rc < 0)
  {
    
    MOD_DEC_USE_COUNT;
    return -EBUSY;
  }
  
  /* insure that the the vcc struct points to the correct entry
       in the lookup table */

  tn7atm_set_lut(priv,vcc, dmachan);
  vcc->dev_data = (void *)&priv->lut[dmachan];
  set_bit(ATM_VF_READY, &vcc->flags);

  mdelay(100);
  priv->lut[dmachan].ready = 1;
  dprintf (1, "Leave tn7atm_open\n"); 
  return 0;     
}


//static void
void tn7atm_close (struct atm_vcc *vcc)
{
  Tn7AtmPrivate *priv;
  int dmachan;

  priv = (Tn7AtmPrivate *)vcc->dev->dev_data;
  dprintf(4, "closing %d.%d.%d.%d\n", vcc->itf, vcc->vpi, vcc->vci, vcc->qos.aal);
 
  clear_bit(ATM_VF_READY, &vcc->flags);  /* ATM_VF_READY: channel is ready to transfer data */

  dmachan = tn7atm_lut_find(vcc->vpi, vcc->vci);
  printk("closing channel: %d\n", dmachan);
  if(dmachan == ATM_NO_DMA_CHAN)
  {
    printk("Closing channel not found.\n");
    return;
  }
  priv->lut[dmachan].bClosing = 1;
  priv->lut[dmachan].ready = 0;
  if(tn7sar_deactivate_vc(priv,dmachan))  /* tear down channel */
  {
    printk("failed to close channel %d.\n", dmachan);
  }

  clear_bit(ATM_VF_READY, &vcc->flags);  /* ATM_VF_READY: channel is ready to transfer data */
  tn7atm_lut_clear(vcc, dmachan);

  MOD_DEC_USE_COUNT;

  dprintf (1, "Leave tn7atm_close\n");
}

#define ATM_TXSTOP 0x800c61f4 
static int
tn7atm_ioctl (struct atm_dev *dev, unsigned int cmd, void *arg)
{
  Tn7AtmPrivate *priv;
  priv = (Tn7AtmPrivate *) dev->dev_data;
  
    //printk("tn7atm_ioctl\n");
    //printk("arg = %x\n", *(int *)arg);
    //printk("cmd =%x\n", cmd);
    switch(cmd)
    {

      case ATM_TXSTOP: /*temp fix for SAR tear down problem */
//        printk("ioctl cmd = 0x%x (%u), arg = 0x%p (%lu)\n", cmd, cmd, arg, (unsigned long)arg);
//        printk("arg = %d\n", *(int*)arg);
        priv->xmitStop = *(int *)arg;
        //printk("Executing ATM_SETLOOP for tn7 \n");
        //printk("Stop variable = :%d: \n",priv->xmitStop);
        return 0;

      //case SAR_DSL_RESET_SOFTBOOT:
      //  return tn7atm_dsl_clean_reboot();
    case 0:
        return 0;
    }
    
    return -ENOSYS;
  
}

static int
tn7atm_change_qos (struct atm_vcc *vcc, struct atm_qos *qos,int flags)
{
  dprintf (1, "Enter tn7atm_change_qos\n");
  dprintf (1, "Leave tn7atm_change_qos\n");
  return 0;
}


int tn7atm_send (struct atm_vcc *vcc, struct sk_buff *skb)
{
  
  Tn7AtmPrivate *priv;
  int bret;
  int chan;
  
  dprintf(4, "tn7atm_send()\n");
  
  priv = (Tn7AtmPrivate*)vcc->dev->dev_data;

  //if(skb->len < 64)
    //xdump((unsigned char *)skb->data, skb->len, 1);
  //else
    //xdump((unsigned char *)skb->data, 64, 1);
  /* check for dsl line connection */

  /* add vcc field in skb for clip inATMARP fix */
  ATM_SKB(skb)->vcc = vcc;
	/* Ron change 2.3 -> 2.4 ??*/
  //if(priv->lConnected != 1 || priv->xmitStop == 1)
  if(priv->lConnected != 1 && priv->xmitStop == 1)
  {
    dprintf(4,"dsl line down\n");
    if(vcc->pop)
      vcc->pop(vcc, skb);   
    else
     tn7atm_kfree_skb(skb);
    return 1;
  }
  
  /* check whether PVC is closing */
  chan = tn7atm_lut_find(vcc->vpi, vcc->vci);
  /* set br2684 dev pointer */
  priv->lut[chan].net_device = skb->dev;
  if(chan == ATM_NO_DMA_CHAN || priv->lut[chan].bClosing == 1)
  {
    dprintf(4, "can find sar channel\n");
    if(vcc->pop)
      vcc->pop(vcc, skb);   
    else
     tn7atm_kfree_skb(skb);
    return 1;
  }

  bret=tn7atm_queue_packet_to_sar(vcc, skb);

  return bret;
}


static int tn7atm_queue_packet_to_sar(void *vcc1, void *skb1)
{
  struct atm_vcc *vcc;
  struct sk_buff *skb;
  int priority = 1;
  Tn7AtmPrivate *priv;
  int dmachan;

  vcc = (struct atm_vcc *)vcc1;
  skb = (struct sk_buff *)skb1;

  priv = (Tn7AtmPrivate*)vcc->dev->dev_data;

  dprintf(4, "vcc->vci=%d\n", vcc->vci);
  dmachan = tn7atm_lut_find(vcc->vpi, vcc->vci);
  if(dmachan == ATM_NO_DMA_CHAN)
  {
    dprintf(4, "can find sar channel\n");
    if(vcc->pop)
      vcc->pop(vcc, skb);   
    else
     tn7atm_kfree_skb(skb);
    return 1;
  }

  // turbo dsl TCP ack check
  if(priv->bTurboDsl)
    priority = turbodsl_check_priority_type(skb->data);

  //skb priority check
  if(priority != 0)
  {
    if((skb->cb[47])>>1)
      priority=1;
    else
      priority = 0;
  }
  
  /* add queue info here */
  skb->cb[47] = (char)priority;
  spin_lock_irqsave(&priv->netifqueueLock, priv->netifqueueLockFlag);
  priv->lut[dmachan].tx_used_bufs[priority]++;
  spin_unlock_irqrestore(&priv->netifqueueLock, priv->netifqueueLockFlag);

  if(tn7sar_send_packet(priv,dmachan, skb, skb->data, skb->len, priority) != 0)
  {
    dprintf(1, "failed to send packet\n");
    if(vcc->pop)
      vcc->pop(vcc, skb);   
    else
     tn7atm_kfree_skb(skb);

    spin_lock_irqsave(&priv->netifqueueLock, priv->netifqueueLockFlag);
    priv->lut[dmachan].tx_used_bufs[priority]--;
    spin_unlock_irqrestore(&priv->netifqueueLock, priv->netifqueueLockFlag);
    return 1;
  }
  
  /* check for whether tx queue is full or not */
  //printk("bufs used = %d\n", priv->lut[dmachan].tx_used_bufs[1]);
  spin_lock_irqsave(&priv->netifqueueLock, priv->netifqueueLockFlag);
  if(priv->lut[dmachan].tx_used_bufs[1] >= (priv->lut[dmachan].tx_total_bufs - STOP_EMPTY_BUFF) ||
      priv->lut[dmachan].tx_used_bufs[0] >= (priv->lut[dmachan].tx_total_bufs - STOP_EMPTY_BUFF))
  {
    //printk("net queue stoped\n");
    netif_stop_queue(priv->lut[dmachan].net_device);
    priv->lut[dmachan].netqueue_stop = 1;
  }
  spin_unlock_irqrestore(&priv->netifqueueLock, priv->netifqueueLockFlag);

  return 0;
}

/* functions needed by SAR HAL */

int tn7atm_send_complete(void *osSendInfo)
{
  Tn7AtmPrivate *priv;
  //struct atm_dev *dev;
  struct sk_buff *skb;
  struct atm_vcc *vcc;
  int chan;

  dprintf(4, "tn7atm_send_complete()\n");


  skb = (struct sk_buff *)osSendInfo;
  //dev = (struct atm_dev *) (skb->dev);
  priv = (Tn7AtmPrivate *)mydev->dev_data;
  vcc =ATM_SKB(skb)->vcc;
  if(vcc)
  {
    dprintf(4, "vcc->vci=%d\n",vcc->vci );
    chan = tn7atm_lut_find(vcc->vpi, vcc->vci);
    if(chan==ATM_NO_DMA_CHAN)
      return 1;

    /*decreament packet queued number */
    spin_lock_irqsave(&priv->netifqueueLock, priv->netifqueueLockFlag);
    priv->lut[chan].tx_used_bufs[(int)skb->cb[47]] --;
    if(priv->lut[chan].tx_used_bufs[1] < priv->lut[chan].tx_total_bufs - START_EMPTY_BUFF &&
       priv->lut[chan].tx_used_bufs[0] < priv->lut[chan].tx_total_bufs - START_EMPTY_BUFF)
    {
      if(priv->lut[chan].netqueue_stop)
      {
        //printk("net queue restarted\n");
        netif_wake_queue(priv->lut[chan].net_device);
        priv->lut[chan].netqueue_stop = 0;
      }
    }
    spin_unlock_irqrestore(&priv->netifqueueLock, priv->netifqueueLockFlag);

    if(vcc->pop)
    {
      dprintf(5, "free packet\n");
      vcc->pop(vcc, skb);
    }

    
  }
    
  

  /* Update Stats: There may be a better place to do this, but this is a start */
  priv->stats.tx_packets++;
#ifdef CONFIG_LED_MODULE
//    led_operation(MOD_ADSL, DEF_ADSL_ACTIVITY);           
#endif

  /* track number of buffer used */
  
  dprintf(4, "tn7atm_send_complete() done\n");

  return 0;
}

void *tn7atm_allocate_rx_skb(void *os_dev, void **os_receive_info, unsigned int size)
{
  struct sk_buff *skb;
  dprintf(4, "tn7atm_allocate_rx_skb size=%d\n", size);
  size = ((size+3)&0xfffffffc);
  skb = dev_alloc_skb(size);
  if(skb==NULL)
  {
    //printk("rx allocate skb failed\n");
    return NULL;
  }
  *os_receive_info = (void *)skb;
  return (skb->data);
}

void tn7atm_free_rx_skb(void *skb)
{
  dprintf(4, "tn7atm_free_rx_skb\n");
  tn7atm_kfree_skb((struct sk_buff *)skb);
}


int tn7atm_receive(void *os_dev, int ch, unsigned int packet_size, void *os_receive_info, void *data)
{
  Tn7AtmPrivate *priv;
  struct atm_dev *dev;
  struct sk_buff *skb;
  struct atm_vcc *vcc;
  

  dprintf(4, "tn7atm_receive()\n");
  dev = (struct atm_dev *)os_dev;

  priv = (Tn7AtmPrivate *)dev->dev_data;

  if(priv->lConnected != 1 || priv->lut[ch].ready == 0)
  {
    //printk("channel not ready\n");
    return 1;
  }

  vcc = (struct atm_vcc *)priv->lut[ch].vcc;
  if(vcc == NULL)
  {
    printk("vcc=Null");
    return 1;
  }


  /* assume no fragment packet for now */
  skb = (struct sk_buff *)os_receive_info;
  
  if(skb==NULL)
  {
    dprintf(1, "received empty skb.\n");
    return 1;
  }
  /* see skbuff->cb definition in include/linux/skbuff.h */
  ATM_SKB(skb)->vcc = vcc;

  skb->len = packet_size;
  dprintf(3, "skb:[0x%p]:0x%x pdu_len: 0x%04x\n",skb,skb->len,packet_size);
  dprintf(3, "data location: 0x%x, 0x%x\n", (unsigned int)skb->data, (unsigned int)data);
  
  /*skb_trim(skb,skb->len); */   /* skb size is incorrect for large packets > 1428 bytes ?? */
  __skb_trim(skb,skb->len); /* change to correct > 1500 ping when firewall is on */

  dprintf(3, "pushing the skb...\n");
  skb->stamp = xtime;

  xdump((unsigned char *)skb->data, skb->len, 5);
  
  if(atm_charge(vcc, skb->truesize) == 0)
  {
    dprintf(1,"Receive buffers saturated for %d.%d.%d - PDU dropped\n", vcc->itf, vcc->vci, vcc->vpi);
    return 1;
  }

  /*pass it up to kernel networking layer and update stats*/
  vcc->push(vcc,skb);

  /* Update receive packet stats */
  priv->stats.rx_packets++;
  atomic_inc(&vcc->stats->rx);

#ifdef CONFIG_LED_MODULE
//    led_operation(MOD_ADSL, DEF_ADSL_ACTIVITY);           
#endif
  dprintf(3, "(a) Receiving:vpi/vci[%d/%d] chan_id: %d  skb len:0x%x  skb truesize:0x%x\n",
                vcc->vpi,vcc->vci,ch,skb->len, skb->truesize);
  
  return 0;
}

static int
tn7atm_proc_channels(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len = 0;
  int limit = count - 80;
  int i;

  struct atm_dev *dev;
  Tn7AtmPrivate *priv;

  dev = (struct atm_dev *)data;
  priv = (Tn7AtmPrivate *)dev->dev_data;

  if(len<=limit)
      len += sprintf(buf+len,"Chan  Inuse   ChanID   VPI     VCI \n");
  if(len<=limit)
      len += sprintf(buf+len,"------------------------------------------------------------------\n");

  for(i=0; i < MAX_DMA_CHAN; i++)
    {
      if(len<=limit)
        {
          len += sprintf(buf+len, 
                        " %02d    %05d   %05d   %05d   %05d \n",
                        i,priv->lut[i].inuse,priv->lut[i].chanid,
                        priv->lut[i].vpi,priv->lut[i].vci);
        }
    }

  return len;
}

static int
tn7atm_proc_private(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len = 0;
  int limit = count - 80;
  struct atm_dev *dev;
  Tn7AtmPrivate *priv;

  dev = (struct atm_dev *)data;
  priv = (Tn7AtmPrivate *)dev->dev_data;
  
  if(len<=limit)
      len += sprintf(buf+len, "\nPrivate Data Structure(%s):\n",priv->name);
  if(len<=limit)
      len += sprintf(buf+len, "----------------------------------------\n");
  if(len<=limit)
      len += sprintf(buf+len, "priv:  0x%p\n",priv);
  if(len<=limit)
      len += sprintf(buf+len, "next:  0x%p",priv->next);
  if(len<=limit)
        len += sprintf(buf+len, "\tdev:   0x%p\n",priv->dev);
 
  if(len<=limit)
      len += sprintf(buf+len, "tx_irq: %02d",priv->sar_irq);
  if(len<=limit)
      len += sprintf(buf+len, "rx_irq: %02d",priv->dsl_irq);
  

  return len;
}

void tn7atm_sarhal_isr_register(void *os_dev, void *hal_isr, int interrupt_num)
{
  struct atm_dev *dev;
  Tn7AtmPrivate *priv;

  dprintf(4, "tn7atm_sarhal_isr_register()\n");

  dev = (struct atm_dev *)os_dev;
  priv = (Tn7AtmPrivate *)dev->dev_data;
  priv->halIsr = (void *)hal_isr;
  priv->int_num = interrupt_num;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int tn7atm_exit(void)
 *
 *  Description: Avalanche SAR exit function
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void
tn7atm_exit (void)
{

  struct atm_dev        *dev;

  Tn7AtmPrivate *priv;

  dprintf(4, "tn7atm_exit()\n");

  dev=mydev;
  priv = (Tn7AtmPrivate *)dev->dev_data;
  priv->lConnected = 0;
  tn7dsl_exit();

  tn7sar_exit(dev, priv);

  /* freeup irq's */
  free_irq(priv->dsl_irq,priv->dev);
  free_irq(priv->sar_irq,priv->dev);
  
  kfree (dev->dev_data);

  //    atm_dev_deregister (dev);
  shutdown_atm_dev(dev);

  /* remove proc entries */
  remove_proc_entry("tiatm/avsar_ver",NULL);
  remove_proc_entry("tiatm/avsar_modem_stats",NULL);
  remove_proc_entry("tiatm/avsar_modem_training",NULL);
  remove_proc_entry("tiatm/avsar_channels",NULL);
  remove_proc_entry("tiatm/avsar_private",NULL);
  remove_proc_entry("tiatm/avsar_sarhal_stats",NULL);
  remove_proc_entry("tiatm/avsar_oam_ping",NULL);
  remove_proc_entry("tiatm/avsar_pvc_table",NULL);
  remove_proc_entry("tiatm",NULL);
  tn7dsl_dslmod_sysctl_unregister();
  
  printk ("Module Removed\n");

}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int tn7atm_registration(struct tn7* tn7)
 *
 *  Description: ATM driver registration
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static int __init
tn7atm_register (Tn7AtmPrivate * priv)
{
  /* allocate memory for the device */

  dprintf(4,"device %s being registered\n", priv->name);

  mydev = atm_dev_register (priv->proc_name, &tn7atm_ops, -1, NULL);

  if (mydev == NULL)
    {
      printk ("atm_dev_register returning NULL\n");     
      return ATM_REG_FAILED;
    }

  printk ("registered device %s\n", priv->name);

  mydev->dev_data = priv;       /* setup device data in atm_dev struct */
  priv->dev = mydev;    /* setup atm_device in avalanche sar struct */

  mydev->ci_range.vpi_bits = ATM_CI_MAX;        /* atm supports 11 bits */
  mydev->ci_range.vci_bits = 16;        /* atm VCI max = 16 bits */

  
  return ATM_REG_OK;
}

static int 
tn7atm_proc_version(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len = 0;
  char dslVer[8];
  char dspVer[10];
  char *pSarVer;
  Tn7AtmPrivate *priv;
  int i;
  unsigned int pdspV1, pdspV2;

  priv = mydev->dev_data;

  len += sprintf(buf+len, "ATM Driver version:[%d.%02d.%02d.%02d]\n",LINUXATM_VERSION_MAJOR, LINUXATM_VERSION_MINOR,
                                                                   LINUXATM_VERSION_BUGFIX, LINUXATM_VERSION_BUILDNUM);

  tn7dsl_get_dslhal_version(dslVer);

  len += sprintf(buf+len, "DSL HAL version: [%d.%02d.%02d.%02d]\n", dslVer[0], dslVer[1], dslVer[2],
                                            dslVer[3]);
  tn7dsl_get_dsp_version(dspVer);

  len += sprintf(buf+len, "DSP Datapump version: [%d.%02d.%02d.%02d] ", dspVer[4], dspVer[5], dspVer[6],
                                            dspVer[7]);
  if(dspVer[8]==2) // annex B
    len += sprintf(buf+len, "Annex B\n");
  else if(dspVer[8]==3) //annex c
    len += sprintf(buf+len, "Annex c\n");
  else
    len += sprintf(buf+len, "Annex A\n");

  tn7sar_get_sar_version(priv, &pSarVer);

  len += sprintf(buf+len, "SAR HAL version: [");
  for(i=0;i<8;i++)
  {
    len += sprintf(buf+len, "%c", pSarVer[i+7]);
  }
  len += sprintf(buf+len, "]\n");

  tn7sar_get_sar_firmware_version(&pdspV1, &pdspV2);
  len += sprintf(buf+len, "PDSP Firmware version:[%01x.%02x]\n",
                 pdspV1,pdspV2);

  return len;
}

/*
static struct net_device_stats
*tn7atm_get_stats(struct atm_dev *dev)
{
  Tn7AtmPrivate *priv;
  //unsigned long flags;

  //spin_lock_irqsave(&priv->stats_lock,flags);
  priv= (Tn7AtmPrivate *)dev->dev_data;
  //spin_unlock_irqrestore(&priv->stats_lock,flags);
  
  return &priv->stats;
  
}
*/
/* Device detection */

static int __init
tn7atm_detect (void)
{
  Tn7AtmPrivate *priv;
  //static struct proc_dir_entry *proc_dir;

  dprintf(4, "tn7atm_detect().\n");
  /* Device allocated as a global static structure at top of code "mydev" */

  /* Alloc priv struct */
  priv=kmalloc(sizeof(Tn7AtmPrivate),GFP_KERNEL);
  if(!priv)
    {
      printk("unable to kmalloc priv structure. Killing autoprobe.\n");
      return -ENODEV;
    }
  memset(priv, 0, sizeof(Tn7AtmPrivate));
#ifdef COMMON_NSP
  priv->name = "TI Avalanche SAR";
  priv->proc_name = "avsar";
#else
  priv->name = "TI tnetd73xx ATM Driver";
  priv->proc_name = "tn7";
#endif

  if ((tn7atm_register (priv)) == ATM_REG_FAILED)
    return -ENODEV;

  tn7atm_init(mydev);

  /* Set up proc entry for atm stats */
  proc_mkdir("tiatm", NULL);
  create_proc_read_entry("tiatm/avsar_modem_stats",0,NULL,tn7dsl_proc_stats,NULL);
  create_proc_read_entry("tiatm/avsar_modem_training",0,NULL,tn7dsl_proc_modem,NULL);
  create_proc_read_entry("tiatm/avsar_ver",0,NULL,tn7atm_proc_version,NULL);
  create_proc_read_entry("tiatm/avsar_channels",0,NULL,tn7atm_proc_channels,mydev);
  create_proc_read_entry("tiatm/avsar_private",0,NULL,tn7atm_proc_private,mydev);
  create_proc_read_entry("tiatm/avsar_sarhal_stats",0,NULL,tn7sar_proc_sar_stat,mydev);
  create_proc_read_entry("tiatm/avsar_oam_ping",0,NULL,tn7sar_proc_oam_ping,mydev);
  create_proc_read_entry("tiatm/avsar_pvc_table",0,NULL,tn7sar_proc_pvc_table,mydev);

  tn7dsl_dslmod_sysctl_register();

  printk("Texas Instruments ATM driver: version:[%d.%02d.%02d.%02d]\n",LINUXATM_VERSION_MAJOR, LINUXATM_VERSION_MINOR,
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
int __init tn7atm_probe(void)
{
  tn7atm_detect();
  return -ENODEV;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int tn7atm_init(struct atm_dev *dev)
 *
 *  Description: Device Initialization
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static int __init
tn7atm_init(struct atm_dev *dev)
{
  Tn7AtmPrivate *priv;
  char *ptr;

  dprintf(4, "tn7atm_init()\n");

  priv = (Tn7AtmPrivate *)dev->dev_data;

  if(tn7sar_init(dev, priv) != 0)
  {
    printk("Failed to init SAR.\n");
    return -ENODEV;
  }

  if(tn7dsl_init(priv) < 0)
  {
    printk("Failed to init DSL.\n");
    return -ENODEV;
  }
  
  if(tn7atm_get_ESI(dev) < 0)                 /* set ESI */  
    return -ENODEV;

  if(tn7atm_irq_request(dev) < 0)     
    return -EBUSY;

  priv->bTurboDsl = 1;
  // read config for turbo dsl
  ptr = prom_getenv("TurboDSL");
  if(ptr)
  {
    priv->bTurboDsl = os_atoi(ptr);
  }

  return 0;
}

int tn7atm_device_connect_status(void *priv, int state)
{
  Tn7AtmPrivate *priv1;

  dprintf(5, "tn7atm_device_connect_status()\n");
  priv1 = (Tn7AtmPrivate *)priv;

  priv1->lConnected = state;
  dprintf(5, "priv1->lConnected=%d\n", priv1->lConnected);
  return 0;
}


#ifdef MODULE
module_init (tn7atm_detect);
module_exit (tn7atm_exit);
#endif /* MODULE */
