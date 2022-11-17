#ifndef tux_ota_H
#define tux_ota_H

#include "../main/events/tux_events.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void run_ota_task(void *pvParameter);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif