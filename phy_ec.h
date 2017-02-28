#ifndef __PHY_EC_H__
#define __PHY_EC_H__ 1

/*******************************************************************************
* FILE PURPOSE: EC physical layer processing.
********************************************************************************
*
* FILE NAME: phy_ec.h
*
* DESCRIPTION:
*
*    Constants, structures and function prototypes for the physical layer
*    training and updating of the echo canceller.
*
* HISTORY:
*
*    08/05/2002 A. Redfern    Created.
*    10/21/2002 A. Redfern    Revision 20 completed, code review formatting.
*    10/31/2002 A. Redfern    Minor cleanup.
*    11/11/2002 D. Mannering  Added FPGA token.
*    01/21/2003 M. Barnett    Implemented Ax7 UNIT-MODULE modular software framework.
*    02/21/2003 A. Redfern    Revised the debug levels, tokenized the showtime
*                             update.
*    03/27/2003 A. Redfern    Code review moving of #defines and includes.
*    06/05/2003 Mallesh       Added varaibles ecImpulseValue and ecImpulseScale
*                             to phy_ec structure.
*    09/24/2003 J. Bergsagel  Moved def for arrayUnion5_t to this file.
*    03/04/2004 D. Mannering  Removed FPGA token added OHIO_SUPPORT token
*    04/01/2004 Sameer V      Seperated parameters that can be dynamically allocated
*                             from the ones that are needed all the time in training.
*                             Seperated EC_reset from EC_reset_HW().
*    07/07/2004 D. Mannering  Add OHIO receive rate and subtract location fields
*                             in PHY_EC_ecSetting_t
*    10/13/2004 D. Mannering  Removed OHIO_SUPPORT Token from around max filter length defs
*    10/09/2004 U.Dasgupta     Cleaned up function interface for PHY_EC_delayAndScale.
*    03/29/2005 Venkat R       Removed PHY_EC_ENABLE (Set to 1) Token
*    04/07/2005 Venkat R       Removed DUAL_TEQ (Set to 1) Token
*    05/06/2005 Mike Locke     Add comments for token usage CQ9583
*
* LIST OF FUNCTIONS:
*
*    PHY_EC_reset
*    PHY_EC_train
*    PHY_EC_hwConfigure
*    PHY_EC_txProcessing
*    PHY_EC_rxProcessing
*    PHY_EC_fdSignalGenerator
*    PHY_EC_tdSignalGenerator
*    PHY_EC_fdEchoEstimate
*    PHY_EC_tdEchoEstimate
*    PHY_EC_fdCoefficients
*    PHY_EC_tdCoefficients
*    PHY_EC_fdWindow
*    PHY_EC_tdWindow
*    PHY_EC_initialEchoEstimate
*    PHY_EC_adaptationParameter
*    PHY_EC_delayEstimate
*    PHY_EC_delayAndScale
*    PHY_EC_delayChange
*    PHY_EC_teqUpdate
*    PHY_EC_trackAndCancelEcho
*    PHY_EC_filterUpdate
*
* (c) Copyright Texas Instruments, Inc. 2002. All rights reserved.
*
*******************************************************************************/

#include "env_def_defines.h"
#include "env_def_typedefs.h"
#include "util_func_superloop.h"
#include "ctrl_msm_swif.h"


/*******************************************************************************
*
* DEFINES
*
*******************************************************************************/

// PHY_EC - real time board flag
//#if FPGA
//#define PHY_EC_REAL_TIME_BOARD              1   // 0 = false (Ax7), 1 = true (real time board)
//#else
//#define PHY_EC_REAL_TIME_BOARD              0   // 0 = false (Ax7), 1 = true (real time board)
//#endif

// debug code flag
#define PHY_EC_DEBUG_LEVEL_0                0                    // Minimize debug code and storage
#define PHY_EC_DEBUG_LEVEL_1                1                    // variable and functionality
#define PHY_EC_DEBUG_LEVEL_2                2                    // adds array saving
#define PHY_EC_DEBUG                        PHY_EC_DEBUG_LEVEL_0 // select a debug level

// enable/disable Showtime updates
#define PHY_EC_ENABLE_SHOWTIME_UPDATES      0   // allows some program memory savings

// PHY_EC - filter lengths
#define PHY_EC_OHIO_MAX_FILTER_LENGTH       384 // maximum EC filter length
#define PHY_EC_SANGAM_MAX_FILTER_LENGTH     360 // maximum EC filter length
#define PHY_EC_MAX_FILTER_LENGTH            360 // maximum EC filter length

// PHY_EC - downstream lengths
#define PHY_EC_SUBCHANNELS_DN               256                       // unique downstream subchannels
#define PHY_EC_FFT_SIZE_DN                  (2*PHY_EC_SUBCHANNELS_DN) // downstream FFT size

