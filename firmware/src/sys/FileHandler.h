#ifndef FILEHANDLER_H
#define FILEHANDLER_H



#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "app_util.h"
extern "C" {
#include "ble_gap.h"
#include "nrf_soc.h"
#include "lava_bootloader.h"
}

#define BLE_FLASH_EMPTY_MASK    0xFFFFFFFF                          /**< Bit mask that defines an empty address in flash. */
#define BLE_FLASH_PAGE_SIZE     ((uint16_t)NRF_FICR->CODEPAGESIZE)  /**< Size of one flash page. */
#define BLE_FLASH_PAGE_END                                     \
        ((NRF_UICR->BOOTLOADERADDR != BLE_FLASH_EMPTY_MASK)    \
        ? (NRF_UICR->BOOTLOADERADDR / BLE_FLASH_PAGE_SIZE)     \
        : NRF_FICR->CODESIZE)

#define BLE_DATA_END_ADDR      (BLE_FLASH_PAGE_END * BLE_FLASH_PAGE_SIZE)        			/**< End address for persistent data, configurable according to system requirements. */
#define FILE_HEADER_DATA_SIZE 8
#define FILE_HANDLER_MAX_BONDS 4
#define FILE_HANDLER_MAX_NOTIFICATIONS 20
#define FILE_HANDLER_MAX_SYS_SETTINGS 1
#define FILE_HANDLER_MAX_MOOD_SETTINGS 20

#define FILE_HANDLER_HANDLE_LENGTH 6
typedef enum {
	kFileNoOperation = 0,
	kFileWriteOperation,
	kFileReadOperation,
	kFileSwapPageOperation,
	kFileEraseSwapOperation,
	kFileErasePageOperation,
}kFileOperation;

typedef enum {
	kFileNoSave = 0,
	kFileSaveHandle,
	kFileSaveSize,
	kFileSaveData,
	kFileSaveHeader,
	kFileSaveNotifID,
	kFileSaveDone,
}kSaveProcess;

typedef enum {
	kBondStoragePage = 0,
	kNotificationSettingsPage1,
	kNotificationSettingsPage2,
	kNotificationSettingsPage3,
	kNotificationSettingsPage4,
	kSystemSettingsPage,
	kMoodSettingsPage,
	kSwapPage,
	kPageArrayLength,
	kNotificationGenericPage,
} flash_page_t;


typedef struct
{
	uint32_t *						temp_data_location;
	int8_t							lookup_table_position;
	kFileOperation					op_code;
	kSaveProcess					save_progress;
}flash_package_t;

typedef struct
{
	uint8_t					 flash_page_type;
    uint8_t                  handle[FILE_HANDLER_HANDLE_LENGTH];
    uint16_t				 location;
    uint8_t 				 size;

} lookup_table_t;


class CFileHandler {

  private:
	flash_package_t task_queue[25];
	uint8_t queue_pos, last_task;
	uint8_t next_arr_position = 0;
	uint8_t ignore_notifications = 0;
	uint16_t storage_page;
	uint8_t num_bonds = 0;
	bool busy_erasing = false;
	uint32_t temp_size;
	bool trigger_bootloader = false;
	struct handle_t{
		uint32_t temp_lower_handle;
		uint32_t temp_upper_handle;
	} handle_word_aligned;
	uint16_t m_flash_page[kPageArrayLength];
	bool task_queue_is_empty = true;
	lookup_table_t	m_lookup_table[FILE_HANDLER_MAX_BONDS + FILE_HANDLER_MAX_NOTIFICATIONS + FILE_HANDLER_MAX_SYS_SETTINGS + FILE_HANDLER_MAX_MOOD_SETTINGS];
	void handle_evt_error();
	void process_evt();
	int32_t find_highest_data_loc(uint8_t page_type);
	int32_t find_lookup_slot(uint8_t page_type);
	uint32_t find_flash_page();
	void populate_notif_table();
	uint32_t delete_memory(uint8_t page_type);
	void populate_lookup_table();
  public:
    void init();
    bool is_busy();
    void sys_event_handler(uint32_t evt_id);
    bool save_data(uint32_t * ptr_packed_data, uint8_t * ptr_packed_handle, uint32_t data_size, uint8_t page_type);
    bool load_data(uint32_t * ptr_packed_data, uint8_t * ptr_packed_handle, uint8_t page_type);

};


extern CFileHandler FileHandler;

#endif /* FILEHANDLER_H_ */
