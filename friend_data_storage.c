#include <string.h>
#include "app_error.h"
#include "friend_data_storage.h"
#include "fds.h"
#include "nrf_error.h"
#include "SEGGER_RTT.h"

uint32_t my_string;
static void fds_evt_handler(ret_code_t          result,
                            fds_cmd_id_t        cmd,
                            fds_record_id_t     record_id,
                            fds_record_key_t    record_key
                            /*fds_record_t  const * const p_record*/)
{
    switch(cmd)
    {
        case FDS_CMD_INIT:
						SEGGER_RTT_WriteString(0, "FDS_CMD_INIT\n");	
            break;

        case FDS_CMD_UPDATE:
        case FDS_CMD_WRITE:
            
						SEGGER_RTT_printf(0, "result of write was: %u\n", result); 
						SEGGER_RTT_WriteString(0, "FDS_CMD_WRITE\n");	
            break;

        case FDS_CMD_CLEAR: 
            break;

        case FDS_CMD_CLEAR_INST:
            {
							SEGGER_RTT_WriteString(0, "FDS_CMD_CLEAR\n");
            }
            break;

        case FDS_CMD_GC:
						SEGGER_RTT_WriteString(0, "GC FDS\n");
            break;

        default:
            break;
    }
}
void save_number_of_friends(uint8_t * num_of_friends) {
	fds_record_key_t key;
	fds_record_chunk_t chunk;
	fds_record_desc_t num_of_friends_descriptor;
	key.type = JWB_NUM_OF_FRIENDS_ID;
	key.instance = 1;
	SEGGER_RTT_printf(0, "saving number of friends; value is: %u\n", *num_of_friends);
	chunk.p_data = &num_of_friends;
	chunk.length_words = 1;
	uint32_t err_code = fds_update(&num_of_friends_descriptor, key, 1, &chunk);
	SEGGER_RTT_printf(0, "attempted to write number of friends. Error Code was: %u\n", err_code);
	APP_ERROR_CHECK(err_code);
}
void load_number_of_friends(uint8_t * number_to_load) {
	fds_record_desc_t descriptor;
	fds_find_token_t tok;
	fds_record_t record;
	SEGGER_RTT_printf(0, "number to load coming in %u\n", *number_to_load);
	SEGGER_RTT_WriteString(0, "trying to load number of friends\n");
	while (fds_find(JWB_NUM_OF_FRIENDS_ID, 1, &descriptor, &tok) == NRF_SUCCESS) {
		fds_open(&descriptor, &record);
		SEGGER_RTT_WriteString(0, "able to read record.\n");
		SEGGER_RTT_printf(0, "record.p_data is: %u\n", *(uint8_t *)record.p_data);
		memcpy((void *)&number_to_load, record.p_data, sizeof(uint8_t));
		SEGGER_RTT_printf(0, "number of friends is: %u\n", *number_to_load);
		fds_close(&descriptor);
	}
	SEGGER_RTT_WriteString(0, "Finished trying to load number of friends.\n");
	
}
void friend_storage_init(void) {
	uint32_t err_code;
	APP_ERROR_CHECK(err_code);
	err_code = fds_register(fds_evt_handler);
	err_code = fds_init();
	APP_ERROR_CHECK(err_code);	
}
