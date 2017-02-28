/***************************************************************************
        TNETD73xx Software Support
        Copyright(c) 2000, Texas Instruments Incorporated. All Rights Reserved.

        FILE: cp_sar_reg.h   Register definitions for the SAR module

        DESCRIPTION:
                This include file contains register definitions for the
                SAR module.

        HISTORY:
                15 Jan 02    G. Guyotte   Original version written
                03 Oct 02    G. Guyotte   C++ style comments removed
****************************************************************************/
#ifndef _INC_SAR_REG
#define _INC_SAR_REG

/* Global Registers */
#define pSAR_ID_REG(base)                   ((volatile bit32u *)(base+0x0000))
#define SAR_ID_REG(base)                    (*pSAR_ID_REG(base))
#define pSAR_STATUS_SET_REG(base)           ((volatile bit32u *)(base+0x0008))
#define SAR_STATUS_SET_REG(base)            (*pSAR_STATUS_SET_REG(base))
#define pSAR_STATUS_CLR_REG(base)           ((volatile bit32u *)(base+0x000C))
#define SAR_STATUS_CLR_REG(base)            (*pSAR_STATUS_CLR_REG(base))
#define pSAR_HOST_INT_EN_SET_REG(base)      ((volatile bit32u *)(base+0x0010))
#define SAR_HOST_INT_EN_SET_REG(base)       (*pSAR_HOST_INT_EN_SET_REG(base))
#define pSAR_HOST_INT_EN_CLR_REG(base)      ((volatile bit32u *)(base+0x0014))
#define SAR_HOST_INT_EN_CLR_REG(base)       (*pSAR_HOST_INT_EN_CLR_REG(base))
#define pSAR_PDSP_INT_EN_SET_REG(base)      ((volatile bit32u *)(base+0x0018))
#define SAR_PDSP_INT_EN_SET_REG(base)       (*pSAR_PDSP_INT_EN_SET_REG(base))
#define pSAR_PDSP_INT_EN_CLR_REG(base)      ((volatile bit32u *)(base+0x001C))
#define SAR_PDSP_INT_EN_CLR_REG(base)       (*pSAR_PDSP_INT_EN_CLR_REG(base))

/* PDSP OAM General Purpose Registers */
#define pSAR_PDSP_HOST_OAM_CONFIG_REG(base) ((volatile bit32u *)(base+0x0020))
#define SAR_PDSP_HOST_OAM_CONFIG_REG(base)  (*pSAR_PDSP_HOST_OAM_CONFIG_REG(base))
#define pSAR_PDSP_OAM_CORR_REG(base)        ((volatile bit32u *)(base+0x0024))
#define SAR_PDSP_OAM_CORR_REG(base)         (*pSAR_PDSP_OAM_CORR_REG(base))
#define pSAR_PDSP_OAM_LB_RESULT_REG(base)   ((volatile bit32u *)(base+0x0028))
#define SAR_PDSP_OAM_LB_RESULT_REG(base)    (*pSAR_PDSP_OAM_LB_RESULT_REG(base))
#define pSAR_PDSP_OAM_F5_LB_COUNT_REG(base) ((volatile bit32u *)(base+0x002c))     /* +GSG 030416 */
#define SAR_PDSP_OAM_F5_LB_COUNT_REG(base)  (*pSAR_PDSP_OAM_F5_LB_COUNT_REG(base)) /* +GSG 030416 */
#define pSAR_PDSP_OAM_F4_LB_COUNT_REG(base) ((volatile bit32u *)(base+0x0030))     /* +GSG 030416 */
#define SAR_PDSP_OAM_F4_LB_COUNT_REG(base)  (*pSAR_PDSP_OAM_F4_LB_COUNT_REG(base)) /* +GSG 030416 */
#define pSAR_PDSP_FWD_UNK_VC_REG(base)      ((volatile bit32u *)(base+0x0034))     /* +GSG 030701 */
#define SAR_PDSP_FWD_UNK_VC_REG(base)       (*pSAR_PDSP_FWD_UNK_VC_REG(base))      /* +GSG 030701 */


