/* 
 *  $Id: 004-atm_driver.patch 2877 2006-01-10 19:43:00Z nbd $
 *
 *   Avalanche SAR driver
 *
 *   Zhicheng Tang, ztang@ti.com
 *   2000 (c) Texas Instruments Inc.
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
#include <linux/sysctl.h>
#include <linux/timer.h>
#include <linux/vmalloc.h>
#include <linux/file.h>
#include <asm/uaccess.h>

#include "tn7atm.h"
#include "tn7api.h"
#include "dsl_hal_api.h"

#ifdef CONFIG_LED_MODULE
#include <asm/ar7/ledapp.h>
#define MOD_ADSL 1
#define DEF_ADSL_IDLE     1
#define DEF_ADSL_TRAINING 2
#define DEF_ADSL_SYNC     3
#define DEF_ADSL_ACTIVITY 4

#define LED_NUM_1 3
#define LED_NUM_2 4

led_reg_t ledreg[2];

static int led_on;
#endif

extern int  __guDbgLevel;
extern sar_stat_t sarStat;
static int dslInSync = 0;
static int bMarginThConfig;
static int bMarginRetrainEnable;
static char EOCVendorID[8]= {0xb5, 0x00, 0x54, 0x53, 0x54, 0x43, 0x00, 0x00};

#define TC_SYNC          1
#define SYNC_TIME_DELAY  500000


#define DEV_DSLMOD       1
#define MAX_STR_SIZE     256
#define DSL_MOD_SIZE     256

#define TRUE   1
#define FALSE  0


enum
{
  NO_MODE,
  MULTI_MODE,
  T1413_MODE,
  GDMT_MODE,
  GLITE_MODE
};



/* a structure to store all information we need
   for our thread */
typedef struct kthread_struct
{
  /* private data */

  /* Linux task structure of thread */
  struct task_struct *thread;
  /* Task queue need to launch thread */
  struct tq_struct tq;
  /* function to be started as thread */
  void (*function) (struct kthread_struct *kthread);
  /* semaphore needed on start and creation of thread. */
  struct semaphore startstop_sem;

  /* public data */

  /* queue thread is waiting on. Gets initialized by
  init_kthread, can be used by thread itself.
  */
  wait_queue_head_t queue;
  /* flag to tell thread whether to die or not.
    When the thread receives a signal, it must check
    the value of terminate and call exit_kthread and terminate
    if set.
  */
  int terminate;
  /* additional data to pass to kernel thread */
  void *arg;
} kthread_t;

#ifndef ADIAG
#define DSP_FIRMWARE_PATH "/lib/modules/ar0700xx.bin"
#else
#define DSP_FIRMWARE_PATH "/var/tmp/ar0700xx_diag.bin"
#endif

/* externs */
extern struct atm_dev *mydev;
extern unsigned int oamFarLBCount[4];
extern int dslhal_support_restoreTrainingInfo(PITIDSLHW_T pIhw);
/* gloabal functions */

/* end of global functions */

/* module wide declars */
static PITIDSLHW_T    pIhw;
static char    mod_req[16]={'\t'};
static volatile int bshutdown; 
static char info[MAX_STR_SIZE];
static DECLARE_MUTEX_LOCKED(adsl_sem_overlay); /* Used for DSL Polling enable     */
kthread_t overlay_thread;
/* end of module wide declars */

/* Internal Functions */
static void tn7dsl_chng_modulation(void* data);
static void tn7dsl_set_modulation(void* data);
static int tn7dsl_reload_overlay(void);
static int dslmod_sysctl(ctl_table *ctl, int write, struct file * filp, void *buffer, size_t *lenp);
static void tn7dsl_register_dslss_led(void);
void tn7dsl_dslmod_sysctl_register(void);
void tn7dsl_dslmod_sysctl_unregister(void);
/* end of internal functions */





/* prototypes */

/* start new kthread (called by creator) */
void start_kthread(void (*func)(kthread_t *), kthread_t *kthread);

/* stop a running thread (called by "killer") */
void stop_kthread(kthread_t *kthread);

/* setup thread environment (called by new thread) */
void init_kthread(kthread_t *kthread, char *name);

/* cleanup thread environment (called by thread upon receiving termination signal) */
void exit_kthread(kthread_t *kthread);



/* private functions */
static void kthread_launcher(void *data)
{
        kthread_t *kthread = data;
        kernel_thread((int (*)(void *))kthread->function, (void *)kthread, 0);
        
}

/* public functions */

/* create a new kernel thread. Called by the creator. */
void start_kthread(void (*func)(kthread_t *), kthread_t *kthread)
{
        /* initialize the semaphore:
           we start with the semaphore locked. The new kernel
           thread will setup its stuff and unlock it. This
           control flow (the one that creates the thread) blocks
           in the down operation below until the thread has reached
           the up() operation.
         */
        //init_MUTEX_LOCKED(&kthread->startstop_sem);

        /* store the function to be executed in the data passed to
           the launcher */
        kthread->function=func;
        
        /* create the new thread my running a task through keventd */

        /* initialize the task queue structure */
        kthread->tq.sync = 0;
        INIT_LIST_HEAD(&kthread->tq.list);
        kthread->tq.routine =  kthread_launcher;
        kthread->tq.data = kthread;

        /* and schedule it for execution */
        schedule_task(&kthread->tq);

        /* wait till it has reached the setup_thread routine */
        //down(&kthread->startstop_sem);
               
}

/* stop a kernel thread. Called by the removing instance */
void stop_kthread(kthread_t *kthread)
{
        if (kthread->thread == NULL)
        {
                printk("stop_kthread: killing non existing thread!\n");
                return;
        }

        /* this function needs to be protected with the big
       kernel lock (lock_kernel()). The lock must be
           grabbed before changing the terminate
       flag and released after the down() call. */
        lock_kernel();
        
        /* initialize the semaphore. We lock it here, the
           leave_thread call of the thread to be terminated
           will unlock it. As soon as we see the semaphore
           unlocked, we know that the thread has exited.
    */
        //init_MUTEX_LOCKED(&kthread->startstop_sem);

        /* We need to do a memory barrier here to be sure that
           the flags are visible on all CPUs. 
        */
        mb();

        /* set flag to request thread termination */
        kthread->terminate = 1;

        /* We need to do a memory barrier here to be sure that
           the flags are visible on all CPUs. 
        */
        mb();
        kill_proc(kthread->thread->pid, SIGKILL, 1);
       
        /* block till thread terminated */
        //down(&kthread->startstop_sem);

        /* release the big kernel lock */
        unlock_kernel();

        /* now we are sure the thread is in zombie state. We
           notify keventd to clean the process up.
        */
        kill_proc(2, SIGCHLD, 1);

}

/* initialize new created thread. Called by the new thread. */
void init_kthread(kthread_t *kthread, char *name)
{
        /* lock the kernel. A new kernel thread starts without
           the big kernel lock, regardless of the lock state
           of the creator (the lock level is *not* inheritated)
        */
        lock_kernel();

        /* fill in thread structure */
        kthread->thread = current;

        /* set signal mask to what we want to respond */
        siginitsetinv(&current->blocked, sigmask(SIGKILL)|sigmask(SIGINT)|sigmask(SIGTERM));

        /* initialise wait queue */
        init_waitqueue_head(&kthread->queue);

        /* initialise termination flag */
        kthread->terminate = 0;

        /* set name of this process (max 15 chars + 0 !) */
        sprintf(current->comm, name);
        
        /* let others run */
        unlock_kernel();

        /* tell the creator that we are ready and let him continue */
        //up(&kthread->startstop_sem);

}

