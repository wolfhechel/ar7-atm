/************************************************************************************************
 *  tn7dsl.c
 *  Avalanche DSL driver
 *
 *   Zhicheng Tang, ztang@ti.com
 *   2000 (c) Texas Instruments Inc.
 *
 *  3/23/05 CPH  Change dslhal_api_setMaxBitsPerCarrier to dslhal_api_setMaxBitsPerCarrierUpstream.
 *               Add "dslctl" to read Phy Feature Control bit.
 *  5/10/05 AV   Added tn7dsl_proc_train_mode_export(). (CQ9605).
 *  5/24/05 AV   Fixing the incorrect parsing of the DSL_BIT_TMODE environment variable after the
 *               the modulation environment variable.
 *  6/02/05 AV   Changing the location of the tn7dsl_set_modulation function to overcome the
 *               degradation in throughput. (MR-2464).
 *  6/2/05  SANJAY Added a print in case of failue of DSL Init. Rearranged the code in case of
 *                 Failure so that cat on proc file doesnt give Seg. fault.
 *  8/8/05  CPH  CQ9600 (partial) AnxBM support
 *  8/10/05 CPH  CQ9776 (partial) TR69 support
 *  8/16/05 CPH  Added nohost_flag
 *  8/19/05 JZ   Returning error code in dslmod_sysctl function.(SR 1-4767201), CQ # 9861.
 *  8/20/05 AV   Modified to use the function to get the capability of the chip,
 *               tn7atm_read_can_support_adsl2. Relocated structure dsl_modes for performance.
 *  8/24/05 CPH  CQ9885: Added clearEoc support for ADSL2/2+
 *  9/5/05  CPH  CQ9660 (partial TR69) SNRpsds, QLNpsds reporting updated.
 *  9/13/05 AV   Replaced some of the shim calls with direct calls for performance. The shim
 *               calls are left in comments for reference.
 *  09/21/05 AV  Renamed the overloaded memcmp and memcpy functions to
 *               tn7atm_memcmp and tn7atm_memcpy respectively to allow support
 *               for being compiled directly into the kernel.
 *   9/22/05 AV  Adding support for the new LED driver in BasePSP 7.4. A new macro TN7DSL_LED_ACTION
 *               has been used to replace the direct calls to the old LED handler.
 *  7/10/05 AV/CPH Changed dprintf to dgprintf and compile time selection of dslhal_api_getQLNpsds()
 *                 dslhal_api_getSNRpsds() and for code size reduction.
 * 10/11/05  JZ   Fix for auto PVC oam ping lost case, changed the type value for 'S' - segment
 *                oam ping to have correct oam cell type.
 * 10/17/05  AV   Enabling the logic to use prom_setenv() and prom_unsetenv() in BasePSP 7.4, for
 *                clearing the "modulation" env variable, when the user upgrades to the new
 *                bit mode scheme.
 * 10/19/05  AV   Removing the prototypes for prom_setenv() and prom_unsetenv(), as they conflict
 *                with the ones in the asm/mips-boards/prom.h file.
 * 11/03/05  MK  CQ10037: Added invntry_vernum environment variable to be able to set version number
 *               in ADSL2/ADSL2+ modes.
 *  11/3/05 CPH  CQ9885 Added ClerarEoc ACK (porting).
 *  11/3/05  JZ  Added code to convert old mode to new bit mode in tn7dsl_set_modulation()
 *  1/16/06 CPH  CQ10269 Fixed tn7dsl_clear_eoc_close() crash problem
 *  01/25/06 JZ   CQ: 10273 Aztech/Singtel oam pvc management issue, new PDSP 0.54
 *  01/25/06 JZ   CQ: 10275 Add Remote Data Log code. Search ADV_DIAG_STATS in ATM
 *                driver code and manually turn on it for enabling the feature
 *  UR8_MERGE_START Report_SES Manjula K
 *  03/14/06 MK  CQ10369: Added print statement to report errored Seconds and severely errored secs.
 *  UR8_MERGE_END  Report_SES
 *  UR8_MERGE_START CQ10449 Jack Zhang
 *  04/04/06 JZ  CQ10449: Enhanced remote data log for showtime SNR for ADSL2/2+ mode
 *  UR8_MERGE_END  CQ10449
 * UR8_MERGE_START CQ10450   Jack Zhang
 * 4/04/06  JZ   CQ10450: Increase Interrupt pacing to 6 when downstream connection rate is higher than 20Mbps
 * UR8_MERGE_END   CQ10450
 * UR8_MERGE_START CQ10505 ManjulaK
 * 05/19/06 MK   CQ10505: Report vendor id through inventory command in ADSL2/2+ mode
 * UR8_MERGE_END CQ10505
 * UR8_MERGE_START CQ10442 Manjula K
 * 05/31/06 MK   CQ10442: Added code to boost interrupt pacing to 6 when SRA occurs and DS connect rate jumps
 *                        higher than 20Mbps.
 * UR8_MERGE_END CQ10442
 * UR8_MERGE_START CQ10639 Manjula K
 * 06/02/06 MK   CQ10639: Setting interrupt pacing to default when DS connect rate drops less than 20Mbps and
 *                        modem is not connected in AnnexM mode.
 * UR8_MERGE_END CQ10639
 * UR8_MERGE_START CQ10682   Jack Zhang
 * 6/15/06  JZ   CQ10682: Display ADSL training messages in ATM driver proc file system
 * UR8_MERGE_END   CQ10682*
 * UR8_MERGE_START CQ10640   Jack Zhang
 * 7/10/06  JZ   CQ10640: DSP memory dump function and memory dump format incorrect
 * UR8_MERGE_END   CQ10640*
 * UR8_MERGE_START CQ10715 Manjula K
 * 07/20/06 MK   CQ10715: Corrected DS Max Attainable Bit Rate calculation in ADSL1 mode.
 * UR8_MERGE_END CQ10715
 * UR8_MERGE_START CQ10700 Manjula K
 * 07/24/06 MK   CQ10700: Added counters for reporting packets dropped by ATM Driver/SAR.
 * UR8_MERGE_END CQ10700
 * UR8_MERGE_START CQ10880   Jack Zhang
 * 8/30/06  JZ   CQ10880: Add DHAL api for sending mailbox message for L3
 * UR8_MERGE_END   CQ10880*
*  UR8_MERGE_START CQ10978   Jack Zhang
*  10/4/06  JZ     CQ10978: Request for the DSL Power Management Status Report
*  UR8_MERGE_END   CQ10978*
*  UR8_MERGE_START CQ10979   Jack Zhang
*  10/4/06  JZ     CQ10979: Request for TR-069 Support for RP7.1
*  UR8_MERGE_END   CQ10979*
*  UR8_MERGE_START CQ11057   Jack Zhang
*  11/03/06 JZ     CQ11057: Request US PMD test parameters from CO side
*  UR8_MERGE_END   CQ11057*
*  UR8_MERGE_START CQ11054   Jack Zhang
*  1/02/07  JZ     CQ11054: Data Precision and Range Changes for TR-069 Conformance
*  UR8_MERGE_END   CQ11054*
*  UR8_MERGE_START CQ11579 Jeremy : #1 - corrected TxPower to allow negative # reporting
*  04/02/07                         #2 - corrected T1413 MaxAttainableRate
*  UR8_MERGE_END CQ11579
*  UR8_MERGE_START CQ11813 Hao-Ting Lin
*  06/11/07 Hao-Ting CQ11813 CLI redirect support in linux
*                            Add debug message Init and Print function for /proc entry
*  UR8_MERGE_END   CQ11813
*  09/18/07 CPH   CQ11466: Added EFM support.
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
#include <linux/sysctl.h>
#include <linux/timer.h>
#include <linux/vmalloc.h>
#include <linux/file.h>
/* Modules specific header files */
#ifdef AR7_EFM
#include "tn7efm.h"
#endif

#include "tn7atm.h"
#include "tn7api.h"
#include "dsl_hal_api.h"
#include "dsl_hal_support.h"
//UR8_MERGE_START CQ10450   Jack Zhang
#define _SIZE_T_
#include "dsl_hal_register.h"
//UR8_MERGE_END   CQ10450*


/* For the changes for trainning modes using bit fields.
   Defaults are for backward compatibility.
*/
#define OLD_TRAINING_VAL_GDMT   3
#define OLD_TRAINING_VAL_T1413  2
#define OLD_TRAINING_VAL_MMODE  1

#define NEW_TRAINING_VAL_GDMT   2
#define NEW_TRAINING_VAL_T1413  128
#define NEW_TRAINING_VAL_MMODE  255

int testflag1 = 0;
extern int  __guDbgLevel;
extern sar_stat_t sarStat;
extern unsigned int  SAR_FREQUNCY;
static int dslInSync = 0;
static int inter_pace_boosted = 0; //UR8_MERGE_START_END CQ10442 Manjula K
static int bMarginThConfig;
static int bMarginRetrainEnable;
static char EOCVendorID[8] =
  { 0xb5, 0x00, 0x54, 0x53, 0x54, 0x43, 0x00, 0x00 };
static int trellis = 1;
static int nohost_flag =0;

static unsigned int dslReg;

#define CLEAREOC_ACK    0x80
#define CEOC_RESP     0x02  // define clearEoc Ctrl as in G.992.3 sec7.8.2.4.1
#define CEOC_CMD      0x00

static int eocctrl_resp=0;
static int eocctrl_cmd=0;
#define TC_SYNC          1
#define SYNC_TIME_DELAY  500000

/* Common LED variables. */
#if defined (CONFIG_MIPS_AVALANCHE_COLORED_LED) || defined (CONFIG_LED_MODULE)
static int led_on;
/* Definition of the handle for LED module. They will be NULL for old LED implementation. */
void *hnd_LED_0 = NULL;
#endif

#if defined (CONFIG_LED_MODULE)
led_reg_t ledreg[2];
#endif

#if defined (CONFIG_MIPS_AVALANCHE_COLORED_LED)
/*since the structure has the same elements as led_reg_t, we are re-using the variable name. */
static struct led_funcs ledreg[2];
#endif

#define DEV_DSLMOD       1
#define MAX_STR_SIZE     256
#define DSL_MOD_SIZE     256

#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#define tn7dsl_kfree_skb(x)      dev_kfree_skb(x)


//---------------------------------------------
// Begin Clear EOC definitions
//---------------------------------------------
//#define MAX_CLEAR_EOC_BUF_SIZE   600       // moved to <tn7api.h>
#define MAX_RAW_CLEAR_EOC_MSGSIZE  514       // see ITU G997.1

#define HDLC_INIT      0xFFFF
#define HDLC_FLAG      0x7E
#define HDLC_ESC       0x7D
#define HDLC_INV       0x20
#define HDLC_GENPOL    0x8408
#define HDLC_GOOD      0xF0B8

typedef struct
{
  unsigned int  len;                               // packet length
  unsigned char   data[MAX_CLEAR_EOC_BUF_SIZE];      // data field
} clearEocBufDesc_type;

/* *INDENT-OFF* */
typedef struct
{
  unsigned int            clearEocEnabled;   // 0: disabled; 1: enabled
  clearEocBufDesc_type   *pTxBufDesc[4];     // Tx buffer desc pointer array
  clearEocBufDesc_type   *pRxBufDesc[4];     // Rx buffer desc pointer array
  unsigned int            txRdIndex;         // host write
  unsigned int            txWrIndex;         // DSP write
  unsigned int            rxRdIndex;         // DSP write
  unsigned int            rxWrIndex;         // host write
} clearEocParm_t;
/* *INDENT-ON* */

//L2 Buffer Struct Type
typedef struct {
  unsigned short L2UBufInInx;              //L2 module write, OVHD module read only
  unsigned short L2UBufOutInx;             //OVHD module write, L2 module read only
  unsigned short L2DBufInInx;              //OVHD module write, L2 module read only
  unsigned short L2DBufOutInx;             //L2 module write, OVHD module read only
  unsigned char  L2StatusFlag;             //rcv L2 status Flag
  unsigned char  dummy[3];
  unsigned char  *L2UBuf;                   //L2 upstream buffer
  unsigned char  *L2DBuf;                   //L2 downstream buffer
  unsigned short L2TxBufSize;              //L2  transmit buffer size
  unsigned short L2RxBufSize;              //L2  receive buffer size
} DI_L2_BufDef_t;

typedef DI_L2_BufDef_t DEV_HOST_L2clearEocDef_t;
static clearEocParm_t    ClrEoc;
static clearEocParm_t   *pClrEOC = &ClrEoc;
static int   EocRxTotalPackets;
static int   EocTxTotalPackets;
static int   EocRxTotalBytes;
static int   EocTxTotalBytes;
static int   ErrEocBufFull;
static int   ErrEocBufIndex;
static int   ErrEocBufMax;
static int   ErrEocMsgOversized;
static int   ErrEocTxHdlcCRC;
static int   ErrEocRxHdlcCRC;
static int   ErrEocRxHdlcFraming;
static int   ErrEocRxPush;

//CQ10269 cph 1/16/06 Fixed clearEoc Crash problem
static clearEocBufDesc_type   *pTxBufDescSave[4];     // Tx buffer desc pointer array
static clearEocBufDesc_type   *pRxBufDescSave[4];
//--------------------------------------------
// End of Clear EOC definitions
//--------------------------------------------

#define host2dspAddrMapping(a)    (((int)a)&~0xe0000000)


/* *INDENT-OFF* */
/* Old enum values for backward compatibility */
enum
{
  NO_MODE           = 0,
  MULTI_MODE        = 1,
  T1413_MODE        = 2,
  GDMT_MODE         = 3,
  GLITE_MODE        = 4,
  ADSL2_MODE        = 8,
  ADSL2_DELT        = 9,
  ADSL2_PLUS         = 0x10,
  ADSL2_PLUS_DELT    = 0x11,
  READSL_MODE        = 0x20,
  READSL_PLUS_DELT   = 0x21,
  ADSL2_ANNEX_I      = 0x100,   //Not supported yet
  ADSL2_ANNEX_J      = 0x200,   //Not supported yet
  ADSL2_ANNEX_M      = 0x400,
  ADSL2_PLUS_ANNEX_I = 0x800,   //Not supported yet
  ADSL2_PLUS_ANNEX_J = 0x1000,  //Not supported yet
  ADSL2_PLUS_ANNEX_M = 0x2000
};
/* *INDENT-ON* */

#ifndef ADIAG
#ifdef AR7_EFM
unsigned char DSP_FIRMWARE_PATH[]= "/lib/modules/ar0700xx.bin";
#define DSP_DEBUG_FIRMWARE_PATH  "/var/ar0700xx.bin"
#else
#define DSP_FIRMWARE_PATH "/lib/modules/ar0700xx.bin"
#endif
#else
#define DSP_FIRMWARE_PATH "/var/tmp/ar0700xx_diag.bin"
#endif

/* externs */
extern struct atm_dev *mydev;
extern unsigned int oamFarLBCount[4];

/* module wide declars */
static PITIDSLHW_T    pIhw;
static volatile int bshutdown;
static char info[MAX_STR_SIZE];
/* Used for DSL Polling enable */
static DECLARE_MUTEX_LOCKED (adsl_sem_overlay);

//kthread_t overlay_thread;
/* end of module wide declars */

/* Internal Functions */
static void tn7dsl_chng_modulation(void* data);
static unsigned int tn7dsl_set_modulation(void* data, int flag);
static void tn7dsl_ctrl_fineGain(int value);
static void tn7dsl_set_fineGainValue(int value);
static int dslmod_sysctl (ctl_table * ctl, int write, struct file *filp,
                          void *buffer, size_t * lenp);
static void tn7dsl_register_dslss_led(void);
void tn7dsl_dslmod_sysctl_register(void);
void tn7dsl_dslmod_sysctl_unregister(void);
static int tn7dsl_clear_eoc_receive(void);
static int tn7dsl_proc_snr_print (char *buf, int count, int *eof, int data);
/* end of internal functions */

//  UR8_MERGE_START CQ11054   Jack Zhang
#define gInt(a) ((int)a/10)
#define gDot1(a) ((a>0)?(a%10):((-a)%10))
//  UR8_MERGE_END   CQ11054*
#ifdef AR7_EFM
void *tn7dsl_get_pIhw(void);
void *tn7dsl_get_pIhw(void)
{
  return pIhw;
}

void tn7dsl_disable_alarm(void)
{
    unsigned int alarmEnable=0;

  // disable alarm
    dslhal_api_setMarginMonitorFlags(pIhw,1,alarmEnable);

    dslhal_api_disableLosAlarm(pIhw,!alarmEnable);
}

#endif

//cph 9/6/07 Automatically detect 0x or not 0x and pass to atoh & atoi accordingly.
int os_atoih (const char *pstr)
{
  int val;
  if((pstr[0] == '0') && ((pstr[1] == 'x') || (pstr[2] == 'X')))
    val = os_atoh(&pstr[2]);
  else
    val = os_atoi(pstr);
  return val;
}

int os_atoi(const char *pStr)
{
  int MulNeg = (*pStr == '-' ? -1 : 1);

  return (MulNeg * (simple_strtoul(pStr, (char **)NULL, 10)));
}


int os_atoh(const char *pStr)
{
  int MulNeg = (*pStr == '-' ? -1 : 1);

  return (MulNeg * (simple_strtoul(pStr, (char **)NULL, 16)));
}

unsigned long os_atoul(const char *pStr)
{
  return((unsigned long) simple_strtoul(pStr, (char **)NULL, 10));
}

void dprintf (int uDbgLevel, char *szFmt, ...)
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

int strcmp(const char *s1, const char *s2)
{

  int size = strlen(s1);

  return(strncmp(s1, s2, size));
}

int strncmp(const char *s1, const char *s2, size_t size)
{
  int i = 0;
  int max_size = (int)size;

  while((s1[i] != 0) && i < max_size)
  {
    if(s2[i] == 0)
    {
      return -1;
    }
    if(s1[i] != s2[i])
    {
       return 1;
    }
    i++;
  }
  if(s2[i] != 0)
  {
    return 1;
  }

  return 0;
}

// * UR8_MERGE_START CQ10640   Jack Zhang
int tn7dsl_dump_dsp_memory(char *input_str) //cph99
  {
    int i;
    volatile int addr;
    volatile int wrd_to_read;
    char tmp[16];
    unsigned int buf[20];

    /* Read the first 8 characters for the address */
    for(i=0; i<8; i++)
      tmp[i] = input_str[i];

    tmp[i] = '\0';
    addr = os_atoh(tmp);
    /* Read the remaining characters as the value */
    wrd_to_read = os_atoh(&input_str[8]);

    /* Read back the values from the memory location */
    printk("Reading %d words from dsp address = 0x%08x\n", wrd_to_read, addr);

    dslhal_support_blockRead((void *) addr, &buf, wrd_to_read*4);

    for(i = 0; i < wrd_to_read; i++)
    {
      if(!(i % 4))
      {
        printk("\n%08x: ", addr);
        addr+=16;
      }
      printk("%08x ", buf[i]);
    }
    printk("\n");

    return(4 * i);
}
// * UR8_MERGE_END   CQ10640*

unsigned int shim_osGetCpuFrequency(void)
{
  char *cp;
  unsigned int CpuFrequency;

  dgprintf(6, "shim_osGetCpuFrequency()\n");
  cp=(char *)prom_getenv("cpufrequency");
  CpuFrequency=os_atoi(cp);
  return CpuFrequency;
}