/* Rx Lookup Table Registers */
#define pRX_LUT_GLOBAL_CFG_REG(base)        ((volatile bit32u *)(base+0x0080))
#define RX_LUT_GLOBAL_CFG_REG(base)         (*pRX_LUT_GLOBAL_CFG_REG(base))
#define pRX_LUT_CH_SETUP_REQ_REG(base)      ((volatile bit32u *)(base+0x0090))
#define RX_LUT_CH_SETUP_REQ_REG(base)       (*pRX_LUT_CH_SETUP_REQ_REG(base))
#define pRX_LUT_CH_SETUP_REQ_VC_REG(base)   ((volatile bit32u *)(base+0x0094))
#define RX_LUT_CH_SETUP_REQ_VC_REG(base)    (*pRX_LUT_CH_SETUP_REQ_VC_REG(base))
#define pRX_LUT_CH_TEARDOWN_REQ_REG(base)   ((volatile bit32u *)(base+0x009C))
#define RX_LUT_CH_TEARDOWN_REQ_REG(base)    (*pRX_LUT_CH_TEARDOWN_REQ_REG(base))

/* Tx Scheduler Registers */
#define pTX_CH_MAPPING_REG(base)            ((volatile bit32u *)(base+0x0170))
#define TX_CH_MAPPING_REG(base)             (*pTX_CH_MAPPING_REG(base))

/* Tx CPPI DMA Controller Registers */
#define pTX_CPPI_CTL_REG(base)              ((volatile bit32u *)(base+0x0700))
#define TX_CPPI_CTL_REG(base)               (*pTX_CPPI_CTL_REG(base))
#define pTX_CPPI_TEARDOWN_REG(base)         ((volatile bit32u *)(base+0x0704))
#define TX_CPPI_TEARDOWN_REG(base)          (*pTX_CPPI_TEARDOWN_REG(base))

/* EOI Interrupt Additions */
#define pSAR_EOI(base)                      ((volatile bit32u *)(base+0x0708))
#define SAR_EOI(base)                       (*pSAR_EOI(base))
#define pSAR_INTR_VECTOR(base)              ((volatile bit32u *)(base+0x070c))
#define SAR_INTR_VECTOR(base)               (*pSAR_INTR_VECTOR(base))
#define pSAR_TX_MASKED_STATUS(base)         ((volatile bit32u *)(base+0x0710))
#define SAR_TX_MASKED_STATUS(base)          (*pSAR_TX_MASKED_STATUS(base))
#define pSAR_TX_RAW_STATUS(base)            ((volatile bit32u *)(base+0x0714))
#define SAR_TX_RAW_STATUS(base)             (*pSAR_TX_RAW_STATUS(base))
#define pSAR_TX_MASK_SET(base)              ((volatile bit32u *)(base+0x0718))
#define SAR_TX_MASK_SET(base)               (*pSAR_TX_MASK_SET(base))
#define pSAR_TX_MASK_CLR(base)              ((volatile bit32u *)(base+0x071c))
#define SAR_TX_MASK_CLR(base)               (*pSAR_TX_MASK_CLR(base))

/* Rx CPPI DMA Controller Registers */
#define pRX_CPPI_CTL_REG(base)              ((volatile bit32u *)(base+0x0780))
#define RX_CPPI_CTL_REG(base)               (*pRX_CPPI_CTL_REG(base))
#define pSAR_RX_MASKED_STATUS(base)         ((volatile bit32u *)(base+0x0790))
#define SAR_RX_MASKED_STATUS(base)          (*pSAR_RX_MASKED_STATUS(base))
#define pSAR_RX_RAW_STATUS(base)            ((volatile bit32u *)(base+0x0794))
#define SAR_RX_RAW_STATUS(base)             (*pSAR_RX_RAW_STATUS(base))
#define pSAR_RX_MASK_SET(base)              ((volatile bit32u *)(base+0x0798))
#define SAR_RX_MASK_SET(base)               (*pSAR_RX_MASK_SET(base))
#define pSAR_RX_MASK_CLR(base)              ((volatile bit32u *)(base+0x079c))
#define SAR_RX_MASK_CLR(base)               (*pSAR_RX_MASK_CLR(base))

/* PDSP Control/Status Registers */
#define pPDSP_CTRL_REG(base)                ((volatile bit32u *)(base+0x4000))
#define PDSP_CTRL_REG(base)                 (*pPDSP_CTRL_REG(base))

/* PDSP Instruction RAM */
#define pPDSP_IRAM(base)                    ((volatile bit32u *)(base+0x4020))
#define PDSP_IRAM(base)                     (*pPDSP_IRAM(base))

