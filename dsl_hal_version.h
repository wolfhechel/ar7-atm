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
*    29Jan04     0.00.36            MCB    Bumped version numbers in support of D3R2 release.
*    04Feb04     0.00.37            JEB    Changed DSLAGENT version numbers for the latest code updates.
*    14Feb04     0.00.38            MCB    Bumped version numbers in support of MFGDIAG 1.0 release.
*    25Feb04     0.00.39            ZT     Bumped version numbers for Linux ATM driver and DHAL
*    08Mar04     0.00.40            JEB    Changed version numbers for DSLHAL and DHALAPP to 03.00.50.01 for DSM
*    30Mar04     0.00.41            JEB    Fixed DSLAGENT version number back to 02.01.01.00.
*    30Mar04     0.00.42            VRB    Bumped the build number for Linux ATM driver
*    30Mar04     0.00.43            MCB    Bumped version numbers in support of LINUXDSLPSP4.2-R2 release.
*    05Apr04     0.00.44            JEB    Bumped build numbers on DSLHAL and DSLAGENT for CHECKPOINT50.
*    26Apr04     0.00.45            JEB    Brought back correct version numbers on CPEdallas_Ax7_ADSL\LATEST
*                                          and fixed the LINUXATM driver version to represent the latest
*    29Apr04     0.00.46            JEB    Bumped build numbers on DSLHAL and DSLAGENT for CHECKPOINT51.
*    14May04     0.00.47            SV     Bumped build numbers on DSLHAL and DSLAGENT for CHECKPOINT52.
*    17May04     0.00.48            ZT     Bumped build number on the Linux ATM driver.
*    18May04     0.00.49            MCB    Bumped version numbers in support of LINUXDSLPSP4.2-R1.2 release.
*    24May04     0.00.50            MCB    Bumped version numbers in support of CPE Agent 2.1 R1 release.
*    01Jun04     0.00.51            MCB    Bumped version numbers in support of SCM_20040601_CPEdallas_Ax7_ADSL2_CHECKPOINT53.
*    02Jun04     0.00.52            JLP    Bumped version numbers in support of CPE Agent 2.1 R2 release.
*    11Jun04     0.00.53            RamP   Bumped version numbers in support of Linux PSP R2-5
*    16Jun04     0.00.54            SV     Changed DSLHAL and DHALAPP version numbers for D3.1R2 release
*    23Jun04     0.00.55            Ram    Bumped build number for Linux ATM driver integration Checkpoint 009
*    24Jun04     0.00.56            Ram    Bumped build number for Linux ATM driver for PSP 4.2L R3
*    06Jul04     0.00.57            JEB    Bumped the bugfix number for DSLAGENT for ADSL2 CHECKPOINT55
*    07Jul04     0.00.58            MCB    Moved ATM Linux driver version info to it's own file.
*    20Jul04     0.00.59            MCB    Bumped version numbers in support of CPE Agent 2.1 CHECKPOINT004.
*    29Jul04     0.00.60            RamP   Bumped version numbers for D3.2R0.
*                                    /SM
*    09Aug04     0.00.61            RamP   Updated build number for DSL HAL in prep of Linux PSP R2-3
*    25Aug04     0.00.62            RamP   Updated build number for DSL HAL in prep of Linux PSP R2-4
*    27Aug04     0.00.63            JEB    Bumped DSLHAL version number for D3.2R1 release.
*    02Sep04     0.00.66            RamP   Bumped DSLHAL and DHALAPP version numbers.
*                                    /SM
*    21Sep04     0.00.67            Ramp/CGH Bumped DSLHAL version number from D3.2.1.1 to D3.2.1.2
*    04Oct04     0.00.68            CGH    Bumped DSLHAL version number from D3.2.1.2 to D3.2.1.3
*    10Oct04     0.00.69            CGH    Bumped DSLHAL version number from D3.2.1.3 to D3.2.1.4
*    10Oct04     0.00.70            CGH    Bumped DHALAP version number from D3.2.0.1 to D3.2.0.2
*    12Oct04     0.00.71            GKD    Bumped DHALAP version number from D3.2.0.1 to D3.2.4.0
*    20Oct04     0.00.72            MCB    Bumped version numbers in support of D4.0 R0
*    27Oct04     0.00.73            JEB    Bumped DSLHAL/DHALAPP version numbers to 04.00.00.02 for ADSL2 CHECKPOINT73
*                                          Fixed DSLAGENT version number to reflect the current DSLAGENT present.
*    16Feb04     0.00.74            CPH    Bumped DHALAP & DHAL version to 04.00.00.03 for Ohio250 support
*******************************************************************************/

/* Dsl Hal API Version Numbers */
#define DSLHAL_VERSION_MAJOR       07
#define DSLHAL_VERSION_MINOR       05
#define DSLHAL_VERSION_BUGFIX      01
#define DSLHAL_VERSION_BUILDNUM    00
#define DSLHAL_VERSION_TIMESTAMP   03

/* dhalapp Adam2 Application Version Numbers */
#define DHALAPP_VERSION_MAJOR      07
#define DHALAPP_VERSION_MINOR      05
#define DHALAPP_VERSION_BUGFIX     01
#define DHALAPP_VERSION_BUILDNUM   00

/* DSL Agent Version Numbers */
#define DSLAGENT_VERSION_MAJOR     02
#define DSLAGENT_VERSION_MINOR     01
#define DSLAGENT_VERSION_BUGFIX    05
#define DSLAGENT_VERSION_BUILDNUM  01

#endif /* pairs with #ifndef __SYSSW_VERSION_H__ */
