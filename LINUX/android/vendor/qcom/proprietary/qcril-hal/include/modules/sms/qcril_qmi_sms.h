/******************************************************************************
#  Copyright (c) 2010, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_sms.h
  @brief   qcril qmi - SMS

  DESCRIPTION
    Handles RIL requests, Callbacks, indications for QMI SMS.

******************************************************************************/


#ifndef QCRIL_QMI_SMS_H
#define QCRIL_QMI_SMS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <pthread.h>
#include "comdef.h"
#include <telephony/ril.h>
#include <telephony/ril_cdma_sms.h>
#include "qmi_idl_lib.h"
#include "qmi_client.h"
#ifdef QMI_RIL_UTF
#include "qcril_log.h"
#endif
#include "wireless_messaging_service_v01.h"
#include <framework/legacy.h>
#ifdef __cplusplus
#include <memory>
#include <modules/sms/RilRequestSendSmsMessage.h>
#include <modules/sms/RilRequestCdmaSendSmsMessage.h>
#include <modules/sms/RilRequestImsSendSmsMessage.h>
#include <modules/sms/RilRequestAckGsmSmsMessage.h>
#include <modules/sms/RilRequestAckCdmaSmsMessage.h>
#include <modules/sms/RilRequestDeleteSmsOnSimMessage.h>
#include <modules/sms/RilRequestWriteSmsToSimMessage.h>
#include <modules/sms/RilRequestCdmaWriteSmsToRuimMessage.h>
#include <modules/sms/RilRequestCdmaDeleteSmsOnRuimMessage.h>
#include <modules/sms/RilRequestGetSmscAddressMessage.h>
#include <modules/sms/RilRequestSetSmscAddressMessage.h>
#include <modules/sms/RilRequestGetGsmBroadcastConfigMessage.h>
#include <modules/sms/RilRequestGetCdmaBroadcastConfigMessage.h>
#include <modules/sms/RilRequestGsmSetBroadcastSmsConfigMessage.h>
#include <modules/sms/RilRequestCdmaSetBroadcastSmsConfigMessage.h>
#include <modules/sms/RilRequestGsmSmsBroadcastActivateMessage.h>
#include <modules/sms/RilRequestCdmaSmsBroadcastActivateMessage.h>
#include <modules/sms/RilRequestReportSmsMemoryStatusMessage.h>
#include <modules/sms/RilRequestGetImsRegistrationMessage.h>
#include <modules/sms/RilRequestAckImsSmsMessage.h>
#endif
#include "modules/sms/SmsImsServiceStatusInd.h"
/*===========================================================================

                                FUNCTIONS

===========================================================================*/

#ifdef __cplusplus
void qcril_qmi_sms_command_cb
(
   unsigned int                 msg_id,
   std::shared_ptr<void>        resp_c_struct,
   unsigned int                 resp_c_struct_len,
   void                        *resp_cb_data,
   qmi_client_error_type        transp_err
);
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// sms_pre_init to be called at bootup, but not on SSR
void qcril_qmi_sms_pre_init(void);

qmi_client_error_type qcril_qmi_sms_init(void);

/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/


#define QCRIL_SMS_BC_MM_TABLE_SIZE  50

/* Size of the buffer to hold a received GW SMS message or Status Report.
   Multiply the size of the QMI payload by two since it will be translated to ASCII hex format. */
#define QCRIL_SMS_BUF_MAX_SIZE  (WMS_MESSAGE_LENGTH_MAX_V01 * 2)

/* Maximum number of special chars that can be added to the SMSC address (quotes, comma, plus) */
#define QCRIL_SMS_MAX_SMSC_SPECIAL_CHARS              4
/* Max size of SMSC Address is sizeof(address_type) + maximum number of address digits +
   special characters */
#define QCRIL_SMS_MAX_SMSC_ADDRESS_SIZE (WMS_ADDRESS_TYPE_MAX_V01 + \
                                         WMS_ADDRESS_DIGIT_MAX_V01 + \
                                         QCRIL_SMS_MAX_SMSC_SPECIAL_CHARS)