/*
 * Channel 0 State/Scratchpad RAM Block
 * 
 * The following registers (Tx DMA State, Rx DMA State, CPPI Completion PTR, 
 * and PDSP Data) have been given the correct address for channel 0.  To
 * reach the registers for channel X, add (X * 0x100) to the pointer address.
 *
 */

#define PDSP_STATE_RAM_SIZE   1024

/* Tx DMA State RAM */
#define pTX_DMA_STATE_WORD_0(base)          ((volatile bit32u *)(base+0x8000))
#define TX_DMA_STATE_WORD_0(base)           (*pTX_DMA_STATE_WORD_0(base))
#define pTX_DMA_STATE_WORD_1(base)          ((volatile bit32u *)(base+0x8004))
#define TX_DMA_STATE_WORD_1(base)           (*pTX_DMA_STATE_WORD_1(base))
#define pTX_DMA_STATE_WORD_2(base)          ((volatile bit32u *)(base+0x8008))
#define TX_DMA_STATE_WORD_2(base)           (*pTX_DMA_STATE_WORD_2(base))
#define pTX_DMA_STATE_WORD_3(base)          ((volatile bit32u *)(base+0x800C))
#define TX_DMA_STATE_WORD_3(base)           (*pTX_DMA_STATE_WORD_3(base))
#define pTX_DMA_STATE_WORD_4(base)          ((volatile bit32u *)(base+0x8010))
#define TX_DMA_STATE_WORD_4(base)           (*pTX_DMA_STATE_WORD_4(base))
#define pTX_DMA_STATE_WORD_5(base)          ((volatile bit32u *)(base+0x8014))
#define TX_DMA_STATE_WORD_5(base)           (*pTX_DMA_STATE_WORD_5(base))
#define pTX_DMA_STATE_WORD_6(base)          ((volatile bit32u *)(base+0x8018))
#define TX_DMA_STATE_WORD_6(base)           (*pTX_DMA_STATE_WORD_6(base))
#define pTX_DMA_STATE_WORD_7(base)          ((volatile bit32u *)(base+0x801C))
#define TX_DMA_STATE_WORD_7(base)           (*pTX_DMA_STATE_WORD_7(base))
#define pTX_DMA_STATE_WORD_8(base)          ((volatile bit32u *)(base+0x8020))
#define TX_DMA_STATE_WORD_8(base)           (*pTX_DMA_STATE_WORD_8(base))

/* Rx DMA State RAM */
#define pRX_DMA_STATE_WORD_0(base)          ((volatile bit32u *)(base+0x8024))
#define RX_DMA_STATE_WORD_0(base)           (*pRX_DMA_STATE_WORD_0(base))
#define pRX_DMA_STATE_WORD_1(base)          ((volatile bit32u *)(base+0x8028))
#define RX_DMA_STATE_WORD_1(base)           (*pRX_DMA_STATE_WORD_1(base))
#define pRX_DMA_STATE_WORD_2(base)          ((volatile bit32u *)(base+0x802C))
#define RX_DMA_STATE_WORD_2(base)           (*pRX_DMA_STATE_WORD_2(base))
#define pRX_DMA_STATE_WORD_3(base)          ((volatile bit32u *)(base+0x8030))
#define RX_DMA_STATE_WORD_3(base)           (*pRX_DMA_STATE_WORD_3(base))
#define pRX_DMA_STATE_WORD_4(base)          ((volatile bit32u *)(base+0x8034))
#define RX_DMA_STATE_WORD_4(base)           (*pRX_DMA_STATE_WORD_4(base))
#define pRX_DMA_STATE_WORD_5(base)          ((volatile bit32u *)(base+0x8038))
#define RX_DMA_STATE_WORD_5(base)           (*pRX_DMA_STATE_WORD_5(base))
#define pRX_DMA_STATE_WORD_6(base)          ((volatile bit32u *)(base+0x803C))
#define RX_DMA_STATE_WORD_6(base)           (*pRX_DMA_STATE_WORD_6(base))

/* Tx CPPI Completion Pointers */
#define pTXH_CPPI_COMP_PTR(base)            ((volatile bit32u *)(base+0x8040))
#define TXH_CPPI_COMP_PTR(base)             (*pTXH_CPPI_COMP_PTR(base))
#define pTXL_CPPI_COMP_PTR(base)            ((volatile bit32u *)(base+0x8044))
#define TXL_CPPI_COMP_PTR(base)             (*pTXL_CPPI_COMP_PTR(base))

