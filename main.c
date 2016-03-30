#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "app_error.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "app_timer_appsh.h"
#include "fds.h"
#include "fstorage.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_soc.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf51_bitfields.h"
#include "softdevice_handler_appsh.h"
// application
#include "friend_data_storage.h"
#include "SEGGER_RTT.h"

#define APP_TIMER_PRESCALER         0                                  /**< Value of the RTC1 PRESCALER register. */

#define SCHED_MAX_EVENT_DATA_SIZE    ((CEIL_DIV(MAX(                                   \
                                                             MAX(BLE_STACK_EVT_MSG_BUF_SIZE,    \
                                                                 ANT_STACK_EVT_STRUCT_SIZE),    \
                                                             SYS_EVT_MSG_BUF_SIZE               \
                                                            ),                                  \
                                                         sizeof(uint32_t))) *                   \
                                               sizeof(uint32_t))
#define SCHED_QUEUE_SIZE             16u

static void scheduler_init(void)
{
	APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

int main(void)
{
	scheduler_init();
	uint8_t num_of_friends = 7;
	friend_storage_init();
	uint8_t num_to_load;
	load_number_of_friends(&num_to_load);
	SEGGER_RTT_printf(0, "number to load was: %u\n", num_to_load);
	save_number_of_friends(&num_of_friends);
	
	for(;;) {
		app_sched_execute();
	}
}

/**
 * @}
 */
