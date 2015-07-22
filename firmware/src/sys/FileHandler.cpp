
#include "FileHandler.h"
#include "Lumen.h"

#include "Serial.h"
#include "LumenSettings.h"
CFileHandler FileHandler ;

/* Changes to be done to this File Handler:
 *
 * Although this is currently abstracted from the datatype it will be storing,
 * it assumes all data packages are the same size. So, to find the next package it takes the last
 * package pointer and adds the last package size. If the last package gets smaller then it
 * will no longer find the next package because there is extra room in flash between them.
 * Also a problem if the last package gets bigger.
 *
 * Options:
 *
 * Have different Storage sections denoted by generic types. BondManager Class is assigned storage type A.
 * Then the file Manager puts all requests from the BondManager in the section for A's.
 * ~Still abstracted data, but classes using it need to use same sized data.
 *
 * Create a more in depth system to track size changes and move data as needed.
 * ~More robust but significantly harder to do.
 *
 * Setup defined data chunks (i.e. 1/4 flash block) and all packages get their own chunk
 * ~Easy implementation, but uses extra space and need to know what size to make a chunk.
 */

void CFileHandler::init() {
	m_flash_page[kBondStoragePage] = 0;
	m_flash_page[kNotificationSettingsPage1] = 0;
	m_flash_page[kNotificationSettingsPage2] = 0;
	m_flash_page[kNotificationSettingsPage3] = 0;
	m_flash_page[kNotificationSettingsPage4] = 0;
	m_flash_page[kSystemSettingsPage] = 0;
	m_flash_page[kMoodSettingsPage] = 0;
	m_flash_page[kSwapPage] = 0;
	find_flash_page();
	populate_lookup_table();

}

uint32_t CFileHandler::find_flash_page() {
	//Read page headers to find what type of pages they are
	uint32_t header;
	for (int i = BLE_FLASH_PAGE_END-10; i < BLE_FLASH_PAGE_END-1; i++) {
		header = *(uint32_t *)(BLE_FLASH_PAGE_SIZE * i);
		if ((header & 0xFFF00FFF == 0xFFF00FFF) && (header != BLE_FLASH_EMPTY_MASK)) {
			header = (header & 0x000FF000) >> 12;
			switch (header) {
				case kBondStoragePage:
					m_flash_page[kBondStoragePage] = i;
					break;
				case kNotificationSettingsPage1:
					m_flash_page[kNotificationSettingsPage1] = i;
					break;
				case kNotificationSettingsPage2:
					m_flash_page[kNotificationSettingsPage2] = i;
					break;
				case kNotificationSettingsPage3:
					m_flash_page[kNotificationSettingsPage3] = i;
					break;
				case kNotificationSettingsPage4:
					m_flash_page[kNotificationSettingsPage4] = i;
					break;
				case kSystemSettingsPage:
					m_flash_page[kSystemSettingsPage] = i;
					break;
				case kMoodSettingsPage:
					m_flash_page[kMoodSettingsPage] = i;
					break;
				case kSwapPage:
					//There should never be a swap page header
					break;
			}
		}
	}

	//If the page type has not been assigned a page number, find a number to assign it.
	bool page_exists;
	for (int i = BLE_FLASH_PAGE_END-10; i < BLE_FLASH_PAGE_END-1; i++) {
		page_exists = false;
		for (int j = 0; j < kPageArrayLength; j++) {
			if (m_flash_page[j] == i) {
				page_exists = true;
				break;
			}
		}
		if (!page_exists) {
			for (int j = 0; j < kPageArrayLength; j++) {
				if (m_flash_page[j] == 0) {
					m_flash_page[j] = i;
					if (j != kSwapPage) {
						flash_package_t flash_task;
						flash_task.op_code = kFileWriteOperation;
						flash_task.lookup_table_position = j;
						flash_task.save_progress = kFileSaveHeader;
						memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;
						if (task_queue_is_empty) {
							process_evt();
							task_queue_is_empty = false;
						}
					}
					break;
				}
			}
		}
	}
}