/* cleanup of thread. Called by the exiting thread. */
void exit_kthread(kthread_t *kthread)
{
        /* we are terminating */

    /* lock the kernel, the exit will unlock it */
        lock_kernel();
        kthread->thread = NULL;
        mb();

        /* notify the stop_kthread() routine that we are terminating. */
    //up(&kthread->startstop_sem);
    /* the kernel_thread that called clone() does a do_exit here. */

    /* there is no race here between execution of the "killer" and real termination
       of the thread (race window between up and do_exit), since both the
       thread and the "killer" function are running with the kernel lock held.
       The kernel lock will be freed after the thread exited, so the code
       is really not executed anymore as soon as the unload functions gets
       the kernel lock back.
       The init process may not have made the cleanup of the process here,
       but the cleanup can be done safely with the module unloaded.
    */

}



int os_atoi(const char *pStr)
{
  int retVal = -1;

  if(*pStr=='-')
    retVal =  -simple_strtoul(pStr+1, (char **)NULL, 10);
  else
    retVal =  simple_strtoul(pStr, (char **)NULL, 10);  
  return retVal ;
}


int os_atoh(const char *pStr)
{
  int retVal = -1;

  if(*pStr=='-')
    retVal =  -simple_strtoul(pStr+1, (char **)NULL, 16);
  else
    retVal =  simple_strtoul(pStr, (char **)NULL, 16);  
  return retVal ;
}

unsigned long os_atoul(const char *pStr)
{
  unsigned long retVal = -1;

  retVal =  simple_strtoul(pStr, (char **)NULL, 10);  
  return retVal ;
}

void dprintf( int uDbgLevel, char * szFmt, ...)
{
#ifdef DEBUG_BUILD
    static char     buff[256];
    va_list  ap; 

    if( __guDbgLevel < uDbgLevel) 
        return;

    va_start( ap, szFmt); 
    vsprintf((char *)buff, szFmt, ap); 
    va_end(ap); 
    printk("%s", buff);
#endif
}

/*int strcmp(const char *s1, const char *s2)
{

  int i=0;
  
  while(s1[i] !=0)
  {
    if(s2[i]==0)
      return -1;
    if(s1[i] != s2[i])
      return 1;
    i++;
  }
  if(s2[i] != 0)
    return 1;
  return 0;
}
*/

int shim_osLoadFWImage(unsigned char *ptr)
{
  unsigned int bytesRead;
  mm_segment_t  oldfs;  
  static struct file *filp;
  unsigned int imageLength=0x4ffff;  
   
    
  dprintf(4, "tn7dsl_read_dsp()\n");

  dprintf(4,"open file %s\n", DSP_FIRMWARE_PATH);
  
  filp=filp_open(DSP_FIRMWARE_PATH
                                  ,00,O_RDONLY);
      
  if(filp ==NULL)
  {
    printk("Failed: Could not open DSP binary file\n");
          return -1;
  }
      
  if (filp->f_op->read==NULL)
          return -1;  /* File(system) doesn't allow reads */

        /* Now read bytes from postion "StartPos" */
        filp->f_pos = 0;
  oldfs = get_fs();
        set_fs(KERNEL_DS);
        bytesRead = filp->f_op->read(filp,ptr,imageLength,&filp->f_pos);

  dprintf(4,"file length = %d\n", bytesRead);

  set_fs(oldfs);

        /* Close the file */
        fput(filp); 
    
  return bytesRead;
}

unsigned int shim_read_overlay_page(void *ptr, unsigned int secOffset, unsigned int secLength)
{
  unsigned int bytesRead;
  mm_segment_t  oldfs;  
  struct file *filp;
  
  dprintf(4,"shim_read_overlay_page\n");
  //dprintf(4,"sec offset=%d, sec length =%d\n", secOffset, secLength);
  
  filp=filp_open(DSP_FIRMWARE_PATH,00,O_RDONLY);  
  if(filp ==NULL)
  {
    printk("Failed: Could not open DSP binary file\n");
          return -1;
  }
      
  if (filp->f_op->read==NULL)
          return -1;  /* File(system) doesn't allow reads */

  /* Now read bytes from postion "StartPos" */

  if(filp->f_op->llseek)
    filp->f_op->llseek(filp,secOffset, 0);
  oldfs = get_fs();
  set_fs(KERNEL_DS);
  filp->f_pos = secOffset;
  bytesRead = filp->f_op->read(filp,ptr,secLength,&filp->f_pos);

  set_fs(oldfs);
   /* Close the file */
  fput(filp);  
  return bytesRead;
}

int shim_osLoadDebugFWImage(unsigned char *ptr)
{
  return 0;
}
int shim_osStringCmp(const char *s1, const char *s2)
{
  return strcmp(s1, s2);
}

void *shim_osAllocateMemory(unsigned int size)
{
  return ((void *)kmalloc(size, GFP_KERNEL));
}

void *shim_osAllocateDmaMemory(unsigned int size)
{
  /*
  int order;

  order=1;
  size=size/4096;
  while(size >= 1)
  {
    order++;
    size=size/2;
  }

  return ( (void *)__get_free_pages(GFP_ATOMIC, order));
  */
  //return ((void *)kmalloc(size, GFP_ATOMIC));
  //return ((void *)kmalloc(size, GFP_KERNEL));
  void *ptr;

  ptr = kmalloc(size, GFP_ATOMIC);
  if(ptr==NULL)
  {
    printk("failed atomic\n");
    ptr = kmalloc(size, GFP_KERNEL);
    if(ptr==NULL)
    {
      printk("failed kernel\n");
      ptr = kmalloc(size, GFP_KERNEL|GFP_DMA);
    }
  }
  printk("size=%d\n", size);
  return ptr;

}


void shim_osFreeMemory(void *ptr, unsigned int size)
{
  
  kfree(ptr);
}

void shim_osFreeDmaMemory(void *ptr, unsigned int size)
{
/*
  int order;

  order=1;
  size=size/4096;
  while(size >=1)
  {
    order++;
    size=size/2;
  }
  free_pages(ptr, order);
*/
  kfree(ptr);
}

void *shim_osAllocateVMemory(unsigned int size)
{

  return ((void *)vmalloc(size));
}

void shim_osFreeVMemory(void *ptr, unsigned int size)
{
  vfree(ptr);
}

void shim_osMoveMemory(char *dst, char *src, unsigned int numBytes)
{
  memcpy(dst, src, numBytes);
}

void shim_osZeroMemory(char *dst, unsigned int numBytes)
{
  memset(dst, 0, numBytes);
}

void shim_osWriteBackCache(void *addr, unsigned int size)
{
  unsigned int i,Size=(((unsigned int)addr)&0xf)+size;

  for (i=0;i<Size;i+=16,addr+=16)
  {
    __asm__(" .set mips3 ");
    __asm__(" cache  25, (%0)" : : "r" (addr));
    __asm__(" .set mips0 ");
  }
}

