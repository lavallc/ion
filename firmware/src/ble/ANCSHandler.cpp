
#include "BluetoothManager.h"
#include "ANCSHandler.h"
#include "PacketHandler.h"
#include "PatternManager.h"
#include "LumenSettings.h"

#define INVALID_SERVICE_HANDLE 0xFF

CANCSHandler ANCSHandler;

void CANCSHandler::request_onconnect_callback(void (*foo)(uint8_t),
		uint8_t param) {
	cb_function = foo;
	cb_param = param;
	callback_requested = true;
}

void CANCSHandler::tx_buffer_process(uint8_t request) {

	if (request == READ_REQ) {
		//sd_ble_gattc_read(conn_handle, m_tx_buffer[m_tx_index].req.read_handle, 0);
	} else {
		uint32_t error = sd_ble_gattc_write(conn_handle, &gattc_params);
		Serial.debug_print("wERR");
		Serial.debug_print(error);
	}

}

void CANCSHandler::init() {
	//Prepares the UUIDs to be passed to the softdevice.
	//The cpp UUID class handles the softdevice calls.
	UUID ANCSServiceUUID = kANCSServiceUUID;
	ANCSServiceUUID.init();
	UUID ANCSControlPoint = kANCSControlPoint;
	ANCSControlPoint.init();
	UUID ANCSNotificationSource = kANCSNotificationSource;
	ANCSNotificationSource.init();
	UUID ANCSDataSource = kANCSDataSource;
	ANCSDataSource.init();
	m_state = STATE_IDLE;
	conn_handle = BLE_CONN_HANDLE_INVALID;
}

//PacketManager checks for an Init Packet with the lowest firmware bit as a 1.
//This turns on "is_ios_device" to enable actions from ble_evt_dispatch below.
void CANCSHandler::enable_ANCS(bool enable) {
	if (enable) {
		Serial.debug_print("ENABLE");
		is_ios_device = true;
	}
	//Since we already connected, this will do the task of event BLE_GAP_EVT_CONNECTED below.
	if (m_state == STATE_IDLE && enable) {
		conn_handle = Nrf51822BluetoothStack::_stack->_conn_handle;
		ancs_bond_data.central_addr =
				Nrf51822BluetoothStack::_stack->_peer_addr;
		event_discover_services();
	}
}