// PHY_EC - upstream lengths
#define PHY_EC_MAX_SUBCHANNELS_UP           64                                                 // maximum number of transmit subchannels for a valid EC mode
#define PHY_EC_MAX_IFFT_SIZE_UP             (2*PHY_EC_MAX_SUBCHANNELS_UP)                      // maximum transmit IFFT size for a valid EC mode
#define PHY_EC_MAX_INPUT_DELAY              4                                                  // maximum delay of input symbols to the echo canceller
#define PHY_EC_MAX_INPUT                    (PHY_EC_MAX_IFFT_SIZE_UP*PHY_EC_MAX_INPUT_DELAY)   // required storage for the EC input
#define PHY_EC_INPUT_FRAME_MARKER           1                                                  // marker used to search for the input frame in delay estimation (must be nonzero)

// PHY_EC - configuration
#define PHY_EC_PATH_1_TRAINING              1   // receive path 1 is used for training
#define PHY_EC_PATH_2_TRAINING              2   // receive path 2 is used for training

// PHY_EC - states
#define PHY_EC_RESET                        0   // reset
#define PHY_EC_DELAY_ESTIMATE_SETUP         1   // setup the frame delay estimate
#define PHY_EC_DELAY_ESTIMATE               2   // estimate the frame delay
#define PHY_EC_INITIAL_ECHO_ESTIMATE        3   // estimate the initial echo channel
#define PHY_EC_CANCEL_ECHO                  4   // cancel the echo without updating the echo canceller coefficients
#define PHY_EC_CP_ENABLE                    5   // compensate for enabling the cyclic prefix (it changes the delay)
#define PHY_EC_TEQ_UPDATE                   6   // compensate for updateing the TEQ coefficients
#define PHY_EC_TRACK_AND_CANCEL_ECHO        7   // cancel the echo and update the echo canceller coefficients (frequency domain LMS)

// PHY_EC - parameters for the initial echo channel estimate state
#define PHY_EC_FRAMES_TO_AVERAGE            256 // number of frames used for averaging in the initial estimate, must be a power of 2
#define PHY_EC_EXCESS_DELAY                 8   // delay used to transition between various operations
#define PHY_EC_ADD_MODE                     0   // indicates add mode to collect the time domain estimate
#define PHY_EC_SCALE_MODE                   1   // indicates scale mode to scale the time domain estimate

// PHY_EC - initial echo channel estimate state lengths
#define PHY_EC_INITIAL_ESTIMATE_CONFIG_DONE_FRAME (PHY_EC_MAX_INPUT_DELAY + PHY_EC_EXCESS_DELAY)
#define PHY_EC_INITIAL_ESTIMATE_AVERAGE_FRAME     (PHY_EC_INITIAL_ESTIMATE_CONFIG_DONE_FRAME + PHY_EC_FRAMES_TO_AVERAGE)
#define PHY_EC_INITIAL_ESTIMATE_FD_ESTIMATE_FRAME (PHY_EC_INITIAL_ESTIMATE_AVERAGE_FRAME + 1)
#define PHY_EC_INITIAL_ESTIMATE_FD_WINDOW_FRAME   (PHY_EC_INITIAL_ESTIMATE_FD_ESTIMATE_FRAME + 1)
#define PHY_EC_INITIAL_ESTIMATE_ADAPTATION_FRAME  (PHY_EC_INITIAL_ESTIMATE_FD_WINDOW_FRAME + 1)
#define PHY_EC_INITIAL_ESTIMATE_TD_WINDOW_FRAME   (PHY_EC_INITIAL_ESTIMATE_ADAPTATION_FRAME + 1)
#define PHY_EC_INITIAL_ESTIMATE_HW_SETUP_FRAME    (PHY_EC_INITIAL_ESTIMATE_TD_WINDOW_FRAME + 1)
#define PHY_EC_INITIAL_ESTIMATE_FRAMES            (PHY_EC_INITIAL_ESTIMATE_HW_SETUP_FRAME + PHY_EC_EXCESS_DELAY)


/*******************************************************************************
*
* MACROS
*
*******************************************************************************/

// integer move
 #define UTIL_MEM_moveInts(a, b, c)          MATH_FUNC_scaleIntData3((int *) a, 0, 0, (int) c, (int *) b)


/*******************************************************************************
*
* STRUCTURES
*
*******************************************************************************/


// EC data structure
typedef struct {

   // input and echo estimate
   complex_int   fdEcho[PHY_EC_SUBCHANNELS_DN];    // frequency domain echo channel estimate
   complex_int   fftBuffer[PHY_EC_SUBCHANNELS_DN]; // FFT/IFFT buffer
   complex_short input[PHY_EC_MAX_INPUT];          // echo canceller input
   SINT32        tdEcho[PHY_EC_FFT_SIZE_DN];       // time domain echo channel estimate
 #if OHIO_SUPPORT
 SINT16        filter[PHY_EC_OHIO_MAX_FILTER_LENGTH]; // time domain echo canceller coefficients
 #else
 SINT16        filter[PHY_EC_MAX_FILTER_LENGTH]; // time domain echo canceller coefficients
  #endif
 SINT16        fdScale[PHY_EC_SUBCHANNELS_DN];   // update scale factor (also used a temporary storage for the TEQ coefficients)
} PHY_EC_ecData_t;


