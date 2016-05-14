#include "STC15F2K60S2.h"
#include "sys_uart.h"
#include "string.h"

#define FOSC 11059200L          //系统频率
#define BAUD 9600

#define ENABLE_INTERRUPT_SEND
void sys_uartRxHandler(void);

void sys_uartSetChannel(UINT8 ch)
{
    #define S1_S0   0x40
    #define S1_S1   0x80
    if(ch==0)
    {
        ACC = P_SW1;
		ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0//(P3.0/RxD, P3.1/TxD)
		P_SW1 = ACC;
    }
    else if(ch==1)
    {
        ACC = P_SW1;
		ACC &= ~(S1_S0 | S1_S1);    //S1_S0=1 S1_S1=0
		ACC |= S1_S0;               //(P3.6/RxD_2, P3.7/TxD_2)
		P_SW1 = ACC;
    }
    else if(ch==2)
    {
        ACC = P_SW1;
		ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=1
		ACC |= S1_S1;               //(P1.6/RxD_3, P1.7/TxD_3)
		P_SW1 = ACC;
    }
}

void sys_uartInit(void)
{
//    sys_uartSetChannel(2);
    SCON = 0x50;                    //8位可变波特率
    T2L = (65536 - (FOSC/4/BAUD));  //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR |= 0x14;                   //T2为1T模式, 并启动定时器2
    AUXR |= 0x01;                   //选择定时器2为串口1的波特率发生器
    ES = 1;                         //使能串口1中断
    EA = 1;
}


#define BOUD_OVER_TIME		(5)		//5ms
#define MSG_LEN				(8)
#define RX_BUFF_LEN         (MSG_LEN+2)
UINT8 u8_BoudOverTime = 0;
UINT8 u8_RxIndex = 0;
BOOL b_RxFlag = FALSE;
UINT8 RxBuff[RX_BUFF_LEN];

#ifdef ENABLE_INTERRUPT_SEND
UINT8 *pTxBuff;
UINT8 TxLen = 0, TxCnt = 0;
#else
BOOL b_TxFlag = TRUE;
#endif
void sys_uartInterrupt() interrupt 4
{
    if(RI)
    {
        RI = 0;
        sys_uartRxHandler();
    }
    if(TI)
    {
        TI = 0;
#ifdef ENABLE_INTERRUPT_SEND
        if(++TxCnt != TxLen)
        {
            SBUF = pTxBuff[TxCnt];
        }
#else
        b_TxFlag = TRUE;
#endif
    }
}

void sys_uartSendData(UINT8 *pData, UINT8 len)
{
#ifdef ENABLE_INTERRUPT_SEND
    while(TxLen!=TxCnt);
    TxLen = len;
    TxCnt = 0;
    pTxBuff = pData;
    SBUF = *pData;
#else
    if(len)
    {
        while(len--)
        {
            while(b_TxFlag == FALSE);
            b_TxFlag = FALSE;
            SBUF = *pData++;
        }
    }
#endif
}

void sys_uartSendString(UINT8 *s)
{
    sys_uartSendData(s, strlen(s));
}

UINT8 sys_uartReadData(UINT8 *pData)
{
	UINT8 len = u8_RxIndex;
	memcpy(pData, RxBuff, MSG_LEN);
	memset(RxBuff, 0x00, RX_BUFF_LEN);
	u8_RxIndex = 0;
	u8_BoudOverTime = 0;
	b_RxFlag = FALSE;
	return len;
}

UINT8 RxNum = 0;
void sys_uartRxHandler(void)
{
    if(SBUF == 0x7F)        //0x7F auto download     boud 9600 0x7F at boud 2400 is 0xF8
    {
        RxNum++;
        if(RxNum >= 80)
        {
            IAP_CONTR = 0x60;
        }
    }
    else
    {
        RxNum = 0;
    }
    RxBuff[u8_RxIndex] = SBUF;
    u8_BoudOverTime = BOUD_OVER_TIME;
    if(u8_RxIndex <= MSG_LEN)
    {
        u8_RxIndex++;
    }
}

UINT8 sys_uartOverTime1ms(void)
{
	if(u8_BoudOverTime && b_RxFlag==FALSE)
	{
		u8_BoudOverTime--;
		if(u8_BoudOverTime==0)
		{
			b_RxFlag = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