void CANCSHandler::ble_evt_dispatch(ble_evt_t * p_ble_evt) {

	uint32_t err_code;
	uint16_t event = p_ble_evt->header.evt_id;
	bool reply;
	if (event == BLE_GAP_EVT_SEC_INFO_REQUEST) {
			ancs_bond_data.central_handle = p_ble_evt->evt.gap_evt.params.sec_info_request.div;
			reply = BondManager.load_bond(&ancs_bond_data, false);
		if (reply) {
			//PatternManager.set_current_pattern(0x0D);
			is_bonded = true;
			is_ios_device = true;
			conn_handle = Nrf51822BluetoothStack::_stack->_conn_handle;
			err_code = sd_ble_gap_sec_info_reply(conn_handle,
					&ancs_bond_data.auth_status.periph_keys.enc_info, NULL);
			if (err_code == NRF_SUCCESS) {
				set_state_running();
				if (callback_requested) {
					cb_function(cb_param);
					callback_requested = false;
				}
			}
			/*	else {
			 if (err_code == NRF_ERROR_INVALID_PARAM)
			 PatternManager.set_current_pattern(5);
			 if (err_code == NRF_ERROR_INVALID_STATE)
			 PatternManager.set_current_pattern(0xB);
			 if (err_code == BLE_ERROR_INVALID_CONN_HANDLE)
			 PatternManager.set_current_pattern(0xC);
			 if (err_code == NRF_ERROR_BUSY)
			 PatternManager.set_current_pattern(0xD);
			 event_disconnect(p_ble_evt);
			 } */
		} else {
			//Todo indicate that we have no information on the pairing device.
			//err_code = sd_ble_gap_sec_info_reply(conn_handle,NULL, NULL);
			nrf_delay_us(200);
			m_state = STATE_WAITING_ENC;
			is_ios_device = true;
			//event_discover_services();
			//is_ios_device = true;
		}
	}

	if (is_ios_device) {
		if (event == BLE_GATTS_EVT_SYS_ATTR_MISSING) {
			if (!is_bonded) {
				err_code = sd_ble_gatts_sys_attr_set(conn_handle, NULL, 0);
			} else {
				// Current central is valid, use its data. Set the corresponding sys_attr.
				err_code = sd_ble_gatts_sys_attr_set(conn_handle,
						ancs_bond_data.central_attr.sys_attr,
						(uint16_t) ancs_bond_data.central_attr.sys_attr_size);
				/*		if (err_code == BLE_ERROR_INVALID_CONN_HANDLE)
				 PatternManager.set_current_pattern(0x12);
				 else if(err_code == NRF_ERROR_INVALID_DATA)
				 PatternManager.set_current_pattern(0x15);
				 else if(err_code == NRF_ERROR_NO_MEM)
				 PatternManager.set_current_pattern(0x18);*/
			}
		}
		if (event == BLE_GAP_EVT_CONN_SEC_UPDATE) {

			if (is_bonded) {
				// Current central is valid, use its data. Set the corresponding sys_attr.
				err_code = sd_ble_gatts_sys_attr_set(conn_handle,
						ancs_bond_data.central_attr.sys_attr,
						(uint16_t) ancs_bond_data.central_attr.sys_attr_size);
				//if (err_code == BLE_ERROR_INVALID_CONN_HANDLE)
				//	PatternManager.set_current_pattern(0x12);
				//else if(err_code == NRF_ERROR_INVALID_DATA)
				//PatternManager.set_current_pattern(0x15);
				//else if (err_code == NRF_ERROR_NO_MEM)
				//	PatternManager.set_current_pattern(0x18);

			} else {
				err_code = sd_ble_gatts_sys_attr_set(conn_handle, NULL, 0);
			}
		}

		if (event == BLE_GAP_EVT_CONN_PARAM_UPDATE) {
			is_bonded = true;
			if (m_service.service.uuid.uuid
					!= BLE_UUID_APPLE_NOTIFICATION_CENTER_SERVICE) {
				event_discover_services();
			} else {
				configure_cccd();
				set_state_running();

			}
		}
		//This event happens after all authentication is done.
		//Save bond information
		if (event == BLE_GAP_EVT_AUTH_STATUS) {
			if (callback_requested) {
				cb_function(cb_param);
				callback_requested = false;
			}
			is_bonded = true;
			memcpy(&ancs_bond_data.central_addr.addr,
					&(Nrf51822BluetoothStack::_stack->_peer_addr.addr),
					BLE_GAP_ADDR_LEN);
			ancs_bond_data.central_addr.addr_type =
					Nrf51822BluetoothStack::_stack->_peer_addr.addr_type;
			ancs_bond_data.auth_status =
					p_ble_evt->evt.gap_evt.params.auth_status;
			ancs_bond_data.central_id_info.div =
					p_ble_evt->evt.gap_evt.params.auth_status.periph_keys.enc_info.div;
			ancs_bond_data.central_handle = ancs_bond_data.central_id_info.div;
			configure_cccd();
			BondManager.save_bond(&ancs_bond_data, false);

		}
		if (event == BLE_GAP_EVT_SEC_PARAMS_REQUEST) {
			ancs_bond_data.central_attr.sys_attr_size = 0;
			memset(ancs_bond_data.central_attr.sys_attr, 0,
					SYS_ATTR_BUFFER_MAX_LEN);
		}

		switch (m_state) {
		case STATE_UNINITIALIZED:
			// Initialization is handle in special case, thus if we enter here, it means that an
			// event is received even though we are not initialized --> ignore.
			break;
		case STATE_IDLE:
			m_state = STATE_WAITING_ENC;
		case STATE_WAITING_ENC:
			Serial.debug_print("1");
			//PatternManager.set_current_pattern(0x6);
			if (event == BLE_GAP_EVT_SEC_INFO_REQUEST
					|| event == BLE_GAP_EVT_AUTH_STATUS) {
				if (m_service.service.uuid.uuid
						!= BLE_UUID_APPLE_NOTIFICATION_CENTER_SERVICE) {
					event_discover_services();
				} else {
					event_connected_req_auth();
				}
			} else if (event == BLE_GAP_EVT_DISCONNECTED) {
				event_disconnect(p_ble_evt);
			}
			break;
		case STATE_DISC_SERV:
			Serial.debug_print("2");
			//PatternManager.set_current_pattern(0x8);
			if (event == BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP) { //We got services, confirm, then discover characteristics.
				event_confirm_service_data(p_ble_evt);
			} else if (event == BLE_GAP_EVT_DISCONNECTED) {
				event_disconnect(p_ble_evt);
			}
			break;

		case STATE_DISC_CHAR:
			Serial.debug_print("3");
			if (event == BLE_GATTC_EVT_CHAR_DISC_RSP) { //We got characteristics, confirm, then discover CCCD.
				//PatternManager.set_current_pattern(0x9);
				event_confirm_char_data(p_ble_evt);
			} else if (event == BLE_GAP_EVT_DISCONNECTED) {
				event_disconnect(p_ble_evt);
			}
			break;

		case STATE_DISC_DESC:
			Serial.debug_print("4");
			if (event == BLE_GATTC_EVT_DESC_DISC_RSP) { //We got CCCD, confirm, then request authentication.
				//PatternManager.set_current_pattern(0x0a);
				//Note about Authentication. It returns BLE_GAP_EVT_SEC_PARAMS_REQUEST which requires a reply(done in BluetoothLE)
				if (p_ble_evt->evt.gattc_evt.gatt_status
						== BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND
						|| p_ble_evt->evt.gattc_evt.gatt_status
								== BLE_GATT_STATUS_ATTERR_INVALID_HANDLE) {
				} else if (p_ble_evt->evt.gattc_evt.gatt_status) {
				} else {
					if (p_ble_evt->evt.gattc_evt.params.desc_disc_rsp.count
							> 0) {
						descriptor_set(
								&(p_ble_evt->evt.gattc_evt.params.desc_disc_rsp.descs[0]));
					}
					//we missed something with the CCCD, request it again?
					if (m_service.notification_source.handle_cccd
							== INVALID_SERVICE_HANDLE
							|| m_service.data_source.handle_cccd
									== INVALID_SERVICE_HANDLE) {
						descriptor_disc_req_send();
					} else if (is_bonded) {
						configure_cccd();
						set_state_running();
					} else {
						event_connected_req_auth();
						set_state_running();
					}
				}
			} else if (event == BLE_GAP_EVT_DISCONNECTED) {
				event_disconnect(p_ble_evt);
			}
			break;

		case STATE_RUNNING:
			//PatternManager.set_current_pattern(0xb);
			if (event == BLE_GATTC_EVT_HVX) { //notification event
				Serial.debug_print("EVT_HVX");

				event_notify(p_ble_evt);
			} else if (event == BLE_GATTC_EVT_WRITE_RSP) { //return with more notification info?
				if (set_configure_cccd) {
					set_configure_cccd = false;
					reconfigure_ds_cccd();
				}
				Serial.debug_print("EVT_WRITE_RSP");
				//PatternManager.set_current_pattern(7);
			} else if (event == BLE_GAP_EVT_DISCONNECTED) {
				event_disconnect(p_ble_evt);
			} else {
				// Do nothing, event not handled in this state.
			}
			break;

		case STATE_RUNNING_NOT_DISCOVERED:
		default:
			if (event == BLE_GAP_EVT_DISCONNECTED) {
				event_disconnect(p_ble_evt);
			}
			break;
		}
	}
}

