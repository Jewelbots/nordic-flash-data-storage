#include "SEGGER_RTT.h"
#include "fds.h"
#include "friend_data_storage.h"
#include "app_error.h"
#include <string.h>

#define DATA_STORAGE_INSTANCE_ID        0x0022
#define DATA_STORAGE_TYPE_ID            0x0021
#define SENTINEL_VALUE                  0xEE

//static uint8_t write_flag=0;
//static uint8_t clear_flag=0;
//static uint8_t gc_flag=0;

static void fds_evt_handler(ret_code_t          result,
                            fds_cmd_id_t        cmd,
                            fds_record_id_t     record_id,
                            fds_record_key_t    record_key
                            /*fds_record_t  const * const p_record*/)
{
if (result != 0) {
  SEGGER_RTT_printf(0, "FLASH STORAGE: FAILURE. %u, cmd: %u\n", result, cmd);
}
    switch(cmd)
    {
        case FDS_CMD_INIT:
            SEGGER_RTT_WriteString(0, "FDS_CMD_INIT\n");
            break;

        case FDS_CMD_UPDATE:
            SEGGER_RTT_printf(0, "FDS_CMD_UPDATE: result: %u, instance: %02x, type: %02x\n", result, record_key.instance, record_key.type);
            //update_flag=1;
						//err_code = fds_gc();
            //APP_ERROR_CHECK(err_code);
            break;
        case FDS_CMD_WRITE:
            SEGGER_RTT_printf(0, "FDS_CMD_WRITE: result: %u, instance: %02x, type: %02x\n", result, record_key.instance, record_key.type);
            //write_flag=1;
						//err_code = fds_gc();
            //APP_ERROR_CHECK(err_code);
            break;

        case FDS_CMD_CLEAR:
            SEGGER_RTT_WriteString(0, "FDS_CMD_CLEAR\n");
						//clear_flag=1;
            break;

        case FDS_CMD_CLEAR_INST:
            {
              SEGGER_RTT_WriteString(0, "FDS_CMD_CLEAR_INST\n");
            }
            break;

        case FDS_CMD_GC:
            SEGGER_RTT_WriteString(0, "GC FDS\n");
						//gc_flag=1;
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
	SEGGER_RTT_printf(0, "Added a friend, number of friends: %u\n", friends_list->num_of_friends);
}
void initialize_friends_list_in_flash(friends_list_t * friends_list) {
	fds_record_chunk_t chunk;
	fds_record_desc_t friend_list_descriptor;
  fds_find_token_t tok;

	//write_flag=0;
  bool has_records = false;

  while(fds_find(DATA_STORAGE_TYPE_ID, DATA_STORAGE_INSTANCE_ID, &friend_list_descriptor, &tok) == NRF_SUCCESS) {
    has_records = true;
    SEGGER_RTT_WriteString(0, "Due to the way the code is laid out; we should never get here.\n");
  }
  if (!has_records) {
    fds_record_key_t key;
    key.type = DATA_STORAGE_TYPE_ID;
    key.instance = DATA_STORAGE_INSTANCE_ID;
    uint16_t max_length =  sizeof(friends_list_t);
    uint16_t length_words = max_length / 4;
    chunk.length_words = length_words; //number of 4-byte word chunks of color_friends_t
    uint32_t err_code;
    chunk.p_data = friends_list;
    err_code = fds_write(&friend_list_descriptor, key, 1, &chunk);
    SEGGER_RTT_printf(0, "INITIAL WRITE FRIENDS: max_length: %u, length_words: %u, Error Code was: %u\n", max_length, length_words, err_code);
    APP_ERROR_CHECK(err_code);
		//while(write_flag==0);
  }
}


void save_friends(friends_list_t * friends_list) {
	static fds_record_key_t key;
	static fds_record_chunk_t chunk;
	static fds_record_desc_t friend_list_descriptor;
	static fds_find_token_t tok;
	key.type = DATA_STORAGE_TYPE_ID;
	key.instance = DATA_STORAGE_INSTANCE_ID;
	
	//write_flag=0;
	//clear_flag=0;
	//gc_flag=0;


	chunk.p_data = friends_list;
  SEGGER_RTT_printf(0, "size should be storing for max_length: %u\n", sizeof(friends_list_t));
	uint16_t max_length =  sizeof(friends_list_t);
	uint16_t length_words = max_length / 4;
	chunk.length_words = length_words; //number of 4-byte word chunks of color_friends_t
	uint32_t err_code;
  //err_code = fds_update(&friend_list_descriptor, key, 1, &chunk);
  //SEGGER_RTT_printf(0, "UPDATE FRIENDS: max_length: %u, length_words: %u, Error Code was: %u\n", max_length, length_words, err_code);
  //APP_ERROR_CHECK(err_code);
	while(fds_find(DATA_STORAGE_TYPE_ID, DATA_STORAGE_INSTANCE_ID, &friend_list_descriptor, &tok) == NRF_SUCCESS) {
			err_code = fds_clear(&friend_list_descriptor);
			SEGGER_RTT_WriteString(0, "Clear existing record.\n");
			APP_ERROR_CHECK(err_code);
			//while(clear_flag==0);  //Not sure if this is necessary, not in the nrf52/SDK11 example
	}
	err_code = fds_gc();
  APP_ERROR_CHECK(err_code);
	//while(gc_flag==0);  //Not sure if this is necessary, not in the nrf52/SDK11 example
	SEGGER_RTT_WriteString(0, "Run Garbage Collect.\n");
	err_code = fds_write(&friend_list_descriptor, key, 1, &chunk);
	SEGGER_RTT_printf(0, "WRITING UPDATED FRIENDS: max_length: %u, length_words: %u, Error Code was: %u\n", max_length, length_words, err_code);
	APP_ERROR_CHECK(err_code);
	//while(write_flag==0);
}

void load_friends(friends_list_t * friends_to_load)
{
  static fds_record_desc_t descriptor;
  static fds_find_token_t tok;
  static fds_record_t record;
  while(fds_find(DATA_STORAGE_TYPE_ID, DATA_STORAGE_INSTANCE_ID, &descriptor, &tok) == NRF_SUCCESS) {
    fds_open(&descriptor, &record);
    memcpy(friends_to_load, record.p_data, sizeof(friends_list_t));
    SEGGER_RTT_printf(0, "friends to load, num of friends is: %u\n", friends_to_load->num_of_friends);
    fds_close(&descriptor);
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
