#ifndef SNS_SAM_ALGO_UTILS
#define SNS_SAM_ALGO_UTILS
/*============================================================================
  @file sns_sam_algo_utils.h

  Utility functions for SAM algorithms.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/

#include "sns_sam_algo_api.h"
#include "sns_smgr_api_v01.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

/* bitmap, all flags disabled */
#define SNS_SAM_DEFAULT_SAMPLE_QUALITY  0x0000

#define SNS_SAM_UTIL_INVALID_SENSORINFO 255

/* Accelerometer range limited to +/-4G */
#define SNS_SAM_UTIL_MAX_ACCEL_Q16 2570755

/* Gyro maximum absolute bias in 25dps */
#define SNS_SAM_GYRO_CAL_MAX_ABS_BIAS   FX_FLTTOFIX_Q16(0.4363f)

/* infinite small report rate for batch mode */
#define SNS_SAM_BATCH_REPORT_RATE_Q16 0x00000001

#if defined(SNS_DSPS_BUILD) || defined(FAST_PLAYBACK) || (defined(SNS_PCSIM) && !defined(ENABLE_APPS_PLAYBACK))
# define SNS_SAM_MODULE              SNS_MODULE_DSPS_SAM
# define SNS_SAM_DBG_MOD             SNS_DBG_MOD_DSPS_SAM
#else
# define SNS_SAM_MODULE              SNS_MODULE_APPS_SAM
# define SNS_SAM_DBG_MOD             SNS_DBG_MOD_APPS_SAM
#endif

#define SNS_SAM_FUSION_DEF_SAMPLE_QUALITY SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01
#define SNS_SAM_FUSION_MIN_SAMPLE_RATE_HZ (5)
#define SNS_SAM_FUSION_MIN_SAMPLE_RATE_HZ_Q16 (FX_CONV_Q16(SNS_SAM_FUSION_MIN_SAMPLE_RATE_HZ, 0))
#define SNS_SAM_FUSION_MAX_SAMPLE_RATE_HZ (200)
#define SNS_SAM_FUSION_MAX_SAMPLE_RATE_HZ_Q16 (FX_CONV_Q16(SNS_SAM_FUSION_MAX_SAMPLE_RATE_HZ, 0))


/*============================================================================
  Type Declarations
  ===========================================================================*/

typedef enum
{
   SNS_SAM_CAL_TYPE_UNCAL = 0,
   SNS_SAM_CAL_TYPE_FAC = 1,
   SNS_SAM_CAL_TYPE_AUTO = 2,
} sns_sam_cal_type_e;

typedef enum
{
   SNS_SAM_CAL_ACCURACY_UNRELIABLE = 0,
   SNS_SAM_CAL_ACCURACY_LOW = 1,
   SNS_SAM_CAL_ACCURACY_MEDIUM = 2,
   SNS_SAM_CAL_ACCURACY_HIGH = 3
} sns_sam_cal_accuracy_e;

typedef struct {
   uint8_t sensorId;             /*sensor id*/
   uint8_t dataType;             /*sensor data type*/
   uint16_t sampleQual;          /*sample quality*/
   uint32_t buffered_sampleRate; /*sample rate in Hz, Q16 – rate at which dependent sensor is required to sample*/
} sns_sam_sensor_info;

typedef struct {
   uint64_t srcUID;              /*sensor Unique ID*/
   uint32_t sampleRate;          /*sample rate in Hz, Q16 (leftover from legacy implementation) set same as buffered_sampleRate from above*/
   uint32_t reportRate;          /*report rate in Hz, Q16 – rate at which dependent sensor streaming or batched data is reported to requesting algorithm */
   uint8_t sensorCount;          /*sensor count*/
   sns_sam_sensor_info sensorDbase[SNS_SMGR_MAX_ITEMS_PER_REPORT_V01];
} sns_sam_sensor_data_info_req;

typedef struct
{
   uint64_t sensorUid;
   sns_sam_timestamp timestamp;
   uint8_t dimension_count;
   int32_t data[SNS_SMGR_SENSOR_DIMENSION_V01];
   sns_sam_cal_type_e cal_type;
   sns_sam_cal_accuracy_e accuracy;
} sns_sam_sensor_data_s;

typedef struct
{
  sns_sam_sensor_data_s current_sample;
  bool buffer_data_valid;
  sns_smgr_buffering_sample_s_v01 * samplePtr[SNS_SMGR_BUFFERING_REQUEST_MAX_ITEMS_V01];
  uint32_t sampleCount[SNS_SMGR_BUFFERING_REQUEST_MAX_ITEMS_V01];
  sns_sam_timestamp timestamp[SNS_SMGR_BUFFERING_REQUEST_MAX_ITEMS_V01];
  uint32_t samplesProcessed;
  uint64_t sensorUID[SNS_SMGR_BUFFERING_REQUEST_MAX_ITEMS_V01];
  sns_smgr_buffering_ind_msg_v01 *indPtr;

}sns_sam_buffered_data;

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
* Generates a periodic SMGR enable request message.
*
* @param[i] sensorReq Information needed to generate the enable message
* @param[io] enableReqMsg Pointer to the enable request messge.
*                         Will be filled be this function.
*/
void
sns_sam_util_gen_smgr_periodic_req( sns_sam_sensor_data_info_req *sensorReq,
  sns_sam_enable_req *enableReqMsg);

