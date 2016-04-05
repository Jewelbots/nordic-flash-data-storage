#ifndef FRIEND_DATA_STORAGE_H_
#define FRIEND_DATA_STORAGE_H_
#include <stdint.h>

void friend_storage_init(void);
void save_number_of_friends(uint32_t * num_of_friends);
void load_number_of_friends(uint32_t * number_to_load);
#endif
