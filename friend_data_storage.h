#ifndef FRIEND_DATA_STORAGE_H_
#define FRIEND_DATA_STORAGE_H_
#include <stdint.h>
typedef enum
{
    JWB_FRIEND_TYPE_ID = (10),
		JWB_NUM_OF_FRIENDS_ID
} jewelbot_data_storage_t;
void friend_storage_init(void);
void save_number_of_friends(uint8_t * num_of_friends);
void load_number_of_friends(uint32_t * number_to_load);
#endif
