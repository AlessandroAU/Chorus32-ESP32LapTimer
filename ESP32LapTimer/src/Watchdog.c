#include "Watchdog.h"

#include <stdint.h>
#include <soc/timer_group_struct.h>
#include <soc/timer_group_reg.h>
#include <esp_task_wdt.h>

void watchdog_add_task() {
	esp_task_wdt_add(NULL);
}

void watchdog_feed() {
	TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
	TIMERG0.wdt_feed = 1;
	TIMERG0.wdt_wprotect = 0;
}
