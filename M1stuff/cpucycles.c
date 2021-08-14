#include <stdint.h>
#include "cpucycles.h"

#if defined(__aarch64__) && defined(__APPLE__)
#include "m1cycles.c"
#endif

