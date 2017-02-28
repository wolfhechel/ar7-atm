#ifndef __DSL_INTERNALFUNCTION_DEFINES_H__
#define __DSL_INTERNALFUNCTION_DEFINES_H__ 1

/*******************************************************************************
* FILE PURPOSE:     DSL HAL Internal Only API functions for Sangam
*
********************************************************************************
* FILE NAME:        dsl_hal_internal_api.h
*
* DESCRIPTION:  
*       Contains API function definitions for Internal use only.
*  by 
*  Ramakrishnan Parasuraman
*                    
* (C) Copyright 2003, Texas Instruments, Inc.
*    History
*    Date        Version            Notes
*    06Feb03     0.00               Intial Revision Written  
*    12Dec03     0.01               Added internal interface location seeker
*******************************************************************************/

/******************************************************************************************
* FUNCTION NAME:    dslhal_internalDiags_anlg_allDiagnosticTests(tidsl_t *ptidsl)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the PGA and AEQ parameters
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int flag; //if flag = TRUE set rateadapt flag else reset it
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
void dslhal_internalDiags_anlg_allDiagnosticTests
(int diagMode,
 int agcFlag,
 short pga1,
 short pga2,
 short pga3,
 short aeq
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_internalDiags_anlg_setDiagMode(tidsl_t *ptidsl)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the PGA and AEQ parameters
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int flag; //if flag = TRUE set rateadapt flag else reset it
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_internalDiags_anlg_setDiagMode
(tidsl_t *ptidsl,
int diagMode
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_internalDiags_anlg_getDiagnosticTestResults(tidsl_t *ptidsl)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the PGA and AEQ parameters
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int flag; //if flag = TRUE set rateadapt flag else reset it
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_internalDiags_anlg_getDiagnosticTestResults
(tidsl_t *ptidsl
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_diags_setPgaParams(tidsl_t *ptidsl,int agcFlag,short pga1, short pga2, short pga3, short aeq)
*
*******************************************************************************************
* DESCRIPTION: This fuction Sets the PGA and AEQ parameters
*
* INPUT:  PITIDSLHW_T *ptidsl
*         unsigned int flag; //if flag = TRUE set rateadapt flag else reset it
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_internalcfg_setPgaParams
(tidsl_t *ptidsl,
 short pga1, 
 short pga2, 
 short pga3, 
 short aeq
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_internalcfg_getCrossTalkParams(tidsl_t *ptidsl)
*
*******************************************************************************************
* DESCRIPTION: This fuction gets the Crosstalk parameters into app interface
*
* INPUT:  PITIDSLHW_T *ptidsl
*        
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_internalcfg_getCrossTalkParams
(
 tidsl_t *ptidsl
);

/******************************************************************************************
* FUNCTION NAME:    dslhal_internalcfg_getPhyParams(tidsl_t *ptidsl)
*
*******************************************************************************************
* DESCRIPTION: This fuction gets the Advanced Phy  params into app interface
*
* INPUT:  PITIDSLHW_T *ptidsl
*        
* RETURN: 0 SUCCESS
*         1 FAILED
*
*****************************************************************************************/
unsigned int dslhal_internalcfg_getPhyParams
(
 tidsl_t *ptidsl
);

unsigned int* dslhal_internalcfg_getInternalInterfaceLocation(void);


#endif /* Pairs #ifndef __DSL_INTERNALFUNCTION_DEFINES_H__ */
