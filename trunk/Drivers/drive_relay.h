#ifndef __DEIVE_RELAY_H__
#define __DEIVE_RELAY_H__

#include "typedef.h"

#define ALL_RELAY       2

void drv_relayInit(void);
void drv_relayOpen(UINT8 ch);
void drv_relayClose(UINT8 ch);

#endif