void CANCSHandler::set_state_running() {
	Serial.debug_print("go5");
	m_state = STATE_RUNNING;
	store_sys_settings = true;
	//LumenSettings.save_sys_settings();
}

void CANCSHandler::event_notify(ble_evt_t * p_ble_evt) {
	ble_ancs_c_evt_t event;
	ble_gattc_evt_hvx_t * p_notification = &p_ble_evt->evt.gattc_evt.params.hvx;

	if (p_notification->handle == m_service.notification_source.handle_value) {
		BLE_UUID_COPY_INST(event.uuid, m_service.notification_source.uuid);
		event.data.notification.event_id = p_notification->data[0]; /*lint -e{*} suppress Warning 415: possible access out of bond */
		event.data.notification.event_flags = p_notification->data[1]; /*lint -e{*} suppress Warning 415: possible access out of bond */
		event.data.notification.category_id = p_notification->data[2]; /*lint -e{*} suppress Warning 415: possible access out of bond */
		event.data.notification.category_count = p_notification->data[3]; /*lint -e{*} suppress Warning 415: possible access out of bond */
		memcpy(event.data.notification.notification_uid,
				&p_notification->data[4], 4); /*lint -e{*} suppress Warning 415: possible access out of bond */
		event.evt_type = BLE_ANCS_C_EVT_IOS_NOTIFICATION;
		Serial.debug_print("ADDED");
		//m_state.evt_handler(&event);

		if (event.data.notification.event_id
				== BLE_ANCS_EVENT_ID_NOTIFICATION_ADDED) {
			ble_ancs_attr_list_t attr_list[2];
			// attr_list[0].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_TITLE;
			//attr_list[0].attribute_len = 32;
			// attr_list[1].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER;
			// attr_list[1].attribute_len = 32;
			//attr_list[2].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER;
			//attr_list[2].attribute_len = 32;

			ble_gattc_write_params_t info_request;
			info_request.write_op = BLE_GATT_OP_WRITE_REQ;
			info_request.handle = m_service.control_point.handle_value;
			info_request.offset = 0;
			info_request.len = 8;
			last_catagory_id = event.data.notification.category_id;

			uint32_t err_code = ble_ancs_get_notification_attributes(
					event.data.notification.notification_uid, 2, attr_list);
			parse_state = COMMAND_ID;
		} else if (event.data.notification.event_id
				== BLE_ANCS_EVENT_ID_NOTIFICATION_REMOVED) {
			NotificationManager.clear_notification();
		}
	} else if (p_notification->handle == m_service.data_source.handle_value) {
		//BLE_UUID_COPY_INST(event.uuid, m_service.data_source.uuid);
		parse_get_notification_attributes_response(p_notification->data,
				p_notification->len);
		Serial.debug_print("resp");
	} else {
		Serial.debug_print("bad");
		// the notification came from the unknown.
	}
}