/* Rx CPPI Completion Pointer */
#define pRX_CPPI_COMP_PTR(base)             ((volatile bit32u *)(base+0x8048))
#define RX_CPPI_COMP_PTR(base)              (*pRX_CPPI_COMP_PTR(base))

/* Tx PDSP Defines */
#define NUM_PDSP_AAL5_STATE_WORDS           24
#define NUM_PDSP_AAL2_STATE_WORDS           20

/* PDSP State RAM Block 0 */
#define pPDSP_BLOCK_0(base)                 ((volatile bit32u *)(base+0x8050))
#define PDSP_BLOCK_0(base)                  (*pPDSP_BLOCK_0(base))

/* AAL5 Tx PDSP State RAM */
#define pPDSP_AAL5_TX_STATE_WORD_0(base)    ((volatile bit32u *)(base+0x8050))
#define PDSP_AAL5_TX_STATE_WORD_0(base)     (*pPDSP_AAL5_TX_STATE_WORD_0(base))

/* AAL5 Rx PDSP State RAM */
#define pPDSP_AAL5_RX_STATE_WORD_0(base)    ((volatile bit32u *)(base+0x8070))
#define PDSP_AAL5_RX_STATE_WORD_0(base)     (*pPDSP_AAL5_RX_STATE_WORD_0(base))

/* AAL5 Tx VP PDSP State RAM */
#define pPDSP_AAL5_TX_VP_STATE_WORD_0(base) ((volatile bit32u *)(base+0x8090))
#define PDSP_AAL5_TX_VP_STATE_WORD_0(base)  (*pPDSP_AAL5_TX_VP_STATE_WORD_0(base))

/* AAL5 Rx VP PDSP State RAM */
#define pPDSP_AAL5_RX_VP_STATE_WORD_0(base) ((volatile bit32u *)(base+0x80A0))
#define PDSP_AAL5_RX_VP_STATE_WORD_0(base)  (*pPDSP_AAL5_RX_VP_STATE_WORD_0(base))

/* AAL2 Tx PDSP State RAM */
#define pPDSP_AAL2_TX_STATE_WORD_0(base)    ((volatile bit32u *)(base+0x80B0))
#define PDSP_AAL2_TX_STATE_WORD_0(base)     (*pPDSP_AAL2_TX_STATE_WORD_0(base))

/* AAL2 Rx PDSP State RAM */
#define pPDSP_AAL2_RX_STATE_WORD_0(base)    ((volatile bit32u *)(base+0x80D0))
#define PDSP_AAL2_RX_STATE_WORD_0(base)     (*pPDSP_AAL2_RX_STATE_WORD_0(base))

/* AAL2 Tx VP PDSP State RAM */
#define pPDSP_AAL2_TX_VP_STATE_WORD_0(base) ((volatile bit32u *)(base+0x80E0))
#define PDSP_AAL2_TX_VP_STATE_WORD_0(base)  (*pPDSP_AAL2_TX_VP_STATE_WORD_0(base))

/* AAL2 Rx VP PDSP State RAM */
#define pPDSP_AAL2_RX_VP_STATE_WORD_0(base) ((volatile bit32u *)(base+0x80F0))
#define PDSP_AAL2_RX_VP_STATE_WORD_0(base)  (*pPDSP_AAL2_RX_VP_STATE_WORD_0(base))

/* PDSP OAM Configuration Block */
#define pOAM_CONFIG_BLOCK_WORD_0(base)      ((volatile bit32u *)(base+0x83C0))
#define OAM_CONFIG_BLOCK_WORD_0(base)       (*pOAM_CONFIG_BLOCK_WORD_0(base))

/* PDSP OAM Padding Block */
#define pOAM_PADDING_BLOCK_WORD_0(base)     ((volatile bit32u *)(base+0x84C0))
#define OAM_PADDING_BLOCK_WORD_0(base)      (*pOAM_PADDING_BLOCK_WORD_0(base))

#define NUM_OAM_RATES 11

/* PDSP OAM Timer State RAM */
#define pOAM_TIMER_STATE_WORD_0(base)       ((volatile bit32u *)(base+0x85B0))
#define OAM_TIMER_STATE_WORD_0(base)        (*pOAM_TIMER_STATE_WORD_0(base))


/* END OF FILE */

#endif _INC_SAR_REG
