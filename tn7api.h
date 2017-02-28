/**********************************************************************************
 *    Tnetd73xx ATM driver.
 *    by Zhicheng Tang, ztang@ti.com
 *    2000 (c) Texas Instruments Inc.
 *
 *    08/15/05 AV      Added a function to get the capability of the chip,
 *                     tn7atm_read_can_support_adsl2.
 *    09/06/05 AV      Adding support for VBR-rt and VBR-nrt modes.(CQ9808).
 *    09/12/05 AV      Moved the invalidation of the cache to a central place.
 *    09/21/05 AV      Renamed the overloaded memcmp and memcpy functions to
 *                     tn7atm_memcmp and tn7atm_memcpy respectively to allow support
 *                     for being compiled directly into the kernel.
 *    10/07/05  AV/CPH Changed dprintf to dgprintf and compile time
 *                     selection of dslhal_api_getQLNpsds() dslhal_api_getSNRpsds()
 *                     and for code size reduction.
 *    11/03/05  CPH    Add ClearEoc ACK.
 *    01/25/06  JZ     CQ: 10273 Aztech/Singtel oam pvc management issue, new PDSP 0.54
 *    01/25/06  JZ     CQ: 10275 Add Remote Data Log code. Search ADV_DIAG_STATS in ATM  
 *                     driver code and manually turn on it for enabling the feature
 *    UR8_MERGE_START CQ10682   Jack Zhang
 *    6/15/06  JZ   CQ10682: Display ADSL training messages in ATM driver proc file system
 *    UR8_MERGE_END   CQ10682*
 *    UR8_MERGE_START CQ10979   Jack Zhang
 *    10/4/06  JZ     CQ10979: Request for TR-069 Support for RP7.1
 *    UR8_MERGE_END   CQ10979*
 ************************************************************************************/

#ifndef __SAPI_H
#define __SAPI_H

#define NUM_ELEMS(x)                 (sizeof(x)/sizeof(x[0]))

#define CLEAR_EOC_VPI         0
#define CLEAR_EOC_VCI         11

#define REMOTE_MGMT_VPI       0
#define REMOTE_MGMT_VCI       16

/*
 * Max clear eoc msgsize must be equal to
 * OVHD_clearEOC_hostBufSize, which is defined
 * <dev_host_interface.h>
 */
#define MAX_CLEAR_EOC_BUF_SIZE   600

/* tn7atm.c */
#ifdef DEBUG_BUILD
extern void xadump(unsigned char *buff, int len, int debugLev);
#define xdump(buff, len, debugLev) xadump(buff, len, debugLev)
#else
#define xdump(buff, len, debugLev)
#endif

/*
#ifdef DEBUG_BUILD
#define dgprintf(uDbgLevel, szFmt, ...) dprintf(uDbgLevel, szFmt, ...)
#else
#define dgprintf(uDbgLevel, szFmt, ...)
#endif
*/

#ifdef DEBUG_BUILD
#define dgprintf dprintf
#else
#define dgprintf(uDbgLevel, szFmt, args...)
#endif

void dprintf( int uDbgLevel, char * szFmt, ...);

int tn7atm_receive(void *os_dev, int ch, unsigned int packet_size, void *os_receive_info, void *data);
void *tn7atm_allocate_rx_skb(void *os_dev, void **os_receive_info, unsigned int size, void *vcc);
extern void tn7atm_free_rx_skb(void *skb, void *pmem);
void tn7atm_sarhal_isr_register(void *os_dev, void *hal_isr, int interrupt_num);
int tn7atm_send_complete(void *osSendInfo);
int tn7atm_device_connect_status(void *priv, int state);
int tn7atm_lut_find(short vpi, int vci);
extern int tn7atm_read_can_support_adsl2(void);
void tn7atm_data_writeback(void *pmem, int size);
void tn7atm_data_invalidate(void *pmem, int size);
int tn7atm_memcmp(void * dst, void const * src, size_t len);
void * tn7atm_memcpy(void * dst, void const * src, size_t len);

