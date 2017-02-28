#ifndef __ENV_DEF_TYPEDEFS_H__
#define __ENV_DEF_TYPEDEFS_H__ 1

/*******************************************************************************
*  FILE PURPOSE: Define data types for C and TMS320C6x C compilers
********************************************************************************
*
*  FILE NAME: dpsys_typedefs.h
*
*  DESCRIPTION:
*        This file contains the main typedefs that we need.
*
*  HISTORY:
*
*  03/11/97  Bob Lee      Created
*  03/13/97  Chishtie
*  03/14/97  Bob Lee      Format change to meet "Engineering Model
*                         - System Architucture Specification"
*                         Rev AP3.  Jan. 29, 1997
*  07/21/00  Barnett      Moved many common typedefs from
*                         host i/f header file to here.
*  03/30/01  Barnett      Mod's per driver team feedback.
*                         Some tokens cannot be def'ed
*                         if _WINDEF_ is def'ed.
*  04/05/01  Barnett      Added DSPDP_ prefix to tokens that originate
*                         in the public interface.
*  06/01/01  J. Bergsagel Modified to add standard typedefs
*  07/25/01  J. Bergsagel Changed name from typedefs.h to dpsys_typedefs.h
*  07/30/01  J. Bergsagel Moved typedefs that were conflicting with Windows
*                         driver software to the "#ifndef _WINDEF_" section.
*  08/09/01  S. Yim       Moved FALSE/TRUE definitions from ctl_interface_u.h
*                         (conflict with BIOS/std.h)
*  09/03/01  S. Yim       Do not include typedef char and float if _STD defined
*                         (conflict with BIOS/std.h)
*  01/21/03  MCB          Implemented Ax7 UNIT-MODULE modular software framework.
*  03/20/03  Mallesh      Defined size of basic variables
*  03/27/03  F. Mujica    Added SINT40 and UINT40 definitions.
*
*  (C) Copyright Texas Instruments Inc. 1997-2001.  All rights reserved.
*******************************************************************************/

// Common type definitions

// Basic constants needed everywhere
#ifndef STD_
#define FALSE 0
#define TRUE 1
#endif

// Read-Write Data Types
typedef signed char           SINT8;      // Signed 8-bit integer (7-bit magnitude)
typedef unsigned char         UINT8;      // Unsigned 8-bit integer
typedef signed short          SINT16;     // Signed 16-bit integer (15-bit magnitude)
typedef unsigned short        UINT16;     // Unsigned 16-bit integer
typedef signed int            SINT32;     // Signed 32-bit integer (31-bit magnitude)
typedef unsigned int          UINT32;     // Unsigned 32-bit integer
typedef long signed int       SINT40;     // Long signed 40-bit integer
typedef long unsigned int     UINT40;     // Long unsigned 40-bit integer

// All pointers are 32 bits long
typedef SINT8                 *PSINT8;    // Pointer to SINT8
typedef UINT8                 *PUINT8;    // Pointer to UINT8
typedef SINT16                *PSINT16;   // Pointer to SINT16
typedef UINT16                *PUINT16;   // Pointer to UINT16
typedef SINT32                *PSINT32;   // Pointer to SINT32
typedef UINT32                *PUINT32;   // Pointer to UINT32

#define SIZEOF_SINT8          1
#define SIZEOF_UINT8          1
#define SIZEOF_SINT16         2
#define SIZEOF_UINT16         2
#define SIZEOF_SINT32         4
#define SIZEOF_UINT32         4
#define SIZEOF_SINT40         8
#define SIZEOF_UINT40         8

// Size of Read-Write Data Types - in bytes
#define SIZEOF_char   1
#define SIZEOF_Int8   1
#define SIZEOF_UChar  1
#define SIZEOF_UInt8  1
#define SIZEOF_Float  4
#define SIZEOF_Double 8
#define SIZEOF_byte   1

// Read-Only Data Types - should be only used for ROM code
typedef const char            CharRom;    // 8 bit signed character
typedef const signed char     Int8Rom;    // 8 bit signed integer
typedef const unsigned char   UCharRom;    // 8 bit unsigned character
typedef const unsigned char   UInt8Rom;   // 8 bit unsigned integer
typedef const float           FloatRom;   // IEEE 32-bit
typedef const double          DoubleRom;  // IEEE 64-bit

#ifndef _WINDEF_ 

// Read-Write Data Types
typedef signed char           Int8;       // 8 bit signed integer
typedef unsigned char         UChar;      // 8 bit unsigned character
typedef unsigned char         UInt8;      // 8 bit unsigned integer
#ifndef STD_
typedef char                  Char;       // 8 bit signed character
typedef float                 Float;      // IEEE 32-bit
#endif
typedef double                Double;     // IEEE 64-bit
typedef signed char           byte;       // 8 bit signed integer