int shim_osLoadFWImage(unsigned char *ptr)
{
  unsigned int bytesRead;
  mm_segment_t  oldfs;
  static struct file *filp;
  unsigned int imageLength=0x5ffff;

#ifdef AR7_EFM
  int dp_alt=0;
  char *ptr1=NULL;
#ifdef EFM_DEBUG  
  char *ptr2=NULL;
  char *ptr3=NULL;
#endif    

  if ((ptr1 = prom_getenv("DSL_DP_ALT")) != NULL)
  {
    dp_alt=os_atoi(ptr1);
    if (dp_alt==1)
    {
      filp = filp_open(DSP_DEBUG_FIRMWARE_PATH,00,O_RDONLY);
      if (!IS_ERR(filp))
      {
          strcpy (DSP_FIRMWARE_PATH, DSP_DEBUG_FIRMWARE_PATH);
      }
    }
#ifdef EFM_DEBUG    
    else if (dp_alt==2)
    {
      if ((ptr2 = prom_getenv("DSL_DP")) != NULL)
      {
        if (!strncmp(ptr2, "DSL_DP", 6))
        { // indirect naming
          if ((ptr3 = prom_getenv(ptr2)) != NULL)
          filp = filp_open(ptr3,00,O_RDONLY);
          ptr2 = ptr3; // redirect ptr2 to ptr3
        }

        filp = filp_open(ptr2,00,O_RDONLY);
        if (!IS_ERR(filp))
        {
          strcpy (DSP_FIRMWARE_PATH, ptr2);
        }
      }
    }
    printk("dp_path=%s\n", DSP_FIRMWARE_PATH);
#endif    
  }
#endif

  dgprintf(4, "tn7dsl_read_dsp()\n");

  dgprintf(4,"open file %s\n", DSP_FIRMWARE_PATH);

  filp=filp_open(DSP_FIRMWARE_PATH,00,O_RDONLY);
  if(IS_ERR(filp))
  {
    printk("Failed: Could not open DSP binary file\n");
          return -1;
  }

  if (filp->f_dentry != NULL)
  {
    if (filp->f_dentry->d_inode != NULL)
    {
      printk ("DSP binary filesize = %d bytes\n",
              (int) filp->f_dentry->d_inode->i_size);
      imageLength = (unsigned int)filp->f_dentry->d_inode->i_size + 0x200;
    }
  }

  if (filp->f_op->read==NULL)
          return -1;  /* File(system) doesn't allow reads */

  /*
   * Disable parameter checking
   */
  oldfs = get_fs();
  set_fs(KERNEL_DS);

  /*
   * Now read bytes from postion "StartPos"
   */
  filp->f_pos = 0;

  bytesRead = filp->f_op->read(filp,ptr,imageLength,&filp->f_pos);

  dgprintf(4,"file length = %d\n", bytesRead);

  set_fs(oldfs);

  /*
   * Close the file
   */
  fput(filp);

  return bytesRead;
}


unsigned int shim_read_overlay_page (void *ptr, unsigned int secOffset,
                                     unsigned int secLength)
{
  unsigned int bytesRead;
  mm_segment_t  oldfs;
  struct file *filp;

  dgprintf(4,"shim_read_overlay_page\n");
  //dgprintf(4,"sec offset=%d, sec length =%d\n", secOffset, secLength);

  filp=filp_open(DSP_FIRMWARE_PATH,00,O_RDONLY);
  if(filp ==NULL)
  {
    printk("Failed: Could not open DSP binary file\n");
          return -1;
  }

  if (filp->f_op->read==NULL)
          return -1;  /* File(system) doesn't allow reads */

  /*
   * Now read bytes from postion "StartPos"
   */

  if(filp->f_op->llseek)
    filp->f_op->llseek(filp,secOffset, 0);
  oldfs = get_fs();
  set_fs(KERNEL_DS);
  filp->f_pos = secOffset;
  bytesRead = filp->f_op->read(filp,ptr,secLength,&filp->f_pos);

  set_fs(oldfs);
  /*
   * Close the file
   */
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
  //printk("size=%d\n", size);
  return ptr;

}


void shim_osFreeMemory(void *ptr, unsigned int size)
{

  kfree(ptr);
}

void shim_osFreeDmaMemory(void *ptr, unsigned int size)
{
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
  tn7atm_memcpy(dst, src, numBytes);
}

void shim_osZeroMemory(char *dst, unsigned int numBytes)
{
  memset(dst, 0, numBytes);
}

/* AV: Moved this definition to a central location. */
void shim_osWriteBackCache(void *addr, unsigned int size)
{
  tn7atm_data_writeback(addr, size);
}

/*
void shim_osInvalidateCache(void *addr, unsigned int size)
{
  unsigned int i,Size=(((unsigned int)addr)&0xf)+size;
  if(!addr)
    return;
  for (i=0;i<Size;i+=16,addr+=16)
  {
    __asm__(" .set mips3 ");
    __asm__("cache  17, (%0)" : : "r" (addr));
    __asm__(" .set mips0 ");
  }
}
*/
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

unsigned int shim_osClockTick(void)
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

static int tn7dsl_proc_snr_print (char *buf, int count, int *eof, int data)
{

  int len = 0;
  int limit = count - 80;
  int i, j;
  int bin = (int) data;
  unsigned short *rxSnrPerBin;

  /* Which bin to use */
  switch (bin)
    {
  case 0:
      rxSnrPerBin = pIhw->AppData.rxSnrPerBin0;
      break;

  case 1:
      rxSnrPerBin = pIhw->AppData.rxSnrPerBin1;
      break;

   case 2:
      rxSnrPerBin = pIhw->AppData.rxSnrPerBin2;
      break;

  default:
  if(len<=limit)
        len += sprintf (buf + len, "\nInvalid bin selected Bin%d :\n", bin);
  return len;
}

  if(len<=limit)
    len += sprintf (buf + len, "\nAR7 DSL Modem Rx SNR Per Bin for Bin%d :\n", bin);

  for (i=0; i<pIhw->AppData.max_ds_tones/16; i++)
  {
    for(j=0;j<16;j++)
    {
      if(len <=limit)
        len +=
          sprintf (buf + len, "%04x ",
                   (unsigned short) rxSnrPerBin[i * 16 + j]);
  }
      if(len <=limit)
        len += sprintf(buf+len, "\n");
    }

  return len;
}


//@Added SNR per bin info per customer request. 05-14-2004
int tn7dsl_proc_snr0 (char *buf, char **start, off_t offset, int count,
                      int *eof, void *data)
    {
  return tn7dsl_proc_snr_print(buf, count, eof, 0);
    }

int tn7dsl_proc_snr1 (char *buf, char **start, off_t offset, int count,
                      int *eof, void *data)
{
  return tn7dsl_proc_snr_print(buf, count, eof, 1);
  }

int tn7dsl_proc_snr2 (char *buf, char **start, off_t offset, int count,
                      int *eof, void *data)
{
  return tn7dsl_proc_snr_print(buf, count, eof, 2);
}

//@Added bit allocation table per customer request. 05-14-2004
int tn7dsl_proc_bit_allocation (char *buf, char **start, off_t offset,
                                int count, int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;
  int i, j;

  if(len<=limit)
    len += sprintf(buf+len, "\nAR7 DSL Modem US Bit Allocation:");

  for(i=0; i<pIhw->AppData.max_us_tones; i++)
    {
    if (!(i%16))
    {
      if(len <=limit)
        len += sprintf(buf+len, "\n");
    }
    if(len <=limit)
      len +=
        sprintf (buf + len, "%02x ",
                 (unsigned char) pIhw->AppData.BitAllocTblUstrm[i]);
  }

  if(len<=limit)
    len += sprintf(buf+len, "\n\nAR7 DSL Modem DS Bit Allocation:\n");

  for (i=0; i<pIhw->AppData.max_ds_tones/16; i++)
  {
    for(j=0;j<16;j++)
    {
      if(len <=limit)
        len +=
          sprintf (buf + len, "%02x ",
                   (unsigned char) pIhw->AppData.BitAllocTblDstrm[i * 16 +
                                                                  j]);
    }
    if(len <=limit)
        len += sprintf(buf+len, "\n");
  }

  return len;
}

#ifndef NO_ACT
int tn7dsl_proc_ds_noise(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;
  int i, j;

  if(len<=limit)
    len += sprintf(buf+len, "\nAR7 DSL Modem DS Noise:\n");
  for (i=0; i< 512/16; i++)
  {
    for(j=0;j<16;j++)
    {
      if(len <=limit)
        len +=
          sprintf (buf + len, "%d ",
                   (unsigned int) pIhw->AppData.dsNoise[i * 16 + j]);
    }
    if(len <=limit)
        len += sprintf(buf+len, "\n");
  }
  return len;
}

#endif

char *tn7dsl_AnnexFromNum(unsigned short annex_selected)
{
static char *pUnknown= "Unknown";

  /* *INDENT-OFF* */
  static struct
  {
  char *annex_str;
  unsigned int bitMask;
} annex_Table[] =
{
  {"AnxA", 0x1},
  {"AnxB", 0x2},
  {"AnxC", 0x4},
  {"AnxI", 0x8},
  {"AnxJ", 0x10},
  {"AnxL", 0x20},
  {"AnxM", 0x40},
};
  /* *INDENT-ON* */

  unsigned int i;

  for (i=0;i<sizeof(annex_Table)/sizeof(annex_Table[0]); i++)
  {
    if (annex_Table[i].bitMask & annex_selected)
      return annex_Table[i].annex_str;
  }
  return pUnknown;
}

#ifdef ADV_DIAG_STATS //CQ10275, CQ10449
//UR8_MERGE_START CQ10449 Jack Zhang

static int proc_adv_stats_header(char* buf, int limit);

int tn7dsl_proc_adv_stats(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;
  //char *cp = buf + offset;
  char *cp = buf;
  int i = 0;
  int strt = 32;
  static int ctr = 0;

//  printk("proc_adv_stats: buf=0x%X, ctr=%d, offset=%d, count=%d, eof=%d\n",
//             (unsigned int)buf, ctr, offset, count, *eof);
  if( ctr == 0)
  {
    len = proc_adv_stats_header( cp, limit);

    if( len<=limit)
      len += sprintf(cp+len, "\n\tBin No.\tBits:\tMargin:\tSNR\n");
  }
  else
  {
    strt = ctr;
  }

  for( i =strt; i<512; i++)
  {
    if(len<=limit)
    {
      len += sprintf(cp+len, "\t%u\t%u\t%u\t%d\n", i,
                    (unsigned int)pIhw->AppData.BitAllocTblDstrm[i],
                    (unsigned int)pIhw->AppData.marginTblDstrm[i],
                    (int)pIhw->AppData.rxSnrPerBin0[i]);
    }
    else
    {
      ctr = i;
      //*eof = 0;
      *(cp + len) = '\0';
      printk("proc_adv_stats - return: ctr=%d, len=%d\n", ctr, len);
      return len;
    }
  }
  ctr = 0;
  *eof = 1;
  printk("proc_adv_stats - return: ctr=%d, len=%d\n", ctr, len);
  return len;
}

static int proc_adv_stats_header(char* buf, int limit)
{
  int len = 0;
  int i = 0;
  /*
   * Read Ax5 Stats
   */

  dslhal_api_gatherStatistics(pIhw);
  if(len<=limit)
    len += sprintf(buf+len, "\nAR7 DSL Modem Advanced Statistics:\n");

  if(len<=limit)
  {
    if(pIhw->lConnected != 1)
    {
      pIhw->AppData.USConRate = 0;
      pIhw->AppData.DSConRate = 0;
    }
    len +=
      sprintf (buf + len,
               "\t[Connection Rate]\tUS:\t%u\tDS:\t%u\n",
                   (unsigned int)pIhw->AppData.USConRate,
                   (unsigned int)pIhw->AppData.DSConRate );
  }
  if(len<=limit)
//  UR8_MERGE_START CQ11054   Jack Zhang
  {
    if (dslhal_api_getHighPrecision())
    {
      len +=
        sprintf (buf + len, "\t[Margin]\tUS:\t%d.%u\tDS:\t\t%d.%u\n",
                   gInt(pIhw->AppData.usMargin), gDot1(pIhw->AppData.usMargin),
                   gInt(pIhw->AppData.dsMargin), gDot1(pIhw->AppData.dsMargin));
    }
    else
    {
      len +=
        sprintf (buf + len, "\t[Margin]\tUS:\t%u\tDS:\t\t%u\n",
                   (unsigned int)pIhw->AppData.usMargin,
                   (unsigned int)pIhw->AppData.dsMargin/2 );
    }
  }
//  UR8_MERGE_END   CQ11054*

  /*
   * Downstream/Upstream Interleaved Errors
   */
  if(len<=limit)
    len += sprintf(buf+len, "\t[Interleave path] US (TX):\tCRC: \t%u\tFEC: \t%u\n",
                   (unsigned int)pIhw->AppData.usICRC_errors,
                   (unsigned int)pIhw->AppData.usIFEC_errors);
  if(len<=limit)
    len += sprintf(buf+len, "\t[Interleave path] DS (RX):\tCRC: \t%u\tFEC: \t%u\n",
                   (unsigned int)pIhw->AppData.dsICRC_errors,
                   (unsigned int)pIhw->AppData.dsIFEC_errors);
  /*
   * Upstream/Downstream Fast Errors
   */
  if(len<=limit)
    len += sprintf(buf+len, "\t[Fast path] US (TX):  \tCRC: \t%u\tFEC: \t%u\n",
                   (unsigned int)pIhw->AppData.usFCRC_errors,
                   (unsigned int)pIhw->AppData.usFFEC_errors);
  if(len<=limit)
    len += sprintf(buf+len, "\t[Fast path] DS (RX):\tCRC: \t%u\tFEC: \t%u\n",
                   (unsigned int)pIhw->AppData.dsFCRC_errors,
                   (unsigned int)pIhw->AppData.dsFFEC_errors);

  return len;
}

static int getDiagDisplayMode()
{
  int ret = 0;

  unsigned int  modeADSL1 = OLD_TRAINING_VAL_GDMT;

  //tn7dsl_set_dslmodes();

  if (pIhw->AppData.useBitField)
  {
    modeADSL1 = NEW_TRAINING_VAL_GDMT;
  }

  if( pIhw->AppData.TrainedMode == modeADSL1)
    ret = 1;
  else if(( pIhw->AppData.TrainedMode == ADSL2_MODE) ||
          ( pIhw->AppData.TrainedMode == ADSL2_PLUS))
    ret = 2;
  return ret;
}
int tn7dsl_proc_adv_stats1(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;
  int i;
  int mode = 0;  //mode = 0 => ADSL1 or ADSL2 & 2+
  unsigned char SNRpsds[512];
  int n;

  len = proc_adv_stats_header( buf+len, limit);
  mode = getDiagDisplayMode();

  if(len<=limit)
    len += sprintf(buf+len, "\tBin No.\tBits:\tMargin:\tSNR (Part 1 of 3)\n");

  if(mode==1) //ADSL1
  {
  for( i =32; i<128; i++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "\t%u\t%u\t%u\t%d\n", i,
                    (unsigned int)pIhw->AppData.BitAllocTblDstrm[i],
                    (unsigned int)pIhw->AppData.marginTblDstrm[i],
                    (int)pIhw->AppData.rxSnrPerBin0[i]);
  }
  }
  else if( mode ==2)//ADSL2 or 2+
  {
    if (dslhal_api_getSNRpsds(pIhw, SNRpsds, 1))
    {
      dgprintf(4, "dslhal_api_getSNRpsds failed!\n");
      return len;
    }
    for( i =32; i<128; i++)
    {
      if(len<=limit)
          len += sprintf(buf+len, "\t%u\t%u\t%u\t%d\n", i,
                    (unsigned int)pIhw->AppData.BitAllocTblDstrm[i],
                    (unsigned int)pIhw->AppData.marginTblDstrm[i],
                    (i<pIhw->AppData.max_ds_tones)?(unsigned char)SNRpsds[i]:0);
    }
  }
  return len;
}

int tn7dsl_proc_adv_stats2(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;
  int i;
  int mode = 0;  //mode = 0 => ADSL1 or ADSL2 & 2+
  unsigned char SNRpsds[512];

  mode = getDiagDisplayMode();

  if( mode==1) //ADSL1
  {
  dslhal_api_gatherStatistics(pIhw);
  if(len<=limit)
      len += sprintf(buf+len, "\tBin No.\tBits:\tMargin:\tSNR (Part 2 of 3):\n");
    for( i =128; i<320; i++)
    {
  if(len<=limit)
        len += sprintf(buf+len, "\t%u\t%u\t%u\t%d\n", i,
                    (unsigned int)pIhw->AppData.BitAllocTblDstrm[i],
                    (unsigned int)pIhw->AppData.marginTblDstrm[i],
                    (int)pIhw->AppData.rxSnrPerBin0[i]);
    }
  }
  else if( mode ==2)//ADSL2 or 2+
  {
    if (dslhal_api_getSNRpsds(pIhw, SNRpsds, 1))
    {
      dgprintf(4, "dslhal_api_getSNRpsds failed!\n");
      return len;
    }
  for( i =128; i<320; i++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "\t%u\t%u\t%u\t%d\n", i,
                    (unsigned int)pIhw->AppData.BitAllocTblDstrm[i],
                    (unsigned int)pIhw->AppData.marginTblDstrm[i],
                    (i<pIhw->AppData.max_ds_tones)?(unsigned char)SNRpsds[i]:0);
    }
  }
  return len;
}

int tn7dsl_proc_adv_stats3(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;
  int i;
  int mode = 0;  //mode = 0 => ADSL1 or ADSL2 & 2+
  unsigned char SNRpsds[512];

  mode = getDiagDisplayMode();

  if( mode==1) //ADSL1
  {
  dslhal_api_gatherStatistics(pIhw);
  if(len<=limit)
      len += sprintf(buf+len, "\tBin No.\tBits:\tMargin:\tSNR (Part 3 of 3):\n");
    for( i =320; i<512; i++)
    {
  if(len<=limit)
        len += sprintf(buf+len, "\t%u\t%u\t%u\t%d\n", i,
                    (unsigned int)pIhw->AppData.BitAllocTblDstrm[i],
                    (unsigned int)pIhw->AppData.marginTblDstrm[i],
                    (int)pIhw->AppData.rxSnrPerBin0[i]);
    }
  }
  else if( mode ==2)//ADSL2 or 2+
  {
    if (dslhal_api_getSNRpsds(pIhw, SNRpsds, 1))
    {
      dgprintf(4, "dslhal_api_getSNRpsds failed!\n");
      return len;
    }
  for( i =320; i<512; i++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "\t%u\t%u\t%u\t%d\n", i,
                    (unsigned int)pIhw->AppData.BitAllocTblDstrm[i],
                    (unsigned int)pIhw->AppData.marginTblDstrm[i],
                    (i<pIhw->AppData.max_ds_tones)?(unsigned char)SNRpsds[i]:0);
    }
  }
  if(len<=limit)
    len += sprintf(buf+len, "[End of Stats]\n");
  return len;
}
//UR8_MERGE_END  CQ10449
//UR8_MERGE_START CQ10682   Jack Zhang
int tn7dsl_proc_dbg_cmsgs(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;

  int rc=0;

  dslhal_api_gatherStatistics(pIhw);

  if(len<=limit)
    len += sprintf(buf+len, "Training Messages (C-Msgs 1-5)..\n");

  if(len<=limit)
    len += sprintf(buf+len, "ADSL2 DELT C-Msg1Ld \t Message Length:%d\n",
                            pIhw->adsl2DiagnosticMessages.cMsg1LdLen);
  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.cMsg1LdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.cMsg1Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }
  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT C-Msg2Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.cMsg2LdLen);

  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.cMsg2LdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.cMsg2Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }

  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT C-Msg3Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.cMsg3LdLen);

  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.cMsg3LdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.cMsg3Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }

  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT C-Msg4Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.cMsg4LdLen);

  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.cMsg4LdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.cMsg4Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }

  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT C-Msg5Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.cMsg5LdLen);

  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.cMsg5LdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.cMsg5Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }
  if(len<=limit)
    len += sprintf(buf+len, "\n");
  return len;
}

