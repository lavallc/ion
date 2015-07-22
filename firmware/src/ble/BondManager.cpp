
#include "BondManager.h"
#include "Serial.h"
#include "BluetoothManager.h"
CBondManager BondManager;

bool CBondManager::save_bond(bond_data_t * new_bond, bool is_static_addr) {
	//Might create some sort of packing process?
	uint8_t bond_handle[6];
	memcpy((uint8_t *)&bond_to_store, (uint8_t*)new_bond,sizeof(bond_data_t));
	bool success = true;
	if (is_static_addr)
		success = FileHandler.save_data((uint32_t *)&bond_to_store, (uint8_t *)bond_to_store.central_addr.addr, sizeof(bond_to_store), kBondStoragePage);
	else {
		bond_handle[0] = (uint8_t)(bond_to_store.central_handle & 0x000000FF);
		bond_handle[1] = (uint8_t)((bond_to_store.central_handle & 0x0000FF00) >> 8);
		bond_handle[2] = (uint8_t)((bond_to_store.central_handle & 0x00FF0000) >> 16);
		bond_handle[3] = (uint8_t)((bond_to_store.central_handle & 0xFF000000) >> 24);
		bond_handle[4] = 0;
		bond_handle[5] = 0;
		success = FileHandler.save_data((uint32_t *)&bond_to_store, bond_handle, sizeof(bond_to_store), kBondStoragePage);
	}
	if (success) {
		Serial.debug_print("SBOND");
		bond_loaded = true;
	}
	else {
		Serial.debug_print("!SBOND");
		bond_loaded = false;
	}
}

//This is setup to take connection parameters and search for the matching bond.
//The hope is that this process is fast enough if the pairing device has sent a sec_info_req and we need to return keys.
bool CBondManager::load_bond(bond_data_t * saved_bond, bool is_static_addr) {
	bool success;
	uint32_t bond_pointer;
	uint8_t bond_handle[6];
	if (is_static_addr) {
		success = FileHandler.load_data(&bond_pointer, saved_bond->central_addr.addr, kBondStoragePage);
	}
	else {
		bond_handle[0] = (uint8_t)(saved_bond->central_handle & 0x000000FF);
		bond_handle[1] = (uint8_t)((saved_bond->central_handle & 0x0000FF00) >> 8);
		bond_handle[2] = (uint8_t)((saved_bond->central_handle & 0x00FF0000) >> 16);
		bond_handle[3] = (uint8_t)((saved_bond->central_handle & 0xFF000000) >> 24);
		bond_handle[4] = 0;
		bond_handle[5] = 0;
		success = FileHandler.load_data(&bond_pointer, bond_handle, kBondStoragePage);
	}
	if (success) {
		//Might create some sort of unpacking process?
		memcpy((uint8_t *)saved_bond, (uint8_t*)bond_pointer,sizeof(bond_data_t));
		bond_loaded = true;
		Serial.debug_print("BOND");
		return true;
	}
	else {
		//If we don't have bonds, then the next layer should handle creating bonds.
		Serial.debug_print("!BOND");
		bond_loaded = false;
		return false;
	}
}
bool CBondManager::set_device_type(uint8_t device, uint8_t mac_address[], uint8_t mac_length) {
	// hold onto our init data for later
	device_type = device;
	mac_len = mac_length;
	if (mac_length == 6) {
		memcpy(mac_addr, mac_address, 6);
	}

	if (device_type == kDeviceTypeApple) {
		Serial.debug_print("apple");
		return bond_loaded;
	}
	else if (device_type == kDeviceTypeAndroid){
		Serial.debug_print("android");
		bond_data_t temp_bond_data;
		// if we're passed the mac address, use it as the identifier
		// otherwise pull it from the BT stack (unreliable for Android 5+)
		if (mac_len != 6)
			memcpy(temp_bond_data.central_addr.addr,Nrf51822BluetoothStack::_stack->_peer_addr.addr,6);
		else
			memcpy(temp_bond_data.central_addr.addr,mac_addr,6);
		return load_bond(&temp_bond_data,true);
	}
	else if (device_type == kDeviceTypeIonode) {
		Serial.debug_print("ionode");
		bond_data_t temp_bond_data;
		memcpy(temp_bond_data.central_addr.addr,Nrf51822BluetoothStack::_stack->_peer_addr.addr,6);
		return load_bond(&temp_bond_data,true);
	}
	else
	{
			Serial.debug_print("bad");
		bond_loaded = false;
	}
}
bool CBondManager::ensure_bond(uint8_t *p_device_type) {
	//Todo find if this device is actually the bond we checked
	bond_data_t temp_bond_data;
	*p_device_type = device_type;
	if (!bond_loaded) {
		if (device_type == kDeviceTypeAndroid) {
			if (mac_len != 6)
				memcpy(temp_bond_data.central_addr.addr,Nrf51822BluetoothStack::_stack->_peer_addr.addr,6);
			else
				memcpy(temp_bond_data.central_addr.addr,mac_addr,6);
			return save_bond(&temp_bond_data,true);
		}
		else if (device_type == kDeviceTypeIonode) {
			memcpy(temp_bond_data.central_addr.addr,Nrf51822BluetoothStack::_stack->_peer_addr.addr,6);
			return save_bond(&temp_bond_data,true);
		}
		else if (device_type == kDeviceTypeApple) {

			ANCSHandler.enable_ANCS(true);
			return false;
		}
		else {
			return false;
			//we really shouldn't be here. Bad device_type;
		}
	}
	else
		return true;

	return false;
}

bool CBondManager::is_bonded() {
	bond_data_t temp_bond_data;
	if (!bond_loaded) {
		if (device_type == kDeviceTypeAndroid) {
			if (mac_len != 6)
				memcpy(temp_bond_data.central_addr.addr,Nrf51822BluetoothStack::_stack->_peer_addr.addr,6);
			else
				memcpy(temp_bond_data.central_addr.addr,mac_addr,6);
			return load_bond(&temp_bond_data,true);
		}
		else if (device_type == device_type == kDeviceTypeIonode) {
			memcpy(temp_bond_data.central_addr.addr,Nrf51822BluetoothStack::_stack->_peer_addr.addr,6);
			return load_bond(&temp_bond_data,true);
		}
		else
			return false;
	}
	else {
		return true;
	}
	return false;
}

bool CBondManager::is_inited() {
	if (device_type == kDeviceTypeAndroid || device_type == kDeviceTypeIonode || device_type == kDeviceTypeApple)
		return true;
	else
		return false;
}



