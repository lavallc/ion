#ifndef ANCSHANDLER_H
#define ANCSHANDLER_H

extern "C" {
#include "ble_types.h"
#include "nordic_common.h"
}

#include <string.h>
#include "BluetoothLE.h"
#include "PatternManager.h"
#include "NotificationManager.h"
#include "BondManager.h"

#define ANCS_ATTRIBUTE_DATA_MAX                     32                                   /*<< Maximium notification attribute data length. */
#define TX_BUFFER_MASK                   0x07                                              /**< TX Buffer mask, must be a mask of contiguous zeroes, followed by contiguous sequence of ones: 000...111. */
#define TX_BUFFER_SIZE                   (TX_BUFFER_MASK + 1)                              /**< Size of send buffer, which is 1 higher than the mask. */
#define WRITE_MESSAGE_LENGTH             20                                                /**< Length of the write message for CCCD/control point. */

#define kANCSServiceUUID UUID("7905F431-B5CE-4E99-A40F-4B1E122D00D0")
#define kANCSControlPoint UUID("69d1d8f3-45e1-49a8-9821-9bbdfdaad9d9")
#define kANCSNotificationSource UUID("9FBF120D-6301-42D9-8C58-25E699A21DBD")
#define kANCSDataSource UUID("22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB")

#define BLE_UUID_APPLE_NOTIFICATION_CENTER_SERVICE  0xf431                               /*<< ANCS service UUID. */
#define BLE_UUID_ANCS_CONTROL_POINT_CHAR            0xd8f3                               /*<< Control point UUID. */
#define BLE_UUID_ANCS_NOTIFICATION_SOURCE_CHAR      0x120d                               /*<< Notification source UUID. */
#define BLE_UUID_ANCS_DATA_SOURCE_CHAR              0xc6e9                               /*<< Data source UUID. */


typedef enum
{
    STATE_UNINITIALIZED = 0,                                                                   /**< Uninitialized state of the internal state machine. */
    STATE_IDLE,                                                                            /**< Idle state, this is the state when no master has connected to this device. */
    STATE_DISC_SERV,                                                                       /**< A BLE master is connected and a service discovery is in progress. */
    STATE_DISC_CHAR,                                                                       /**< A BLE master is connected and characteristic discovery is in progress. */
    STATE_DISC_DESC,                                                                       /**< A BLE master is connected and descriptor discovery is in progress. */
    STATE_RUNNING,                                                                         /**< A BLE master is connected and complete service discovery has been performed. */
    STATE_WAITING_ENC,                                                                     /**< A previously bonded BLE master has re-connected and the service awaits the setup of an encrypted link. */
    STATE_RUNNING_NOT_DISCOVERED,                                                          /**< A BLE master is connected and a service discovery is in progress. */
} ancs_state_t;


/* brief Structure used for holding the characteristic found during discovery process.
 */
typedef struct __attribute__ ((packed))
{
    ble_uuid_t               uuid;                                                         /**< UUID identifying this characteristic. */
    ble_gatt_char_props_t    properties;                                                   /**< Properties for this characteristic. */
    uint16_t                 handle_decl;                                                  /**< Characteristic Declaration Handle for this characteristic. */
    uint16_t                 handle_value;                                                 /**< Value Handle for the value provided in this characteristic. */
    uint16_t                 handle_cccd;                                                  /**< CCCD Handle value for this characteristic. BLE_ANCS_INVALID_HANDLE if not present in the master. */
} apple_characteristic_t;

/**@brief Structure used for holding the Alert Notification Service found during discovery process.
 */
typedef struct __attribute__ ((packed))
{
    uint8_t                  handle;                                                       /**< Handle of Alert Notification Service which identifies to which master this discovered service belongs. */
    ble_gattc_service_t      service;                                                      /**< The GATT service holding the discovered Alert Notification Service. */
    apple_characteristic_t   control_point;
    apple_characteristic_t   notification_source;
    apple_characteristic_t   data_source;
} apple_service_t;




typedef enum
{
    BLE_ANCS_C_EVT_DISCOVER_COMPLETE,          /**< A successful connection has been established and the characteristics of the server has been fetched. */
    BLE_ANCS_C_EVT_DISCOVER_FAILED,            /**< It was not possible to discover service or characteristics of the connected peer. */
    BLE_ANCS_C_EVT_IOS_NOTIFICATION,           /**< An iOS notification was received on the notification source control point. */
    BLE_ANCS_C_EVT_NOTIF_ATTRIBUTE             /**< A received iOS a received notification attribute has been completely parsed and reassembled. */
} ble_ancs_c_evt_type_t;


