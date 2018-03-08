#ifndef _DTC_SLEEP_H_
#define _DTC_SLEEP_H_

#include <stdio.h>
#include <time.h>
#include <errno.h>

void dtc_sleep(struct timespec *ts_old, struct timespec *ts_wait);

#endif 
