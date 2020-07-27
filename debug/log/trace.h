#ifndef TRACE_H__
#define TRACE_H__

#include "nrf_log_ctrl.h"
#include <inttypes.h>
#include "nrf_delay.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* clang-format off */

#define L_VERBOSE   "0"
#define L_INFO      "1"
#define L_WARNING   "2"
#define L_ERROR     "3"
#define L_FATAL     "4"

#define xstr(s) str(s)
#define str(s)  #s

/* clang-format on */

#ifdef ENABLE_DEBUG_LOG_SUPPORT

#include "app_scheduler.h"
#include "clock.h"

#define LOG_VERBOSE(...)                                       \
    do                                                         \
    {                                                          \
        printf("[%010" PRIu32 ":", (uint32_t)time_get_time()); \
        printf("%u:", app_sched_queue_utilization_get());      \
        printf(L_VERBOSE);                                     \
        printf(":%s:%d] ", __FILE__, __LINE__);                \
        printf(__VA_ARGS__);                                   \
        printf("\r\n");                                        \
    } while (0)

#define LOG_INFO(...)                                          \
    do                                                         \
    {                                                          \
        printf("[%010" PRIu32 ":", (uint32_t)time_get_time()); \
        printf("%u:", app_sched_queue_utilization_get());      \
        printf(L_INFO);                                        \
        printf(":%s:%d] ", __FILE__, __LINE__);                \
        printf(__VA_ARGS__);                                   \
        printf("\r\n");                                        \
    } while (0)

#define LOG_WARN(...)                                          \
    do                                                         \
    {                                                          \
        printf("[%010" PRIu32 ":", (uint32_t)time_get_time()); \
        printf("%u:", app_sched_queue_utilization_get());      \
        printf(L_WARNING);                                     \
        printf(":%s:%d] ", __FILE__, __LINE__);                \
        printf(__VA_ARGS__);                                   \
        printf("\r\n");                                        \
    } while (0)

#define LOG_ERROR(...)                                         \
    do                                                         \
    {                                                          \
        printf("[%010" PRIu32 ":", (uint32_t)time_get_time()); \
        printf("%u:", app_sched_queue_utilization_get());      \
        printf(L_ERROR);                                       \
        printf(":%s:%d] ", __FILE__, __LINE__);                \
        printf(__VA_ARGS__);                                   \
        printf("\r\n");                                        \
    } while (0)

#define LOG_FATAL(...)                                    \
    do                                                    \
    {                                                     \
        printf("[%010" PRIu32 ":", time_get_time());      \
        printf("%u:", app_sched_queue_utilization_get()); \
        printf(L_FATAL);                                  \
        printf(":%s:%d] ", __FILE__, (int)__LINE__);      \
        printf(__VA_ARGS__);                              \
        printf("\r\n");                                   \
    } while (0)

#define LOG_LOG(...)         \
    do                       \
    {                        \
        printf(__VA_ARGS__); \
        printf("\r\n");      \
    } while (0)

#else

#define LOG_LOG(...)
#define LOG_VERBOSE(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)

#endif

#endif  // TRACE_H__