int tn7dsl_proc_dbg_rmsgs1(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;

  int rc=0;

  dslhal_api_gatherStatistics(pIhw);

  if(len<=limit)
    len += sprintf(buf+len, "Training Messages (R-Msgs 1-3)..\n");

  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT R-Msg1Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.rMsg1LdLen);

  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.rMsg1LdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.rMsg1Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }

  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT R-Msg2Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.rMsgxLdLen);

  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.rMsgxLdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.rMsg2Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }

  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT R-Msg3Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.rMsgxLdLen);
  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.rMsgxLdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.rMsg3Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }
  if(len<=limit)
    len += sprintf(buf+len, "\n");
  return len;
}

int tn7dsl_proc_dbg_rmsgs2(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;

  int rc=0;

  dslhal_api_gatherStatistics(pIhw);

  if(len<=limit)
    len += sprintf(buf+len, "Training Messages (R-Msgs 4-5)..\n");

  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT R-Msg4Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.rMsgxLdLen);
  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.rMsgxLdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.rMsg4Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }

  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT R-Msg5Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.rMsgxLdLen);
  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.rMsgxLdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.rMsg5Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }

  if(len<=limit)
    len += sprintf(buf+len, "\n");
  return len;
}

int tn7dsl_proc_dbg_rmsgs3(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;

  int rc=0;

  dslhal_api_gatherStatistics(pIhw);

  if(len<=limit)
    len += sprintf(buf+len, "Training Messages (R-Msgs 6-7)..\n");

  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT R-Msg6Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.rMsgxLdLen);
  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.rMsgxLdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.rMsg6Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }
  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT R-Msg7Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.rMsgxLdLen);
  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.rMsgxLdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.rMsg7Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }
  if(len<=limit)
    len += sprintf(buf+len, "\n");

  return len;
}

int tn7dsl_proc_dbg_rmsgs4(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;

  int rc=0;

  dslhal_api_gatherStatistics(pIhw);

  if(len<=limit)
    len += sprintf(buf+len, "Training Messages (R-Msgs 8-9)..\n");

  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT R-Msg8Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.rMsgxLdLen);
  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.rMsgxLdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.rMsg8Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }

  if(len<=limit)
    len += sprintf(buf+len, "\nADSL2 DELT R-Msg9Ld \t Message Length:%d\n",pIhw->adsl2DiagnosticMessages.rMsgxLdLen);
  for(rc=0;rc<pIhw->adsl2DiagnosticMessages.rMsgxLdLen;rc++)
  {
    if(len<=limit)
      len += sprintf(buf+len, "  %02x",(unsigned char)pIhw->adsl2DiagnosticMessages.rMsg9Ld[rc]);
    if(rc!=0 && (rc%16==0))
      if(len<=limit)
        len += sprintf(buf+len, "\n");
  }
  if(len<=limit)
    len += sprintf(buf+len, "\n");

  return len;
}
//UR8_MERGE_END   CQ10682*
#endif //ADV_DIAG_STATS

