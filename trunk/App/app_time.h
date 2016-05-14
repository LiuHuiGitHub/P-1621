#ifndef __APP_TIME_H__
#define __APP_TIME_H__

#include "typedef.h"
#include "app_adc.h"

#define TIME_SAVE_SECTOR                2

extern data UINT16 u16_DisplayTime[AD_CHANNEL_NUM];

void app_timeInit(void);
void app_timeClear(UINT8 channel);
void app_timeLoad(void);
void app_timeSaveTime(void);
void app_timeDisplay500ms(void);
void app_timePower1min(void);
void app_timeAddTime(UINT8 channel, UINT16 money);
//void app_timeAddMoney(UINT8 channel, UINT16 money);
UINT16 app_timeRefundMoney(UINT8 channel, UINT16 *money);


#endif