uint32_t CANCSHandler::ble_ancs_get_notification_attributes(uint8_t * p_uid,
		uint8_t num_attr, ble_ancs_attr_list_t *p_attr) {
	Serial.debug_print("GETMORE");
	uint8_t i = 0;
	gattc_params.handle = m_service.control_point.handle_value;
	gattc_params.p_value = gattc_value;
	gattc_params.offset = 0;
	gattc_params.write_op = BLE_GATT_OP_WRITE_REQ;

	gattc_value[0] = 0;
	memcpy(&gattc_value[1], p_uid, 4);
	i = 5;

	/*	gattc_value[i++] = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_TITLE;
	 gattc_value[i++] = (uint8_t) 32;
	 gattc_value[i++] = (uint8_t) (32<< 8);
	 */
	gattc_value[i++] = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER;
	gattc_value[i++] = (uint8_t) 32;
	gattc_value[i++] = (uint8_t) 0;

	gattc_params.len = i;

	tx_buffer_process(WRITE_REQ);
	return NRF_SUCCESS;

}

void CANCSHandler::parse_get_notification_attributes_response(
		const uint8_t *data, int len) {
	static uint8_t *ptr;
	static uint8_t *ptr_string;
	static uint16_t current_len;
	static ble_ancs_c_evt_t event;
	int i;

	for (i = 0; i < len; i++) {

		switch (parse_state) {
		case COMMAND_ID:
			event.data.attribute.command_id = data[i];
			parse_state = NOTIFICATION_UID1;
			break;

		case NOTIFICATION_UID1:
			event.data.attribute.notification_uid[0] = data[i];
			parse_state = NOTIFICATION_UID2;
			break;

		case NOTIFICATION_UID2:
			event.data.attribute.notification_uid[1] = data[i];
			parse_state = NOTIFICATION_UID3;
			break;

		case NOTIFICATION_UID3:
			event.data.attribute.notification_uid[2] = data[i];
			parse_state = NOTIFICATION_UID4;
			break;

		case NOTIFICATION_UID4:
			event.data.attribute.notification_uid[3] = data[i];
			parse_state = ATTRIBUTE_ID;
			break;

		case ATTRIBUTE_ID:
			event.data.attribute.attribute_id = data[i];
			parse_state = ATTRIBUTE_LEN1;
			break;

		case ATTRIBUTE_LEN1:
			event.data.attribute.attribute_len = data[i];
			parse_state = ATTRIBUTE_LEN2;
			break;

		case ATTRIBUTE_LEN2:
			event.data.attribute.attribute_len |= (data[i] << 8);
			parse_state = ATTRIBUTE_READY;
			ptr = event.data.attribute.data;
			ptr_string = ptr;
			current_len = 0;
			break;

		case ATTRIBUTE_READY:
			*ptr++ = data[i];
			current_len++;

			if (current_len == event.data.attribute.attribute_len) {
				*ptr++ = 0;
				Serial.debug_print("rdy");
				Serial.store_in_buffer(ptr_string, current_len);
				NotificationManager.convert_ancs_notification(ptr_string,
						current_len, last_catagory_id);
				parse_state = ATTRIBUTE_ID;
			}
			break;
		}
	}
}

