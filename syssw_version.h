#ifndef __SYSSW_VERSION_H__
#define __SYSSW_VERSION_H__ 1

/*******************************************************************************
* FILE PURPOSE:     DSL Driver API functions for Sangam
*
********************************************************************************
* FILE NAME:        dsl_hal_basicapi.c
*
* DESCRIPTION:  
*       Contains basic DSL HAL APIs for Sangam
*  
* (C) Copyright 2003-04, Texas Instruments, Inc.
*    History
*    Date        Version            Notes
*    14May03     0.00.00            RamP   Original Version Created
*    14May03     0.00.01            RamP   Initial Rev numbers inserted 
*    14May03     0.00.02            RamP   Bumped version numbers for Dsl Hal
*                                          & dhalapp for alpha plus
*    19May03     0.00.03            MCB    Bumped dslhal version number
*                                          because of dependant changes
*                                          wrt. linux-nsp atm drivers.
*    22May03     0.00.04            RamP   Bumped dslhal & dhalapp buildnum
*                                          for inner/outer pair & DGASP code
*    06Jun03     0.00.05            RamP   Bumped up buildnum for LED, STM, 
*                                          interrupt processing, statistics
*                                          and other pre-beta features
*    09Jun03     0.00.06            JEB    Fixed error in DHALAPP bugfix/buildnum
*    09Jun03     0.00.07            RamP   Bumped up buildnum for incremental
*                                          changes to apis, statistics, memory
*                                          fixes, parameter configurations
*    11Jun03     0.00.08            RamP   Bumped up buildnum for Co profile
*                                          free memory fix
*    12Jun03     0.00.09            JEB    Bumped version numbers for AR7 1.00 Beta
*    02Jul03     0.00.10            ZT     Bumped HAL version for overlay page
*    18Jul03     0.00.11            RamP   Bumped HAL version for analog diags
*    22Jul03     0.00.12            JEB    Bumped DHALAPP buildnum for analog diags
*    31Jul03     0.00.13            RamP   Bumped HAL version for engr. drop
*    04Aug03     0.00.14            JEB    Bumped HAL version buildnum for CHECKPOINT65 changes
*                                          Bumped LINUX version buildnum for CHECKPOINT65 changes
*    06Aug03     0.00.15            MCB    Bumped all version numbers in prep for AR7 1.0 R2 release for POTS.
*    13Aug03     0.00.16            MCB    Set rev id's for D3/R1.1 (ADSL2).
*    21Aug03     0.00.17            JEB    Bumped up build numbers for merge of code additions from D1
*    26Sep03     0.00.18            JEB    Set rev id's for another D3/R1 (ADSL2).
*    14Oct03     0.00.19            JEB    Bumped Linux minor number and reset bugfix number for release.
*                                          Bumped build numbers on DSLHAL and DHALAPP for this checkpoint.
*    14Oct03     0.00.20            JEB    Bumped build numbers on DSLHAL and DHALAPP for CHECKPOINT15.
*    21Oct03     0.00.21            JEB    Bumped build number on DSLHAL for CHECKPOINT16.
*    22Oct03     0.00.22            MCB    Bumped all version numbers in support of D3R1 release.
*    27Oct03     0.00.23            JEB    Bumped build numbers on DSLHAL and DHALAPP for CHECKPOINT19.
*                                          Updated version for DSLAGENT to be 02.01.00.01 for ACT 2.1 R0.
*    30Oct03     0.00.24            JEB    Bumped bugfix number on LINUXATM Version for next release.
*                                          Bumped build numbers on DSLHAL and DHALAPP
*    31Oct03     0.00.25            MCB    Bumped all version numbers in support of D3R2 release.
*    14Nov03     0.00.26            JEB    Bumped build numbers on DSLHAL and DHALAPP
*                                          Changed version for DSLAGENT to be 02.00.01.01 for an ACT 2.0 R0
*    20Nov03     0.00.27            JEB    Bumped build number on DSLHAL.
*                                          Changed version for DSLAGENT to be 02.00.02.00 for the next ACT 2.0 R2
*    21Nov03     0.00.28            MCB    Bumped all version numbers in support of D3R2 release.
*    21Nov03     0.00.29            JEB    Bumped build numbers on DSLHAL and DHALAPP for D3-R0 drop on 11/21.
*    16Dec03     0.00.30            JEB    Bumped build numbers on DSLHAL and DHALAPP for CHECKPOINT31.
*    21Dec03     0.00.31            MCB    Bumped all version numbers in support of D3R2 release.
*    05Jan04     0.00.32            JEB    Bumped build numbers on DSLHAL and Linux ATM for CHECKPOINT 34.
*    15Jan04     0.00.33            JEB    Bumped build numbers on DSLHAL and Linux ATM for CHECKPOINT 36.
*    26Jan04     0.00.34            JEB    Changed Linux ATM version number to be 04.02.03.00.
*    27Jan04     0.00.35            MCB    Bumped all version numbers in support of D3R2 release.
*******************************************************************************/

/* Dsl Hal API Version Numbers */
#define DSLHAL_VERSION_MAJOR       03
#define DSLHAL_VERSION_MINOR       00
#define DSLHAL_VERSION_BUGFIX      06
#define DSLHAL_VERSION_BUILDNUM    00
#define DSLHAL_VERSION_TIMESTAMP   00

/* dhalapp Adam2 Application Version Numbers */
#define DHALAPP_VERSION_MAJOR      03
#define DHALAPP_VERSION_MINOR      00
#define DHALAPP_VERSION_BUGFIX     05
#define DHALAPP_VERSION_BUILDNUM   00

/* Linux ATM Driver Version Numbers */
#define LINUXATM_VERSION_MAJOR     04
#define LINUXATM_VERSION_MINOR     02
#define LINUXATM_VERSION_BUGFIX    04
#define LINUXATM_VERSION_BUILDNUM  00

/* DSL Agent Version Numbers */
#define DSLAGENT_VERSION_MAJOR     02
#define DSLAGENT_VERSION_MINOR     00
#define DSLAGENT_VERSION_BUGFIX    02
#define DSLAGENT_VERSION_BUILDNUM  00

#endif /* pairs with #ifndef __SYSSW_VERSION_H__ */