// These typedefs collide with those in Win2k DDK inc\WINDEF.H

// common type definition
typedef unsigned char   BYTE;    // 8-bit
typedef signed short    SHORT;   // 16-bit signed
typedef unsigned short  WORD;    // 16-bit
typedef unsigned int    DWORD;   // 32-bit, TI DSP has 40 bit longs

// All pointers are 32 bits long
typedef BYTE           *PBYTE;  // pointer to 8 bit data
typedef unsigned char  *PCHAR;  // pointer to 8 bit data
typedef SHORT          *PSHORT; // pointer to 16 bit data
typedef WORD           *PWORD;  // pointer to 16 bit data
typedef DWORD          *PDWORD; // pointer to 32 bit data

#endif // #ifndef _WINDEF_


#define SIZEOF_BYTE   1
#define SIZEOF_SHORT  2
#define SIZEOF_WORD   2
#define SIZEOF_DWORD  4
#define SIZEOF_PCHAR  4
#define SIZEOF_PWORD  4
#define SIZEOF_PDWORD 4

// Size of Read-Only Data Types - in bytes
#define SIZEOF_CharRom   1
#define SIZEOF_Int8Rom   1
#define SIZEOF_UCharRom  1
#define SIZEOF_UInt8Rom  1
#define SIZEOF_FloatRom  4
#define SIZEOF_DoubleRom 8

#define SIZEOF_complex_byte     (2*SIZEOF_byte)
#define SIZEOF_PTR_complex_byte 4
typedef struct {
  byte    re;
  byte    im;
} complex_byte, *PTR_complex_byte;

#define SIZEOF_complex_short   4
#define SIZEOF_PTR_complex_short 4
typedef struct {
  short   re;
  short   im;
} complex_short, *PTR_complex_short;

#define SIZEOF_complex_int     8
#define SIZEOF_PTR_complex_int 4
typedef struct {
  int     re;
  int     im;
} complex_int, *PTR_complex_int;

typedef struct {
  int high;
  unsigned int low;
} int64;

typedef struct {
  int64 real;
  int64 imag;
} complex_int64;

#define SIZEOF_PVOID 4
typedef void *PVOID;            // pointer to void

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#if defined(_TMS320C6X)           // TMS320C6xx type definitions 

// Read-Write Data Types 
typedef short                   Int16;      // 16 bit signed integer 
typedef unsigned short          UInt16;     // 16 bit unsigned integer 
typedef int                     Int32;      // 32 bit signed integer 
typedef unsigned int            UInt32;     // 32 bit unsigned signed integer 
typedef long                    Long40;     // 40 bit signed integer 
typedef unsigned long           ULong40;    // 40 bit unsigned signed integer 

// Size of Read-Write Data Types - in bytes
#define SIZEOF_Int16   2
#define SIZEOF_UInt16  2
#define SIZEOF_Int32   4
#define SIZEOF_UInt32  4
#define SIZEOF_Long40  5
#define SIZEOF_ULong40 5

// Read-Only Data Types - should be only used for ROM code
typedef const short             Int16Rom;   // 16 bit signed integer 
typedef const unsigned short    UInt16Rom;  // 16 bit unsigned integer 
typedef const int               Int32Rom;   // 32 bit signed integer 
typedef const unsigned int      UInt32Rom;  // 32 bit unsigned signed integer 
typedef const long              Long40Rom;  // 40 bit signed integer 
typedef const unsigned long     ULong40Rom; // 40 bit unsigned signed integer 

// Size of Read-Only Data Types - in bytes
#define SIZEOF_Int16Rom   2
#define SIZEOF_UInt16Rom  2
#define SIZEOF_Int32Rom   4
#define SIZEOF_UInt32Rom  4
#define SIZEOF_Long40Rom  5
#define SIZEOF_ULong40Rom 5

#else                             // 32 bits PC Host type definitions 

// Read-Write Data Types 
typedef short                   Int16;      // 16 bit signed integer 
typedef unsigned short          UInt16;     // 16 bit unsigned integer 
typedef int                     Int32;      // 32 bit signed integer 
typedef unsigned int            UInt32;     // 32 bit unsigned integer 

// Read-Only Data Types - should be only used for ROM code 
typedef const short             Int16Rom;   // 16 bit signed integer 
typedef const unsigned short    UInt16Rom;  // 16 bit unsigned integer 
typedef const int               Int32Rom;   // 32 bit signed integer 
typedef const unsigned int      UInt32Rom;  // 32 bit unsigned integer 

#endif

#endif