void CANCSHandler::configure_cccd() {
	uint32_t err_code;
	Serial.debug_print("cccd");
	gattc_params.handle = m_service.notification_source.handle_cccd;
	gattc_params.len = 2;
	gattc_params.p_value = gattc_value;
	gattc_params.offset = 0;
	gattc_params.write_op = BLE_GATT_OP_WRITE_REQ;
	gattc_value[0] = LSB(0x01);
	gattc_value[1] = MSB(0x00);
	err_code = sd_ble_gattc_write(conn_handle, &gattc_params);
	Serial.debug_print(err_code);

	gattc_params_spare.handle = m_service.data_source.handle_cccd;
	gattc_params_spare.len = 2;
	gattc_params_spare.p_value = gattc_value;
	gattc_params_spare.offset = 0;
	gattc_params_spare.write_op = BLE_GATT_OP_WRITE_REQ;
	set_configure_cccd = true;

}
void CANCSHandler::reconfigure_ds_cccd() {
	gattc_params.handle = m_service.data_source.handle_cccd;
	uint32_t err_code = sd_ble_gattc_write(conn_handle, &gattc_params);
	Serial.debug_print(err_code);
	if (err_code != 0)
		set_configure_cccd = true;
}

void CANCSHandler::event_discover_services() {
	ble_uuid_t ancs_uuid;
	BLE_UUID_BLE_ASSIGN(ancs_uuid, BLE_UUID_APPLE_NOTIFICATION_CENTER_SERVICE);
	ancs_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;
	sd_ble_gattc_primary_services_discover(conn_handle, 1, &ancs_uuid);
	m_state = STATE_DISC_SERV;
	//PatternManager.set_current_pattern(0x7);
}