typedef struct
{
  /* GW Ack Info */
  boolean gw_ack_pending;                       /* Indicates whether QCRIL is waiting for Android
                                                   to ack an MT SMS message */
  boolean gw_ack_needed;                        /* Indicates whether QCRIL will drop the Android ack
                                                   (QCRIL will drop the ack if the modem indicates
                                                   that no ack is needed) */
  uint32 gw_transaction_id;                     /* Transaction id for the GW SMS message to be acked */
  boolean gw_send_ack_on_ims;                   /* Indicates whether the ack must be sent on IMS */

  /* CDMA Ack Info */
  boolean cdma_ack_pending;                     /* Indicates whether QCRIL is waiting for Android
                                                   to ack an MT SMS message */
  boolean cdma_ack_needed;                      /* Indicates whether QCRIL will drop the Android ack
                                                   (QCRIL will drop the ack if the modem indicates
                                                   that no ack is needed) */
  uint32 cdma_transaction_id;                   /* Transaction id for the CDMA SMS message to be acked */
  boolean cdma_send_ack_on_ims;                 /* Indicates whether the ack must be sent on IMS */
} qcril_sms_ack_info_type;

typedef enum
{
    QMI_RIL_SMS_FORMAT_UNKNOWN = 0,
    QMI_RIL_SMS_FORMAT_3GPP = 1,
    QMI_RIL_SMS_FORMAT_3GPP2 = 2,
} qmi_ril_sms_format_type;

typedef struct
{
  uint8_t                   block_sms_on_1x;
  uint8_t                   registered_ind_valid;
  uint8_t                   registered_ind;
  uint8_t                   sms_format_valid;
  qmi_ril_sms_format_type   sms_format;
}qcril_sms_transport_layer_info;

typedef struct
{
  qcril_sms_ack_info_type     sms_ack_info; /* Ack info */
  qtimutex::QtiSharedMutex sms_ack_info_mutex;       /* Mutex to protect access to SMS ACK info */
  uint32                      mt_pending_ack_expry_tmr;
  qcril_sms_transport_layer_info     transport_layer_info; /* Transport layer info */
  qtimutex::QtiSharedMutex transport_layer_info_mutex;       /* Mutex to protect access to Transport layer info */

} qcril_sms_struct_type;

//Bootup Power Optimization
#define MAX_MT_SMS_POWER_OPT_BUFFER 20
typedef struct
{
  uint8_t mt_sms_with_ack_needed_valid;
  int mt_sms_with_ack_needed_length;
  wms_event_report_ind_msg_v01 mt_sms_with_ack_needed[MAX_MT_SMS_POWER_OPT_BUFFER];
  uint32 mt_sms_ack_needed_expry_tmr;

  uint8_t mt_sms_with_ack_not_needed_valid;
  int mt_sms_with_ack_not_needed_length;
  wms_event_report_ind_msg_v01 mt_sms_with_ack_not_needed[MAX_MT_SMS_POWER_OPT_BUFFER];
} qcril_mt_sms_buffer_power_opt_payload;

/*===========================================================================

                                UTILITY DEFINITIONS AND TYPES

===========================================================================*/

enum { QCRIL_SMS_TL_MAX_LEN        = 246 };

typedef enum
{
  QCRIL_SMS_TL_TYPE_POINT_TO_POINT    = 0,
  QCRIL_SMS_TL_TYPE_BROADCAST         = 1,
  QCRIL_SMS_TL_TYPE_ACK               = 2,
  QCRIL_SMS_TL_TYPE_MAX               = 2
} qcril_sms_tl_message_type_e_type;