typedef struct __attribute__ ((packed))
{
    uint8_t                            event_id;
    uint8_t                            event_flags;
    uint8_t                            category_id;
    uint8_t                            category_count;
    uint8_t                            notification_uid[4];
} ble_ancs_c_evt_ios_notification_t;

typedef struct __attribute__ ((packed))
{
    uint8_t                            command_id;
    uint8_t                            notification_uid[4];
    uint8_t                            attribute_id;
    uint16_t                           attribute_len;
    uint8_t                            data[ANCS_ATTRIBUTE_DATA_MAX];
} ble_ancs_c_evt_notif_attribute_t;

typedef struct __attribute__ ((packed))
{
    ble_ancs_c_evt_type_t               evt_type;                                         /**< Type of event. */
    ble_uuid_t                          uuid;                                             /**< UUID of the event in case of an apple or notification. */
    union
    {
        ble_ancs_c_evt_ios_notification_t   notification;
        ble_ancs_c_evt_notif_attribute_t    attribute;
        uint32_t                        error_code;                                       /**< Additional status/error code if the event was caused by a stack error or gatt status, e.g. during service discovery. */
    } data;
} ble_ancs_c_evt_t;
typedef struct {
    uint8_t                            attribute_id;
    uint16_t                           attribute_len;
} ble_ancs_attr_list_t;
typedef enum
{
    COMMAND_ID,
    NOTIFICATION_UID1,
    NOTIFICATION_UID2,
    NOTIFICATION_UID3,
    NOTIFICATION_UID4,
    ATTRIBUTE_ID,
    ATTRIBUTE_LEN1,
    ATTRIBUTE_LEN2,
    ATTRIBUTE_READY
} parse_state_t;
typedef enum
{
    BLE_ANCS_EVENT_ID_NOTIFICATION_ADDED,
    BLE_ANCS_EVENT_ID_NOTIFICATION_MODIFIED,
    BLE_ANCS_EVENT_ID_NOTIFICATION_REMOVED
} ble_ancs_event_id_values_t;

typedef enum
{
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_TITLE,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_SUBTITLE,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE_SIZE,
    BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_DATE,
} ble_ancs_notification_attribute_id_values_t;

typedef enum
{
    BLE_ANCS_COMMAND_ID_GET_NOTIFICATION_ATTRIBUTES,
    BLE_ANCS_COMMAND_ID_GET_APP_ATTRIBUTES
} ble_ancs_command_id_values_t;

typedef enum
{
    READ_REQ = 1,                                                                          /**< Type identifying that this tx_message is a read request. */
    WRITE_REQ                                                                              /**< Type identifying that this tx_message is a write request. */
} ancs_tx_request_t;

class CANCSHandler {

  private:
	ble_gattc_write_params_t gattc_params;
	ble_gattc_write_params_t gattc_params_spare;
	uint8_t gattc_value[WRITE_MESSAGE_LENGTH];
	parse_state_t parse_state;
	bool addr_check = false;
	uint16_t conn_handle;
	ancs_state_t m_state;
	apple_service_t  m_service;
	bond_data_t ancs_bond_data;

	bool has_bonds = false;
	uint32_t err_num;
	uint8_t cb_param;
	uint8_t last_catagory_id;
	bool callback_requested;
	void (*cb_function)(uint8_t);
	void event_confirm_service_data(ble_evt_t * p_ble_evt);
	void event_confirm_char_data(ble_evt_t * p_ble_evt);
	void characteristic_disc_req_send(ble_gattc_handle_range_t * p_handle);
	void descriptor_disc_req_send();
	void event_notify(ble_evt_t * p_ble_evt);
	void event_connected_req_auth();
	void descriptor_set(const ble_gattc_desc_t * p_desc_resp);
	void event_disconnect(ble_evt_t * p_ble_evt);
	void parse_get_notification_attributes_response(const uint8_t *data, int len);
	uint32_t ble_ancs_get_notification_attributes(uint8_t * p_uid, uint8_t num_attr, ble_ancs_attr_list_t *p_attr);
	void tx_buffer_process(uint8_t request);
	bool is_ios_device = false;
	bool set_configure_cccd = false;
	void reconfigure_ds_cccd();
	void set_state_running();
  public:
	void request_onconnect_callback(void (*foo)(uint8_t), uint8_t param);
	void event_discover_services();
	bool enable_cccd_check = false;
	bool is_bonded = false;
	bool store_sys_settings = false;
	void enable_ANCS(bool enable);
	void configure_cccd();
    void init();
    void ble_evt_dispatch(ble_evt_t * p_ble_evt);

};


extern CANCSHandler ANCSHandler;

#endif /* ANCSHANDLER_H_ */