void CFileHandler::populate_notif_table() {
	//Get all Notifications
	uint8_t notif_page = 0;
	uint8_t unused_page = 0;
	uint8_t empty_mask[6];
	empty_mask[0] = 0xFF;
	empty_mask[1] = 0xFF;
	empty_mask[2] = 0xFF;
	empty_mask[3] = 0xFF;
	empty_mask[4] = 0xFF;
	empty_mask[5] = 0xFF;
	for (int i = kNotificationSettingsPage1; i <= kNotificationSettingsPage4; i++) {
		uint8_t * temp_handle = (uint8_t *)(m_flash_page[i] * BLE_FLASH_PAGE_SIZE + sizeof(uint32_t));
		if (memcmp(temp_handle,m_lookup_table[0].handle,FILE_HANDLER_HANDLE_LENGTH) == 0) {
			notif_page = i;
			break;
		}
		else if (memcmp(temp_handle,empty_mask,FILE_HANDLER_HANDLE_LENGTH) == 0) {
			unused_page = i;
		}
	}
	Serial.debug_print(notif_page);
	if (notif_page > 0) {
		Serial.debug_print("found");
		m_lookup_table[0].flash_page_type = notif_page;
		uint32_t header_value;
		uint16_t location = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
		uint32_t flash_addr = m_flash_page[notif_page] * BLE_FLASH_PAGE_SIZE;
		for (int i = 1; i <= FILE_HANDLER_MAX_NOTIFICATIONS; i++) {
			header_value = *(uint32_t *)(location + flash_addr);
			Serial.debug_print("N");
			if (header_value != BLE_FLASH_EMPTY_MASK){
				Serial.debug_print(location);
				Serial.debug_print(notif_page);
				Serial.debug_print(header_value);
				m_lookup_table[i].flash_page_type = notif_page;
				m_lookup_table[i].location = location;
				m_lookup_table[i].handle[0] = (uint8_t)(header_value & 0x000000FF);
				m_lookup_table[i].handle[1] = 0;//(uint8_t)((header_value & 0x0000FF00) >> 8);
				m_lookup_table[i].handle[2] = 0;//(uint8_t)((header_value & 0x00FF0000) >> 16);
				m_lookup_table[i].handle[3] = 0;//(uint8_t)((header_value & 0xFF000000) >> 24);
				m_lookup_table[i].handle[4] = 0;
				m_lookup_table[i].handle[5] = 0;
				m_lookup_table[i].size = *(uint8_t *)(location + flash_addr + sizeof(uint32_t) + sizeof(uint32_t));
				location += m_lookup_table[i].size + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
			}
			else {
				m_lookup_table[i].flash_page_type = kNotificationGenericPage;
				m_lookup_table[i].location = 0;
				memset(m_lookup_table[i].handle, 0, FILE_HANDLER_HANDLE_LENGTH);
			}
		}
	}
	else {
		Serial.debug_print("notfound");

		if (unused_page == 0) {
			Serial.debug_print("zero?");
			//we've got a problem.. it shouldn't get into making a new notification page if all the notification pages are full.
		}
		else {
			for (int i = 1; i <= FILE_HANDLER_MAX_NOTIFICATIONS; i++) {
				m_lookup_table[i].flash_page_type = kNotificationGenericPage;
				m_lookup_table[i].location = 0;
				memset(m_lookup_table[i].handle, 0, FILE_HANDLER_HANDLE_LENGTH);
			}
			m_lookup_table[0].flash_page_type = unused_page;
		}
	}
}

