#include "SEGGER_RTT.h"
#include "fds.h"
#include "friend_data_storage.h"
#include "app_error.h"
#include <string.h>

#define DATA_STORAGE_INSTANCE_ID        0x0022
#define DATA_STORAGE_TYPE_ID            0x0021
#define SENTINEL_VALUE                  0xEE
static fds_record_desc_t friend_list_descriptor;

static void fds_evt_handler(fds_evt_t const * const p_fds_evt)
{
if (p_fds_evt->result != 0) {
  SEGGER_RTT_printf(0, "FLASH STORAGE: FAILURE. %u, cmd: %u\n", p_fds_evt->result, p_fds_evt->id);
}

    switch(p_fds_evt->id)
    {
        case FDS_EVT_INIT:
            SEGGER_RTT_WriteString(0, "FDS_CMD_INIT\n");
            break;

        case FDS_EVT_UPDATE:
					SEGGER_RTT_printf(0, "FDS_EVT_UPDATE: result: %u, record_key: %02x, record_id: %02x, file_id: %02x\n", p_fds_evt->result, p_fds_evt->write.record_key, p_fds_evt->write.record_id, p_fds_evt->write.file_id);
            //err_code = fds_gc();
            //APP_ERROR_CHECK(err_code);
            break;
        case FDS_EVT_WRITE:
            SEGGER_RTT_printf(0, "FDS_EVT_WRITE: result: %u, record_key: %02x, record_id: %02x, file_id: %02x\n", p_fds_evt->result, p_fds_evt->write.record_key, p_fds_evt->write.record_id, p_fds_evt->write.file_id);
            //err_code = fds_gc();
            //APP_ERROR_CHECK(err_code);
            break;

        case FDS_EVT_DEL_RECORD:
            SEGGER_RTT_WriteString(0, "FDS_EVT_DEL_RECORD\n");
            break;

        case FDS_EVT_DEL_FILE:
            {
              SEGGER_RTT_WriteString(0, "FDS_EVT_DEL_FILE\n");
            }
            break;

        case FDS_EVT_GC:
            SEGGER_RTT_WriteString(0, "GC FDS\n");
            break;

        default:
            break;
    }
}
void add_friend(friends_list_t * friends_list) {
  friend_t friend_to_add = {
    .address = { .addr_type = BLE_GAP_ADDR_TYPE_PUBLIC, .addr = {0,1,2,3,4,5}},
    .color_index = 1                 /**< See @ref . */
  };
  uint8_t current_friends = friends_list->num_of_friends;
  memcpy(&(friends_list->friends[friends_list->num_of_friends]), &friend_to_add, sizeof(friend_t));
  friends_list->num_of_friends = current_friends + 1;
}
void initialize_friends_list_in_flash(friends_list_t * friends_list) {
	static fds_record_chunk_t chunk;
	//fds_record_desc_t friend_list_descriptor;
  fds_find_token_t tok;

  bool has_records = false;

  while(fds_record_find(DATA_STORAGE_TYPE_ID, DATA_STORAGE_INSTANCE_ID, &friend_list_descriptor, &tok) == NRF_SUCCESS) {
    has_records = true;
    SEGGER_RTT_WriteString(0, "Due to the way the code is laid out; we should never get here.\n");
  }
  if (!has_records) {
    fds_record_t record;
    record.file_id = DATA_STORAGE_TYPE_ID;
    record.key = DATA_STORAGE_INSTANCE_ID;

    uint16_t max_length =  sizeof(friends_list_t);
    uint16_t length_words = max_length / 4;
    chunk.length_words = length_words; //number of 4-byte word chunks of color_friends_t
    uint32_t err_code;
    chunk.p_data = friends_list;
    record.data.p_chunks = &chunk;
    record.data.num_chunks = 1;
    err_code = fds_record_write(&friend_list_descriptor, &record);
    SEGGER_RTT_printf(0, "INITIAL WRITE FRIENDS: max_length: %u, length_words: %u, Error Code was: %u\n", max_length, length_words, err_code);
    APP_ERROR_CHECK(err_code);
  }
}


void save_friends(friends_list_t * friends_list) {
	static fds_record_chunk_t chunk;

	fds_record_t record;
  record.file_id = DATA_STORAGE_TYPE_ID;
  record.key = DATA_STORAGE_INSTANCE_ID;

	chunk.p_data = friends_list;
  SEGGER_RTT_printf(0, "size should be storing for max_length: %u\n", sizeof(friends_list_t));
	uint16_t max_length =  sizeof(friends_list_t);
	uint16_t length_words = max_length / 4;
	chunk.length_words = length_words; //number of 4-byte word chunks of color_friends_t
	uint32_t err_code;
  record.data.p_chunks = &chunk;
  record.data.num_chunks = 1;
  err_code = fds_record_update(&friend_list_descriptor, &record);
  SEGGER_RTT_printf(0, "UPDATE FRIENDS: max_length: %u, length_words: %u, Error Code was: %u\n", max_length, length_words, err_code);
  APP_ERROR_CHECK(err_code);
}

void load_friends(friends_list_t * friends_to_load)
{
  static fds_find_token_t tok;
  static fds_flash_record_t record;
  while(fds_record_find(DATA_STORAGE_TYPE_ID, DATA_STORAGE_INSTANCE_ID, &friend_list_descriptor, &tok) == NRF_SUCCESS) {
    fds_record_open(&friend_list_descriptor, &record);
    memcpy(friends_to_load, record.p_data, sizeof(friends_list_t));
    SEGGER_RTT_printf(0, "friends to load, num of friends is: %u\n", friends_to_load->num_of_friends);
    fds_record_close(&friend_list_descriptor);
    SEGGER_RTT_WriteString(0, "Found friends list. Loading!\n");
    #warning "This operation is happening twice when it shouldn't be (we should only find one record)."
  }
}

bool has_friends(friends_list_t * friends_list) {
  return (friends_list->num_of_friends > 0);
}
void init_friends_list(friends_list_t * friends_list) {
  if (!has_friends(friends_list)) {
    SEGGER_RTT_WriteString(0, "Initializing friends in flash\n");
    initialize_friends_list_in_flash(friends_list);
  }
}
void friend_storage_init(void) {
	uint32_t err_code;
	err_code = fds_register(fds_evt_handler);
  SEGGER_RTT_printf(0, "friend storage event handler registered %u\n", err_code);
	err_code = fds_init();
  SEGGER_RTT_printf(0, "friend storage init'd %u\n", err_code);
	APP_ERROR_CHECK(err_code);
}