void shim_osInvalidateCache(void *addr, unsigned int size)
{
  unsigned int i,Size=(((unsigned int)addr)&0xf)+size;

  for (i=0;i<Size;i+=16,addr+=16)
  {
    __asm__(" .set mips3 ");
    __asm__("cache  17, (%0)" : : "r" (addr));
    __asm__(" .set mips0 ");
  }
}

void shim_osClockWait(int val)
{
  unsigned int chkvalue;
  chkvalue=val/64;
  
  if(chkvalue > 1000)
    {
      mdelay(chkvalue/1000);
      return;
    }
  else
    udelay(val/64);
} /* end of cwait() */

unsigned int shim_osClockTick(int val)
{
  return jiffies;
}

int flags;
spinlock_t shimLock;

void shim_osCriticalEnter(void)
{
  spin_lock_irqsave(&shimLock, flags);

}


void shim_osCriticalExit(void)
{
  spin_unlock_irqrestore(&shimLock, flags);
}


int tn7dsl_proc_stats(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{
  
  int len = 0;
  int limit = count - 80;
  int F4count, F5count;
  
  
  /* Read Ax5 Stats */
  dslhal_api_gatherStatistics(pIhw);
  
  if(len<=limit)
    len += sprintf(buf+len, "\nAR7 DSL Modem Statistics:\n");
  if(len<=limit)
    len += sprintf(buf+len, "--------------------------------\n");
  /* us and ds Connection Rates */
  if(len<=limit)
    len += sprintf(buf+len, "[DSL Modem Stats]\n");


  if(len<=limit)
  {
    if(pIhw->lConnected != 1)
    {
      pIhw->AppData.USConRate = 0;
      pIhw->AppData.DSConRate = 0;
    }
    len += sprintf(buf+len, "\tUS Connection Rate:\t%u\tDS Connection Rate:\t%u\n",
                   (unsigned int)pIhw->AppData.USConRate,
                   (unsigned int)pIhw->AppData.DSConRate );
  }
  if(len<=limit)
    len += sprintf(buf+len, "\tDS Line Attenuation:\t%u\tDS Margin:\t\t%u\n",
                   (unsigned int)pIhw->AppData.dsLineAttn/2,
                   (unsigned int)pIhw->AppData.dsMargin/2 );
  if(len<=limit)
    len += sprintf(buf+len, "\tUS Line Attenuation:\t%u\tUS Margin:\t\t%u\n",
                   (unsigned int)pIhw->AppData.usLineAttn,
                   (unsigned int)pIhw->AppData.usMargin );
  if(len<=limit)
    len += sprintf(buf+len, "\tUS Payload :\t\t%u\tDS Payload:\t\t%u\n",
                   ((unsigned int)pIhw->AppData.usAtm_count[0] + (unsigned int)pIhw->AppData.usAtm_count[1])*48,
                   ((unsigned int)pIhw->AppData.dsGood_count[0] + (unsigned int)pIhw->AppData.dsGood_count[1])*48);
  /* Superframe Count */
  if(len<=limit)
    len += sprintf(buf+len, "\tUS Superframe Cnt :\t%u\tDS Superframe Cnt:\t%u\n",
                   (unsigned int)pIhw->AppData.usSuperFrmCnt,
                   (unsigned int)pIhw->AppData.dsSuperFrmCnt );

  /* US and DS power */
  if(len<=limit)
    len += sprintf(buf+len, "\tUS Transmit Power :\t%u\tDS Transmit Power:\t%u\n",
                   (unsigned int)pIhw->AppData.usTxPower/256,
                   (unsigned int)pIhw->AppData.dsTxPower/256 );
  /* DSL Stats Errors*/  
  if(len<=limit)
    len += sprintf(buf+len, "\tLOS errors:\t\t%u\tSEF errors:\t\t%u\n",
                   (unsigned int)pIhw->AppData.LOS_errors,
                   (unsigned int)pIhw->AppData.SEF_errors );
  if(len<=limit)
    len += sprintf(buf+len, "\tFrame mode:\t\t%u\tMax Frame mode:\t\t%u\n",
                   (unsigned int)pIhw->AppData.FrmMode,
                   (unsigned int)pIhw->AppData.MaxFrmMode );
  if(len<=limit)
    len += sprintf(buf+len, "\tTrained Path:\t\t%u\tUS Peak Cell Rate:\t%u\n",
                   (unsigned int)pIhw->AppData.TrainedPath,
                   (unsigned int)pIhw->AppData.USConRate*1000/8/53 );
  if(len<=limit)
    len += sprintf(buf+len, "\tTrained Mode:\t\t%u\tSelected Mode:\t\t%u\n",
                   (unsigned int)pIhw->AppData.TrainedMode, (unsigned int)pIhw->AppData.StdMode );

  if(len<=limit)
    len += sprintf(buf+len, "\tATUC Vendor ID:\t%u\tATUC Revision:\t\t%u\n",
                   (unsigned int)pIhw->AppData.atucVendorId, pIhw->AppData.atucRevisionNum);
  if(len<=limit)
    len += sprintf(buf+len, "\tHybrid Selected:\t%u\n",
                   (unsigned int)pIhw->AppData.currentHybridNum);
 
  /* Upstream Interleaved Errors */
  if(len<=limit)
    len += sprintf(buf+len, "\n\t[Upstream (TX) Interleave path]\n");
  if(len<=limit)
    len += sprintf(buf+len, "\tCRC: \t%u\tFEC: \t%u\tNCD: \t%u\n",
                   (unsigned int)pIhw->AppData.usICRC_errors,
                   (unsigned int)pIhw->AppData.usIFEC_errors,
                   (unsigned int)pIhw->AppData.usINCD_error);
  if(len<=limit)
    len += sprintf(buf+len, "\tLCD: \t%u\tHEC: \t%u\n",
                   (unsigned int)pIhw->AppData.usILCD_errors,
                   (unsigned int)pIhw->AppData.usIHEC_errors);
  /* Downstream Interleaved Errors */
  if(len<=limit)
    len += sprintf(buf+len, "\n\t[Downstream (RX) Interleave path]\n");
  if(len<=limit)
    len += sprintf(buf+len, "\tCRC: \t%u\tFEC: \t%u\tNCD: \t%u\n",
                   (unsigned int)pIhw->AppData.dsICRC_errors,
                   (unsigned int)pIhw->AppData.dsIFEC_errors,
                   (unsigned int)pIhw->AppData.dsINCD_error);
  if(len<=limit)
    len += sprintf(buf+len, "\tLCD: \t%u\tHEC: \t%u\n",
                   (unsigned int)pIhw->AppData.dsILCD_errors,
                   (unsigned int)pIhw->AppData.dsIHEC_errors);
  /* Upstream Fast Errors */
  if(len<=limit)
    len += sprintf(buf+len, "\n\t[Upstream (TX) Fast path]\n");
  if(len<=limit)
    len += sprintf(buf+len, "\tCRC: \t%u\tFEC: \t%u\tNCD: \t%u\n",
                   (unsigned int)pIhw->AppData.usFCRC_errors,
                   (unsigned int)pIhw->AppData.usFFEC_errors,
                   (unsigned int)pIhw->AppData.usFNCD_error);
  if(len<=limit)
    len += sprintf(buf+len, "\tLCD: \t%u\tHEC: \t%u\n",
                   (unsigned int)pIhw->AppData.usFLCD_errors,
                   (unsigned int)pIhw->AppData.usFHEC_errors);
  /* Downstream Fast Errors */
  if(len<=limit)
    len += sprintf(buf+len, "\n\t[Downstream (RX) Fast path]\n");
  if(len<=limit)
    len += sprintf(buf+len, "\tCRC: \t%u\tFEC: \t%u\tNCD: \t%u\n",
                   (unsigned int)pIhw->AppData.dsFCRC_errors,
                   (unsigned int)pIhw->AppData.dsFFEC_errors,
                   (unsigned int)pIhw->AppData.dsFNCD_error);
  if(len<=limit)
    len += sprintf(buf+len, "\tLCD: \t%u\tHEC: \t%u\n",
                   (unsigned int)pIhw->AppData.dsFLCD_errors,
                   (unsigned int)pIhw->AppData.dsFHEC_errors);
  /* ATM stats upstream */
  if(len<=limit)
    len += sprintf(buf+len, "\n[ATM Stats]");
  if(len<=limit)
    len += sprintf(buf+len, "\n\t[Upstream/TX]\n");
  if(len<=limit)
    len += sprintf(buf+len, "\tGood Cell Cnt:\t%u\n\tIdle Cell Cnt:\t%u\n\n",
                   (unsigned int)pIhw->AppData.usAtm_count[0] + (unsigned int)pIhw->AppData.usAtm_count[1],
                   (unsigned int)pIhw->AppData.usIdle_count[0] + (unsigned int)pIhw->AppData.usIdle_count[1]);
  /* ATM stats downstream */
  if(len<=limit)
    len += sprintf(buf+len, "\n\t[Downstream/RX)]\n");
  if(len<=limit)
    len += sprintf(buf+len, "\tGood Cell Cnt:\t%u\n\tIdle Cell Cnt:\t%u\n\tBad Hec Cell Cnt:\t%u\n",
                   (unsigned int)pIhw->AppData.dsGood_count[0] + (unsigned int)pIhw->AppData.dsGood_count[1],
                   (unsigned int)pIhw->AppData.dsIdle_count[0] + (unsigned int)pIhw->AppData.dsIdle_count[1],
                   (unsigned int)pIhw->AppData.dsBadHec_count[0] + (unsigned int)pIhw->AppData.dsBadHec_count[1]);
  if(len<=limit)
    len += sprintf(buf+len, "\tOverflow Dropped Cell Cnt:\t%u\n",
                   (unsigned int)pIhw->AppData.dsOVFDrop_count[0] + (unsigned int)pIhw->AppData.dsOVFDrop_count[1]);
  tn7sar_get_stats(pIhw->pOsContext);
  if(len<=limit)
    len += sprintf(buf+len, "\n[SAR AAL5 Stats]\n");
  if(len<=limit)
    len += sprintf(buf+len, "\tTx PDU's:\t%u\n\tRx PDU's:\t%u\n",
                   sarStat.txPktCnt,
                   sarStat.rxPktCnt);
  if(len<=limit)
    len += sprintf(buf+len, "\tTx Total Bytes:\t%u\n\tRx Total Bytes:\t%u\n",
                   sarStat.txBytes,
                   sarStat.rxBytes);
  if(len<=limit)
    len += sprintf(buf+len, "\tTx Total Error Counts:\t%u\n\tRx Total Error Counts:\t%u\n\n",
                   sarStat.txErrors,
                   sarStat.rxErrors);

  /* oam loopback info */
  if(len<=limit)
    len += sprintf(buf+len, "\n[OAM Stats]\n");

  tn7sar_get_near_end_loopback_count(&F4count, &F5count);

  if(len<=limit)
    {
    len += sprintf(buf+len, "\tNear End F5 Loop Back Count:\t%u\n\tNear End F4 Loop Back Count:\t%u\n\tFar End F5 Loop Back Count:\t%u\n\tFar End F4 Loop Back Count:\t%u\n",
                  F5count,
                  F4count,
                  oamFarLBCount[0] + oamFarLBCount[2],
                  oamFarLBCount[1] + oamFarLBCount[3]);
    }
  return len;
}

int
tn7dsl_proc_modem(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{
  
  int len = 0;
  int limit = count - 80;

  char *state;
  int tag;
   
  tag= dslhal_api_pollTrainingStatus(pIhw);
  tag = pIhw->AppData.bState;
  
  switch(tag){
    case 0:       state = "ACTREQ";     break;
    case 1:       state = "QUIET1";     break;
    case 2:       state = "IDLE";       break;
    case 3:       state = "INIT";       break;
    case 4:       state = "RTDL";       break;
    case 5:       state = "SHOWTIME";   break;
    default:      state = "unknown";    break;
  }
  
  if(pIhw->lConnected == 1)
    state = "SHOWTIME";
  if(len<=limit)
    len += sprintf(buf+len,"%s\n",state);
  
  return len;
}



int tn7dsl_handle_interrupt(void)
{
  int intsrc;
  unsigned char cMsgRa[6];
  short margin;

  dprintf(4, "tn7dsl_handle_dsl_interrupt()\n");
  if(pIhw)
  {
    intsrc=dslhal_api_acknowledgeInterrupt(pIhw);
    dslhal_api_handleTrainingInterrupt(pIhw, intsrc);

    if(pIhw->lConnected == TC_SYNC)
    {
      
      if(dslInSync == 0)
      {
        printk("DSL in Sync\n");
        tn7atm_device_connect_status(pIhw->pOsContext, 1);
        dslhal_api_initStatistics(pIhw);
        dslhal_api_gatherStatistics(pIhw);
#ifdef CONFIG_LED_MODULE
//        led_operation(MOD_ADSL, DEF_ADSL_SYNC);           
        led_on = DEF_ADSL_SYNC;
#endif
        /* add auto margin retrain */
        if(pIhw->AppData.TrainedMode < 5)
        {
          if(bMarginRetrainEnable && bMarginThConfig == 0)
          {
            dslhal_support_getCMsgsRa(pIhw, cMsgRa);
            margin = *(unsigned short *)&cMsgRa[4];
            margin = (margin >> 6) & 0x3f;
            if(margin & 0x20) // highest bit is 1
            {
              margin = -(margin & 0x1f);
            }
          
            //printk("margin = %d, cmsg-ra = %02x %02x %02x %02x %02x %02x\n", margin, cMsgRa[0],cMsgRa[1],cMsgRa[2],cMsgRa[3],cMsgRa[4],cMsgRa[5]);
            dslhal_api_setMarginThreshold(pIhw, margin*2); /* DSL margin is in 0.5db */
          }
        }

      }
      dslInSync = 1;
    }
    else
    {
      if(dslInSync == 1)
      {
        dslInSync = 0;
        tn7atm_device_connect_status(pIhw->pOsContext, 0);
        up(&adsl_sem_overlay);
        printk("DSL out of syn\n");
      }
#ifdef CONFIG_LED_MODULE
      if(pIhw->AppData.bState < RSTATE_INIT)
      {
        if(led_on != DEF_ADSL_IDLE)
        {
//          led_operation(MOD_ADSL, DEF_ADSL_IDLE);           
          led_on = DEF_ADSL_IDLE;
        }
      }
      else
      {
        if(led_on != DEF_ADSL_TRAINING)
        {
//          led_operation(MOD_ADSL, DEF_ADSL_TRAINING);           
          led_on = DEF_ADSL_TRAINING;
        }

      }
      
#endif

    }
  }
  return 0;
}


int tn7dsl_get_dslhal_version(char *pVer)
{
  dslVer ver;

  dslhal_api_getDslHalVersion(&ver);

  memcpy(pVer,&ver,8);
  return 0;
}

int tn7dsl_get_dsp_version(char *pVer)
{
  dspVer ver;
  dslhal_api_getDspVersion(pIhw, &ver);
  memcpy(pVer, &ver, 9);
  return 0;
}


static int
tn7dsl_get_modulation(void)
{
  char  *ptr = NULL;
  
  dprintf(4, "tn7dsl_get_modulation\n");
  //printk("tn7dsl_get_modulation\n");
  ptr = prom_getenv("modulation");

  if (!ptr) {
    //printk("modulation is not set in adam2 env\n");
    //printk("Using multimode\n");
    return 0;
  }
  printk("dsl modulation = %s\n", ptr);

  tn7dsl_set_modulation(ptr);

  return 0;
}


static int tn7dsl_set_dsl(void)
{

  char  *ptr = NULL;
  int value;
  int i, offset[2]={4,11},oamFeature=0;
  char tmp[4];
  char dspVer[10];
 
  // OAM Feature Configuration
  dslhal_api_dspInterfaceRead(pIhw,(unsigned int)pIhw->pmainAddr, 2, (unsigned int *)&offset, (unsigned char *)&oamFeature, 4);
  oamFeature |= dslhal_support_byteSwap32(0x0000000C);
  dslhal_api_dspInterfaceWrite(pIhw,(unsigned int)pIhw->pmainAddr, 2, (unsigned int *)&offset, (unsigned char *)&oamFeature, 4);

  // modulation 
  ptr = prom_getenv("modulation");
  if (ptr) 
  {
    printk("dsl modulation = %s\n", ptr);
    tn7dsl_set_modulation(ptr);  
  }

  // margin retrain
  ptr = NULL;
  ptr = prom_getenv("enable_margin_retrain");
  if(ptr)
  {
    value = os_atoi(ptr);
    if(value == 1)
    {
      dslhal_api_setMarginMonitorFlags(pIhw, 0, 1);
      bMarginRetrainEnable = 1;
      printk("enable showtime margin monitor.\n");
      ptr = NULL;
      ptr = prom_getenv("margin_threshold");
      if(ptr)
      {
        value = os_atoi(ptr);
        printk("Set margin threshold to %d x 0.5 db\n",value);
        if(value >= 0)
        {
          dslhal_api_setMarginThreshold(pIhw, value);
          bMarginThConfig=1;
        }
      }
    }
  }

  // rate adapt
  ptr = NULL;
  ptr = prom_getenv("enable_rate_adapt");
  if(ptr)
  {
    dslhal_api_setRateAdaptFlag(pIhw, os_atoi(ptr));
  }

  // trellis
  ptr = NULL;
  ptr = prom_getenv("enable_trellis");
  if(ptr)
  {
    dslhal_api_setTrellisFlag(pIhw, os_atoi(ptr));
  }

  // maximum bits per carrier
  ptr = NULL;
  ptr = prom_getenv("maximum_bits_per_carrier");
  if(ptr)
  {
    dslhal_api_setMaxBitsPerCarrier(pIhw, os_atoi(ptr));
  }

  // maximum interleave depth
  ptr = NULL;
  ptr = prom_getenv("maximum_interleave_depth");
  if(ptr)
  {
    dslhal_api_setMaxInterleaverDepth(pIhw, os_atoi(ptr));
  }

  // inner and outer pairs
  ptr = NULL;
  ptr = prom_getenv("pair_selection");
  if(ptr)
  {
    dslhal_api_selectInnerOuterPair(pIhw, os_atoi(ptr));
  }
  
  ptr = NULL;
  ptr = prom_getenv("dgas_polarity");
  if(ptr)
  {
    dslhal_api_configureDgaspLpr(pIhw, 1, 1);
    dslhal_api_configureDgaspLpr(pIhw, 0, os_atoi(ptr));
  }

  ptr = NULL;
  ptr = prom_getenv("los_alarm");
  if(ptr)
  {
    dslhal_api_disableLosAlarm(pIhw, os_atoi(ptr));
  }

  ptr = NULL;
  ptr = prom_getenv("eoc_vendor_id");
  if(ptr)
  {
    for(i=0;i<8;i++)
    {
      tmp[0]=ptr[i*2];
      tmp[1]=ptr[i*2+1];
      tmp[2]=0;
      EOCVendorID[i] = os_atoh(tmp);
      //printk("tmp=%s--", tmp);
      //printk("ID[%d]=0x%02x ", i, (unsigned char)EOCVendorID[i]);
    }
    tn7dsl_get_dsp_version(dspVer);
    //printk("Annex =%d\n", dspVer[8]);
    if(dspVer[8]==2) // annex b
    {
      //printk("EOCVendorID=%02x %02x %02x %02x %02x %02x %02x %02x\n", EOCVendorID[0], EOCVendorID[1], EOCVendorID[2], EOCVendorID[3],
      //                                                                EOCVendorID[4], EOCVendorID[5], EOCVendorID[6], EOCVendorID[7]);
      dslhal_api_setEocVendorId(pIhw, EOCVendorID);
    }
    
  }
  
  return 0;
}
  


  

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Function: static void tn7dsl_init(void)
 *
 *  Description: This function initializes
 *               Ar7 DSL interface
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int tn7dsl_init(void *priv)
{
  
  printk("Initializing DSL interface\n");
  
  
  /* start dsl */
  if(dslhal_api_dslStartup(&pIhw) !=0 )
  {
    printk("DSL start failed.\n");
    return -1;
  }
 
  // set dsl into overlay page reload mode
  pIhw->bAutoRetrain = 1;

  // set default training properties
  tn7dsl_set_dsl();

  pIhw->pOsContext = priv;

  //start_kthread(tn7dsl_reload_overlay, &overlay_thread);

  /*register dslss LED with led module */
#ifdef CONFIG_LED_MODULE
  tn7dsl_register_dslss_led();
#endif

  
  return 0;  /* What do we return here? */
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *  Function: int avsar_exit(void)
 *
 *  Description: Avalanche SAR exit function
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void tn7dsl_exit (void)
{
 
  bshutdown = 1;
#ifdef CONFIG_LED_MODULE
#ifdef DEREGISTER_LED
  //down(&adsl_sem_overlay);
  deregister_led_drv(LED_NUM_1);
  deregister_led_drv(LED_NUM_2);
#else
//  led_operation(MOD_ADSL,DEF_ADSL_IDLE);
#endif
#endif
  stop_kthread(&overlay_thread);
  dslhal_api_dslShutdown(pIhw);

}


static int tn7dsl_process_oam_string(int *type, int *pvpi, int *pvci, int *pdelay)
{
  int i=1;
  int j=0;
  int vci, vpi;
  char tmp[16];
  int chan;
  int tt;

  while(j<8)
  {
    tmp[j] = mod_req[i];
    //printk("tmp[%d]=%c, %d\n", j, tmp[j], tmp[j]); 
    if(tmp[j] == 0x50 || tmp[j] == 0x70)
      break;
    j++;
    i++;
  }

  tmp[j] = 0;
  vpi = os_atoi(tmp);

  i++;
  j=0;
  while(j<8)
  {
    tmp[j] = mod_req[i];
    //printk("tmp[%d]=%c, %d\n", j, tmp[j], tmp[j]); 
    if(tmp[j] == 0x43 || tmp[j] == 0x63)
      break;

    j++;
    i++;
  }

  vci = os_atoi(tmp);

  if(vci==0) // f4 oam
    *type = 1;
  else
    *type = 0;


  tt=5000;
  i++;
  j=0;
  tmp[j] = mod_req[i];
  if(tmp[j]==0x44 || tmp[j]==0x64)
  {
    i++;
    while(j<8)
    {
      tmp[j] = mod_req[i];

      //printk("tmp[%d]=%c, %d\n", j, tmp[j], tmp[j]); 
      if(tmp[j] == 0x54 || tmp[j] == 0x74)
        break;

      j++;
      i++;
    }
    tt = os_atoi(tmp);
  }

  chan = tn7atm_lut_find(vpi, vci);
  
  *pvci=vci;
  *pvpi=vpi;
  *pdelay =tt;
  dprintf(2, "oam chan=%d, type =%d\n", chan, *type);
 
  return chan;
}

static void tn7dsl_dump_memory(void)
{
  unsigned int *pUi;
  int i=1;
  int j=0;
  int addr, len;
  char tmp[16];


  while(j<8)
  {
    tmp[j] = mod_req[i];
    j++;
    i++;
  }

  tmp[j] = 0;
          
  addr = os_atoh(tmp);

  printk("start dump address =0x%x\n", addr);
  pUi = (unsigned int *)addr;
  i++;
  j=0;
  while(j<8)
  {
    tmp[j] = mod_req[i];
    //printk("tmp[%d]=%c, %d\n", j, tmp[j], tmp[j]); 
    if(tmp[j] == 0x43 || tmp[j] == 0x63)
      break;

    j++;
    i++;
  }

  len = os_atoi(tmp);
  j=0;
  for(i=0; i<len; i++)
  {
    if(j==0)
      printk("0x%08x: ", (unsigned int)pUi);
    printk("%08x ", *pUi);
    pUi++;
    j++;
    if(j==4)
    {
      printk("\n");
      j=0;
    }
  }
     
}



static int dslmod_sysctl(ctl_table *ctl, int write, struct file * filp,
      void *buffer, size_t *lenp)
{
  char *ptr;
  int ret, len = 0;
  int chan;
  int type;
  int vpi,vci,timeout;
  
  if (!*lenp || (filp->f_pos && !write)) {
    *lenp = 0;
    return 0;
    }
  /* DSL MODULATION is changed */
  if(write)
    {
    ret = proc_dostring(ctl, write, filp, buffer, lenp);
    
    switch (ctl->ctl_name)
      {
      case DEV_DSLMOD:
      ptr = strpbrk(info, " \t");
      strcpy(mod_req, info);

      /* parse the string to determine the action */
      if(mod_req[0] == 0x45 || mod_req[0] == 0x65 ) // 'e', or 'E' f5 end to end 
      {
        chan = tn7dsl_process_oam_string(&type, &vpi, &vci, &timeout);
        tn7sar_oam_generation(pIhw->pOsContext, chan, type, vpi, vci, timeout);
      }
      else if(mod_req[0] == 0x53 || mod_req[0] == 0x73 ) // 's', or 'S' f5 seg to seg
      {
        chan=tn7dsl_process_oam_string(&type, &vpi, &vci,&timeout);
        type = type | (1<<1);
        tn7sar_oam_generation(pIhw->pOsContext, chan, type, vpi, vci,timeout);
      }
    //debug only. Dump memory
      else if(mod_req[0] == 0x44 || mod_req[0] == 0x64 ) // 'd' or 'D'
        tn7dsl_dump_memory();
      else
        tn7dsl_chng_modulation(info);
      break;
      }
    }
  else
    {
    len += sprintf(info+len, mod_req);
    ret = proc_dostring(ctl, write, filp, buffer, lenp);
    }
  return ret;
}


ctl_table dslmod_table[] = {
  {DEV_DSLMOD, "dslmod", info, DSL_MOD_SIZE, 0644, NULL, &dslmod_sysctl},
  {0}
  };

/* Make sure that /proc/sys/dev is there */
ctl_table dslmod_root_table[] = {
#ifdef CONFIG_PROC_FS
  {CTL_DEV, "dev", NULL, 0, 0555, dslmod_table},
#endif /* CONFIG_PROC_FS */
  {0}
  };

static struct ctl_table_header *dslmod_sysctl_header;

void tn7dsl_dslmod_sysctl_register(void)
{
  static int initialized;
  
  if (initialized == 1)
    return;
  
  dslmod_sysctl_header = register_sysctl_table(dslmod_root_table, 1);
  dslmod_root_table->child->de->owner = THIS_MODULE;
  
  /* set the defaults */
  info[0] = 0;
  
  initialized = 1;
}

void tn7dsl_dslmod_sysctl_unregister(void)
{
  unregister_sysctl_table(dslmod_sysctl_header);
}

static void 
tn7dsl_set_modulation(void* data)
{
  dprintf(4,"tn7dsl_set_modulation\n");
  
  if(!strcmp(data, "T1413"))
  {
    printk("retraining in T1413 mode\n");
    dslhal_api_setTrainingMode(pIhw, T1413_MODE);
    return;
  }
  if(!strcmp(data, "GDMT"))
  {
    dslhal_api_setTrainingMode(pIhw, GDMT_MODE);
    return;
  }
  if(!strcmp(data, "GLITE"))
  {
    dslhal_api_setTrainingMode(pIhw, GLITE_MODE);
    return;
  }
  if(!strcmp(data, "MMODE"))
  {
    dslhal_api_setTrainingMode(pIhw, MULTI_MODE);
    return;
  }
  if(!strcmp(data, "NMODE"))
  {
    dslhal_api_setTrainingMode(pIhw, NO_MODE);
    return;
  }

  return;
}


/* Codes added for compiling tiadiag.o for Analog Diagnostic tests */
#ifdef ADIAG

enum
{
  HOST_ACTREQ,         // Send R-ACKREQ and monitor for C-ACKx
  HOST_QUIET,          // Sit quietly doing nothing for about 60 seconds, DEFAULT STATE; R_IDLE
  HOST_XMITBITSWAP,    // Perform upstream bitswap - FOR INTERNAL USE ONLY
  HOST_RCVBITSWAP,     // Perform downstream bitswap - FOR INTERNAL USE ONLY
  HOST_RTDLPKT,        // Send a remote download packet - FOR INTERNAL USE ONLY
  HOST_CHANGELED,      // Read the LED settings and change accordingly
  HOST_IDLE,           // Sit quiet
  HOST_REVERBTEST,     // Generate REVERB for manufacturing test
  HOST_CAGCTEST,       // Set coarse receive gain for manufacturing test
  HOST_DGASP,          // send Dying Gasp messages through EOC channel
  HOST_GHSREQ,         // G.hs - FOR INTERNAL USE ONLY
  HOST_GHSMSG,         // G.hs - FOR INTERNAL USE ONLY
  HOST_GHS_SENDGALF,   // G.hs - FOR INTERNAL USE ONLY
  HOST_GHSEXIT,        // G.hs - FOR INTERNAL USE ONLY
  HOST_GHSMSG1,        // G.hs - FOR INTERNAL USE ONLY
  HOST_HYBRID,         // Enable/Disable automatic hybrid switch
  HOST_RJ11SELECT,     // RJ11 inner/outer pair select
  HOST_DIGITAL_MEM,    // Digital Diags: run external memory tests
  HOST_TXREVERB,       // AFE Diags: TX path Reverb
  HOST_TXMEDLEY,       // AFE Diags: TX path Medley
  HOST_RXNOISEPOWER,   // AFE Diags: RX noise power
  HOST_ECPOWER,        // AFE Diags: RX eco power
  HOST_ALL_ADIAG,      // AFE Diags: all major analog diagnostic modes. Host is responsible to initiate each diagnostic sessions
  HOST_USER_ADIAG,     // AFE Diags: Host fills in analog diagnostic input data structure as specified and requests DSP to perform measurements as specified
  HOST_QUIT_ADIAG,     // AFE Diags: Host requests DSP to quit current diagnostic session. This is used for stopping the transmit REVERB/MEDLEY
  HOST_NO_CMD,         // All others - G.hs - FOR INTERNAL USE ONLY
  HOST_DSLSS_SHUTDOWN, // Host initiated DSLSS shutdown message
  HOST_SET_GENERIC,    // Set generic CO profile
  HOST_UNDO_GENERIC    // Set profile previous to Generic
};

enum
{
  DSP_IDLE,               // R_IDLE state entered
  DSP_ACTMON,             // R_ACTMON state entered
  DSP_TRAIN,              // R_TRAIN state entered
  DSP_ACTIVE,             // R_ACTIVE state entered
  DSP_XMITBITSWAP,        // Upstream bitswap complete - FOR INTERNAL USE ONLY
  DSP_RCVBITSWAP,         // Downstream bitswap complete - FOR INTERNAL USE ONLY
  DSP_RTDL,               // R_RTDL state entered - FOR INTERNAL USE ONLY
  DSP_RRTDLPKT,           // RTDL packet received - FOR INTERNAL USE ONLY
  DSP_XRTDLPKT,           // RTDL packet transmitted - FOR INTERNAL USE ONLY
  DSP_ERROR,              // Command rejected, wrong state for this command
  DSP_REVERBTEST,         // Manufacturing REVERB test mode entered
  DSP_CAGCTEST,           // Manufacturing receive gain test done
  DSP_OVERLAY_START,      // Notify host that page overlay has started - overlay number indicated by "tag"
  DSP_OVERLAY_END,        // Notify host that page overlay has ended   - overlay number indicated by "tag"
  DSP_CRATES1,            // CRATES1 message is valid and should be copied to host memory now
  DSP_SNR,                // SNR calculations are ready and should be copied to host memory now
  DSP_GHSMSG,             // G.hs - FOR INTERNAL USE ONLY
  DSP_RCVBITSWAP_TIMEOUT, // Acknowledge Message was not received within ~500 msec (26 Superframes).
  DSP_ATM_TC_SYNC,        // Indicates true TC sync on both the upstream and downstream. Phy layer ready for data xfer.
  DSP_ATM_NO_TC_SYNC,     // Indicates loss of sync on phy layer on either US or DS.
  DSP_HYBRID,             // DSP completed hybrid switch
  DSP_RJ11SELECT,         // DSP completed RJ11 inner/outer pair select
  DSP_INVALID_CMD,        // Manufacturing (Digital and AFE) diags: CMD received not recognized
  DSP_TEST_PASSED,        // Manufacturing diags: test passed
  DSP_TEST_FAILED,        // Manufacturing diags: test failed
  DSP_TXREVERB,           // Manufacturing AFE diags: Response to HOST_TXREVERB
  DSP_TXMEDLEY,           // Manufacturing AFE diags: Response to HOST_TXMEDLEY
  DSP_RXNOISEPOWER,       // Manufacturing AFE diags: Response to HOST_RXNOISEPOWER
  DSP_ECPOWER,            // Manufacturing AFE diags: Response to HOST_ECPOWER
  DSP_ALL_ADIAG,          // Manufacturing AFE diags: Response to HOST_ALL_ADIAG
  DSP_USER_ADIAG,         // Manufacturing AFE diags: Response to HOST_USER_ADIAG
  DSP_QUIT_ADIAG,         // Manufacturing AFE diags: Response to HOST_QUIT_ADIAG
  DSP_DGASP               // DSP Message to indicate dying gasp
};

static unsigned char analogNoTonesTestArray[64]=
  {
    0,0,0,0,0,0,0,0,  // Tones 01-08
    0,0,0,0,0,0,0,0,  // Tones 09-16
    0,0,0,0,0,0,0,0,  // Tones 17-24
    0,0,0,0,0,0,0,0,  // Tones 25-32
    0,0,0,0,0,0,0,0,  // Tones 33-40
    0,0,0,0,0,0,0,0,  // Tones 41-48
    0,0,0,0,0,0,0,0,  // Tones 49-56
    0,0,0,0,0,0,0,0   // Tones 57-64
  };

static unsigned char analogAllTonesTestArray[64]=
  {
    1,1,1,1,1,1,1,1,  // Tones 01-08
    1,1,1,1,1,1,1,1,  // Tones 09-16
    1,1,1,1,1,1,1,1,  // Tones 17-24
    1,1,1,1,1,1,1,1,  // Tones 25-32
    1,1,1,1,1,1,1,1,  // Tones 33-40
    1,1,1,1,1,1,1,1,  // Tones 41-48
    1,1,1,1,1,1,1,1,  // Tones 49-56
    1,1,1,1,1,1,1,1   // Tones 57-64
  };

static unsigned char analogEvenTonesTestArray[64]=
  {
    0,1,0,1,0,1,0,1,  // Tones 01-08
    0,1,0,1,0,1,0,1,  // Tones 09-16
    0,1,0,1,0,1,0,1,  // Tones 17-24
    0,1,0,1,0,1,0,1,  // Tones 25-32
    0,1,0,1,0,1,0,1,  // Tones 33-40
    0,1,0,1,0,1,0,1,  // Tones 41-48
    0,1,0,1,0,1,0,1,  // Tones 49-56
    0,1,0,1,0,1,0,1   // Tones 57-64
  };

static unsigned char analogOddTonesTestArray[64]=
  {
    1,0,1,0,1,0,1,0,  // Tones 01-08
    1,0,1,0,1,0,1,0,  // Tones 09-16
    1,0,1,0,1,0,1,0,  // Tones 17-24
    1,0,1,0,1,0,1,0,  // Tones 25-32
    1,0,1,0,1,0,1,0,  // Tones 33-40
    1,0,1,0,1,0,1,0,  // Tones 41-48
    1,0,1,0,1,0,1,0,  // Tones 49-56
    1,0,1,0,1,0,1,0   // Tones 57-64
  };

unsigned int shim_osGetCpuFrequency(void)
{
  return 150;
}

static void tn7dsl_adiag(int Test, unsigned char *missingTones)
{
  int rc,cmd, tag;

  rc = dslhal_diags_anlg_setMissingTones(pIhw,missingTones);
  if(rc)
  {
    printk(" failed to set Missing town\n");
    return;
  }

/*********** Start the actual test **********************/

 if(Test==0)
   {
   printk("TX REVERB Test\n");
   rc = dslhal_support_writeHostMailbox(pIhw, HOST_TXREVERB, 0, 0, 0);
   if (rc)
     {
     printk("HOST_TXREVERB failed\n");
     return;
     }

   }
 if(Test==1)
   {
     dprintf(0,"TX MEDLEY Test\n");
     rc = dslhal_support_writeHostMailbox(pIhw, HOST_TXMEDLEY, 0, 0, 0);
     if (rc)
       return;
   }
 dprintf(4,"dslhal_diags_anlg_testA() done\n");
 return;
}
  

static void tn7dsl_diagnostic_test(char *data)
{
  if(!strcmp(data, "ADIAGRALL"))
  {
    printk("TX Reverb All tone\n");
    tn7dsl_adiag(0,analogAllTonesTestArray);
    return;
  }
  if(!strcmp(data, "ADIAGRNONE"))
  {
    printk("TX Reverb No tone\n");
    tn7dsl_adiag(0,analogNoTonesTestArray);
    return;
  }
  if(!strcmp(data, "ADIAGREVEN"))
  {
    printk("TX Reverb Even tone\n");
    tn7dsl_adiag(0,analogEvenTonesTestArray);
    return;
  }
  if(!strcmp(data, "ADIAGRODD"))
  {
    printk("TX Reverb Odd tone\n");
    tn7dsl_adiag(0,analogOddTonesTestArray);
    return;
  }
  if(!strcmp(data, "ADIAGMALL"))
  {
    printk("TX Mdelay All tone\n");
    tn7dsl_adiag(1,analogAllTonesTestArray);
    return;
  }
  if(!strcmp(data, "ADIAGMNONE"))
  {
    printk("TX Mdelay No tone\n");
    tn7dsl_adiag(1,analogNoTonesTestArray);
    return;
  }
  if(!strcmp(data, "ADIAGMEVEN"))
  {
    printk("TX Mdelay Even tone\n");
    tn7dsl_adiag(1,analogEvenTonesTestArray);
    return;
  }
  if(!strcmp(data, "ADIAGMODD"))
  {
    printk("TX Mdelay Odd tone\n");
    tn7dsl_adiag(1,analogOddTonesTestArray);
    return;
  }
  if(!strcmp(data, "ADIAGQUIET"))
  {
    dslhal_api_sendIdle(pIhw);
    return;
  }
  if(!strncmp(data, "ADIAGRN", 7))
  {
    char tones[64], tmp[4];
    int  nth, i;
    
    tmp[0]=data[7];
    tmp[1]=data[8];
    tmp[2]=data[9];
    
    nth = os_atoi(tmp);

    for(i=0;i<64;i++)
    {
      if(((i+1)% nth)==0)
      {
        tones[i]=0;
      }
      else
      {
        tones[i]=1;
      }
    }
    printk("TX Reverb with %dth tones missing.\n", nth);
    tn7dsl_adiag(0,tones);
    return;
  }
  if(!strncmp(data, "ADIAGMN", 7))
  {
    char tones[64], tmp[4];
    int  nth, i;

    tmp[0]=data[7];
    tmp[1]=data[8];
    tmp[2]=data[9];
    nth = os_atoi(tmp);

    for(i=0;i<64;i++)
    {
      if(((i+1)% nth)==0)
      {
        tones[i]=0;
      }
      else
      {
        tones[i]=1;
      }
    }
    printk("TX Mdelay with %dth tones missing.\n", nth);
    tn7dsl_adiag(1,tones);
    return;
  }


}

#endif

static void 
tn7dsl_chng_modulation(void* data)
{
  //printk("DSL Modem Retraining\n");
  
  if(!strcmp(data, "T1413"))
  {
    printk("retraining in T1413 mode\n");
    dslhal_api_setTrainingMode(pIhw, T1413_MODE);
    dslhal_api_sendQuiet(pIhw);
    return;
  }
  if(!strcmp(data, "GDMT"))
  {
    dslhal_api_setTrainingMode(pIhw, GDMT_MODE);
    dslhal_api_sendQuiet(pIhw);
    return;
  }
  if(!strcmp(data, "GLITE"))
  {
    dslhal_api_setTrainingMode(pIhw, GLITE_MODE);
    dslhal_api_sendQuiet(pIhw);
    return;
  }
  if(!strcmp(data, "MMODE"))
  {
    dslhal_api_setTrainingMode(pIhw, MULTI_MODE);
    dslhal_api_sendQuiet(pIhw);
    return;
  }
  if(!strcmp(data, "NMODE"))
  {
    dslhal_api_setTrainingMode(pIhw, NO_MODE);
    dslhal_api_sendQuiet(pIhw);
    return;
  }

#ifdef ADIAG
  tn7dsl_diagnostic_test(data);
#endif


  return;
}

#ifdef CONFIG_LED_MODULE
static void tn7dsl_led_on(unsigned long parm)
{
  dslhal_api_configureLed(pIhw,parm, 0);
}


static void tn7dsl_led_off(unsigned long parm)
{
  dslhal_api_configureLed(pIhw,parm, 1);
}

static void tn7dsl_led_init(unsigned long parm)
{
  dslhal_api_configureLed(pIhw,parm, 2);
}
#endif

static void tn7dsl_register_dslss_led(void)
{
#ifdef CONFIG_LED_MODULE
    
    // register led0 with led module
    ledreg[0].param = 0;
    ledreg[0].init = (void *)tn7dsl_led_init;
    ledreg[0].onfunc = (void *)tn7dsl_led_on;
    ledreg[0].offfunc = (void *)tn7dsl_led_off;
    register_led_drv(LED_NUM_1, &ledreg[0]);

    // register led1 output with led module
    ledreg[1].param = 1;
    ledreg[1].init = (void *)tn7dsl_led_init;
    ledreg[1].onfunc = (void *)tn7dsl_led_on;
    ledreg[1].offfunc = (void *)tn7dsl_led_off;
    register_led_drv(LED_NUM_2, &ledreg[1]);
#endif  
}

static int tn7dsl_reload_overlay(void)
{
  int overlayFlag;
  spinlock_t overlayLock;

  init_kthread(&overlay_thread, "adsl");
  down(&adsl_sem_overlay);
  while(1)
  {
    mdelay(500);
    if(pIhw->lConnected == 0)
    {
      spin_lock_irqsave(&overlayLock, overlayFlag);
      dslhal_support_restoreTrainingInfo(pIhw);
      spin_unlock_irqrestore(&overlayLock, overlayFlag);
    }
    down(&adsl_sem_overlay);
  }
  return 0;
}