/**
* Generates a buffered SMGR enable request message.
*
* @param[i] sensorReq Information needed to generate the enable message
* @param[io] enableReqMsg Pointer to the enable request messge.
*                         Will be filled be this function.
*
* @return SAM_ENONE
*         SAM_ERANGE > 1 sensor is requested in a single enable req
*/
sns_sam_err
sns_sam_util_gen_smgr_buffer_req( sns_sam_sensor_data_info_req *sensorReq,
  sns_sam_enable_req *enableReqMsg);

/**
* Initializes internal data structures to be used in sns_sam_get_next_buffer_item
*
* @param[io] bufSample Pointer to a structure that contains information needed
*            to extract the next sample from the indication. Also contains sensor
*            data from the current indication.
* @retuen SAM_ENONE
*/

sns_sam_err sns_sam_buffered_data_init(sns_sam_buffered_data *bufSample);

/**
* Extracts a single sample from an SMGR buffering indication. This function can
* be called repeatedly in a loop to extract all the samples from an indication.
*
* To restart at the beginning of the indication message, call sns_sam_buffered_data_init
*
* @param[i] indPtr      Indication message pointer from which sensor samples are
*           to be extracted.
* @param[io] bufSample  Pointer to a structure that contains information needed
*            to extract the next sample from the indication. Also contains sensor
*            data from the current indication.
* @param[o] sampleIdx The index number for this item generated from the buffering indication.
*
* @return SAM_EMAX when all elements have been extracted
*         SAM_ENOT_AVAILABLE if the sample was marked as invalid.
*         SAM_EFAILED An error occured while extracting the sample.
*         SAM_ENONE otherwise
*/
sns_sam_err sns_sam_get_single_buffered_sample(sns_smgr_buffering_ind_msg_v01 *indPtr,
  sns_sam_buffered_data *bufSample, uint32_t *sampleIdx);

/**
* Extracts samples from an SMGR report indication
*
* @param[i] indPtr Indication message pointer from which sensor samples
*                  are to be extracted
* @param[i] maxSamples Maxi number of samples that can be put into bufSamples
* @param[o] periodicSample Sensor samples extracted from indication
* @param[o] PeriodicSampleCnt # of sensor samples extracted from indication
*
* @return SAM_ENONE
*         SAM_EFAILED A problem occured in parsing this indication.
*/
sns_sam_err
sns_sam_process_smgr_report_ind(
  const sns_smgr_periodic_report_ind_msg_v01 *indPtr,
  sns_sam_sensor_data_s periodicSample[], uint32_t *PeriodicSampleCnt );

/**
* Obtains the SensorId and the number of dimensions used by that sensor type,
* given the Sensor's UID.
*
* @param[i] sensorUID The sensor to lookup
*
* @return  8 bit sensor ID
*          SNS_SAM_UTIL_INVALID_SENSORINFO Sensor is not known
*/
uint8_t
sns_sam_get_sensor_id( sns_sam_sensor_uid sensorUID );


/**
* Obtains the the number of dimensions used by that sensor type, given
* the Sensor's UID.
*
* @param[i] sensorUID The Sensor's 64 bit UID
*
* @return   Number of dimensions supported by this sensor
*           SNS_SAM_UTIL_INVALID_SENSORINFO is returned if sensor is not known.
*/
uint8_t
sns_sam_get_sensor_dimensions( sns_sam_sensor_uid sensorUID );

/**
* Obtains the the Sensor UID, given the SMGR SensorId.
*
* @param[i] sensorID SMGR's Sensor Identifier
*
* @return The Sensor's UID
*         Returns 0 if sensor is not known
*/
sns_sam_sensor_uid
sns_sam_get_sensor_uid( uint8_t sensorID );

/**
* Calculates the appropriate sample rate, given the sample rate and report
* period from an enable request message.
*
* @param[i] sampleRate Sample rate in hertz; 0 if none was given
* @param[i] reportPeriod Report period in seconds
* @param[i] defSampleRate Default sample rate for this algorithm
*
* @return Sample rate (in Hz), or 0 if no valid sample rate could be determined
*/
q16_t
sns_sam_calc_sample_rate( q16_t sampleRate, q16_t reportPeriod,
  q16_t defSampleRate );

/**
* Limit the sensor data to the range +/- maxRange specified
*
* @param[i] input Input sensor data specified in q16
* @param[o] output Output (clipped) sensor data specified in q16
* @param[i] maxRange Maximum range for output sensor data in q16
*
* @return TRUE if sensor data is clipped, FALSE otherwise
*/
bool sns_sam_limit_sensor_data_range(
  const q16_t input[SNS_SMGR_SENSOR_DIMENSION_V01],
  q16_t output[SNS_SMGR_SENSOR_DIMENSION_V01],
  q16_t maxRange);

#endif /* SNS_SAM_ALGO_UTILS */
