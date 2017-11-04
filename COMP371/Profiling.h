#pragma once

#ifdef DEBUG_BUILD

#include <Brofiler/Brofiler.h>
#define INSTRUMENT_FUNCTION(category,color) BROFILER_CATEGORY(category, color)

#else
#define INSTRUMENT_FUNCTION(category,color) ((void)0)
#endif

