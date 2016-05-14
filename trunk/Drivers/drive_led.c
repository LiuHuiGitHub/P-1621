#include <STC15F2K60S2.H>
#include "drive_led.h"

sbit LED_CS0 = P1 ^ 0;
sbit LED_CS1 = P3 ^ 6;
sbit LED_CS2 = P3 ^ 7;
sbit LED_CS3 = P1 ^ 6;
sbit LED_CS4 = P1 ^ 7;
sbit LED_CS5 = P5 ^ 4;

#define CHANNEL_NUM			2

/*数码管管脚定义
P27~0
g c b f a e d h
*/

//与显示相关变量
//	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	U,	-,	空,	8. ,A, P
code UINT8 ledCoding[] = { 0x7e, 0x60, 0xae, 0xea, 0xf0, 0xda, 0xde, 0x68, 0xfe, 0xfa, 0x76, 0x80, 0x00, 0xff, 0xfc, 0xbc };
data UINT8 u8_ledDisBuff[CHANNEL_NUM * 3] = { 15, 11, 1, 6, 2, 1 }; // P-1621
data UINT8 u8_ledDot = 0;
static data UINT8 u8_ledIndex = 0;//从左到右为0,1,2
static data UINT8 u8_ledReqDisBuff[CHANNEL_NUM * 3];
static data UINT16 u16_ledReqDisTime[CHANNEL_NUM] = { 0, 0 };
static data UINT8 u8_ledReqDot = 0;

void drv_ledInit(void)
{
	P1M1 &= ~0xC1;      //推挽输出
	P1M0 |= 0xC1;
	P2M1 &= ~0xFF;      //推挽输出
	P2M0 |= 0xFF;
	P5M1 &= ~0x10;      //推挽输出
	P5M0 |= 0x10;
	P3M1 &= ~0xC0;      //推挽输出
	P3M0 |= 0xC0;
}

/* LED动态显示 */
void drv_ledHandler1ms(void)
{
	UINT8 buff, channel, place;
	LED_CS0 = 1;
	LED_CS1 = 1;
	LED_CS2 = 1;
	LED_CS3 = 1;
	LED_CS4 = 1;
	LED_CS5 = 1;
	u8_ledIndex++;
	u8_ledIndex %= 6;
	channel = u8_ledIndex / 3;
	if (u16_ledReqDisTime[channel] == 0)
	{
		buff = u8_ledDisBuff[u8_ledIndex];
		place = u8_ledDot;
	}
	else
	{
		u16_ledReqDisTime[channel]--;
		buff = u8_ledReqDisBuff[u8_ledIndex];
		place = u8_ledReqDot;
	}
	P2 = ledCoding[buff];
	if (place & (1 << u8_ledIndex))//show dot
	{
		P2 |= 0X01;
	}
	if (u8_ledIndex == 0)
	{
		LED_CS0 = 0;
	}
	else if (u8_ledIndex == 1)
	{
		LED_CS1 = 0;
	}
	else if (u8_ledIndex == 2)
	{
		LED_CS2 = 0;
	}
	else if (u8_ledIndex == 3)
	{
		LED_CS3 = 0;
	}
	else if (u8_ledIndex == 4)
	{
		LED_CS4 = 0;
	}
	else
	{
		LED_CS5 = 0;
	}
}
//
//void drv_ledShowNumber(UINT16 L_num, UINT16 R_num, UINT8 DotPlace)
//{
//	if (L_num > 999)
//	{
//		L_num = 999;
//	}
//	if (R_num > 999)
//	{
//		R_num = 999;
//	}
//	u8_ledDisBuff[0] = (UINT8)(L_num / 100);
//	u8_ledDisBuff[1] = (UINT8)(L_num / 10 % 10);
//	u8_ledDisBuff[2] = (UINT8)(L_num % 10);
//	u8_ledDisBuff[3] = (UINT8)(R_num / 100);
//	u8_ledDisBuff[4] = (UINT8)(R_num / 10 % 10);
//	u8_ledDisBuff[5] = (UINT8)(R_num % 10);
//	u8_ledDot = DotPlace;
//}

void drv_ledDisplayChannel(UINT8 channel, UINT16 value)
{
    UINT8 place;
	if (channel >= 2)
	{
		return;
	}
    place = channel*3;
	if (value == DISPLAY_NONE)
	{
		u8_ledDisBuff[place] = 12;
		u8_ledDisBuff[place + 1] = 12;
		u8_ledDisBuff[place + 2] = 12;
	}
    else if(value == DISPALY_CH_NULL)
	{
		u8_ledDisBuff[place] = 12;
		u8_ledDisBuff[place + 1] = channel+1;
		u8_ledDisBuff[place + 2] = 12;
	}
    else
    {
        if (value > 999)
        {
            value = 999;
        }
        u8_ledDisBuff[place] = (UINT8)(value / 100);
        u8_ledDisBuff[place + 1] = (UINT8)(value / 10 % 10);
        u8_ledDisBuff[place + 2] = (UINT8)(value % 10);
    }
	u8_ledDot = 0;
}

void drv_ledRequestDisplayChannel0(UINT16 value, UINT16 time, UINT8 dot)
{
	u8_ledReqDisBuff[0] = (UINT8)(value / 100);
	u8_ledReqDisBuff[1] = (UINT8)(value / 10 % 10);
	u8_ledReqDisBuff[2] = (UINT8)(value % 10);
	u16_ledReqDisTime[0] = time;
	u8_ledReqDot &= 0xF8;
	u8_ledReqDot |= dot;
}
void drv_ledRequestDisplayChannel1(UINT16 value, UINT16 time, UINT8 dot)
{
	u8_ledReqDisBuff[3] = (UINT8)(value / 100);
	u8_ledReqDisBuff[4] = (UINT8)(value / 10 % 10);
	u8_ledReqDisBuff[5] = (UINT8)(value % 10);
	u16_ledReqDisTime[1] = time;
	u8_ledReqDot &= 0x07;
	u8_ledReqDot |= dot;
}