int tn7dsl_proc_stats(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{

  int len = 0;
  int limit = count - 80;
  int F4count, F5count;
  unsigned int maxRate=0;
  unsigned int us_maxRate=0;
  int i;
  //UR8_MERGE_START CQ10700 Manjula K
  struct atm_dev *dev;
  Tn7AtmPrivate *priv;
  dev = (struct atm_dev *)data;
  priv = (Tn7AtmPrivate *)dev->dev_data;
  //UR8_MERGE_END CQ10700

  /*
   * Read Ax5 Stats
   */

  dslhal_api_gatherStatistics(pIhw);
  if(len<=limit)
    len += sprintf(buf+len, "\nAR7 DSL Modem Statistics:\n");
  if(len<=limit)
    len += sprintf(buf+len, "--------------------------------\n");
  /*
   * us and ds Connection Rates
   */
  if(len<=limit)
    len += sprintf(buf+len, "[DSL Modem Stats]\n");


  if(len<=limit)
  {
    if(pIhw->lConnected != 1)
    {
      pIhw->AppData.USConRate = 0;
      pIhw->AppData.DSConRate = 0;
    }
    len +=
      sprintf (buf + len,
               "\tUS Connection Rate:\t%u\tDS Connection Rate:\t%u\n",
                   (unsigned int)pIhw->AppData.USConRate,
                   (unsigned int)pIhw->AppData.DSConRate );
  }
  if(len<=limit)
//  UR8_MERGE_START CQ11054   Jack Zhang
  {
    if (dslhal_api_getHighPrecision())
    {
      len +=
        sprintf (buf + len, "\tDS Line Attenuation:\t%u.%u\tDS Margin:\t\t%d.%u\n",
                   gInt(pIhw->AppData.dsLineAttn), gDot1(pIhw->AppData.dsLineAttn),
                   gInt(pIhw->AppData.dsMargin), gDot1(pIhw->AppData.dsMargin));
    }
    else{
      len +=
        sprintf (buf + len, "\tDS Line Attenuation:\t%u\tDS Margin:\t\t%u\n",
                   (unsigned int)pIhw->AppData.dsLineAttn/2,
                   (unsigned int)pIhw->AppData.dsMargin/2 );
    }
  }
//  UR8_MERGE_END   CQ11054*

  if(len<=limit)
//  UR8_MERGE_START CQ11054   Jack Zhang
  {
    if (dslhal_api_getHighPrecision())
    {
      len +=
        sprintf (buf + len, "\tUS Line Attenuation:\t%u.%u\tUS Margin:\t\t%d.%u\n",
                   gInt(pIhw->AppData.usLineAttn), gDot1(pIhw->AppData.usLineAttn),
                   gInt(pIhw->AppData.usMargin), gDot1(pIhw->AppData.usMargin));
    }
    else
    {
      len +=
        sprintf (buf + len, "\tUS Line Attenuation:\t%u\tUS Margin:\t\t%u\n",
                   (unsigned int)pIhw->AppData.usLineAttn/2,
                   (unsigned int)pIhw->AppData.usMargin );
    }
  }
//  UR8_MERGE_END   CQ11054*

  if(len<=limit)
    len += sprintf(buf+len, "\tUS Payload :\t\t%u\tDS Payload:\t\t%u\n",
                    ((unsigned int) pIhw->AppData.usAtm_count[0] +
                     (unsigned int) pIhw->AppData.usAtm_count[1]) * 48,
                    ((unsigned int) pIhw->AppData.dsGood_count[0] +
                     (unsigned int) pIhw->AppData.dsGood_count[1]) * 48);
  /*
   * Superframe Count
   */
  if(len<=limit)
    len +=
      sprintf (buf + len,
               "\tUS Superframe Cnt :\t%u\tDS Superframe Cnt:\t%u\n",
                   (unsigned int)pIhw->AppData.usSuperFrmCnt,
                   (unsigned int)pIhw->AppData.dsSuperFrmCnt );

  /*
   * US and DS power
   */
  if(len<=limit)
  {
    if(pIhw->AppData.bState < 5)
    {
      pIhw->AppData.usTxPower = 0;
      pIhw->AppData.dsTxPower = 0;
    }
    len +=
      sprintf (buf + len,
// UR8_MERGE_START - CQ11579 - Jeremy #1
               "\tUS Transmit Power :\t%d\tDS Transmit Power:\t%d\n",
                   pIhw->AppData.usTxPower/256,
                   pIhw->AppData.dsTxPower/256 );
// UR8_MERGE_END - CQ11579
  }
  /*
   * DSL Stats Errors
   */
  if(len<=limit)
    len += sprintf(buf+len, "\tLOS errors:\t\t%u\tSEF errors:\t\t%u\n",
                   (unsigned int)pIhw->AppData.LOS_errors,
                   (unsigned int)pIhw->AppData.SEF_errors );

  //UR8_MERGE_START Report_SES Manjula K
  //CQ10369
  if(len<=limit)
    len += sprintf(buf+len, "\tErrored Seconds:\t%u\tSeverely Err Secs:\t%u\n",
                   (unsigned int)pIhw->AppData.erroredSeconds,
                   (unsigned int)pIhw->AppData.severelyerrsecs );
  //UR8_MERGE_END Report_SES

  if(len<=limit)
    len += sprintf(buf+len, "\tFrame mode:\t\t%u\tMax Frame mode:\t\t%u\n",
                   (unsigned int)pIhw->AppData.FrmMode,
                   (unsigned int)pIhw->AppData.MaxFrmMode );
  if(len<=limit)
    len +=
      sprintf (buf + len, "\tTrained Path:\t\t%u\tUS Peak Cell Rate:\t%u\n",
                   (unsigned int)pIhw->AppData.TrainedPath,
                   (unsigned int)pIhw->AppData.USConRate*1000/8/53 );
  if(len<=limit)
    len +=
      sprintf (buf + len, "\tTrained Mode:\t\t%u\tSelected Mode:\t\t%u\n",
               (unsigned int) pIhw->AppData.TrainedMode,
               (unsigned int) pIhw->AppData.StdMode);

  if(len<=limit)
    len +=
      sprintf (buf + len, "\tATUC Vendor Code:\t%X\tATUC Revision:\t%u\n",
               (unsigned int) pIhw->AppData.atucVendorId,
               pIhw->AppData.atucRevisionNum);
  if(len<=limit)
    len += sprintf(buf+len, "\tHybrid Selected:\t%u\tTrellis:\t\t%u\n",
                   (unsigned int)pIhw->AppData.currentHybridNum, trellis);

  //@Added Maximum attainable bit rate information. 05-14-2004
  // UR8_MERGE_START - CQ11579 - Jeremy
  if((pIhw->AppData.dsl_modulation > 5) && (pIhw->AppData.dsl_modulation != 128))
// UR8_MERGE_END - CQ11579 - Jeremy
  {
    tn7atm_memcpy(&maxRate, &pIhw->adsl2TrainingMessages.rParams[6],4);
    maxRate /= 1000;
    tn7atm_memcpy(&us_maxRate, &pIhw->adsl2TrainingMessages.cParams[6],4);
  }
  else
  {
     int offset[2] = {5, 1};
     unsigned char rMsgsRA[12];
     int numPayloadBytes = 0;

    dslhal_api_dspInterfaceRead (pIhw, (unsigned int) pIhw->pmainAddr, 2,
                                 (unsigned int *) &offset,
                                 (unsigned char *) &rMsgsRA[0], 12);

     maxRate = (unsigned int)pIhw->AppData.DSConRate;

     numPayloadBytes  = ((rMsgsRA[6] & 0x01) << 8) | rMsgsRA[5];
    numPayloadBytes =
      dslhal_support_byteSwap32 ((unsigned int) numPayloadBytes);
     numPayloadBytes = 32*(numPayloadBytes -1); //UR8_MERGE_START_END CQ10715 ManjulaK

     if (numPayloadBytes > maxRate)
     {
        maxRate = numPayloadBytes;
     }
  }

  if(len<=limit)
    len +=
      sprintf (buf + len,
               "\tShowtime Count:\t\t%u\tDS Max Attainable Bit Rate: %u kbps\n",
                   (unsigned int)pIhw->AppData.showtimeCount, maxRate);

  if(len<=limit)
  {
    int offset[2] = {32, 0};
    unsigned int usBitswap, dsBitswap;

    tn7dsl_generic_read(2, (unsigned int *)&offset);
    dsBitswap = dslReg & dslhal_support_byteSwap32(0x000000ff);

    offset[0] = 33;
    tn7dsl_generic_read(2, (unsigned int *)&offset);
    usBitswap = dslReg & dslhal_support_byteSwap32(0x000000ff);

// UR8_MERGE_START - CQ11579 - Jeremy
    if((pIhw->AppData.dsl_modulation > 5) && (pIhw->AppData.dsl_modulation != 128))
// UR8_MERGE_END - CQ11579 - Jeremy
      len +=
        sprintf (buf + len,
                 "\tBitSwap:\t\t%u\tUS Max Attainable Bit Rate: %u bps\n",
                   (unsigned int)(usBitswap && dsBitswap), us_maxRate);
    else
      len +=
        sprintf (buf + len,
                 "\tBitSwap:\t\t%u\tUS Max Attainable Bit Rate:\tn/a\n",
                   (unsigned int)(usBitswap && dsBitswap));
  }

#if 1 // TR69
  if(len<=limit)
    len +=
      sprintf (buf + len, "\tAnnex: \t\t\t%s\tpsd_mask_qualifier: 0x%04x\n",
             tn7dsl_AnnexFromNum(pIhw->AppData.annex_selected),
             pIhw->AppData.psd_mask_qualifier);

//  UR8_MERGE_START CQ10979   Jack Zhang
//  UR8_MERGE_START CQ10978   Jack Zhang
  if(len<=limit)
    len +=
      sprintf (buf + len, "\tPower Management Status: L%d\tDS HLINSC: %d\n",
             pIhw->AppData.pwrStatus, pIhw->AppData.dsHLINSC);
//  UR8_MERGE_END   CQ10978*

  if(len<=limit)
    len +=
      sprintf (buf + len, "\tUS ACTPSD: \t\t%d\tDS ACTPSD: %d\n",
             pIhw->AppData.usACTPSD, pIhw->AppData.dsACTPSD);

  if(len<=limit)
    len +=
      sprintf (buf + len, "\tTotal init. errors: \t%d\tTotal init. timeouts: %d\n",
             pIhw->AppData.totalInitErrs, pIhw->AppData.totalInitTOs);

  if(len<=limit)
    len +=
      sprintf (buf + len, "\tShowtime init. errors: \t%d\tShowtime init. timeouts: %d\n",
             pIhw->AppData.showtimeInitErrs, pIhw->AppData.showtimeInitTOs);

  if(len<=limit)
    len +=
      sprintf (buf + len, "\tLast showtime init. errors: %d\tLast showtime init. timeouts: %d\n",
             pIhw->AppData.lastshowInitErrs, pIhw->AppData.lastshowInitTOs);
//  UR8_MERGE_END   CQ10979*

  if (len<=limit)
  {
    len += sprintf(buf+len,"\tATUC ghsVid: ");
    for (i=0; i<8; i++)
      len+= sprintf(buf+len, " %02x", pIhw->AppData.ghsATUCVendorId[i]);
  }

  if (len<=limit)
  {
    len += sprintf (buf + len, "\n");
  }

  if (len <= limit)
  {
    len +=
      sprintf (buf + len,
               "\tT1413Vid: %02x %02x\t\tT1413Rev: %02x\t\tVendorRev: %02x\n",
               pIhw->AppData.t1413ATUC.VendorId[0],
               pIhw->AppData.t1413ATUC.VendorId[1],
      pIhw->AppData.t1413ATUC.t1413Revision,
      pIhw->AppData.t1413ATUC.VendorRevision);
  }

  if (len<=limit)
  {
    len += sprintf(buf+len,"\tATUR ghsVid: ");
    for (i=0; i<8; i++)
      len+= sprintf(buf+len, " %02x", pIhw->AppData.ghsATURVendorId[i]);
  }

  if (len<=limit)
  {
    len += sprintf (buf + len, "\n");
  }

  if (len <= limit)
  {
    len +=
      sprintf (buf + len,
               "\tT1413Vid: %02x %02x\tT1413Rev: %02x\tVendorRev: %02x\n",
               pIhw->AppData.t1413ATUR.VendorId[0],
               pIhw->AppData.t1413ATUR.VendorId[1],
      pIhw->AppData.t1413ATUR.t1413Revision,
      pIhw->AppData.t1413ATUR.VendorRevision);
  }

#ifdef AR7_EFM
  if (len <= limit)
  {
    len += sprintf(buf + len, "\tTC Mode: %s\n",
      (priv->curr_TC_mode == TC_MODE_PTM) ? "PTM" : "ATM");
  }
#endif

#endif
  /*
   * Upstream Interleaved Errors
   */
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
  /*
   * Downstream Interleaved Errors
   */
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
  /*
   * Upstream Fast Errors
   */
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
  /*
   * Downstream Fast Errors
   */
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

  /*
   * ATM stats upstream
   */
  if(len<=limit)
    len += sprintf(buf+len, "\n[ATM Stats]");
  if(len<=limit)
    len += sprintf(buf+len, "\n\t[Upstream/TX]\n");
  if(len<=limit)
    len +=
      sprintf (buf + len, "\tGood Cell Cnt:\t%u\n\tIdle Cell Cnt:\t%u\n\n",
               (unsigned int) pIhw->AppData.usAtm_count[0] +
               (unsigned int) pIhw->AppData.usAtm_count[1],
               (unsigned int) pIhw->AppData.usIdle_count[0] +
               (unsigned int) pIhw->AppData.usIdle_count[1]);
//UR8_MERGE_START CQ10700 Manjula K
  if (len <= limit)
    len +=
      sprintf (buf + len,
               "\tTx Packets Dropped Count:\t%lu\n\tTx Bad Packets Count:\t%lu\n",
               priv->stats.tx_dropped, priv->stats.tx_errors);
//UR8_MERGE_END CQ10700
  /*
   * ATM stats downstream
   */
  if(len<=limit)
    len += sprintf(buf+len, "\n\t[Downstream/RX)]\n");
  if(len<=limit)
    len +=
      sprintf (buf + len,
               "\tGood Cell Cnt:\t%u\n\tIdle Cell Cnt:\t%u\n\tBad Hec Cell Cnt:\t%u\n",
               (unsigned int) pIhw->AppData.dsGood_count[0] +
               (unsigned int) pIhw->AppData.dsGood_count[1],
               (unsigned int) pIhw->AppData.dsIdle_count[0] +
               (unsigned int) pIhw->AppData.dsIdle_count[1],
               (unsigned int) pIhw->AppData.dsBadHec_count[0] +
               (unsigned int) pIhw->AppData.dsBadHec_count[1]);
  if(len<=limit)
    len += sprintf(buf+len, "\tOverflow Dropped Cell Cnt:\t%u\n",
                    (unsigned int) pIhw->AppData.dsOVFDrop_count[0] +
                    (unsigned int) pIhw->AppData.dsOVFDrop_count[1]);

 //UR8_MERGE_START CQ10700 Manjula K
  if (len <= limit)
    len +=
      sprintf (buf + len,
               "\tRx Packets Dropped Count:\t%lu\n\tRx Bad Packets Count:\t%lu\n\n",
               priv->stats.rx_dropped, priv->stats.rx_errors);
//UR8_MERGE_END CQ10700

  tn7sar_get_stats(pIhw->pOsContext);
  if(len<=limit)
    len += sprintf(buf+len, "\n[SAR AAL5 Stats]\n");
  if(len<=limit)
    len += sprintf(buf+len, "\tTx PDU's:\t%u\n\tRx PDU's:\t%u\n",
                    sarStat.txPktCnt, sarStat.rxPktCnt);
  if(len<=limit)
    len +=
      sprintf (buf + len, "\tTx Total Bytes:\t%u\n\tRx Total Bytes:\t%u\n",
               sarStat.txBytes, sarStat.rxBytes);
  if (len <= limit)
    len +=
      sprintf (buf + len,
               "\tTx Total Error Counts:\t%u\n\tRx Total Error Counts:\t%u\n\n",
               sarStat.txErrors, sarStat.rxErrors);

  /*
   * oam loopback info
   */
  if(len<=limit)
    len += sprintf(buf+len, "\n[OAM Stats]\n");

  tn7sar_get_near_end_loopback_count(&F4count, &F5count);

  if(len<=limit)
    {
    len +=
      sprintf (buf + len,
               "\tNear End F5 Loop Back Count:\t%u\n\tNear End F4 Loop Back Count:\t%u\n\tFar End F5 Loop Back Count:\t%u\n\tFar End F4 Loop Back Count:\t%u\n",
               F5count, F4count, oamFarLBCount[0] + oamFarLBCount[2],
                  oamFarLBCount[1] + oamFarLBCount[3]);
    }

#define USE_OAM_DROP_COUNT   //CQ10273
  //Read OAM ping responses count:
#ifdef USE_OAM_DROP_COUNT
  if(len<=limit)
  {
  /*  len +=
      sprintf (buf + len,
               "\tSAR OAM Retry in 0x%X cycles, Drop Count=%d\n",
               tn7dsl_get_memory(0xa30085cc), tn7dsl_get_memory(0xa30085c4)); */

    len += sprintf (buf + len, "\tSAR OAM Ping Response Drop Count=%d\n",
               tn7dsl_get_memory(0xa30085b0));
  }
#endif // USE_OAM_DROP_COUNT

  return len;
}

int tn7dsl_proc_modem(char* buf, char **start, off_t offset, int count,
                 int *eof, void *data)
{
#ifdef AR7_EFM
extern int tn7efm_get_currTCmode(void);
#endif
  int len = 0;
  int limit = count - 80;
  char *state;
  int tag;

  tag= dslhal_api_pollTrainingStatus(pIhw);
  tag = pIhw->AppData.bState;

  switch (tag)
  {
  case 0:
    state = "ACTREQ";
    break;
  case 1:
    state = "QUIET1";
    break;
  case 2:
    state = "IDLE";
    break;
  case 3:
    state = "INIT";
    break;
  case 4:
    state = "RTDL";
    break;
  case 5:
    state = "SHOWTIME";
    break;
  default:
    state = "unknown";
    break;
  }

  if(pIhw->lConnected == 1)
    state = "SHOWTIME";
  if(len<=limit)
    len += sprintf(buf+len,"%s\n",state);
  if(len<=limit)
    len += sprintf(buf+len, "%d\n", dslReg);
  if(len<=limit)
    len += sprintf(buf+len, "failTrains=%d\n", pIhw->AppData.trainFails);

#ifdef AR7_EFM
  if (len<=limit)
    len += sprintf(buf+len, "TCMODE=%s\n", 
      tn7efm_get_currTCmode()== TC_MODE_PTM ? "EFM" : "ATM");
#endif

  return len;
}

/**********************************************************************
**                                                                    *
**  tn7dsl_hdlc_update_crc() -- Calculate CRC                         *
**                                                                    *
**  Input Parms:                                                      *
**      new_byte  --  data byte to be counted into CRC                *
**      crc_reg   --  current CRC value before the new_byte           *
**                                                                    *
**  Output:                                                           *
**      --                                                            *
**                                                                    *
**  Return:                                                           *
**      new_crc   --  new CRC value after new_byte is calculated in   *
**                                                                    *
***********************************************************************/
static unsigned short tn7dsl_hdlc_update_crc (unsigned char new_byte,
                                              unsigned short crc_reg)
{
  int i;

  crc_reg = crc_reg ^ new_byte;
  for (i=0; i<8; i++)
     if (crc_reg & 0x0001)
        crc_reg = (crc_reg>>1) ^ HDLC_GENPOL;
     else
        crc_reg = crc_reg >> 1;

  return (crc_reg);
}

/**********************************************************************
**                                                                    *
**  tn7dsl_hdlc_tx_process()  -- HDLC Framing                         *
**                                                                    *
**  Input Parms:                                                      *
**      data   --   pointer to HDLC framed data btyes (output)        *
**      msg    --   pointer to data bytes to be HDLC framed (input)   *
**      len    --   length of data bytes pointed to by msg            *
**      msgOriented_flag --                                           *
**                  1= message oriented clear eoc (ADSL2/2+)          *
**                  0= bit oriented clear eoc (GDMT)                  *
**  Output:                                                           *
**      data   --   pointer to HDLC frame generated                   *
**      len    --   length of data bytes in the HDLC frame pointed to *
**                  by 'data' pointer                                 *
**                                                                    *
**  Return:                                                           *
**      0  --  Success                                                *
**      1  --  Failure                                                *
**                                                                    *
***********************************************************************/
static int tn7dsl_hdlc_tx_process(unsigned char *data, unsigned char *msg, unsigned int *len, int msgOriented_flag)
{
  unsigned short int crc; /* 16 bit unsigned integer */
  int      N, j, msglen;
  char     temp;

  if ((data==NULL) || (msg==NULL))
  {
     return(1);
  }

 if (msgOriented_flag)
  {
// message oriented clear eoc
    // used in ADSL2/2+
    // CTRL:   LSB: 0 & 1 Alternate when send
    //     2nd LSB: 0=command, 1=response.
    data[0]= 0x01; // NORMAL_PRIORITY (addr)
    data[2]= 0x08;  // clearEoc cmd
    if (msgOriented_flag & CLEAREOC_ACK)
    {
      data[1]= CEOC_RESP | eocctrl_resp; // CTRL: response
      data[3]= 0x80;  // ACK Response
      *len=4;
      eocctrl_resp ^=0x01;   // Toggle resp LSB
    }
    else
    {
      data[1]= CEOC_CMD | eocctrl_cmd; // CTRL: response
      data[3]= 0x01;    // clearEoc sub-command
      data[4]= 0x81;
      data[5]= 0x4c;

    //shim_osMoveMemory(&data[6], msg, *len);
      tn7atm_memcpy(&data[6], msg, *len);
      *len+=6;
      eocctrl_cmd ^= 0x01; // Toggle cmd LSB
    }
    return 0;
  }

  msglen = *len;

  data[0] = 0x7E;
  crc     = HDLC_INIT;
  data[1] = 0xFF;
  crc     = tn7dsl_hdlc_update_crc(data[1], crc);
  data[2] = 0x03;
  crc     = tn7dsl_hdlc_update_crc(data[2], crc);

  data[3] = 0x81;      // SNMP protocol identifier -- G.997.1
  crc     = tn7dsl_hdlc_update_crc(data[3], crc);
  data[4] = 0x4C;      // SNMP protocol identifier -- G.997.1
  crc     = tn7dsl_hdlc_update_crc(data[4], crc);

  N = 5;
  j = 0;
  while (j < msglen)
  {
     temp = msg[j++];
     crc = tn7dsl_hdlc_update_crc(temp, crc);
     if ((temp == HDLC_FLAG) || (temp == HDLC_ESC))
     {
        data[N] = HDLC_ESC;
        data[N+1] = temp ^ HDLC_INV;
        N = N + 2;
     }
     else
     {
        data[N] = temp;
        N = N + 1;
     }
  }

  crc = ~crc;

  for (j=0; j<2; crc >>= 8, j++)
  {
     temp = crc & 0x00FF;
     if ((temp == HDLC_FLAG) || (temp == HDLC_ESC))
     {
        data[N] = HDLC_ESC;
        data[N+1] = temp ^ HDLC_INV;
        N = N + 2;
     }
     else
     {
        data[N] = temp;
        N = N + 1;
     }
  }

  data[N] = 0x7E;

  *len = N + 1;

  return(0);
}

/**********************************************************************
**                                                                    *
**  tn7dsl_hdlc_rx_process()  -- HDLC De-Framing                      *
**                                                                    *
**  Input Parms:                                                      *
**      data   --   pointer to HDLC-deframed data bytes (output)      *
**      msg    --   pointer to HDLC frame (input)                     *
**      len    --   length of data bytes pointed to by msg            *
**      msgOriented_flag --                                           *
**                  1= message oriented clear eoc (ADSL2/2+)          *
**                  0= bit oriented clear eoc (GDMT)                  *
**  Output:                                                           *
**      data   --   pointer to HDLC de-framed                         *
**      len    --   length of data bytes pointed to by 'data'         *
**                                                                    *
**  Return:                                                           *
**      0  --  Success                                                *
**      1  --  Failure                                                *
**                                                                    *
***********************************************************************/
static int tn7dsl_hdlc_rx_process(unsigned char *data, unsigned char *msg, unsigned int *len, int msgOriented_flag)
{
  unsigned short int crc; /* 16 bit unsigned integer */
  unsigned char  rxbyte;
  int      N, j, msglen;
  int      ret = 1;

  if ((data==NULL) || (msg==NULL))
  {
     return(1);
  }

  msglen = *len;
  rxbyte = msg[0];
 printk("hdlc rx v0.6 len %d msg %x,%x,%x,%x,%x,%x,%x\n",*len,msg[0],msg[1],msg[2],msg[3],msg[4],msg[5],msg[6]);

  if (msgOriented_flag)
  {
    // message oriented clearEoc, used in ADSL2/2+
    // ADSL2/2+; expected pattern: 01 01 08 01 81 4c ....
    if (rxbyte != 0x01) // Normal priority
      return(ret);

    if (msglen<6||((msg[2]!= 0x08) && (msg[3]!= 0x01)) || (msg[4]!= 0x81) || (msg[5]!= 0x4c))
    {
      ErrEocRxHdlcFraming++;
      return(ret);   // not an expected clear EOC frame
    }

    *len -= 6; // data[] start from msg[6]
//    for (i=0; i<*len; i++) *data++ = *msg1++;
    //shim_osMoveMemory(data, &msg[6], *len);
    tn7atm_memcpy(data, &msg[6], *len);
    return 0;
  }


  if (rxbyte != HDLC_FLAG)
  {
     return(ret);   // invalid HDLC frame
  }

  if ((msg[1] != 0xFF) || (msg[2] != 0x03) ||
      (msg[3] != 0x81) || (msg[4] != 0x4c))
  {
     ErrEocRxHdlcFraming++;
     return(ret);   // not an expected clear EOC frame
  }

  crc = HDLC_INIT;
  j = 1;
  N = 0;

  while (j < msglen)
  {
     rxbyte = msg[j++];
     if (rxbyte == HDLC_ESC)
     {
        rxbyte = msg[j++];
        if ((rxbyte == (HDLC_ESC ^ HDLC_INV)) ||
            (rxbyte == (HDLC_FLAG ^ HDLC_INV)))
        {
           rxbyte ^= HDLC_INV;
        }
        else
        {
           break;
        }
     }
     else if (rxbyte == HDLC_FLAG)
     {
        break;
     }

     crc = tn7dsl_hdlc_update_crc(rxbyte, crc);

     if (j > 5)
     {
       data[N] = rxbyte;
       N++;
     }
  }

  if (crc == HDLC_GOOD && N >= 2)  // since 0xFF 03 81 4C are not counted in
  {
     N   = N - 2; // uncount FCS bytes
     ret = 0;     // success
  }

  *len = N;

  return(ret);
}

int tn7dsl_proc_eoc (char *buf, char **start, off_t OffSet, int count,
                     int *eof, void *data)
{
  int len = 0;
  int limit = count - 80;
  int offset[2] = {34, 0};  // point to buffer parameter data structure
  clearEocParm_t   peoc;

  dslhal_api_dspInterfaceRead (pIhw, (unsigned int) pIhw->pmainAddr, 2,
                               (unsigned int *) &offset,
                               (unsigned char *) &peoc,
                               sizeof (clearEocParm_t));

  if (len <= limit)
     len += sprintf(buf+len, "\nClear EOC Channel:\n\n");
  if (len <= limit)
     len += sprintf(buf+len, "  Enabled:\t%d\n", dslhal_support_byteSwap32(peoc.clearEocEnabled));
  if (len <= limit)
     len += sprintf(buf+len, "  TxBuf[0]:\t0x%08x\n", dslhal_support_byteSwap32((unsigned int)peoc.pTxBufDesc[0]));
  if (len <= limit)
     len += sprintf(buf+len, "  TxBuf[1]:\t0x%08x\n", dslhal_support_byteSwap32((unsigned int)peoc.pTxBufDesc[1]));
  if (len <= limit)
     len += sprintf(buf+len, "  TxBuf[2]:\t0x%08x\n", dslhal_support_byteSwap32((unsigned int)peoc.pTxBufDesc[2]));
  if (len <= limit)
     len += sprintf(buf+len, "  TxBuf[3]:\t0x%08x\n", dslhal_support_byteSwap32((unsigned int)peoc.pTxBufDesc[3]));
  if (len <= limit)
     len += sprintf(buf+len, "  RxBuf[0]:\t0x%08x\n", dslhal_support_byteSwap32((unsigned int)peoc.pRxBufDesc[0]));
  if (len <= limit)
     len += sprintf(buf+len, "  RxBuf[1]:\t0x%08x\n", dslhal_support_byteSwap32((unsigned int)peoc.pRxBufDesc[1]));
  if (len <= limit)
     len += sprintf(buf+len, "  RxBuf[2]:\t0x%08x\n", dslhal_support_byteSwap32((unsigned int)peoc.pRxBufDesc[2]));
  if (len <= limit)
     len += sprintf(buf+len, "  RxBuf[3]:\t0x%08x\n", dslhal_support_byteSwap32((unsigned int)peoc.pRxBufDesc[3]));
  if (len <= limit)
     len += sprintf(buf+len, "  txRdIndex:\t%d\n", dslhal_support_byteSwap32(peoc.txRdIndex));
  if (len <= limit)
     len += sprintf(buf+len, "  txWrIndex:\t%d\n", dslhal_support_byteSwap32(peoc.txWrIndex));
  if (len <= limit)
     len += sprintf(buf+len, "  rxRdIndex:\t%d\n", dslhal_support_byteSwap32(peoc.rxRdIndex));
  if (len <= limit)
     len += sprintf(buf+len, "  rxWrIndex:\t%d\n\n", dslhal_support_byteSwap32(peoc.rxWrIndex));
  if (len <= limit)
     len += sprintf(buf+len, "  TotalTxPkts:\t%d\n", EocTxTotalPackets);
  if (len <= limit)
     len += sprintf(buf+len, "  TotalRxPkts:\t%d\n", EocRxTotalPackets);
  if (len <= limit)
     len += sprintf(buf+len, "  TotalTxBytes:\t%d\n", EocTxTotalBytes);
  if (len <= limit)
     len += sprintf(buf+len, "  TotalRxBytes:\t%d\n\n", EocRxTotalBytes);
  if (len <= limit)
     len += sprintf(buf+len, "  ErrBufFull:\t%d\n", ErrEocBufFull);
  if (len <= limit)
     len += sprintf(buf+len, "  ErrBufIndx:\t%d\n", ErrEocBufIndex);
  if (len <= limit)
     len += sprintf(buf+len, "  ErrBufMax:\t%d\n", ErrEocBufMax);
  if (len <= limit)
     len += sprintf(buf+len, "  ErrMsgMax:\t%d\n", ErrEocMsgOversized);
  if (len <= limit)
     len += sprintf(buf+len, "  ErrTxHDLC:\t%d\n", ErrEocTxHdlcCRC);
  if (len <= limit)
     len += sprintf(buf+len, "  ErrRxHDLC:\t%d\n", ErrEocRxHdlcCRC);
  if (len <= limit)
     len += sprintf(buf+len, "  ErrRxSnmp:\t%d\n", ErrEocRxHdlcFraming);
  if (len <= limit)
     len += sprintf(buf+len, "  ErrRxPush:\t%d\n\n", ErrEocRxPush);

  return len;
}

int tn7dsl_clear_eoc_setup(void)
{
  int   i;
  int   offset[2] = {34, 0};
  int   offset_int[]={11};  // internal interface
  DEV_HOST_L2clearEocDef_t L2ceoc, *L2ceoc_p;
  char *p;
  int   eoc_ptr;
  clearEocParm_t   peoc;
  unsigned int phyAddr;

  dslhal_api_dspInterfaceRead(pIhw, (unsigned int)pIhw->pmainAddr, 2, (unsigned int *)&offset, (unsigned char *)pClrEOC, sizeof(clearEocParm_t));

  for (i=0; i<4; i++)
  {
    clearEocBufDesc_type  *pBuf;

    // Alloc buf and descriptor for Tx
    p = kmalloc(sizeof(clearEocBufDesc_type), GFP_ATOMIC);
    //p = shim_osAllocateDmaMemory(sizeof(clearEocBufDesc_type));
    if (p == NULL)
    {
      int j;

//    printk("tn7dsl_clear_eoc_setup: kmalloc() failed\n");

      for (j=i-1; j>=0; j--)
      {
         //shim_osFreeDmaMemory(pClrEOC->pTxBufDesc[j], sizeof(clearEocBufDesc_type));
//         kfree(pClrEOC->pTxBufDesc[j]);
         kfree(pTxBufDescSave[j]); //cph 1/16/06
         //shim_osFreeDmaMemory(pClrEOC->pRxBufDesc[j], sizeof(clearEocBufDesc_type));
//         kfree(pClrEOC->pRxBufDesc[j]);
         kfree(pRxBufDescSave[j]); //cph 1/16/06
      }

      return(1);
    }

    pBuf = (clearEocBufDesc_type *)p;
    pBuf->len  = 0;
    pTxBufDescSave[i] = pBuf; //cph 1/16/06
    pClrEOC->pTxBufDesc[i] = (clearEocBufDesc_type *) PHYS_TO_K1((unsigned int) pBuf);

    /*
     * Alloc buf and descriptor for Rx
     */
     p = kmalloc(sizeof(clearEocBufDesc_type), GFP_ATOMIC);
    //p = shim_osAllocateDmaMemory(sizeof(clearEocBufDesc_type));
    if (p == NULL)
    {
      int j;

//    printk("tn7dsl_clear_eoc_setup: kmalloc() failed\n");

      //shim_osFreeDmaMemory(pClrEOC->pTxBufDesc[i], sizeof(clearEocBufDesc_type));
//      kfree(pClrEOC->pTxBufDesc[i]);
      kfree(pTxBufDescSave[i]); //cph 1/16/06

      for (j=i-1; j>=0; j--)
      {
         //shim_osFreeDmaMemory(pClrEOC->pTxBufDesc[j], sizeof(clearEocBufDesc_type));
//         kfree(pClrEOC->pTxBufDesc[j]);
         kfree(pTxBufDescSave[j]); //cph 1/16/06
         //shim_osFreeDmaMemory(pClrEOC->pRxBufDesc[j], sizeof(clearEocBufDesc_type));
//         kfree(pClrEOC->pRxBufDesc[j]);
         kfree(pRxBufDescSave[j]); //cph 1/16/06
      }

      return(1);
    }

    pBuf = (clearEocBufDesc_type *)p;
    pBuf->len  = 0;
    pRxBufDescSave[i] = pBuf; //cph 1/16/06
    pClrEOC->pRxBufDesc[i] = (clearEocBufDesc_type *) PHYS_TO_K1((unsigned int) pBuf);
  }

  pClrEOC->txRdIndex = pClrEOC->txWrIndex = 0;
  pClrEOC->rxRdIndex = pClrEOC->rxWrIndex = 0;

  pClrEOC->clearEocEnabled = 1;

  for (i=0; i<4; i++)
  {
    /*shim_osInvalidateCache(pClrEOC->pTxBufDesc[i], sizeof(clearEocBufDesc_type)); */
    tn7atm_data_invalidate(pClrEOC->pTxBufDesc[i], sizeof(clearEocBufDesc_type));

    eoc_ptr = host2dspAddrMapping((unsigned int)pClrEOC->pTxBufDesc[i]);
    peoc.pTxBufDesc[i] = (clearEocBufDesc_type *)dslhal_support_byteSwap32(eoc_ptr);
//  printk("pTxBufDesc[%d] = 0x%08x\n", i, (unsigned int)peoc.pTxBufDesc[i]);
  }

  for (i=0; i<4; i++)
  {
    /*shim_osInvalidateCache(pClrEOC->pRxBufDesc[i], sizeof(clearEocBufDesc_type));*/
    tn7atm_data_invalidate(pClrEOC->pRxBufDesc[i], sizeof(clearEocBufDesc_type));

    eoc_ptr = host2dspAddrMapping((unsigned int)pClrEOC->pRxBufDesc[i]);
    peoc.pRxBufDesc[i] = (clearEocBufDesc_type *)dslhal_support_byteSwap32(eoc_ptr);
//  printk("pRxBufDesc[%d] = 0x%08x\n", i, (unsigned int)peoc.pRxBufDesc[i]);
  }

  peoc.clearEocEnabled = dslhal_support_byteSwap32(1);
//printk("clearEocEnabled = %d\n", peoc.clearEocEnabled);

  peoc.txRdIndex = peoc.txWrIndex = 0;
  peoc.rxRdIndex = peoc.rxWrIndex = 0;

#if 1 // clearEoc L2 buffer setup
  // read
  L2ceoc_p = (DEV_HOST_L2clearEocDef_t *) dslhal_support_readInternalOffset(pIhw, 1, offset_int);
  dslhal_support_blockRead((void *) L2ceoc_p, &L2ceoc, sizeof(DEV_HOST_L2clearEocDef_t));

  phyAddr =  PHYS_ADDR((unsigned int) pClrEOC->pTxBufDesc[0]->data);
  L2ceoc.L2UBuf= (unsigned char *) dslhal_support_byteSwap32(phyAddr);

  phyAddr =  PHYS_ADDR((unsigned int) pClrEOC->pRxBufDesc[0]->data);
  L2ceoc.L2DBuf= (unsigned char *) dslhal_support_byteSwap32(phyAddr);

  dslhal_support_blockWrite(&L2ceoc, (void *) L2ceoc_p, sizeof(DEV_HOST_L2clearEocDef_t));
#endif
  // write the clear eoc memory ptr to DSP
  dslhal_api_dspInterfaceWrite(pIhw,(unsigned int)pIhw->pmainAddr, 2, (unsigned int *)&offset, (unsigned char *)&peoc, sizeof(clearEocParm_t));

  return(0);
}

int tn7dsl_clear_eoc_close(void)
{
  int    i;
  int    offset[2] = {34, 0};
  clearEocBufDesc_type  *p;

  EocRxTotalPackets  = 0;
  EocTxTotalPackets  = 0;
  EocRxTotalBytes    = 0;
  EocTxTotalBytes    = 0;

  ErrEocBufFull      = 0;
  ErrEocBufIndex     = 0;
  ErrEocBufMax       = 0;
  ErrEocMsgOversized = 0;
  ErrEocTxHdlcCRC    = 0;
  ErrEocRxHdlcCRC    = 0;
  ErrEocRxHdlcFraming= 0;
  ErrEocRxPush       = 0;

  for (i=0; i<4; i++)
  {
//     p = pClrEOC->pTxBufDesc[i];
     p = pTxBufDescSave[i];
     if (p)
     {
        //shim_osFreeDmaMemory((char *)p, sizeof(clearEocBufDesc_type));
        kfree(p);
     }

     pTxBufDescSave[i] = pClrEOC->pTxBufDesc[i] = 0;

//     p = pClrEOC->pRxBufDesc[i];
     p = pRxBufDescSave[i];
     if (p)
     {
        //shim_osFreeDmaMemory((char *)p, sizeof(clearEocBufDesc_type));
        kfree(p);
     }

     pTxBufDescSave[i] = pClrEOC->pRxBufDesc[i] = 0;
  }

  pClrEOC->clearEocEnabled = 0;
  pClrEOC->txRdIndex = pClrEOC->txWrIndex = 0;
  pClrEOC->rxRdIndex = pClrEOC->rxWrIndex = 0;

  dslhal_api_dspInterfaceWrite(pIhw,(unsigned int)pIhw->pmainAddr, 2, (unsigned int *)&offset, (unsigned char *)pClrEOC, sizeof(clearEocParm_t));

  return(0);
}

int tn7dsl_clear_eoc_send(unsigned char *data, unsigned int len, int ackFlag)
{
  unsigned int   indx;
  unsigned int   index[2];
  unsigned int   enabled;
  int            offset0[2] = {34, 9};  // Tx buffer Read Index
  int            offset[2]  = {34, 10}; // Tx buffer Write Index
  int            offset2[2] = {34, 0};  // point to buffer parameter data structure
  int            ret = 0;
  unsigned int   i, num_swap, *pInt32;

  if (len > MAX_RAW_CLEAR_EOC_MSGSIZE)
  {
     ErrEocMsgOversized++;
//   printk("tn7dsl_clear_eoc_send: ClearEOC max packet size is exceeded\n");
     return(1);
  }

  dslhal_api_dspInterfaceRead(pIhw,(unsigned int)pIhw->pmainAddr, 2, (unsigned int *)&offset2, (unsigned char *)&enabled, 4);
  if (!enabled)
  {
//   printk("tn7dsl_clear_eoc_send: ClearEOC not enabled\n");
     return(1);
  }

  dslhal_api_dspInterfaceRead(pIhw,(unsigned int)pIhw->pmainAddr, 2, (unsigned int *)&offset0, (unsigned char *)&index[0], 8);
  pClrEOC->txRdIndex = dslhal_support_byteSwap32(index[0]);
  indx = dslhal_support_byteSwap32(index[1]);

  if (indx >= 4)
  {
     ErrEocBufIndex++;
//   printk("tn7dsl_clear_eoc_send: Invalid txWrIndex (=%d)\n", indx);
     return(1);
  }

  if (((indx+1)%4) == pClrEOC->txRdIndex)
  {
     ErrEocBufFull++;
//   printk("tndsl_clear_eoc_send: buffer full\n");
     return(1);
  }

  // HDLC framing, calculate FCS and pad HDLC
  ret = tn7dsl_hdlc_tx_process(pClrEOC->pTxBufDesc[indx]->data, data, &len,
          (pIhw->AppData.TrainedMode & (ADSL2_MODE|ADSL2_PLUS)) | ackFlag);

  if (!ret)
  {
     unsigned int wrIndex;

    // don't need to do endian swap because pTxBufDesc is in external SDRAM & len is 32 bit
     pClrEOC->pTxBufDesc[indx]->len = len;  // set HDLC frame length

    // do endian swap for data since they are bytes
     pInt32=(unsigned int *) pClrEOC->pTxBufDesc[indx]->data;

     for (i=0,num_swap=(len+3)>>2; i<num_swap; i++, pInt32++)
     {
        *pInt32=dslhal_support_byteSwap32(*pInt32);
     }
     tn7atm_data_writeback(pClrEOC->pTxBufDesc[indx], sizeof(clearEocBufDesc_type));

     pClrEOC->txWrIndex = (indx+1)%4;

     wrIndex = dslhal_support_byteSwap32(pClrEOC->txWrIndex);
     dslhal_api_dspInterfaceWrite(pIhw,(unsigned int)pIhw->pmainAddr, 2, (unsigned int *)&offset, (unsigned char *)&wrIndex, 4);

     EocTxTotalPackets++;
     EocTxTotalBytes += len;
  }
  else
  {
     ErrEocTxHdlcCRC++;
   //printk("tn7dsl_clear_eoc_send: tx HDLC processing error\n");
  }

  return(ret);
}

static int tn7dsl_clear_eoc_receive(void)
{
  int               rc, ch;
  int               ret = 0;
  int               enabled;
  int               offset[2]  = {34, 11};  // Rx buffer read index
  int               offset2[2] = {34, 0};   // point to buffer parameter data structure
  void              *recvinfo, *skb;
  tn7atm_private_t *priv;
  unsigned char    *data;
  unsigned int      indx;
  unsigned int      index[2], len0, len;

  rc = 0;

  while (1)
  {
    dslhal_api_dspInterfaceRead(pIhw, (unsigned int)pIhw->pmainAddr, 2, (unsigned int *)&offset, (unsigned char *)&index[0], 8);
    indx = dslhal_support_byteSwap32(index[0]);
    pClrEOC->rxWrIndex = dslhal_support_byteSwap32(index[1]);

    if (indx == pClrEOC->rxWrIndex)
    {
       return(0);
    }

    if (indx >= 4)
    {
       ErrEocBufIndex++;
     //printk("tn7dsl_clear_eoc_receive: Invalid rxRdIndex (=%d)\n", indx);
       indx = (indx + 1)%4;
       rc = 1;
       goto clrEoc_rx_exit;
    }

    dslhal_api_dspInterfaceRead(pIhw,(unsigned int)pIhw->pmainAddr, 2, (unsigned int *)&offset2, (unsigned char *)&enabled, 4);
    if (!enabled)
    {
     //printk("tn7dsl_clear_eoc_receive: ClearEOC not enabled\n");
       rc = 1;
       goto clrEoc_rx_exit;
    }

//
//Note: don't need to swap 'len' because pClrEOC->pRxBufDesc[indx] already point to
// external SDRAM
//    len = dslhal_support_byteSwap32(pClrEOC->pRxBufDesc[indx]->len);
    len = pClrEOC->pRxBufDesc[indx]->len;
    len0 = len;

    //Sending ClearEoc ACK(80)
    // Check for length > 4 so we don't ACk the ACK packet from CO.
    // ACK packet: 01 <01|03> 08 80 (4 bytes)
    if (pClrEOC->pRxBufDesc[indx]->len>4)
    {
      char dummy[4];
      tn7dsl_clear_eoc_send(dummy,4, CLEAREOC_ACK);
    }
    if (len > MAX_CLEAR_EOC_BUF_SIZE)
    {
     //printk("tn7dsl_clear_eoc_receive: buf size is exceeded\n");
       ErrEocBufMax++;
       rc = 1;
       goto clrEoc_rx_exit;
    }

    // chanid for clear eoc
    ch = EOC_DMA_CHAN;

    // allocate packet data memory
    priv = (tn7atm_private_t *)mydev->dev_data;
    data = (unsigned char *)tn7atm_allocate_rx_skb(mydev, &recvinfo, len, (void *)priv->lut[ch].vcc);
    if (data == NULL)
    {
     //printk("tn7dsl_clear_eoc_receive: kmalloc() failed\n");
       rc = 1;
       goto clrEoc_rx_exit;
    }

    skb = recvinfo;

    ret= tn7dsl_hdlc_rx_process(data, pClrEOC->pRxBufDesc[indx]->data, &len,
            (pIhw->AppData.TrainedMode & (ADSL2_MODE|ADSL2_PLUS)) );

    if (ret)
    {
       //printk("tn7dsl_clear_eoc_receive: rx processing error\n");

       // free skb
       tn7atm_free_rx_skb(skb, NULL);

       ErrEocRxHdlcCRC++;
       rc = 1;
       goto clrEoc_rx_exit;
    }

    if (len > MAX_RAW_CLEAR_EOC_MSGSIZE)
    {
       ErrEocMsgOversized++;
       printk("WARNING: tn7dsl -- max_raw_clear_eoc_msgsize is exceeded\n");
    }

    // send packet to network stacks
    ret = tn7atm_receive(mydev, ch, len, skb, data);

    if (ret)
    {
     //printk("tn7dsl_clear_eoc_receive: failed to push up packet\n");
       tn7atm_free_rx_skb(skb, NULL);

       ErrEocRxPush++;
       rc = 1;
    }

    EocRxTotalPackets++;
    EocRxTotalBytes += len0;

clrEoc_rx_exit:
    // update rxRdIndex anyway
    pClrEOC->rxRdIndex = (indx+1)%4;

    pClrEOC->pRxBufDesc[indx]->len = 0;

    /*shim_osInvalidateCache(pClrEOC->pRxBufDesc[indx], sizeof(clearEocBufDesc_type));*/
    tn7atm_data_invalidate(pClrEOC->pRxBufDesc[indx], sizeof(clearEocBufDesc_type));

    indx = dslhal_support_byteSwap32(pClrEOC->rxRdIndex);
    dslhal_api_dspInterfaceWrite(pIhw,(unsigned int)pIhw->pmainAddr, 2, (unsigned int *)&offset, (unsigned char *)&indx, 4);
  } // while

  return(rc);
}

inline int tn7dsl_handle_interrupt(void)
{
  int intsrc;
  unsigned char cMsgRa[6];
  short margin;
  extern unsigned int def_sar_inter_pace;   //Sorry
#ifdef AR7_EFM
  Tn7AtmPrivate *priv = (tn7atm_private_t *)mydev->dev_data;
  extern void efm_pdsp_showtime_init(void);
#else
//UR8_MERGE_START CQ10450   Jack Zhang
  Tn7AtmPrivate *priv;
//UR8_MERGE_END   CQ10450*
#endif

  dgprintf(4, "tn7dsl_handle_dsl_interrupt()\n");
  if(pIhw)
  {
    intsrc=dslhal_api_acknowledgeInterrupt(pIhw);

    if (nohost_flag) return 0;


    dslhal_api_handleTrainingInterrupt(pIhw, intsrc);

#ifdef AR7_EFM  
    switch (pIhw->AppData.bState) {
    case RSTATE_IDLE:
      if (pIhw->AppData.CurrDslState == RSTATE_HS)
      {
        printk("DSL leave showtime\n");
      }
      break;
    case RSTATE_SHOWTIME:
      if (pIhw->AppData.CurrDslState == RSTATE_HS)
      { // just entering SHOWTIME
        printk("DSL ShowTime(%s)\n", (priv->curr_TC_mode==TC_MODE_PTM) ? "PTM" : "ATM");
        efm_pdsp_showtime_init();
        tn7efm_showtime_init();        
      }
      break;
    default:
      break;
    }
    pIhw->AppData.CurrDslState = pIhw->AppData.bState;
#endif

#ifdef AR7_EFM
    if (pIhw->configFlag & CONFIG_FLAG_TC)
    {
      // Handle mode_switch as earlier than showtime as possible
      // because of the latency. The exception is T1413, because it
      // it doesn't have GHS. It can only be detected after showtime.
      printk("TCMODE=%d(%s)\n", pIhw->ghs_TCmode, pIhw->ghs_TCmode==TC_MODE_PTM ? "PTM" : "ATM");      
      
      if (pIhw->ghs_TCmode != priv->curr_TC_mode)
      {
        priv->target_TC_mode = pIhw->ghs_TCmode;
        tasklet_hi_schedule (&priv->tn7efm_tasklet);        
      }
      pIhw->configFlag &= ~CONFIG_FLAG_TC; // clear the flag
    }
#endif

    if(pIhw->lConnected == TC_SYNC)
    {

      if(dslInSync == 0)
      {
        printk("DSL in Sync\n");
        tn7atm_device_connect_status(pIhw->pOsContext, 1);
        dslhal_api_gatherStatistics(pIhw);

#ifdef AR7_EFM
        // Special care for T1413 to see if it need to switch mode.
        // Can only be done after showtime because T1413 doesn't have GHS.
        if ( (!pIhw->AppData.useBitField && (pIhw->AppData.TrainedMode==OLD_TRAINING_VAL_T1413))
          || (pIhw->AppData.useBitField && (pIhw->AppData.TrainedMode==NEW_TRAINING_VAL_T1413)) )
        {
          if (priv->curr_TC_mode == TC_MODE_PTM)
          {           
            priv->target_TC_mode = TC_MODE_ATM;
            tasklet_hi_schedule (&priv->tn7efm_tasklet);
          }
        }
#endif

#ifndef AR7_EFM
//UR8_MERGE_START CQ10450   Jack Zhang
//      priv = (Tn7AtmPrivate *)pIhw->pOsContext;
        priv = (tn7atm_private_t *)mydev->dev_data;
//UR8_MERGE_END   CQ10450*
#endif

#if defined (CONFIG_LED_MODULE) || defined (CONFIG_MIPS_AVALANCHE_COLORED_LED)
        if(hnd_LED_0)
            TN7DSL_LED_ACTION(hnd_LED_0, MOD_ADSL, DEF_ADSL_SYNC);
        led_on = DEF_ADSL_SYNC;
#endif
        /*
         * add auto margin retrain
         */
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

            // printk("margin = %d, cmsg-ra = %02x %02x %02x %02x %02x
            // %02x\n", margin,
            // cMsgRa[0],cMsgRa[1],cMsgRa[2],cMsgRa[3],cMsgRa[4],cMsgRa[5]);
            dslhal_api_setMarginThreshold (pIhw, margin * 2);   /* DSL margin
                                                                 * is in
                                                                 * 0.5db */
          }

//UR8_MERGE_START CQ10639 Manjula K
       /* Setting Pacing rate to default if it was boosted earlier */
       /* THE ASSUMPTION HERE IS DEFAULT INTERRUPT PACING VALUE STORED IN def_sar_inter_pace IS NOT ALTERED */
        if(inter_pace_boosted)
          {
            inter_pace_boosted = 0;
            avalanche_request_intr_pacing(LNXINTNUM (ATM_SAR_INT),
                  ATM_SAR_INT_PACING_BLOCK_NUM, def_sar_inter_pace);

          }
//UR8_MERGE_END CQ10639

        }
        else if((inter_pace_boosted != ANNEX_M_2PLUS_PACEMAX_VAL) && (pIhw->AppData.TrainedMode == ADSL2_PLUS_ANNEX_M)) //UR8_MERGE_START_END CQ10442 Manjula K
          {
            /* Higher Pacing rate is required for AnnexM 2+ mode. */
            if(def_sar_inter_pace < ANNEX_M_2PLUS_PACEMAX_VAL)
            {
              inter_pace_boosted = ANNEX_M_2PLUS_PACEMAX_VAL; //UR8_MERGE_START_END CQ10442 Manjula K
              avalanche_request_intr_pacing(LNXINTNUM (ATM_SAR_INT),
                  ATM_SAR_INT_PACING_BLOCK_NUM, ANNEX_M_2PLUS_PACEMAX_VAL);
          }
        }
//UR8_MERGE_START CQ10450   Jack Zhang
        else if( (inter_pace_boosted != HIGH_DS_CONN_RATE_PACEMAX_VAL) && (( priv->chip_id == CHIP_AR7O_212) || //UR8_MERGE_START_END CQ10442 Manjula K
                 (priv->chip_id == CHIP_AR7O_250_212)) && (pIhw->AppData.DSConRate > HIGH_DS_CONN_RATE_THRESHOLD)) //JZ: CQ10450
          {
            /* Higher Pacing rate is required for DS Conn rate > 20Mbps. */
            if(def_sar_inter_pace < HIGH_DS_CONN_RATE_PACEMAX_VAL)
            {
              inter_pace_boosted = HIGH_DS_CONN_RATE_PACEMAX_VAL; //UR8_MERGE_START_END CQ10442 Manjula K
              avalanche_request_intr_pacing(LNXINTNUM (ATM_SAR_INT),
                  ATM_SAR_INT_PACING_BLOCK_NUM, HIGH_DS_CONN_RATE_PACEMAX_VAL);
          }
        }
//UR8_MERGE_END   CQ10450*

//UR8_MERGE_START CQ10639 Manjula K

       /* Setting Pacing rate to default when DS Conn rate < 20Mbps and not connected in AnnexM. */
       /* THE ASSUMPTION HERE IS DEFAULT INTERRUPT PACING VALUE STORED IN def_sar_inter_pace IS NOT ALTERED */
        else if((inter_pace_boosted)&& (pIhw->AppData.TrainedMode != ADSL2_PLUS_ANNEX_M)&&
                 (pIhw->AppData.DSConRate < HIGH_DS_CONN_RATE_THRESHOLD))
          {
              inter_pace_boosted = 0;
              avalanche_request_intr_pacing(LNXINTNUM (ATM_SAR_INT),
                  ATM_SAR_INT_PACING_BLOCK_NUM, def_sar_inter_pace);

          }
//UR8_MERGE_END CQ10639

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
        printk("DSL out of sync\n");
      }

#if defined (CONFIG_LED_MODULE) || defined (CONFIG_MIPS_AVALANCHE_COLORED_LED)
      if(pIhw->AppData.bState < RSTATE_INIT)
      {
        if(led_on != DEF_ADSL_IDLE)
        {
          if(hnd_LED_0)
          {
          	TN7DSL_LED_ACTION(hnd_LED_0, MOD_ADSL,0);       
            TN7DSL_LED_ACTION(hnd_LED_0, MOD_ADSL, DEF_ADSL_IDLE);
          }
          led_on = DEF_ADSL_IDLE;
        }
      }
      else
      {
        if(led_on != DEF_ADSL_TRAINING)
        {
          if(hnd_LED_0)
          {
          	TN7DSL_LED_ACTION(hnd_LED_0, MOD_ADSL,0);         
            TN7DSL_LED_ACTION(hnd_LED_0, MOD_ADSL, DEF_ADSL_TRAINING);
          }
          led_on = DEF_ADSL_TRAINING;
        }

      }

#endif

    }

    //UR8_MERGE_START CQ10442 Manjula K
    if (pIhw->AppData.SRA)
    {
      dgprintf(4, "SRA Occured! New DS Connection Rate:\t%u\n", pIhw->AppData.DSConRate );

      if (inter_pace_boosted == 0)
      {
        if( (( priv->chip_id == CHIP_AR7O_212) ||( priv->chip_id == CHIP_AR7O_250_212)) &&
          (pIhw->AppData.DSConRate > HIGH_DS_CONN_RATE_THRESHOLD))
          {
            /* Higher Pacing rate is required for DS Conn rate > 20Mbps. */
            if(def_sar_inter_pace < HIGH_DS_CONN_RATE_PACEMAX_VAL)
            {
              inter_pace_boosted = HIGH_DS_CONN_RATE_PACEMAX_VAL;
              avalanche_request_intr_pacing(LNXINTNUM (ATM_SAR_INT),
                  ATM_SAR_INT_PACING_BLOCK_NUM, HIGH_DS_CONN_RATE_PACEMAX_VAL);
            }
          }
       }

    //UR8_MERGE_START CQ10639 Manjula K
      else
      {
       /* Setting Pacing rate to default when DS Conn rate < 20Mbps and not connected in AnnexM. */
       /* THE ASSUMPTION HERE IS DEFAULT INTERRUPT PACING VALUE STORED IN def_sar_inter_pace IS NOT ALTERED */
       if((pIhw->AppData.TrainedMode != ADSL2_PLUS_ANNEX_M) &&
          (pIhw->AppData.DSConRate < HIGH_DS_CONN_RATE_THRESHOLD))
          {

              inter_pace_boosted = 0;
              avalanche_request_intr_pacing(LNXINTNUM (ATM_SAR_INT),
                  ATM_SAR_INT_PACING_BLOCK_NUM, def_sar_inter_pace);

          }
       }
    //UR8_MERGE_END CQ10639

      pIhw->AppData.SRA = 0;
    }
    //UR8_MERGE_END CQ10442

    if (pIhw->AppData.clear_eoc)
    {
      tn7dsl_clear_eoc_receive();
      pIhw->AppData.clear_eoc = 0;
    }
  }
  return 0;
}