/* ------------------------ */
/* ---- Teleservice Id ---- */
/* ------------------------ */
typedef enum
{
  QCRIL_SMS_TELESERVICE_WMT                = 0x00001002,  // Wireless Messaging Teleservice
  QCRIL_SMS_TELESERVICE_WEMT               = 0x00001005,  // Wireless Enhanced Messaging Teleservice
  QCRIL_SMS_TELESERVICE_IS91_PAGE          = 0x00010000,
  QCRIL_SMS_TELESERVICE_IS91_VOICE_MAIL    = 0x00020000,
  QCRIL_SMS_TELESERVICE_IS91_SHORT_MESSAGE = 0x00030000,
  QCRIL_SMS_TELESERVICE_MWI                = 0x00040000,
  QCRIL_SMS_TELESERVICE_BROADCAST          = 0x00050000,
  QCRIL_SMS_TELESERVICE_UNKNOWN            = 0x0FFFFFFF
} qcril_sms_teleservice_e_type;

/* Transport Layer parameter mask values:
*/
enum{ QCRIL_SMS_MASK_TL_NULL                = 0x00000000 };
enum{ QCRIL_SMS_MASK_TL_TELESERVICE_ID      = 0x00000001 };
enum{ QCRIL_SMS_MASK_TL_BC_SRV_CATEGORY     = 0x00000002 };
enum{ QCRIL_SMS_MASK_TL_ADDRESS             = 0x00000004 };
enum{ QCRIL_SMS_MASK_TL_SUBADDRESS          = 0x00000008 };
enum{ QCRIL_SMS_MASK_TL_BEARER_REPLY_OPTION = 0x00000040 };
enum{ QCRIL_SMS_MASK_TL_CAUSE_CODES         = 0x00000080 };
enum{ QCRIL_SMS_MASK_TL_BEARER_DATA         = 0x00000100 };

/* Transport Layer parameter Ids:
*/
typedef enum
{
  QCRIL_SMS_TL_DUMMY          = -1,  /* dummy */
  QCRIL_SMS_TL_TELESERVICE_ID = 0,  /* Teleservice Identifier     */
  QCRIL_SMS_TL_BC_SRV_CATEGORY,     /* Broadcast Service Category */
  QCRIL_SMS_TL_ORIG_ADDRESS,        /* Originating Address        */
  QCRIL_SMS_TL_ORIG_SUBADDRESS,     /* Originating Subaddress     */
  QCRIL_SMS_TL_DEST_ADDRESS,        /* Destination Address        */
  QCRIL_SMS_TL_DEST_SUBADDRESS,     /* Destination Subaddress     */
  QCRIL_SMS_TL_BEARER_REPLY_OPTION, /* Bearer Reply Option        */
  QCRIL_SMS_TL_CAUSE_CODES,         /* Cause Codes                */
  QCRIL_SMS_TL_BEARER_DATA          /* Bearer Data                */

} qcril_sms_tl_parm_id_e_type;

// C.S0015-B v1.0 - Table 4.5.1-1. Message Types
// ‘0100’ Delivery Acknowledgment (mobile-terminated only)
#define QCRIL_3GPP2_SMS_DELIVERY_ACK    0x04

#define TL_HEADER_SIZE    1
#define TL_PARM_SIZE      2

#define MSG_DUP_PARM QCRIL_LOG_ERROR("Duplicate parm: %d", parm_id, 0,0)

typedef enum
{
    QMI_RIL_SMS_SVC_NOT_INITIALZIED = 0,
    QMI_RIL_SMS_SVC_INIT_PENDING,
    QMI_RIL_SMS_SVC_FULLY_OPERATIONAL
} qmi_ril_sms_svc_status_type;

typedef enum
{
    QMI_RIL_SMS_PRIMARY_CLIENT_NOT_SET = 0,
    QMI_RIL_SMS_PRIMARY_CLIENT_SET,
} qmi_ril_sms_primary_client_status_type;

typedef struct
{
    boolean is_wms_full_service;
    boolean is_ims_wms_reg;
    qmi_ril_sms_format_type format;
} qmi_ril_sms_ims_reg_state;

typedef struct
{
  boolean                      is_mo;
  qcril_sms_tl_message_type_e_type   tl_message_type;
  uint16                       mask;
  uint32                       teleservice;
  RIL_CDMA_SMS_Address         address;
  RIL_CDMA_SMS_Subaddress      subaddress;
  uint8                        bearer_reply_seq_num;
  uint32                       service;
} qcril_sms_tl_message_type;