//pulls the service data from the evt data
void CANCSHandler::event_confirm_service_data(ble_evt_t * p_ble_evt) {
	BLE_UUID_COPY_INST(m_service.service.uuid,
			p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[0].uuid);
	uint32_t err_code = 0;
	if (p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.count > 0) {
		const ble_gattc_service_t * p_service;

		p_service =
				&(p_ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp.services[0]);

		m_service.service.handle_range.start_handle =
				p_service->handle_range.start_handle;
		m_service.service.handle_range.end_handle =
				p_service->handle_range.end_handle;

		characteristic_disc_req_send(&(m_service.service.handle_range));

		if (err_code == NRF_SUCCESS) {
			m_state = STATE_DISC_CHAR;
		}
	}
}

//request characteristics
void CANCSHandler::characteristic_disc_req_send(
		ble_gattc_handle_range_t * p_handle) {
	uint32_t err_code;
	// With valid service, we should discover characteristics.
	err_code = sd_ble_gattc_characteristics_discover(conn_handle, p_handle);

	if (err_code == NRF_SUCCESS) {
		m_state = STATE_DISC_CHAR;
	}
}

//confirm characteristic data
void CANCSHandler::event_confirm_char_data(ble_evt_t * p_ble_evt) {
	uint32_t i;
	const ble_gattc_char_t * p_char_resp = NULL;
	if (p_ble_evt->evt.gattc_evt.gatt_status
			== BLE_GATT_STATUS_ATTERR_ATTRIBUTE_NOT_FOUND
			|| p_ble_evt->evt.gattc_evt.gatt_status
					== BLE_GATT_STATUS_ATTERR_INVALID_HANDLE) {
		if ((m_service.notification_source.handle_value == 0)
				|| (m_service.control_point.handle_value == 0)
				|| (m_service.data_source.handle_value == 0)) {
			// At least one required characteristic is missing on the server side.
		} else {
			descriptor_disc_req_send();
		}
	}
	// Iterate trough the characteristics and save their data if they are ANCS
	for (i = 0; i < p_ble_evt->evt.gattc_evt.params.char_disc_rsp.count; i++) {
		p_char_resp = &(p_ble_evt->evt.gattc_evt.params.char_disc_rsp.chars[i]);
		switch (p_char_resp->uuid.uuid) {
		case BLE_UUID_ANCS_CONTROL_POINT_CHAR:
			BLE_UUID_COPY_INST(m_service.control_point.uuid, p_char_resp->uuid);
			m_service.control_point.properties = p_char_resp->char_props;
			m_service.control_point.handle_decl = p_char_resp->handle_decl;
			m_service.control_point.handle_value = p_char_resp->handle_value;
			m_service.control_point.handle_cccd = INVALID_SERVICE_HANDLE; //invalid handle
			break;

		case BLE_UUID_ANCS_NOTIFICATION_SOURCE_CHAR:
			BLE_UUID_COPY_INST(m_service.control_point.uuid, p_char_resp->uuid);
			m_service.notification_source.properties = p_char_resp->char_props;
			m_service.notification_source.handle_decl =
					p_char_resp->handle_decl;
			m_service.notification_source.handle_value =
					p_char_resp->handle_value;
			m_service.notification_source.handle_cccd = INVALID_SERVICE_HANDLE; //invalid handle
			break;

		case BLE_UUID_ANCS_DATA_SOURCE_CHAR:
			BLE_UUID_COPY_INST(m_service.control_point.uuid, p_char_resp->uuid);
			m_service.data_source.properties = p_char_resp->char_props;
			m_service.data_source.handle_decl = p_char_resp->handle_decl;
			m_service.data_source.handle_value = p_char_resp->handle_value;
			m_service.data_source.handle_cccd = INVALID_SERVICE_HANDLE; //invalid handle
			break;

		default:
			break;
		}
	}
	if (p_char_resp != NULL) {
		// If not all characteristics are received, we do a 2nd/3rd/... round.
		ble_gattc_handle_range_t char_handle;
		char_handle.start_handle = p_char_resp->handle_value + 1;
		char_handle.end_handle = m_service.service.handle_range.end_handle;
		characteristic_disc_req_send(&char_handle);
	} else {
		characteristic_disc_req_send(&(m_service.service.handle_range));
	}
}

