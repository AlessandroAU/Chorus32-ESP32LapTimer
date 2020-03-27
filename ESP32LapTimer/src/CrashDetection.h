#ifndef _CRASHDETECTION_H_
#define _CRASHDETECTION_H_

#define MAX_CRASH_COUNT 5

bool is_crash_mode();
void init_crash_detection();
void restart_esp();
int get_crash_count();

#endif  // _CRASHDETECTION_H_