void CFileHandler::populate_lookup_table() {
	m_lookup_table[0].location = 0;
	m_lookup_table[0].size = sizeof(uint32_t);
	next_arr_position = 1;
	uint32_t header_value = 0;
	uint16_t location = m_lookup_table[0].size;
	next_arr_position += FILE_HANDLER_MAX_NOTIFICATIONS;

	for (int i = 1; i < 1 + FILE_HANDLER_MAX_NOTIFICATIONS; i++) {
		m_lookup_table[i].flash_page_type = kNotificationGenericPage;
	}
	//Get all Bonds
	uint32_t flash_addr = m_flash_page[kBondStoragePage] * BLE_FLASH_PAGE_SIZE;
	uint16_t start = next_arr_position;
	for (int i= start; i < start + FILE_HANDLER_MAX_BONDS; i++) {
		header_value = *(uint32_t *)(location + flash_addr);
		if (header_value == BLE_FLASH_EMPTY_MASK)
			break;
		m_lookup_table[i].flash_page_type = kBondStoragePage;
		m_lookup_table[i].location = location;
		m_lookup_table[i].handle[0] = (uint8_t)(header_value & 0x000000FF);
		m_lookup_table[i].handle[1] = (uint8_t)((header_value & 0x0000FF00) >> 8);
		m_lookup_table[i].handle[2] = (uint8_t)((header_value & 0x00FF0000) >> 16);
		m_lookup_table[i].handle[3] = (uint8_t)((header_value & 0xFF000000) >> 24);
		header_value = *(uint32_t *)(location + flash_addr + sizeof(uint32_t));

		m_lookup_table[i].handle[4] = (uint8_t)(header_value & 0x000000FF);
		m_lookup_table[i].handle[5] = (uint8_t)((header_value & 0x0000FF00) >> 8);
		m_lookup_table[i].size = *(uint16_t *)(location + flash_addr + sizeof(uint32_t) + sizeof(uint32_t));
		location += m_lookup_table[i].size + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
		next_arr_position++;
	}

	//Get all System settings
	location = m_lookup_table[0].size;
	flash_addr = m_flash_page[kSystemSettingsPage] * BLE_FLASH_PAGE_SIZE;
	start = next_arr_position;
	for (int i = start; i < start + FILE_HANDLER_MAX_SYS_SETTINGS; i++) {
		header_value = *(uint32_t *)(location + flash_addr);
		if (header_value == BLE_FLASH_EMPTY_MASK)
			break;
		m_lookup_table[i].flash_page_type = kSystemSettingsPage;
		m_lookup_table[i].location = location;
		m_lookup_table[i].handle[0] = (uint8_t)(header_value & 0x000000FF);
		m_lookup_table[i].handle[1] = (uint8_t)((header_value & 0x0000FF00) >> 8);
		m_lookup_table[i].handle[2] = (uint8_t)((header_value & 0x00FF0000) >> 16);
		m_lookup_table[i].handle[3] = (uint8_t)((header_value & 0xFF000000) >> 24);
		m_lookup_table[i].handle[4] = 0;
		m_lookup_table[i].handle[5] = 0;
		m_lookup_table[i].size = *(uint16_t *)(location + flash_addr + sizeof(uint32_t) + sizeof(uint32_t));
		location += m_lookup_table[i].size + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
		next_arr_position++;
	}

	//Gets all Mood settings
	location = m_lookup_table[0].size;
	flash_addr = m_flash_page[kMoodSettingsPage] * BLE_FLASH_PAGE_SIZE;
	start = next_arr_position;
	for (int i = start; i < start + FILE_HANDLER_MAX_MOOD_SETTINGS; i++) {
		header_value = *(uint32_t *)(location + flash_addr);
		if (header_value == BLE_FLASH_EMPTY_MASK)
			break;
		m_lookup_table[i].flash_page_type = kMoodSettingsPage;
		m_lookup_table[i].location = location;
		m_lookup_table[i].handle[0] = (uint8_t)(header_value & 0x000000FF);
		m_lookup_table[i].handle[1] = (uint8_t)((header_value & 0x0000FF00) >> 8);
		m_lookup_table[i].handle[2] = (uint8_t)((header_value & 0x00FF0000) >> 16);
		m_lookup_table[i].handle[3] = (uint8_t)((header_value & 0xFF000000) >> 24);
		m_lookup_table[i].handle[4] = 0;
		m_lookup_table[i].handle[5] = 0;
		m_lookup_table[i].size = *(uint16_t *)(location + flash_addr + sizeof(uint32_t) + sizeof(uint32_t));
		location += m_lookup_table[i].size + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
		next_arr_position++;
	}

}

void CFileHandler::sys_event_handler(uint32_t evt_id) {
	if (evt_id == NRF_EVT_FLASH_OPERATION_SUCCESS) {
		nrf_gpio_pin_clear(kSunBoardDigPin1);
		if (ignore_notifications == 2){
			ignore_notifications = 0;
			populate_notif_table();
			Serial.debug_print("doITnow");
		}
		Serial.debug_print("SUC");
		this->process_evt();
	}
	else if (evt_id == NRF_EVT_FLASH_OPERATION_ERROR) {
		nrf_gpio_pin_clear(kSunBoardDigPin1);
		Serial.debug_print("FAIL");
		this->handle_evt_error();
	}
}

void CFileHandler::handle_evt_error() {

}
bool CFileHandler::is_busy() {
	return busy_erasing;
}

