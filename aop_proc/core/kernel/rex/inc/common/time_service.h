#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

void time_service_init(void);
uint64 time_service_now(void);
void busywait (uint32 pause_time_us);

#ifdef __cplusplus
}
#endif

#endif // TIME_SERVICE_H

