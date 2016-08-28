#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "app_error.h"
#include "app_timer.h"
#include "fds.h"
#include "fstorage.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_sdm.h"
#include "nrf_soc.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf51_bitfields.h"
#include "softdevice_handler.h"
// application
#include "friend_data_storage.h"
#include "SEGGER_RTT.h"


#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_RC,            \
                                 .rc_ctiv       = 16,                                \
                                 .rc_temp_ctiv  = 2,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM}


#define APP_TIMER_PRESCALER         0                                  /**< Value of the RTC1 PRESCALER register. */

#define SCHED_MAX_EVENT_DATA_SIZE    ((CEIL_DIV(MAX(                                   \
                                                             MAX(BLE_STACK_EVT_MSG_BUF_SIZE,    \
                                                                 ANT_STACK_EVT_STRUCT_SIZE),    \
                                                             SYS_EVT_MSG_BUF_SIZE               \
                                                            ),                                  \
                                                         sizeof(uint32_t))) *                   \
                                               sizeof(uint32_t))
#define SCHED_QUEUE_SIZE             16u

static friends_list_t                   friends_list;

static void sys_evt_dispatch(uint32_t sys_evt)
{
	fs_sys_event_handler(sys_evt);
}
static void ble_stack_init(void)
{
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    
    // Initialize the SoftDevice handler module.	
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

#if defined(S110) || defined(S130) || defined(S310)  || defined(S132)
    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
#if defined(S130) || defined(S310) || defined(S132)
    ble_enable_params.gatts_enable_params.attr_tab_size   = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
#endif
    ble_enable_params.gatts_enable_params.service_changed = 0;
    err_code = softdevice_enable(&ble_enable_params);
    SEGGER_RTT_printf(0, "ble_stack_enable error code is: %u\n", err_code);
    APP_ERROR_CHECK(err_code);
#endif

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    SEGGER_RTT_printf(0, "sys_evt_dispatch error code is: %u\n", err_code);
    APP_ERROR_CHECK(err_code);
}




static void ble_address_to_string_convert(ble_gap_addr_t address, uint8_t * string_buffer)
{
    const int address_length = 6;
    char temp_str[3];
    int i = 0;

    for (i = address_length - 1; i >= 0; --i)
    {
        sprintf(temp_str, "%02X", address.addr[i]);
        strcat((char *)string_buffer, temp_str);
    }
}

static void print_friends() {
   SEGGER_RTT_WriteString(0, "------------- PRINTING FRIENDS -------------\n");
   for (uint8_t i = 0; i <= MAX_NUM_OF_FRIENDS; i++) {
    if (i >= friends_list.num_of_friends) {
      break;
    }
    uint8_t str[50] = {0};
    ble_address_to_string_convert(friends_list.friends[i].address, str);
    SEGGER_RTT_printf(0, "Friends Are: color index: %02x, friend address: 0x%s\n", friends_list.friends[i].color_index, str);
  }
  SEGGER_RTT_WriteString(0, "------------- END FRIENDS -------------\n");
}


int main(void)
{
  ble_stack_init();
	friend_storage_init();
  nrf_delay_ms(1000); //wait because fds is doing stuff.
  SEGGER_RTT_WriteString(0, "After FDS init\n");
	print_friends();
  load_friends(&friends_list);
	SEGGER_RTT_WriteString(0, "After Load Friends\n");
  print_friends();
  
  if (!has_friends(&friends_list)) {
    init_friends_list(&friends_list);
  }
  nrf_delay_ms(5000);
	SEGGER_RTT_WriteString(0, "\n\nAdd, Print, Save Friends\n");
  add_friend(&friends_list);
  print_friends();
	save_friends(&friends_list);

  
	for(;;) {
		nrf_delay_ms(10000);
		SEGGER_RTT_WriteString(0, "\n\nAdd, Print, Save Friends\n");
    add_friend(&friends_list);
		print_friends();
    save_friends(&friends_list);
#ifndef RTT
		ret_code_t err_code = sd_app_evt_wait();
		APP_ERROR_CHECK(err_code);
#endif
	}
}

/**
 * @}
 */
