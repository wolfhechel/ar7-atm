#ifndef __DEV_HOST_VERDEF_H__
#define __DEV_HOST_VERDEF_H__ 1

//********************************************************************
//*
//*    DMT-BASE ADSL MODEM PROGRAM
//*    TEXAS INSTRUMENTS PROPRIETARTY INFORMATION
//*    AMATI CONFIDENTIAL PROPRIETARY
//*
//* (c) Copyright April 1999, Texas Instruments Incorporated.
//*     All Rights Reserved.
//*
//*   Property of Texas Instruments Incorporated and Amati Communications Corp.
//*
//* Restricted Rights - Use, duplication, or disclosure is subject to
//* restrictions set forth in TI's and Amati program license agreement and
//* associated documentation
//*
//*********************************************************************
//*
//* FILENAME: dev_host_verdef.h
//*
//* ABSTRACT: This file defines the version structure
//*
//* TARGET:  Non specific.
//*
//* TOOLSET:  Non specific.
//*
//* ACTIVATION:
//*
//* HISTORY:  DATE        AUTHOR   DESCRIPTION
//*           04/29/99    FLW      Created
//*           01/17/00    Barnett  Mod's in support of merging NIC
//*                                hardware rev 6/7 T1.413 codebases.
//*           01/21/00    Wagner   derfmake mods
//*           05/11/00    Barnett  hardware_rev is a 2 char string.
//*           07/24/00    Barnett  Rework as part of host interface redesign.
//*           11/29/00    Hunt     added chipset_id2
//*           03/30/01    Barnett  Prefixed all public elements with DSPDP_.
//*                                This insures uniqueness of names that might
//*                                match host names by coincidence.
//*           03/30/01    Barnett  Added DSPDP_Host_VersionDef to facilitate
//*                                representing a version id for the host i/f
//*                                separate from the firmware version id as
//*                                a courtesy to the host.
//*           07/23/01    JEB      Changed name from verdef_u.h to dpsys_verdef.h
//*           04/12/02    Barnett  Make timestamp unsigned 32-bit field.
//*                                Generalizes for all kinds of hosts.
//*           11/15/02    JEB      Changed name from dpsys_verdef.h to dev_host_verdef.h
//*                                Updated structs according to coding guidelines
//*           12/16/02    JEB      Renamed some struct elements for new usage in Ax7
//*           01/21/03    MCB      Implemented Ax7 UNIT-MODULE modular software framework.
//*           03/19/03    JEB      Added back in "bugFix" elements into each struct type.
//*                                Rearranged elements.
//*
//********************************************************************

#include "env_def_typedefs.h"

#define DSPDP_FLAVOR_NEWCODES 0xFF // Other values are valid old-style flavors

// ------------------------------
// ------------------------------
// Begin DSP version definitions.
// ------------------------------
// ------------------------------

typedef struct
{
  UINT32 timestamp;    // Number of seconds since 01/01/1970
  UINT8  major;        // Major "00".00.00.00 revision nomenclature
  UINT8  minor;        // Minor 00."00".00.00 revision nomenclature
  UINT8  bugFix;       // Bug Fix 00.00."00".00 revision nomenclature
  UINT8  buildNum;     // Build Number 00.00.00."00" revision nomenclature
  UINT8  netService;   // Network service identifier
  UINT8  chipsetGen;   // chipset generation
  UINT8  chipsetId;    // chipset identifier
  UINT8  chipsetId2;   // second byte for "RV" chipset et al.
  UINT8  hardwareRev1; // hardware revision, 1st char
  UINT8  hardwareRev2; // hardware revision, 2nd char
  UINT8  featureCode;  // feature code
  UINT8  dummy1;       // dummy byte for explicit 32-bit alignment
} DEV_HOST_dspVersionDef_t;

// -------------------------------
// -------------------------------
// Begin host version definitions.
// -------------------------------
// -------------------------------

typedef struct
{
  UINT8 major;        // Major "00".00.00.00 revision nomenclature
  UINT8 minor;        // Minor 00."00".00.00 revision nomenclature
  UINT8 bugFix;       // Bug Fix 00.00."00".00 revision nomenclature
  UINT8 buildNum;     // Build Number 00.00.00."00" revision nomenclature
  UINT8 netService;   // Network service identifier
  UINT8 dummy[3];     // dummy bytes for explicit 32-bit alignment
} DEV_HOST_hostIntfcVersionDef_t;


#endif // __DEV_HOST_VERDEF_H__
