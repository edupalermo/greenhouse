#ifndef NTP_TIME_H
#define NTP_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void sntp_set_system_time(uint32_t sec);

#ifdef __cplusplus
}
#endif

#endif // NTP_TIME_H