int tn7dsl_get_dslhal_version(char *pVer)
{
  dslVer ver;

  dslhal_api_getDslHalVersion(&ver);

  tn7atm_memcpy(pVer,&ver,8);
  return 0;
}

int tn7dsl_get_dsp_version(char *pVer)
{
  dspVer ver;
  dslhal_api_getDspVersion(pIhw, &ver);
  tn7atm_memcpy(pVer, &ver, 9);
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
  dslhal_api_dspInterfaceRead (pIhw, (unsigned int) pIhw->pmainAddr, 2,
                               (unsigned int *) &offset,
                               (unsigned char *) &oamFeature, 4);
  oamFeature |= dslhal_support_byteSwap32(0x0000000C);
  dslhal_api_dspInterfaceWrite (pIhw, (unsigned int) pIhw->pmainAddr, 2,
                                (unsigned int *) &offset,
                                (unsigned char *) &oamFeature, 4);
	
  ptr = prom_getenv("DSL_FEATURE_CNTL_0"); 
  if(!ptr)
    prom_setenv("DSL_FEATURE_CNTL_0", "0x00004000");

  ptr = prom_getenv("DSL_FEATURE_CNTL_1"); 
  if(!ptr)   
	prom_setenv("DSL_FEATURE_CNTL_1", "0x00000000");

  ptr = prom_getenv("DSL_PHY_CNTL_0"); 
  if(!ptr)   
	prom_setenv("DSL_PHY_CNTL_0", "0x00000400");
	
  ptr = prom_getenv("enable_margin_retrain"); 
  if(!ptr)   
	prom_setenv("enable_margin_retrain", "0");
	
  ptr = prom_getenv("modulation");
  if(!ptr)
    prom_setenv("modulation", "0xbf");
  
#define EOC_VENDOR_ID "4200534153000000"
#define EOC_VENDOR_REVISION "FW370090708b1_55"
#define EOC_VENDOR_SERIALNUM "ID937964FW370090708b1_55"
    
  ptr = prom_getenv("eoc_vendor_id");
  if(!ptr || strcmp(ptr,EOC_VENDOR_ID) != 0 || strlen(ptr) != strlen(EOC_VENDOR_ID))
  {
      if(ptr)      	
         prom_unsetenv("eoc_vendor_id");
      prom_setenv("eoc_vendor_id",EOC_VENDOR_ID);
  }
      
  ptr = prom_getenv("eoc_vendor_revision");
  if(!ptr || strcmp(ptr,EOC_VENDOR_REVISION) != 0 || strlen(ptr) != strlen(EOC_VENDOR_REVISION))
  {  
      if(ptr)      	
         prom_unsetenv("eoc_vendor_revision");
      prom_setenv("eoc_vendor_revision",EOC_VENDOR_REVISION);
  }
      
  ptr = prom_getenv("eoc_vendor_serialnum");
  if(!ptr || strcmp(ptr,EOC_VENDOR_SERIALNUM) != 0 || strlen(ptr) != strlen(EOC_VENDOR_SERIALNUM))
  {
      if(ptr)      	
         prom_unsetenv("eoc_vendor_serialnum");
      prom_setenv("eoc_vendor_serialnum",EOC_VENDOR_SERIALNUM);
  }

  /* Do only if we are in the new Base PSP 7.4.*/
  #if ((PSP_VERSION_MAJOR == 7) && (PSP_VERSION_MINOR == 4))
  /* Check to see if we are operating in the new bit mode. */
  ptr = prom_getenv("DSL_BIT_TMODE");
  if (ptr)
  {
    /* If we are see if this is the first time the user has upgraded. */
    ptr = prom_getenv("DSL_UPG_DONE");
    if(!ptr)
      {
         /* If it is the first time the user is upgrading, then make sure that
             we clear the modulation environment variable, as this could potentially
             not have the same meaning in the new mode.
          */
         prom_unsetenv("modulation");
         prom_setenv("DSL_UPG_DONE", "1");
      }
  }
  #endif

  // modulation
  ptr = prom_getenv("modulation");
  if (ptr)
  {
    tn7dsl_set_modulation(ptr, FALSE);
  }

  // Fine Gains
  ptr = prom_getenv("fine_gain_control");
  if (ptr)
  {
    value = os_atoi(ptr);
    tn7dsl_ctrl_fineGain(value);
  }
  ptr = NULL;
  ptr = prom_getenv("fine_gain_value");
  if(ptr)
    tn7dsl_set_fineGainValue(os_atoh(ptr));

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
      //printk("enable showtime margin monitor.\n");
      ptr = NULL;
      ptr = prom_getenv("margin_threshold");
      if(ptr)
      {
        value = os_atoi(ptr);
        //printk("Set margin threshold to %d x 0.5 db\n",value);
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
  ptr = prom_getenv("trellis");
  if(ptr)
  {
    dslhal_api_setTrellisFlag(pIhw, os_atoi(ptr));
    trellis = os_atoi(ptr);
    //printk("trellis=%d\n");
  }

  // bitswap
  ptr = NULL;
  ptr = prom_getenv("bitswap");
  if(ptr)
  {
    int offset[2] = {33, 0};
    unsigned int bitswap;

    bitswap = os_atoi(ptr);

    tn7dsl_generic_read(2, offset);
    dslReg &= dslhal_support_byteSwap32(0xFFFFFF00);
    dslReg |= dslhal_support_byteSwap32(bitswap & 0x000000FF);
    tn7dsl_generic_write(2, offset, dslReg);

    offset[0] = 32;       // olrDspRx_p
    tn7dsl_generic_read(2, offset);
    dslReg &= dslhal_support_byteSwap32(0xFFFFFF00);
    dslReg |= dslhal_support_byteSwap32(bitswap & 0x000000FF);
    tn7dsl_generic_write(2, offset, dslReg);
    //printk("bitswap=%d\n", bitswap);
  }

  // maximum bits per carrier
  ptr = NULL;
  ptr = prom_getenv("maximum_bits_per_carrier");
  if(ptr)
  {
    dslhal_api_setMaxBitsPerCarrierUpstream(pIhw, os_atoi(ptr));
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
    //UR8_MERGE_START CQ10505 ManjulaK
    //tn7dsl_get_dsp_version(dspVer);
    //printk("Annex =%d\n", dspVer[8]);

    //if(dspVer[8]==2) // annex b
    //{
      // printk("EOCVendorID=%02x %02x %02x %02x %02x %02x %02x %02x\n",
      // EOCVendorID[0], EOCVendorID[1], EOCVendorID[2], EOCVendorID[3],
      // EOCVendorID[4], EOCVendorID[5], EOCVendorID[6], EOCVendorID[7]);
      dslhal_api_setEocVendorId(pIhw, EOCVendorID);
    //} //UR8_MERGE_END CQ10505

  }
  ptr = NULL;
  ptr = prom_getenv("eoc_vendor_revision");
  if(ptr)
  {
    value = os_atoi(ptr);
    //printk("eoc rev=%d\n", os_atoi(ptr));
    dslhal_api_setEocRevisionNumber(pIhw, (char *)&value);

  }
  ptr = NULL;
  ptr = prom_getenv("eoc_vendor_serialnum");
  if(ptr)
  {
    dslhal_api_setEocSerialNumber(pIhw, ptr);
  }

  // CQ10037 Added invntry_vernum environment variable to be able to set version number in ADSL2, ADSL2+ modes.
  ptr = NULL;
  ptr = prom_getenv("invntry_vernum");
  if(ptr)
  {
    dslhal_api_setEocRevisionNumber(pIhw, ptr);
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
  char *cp = NULL;
  int retVal = 0;

//  UR8_MERGE_START CQ11054   Jack Zhang
  int high_precision_selected = 0;
//  UR8_MERGE_END   CQ11054*

  /*
   * start dsl
   */
  if((retVal  = dslhal_api_dslStartup(&pIhw)) !=0 )
  {
    printk("DSL start failed. Error Code (%d)\n", retVal);
    return -1;
  }

  // set dsl into overlay page reload mode
  pIhw->bAutoRetrain = 1;

  /*
   * For the changes for training modes using bit fields. Defaults are for
   * backward compatibility.
   */
  cp = prom_getenv("DSL_BIT_TMODE");
  if (cp)
  {
    printk("%s : env var DSL_BIT_TMODE is set\n", __FUNCTION__);
    /*
     * indicate to the data pump to use the bitfield mode.
     */
    pIhw->AppData.useBitField = TRUE;
  }

  /* set default training properties */
  tn7dsl_set_dsl();

  pIhw->pOsContext = priv;

  //start_kthread(tn7dsl_reload_overlay, &overlay_thread);

  /*
   * register dslss LED with led module. This is an empty function if
   * LEDs are not configured.
   */
  tn7dsl_register_dslss_led();

  pIhw->AppData.usTxPower=0;
  pIhw->AppData.dsTxPower=0;

// tn7dsl_clear_eoc_setup(); //cph clearEoc debug

//  UR8_MERGE_START CQ11054   Jack Zhang
  cp = prom_getenv("high_precision");
  if (cp)
  {
    high_precision_selected = os_atoi(cp);
  }
  if ( high_precision_selected)
  {
    printk("%s : env var high_precision is set.\n", __FUNCTION__);
    /*
     * indicate to the DHAL to use the high precision.
     */
    dslhal_api_setHighPrecision();
  }
//  UR8_MERGE_END   Cq11054*

  return 0;
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
  led_operation(MOD_ADSL,DEF_ADSL_IDLE);
#endif
    hnd_LED_0 = NULL;
#endif
#if defined (CONFIG_MIPS_AVALANCHE_COLORED_LED)
    led_manager_unregister_module(hnd_LED_0);
    hnd_LED_0 = NULL;
#endif

  //stop_kthread(&overlay_thread);
  tn7dsl_clear_eoc_close();
  dslhal_api_dslShutdown(pIhw);

}

/*
    Input format:   xAAAAAAAApBBBBBBc
    where AAAAAAAA is the vpi in hex and BBBBBB is the vci.
*/

static int tn7dsl_parse_cmd_vc(int *vpi, int *vci, char *in_cmd)
{
  int i=1;
  int j=0;
  char tmp[16];

  while(j<8)
  {
    tmp[j] = in_cmd[i];
    //printk("tmp[%d]=%c, %d\n", j, tmp[j], tmp[j]);
    if (tmp[j] == 'p' || tmp[j] == 'P')
      break;
    j++;
    i++;
  }

  tmp[j] = 0;
  *vpi = os_atoi (tmp);

  i++;
  j=0;
  while(j<8)
  {
    tmp[j] = in_cmd[i];
    //printk("tmp[%d]=%c, %d\n", j, tmp[j], tmp[j]);
    if (tmp[j] == 'c' || tmp[j] == 'C')
      break;

    j++;
    i++;
  }

  *vci = os_atoi (tmp);

  return i;
}

static int tn7dsl_process_txflush_string (int *pqueue, char *in_cmd)
{
  int i = 1;
  int j = 0;
  int vci, vpi;
  char tmp[16];
  int chan;
  int tt;

  i = tn7dsl_parse_cmd_vc(&vpi, &vci, in_cmd);

  if(!i)
      return (ATM_NO_DMA_CHAN);

  i++;
  j=0;
  tmp[j] = in_cmd[i];

  while(j<8)
  {
    tmp[j] = in_cmd[i];

    if (tmp[j] == 'q' || tmp[j] == 'Q')
      break;

    j++;
    i++;
  }
  tt = os_atoi(tmp);


  chan = tn7atm_lut_find(vpi, vci);

  *pqueue =tt;

  return chan;
}

static int tn7dsl_process_oam_string (int *type, int *pvpi, int *pvci,
                                      int *pdelay, char *in_cmd)
{
  int i=1;
  int j=0;
  int vci, vpi;
  char tmp[16];
  int chan;
  int tt;

  i = tn7dsl_parse_cmd_vc(&vpi, &vci, in_cmd);

  if(!i)
      return (ATM_NO_DMA_CHAN);

  if(vci==0) // f4 oam
    *type = 1;
  else
    *type = 0;


  tt=5000;
  i++;
  j=0;
  tmp[j] = in_cmd[i];
  if (tmp[j] == 'd' || tmp[j] == 'D')
  {
    i++;
    while(j<8)
    {
      tmp[j] = in_cmd[i];

      //printk("tmp[%d]=%c, %d\n", j, tmp[j], tmp[j]);
      if (tmp[j] == 't' || tmp[j] == 'T')
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
  dgprintf(2, "oam chan=%d, type =%d\n", chan, *type);

  return chan;
}

/*
    Input format:   wAAAAAAAAVVVVVVVV
    where AAAAAAAA is the address in hex and VVVVVVVV is the value to write.
*/
//static void tn7dsl_write_memory(char *input_str)
void tn7dsl_write_memory(char *input_str) //Export to tn7atm.c for oam ping loss
{
    volatile int *pUi;
    int i = 0;
    volatile int addr;
    volatile int value;
    volatile int read;
    char tmp[16];

    /* Read the first 8 characters for the address */
    for(i = 0; i < 8; i++)
        tmp[i] = input_str[i];

    tmp[i] = '\0';

    addr = os_atoh(tmp);

    pUi = (volatile int *)addr;

    /* Read the remaining characters as the value */
    value = os_atoh(&input_str[8]);

    /* Write the value into the memory location */
// * UR8_MERGE_START CQ10640   Jack Zhang
//    printk("Writing %#08x to address = 0x%p\n", value, pUi);
    printk("Writing 0x%x to address = 0x%p\n", value, pUi); //cph99
// * UR8_MERGE_END   CQ10640*
    tn7atm_data_writeback((int *)pUi, 1);
    *pUi = value;

    /* Read back the value that was written */
    /*shim_osInvalidateCache((int *)pUi, 1);*/
    tn7atm_data_invalidate((int *)pUi, 1);
    read = *pUi;

    if(value != read)
        printk("The value at address %#08x is %#08x\n", addr, read);
}

/*
    Input format:   rAAAAAAAAVVVVVVVV
    where AAAAAAAA is the address in hex and VVVVVVVV is the number of words
    to read back.
*/
//static int tn7dsl_dump_memory(char *input_str)
int tn7dsl_dump_memory(char *input_str)
  {
    volatile int *pUi;
    int i = 0;
    volatile int addr;
    volatile int wrd_to_read;
    char tmp[16];

    /* Read the first 8 characters for the address */
    for(i = 0; i < 8; i++)
    {
        /* For legacy reasons, the address can be terminated by a 'c' character. */
        //XZ -- NO USE! if(toupper(input_str[i]) != 'C')
            tmp[i] = input_str[i];
  }

    tmp[i] = '\0';

  addr = os_atoh(tmp);

    pUi = (volatile int *)addr;

    /* Read the remaining characters as the value */
    wrd_to_read = os_atoh(&input_str[8]);

    /* Read back the values from the memory location */
    printk("Reading %d words from address = 0x%p\n", wrd_to_read, pUi);

    for(i = 0; i < wrd_to_read; i++, pUi++)
    {
// * UR8_MERGE_START CQ10640   Jack Zhang
#if 1//cph99
      if(!(i % 4))
      {
        printk("\n%08x: ", addr);
        addr+=16;
      }
#else
      if(!(i % 4))
        printk("\n");
#endif
// * UR8_MERGE_END   CQ10640*

        /* Read back the value that was written */
        /*shim_osInvalidateCache((int *)pUi, 1);*/
        tn7atm_data_invalidate((int *)pUi, 1);
// * UR8_MERGE_START CQ10640   Jack Zhang
        //printk("%#08x ", *pUi);
        printk("%08x ", *pUi); //cph99
// * UR8_MERGE_END   CQ10640*

  }
    printk("\n");

    return(4 * i);
}

unsigned int tn7dsl_get_memory(unsigned int addr_in)
{
    volatile int *pUi;
    volatile int addr;

    unsigned int ret;

    addr = addr_in;

    pUi = (volatile int *)addr;

    /*shim_osInvalidateCache((int *)pUi, 1);*/
    tn7atm_data_invalidate((int *)pUi, 1);
    ret = *pUi;

    return ret;
}



static int dslmod_sysctl(ctl_table *ctl, int write, struct file * filp,
      void *buffer, size_t *lenp)
{
  char *ptr;
  int ret, len = 0;
  int chan;
  int type;
  int vpi,vci,timeout,queue;
  int i;
  phySettings curr_setting;
  char mod_req[16] = { '\t' };
  char fst_byt;

  if (!*lenp || (filp->f_pos && !write))
  {
    *lenp = 0;
    return 0;
    }
  /*
   * Command sent to module to do...
   */
  if(write)
    {
    ret = proc_dostring(ctl, write, filp, buffer, lenp);

    switch (ctl->ctl_name)
      {
      case DEV_DSLMOD:
      ptr = strpbrk(info, " \t");
      strcpy(mod_req, info);

      /*
       * parse the string to determine the action
       */
      if (!strcmp (info, "dspfreq"))
      {
        printk("dsp_freq = %d\n", SAR_FREQUNCY * 4);
        break;
      }
      else if (!strncmp(info,"nohost", 6))
      {
        if (strlen(info)==6) // read
          printk("nohost=%d\n", nohost_flag);
        else // write
        {
          nohost_flag= mod_req[6]-'0';
          printk("nohost<-%d\n", nohost_flag);
        }
      }

      else if (!strcmp(info,"dslctl"))
      {
        for (i=0; i<2; i++)
        {
          memset(&curr_setting, 0, sizeof(curr_setting));
          dslhal_api_readPhyFeatureSettings(pIhw,
                            INTEROP_FEATURELIST0_PARAMID+i,
                           (void *) &curr_setting);
          printk("phy_feature_%d=0x%08x, phy_cntl_mask_%d=0x%08x\n",
            i, curr_setting.phyEnableDisableWord,
            i, curr_setting.phyControlWord);
        }
        break;
      }
// * UR8_MERGE_START CQ10880   Jack Zhang
      else if (!strcmp(info,"dsp_l3msg"))
      {
        //printk("dsp_l3msg sent to DSP\n");
        dslhal_api_sendL3Command(pIhw);
        break;
      }
// * UR8_MERGE_END   CQ10880*

      /* Now for the famous ioctl commands */
      fst_byt = toupper(mod_req[0]);
      switch (fst_byt)
      {
      case 'S':
                /* Deliberate fall through for this condition. */
      case 'E':
                chan = tn7dsl_process_oam_string (&type, &vpi, &vci, &timeout, mod_req);
                if( fst_byt == 'S') type = type | (1 << 1);
                ret = tn7sar_oam_generation (pIhw->pOsContext, chan, type, vpi, vci, timeout);
                break;

       case 'F':
                chan = tn7dsl_process_txflush_string (&queue, mod_req);
                if (chan < 16)
                    tn7sar_tx_flush (pIhw->pOsContext, chan, queue, 0);
                break;
      case 'D':
// * UR8_MERGE_START CQ10640   Jack Zhang
                if (mod_req[0]=='d') //cph99
                  tn7dsl_dump_dsp_memory (&mod_req[1]);
                else
                  tn7dsl_dump_memory (&mod_req[1]);
// * UR8_MERGE_END   CQ10640*
      break;
      case 'W':
                tn7dsl_write_memory(&mod_req[1]);
                break;
      default:
               tn7dsl_chng_modulation (info);
      }
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
  {DEV_DSLMOD, "dslmod", info, DSL_MOD_SIZE, 0644, NULL, &dslmod_sysctl}
  ,
  {0}
  };

/* Make sure that /proc/sys/dev is there */
ctl_table dslmod_root_table[] = {
#ifdef CONFIG_PROC_FS
  {CTL_DEV, "dev", NULL, 0, 0555, dslmod_table}
  ,
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

  /*
   * set the defaults
   */
  info[0] = 0;

  initialized = 1;
}

void tn7dsl_dslmod_sysctl_unregister(void)
{
  unregister_sysctl_table(dslmod_sysctl_header);
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

static void tn7dsl_adiag(int Test, unsigned char *missingTones)
{
  int rc,cmd, tag;

  rc = dslhal_diags_anlg_setMissingTones(pIhw,missingTones);
  if(rc)
  {
    printk (" failed to set Missing tone\n");
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
     dgprintf(0,"TX MEDLEY Test\n");
     rc = dslhal_support_writeHostMailbox(pIhw, HOST_TXMEDLEY, 0, 0, 0);
     if (rc)
       return;
   }
 dgprintf(4,"dslhal_diags_anlg_testA() done\n");
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

static void tn7dsl_chng_modulation(void* data)
{
  unsigned char tmp1[64], tmp2[64], *p;
  int k, m;

  if(!strcmp(data, "trellison"))
  {
    //printk("set Trellis on\n");
    dslhal_api_setTrellisFlag(pIhw, 1);
    trellis = 1;
    dslhal_api_sendQuiet(pIhw);
    return;
  }
  if(!strcmp(data, "trellisoff"))
  {
    //printk("set Trellis off\n");
    dslhal_api_setTrellisFlag(pIhw, 0);
    trellis = 0;
    dslhal_api_sendQuiet(pIhw);
    return;
  }

  // pair selection
  if(!strcmp(data, "innerpair"))
  {
    //printk("set inner pair\n");
    dslhal_api_selectInnerOuterPair(pIhw, 0);
    dslhal_api_sendQuiet(pIhw);
    return;
  }
  if(!strcmp(data, "outerpair"))
  {
    //printk("set outer pair\n");
    dslhal_api_selectInnerOuterPair(pIhw, 1);
    dslhal_api_sendQuiet(pIhw);
    return;
  }

  //@Send modem to Idle.
  if(!strcmp(data, "idle"))
  {
    //printk("send idle\n");
    dslhal_api_sendIdle(pIhw);
    return;
  }
  //@Send modem to retrain.
  if(!strcmp(data, "retrain"))
  {
    //printk("retrain\n");
    dslhal_api_sendQuiet(pIhw);
    return;
  }

  //fine gain
  if(!strcmp(data, "gainctlon"))
  {
    //printk("gain control on\n");
    tn7dsl_ctrl_fineGain(1);
    dslhal_api_sendQuiet(pIhw);
    return;
  }
  if(!strcmp(data, "gainctloff"))
  {
    //printk("gain control off\n");
    tn7dsl_ctrl_fineGain(0);
    dslhal_api_sendQuiet(pIhw);
    return;
  }

  //dgas polarity
  if(!strcmp(data, "gaspolarityon"))
  {
    printk("gaspolarityon\n");
    dslhal_api_configureDgaspLpr(pIhw, 1, 1);
    dslhal_api_configureDgaspLpr(pIhw, 0, 1);
    return;
  }
  if(!strcmp(data, "gaspolarityoff"))
  {
    printk("gaspolarityoff\n");
    dslhal_api_configureDgaspLpr(pIhw, 1, 1);
    dslhal_api_configureDgaspLpr(pIhw, 0, 0);
    return;
  }

  if(!strcmp(data, "losalarmon"))
  {
    printk("losalarmon\n");
    dslhal_api_disableLosAlarm(pIhw, 0);
    return;
  }

  if(!strcmp(data, "losalarmoff"))
  {
    printk("losalarmoff\n");
    dslhal_api_disableLosAlarm(pIhw, 1);
    return;
  }

  if(!strcmp(data, "bitswapon"))
  {
    int offset[2] = {33, 0};

//  printk("set bitswap on\n");
    tn7dsl_generic_read(2, offset);
    dslReg |= dslhal_support_byteSwap32(0x00000001);
    tn7dsl_generic_write(2, offset, dslReg);    // enable Tx bitswap

    offset[0] = 32;       // olrDspRx_p
    tn7dsl_generic_read(2, offset);
    dslReg |= dslhal_support_byteSwap32(0x00000001);
    tn7dsl_generic_write(2, offset, dslReg);    // enable Rx bitswap

    dslhal_api_sendQuiet(pIhw);
    return;
  }

  if(!strcmp(data, "bitswapoff"))
  {
    int offset[2] = {33, 0};

//  printk("set bitswap off\n");
    tn7dsl_generic_read(2, offset);  // olrDspTx_p
    dslReg &= dslhal_support_byteSwap32(0xffffff00);
    tn7dsl_generic_write(2, offset, dslReg);   // disable Tx bitswap

    offset[0] = 32;       // olrDspRx_p
    dslReg &= dslhal_support_byteSwap32(0xffffff00);
    tn7dsl_generic_write(2, offset, dslReg);   // disable Rx bitswap

    dslhal_api_sendQuiet(pIhw);
    return;
  }

  if(!strcmp(data, "rmsgsra"))
  {
     int offset[2] = {5, 1};
     unsigned char rMsgsRA[12];

    dslhal_api_dspInterfaceRead (pIhw, (unsigned int) pIhw->pmainAddr, 2,
                                 (unsigned int *) &offset,
                                 (unsigned char *) &rMsgsRA[0], 12);

    printk
      ("rMsgsRA = 0x%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
       rMsgsRA[0], rMsgsRA[1], rMsgsRA[2], rMsgsRA[3], rMsgsRA[4], rMsgsRA[5],
       rMsgsRA[6], rMsgsRA[7], rMsgsRA[8], rMsgsRA[9], rMsgsRA[10],
       rMsgsRA[11]);
  }

  p=(unsigned char *)data;

  // fine gain
  for(k=0;k<5;k++)
  {
    tmp1[k]=p[k];
  }
  tmp1[k]=0;
  if(!strcmp(tmp1, "gainv"))
  {
    for(m=0;m<4;m++)
    {
      tmp2[m]=p[m+k];
    }
    tmp2[m]=0;
    //printk("name =%s, %d\n", tmp1, os_atoh(tmp2));
    tn7dsl_set_fineGainValue(os_atoh(tmp2));
    return;
  }

  // interleave depth
  for(k=0;k<6;k++)
  {
    tmp1[k]=p[k];
  }
  tmp1[k]=0;
  if(!strcmp(tmp1, "idepth"))
  {
    for(m=0;m<1;m++)
    {
      tmp2[m]=p[m+k];
    }
    tmp2[m]=0;
    printk("name =%s, %d\n", tmp1, os_atoi(tmp2));
    dslhal_api_setMaxInterleaverDepth(pIhw, os_atoi(tmp2));
    dslhal_api_sendQuiet(pIhw);
    return;
  }


  // carrier per bit
  for(k=0;k<13;k++)
  {
    tmp1[k]=p[k];
  }
  tmp1[k]=0;
  if(!strcmp(tmp1, "carrierperbit"))
  {
    for(m=0;m<4;m++)
    {
      tmp2[m]=p[m+k];
    }
    tmp2[m]=0;
    //printk("name =%s, %d\n", tmp1, os_atoh(tmp2));
    dslhal_api_setMaxBitsPerCarrierUpstream(pIhw, os_atoh(tmp2));
    dslhal_api_sendQuiet(pIhw);
    return;
  }

  p=(unsigned char *)data;
  if(p[0]==0x58 || p[0]==0x78) //"X" or "x"
  {
    unsigned int dAddr;
    //printk("main address = 0x%x\n", pIhw->pmainAddr);
    for(k=0;k<8;k++)
    {
      tmp1[k]=p[k+1];
    }
    tmp1[k]=0;
    dAddr=os_atoh(tmp1);
    printk("address =0x%x\n", os_atoh(tmp1));
    if(p[k+1]==0x4c || p[k+1]==0x6c)
    {
      unsigned int dVal;
      k++;
      for(m=0;m<8;m++)
      {
        tmp2[m]=p[m+k+1];
      }
      tmp2[m]=0;
      dVal=os_atoh(tmp2);
      printk("writing value =0x%x\n",dVal);
      dslhal_support_blockWrite((void *)&(dVal), (void *)dAddr, 4);
    }
    else
    {
      dslhal_support_blockRead((void *)dAddr, &dslReg, 4);
      printk("dslReg=0x%x\n", dslReg);
    }
  }

  p=(unsigned char *)data;
  if(p[0]==0x59 || p[0]==0x79) //"Y" or "y"
  {
    for(k=0;k<8;k++)
    {
      tmp1[k]=p[k+1];
      if(p[k+1]== 0)
        break;
    }
    tmp1[k]=0;
    printk("write mail box cmd =%d\n", os_atoi(tmp1));
    dslhal_support_writeHostMailbox(pIhw, os_atoi(tmp1), 0, 0, 0);
  }


#ifdef ADIAG
  tn7dsl_diagnostic_test(data);
#endif

  /*
   * The command is to set the modulation.
   */
  if(!tn7dsl_set_modulation(data, TRUE))
  {
      dslhal_api_sendQuiet(pIhw);
      return;
  }

}

static unsigned int tn7dsl_set_modulation(void* data, int flag)
{
  /* *INDENT-OFF* */
  struct
  {
  char        *mode_name;
  unsigned int mode_num;
} mode_table[]={
//  {"T1413", T1413_MODE},
//  {"GDMT",  GDMT_MODE},
//  {"MMODE", MULTI_MODE},
  {"T1413", OLD_TRAINING_VAL_T1413},
  {"GDMT",  OLD_TRAINING_VAL_GDMT},
  {"GLITE", GLITE_MODE},
  {"MMODE", OLD_TRAINING_VAL_MMODE},
  {"NMODE", NO_MODE},
  {"AD2MOD", ADSL2_MODE},
  {"AD2DEL", ADSL2_DELT},
  {"A2PMOD", ADSL2_PLUS},
  {"A2PDEL", ADSL2_PLUS_DELT}
};
  /* *INDENT-ON* */

    int retval = TRUE;
    int train_mode = -1;
    int size;
    char *cp = (char *)data;
    int i;

    dgprintf(4,"tn7dsl_set_modulation\n");

  if (pIhw->AppData.useBitField)
  {
    mode_table[0].mode_num = NEW_TRAINING_VAL_T1413;
    mode_table[1].mode_num = NEW_TRAINING_VAL_GDMT;
    mode_table[3].mode_num = NEW_TRAINING_VAL_MMODE;
  }
  for (i = 0; i < NUM_ELEMS (mode_table); i++)
  {
    if (!strcmp (data, mode_table[i].mode_name))
    {
        train_mode = mode_table[i].mode_num;
        break;
      }
    }

      if (train_mode==-1)
      {
    /*
     * Since we don't want to compare the nul terminator
     */
        size = strlen("tmode");

        if(!strncmp(cp, "tmode", size))
        {
            cp += size;

      /*
       * Read the specified mode, which has to be a hex value
       */
           if((*cp == '0') && ((*(cp + 1) == 'x') || (*(cp + 1) == 'X')))
            {
               train_mode = os_atoh(cp + 2);
            }
            else
            {
                train_mode = os_atoh(cp);
            }
        }
        else
        {
          /*
           * CQ9605: The new default command format for changing the modulation.
           */
            if((*cp == '0') && ((*(cp + 1) == 'x') || (*(cp + 1) == 'X')))
            {
                train_mode = os_atoh(cp + 2);
            }
            else if(!flag)
            {
             /*
              * For backward compatibility for modulation
              */
              train_mode = os_atoi(cp);

            }
        }
    }

    if(train_mode >= 0)
    {
        printk("%s : Setting mode to %#x\n", __FUNCTION__, train_mode);
        retval = dslhal_api_setTrainingMode(pIhw, train_mode);
    }

    return retval;
}

#if defined (CONFIG_MIPS_AVALANCHE_COLORED_LED) || defined (CONFIG_LED_MODULE)
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
#if defined (CONFIG_MIPS_AVALANCHE_COLORED_LED)
    /* Register with the new LED module */
    hnd_LED_0 = led_manager_register_module("adsl",0);

    if(!hnd_LED_0)
    {
        printk("ERROR DSL: Unable to register with LED module\n");
        return;
    }

#if 0
    // register LED0 with led module
    ledreg[0].param = 2;
    ledreg[0].led_init = (void *)tn7dsl_led_init;
    ledreg[0].led_on   = (void *)tn7dsl_led_on;
    ledreg[0].led_off  = (void *)tn7dsl_led_off;


    if( led_manager_install_callbacks(hnd_LED_0, LED_NUM_1, &ledreg[0]) < 0)
    {
        led_manager_unregister_module(hnd_LED_0);
        printk("ERROR DSL: Unable to register callbacks with LED module\n");
        hnd_LED_0 = NULL;
        return;
    }
#endif

    // register LED1 output with led module
    ledreg[1].led_init = (void *)tn7dsl_led_init;
    ledreg[1].led_on   = (void *)tn7dsl_led_on;
    ledreg[1].led_off  = (void *)tn7dsl_led_off;
    ledreg[1].param = 1;


    if( led_manager_install_callbacks(hnd_LED_0, LED_NUM_1, &ledreg[1]) < 0)
    {
        printk("ERROR DSL: Unable to register with LED module\n"); ;
        led_manager_unregister_module(hnd_LED_0);
        hnd_LED_0 = NULL;
    }
#endif
#ifdef CONFIG_LED_MODULE
    // register LED0 with led module
    ledreg[0].param = 2;
    ledreg[0].init = (void *)tn7dsl_led_init;
    ledreg[0].onfunc = (void *)tn7dsl_led_on;
    ledreg[0].offfunc = (void *)tn7dsl_led_off;

    // register LED1 output with led module
    ledreg[1].init = (void *)tn7dsl_led_init;
    ledreg[1].onfunc = (void *)tn7dsl_led_on;
    ledreg[1].offfunc = (void *)tn7dsl_led_off;
    ledreg[1].param = 1;

    /* Register with the old LED module */
    register_led_drv(LED_NUM_1, &ledreg[0]);
    register_led_drv(LED_NUM_2, &ledreg[1]);

    /* Indicate that the initialization has occurred. */
    hnd_LED_0 = (int *) 1;
#endif
}

static void tn7dsl_ctrl_fineGain(int value)
{
  int offset[2]={25,2},fineGain_f=0;

  //printk("gain control=%d\n", value);
  dslhal_api_dspInterfaceRead (pIhw, (unsigned int) pIhw->pmainAddr, 2,
                               (unsigned int *) &offset,
                               (unsigned char *) &fineGain_f, 4);
  if(value == 1)
  {
    fineGain_f |= dslhal_support_byteSwap32(0x01000000);
  }
  else
  {
    fineGain_f &= dslhal_support_byteSwap32(0x00ffffff);
  }
  dslhal_api_dspInterfaceWrite (pIhw, (unsigned int) pIhw->pmainAddr, 2,
                                (unsigned int *) &offset,
                                (unsigned char *) &fineGain_f, 4);
}

static void tn7dsl_set_fineGainValue(int value)
{
  int offset[2]={25,4};
  int fineGainVal=0;

  //printk("gain value =%d\n", value);
  if(value)
   {
    dslhal_api_dspInterfaceRead (pIhw, (unsigned int) pIhw->pmainAddr, 2,
                                 (unsigned int *) &offset,
                                 (unsigned char *) &fineGainVal, 4);
    fineGainVal &= dslhal_support_byteSwap32(0xffff0000);
    fineGainVal |= dslhal_support_byteSwap32((value & 0xffff));
    dslhal_api_dspInterfaceWrite (pIhw, (unsigned int) pIhw->pmainAddr, 2,
                                  (unsigned int *) &offset,
                                  (unsigned char *) &fineGainVal, 4);
   }
}

int tn7dsl_generic_read(int offsetnum, int *offset)
{

  dslhal_api_dspInterfaceRead (pIhw, (unsigned int) pIhw->pmainAddr,
                               offsetnum, (unsigned int *) offset,
                               (unsigned char *) &dslReg, 4);
  //printk("value = %d\n", dslReg);
  return 0;
}

int tn7dsl_generic_write( int offsetnum, int *offset, int data)
{
  dslhal_api_dspInterfaceWrite (pIhw, (unsigned int) pIhw->pmainAddr,
                                offsetnum, (unsigned int *) offset,
                                (unsigned char *) &data, 4);
  return 0;
}

int tn7dsl_proc_write_stats (struct file *fp, const char *buf,
                             unsigned long count, void *data)
{
    char local_buf[31];
    int  ret_val = 0;
    //UR8_MERGE_START CQ10700 MANJULAK
    struct atm_dev *dev;
    Tn7AtmPrivate *priv;
    dev = (struct atm_dev *)data;
    priv = (Tn7AtmPrivate *)dev->dev_data;
    //UR8_MERGE_END CQ10700

        if(count > 30)
        {
                printk("Error : Buffer Overflow\n");
                printk("Use \"echo 0> avsar_modem_stats \" to reset the statistics\n");
                return -1;
        }

        copy_from_user(local_buf,buf,count);
        local_buf[count-1]='\0'; /* Ignoring last \n char */
        ret_val = count;

        if(strcmp("0",local_buf)==0)
        {
        /* *INDENT-OFF* */

            /*
             * Valid command
             */
             printk("Resetting AAL5 statistics.\n");
             sarStat.txPktCnt =0;
             sarStat.rxPktCnt =0;
             sarStat.txBytes = 0;
             sarStat.rxBytes = 0;
             sarStat.txErrors =0;
             sarStat.rxErrors =0;

             //UR8_MERGE_START CQ10700 Manjula K
             priv->stats.tx_dropped = 0;
             priv->stats.tx_errors  = 0;
             priv->stats.rx_dropped = 0;
             priv->stats.rx_errors  = 0;
             //UR8_MERGE_END CQ10700


        /* *INDENT-ON* */
        }
        else
        {
                printk("\nError: Unknown operation on DSL/AAL5 statistics\n");
                printk("Use \"echo 0 > avsar_modem_stats\" to reset the statistics\n");
                return -1;
        }

        return ret_val;
}


int tn7dsl_proc_train_mode_export (char *buf, char **start, off_t offset,
                                   int count, int *eof, void *data)
{

  int len = 0;
  char *cp = buf + offset;
  int i = 0;
  static int ctr = 0;

  typedef struct
  {
    unsigned char mode_name[20];
    unsigned int mode_value;
  } dsl_modes_t;


  /* *INDENT-OFF* */
  /* AV: Relocated code for performance. */
  dsl_modes_t dsl_modes[] = {
  {"NO_MODE",               NO_MODE},
  {"ADSL_G.dmt",            OLD_TRAINING_VAL_GDMT},
  {"ADSL_G.lite",           GLITE_MODE},
  {"ADSL_G.dmt.bis",        ADSL2_MODE},
  {"ADSL_G.dmt.bis_DELT",   ADSL2_DELT},
  {"ADSL_2plus",            ADSL2_PLUS},
  {"ADSL_2plus_DELT",       ADSL2_PLUS_DELT},
  {"ADSL_re-adsl",          READSL_MODE},
  {"ADSL_re-adsl_DELT",     READSL_PLUS_DELT},
  {"ADSL_ANSI_T1.413",      OLD_TRAINING_VAL_T1413},
  {"MULTI_MODE",            OLD_TRAINING_VAL_MMODE},
  {"ADSL_G.dmt.bis_AnxI",   0},       // ADSL2 AnxI 0x0100
  {"ADSL_G.dmt.bis_AnxJ",   0},       // ADSL2 AnxJ 0x0200
  {"ADSL_G.dmt.bis_AnxM",   ADSL2_ANNEX_M},  // ADSL2 AnxM 0x0400
  {"ADSL_2plus_AnxI",       0},           // ADSL2+ AnxI 0x0800
  {"ADSL_2plus_AnxJ",       0},           // ADSL2+ AnxJ 0x1000
  {"ADSL_2plus_AnxM",       ADSL2_PLUS_ANNEX_M},      // ADSL2+ AnxM 0x2000
  {"G.shdsl",               0},
  {"IDSL",                  0},
  {"HDSL",                  0},
  {"SDSL",                  0},
  {"VDSL",                  0}
  };

  /* Offsets in the dsl_modes structure */
  #define G_DMT_OFFSET            1
  #define G_LITE_OFFSET           2
  #define G_DMT_BIS_OFFSET        3
  #define G_DMT_BIS_DELT_OFFSET   4
  #define ADSL_2PLUS_OFFSET       5
  #define ADSL_2PLUS__DELT_OFFSET 6
  #define ADSL_READSL_OFFSET      7
  #define ADSL_READSL_DELT_OFFSET 8
  #define T1413_OFFSET            9
  #define MMODE_OFFSET            10
  #define ADSL2_ANNEXM_OFFSET     13
  #define ADSL2PLUS_ANNEXM_OFFSET 16

  /* *INDENT-ON* */

  int num_entries = sizeof (dsl_modes) / sizeof (dsl_modes[0]);

  /* Check to see if we need to use the new bit mode scheme */
  if (pIhw->AppData.useBitField)
  {
    dsl_modes[G_DMT_OFFSET].mode_value = NEW_TRAINING_VAL_GDMT;
    dsl_modes[T1413_OFFSET].mode_value = NEW_TRAINING_VAL_T1413;
    dsl_modes[MMODE_OFFSET].mode_value = NEW_TRAINING_VAL_MMODE;
  }

  /* Check to see if we are in POTS or ISDN mode. */
  if (pIhw->netService == 2)    // 1=POTS, 2=ISDN
  {
    dsl_modes[ADSL2_ANNEXM_OFFSET].mode_value = 0;
    dsl_modes[ADSL2PLUS_ANNEXM_OFFSET].mode_value = 0;
  }

  /*
   * Change value, since we don't support these modes on this chip
   */
  if (!tn7atm_read_can_support_adsl2())
  {
    dsl_modes[ADSL_2PLUS_OFFSET].mode_value = 0;
    dsl_modes[ADSL_2PLUS__DELT_OFFSET].mode_value = 0;
    dsl_modes[ADSL_READSL_OFFSET].mode_value = 0;
    dsl_modes[ADSL_READSL_DELT_OFFSET].mode_value = 0;
  }


    if(len <= count)
    {
        for (i = ctr; ((i < num_entries)&& (len <= count)) ; i++)
        {
      /*
       * Write the current string only if we can fit it into the buffer
       */
            if((strlen(dsl_modes[i].mode_name) + 6 + len) <= count)
        {
                len += snprintf(cp+len, (count - len), "%s\t\t\t%#x\n",
                                    dsl_modes[i].mode_name, dsl_modes[i].mode_value);
            }
            else
                break;
        }
    }

  /*
   * Data was completely written
   */
    if (i >= num_entries)
    {
    /*
     * We are done with this
     */
        *eof = 1;
        ctr = 0;
    }
    else
    {
    /*
     * We have not been able to write the complete data, and we have to nul
     * terminate the buffer.
        */
        *(cp + len) = '\0';

    /*
     * Save the value of the counter for the next read for the rest of the
     * data.
        */
        ctr = i;
    }

    return len;
}

#ifndef NO_ADV_STATS
int tn7dsl_proc_SNRpsds(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len = 0;



  int limit = count - 80;
  int i;
  unsigned char SNRpsds[512];

  if(len<=limit)
    len += sprintf(buf+len, "\nAR7 SNRpsds:");

  if (dslhal_api_getSNRpsds(pIhw, SNRpsds, 1))
  {
    dgprintf(4, "dslhal_api_getSNRpsds failed!\n");
    return len;
  }

  for (i=0; i<pIhw->AppData.max_ds_tones; i++)
  {
    if (!(i%16))
    {
      if(len <=limit)
        len += sprintf(buf+len, "\n");
    }

    if(len <=limit)
      len += sprintf(buf+len, "%d ", (unsigned char)SNRpsds[i]);
  }

  if(len <=limit)
    len += sprintf(buf+len, "\n");



  return len;
}

#endif

#ifndef NO_ADV_STATS
int tn7dsl_proc_QLNpsds(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len = 0;

  int limit = count - 80;
  unsigned char QLNpsds[512];
  int i;

  if(len<=limit)
    len += sprintf(buf+len, "\nAR7 QLNpsds:");

  // call API instead of access internal buf directly
  if (dslhal_api_getQLNpsds(pIhw, QLNpsds, 0))
  {
    dgprintf(4, "dslhal_api_getQLNpsds failed!\n");
    return len;
  }

  for (i=0; i<pIhw->AppData.max_ds_tones; i++)
  {
    if (!(i%16))
    {
      if(len <=limit)
        len += sprintf(buf+len, "\n");
    }

    if(len <=limit)
      len += sprintf(buf+len, "%d ", (unsigned char)QLNpsds[i]);
  }

  if(len <=limit)
    len += sprintf(buf+len, "\n");


  return len;
}
#endif

//  UR8_MERGE_START CQ10979   Jack Zhang
#ifdef TR69_HLIN_IN
#ifndef NO_ADV_STATS
int tn7dsl_proc_HLINpsds(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len = 0;

  int limit = count - 80;
  short HLINpsds[2*512];
  int i;

  if(len<=limit)
    len += sprintf(buf+len, "\nAR7 HLINpsds:");

  // call API instead of access internal buf directly
  if (dslhal_api_getHLINpsds(pIhw, (unsigned char *)HLINpsds, 1))
  {
    dgprintf(4, "dslhal_api_getHLINpsds failed!\n");
    return len;
  }

  for (i=0; i<pIhw->AppData.max_ds_tones; i++)
  {
    if (!(i%8))
    {
      if(len <=limit)
        len += sprintf(buf+len, "\n");
    }

    if(len <=limit)
      len += sprintf(buf+len, "(%d,%d) ", HLINpsds[2*i], HLINpsds[2*i+1]);
  }

  if(len <=limit)
    len += sprintf(buf+len, "\n");


  return len;
}

static int tn7dsl_proc_HLINpsdsIndx(char* buf, char **start, off_t offset, int count,int *eof, void *data, int indx)
{
  int len = 0;

  int limit = count - 80;
  short HLINpsds[2*512];
  int i;
  int start=0, dim=128;

  if(len<=limit)
    len += sprintf(buf+len, "\nAR7 HLINpsds: (section %d)", indx);

  if((indx > 2) && (pIhw->AppData.max_ds_tones <= 256))
  {
    if(len <=limit)
      len += sprintf(buf+len, "\n[End of data]");
    return len;
  }

  // call API instead of access internal buf directly
  if (dslhal_api_getHLINpsds(pIhw, (unsigned char *)HLINpsds, 1))
  {
    dgprintf(4, "dslhal_api_getHLINpsds failed!\n");
    return len;
  }

  start = (indx -1) * 128;

  for (i=start; i<(start+dim); i++)
  {
    if (!(i%8))
    {
      if(len <=limit)
        len += sprintf(buf+len, "\n%d: ", i);
    }

    if(len <=limit)
      len += sprintf(buf+len, "(%d,%d) ", HLINpsds[2*i], HLINpsds[2*i+1]);
  }

  if(len <=limit)
    len += sprintf(buf+len, "\n");

  return len;
}

int tn7dsl_proc_HLINpsds1(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  return tn7dsl_proc_HLINpsdsIndx(buf, start, offset, count,eof, data, 1);
}

int tn7dsl_proc_HLINpsds2(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  return tn7dsl_proc_HLINpsdsIndx(buf, start, offset, count,eof, data, 2);
}

int tn7dsl_proc_HLINpsds3(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  return tn7dsl_proc_HLINpsdsIndx(buf, start, offset, count,eof, data, 3);
}

int tn7dsl_proc_HLINpsds4(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  return tn7dsl_proc_HLINpsdsIndx(buf, start, offset, count,eof, data, 4);
}
#endif
#endif //TR69_HLIN_IN
//  UR8_MERGE_END   CQ10979*

// *    UR8_MERGE_START CQ11057   Jack Zhang
#ifdef TR69_PMD_IN
#ifndef NO_ADV_STATS
int tn7dsl_proc_PMDus(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
  int len = 0;

  int limit = count - 80;
  int i;
  CoPMDTestParams_t  co_pmdtest_params;

  if(len<=limit)
    len += sprintf(buf+len, "\nAR7 US PMD Test:\n");

  // call API instead of access internal buf directly
  if (dslhal_api_getPMDTestus(pIhw, &co_pmdtest_params, 0) != DSLHAL_ERROR_NO_ERRORS)
  {
    dgprintf(4, "dslhal_api_getPMDTestus failed!\n");
    return len;
  }

  if(len<=limit)
    len += sprintf(buf+len, "LATN=%d\n", co_pmdtest_params.co_latn);

  if(len<=limit)
    len += sprintf(buf+len, "SATN=%d\n", co_pmdtest_params.co_satn);

  if(len<=limit)
    len += sprintf(buf+len, "SNRM=%d\n", co_pmdtest_params.usMargin);

  if(len<=limit)
    len += sprintf(buf+len, "attndr=%ld\n", co_pmdtest_params.co_attndr);

  if(len<=limit)
    len += sprintf(buf+len, "NearActatp=%d\n", co_pmdtest_params.co_near_actatp);

  if(len<=limit)
    len += sprintf(buf+len, "FarActatp=%d\n", co_pmdtest_params.co_far_actatp);

  //HLOG
  for (i=0; i<pIhw->AppData.max_us_tones; i++)
  {
    if (!(i%16))
    {
      if(len <=limit)
        len += sprintf(buf+len, "\nHLOG(%3d):", i);
    }
    if(len <=limit)
      len += sprintf(buf+len, " %d", co_pmdtest_params.TestParmCOHlogfMsg[i]);
  }

  //QLN
  for (i=0; i<pIhw->AppData.max_us_tones; i++)
  {
    if (!(i%16))
    {
      if(len <=limit)
        len += sprintf(buf+len, "\nQLN(%3d):", i);
    }
    if(len <=limit)
      len += sprintf(buf+len, " %d", co_pmdtest_params.TestParmCOQLNfMsg[i]);

  }

  //SNR
  for (i=0; i<pIhw->AppData.max_us_tones; i++)
  {
    if (!(i%16))
    {
      if(len <=limit)
        len += sprintf(buf+len, "\nSNR(%3d):", i);
    }
    if(len <=limit)
      len += sprintf(buf+len, " %d", co_pmdtest_params.TestParmCOSNRfMsg[i]);
  }

  if(len <=limit)
    len += sprintf(buf+len, "\n");

  return len;
}
#endif //NO_ADV_STATS
#endif //TR69_PMD_IN
// *    UR8_MERGE_END   CQ11057 *
//UR8_MERGE_START CQ11813 Hao-Ting Lin
#ifdef LINUX_CLI_SUPPORT
//*************************************************************
//
//  int tn7dsl_proc_dbgmsg_read(...)
//
//  DESCRIPTION: print the debug message from CLI redirect buffer
//
//**************************************************************
int tn7dsl_proc_dbgmsg_read(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
   int len = 0;
   int i;
   unsigned int writePointer;
   int delta;
   int limit = count - 80;

   if(pIhw->AppData.cliRedirect == 1)
   {
     writePointer = DSLHAL_REG32((unsigned int)(pIhw->AppData.p_cliBuffAddr + pIhw->AppData.cliBuffsize));
     delta = writePointer - (pIhw->AppData.readPointer + 1);

     if (delta < 0)
       delta += pIhw->AppData.cliBuffsize;

     for(i=0;i<delta;i++)
     {
       pIhw->AppData.readPointer++;
       if (pIhw->AppData.readPointer == pIhw->AppData.cliBuffsize)
          pIhw->AppData.readPointer = 0;
       //since the debug message is already U8, we don't have to translate format using sprintf
       //And it would save resources by doing so.
       //len += sprintf(buf+len, "%c", pIhw->AppData.p_cliBuffAddr[pIhw->AppData.readPointer]);
       if(len <= limit)
       {
         len++;
         buf[i] = pIhw->AppData.p_cliBuffAddr[pIhw->AppData.readPointer];
       }
     }
   }
   return len;
}

//*************************************************************
//
//  int tn7dsl_proc_dbgmsg_write(...)
//
//  DESCRIPTION: Enable/Disable CLI redirect
//
//**************************************************************
int tn7dsl_proc_dbgmsg_write(struct file *fp, const char *buf, unsigned long count, void *data)
{
    char local_buf[10];
    int  ret_val = 0;
    struct atm_dev *dev;
    Tn7AtmPrivate *priv;

    dev = (struct atm_dev *)data;
    priv = (Tn7AtmPrivate *)dev->dev_data;
    ret_val = count;

    copy_from_user(local_buf,buf,count);

    if(strcmp("0",local_buf)==0)
    {
       printk("\nDISABLE debug message redirect\n");
       if(pIhw->AppData.cliRedirect == 1)
       {
         dslhal_api_redirectMode(pIhw, 0);
         dslhal_api_redirectFree(pIhw);
       }
    }
    else if(strcmp("1",local_buf)==0)
    {
       printk("\nENABLE debug message redirect\n");
       if(pIhw->AppData.cliRedirect == 0)
       {
         dslhal_api_redirectInit(pIhw);
         dslhal_api_redirectMode(pIhw, 1);
       }
    }
    else
    {
       printk("\nError: Unknown operation\n");
       printk("use echo x > avsar_dbg_enable\n");
       printk("0: disable, 1: enable");
    }

    return ret_val;
}
#endif
//UR8_MERGE_END CQ11813 Hao-Ting Lin
