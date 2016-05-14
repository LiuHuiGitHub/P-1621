#ifndef __DRIVE_LED_H__
#define __DRIVE_LED_H__

#include "typedef.h"

#define DISPLAY_NONE        0xFFFF
#define DISPALY_CH_NULL     DISPLAY_NONE-1

extern data UINT8 u8_ledDisBuff[6];
extern data UINT8 u8_ledDot;

void drv_ledInit(void);
void drv_ledHandler1ms(void);
void drv_ledShowNumber(UINT16 L_num, UINT16 R_num, UINT8 DotPlace);
void drv_ledDisplayChannel(UINT8 channel, UINT16 value);

void drv_ledRequestDisplayChannel0(UINT16 value, UINT16 time, UINT8 dot);
void drv_ledRequestDisplayChannel1(UINT16 value, UINT16 time, UINT8 dot);

#endif
