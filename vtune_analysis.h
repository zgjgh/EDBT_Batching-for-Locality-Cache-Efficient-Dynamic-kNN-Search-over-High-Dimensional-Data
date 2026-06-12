// vtune_analysis.h
#pragma once

#ifdef ENABLE_VTUNE
#include <ittnotify>

static __itt_domain* cache_domain = __itt_domain_create("Distance_Calculation");
static __itt_string_handle* single_dist = __itt_string_handle_create("Single_Distance");
static __itt_string_handle* batch_dist = __itt_string_handle_create("Batch_Distance");

#define VTUNE_START_SINGLE() __itt_task_begin(cache_domain, __itt_null, __itt_null, single_dist)
#define VTUNE_END_SINGLE() __itt_task_end(cache_domain)

#define VTUNE_START_BATCH() __itt_task_begin(cache_domain, __itt_null, __itt_null, batch_dist)
#define VTUNE_END_BATCH() __itt_task_end(cache_domain)
#else
    #define VTUNE_START_SINGLE()
    #define VTUNE_END_SINGLE()
    #define VTUNE_START_BATCH()
    #define VTUNE_END_BATCH()
#endif