void CANCSHandler::descriptor_disc_req_send() {
	ble_gattc_handle_range_t descriptor_handle;
	uint32_t err_code = NRF_SUCCESS;

	// If we have not discovered descriptors for all characteristics,
	// we will discover next descriptor.
	if (m_service.notification_source.handle_cccd == INVALID_SERVICE_HANDLE) { //invalid
		descriptor_handle.start_handle =
				m_service.notification_source.handle_value + 1;
		descriptor_handle.end_handle =
				m_service.notification_source.handle_value + 1;

		err_code = sd_ble_gattc_descriptors_discover(conn_handle,
				&descriptor_handle);
	} else if (m_service.data_source.handle_cccd == INVALID_SERVICE_HANDLE) {
		descriptor_handle.start_handle = m_service.data_source.handle_value + 1;
		descriptor_handle.end_handle = m_service.data_source.handle_value + 1;

		err_code = sd_ble_gattc_descriptors_discover(conn_handle,
				&descriptor_handle);
	}

	if (err_code == NRF_SUCCESS) {
		m_state = STATE_DISC_DESC;
	}
}

void CANCSHandler::descriptor_set(const ble_gattc_desc_t * p_desc_resp) {
	if (m_service.control_point.handle_value == (p_desc_resp->handle - 1)) {
		if (p_desc_resp->uuid.uuid == BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG) {
			m_service.control_point.handle_cccd = p_desc_resp->handle;
		}
	}

	else if (m_service.notification_source.handle_value
			== (p_desc_resp->handle - 1)) {
		if (p_desc_resp->uuid.uuid == BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG) {
			m_service.notification_source.handle_cccd = p_desc_resp->handle;
		}
	} else if (m_service.data_source.handle_value
			== (p_desc_resp->handle - 1)) {
		if (p_desc_resp->uuid.uuid == BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG) {
			m_service.data_source.handle_cccd = p_desc_resp->handle;
		}
	}
}

void CANCSHandler::event_connected_req_auth() {
	set_state_running();
	Nrf51822BluetoothStack::_stack->sec_params.timeout = 30; // seconds
	Nrf51822BluetoothStack::_stack->sec_params.bond = 1;  // perform bonding.
	Nrf51822BluetoothStack::_stack->sec_params.mitm = 0; // man in the middle protection not required.
	Nrf51822BluetoothStack::_stack->sec_params.io_caps = BLE_GAP_IO_CAPS_NONE; // no display capabilities.
	Nrf51822BluetoothStack::_stack->sec_params.oob = 0; // out of band not available.l;l
	Nrf51822BluetoothStack::_stack->sec_params.min_key_size = 7; // min key size
	Nrf51822BluetoothStack::_stack->sec_params.max_key_size = 16; // max key size.
	sd_ble_gap_authenticate(conn_handle,
			&Nrf51822BluetoothStack::_stack->sec_params);
}

void CANCSHandler::event_disconnect(ble_evt_t * p_ble_evt) {
	enable_cccd_check = false;
	uint16_t temp_attr_size;
	sd_ble_gatts_sys_attr_get(conn_handle, ancs_bond_data.central_attr.sys_attr,
			&temp_attr_size);
	ancs_bond_data.central_attr.sys_attr_size = (uint32_t) temp_attr_size;
	conn_handle = BLE_CONN_HANDLE_INVALID;
	if (is_bonded)
		BondManager.save_bond(&ancs_bond_data, false);
	has_bonds = false;
	is_bonded = false;
	is_ios_device = false;
	m_state = STATE_IDLE;
	memset(&m_service, 0, sizeof(apple_service_t));
}

