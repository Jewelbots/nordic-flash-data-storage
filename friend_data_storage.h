#ifndef FRIEND_DATA_STORAGE_H_
#define FRIEND_DATA_STORAGE_H_
#include "ble_gap.h"
#include <stdint.h>
#include <stdbool.h>
static const uint8_t MAX_NUM_OF_FRIENDS = 16;

typedef struct
{
  ble_gap_addr_t address;
  uint8_t color_index;
} friend_t;

#pragma pack(1)
typedef struct
{
  friend_t friends[MAX_NUM_OF_FRIENDS];
  uint8_t num_of_friends;
  uint8_t __padding[3]; //to ensure it's 4-byte aligned: required for fds.h
} friends_list_t;
#pragma pack()
void friend_storage_init(void);
bool has_friends(friends_list_t * friends_list);
void init_friends_list(friends_list_t * friends_list);
void save_friends(friends_list_t * friends_list);
void load_friends(friends_list_t * friends_list_to_load);
void add_friend(friends_list_t * friends_list);
#endif
