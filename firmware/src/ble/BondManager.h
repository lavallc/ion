#ifndef BONDMANAGER_H
#define BONDMANAGER_H



#include <stdint.h>
#include <string.h>
#include "app_util.h"
#include "FileHandler.h"
extern "C" {
#include "ble_gap.h"
}

#define kDeviceTypeApple 		0x01
#define kDeviceTypeAndroid		0x02
#define kDeviceTypeIonode 		0x03
#define CCCD_SIZE                    6                                                                   /**< Number of bytes needed for storing the state of one CCCD. */
#define CRC_SIZE                     2                                                                   /**< Size of CRC in sys_attribute data. */
#define SYS_ATTR_BUFFER_MAX_LEN      (((2 + 1) * CCCD_SIZE) + CRC_SIZE)            /**< Size of sys_attribute data. */ //2 was BLE_BONDMNGR_CCCD_COUNT

typedef struct
{
    uint8_t  sys_attr[SYS_ATTR_BUFFER_MAX_LEN];                              /**< Central sys_attribute data. */
    uint32_t sys_attr_size;                                                  /**< Central sys_attribute data's size (NOTE: Size is 32 bits just to make struct size dividable by 4). */
} central_sys_attr_t;

typedef struct
{
	int32_t                        central_handle;                           /**< Central's handle (NOTE: Size is 32 bits just to make struct size divisable by 4). */
	ble_gap_evt_auth_status_t      auth_status;                              /**< Central authentication data. */
	ble_gap_evt_sec_info_request_t central_id_info;                          /**< Central identification info. */
	ble_gap_addr_t				   central_addr;                             /**< Central's address. */
	central_sys_attr_t			   central_attr;
} bond_data_t;



class CBondManager {

  private:
	uint8_t device_type = 0;
    uint8_t mac_addr[6];
    uint8_t mac_len = 0;
	bond_data_t bond_to_store;
	bool bond_loaded = false;
  public:
    bool set_device_type(uint8_t device, uint8_t mac_address[], uint8_t mac_length);
    bool is_bonded();
    bool is_inited();
    bool ensure_bond(uint8_t *p_device_type);
    bool save_bond(bond_data_t * new_bond, bool is_static_addr);
    bool load_bond(bond_data_t * saved_bond, bool is_static_addr);

};


extern CBondManager BondManager;

#endif /* BONDMANAGER_H_ */
