#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#ifdef __cplusplus
extern "C" {
#endif


void watchdog_add_task();
void watchdog_feed();

#ifdef __cplusplus
}
#endif


#endif // __WATCHDOG_H__
