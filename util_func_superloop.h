#ifndef __UTIL_FUNC_SUPERLOOP_H__
#define __UTIL_FUNC_SUPERLOOP_H__ 1

// Copyright Amati Communications Inc. 1997.  All rights reserved.
// ******************************************************************
// superloop_U.h
// This file contains the interface constant definitions and
// function prototypes needed to use the functions
// in superloop_u.asm
// History
// 10/13/97  Mike Locke   Written
// 10/21/97  Mike Locke   Add MATH_FUNC_scaleIntData3() and MATH_FUNC_int2Short
// 11/11/97  Sujai Chari  Modified MATH_FUNC_int2Short to include offset parameter
// 01/21/00 Fred Wagner      derfmake changes
// 07/12/00 Derf         Added typedefs.h
// 07/25/01 J. Bergsagel Changed typedefs.h to dpsys_typedefs.h
// 04/23/02 D Mannering  Added short shift function prototypes
// 01/21/03 MCB          Implemented Ax7 UNIT-MODULE modular software framework.
// 01/30/03 Mannering    Added  _MATH_FUNC_scaleIntToShortData3
// 02/21/03 D. Cox       Added MATH_FUNC_scaleComplexIntToComplexConjShort and MATH_FUNC_combineArrays.
// 03/24/03 Mannering    Changed data types to coding standards
// 05/05/03 Mannering    Review Comments - Changed pointer types to comply with
//                       coding standard
// 09/24/03 J. Bergsagel Added check for define of _TMS320C6X in case of Host code builds
// 04/01/04 Sameer V     Updated function prototypes.
// ******************************************************************

#include "env_def_typedefs.h"

// Copies w_init to *w and zero fills unused parts
void PHY_TEQ_initWTime(SINT32 *w, unsigned short downstream_samps, unsigned short teq_length);

// Fills num entries of array with realp, imagp
void MATH_FUNC_intArraysToComplex(complex_int *array, SINT32 num, SINT32 realp, SINT32 imagp);

// Compute sum of squares of shifted 32 bit data
//  sum += sqr(*data++ >> shift);
SINT32  MATH_FUNC_sumIntSquares(const PSINT32 data,SINT32  shift, SINT32  count);

// General purpose int shift function
// if (direction) *array++ >>= shift; else *array++ <<= shift;
void MATH_FUNC_scaleIntData(SINT32  *array,   SINT32 shift, SINT32 direction, SINT32 num);
void MATH_FUNC_scaleIntData2(SINT32 *array,   SINT32 shift, SINT32 direction, SINT32 num);
void MATH_FUNC_scaleIntData3(SINT32 *inarray, SINT32 shift, SINT32 direction, SINT32 num, PSINT32 outarray);

// General purpose short shift function
// if (direction) *array++ >>= shift; else *array++ <<= shift;
void MATH_FUNC_scaleShortData(PSINT16 array,SINT32  shift, SINT32  direction);
void MATH_FUNC_scaleShortData2(PSINT16 array,SINT32  shift, SINT32  direction, SINT32  num);
void MATH_FUNC_scaleShortData3(PSINT16 inarray,SINT32  shift, SINT32  direction, SINT32  num, PSINT16 outarray);

// Finds index of largest abs int
SINT32  PHY_TEQ_findMaxAbsInt(SINT32  num, const PSINT32 array);

#if defined(_TMS320C6X) // Necessary for Host code builds that need this file
// Special function used in teq training
// for (num) sum += sqr(*array[start++ & mask] >> 16)*2;
Long40 MATH_FUNC_circSumIntSquare(const PSINT32 array, SINT32  mask, SINT32  start, SINT32  num);

// General purpose sum of squares
// for (num) sum += sqr(*array++);
Long40 MATH_FUNC_sumIntSquares16(SINT32  num, PSINT16 array);
#endif

// Compute sum of squares of shifted 16 bit data
//  sum += sqr(*data++ >> shift);
SINT32  MATH_FUNC_sumIntSquares16WShift(const PSINT16 data,SINT32  shift, SINT32  count);

// General purpose 16 bit data scaling
// for (num) *array++ = (*array * mpy) >> shift;
void MATH_FUNC_scaleIntData16(PSINT16 array, SINT32  shift, SINT32  mpy, SINT32  num);

// General purpose 16 bit accurate scaling of 32 bit data
// for (num) *array++ = smpyh(*array,mpy);
void MATH_FUNC_satMpyData(const PSINT32 array, SINT32  mpy, SINT32  num);

// Special function used in teq training
// for (num) *array[start++ & mask] = 0;
void PHY_TEQ_circClearWall(PSINT32 array, SINT32  start, SINT32  mask, SINT32  num);

// Zeroes num entries in array.  num must be even and greater than 0
void UTIL_MEM_fillBufferZeros(PSINT32 array, SINT32  num);

void MATH_FUNC_int2Short(SINT32 *inarray, SINT32 num, SINT32 shift, SINT16 *outarray, SINT32 offset, unsigned short count);

void MATH_FUNC_scaleIntToShortData3(PSINT32 inarray, SINT32  shift, SINT32  direction, SINT32  number, PSINT16 outarray);

void MATH_FUNC_scaleComplexIntToComplexConjShort(complex_int   *inArray,
                                                 SINT32         inputScale,
                                                 SINT32         subchannels,
                                                 complex_short *outArray);

void MATH_FUNC_combineArrays(SINT8         *arraySelect,
                             complex_short *array0,
                             complex_short *array1,
                             SINT32        length);

#endif