typedef struct
{
  uint16                      data_len;
  uint8                       data[ RIL_CDMA_SMS_BEARER_DATA_MAX ];
} qcril_sms_OTA_message_type;



/*===========================================================================

                                UTILITY FUNCTIONS

===========================================================================*/

boolean qcril_sms_convert_sms_ril_to_qmi
(
  RIL_CDMA_SMS_Message        *cdma_sms_msg,
  uint8                       *data_buf,
  uint16                       data_buf_len,
  uint16                      *ota_data_len,
  boolean                     sms_on_ims,
  boolean                     is_mo_sms
);

boolean qcril_sms_convert_mt_sms_qmi_to_ril
(
  const uint8                 *data,
  uint16                      length,
  RIL_CDMA_SMS_Message        *cdma_sms_msg
);

void qcril_sms_hex_to_byte(const char * hex_pdu,   // INPUT
                           byte * byte_pdu,        // OUTPUT
                           uint32 num_hex_chars);

void qcril_sms_byte_to_hex(byte * byte_pdu,   // INPUT
                           char * hex_pdu,    // OUTPUT
                           uint32 num_bytes);

boolean qcril_sms_convert_smsc_address_to_qmi_format(const char * input_smsc_address,
                                                     wms_set_smsc_address_req_msg_v01 * qmi_request);

RIL_Errno qcril_sms_convert_smsc_address_to_ril_format(const wms_get_smsc_address_resp_msg_v01 * qmi_response,
                                                       std::string& output_sms_address);

wms_message_tag_type_enum_v01 qcril_sms_map_ril_tag_to_qmi_tag(int ril_tag);
boolean qcril_sms_is_tag_mo(int ril_tag);

const char *qcril_sms_lookup_cmd_name(unsigned long qmi_cmd);

const char *qcril_sms_lookup_ind_name(unsigned long qmi_ind);

qmi_client_error_type qcril_sms_perform_initial_configuration( void );

void qmi_ril_set_sms_svc_status(qmi_ril_sms_svc_status_type new_status);
qmi_ril_sms_svc_status_type qmi_ril_get_sms_svc_status(void);

void qmi_ril_set_primary_client_status(qmi_ril_sms_primary_client_status_type new_status);
qmi_ril_sms_primary_client_status_type qmi_ril_get_primary_client_status(void);

void qcril_sms_post_ready_status_update(void);

#ifdef QMI_RIL_UTF
boolean qcril_sms_convert_tl_to_qmi
(
  qcril_sms_tl_message_type        * tl_msg_ptr,   /* IN */
  uint32                       raw_bd_len,   /* IN */
  uint8                      * raw_bd_ptr,   /* IN */
  qcril_sms_OTA_message_type       * OTA_msg_ptr   /* OUT */
);

void qcril_sms_convert_ril_to_tl
(
  RIL_CDMA_SMS_Message *cdma_sms_msg,
  qcril_sms_tl_message_type * tl_ptr,
  boolean sms_on_ims,
  boolean is_mo_sms
);
#endif

void qcril_qmi_sms_unsolicited_indication_cb_helper
(
  unsigned int   msg_id,
  unsigned char *decoded_payload,
  uint32_t       decoded_payload_len
);

void qcril_sms_request_send_gw_sms
(
  std::shared_ptr<RilRequestSendSmsMessage> msg
);

void qcril_sms_request_raw_read
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
);

void qcril_sms_request_sms_acknowledge
(
  std::shared_ptr<RilRequestAckGsmSmsMessage> msg
);

void qcril_sms_request_write_sms_to_sim
(
  std::shared_ptr<RilRequestWriteSmsToSimMessage> msg
);

void qcril_sms_request_delete_sms_on_sim
(
  std::shared_ptr<RilRequestDeleteSmsOnSimMessage> msg
);

void qcril_sms_request_get_smsc_address
(
  std::shared_ptr<RilRequestGetSmscAddressMessage> msg
);

void qcril_sms_request_set_smsc_address
(
  std::shared_ptr<RilRequestSetSmscAddressMessage> msg
);