/* tn7dsl.h */
void tn7dsl_exit(void);
int tn7dsl_init(void *priv);
int tn7dsl_proc_eoc(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_stats(char* buf, char **start, off_t offset, int count,int *eof, void *data);

//#define ADV_DIAG_STATS  1  //CQ10275 To enable Adv Stats

#ifdef ADV_DIAG_STATS
int tn7dsl_proc_adv_stats(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_adv_stats1(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_adv_stats2(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_adv_stats3(char* buf, char **start, off_t offset, int count,int *eof, void *data);
//UR8_MERGE_START CQ10682   Jack Zhang
int tn7dsl_proc_dbg_cmsgs(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_dbg_rmsgs1(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_dbg_rmsgs2(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_dbg_rmsgs3(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_dbg_rmsgs4(char* buf, char **start, off_t offset, int count,int *eof, void *data);
//UR8_MERGE_END   CQ10682*
#endif //ADV_DIAG_STATS

int tn7dsl_proc_write_stats(struct file *fp, const char * buf, unsigned long count, void * data);
int tn7dsl_proc_modem(char* buf, char **start, off_t offset, int count,int *eof, void *data);
inline int tn7dsl_handle_interrupt(void);

void tn7dsl_dslmod_sysctl_register(void);
void tn7dsl_dslmod_sysctl_unregister(void);
int tn7dsl_get_dslhal_version(char *pVer);
int tn7dsl_get_dsp_version(char *pVer);
int tn7dsl_generic_read(int offsetnum, int *offset);
int tn7dsl_generic_write( int offsetnum, int *offset, int data);
int tn7dsl_clear_eoc_setup(void);
int tn7dsl_clear_eoc_close(void);
int tn7dsl_clear_eoc_send(unsigned char *data, unsigned int len, int ackFlag);

//CQ10273: Export to tn7atm.c for oam ping loss -- JZ 12/03/05
void tn7dsl_write_memory(char *input_str);
int tn7dsl_dump_memory(char *input_str);
unsigned int tn7dsl_get_memory(unsigned int addr_in);

int os_atoi(const char *pStr);
int os_atoh(const char *pStr);
unsigned long os_atoul(const char *pStr);
int tn7dsl_proc_snr0(char* buf, char **start, off_t offset, int count, int *eof, void *data);
int tn7dsl_proc_snr1(char* buf, char **start, off_t offset, int count, int *eof, void *data);
int tn7dsl_proc_snr2(char* buf, char **start, off_t offset, int count, int *eof, void *data);
int tn7dsl_proc_bit_allocation(char* buf, char **start, off_t offset, int count, int *eof, void *data);
int tn7dsl_proc_ds_noise(char* buf, char **start, off_t offset, int count, int *eof, void *data);
int tn7dsl_proc_generic_read_result(char* buf, char **start, off_t offset, int count, int *eof, void *data);
int tn7dsl_proc_train_mode_export(char* buf, char **start, off_t offset, int count,int *eof, void *data);

#ifndef NO_ADV_STATS
int tn7dsl_proc_SNRpsds(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_QLNpsds(char* buf, char **start, off_t offset, int count,int *eof, void *data);
// *    UR8_MERGE_START CQ10979   Jack Zhang
#ifdef TR69_HLIN_IN
//int tn7dsl_proc_HLINpsds(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_HLINpsds1(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_HLINpsds2(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_HLINpsds3(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_HLINpsds4(char* buf, char **start, off_t offset, int count,int *eof, void *data);
#endif //TR69_HLIN_IN
// *    UR8_MERGE_END   CQ10979*
#endif

/* tn7sar.c */
int tn7sar_activate_vc(tn7atm_activate_vc_parm_t* tn7atm_activate_vc_parm_p);
int tn7sar_init(struct atm_dev *dev, Tn7AtmPrivate *priv);
int tn7sar_register_interrupt_handle(void *os_dev, void *hal_isr, int *interrupt_num);
void tn7sar_exit(struct atm_dev *dev, Tn7AtmPrivate *priv);
int tn7sar_deactivate_vc(Tn7AtmPrivate *priv, int chan);
int tn7sar_handle_interrupt(struct atm_dev *dev, Tn7AtmPrivate *priv, int *more);
int tn7sar_send_packet(Tn7AtmPrivate *priv, int chan, void *new_skb, void *data,unsigned int len, int priority);
void tn7sar_get_sar_version(Tn7AtmPrivate *priv, char **pVer);
int tn7sar_get_near_end_loopback_count(unsigned int *pF4count, unsigned int *pF5count);
int tn7sar_oam_generation(void *privContext, int chan, int type, int vpi, int vci, int timeout);
int tn7sar_get_stats(void *priv1);
int tn7sar_proc_sar_stat(char* buf, char **start, off_t offset, int count,int *eof, void *data);
void tn7sar_get_sar_firmware_version(unsigned int *pdsp_version_ms, unsigned int *pdsp_version_ls);
int tn7sar_proc_oam_ping(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7sar_proc_pvc_table(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7sar_tx_flush(void *privContext, int chan, int queue, int skip);
#endif __SGAPI_H