void CFileHandler::process_evt() {
	uint32_t err_code;
	flash_package_t flash_task;
	if (last_task != queue_pos) {
		nrf_gpio_pin_set(kSunBoardDigPin1);
		task_queue_is_empty = false;
		memcpy(&flash_task, &task_queue[queue_pos], sizeof(flash_package_t));
		switch(flash_task.op_code) {
			case kFileSwapPageOperation : {
				Serial.debug_print("SWAP");
				uint16_t temp_page_val = m_flash_page[m_lookup_table[flash_task.lookup_table_position].flash_page_type];
				m_flash_page[m_lookup_table[flash_task.lookup_table_position].flash_page_type] = m_flash_page[kSwapPage];	//Update the flash page addresses
				m_flash_page[kSwapPage] = temp_page_val;
				//Write the new header data
				temp_size = ((uint32_t)m_lookup_table[flash_task.lookup_table_position].flash_page_type << 12) | 0xFFF00FFF;
				sd_flash_write((uint32_t *)(m_flash_page[m_lookup_table[flash_task.lookup_table_position].flash_page_type] * BLE_FLASH_PAGE_SIZE),
								(uint32_t*)&temp_size,
								sizeof(m_lookup_table[0].handle) / sizeof(uint32_t));
				queue_pos++; if(queue_pos > 24) queue_pos = 0;
				break;
			}
			case kFileWriteOperation: {
				if (flash_task.save_progress == kFileSaveHandle) {
					Serial.debug_print("H");

					handle_word_aligned.temp_upper_handle = (uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[5];
					handle_word_aligned.temp_upper_handle =	(handle_word_aligned.temp_upper_handle << 8) | ((uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[4]);
					handle_word_aligned.temp_lower_handle = (uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[3];
					handle_word_aligned.temp_lower_handle = (handle_word_aligned.temp_lower_handle << 8) | ((uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[2]);
					handle_word_aligned.temp_lower_handle = (handle_word_aligned.temp_lower_handle << 8) | ((uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[1]);
					handle_word_aligned.temp_lower_handle = (handle_word_aligned.temp_lower_handle << 8) | ((uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[0]);
					uint32_t error = sd_flash_write((uint32_t *)(m_lookup_table[flash_task.lookup_table_position].location + m_flash_page[m_lookup_table[flash_task.lookup_table_position].flash_page_type] * BLE_FLASH_PAGE_SIZE),
							(uint32_t*)&handle_word_aligned,
							sizeof(handle_t) / sizeof(uint32_t));
					flash_task.save_progress = kFileSaveSize;
					Serial.debug_print(error);
					memcpy(&task_queue[queue_pos], &flash_task, sizeof(flash_task));
				}
				else if (flash_task.save_progress == kFileSaveSize) {
					Serial.debug_print("S");
					temp_size = m_lookup_table[flash_task.lookup_table_position].size;
					uint32_t error = sd_flash_write((uint32_t *)(m_lookup_table[flash_task.lookup_table_position].location + m_flash_page[m_lookup_table[flash_task.lookup_table_position].flash_page_type] * BLE_FLASH_PAGE_SIZE + sizeof(uint32_t) + sizeof(uint32_t)),
							(uint32_t*)&temp_size,
							sizeof(temp_size) / sizeof(uint32_t));
					flash_task.save_progress = kFileSaveData;
					Serial.debug_print(error);
					memcpy(&task_queue[queue_pos], &flash_task, sizeof(flash_task));
				}
				else if (flash_task.save_progress == kFileSaveData) {
					Serial.debug_print("D");
					//Serial.debug_print(m_lookup_table[flash_task.lookup_table_position].location + m_flash_page[m_lookup_table[flash_task.lookup_table_position].flash_page_type] * BLE_FLASH_PAGE_SIZE + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
					uint32_t error = sd_flash_write((uint32_t *)(m_lookup_table[flash_task.lookup_table_position].location + m_flash_page[m_lookup_table[flash_task.lookup_table_position].flash_page_type] * BLE_FLASH_PAGE_SIZE + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t)),
							flash_task.temp_data_location,
							m_lookup_table[flash_task.lookup_table_position].size / sizeof(uint32_t));
					Serial.debug_print(m_lookup_table[flash_task.lookup_table_position].location + m_flash_page[m_lookup_table[flash_task.lookup_table_position].flash_page_type] * BLE_FLASH_PAGE_SIZE + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
					queue_pos++; if(queue_pos > 24) queue_pos = 0;
				}
				else if (flash_task.save_progress == kFileSaveHeader) { //Lookuptable position MUST point to a spot in the flash page array.
					Serial.debug_print("HEADER");
					temp_size = ((uint32_t)flash_task.lookup_table_position << 12) | 0xFFF00FFF;
					uint32_t error = sd_flash_write((uint32_t *)(m_flash_page[flash_task.lookup_table_position] * BLE_FLASH_PAGE_SIZE),
							(uint32_t*)&temp_size,
							sizeof(temp_size) / sizeof(uint32_t));
					Serial.debug_print(error);
					queue_pos++; if(queue_pos > 24) queue_pos = 0;
					if (ignore_notifications == 1)
						ignore_notifications = 2;
				}
				else if (flash_task.save_progress == kFileSaveNotifID) {
					Serial.debug_print("NOTIF");
					handle_word_aligned.temp_upper_handle = (uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[5];
					handle_word_aligned.temp_upper_handle =	(handle_word_aligned.temp_upper_handle << 8) | ((uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[4]);
					handle_word_aligned.temp_lower_handle = (uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[3];
					handle_word_aligned.temp_lower_handle = (handle_word_aligned.temp_lower_handle << 8) | ((uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[2]);
					handle_word_aligned.temp_lower_handle = (handle_word_aligned.temp_lower_handle << 8) | ((uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[1]);
					handle_word_aligned.temp_lower_handle = (handle_word_aligned.temp_lower_handle << 8) | ((uint32_t)m_lookup_table[flash_task.lookup_table_position].handle[0]);
					uint32_t error = sd_flash_write((uint32_t *)(m_flash_page[m_lookup_table[flash_task.lookup_table_position].flash_page_type] * BLE_FLASH_PAGE_SIZE + sizeof(uint32_t)),
							(uint32_t*)&handle_word_aligned,
							sizeof(handle_t) / sizeof(uint32_t));
					Serial.debug_print(error);
					queue_pos++; if(queue_pos > 24) queue_pos = 0;

				}
				break;
			}
			case kFileReadOperation:
				//Do nothing here because we handled the request when it came in.
				break;
			case kFileEraseSwapOperation:
				Serial.debug_print("ESWAP");
				err_code = sd_flash_page_erase(m_flash_page[kSwapPage]);
				queue_pos++; if(queue_pos > 24) queue_pos = 0;
				break;
			case kFileErasePageOperation: //Lookuptable position MUST point to a spot in the flash page array.
				Serial.debug_print("EPAGE");
				err_code = sd_flash_page_erase(m_flash_page[flash_task.lookup_table_position]);
				queue_pos++; if(queue_pos > 24) queue_pos = 0;
				break;
			default:
				Serial.debug_print("wtf!?");
		}
	}
	else {
		busy_erasing = false;
		task_queue_is_empty = true;
		queue_pos = 0;
		last_task = 0;
	}
}

int32_t CFileHandler::find_highest_data_loc(uint8_t page_type) {
	uint32_t highest_data_slot = 0;
	for(int i = 1; i < next_arr_position; i++) {
		if ((m_lookup_table[i].location > m_lookup_table[highest_data_slot].location) && (m_lookup_table[i].flash_page_type == page_type) && (page_type != kNotificationGenericPage)) {
			highest_data_slot = i;
		}
		else if ((page_type == kNotificationGenericPage) && (m_lookup_table[i].flash_page_type == m_lookup_table[0].flash_page_type) && (m_lookup_table[i].location > m_lookup_table[highest_data_slot].location))
			highest_data_slot = i;
	}
	return highest_data_slot;

}

int32_t CFileHandler::find_lookup_slot(uint8_t page_type) {
	for(int i = 1; i < next_arr_position; i++) {
		if ((m_lookup_table[i].location == 0) && (m_lookup_table[i].flash_page_type == page_type)) {
			return i;
		}
	}
	return next_arr_position;

}

bool CFileHandler::save_data(uint32_t * ptr_packed_data, uint8_t * ptr_packed_handle, uint32_t data_size, uint8_t page_type) {

	Serial.debug_print("ign_not");
	Serial.debug_print(ignore_notifications);
	if (page_type == kBondStoragePage) {
		memcpy(m_lookup_table[0].handle, ptr_packed_handle,FILE_HANDLER_HANDLE_LENGTH);
		if (ignore_notifications == 0)
			populate_notif_table();
	}
	flash_package_t flash_task;
	flash_task.op_code = kFileWriteOperation;
	for(int i = 1; i < next_arr_position; i++) {		//Go through the save slots looking for a match
		//If we find one, its time to do a page swap since we can't overwrite information in flash.
		if (memcmp(ptr_packed_handle,m_lookup_table[i].handle,FILE_HANDLER_HANDLE_LENGTH) == 0) {
			if (page_type == m_lookup_table[i].flash_page_type) {
				uint8_t tasks_to_make = 0;
				for(int j = 1; j < next_arr_position; j++) {	//Grab every piece of data that isn't the changed one and create a task to write it to the new page.
					if ((j != i) && (m_lookup_table[j].flash_page_type == page_type)) {
						tasks_to_make++;
					}
				}
				tasks_to_make += 3;
				if (tasks_to_make + last_task > 25)
					return false;

				flash_task.op_code = kFileSwapPageOperation;	//Set task to reassign page numbers and move page header data
				flash_task.lookup_table_position = i;
				flash_task.save_progress = kFileNoSave;
				memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;

				for(int j = 1; j < next_arr_position; j++) {	//Grab every piece of data that isn't the changed one and create a task to write it to the new page.
					if ((j != i) && (m_lookup_table[j].flash_page_type == page_type)) {
						flash_task.op_code = kFileWriteOperation;
						flash_task.lookup_table_position = j;
						flash_task.temp_data_location = (uint32_t *)(m_flash_page[page_type] * BLE_FLASH_PAGE_SIZE + m_lookup_table[j].location + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
						flash_task.save_progress = kFileSaveHandle;
						memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;
					}
				}
				flash_task.op_code = kFileWriteOperation;	//Now we can write the new data to the new flash page
				flash_task.lookup_table_position = i;
				flash_task.temp_data_location = ptr_packed_data;
				flash_task.save_progress = kFileSaveHandle;
				memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;

				flash_task.op_code = kFileEraseSwapOperation;	//Finally, erase the old page of data to become swap page.
				flash_task.save_progress = kFileNoSave;
				memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;

				LumenSettings.save_bond_offset(m_lookup_table[i].location);

				busy_erasing = true;
				if (task_queue_is_empty)
					process_evt();
				return true;
			}
		//We need to check for notification stuff. Means there is a bit different setup to do in determining the flash page.
			else if ((page_type == kNotificationGenericPage) && (i <= FILE_HANDLER_MAX_NOTIFICATIONS)) {

				uint8_t tasks_to_make = 0;
				for(int j = 1; j < next_arr_position; j++) {	//Grab every piece of data that isn't the changed one and create a task to write it to the new page.
					//If it isn't the one we are changing && it is within the notification area of the lookup table && there is actually data there
					if ((j != i) && (j <= FILE_HANDLER_MAX_NOTIFICATIONS) && (m_lookup_table[j].flash_page_type == m_lookup_table[0].flash_page_type)) {
						tasks_to_make++;
					}
				}
				tasks_to_make += 4;
				if (tasks_to_make + last_task > 25)
					return false;

				flash_task.op_code = kFileSwapPageOperation;	//Set task to reassign page numbers and move page header data
				flash_task.lookup_table_position = i;
				flash_task.save_progress = kFileNoSave;
				memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;

				flash_task.lookup_table_position = 0;
				flash_task.op_code = kFileWriteOperation;
				flash_task.save_progress = kFileSaveNotifID;
				memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;

				for(int j = 1; j < next_arr_position; j++) {	//Grab every piece of data that isn't the changed one and create a task to write it to the new page.
					//If it isn't the one we are changing && it is within the notification area of the lookup table && there is actually data there
					if ((j != i) && (j <= FILE_HANDLER_MAX_NOTIFICATIONS) && (m_lookup_table[j].flash_page_type == m_lookup_table[0].flash_page_type)) {
						flash_task.op_code = kFileWriteOperation;
						flash_task.lookup_table_position = j;
						flash_task.temp_data_location = (uint32_t *)(m_flash_page[m_lookup_table[j].flash_page_type] * BLE_FLASH_PAGE_SIZE + m_lookup_table[j].location + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
						flash_task.save_progress = kFileSaveHandle;
						memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;
					}
				}
				//m_lookup_table[i].flash_page_type = m_lookup_table[0].flash_page_type;
				//m_lookup_table[i].size = data_size;
				flash_task.op_code = kFileWriteOperation;	//Now we can write the new data to the new flash page
				flash_task.lookup_table_position = i;
				flash_task.temp_data_location = ptr_packed_data;
				flash_task.save_progress = kFileSaveHandle;
				memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;
				Serial.debug_print("pos");
				Serial.debug_print(i);
				flash_task.op_code = kFileEraseSwapOperation;	//Finally, erase the old page of data to become swap page.
				flash_task.save_progress = kFileNoSave;
				memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;

				busy_erasing = true;
				if (task_queue_is_empty) {
					Serial.debug_print("NOW");
					process_evt();
				}
				return true;
			}
		}
	}
	//We didn't find a place already made for this. Make one and find flash location for it.
	if (next_arr_position < FILE_HANDLER_MAX_BONDS + FILE_HANDLER_MAX_NOTIFICATIONS + FILE_HANDLER_MAX_SYS_SETTINGS + FILE_HANDLER_MAX_MOOD_SETTINGS) {
		if (last_task + 1 > 25)
			return false;
		if (page_type == kBondStoragePage)
				num_bonds++;
		uint8_t free_loc;
		if (num_bonds > FILE_HANDLER_MAX_BONDS) {
			if (last_task + 6 > 25)
				return false;
			free_loc = delete_memory(page_type);
			memcpy(m_lookup_table[free_loc].handle, ptr_packed_handle,FILE_HANDLER_HANDLE_LENGTH);
			LumenSettings.clear_oldest_bond_offset();
			memcpy(m_lookup_table[0].handle, ptr_packed_handle,FILE_HANDLER_HANDLE_LENGTH);
			flash_task.op_code = kFileErasePageOperation;	//Set task to erase notification page.
			flash_task.lookup_table_position = m_lookup_table[0].flash_page_type; //If this bond turns out to have never had a notification then this logic will break.
			flash_task.save_progress = kFileNoSave;
			memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;

			flash_task.op_code = kFileWriteOperation;
			flash_task.save_progress = kFileSaveHeader;
			memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;

			flash_task.lookup_table_position = 0;
			flash_task.op_code = kFileWriteOperation;
			flash_task.save_progress = kFileSaveNotifID;
			memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;

			ignore_notifications = 1;

			busy_erasing = true;
			if (task_queue_is_empty) {
				Serial.debug_print("NOW");
				process_evt();
			}

			return save_data(ptr_packed_data, ptr_packed_handle, data_size, page_type);
		}
		else {
			free_loc = find_lookup_slot(page_type);
			memcpy(m_lookup_table[free_loc].handle, ptr_packed_handle,FILE_HANDLER_HANDLE_LENGTH);
			m_lookup_table[free_loc].size = data_size;
			if (page_type == kNotificationGenericPage) {
				if (free_loc == 1) {
					flash_task.lookup_table_position = 0;
					flash_task.save_progress = kFileSaveNotifID;
					memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;
				}
				m_lookup_table[free_loc].flash_page_type = m_lookup_table[0].flash_page_type;
			}
			else
				m_lookup_table[free_loc].flash_page_type = page_type;


			//Todo check for max notifications
			int32_t highest_loc = find_highest_data_loc(page_type);
			if (highest_loc == 0) {
				if (page_type == kNotificationGenericPage)
					m_lookup_table[free_loc].location = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
				else
					m_lookup_table[free_loc].location = sizeof(uint32_t);
			}
			else
				m_lookup_table[free_loc].location =  m_lookup_table[highest_loc].location + m_lookup_table[highest_loc].size + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
			Serial.debug_print("new");
			Serial.debug_print(free_loc);
			if (page_type == kBondStoragePage)
				LumenSettings.save_bond_offset(m_lookup_table[free_loc].location);
			flash_task.lookup_table_position = free_loc;
			flash_task.temp_data_location = ptr_packed_data;
			flash_task.save_progress = kFileSaveHandle;
			if (next_arr_position == free_loc)
				next_arr_position++;
			memcpy(&task_queue[last_task],&flash_task,sizeof(flash_package_t)); last_task++;
			if (task_queue_is_empty)
				process_evt();
			return true;
		}
	}
	else {
		//no room for more bonds
		//will probably erase oldest bond for now
		return false;
	}
}


bool CFileHandler::load_data(uint32_t * ptr_packed_data, uint8_t * ptr_packed_handle, uint8_t page_type) {
	//Found that bond in storage, give it pointer location
/*	uint32_t * temp_handle = (uint32_t *)(m_flash_page[m_lookup_table[1].flash_page_type] * BLE_FLASH_PAGE_SIZE);
	Serial.debug_print("N");
	Serial.debug_print(m_lookup_table[1].flash_page_type);
	Serial.debug_print(temp_handle[0]);
	Serial.debug_print(temp_handle[1]);
	Serial.debug_print(temp_handle[2]);
	Serial.debug_print(temp_handle[3]);
	Serial.debug_print(temp_handle[4]);
*/
	for(int i = 1; i < next_arr_position; i++) {
		if ((memcmp(ptr_packed_handle,m_lookup_table[i].handle,FILE_HANDLER_HANDLE_LENGTH) == 0) && (page_type == m_lookup_table[i].flash_page_type) && (page_type != kNotificationGenericPage)) {
			*ptr_packed_data = (m_flash_page[m_lookup_table[i].flash_page_type] * BLE_FLASH_PAGE_SIZE + m_lookup_table[i].location + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
			//Serial.debug_print("P");
			//Serial.debug_print((m_flash_page[m_lookup_table[i].flash_page_type] * BLE_FLASH_PAGE_SIZE + m_lookup_table[i].location + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t)));
			//Serial.debug_print(i);
			if (page_type == kBondStoragePage) {
				LumenSettings.save_bond_offset(m_lookup_table[i].location);
				memcpy(m_lookup_table[0].handle, ptr_packed_handle,FILE_HANDLER_HANDLE_LENGTH);
				populate_notif_table();
/*				uint32_t temp;
				Serial.debug_print("M");
				Serial.debug_print(ptr_packed_handle[0]);//m_lookup_table[1].flash_page_type);
				Serial.debug_print(ptr_packed_handle[1]);//m_lookup_table[1].location);
				Serial.debug_print(ptr_packed_handle[2]);//m_lookup_table[1].size);
				Serial.debug_print(ptr_packed_handle[3]);
*/			}
			return true;
		}
		else if ((memcmp(ptr_packed_handle,m_lookup_table[i].handle,FILE_HANDLER_HANDLE_LENGTH) == 0) && (page_type == kNotificationGenericPage) && (i <= FILE_HANDLER_MAX_NOTIFICATIONS)) {
			*ptr_packed_data = (m_flash_page[m_lookup_table[i].flash_page_type] * BLE_FLASH_PAGE_SIZE + m_lookup_table[i].location + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));
/*			Serial.debug_print("L");
			Serial.debug_print(m_lookup_table[1].flash_page_type);
			Serial.debug_print(m_lookup_table[1].location);
			Serial.debug_print(m_lookup_table[1].size);
*/			return true;
		}
	}
	//No bond found
	return false;
}

//This needs to be fixed!
uint32_t CFileHandler::delete_memory(uint8_t page_type) {
	uint16_t bond_offset;
	if (page_type == kBondStoragePage) {
		bond_offset = LumenSettings.get_oldest_bond_offset();
		for (int i = FILE_HANDLER_MAX_NOTIFICATIONS + 1; i < next_arr_position; i++) {
				if (m_lookup_table[i].flash_page_type == kBondStoragePage && (m_lookup_table[i].location == bond_offset || bond_offset == 0 || bond_offset == 0xFFFF)) {
					memcpy(m_lookup_table[0].handle, m_lookup_table[i].handle, FILE_HANDLER_HANDLE_LENGTH);
					populate_notif_table();
					num_bonds--;
					//this is a bad way to do this... really we should be handling the actual bond deletion here, but I'm gonna pass that job it up to the thing that called this.
					//Though, I did load the bond and notifications that needs to be deleted and returned what lookup slot the bond is in.
					return i;
				}
			}
	}
	else {
		for (int i = 1; i < next_arr_position; i++) {
			if (m_lookup_table[i].flash_page_type == page_type) {
				m_lookup_table[i].handle[0] = 0;
				m_lookup_table[i].handle[1] = 0;
				m_lookup_table[i].handle[2] = 0;
				m_lookup_table[i].handle[3] = 0;
				m_lookup_table[i].handle[4] = 0;
				m_lookup_table[i].handle[5] = 0;
				return i;
			}
		}
	}
	return 0;	//an error occurred. delete shouldn't have been called if there was nothing to delete.
}