void qcril_sms_request_report_sms_memory_status
(
  std::shared_ptr<RilRequestReportSmsMemoryStatusMessage> msg
);

void qcril_sms_request_gsm_get_broadcast_sms_config
(
  std::shared_ptr<RilRequestGetGsmBroadcastConfigMessage> msg 
);

void qcril_sms_request_gsm_set_broadcast_sms_config
(
  std::shared_ptr<RilRequestGsmSetBroadcastSmsConfigMessage> msg
);

void qcril_sms_request_gsm_sms_broadcast_activation
(
  std::shared_ptr<RilRequestGsmSmsBroadcastActivateMessage> msg
);

void qcril_sms_request_cdma_send_sms
(
  std::shared_ptr<RilRequestCdmaSendSmsMessage> msg
);

void qcril_sms_request_cdma_sms_acknowledge
(
  std::shared_ptr<RilRequestAckCdmaSmsMessage> msg
);

void qcril_sms_request_cdma_write_sms_to_ruim
(
  std::shared_ptr<RilRequestCdmaWriteSmsToRuimMessage> msg
);

void qcril_sms_request_cdma_delete_sms_on_ruim
(
  std::shared_ptr<RilRequestCdmaDeleteSmsOnRuimMessage> msg
);

void qcril_sms_request_cdma_get_broadcast_sms_config
(
  std::shared_ptr<RilRequestGetCdmaBroadcastConfigMessage> msg
);

void qcril_sms_request_cdma_set_broadcast_sms_config
(
  std::shared_ptr<RilRequestCdmaSetBroadcastSmsConfigMessage> msg
);

void qcril_sms_request_cdma_sms_broadcast_activation
(
  std::shared_ptr<RilRequestCdmaSmsBroadcastActivateMessage> msg
);

void qcril_sms_request_ims_registration_state
(
  std::shared_ptr<RilRequestGetImsRegistrationMessage> msg
);

void qcril_sms_request_ims_send_sms
(
  std::shared_ptr<RilRequestImsSendSmsMessage> msg
);

void qcril_sms_request_ims_ack_sms
(
  std::shared_ptr<RilRequestAckImsSmsMessage> msg
);

int qcril_qmi_sms_block_retry_3pgg_sms_on_lte_only(RIL_RadioTechnologyFamily tech, unsigned char retry);

void qcril_qmi_sms_report_unsol_ims_state_change();
void query_wms_ready_status(void);

void qcril_sms_process_event_report_ind
(
  wms_event_report_ind_msg_v01 * event_report_ind,
  qcril_instance_id_e_type instance_id
);
void qcril_qmi_sms_reset_mt_sms_ack_needed_power_opt_buffer();
void qcril_qmi_sms_reset_mt_sms_ack_not_needed_power_opt_buffer();
void qcril_qmi_sms_update_mt_sms_with_ack_needed_power_opt_buffer(wms_event_report_ind_msg_v01 *ind_data_ptr);
void qcril_qmi_sms_notify_mt_sms_with_ack_needed_power_opt_to_atel();
void qcril_qmi_sms_update_mt_sms_with_ack_not_needed_power_opt_buffer(wms_event_report_ind_msg_v01 *ind_data_ptr);
void qcril_qmi_sms_notify_mt_sms_with_ack_not_needed_power_opt_to_atel();
void qcril_sms_update_mt_sms_power_opt_buffer(wms_event_report_ind_msg_v01 *event_report_ind);
qmi_ril_sms_format_type qcril_qmi_sms_get_ims_sms_format();
RIL_Errno qcril_qmi_sms_retrieve_ims_sms_registration_state(qmi_ril_sms_ims_reg_state& ims_sms_state, SmsImsServiceStatusInd::SmsImsServiceStatusInfo& smsImsServiceStatus);


#ifdef QMI_RIL_UTF
void qcril_qmi_hal_sms_module_cleanup();
#endif

#ifdef __cplusplus
}
#endif
#endif /* QCRIL_QMI_SMS_H */
