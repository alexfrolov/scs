#ifndef RW_H_GUARD
#define RW_H_GUARD

#ifdef __cplusplus
extern "C" {
#endif

#include "glbopts.h"
#include "scs.h"

void SCS(write_data)(const ScsData *d, const ScsCone *k);

#ifdef __cplusplus
}
#endif
#endif
