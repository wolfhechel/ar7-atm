/* 
 *    Tnetd73xx ATM driver.
 *    by Zhicheng Tang, ztang@ti.com
 *    2000 (c) Texas Instruments Inc.
 *
 *
*/

#ifndef __SAPI_H
#define __SAPI_H

/* tn7atm.c */
void xdump(unsigned char *buff, int len, int debugLev);
int tn7atm_receive(void *os_dev, int ch, unsigned int packet_size, void *os_receive_info, void *data);
void *tn7atm_allocate_rx_skb(void *os_dev, void **os_receive_info, unsigned int size);
void tn7atm_free_rx_skb(void *skb);
void tn7atm_sarhal_isr_register(void *os_dev, void *hal_isr, int interrupt_num);
int tn7atm_send_complete(void *osSendInfo);
int tn7atm_device_connect_status(void *priv, int state);
int tn7atm_lut_find(short vpi, int vci);

/* tn7dsl.h */
void tn7dsl_exit(void);
int tn7dsl_init(void *priv);
int tn7dsl_proc_stats(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_proc_modem(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7dsl_handle_interrupt(void);
void dprintf( int uDbgLevel, char * szFmt, ...);
void tn7dsl_dslmod_sysctl_register(void);
void tn7dsl_dslmod_sysctl_unregister(void);
int tn7dsl_get_dslhal_version(char *pVer);
int tn7dsl_get_dsp_version(char *pVer);

int os_atoi(const char *pStr);
int os_atoh(const char *pStr);
unsigned long os_atoul(const char *pStr);

/* tn7sar.c */
int tn7sar_activate_vc(Tn7AtmPrivate *priv, short vpi, int vci, int pcr, int scr, int mbs, int cdvt, int chan, int qos);
int tn7sar_init(struct atm_dev *dev, Tn7AtmPrivate *priv);
int tn7sar_register_interrupt_handle(void *os_dev, void *hal_isr, int *interrupt_num);
void tn7sar_exit(struct atm_dev *dev, Tn7AtmPrivate *priv);
int tn7sar_deactivate_vc(Tn7AtmPrivate *priv, int chan);
int tn7sar_handle_interrupt(struct atm_dev *dev, Tn7AtmPrivate *priv);
int tn7sar_send_packet(Tn7AtmPrivate *priv, int chan, void *new_skb, void *data,unsigned int len, int priority);
void tn7sar_get_sar_version(Tn7AtmPrivate *priv, char **pVer);
int tn7sar_get_near_end_loopback_count(unsigned int *pF4count, unsigned int *pF5count);
int tn7sar_oam_generation(void *privContext, int chan, int type, int vpi, int vci, int timeout);
int tn7sar_get_stats(void *priv1);
int tn7sar_proc_sar_stat(char* buf, char **start, off_t offset, int count,int *eof, void *data);
void tn7sar_get_sar_firmware_version(unsigned int *pdsp_version_ms, unsigned int *pdsp_version_ls);
int tn7sar_proc_oam_ping(char* buf, char **start, off_t offset, int count,int *eof, void *data);
int tn7sar_proc_pvc_table(char* buf, char **start, off_t offset, int count,int *eof, void *data);
#endif