typedef struct {
  UINT32        ecPath1Setting;                   // enabled or disabled
  UINT32        ecPath2Setting;                   // enabled or disabled
#if OHIO_SUPPORT
  UINT32        ecReceiveRate;                    // 2.2Mhz or 4.4Mhz
  UINT32        ecSubtractLocation;               // PRE_TEQ or POST_TEQ
#endif

  SINT16        delay;                            // echo canceller delay
  SINT16        scale;                            // echo canceller shift
  SINT16        mode;                             // echo canceller mode
  // transmit and receive processing
  SINT32        state;                            // echo canceller state
  SINT32        counter;                          // internal frame counter
  SINT16        frameDelay;                       // echo canceller path frame delay
  SINT16        sampleDelay;                      // echo canceller path sample delay
  UINT16        write;                            // index used to write echo canceller input symbols into the input buffer
  UINT16        inputScale;                       // scale in converting from 32 bit to 16 bit input values
  SINT32        teqMode;                          // TEQ update substate
  SINT32        trackMode;                        // EC tracking substate
  SINT32        trackCounter;                     // EC tracking substate counter
  // UINT16        prbsIndex;                        // start index for the PRBS used in the frequency domain training

  // configuration
  SINT16        ecRxSignalPath;                   // received signal path used for TEQ training
  UINT16        txSubchannels;                    // number of subchannels
  SINT16        upsample;                         // upsampling factor between echo canceller input and output

  // hardware settings
  SINT16        filterLength;                     // echo canceller filter length
  SINT16        ecImpulseValue;                   // echo canceller tx impulse value
  SINT16        ecImpulseScale;                   // corresponding shift for the impulse (round(log2(PHY_EC_IMPULSE_VALUE)))
  // number of coefficients
#if OHIO_SUPPORT
  SINT16        ecMaxFilterLength;                // max filter for Sangam/Ohio
#endif
} PHY_EC_ecSetting_t;

// Unionize EC data with other stuff
typedef union {
  PHY_EC_ecData_t PHY_EC_ecData_s;
} arrayUnion5_t;


/*******************************************************************************
*
* FUNCTION PROTOTYPES
*
*******************************************************************************/


// PHY_EC - initialization
void PHY_EC_reset();
void PHY_EC_resetEcHw();

int  PHY_EC_train();

// PHY_EC - configuration
void PHY_EC_hwConfigure(SINT32 ecConfiguration, PHY_EC_ecData_t *ec);

// PHY_EC - transmit and receive processing
void PHY_EC_txProcessing(complex_int *txSignal, PHY_EC_ecData_t *ec);
void PHY_EC_rxProcessing(complex_short *rxSignal, PHY_EC_ecData_t *ec);

// PHY_EC - signal generation
void PHY_EC_fdSignalGenerator();
void PHY_EC_tdSignalGenerator(SINT16 *txSignal, SINT32 length);

// PHY_EC - echo estimation
#if PHY_EC_ENABLE_SHOWTIME_UPDATES
void PHY_EC_fdEchoEstimate(complex_short *rxSignal, PHY_EC_ecData_t *ec);
#endif
void PHY_EC_tdEchoEstimate(SINT32 mode, SINT16 *rxSignal, PHY_EC_ecData_t *ec);

// PHY_EC - echo estimate transformation
void PHY_EC_fdCoefficients(PHY_EC_ecData_t *ec, SINT32 mode, SINT32 stage);
void PHY_EC_tdCoefficients(PHY_EC_ecData_t *ec, SINT32 mode, SINT32 stage);

// PHY_EC - echo estimate windowing
void PHY_EC_fdWindow(PHY_EC_ecData_t *ec, SINT32 mode, SINT32 stage);
void PHY_EC_tdWindow(PHY_EC_ecData_t *ec, SINT32 mode, SINT32 stage);

// PHY_EC - initial echo estimate
void PHY_EC_initialEchoEstimate(complex_short *rxSignal, PHY_EC_ecData_t *ec);

// PHY_EC - delay and scale
void PHY_EC_adaptationParameter(PHY_EC_ecData_t *ec);
void PHY_EC_delayEstimate(SINT16 *rxSignal, PHY_EC_ecData_t *ec);
void PHY_EC_delayAndScale();
void PHY_EC_delayChange(PHY_EC_ecData_t *ec, SINT32 changeInDelay);

// PHY_EC - TEQ update
void PHY_EC_teqUpdate(PHY_EC_ecData_t *ec);

// PHY_EC - tracking and cancellation
#if PHY_EC_ENABLE_SHOWTIME_UPDATES
void PHY_EC_trackAndCancelEcho(complex_short *rxSignal, PHY_EC_ecData_t *ec);
#endif

// PHY_EC - filter writing
void PHY_EC_filterUpdate(PHY_EC_ecData_t *ec, SINT32 hwWriteMode);


/*******************************************************************************
*
* ENDIF
*
*******************************************************************************/

#endif // _PHY_EC_H
