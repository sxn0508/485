#ifndef UART_H
#define UART_H
#include "stm32f37x.h"
#include "BaseDef.h"

   

#define DRV_BUF_SIZE			300	 //串口缓存大小
#define DATA_BUF_SIZE			300	

#define SEARCH_SOH 0
#define RCV_DATA1  1
#define RCV_DATA2  2






/*缓存结构体类型定义*/
typedef struct ucdrv_buf{
	uint8_t ucBuf[DRV_BUF_SIZE];
	uint16_t WR_Index;
	uint16_t RD_Index;

}UCDRV_BUF;

/*串口驱动缓存结构体声明*/
extern  UCDRV_BUF ucDrv_Buf_Uart1_Rcv;//串口1接收驱动缓存结构体
extern  UCDRV_BUF ucDrv_Buf_Uart1_Snd;//串口1发送驱动缓存结构体

extern  UCDRV_BUF ucDrv_Buf_Uart2_Rcv;//串口2接收驱动缓存结构体
extern  UCDRV_BUF ucDrv_Buf_Uart2_Snd;//串口2发送驱动缓存结构体

extern  UCDRV_BUF ucDrv_Buf_Uart3_Rcv;//串口3接收驱动缓存结构体
extern  UCDRV_BUF ucDrv_Buf_Uart3_Snd;//串口3发送驱动缓存结构体


static void vIRQ_UART_Config(void);   //串口中断初始化

extern uint16_t	ucDrv_Buf_DataLen(const UCDRV_BUF *ucdrv_buf);				//驱动缓存中未读取的字节数
extern void			vUart_Init(void);															//串口初始化
extern bool			blDrv_Buf_IsEmpty(const UCDRV_BUF *ucdrv_buf);					//判断驱动缓存是否为空
extern bool			blDrv_Buf_IsFull(const UCDRV_BUF *ucdrv_buf);					//判断驱动缓存是否满
extern uint16_t	ucDrv_Buf_EmpLen(const UCDRV_BUF *ucdrv_buf);				//驱动缓存中的剩余空间
extern bool			ucDrv_Buf_GetBytes(UCDRV_BUF *ucdrv_buf,uint8_t *ucdata_buf,uint16_t length);//从驱动缓存中读取指定数量的字节到数据缓存数组
extern bool			ucDrv_Buf_PutBytes(uint8_t *ucdata_buf,UCDRV_BUF *ucdrv_buf,uint16_t length);//从驱动缓存中读取指定数量的字节到数据缓存数组

//extern bool blData_Buf_IsEmpty(const UCDATA_BUF *ucdata_buf);
//extern bool blData_Buf_IsFull(const UCDATA_BUF *ucdata_buf);
//extern uint16_t ucData_Buf_DataLen(const UCDATA_BUF *ucdata_buf);
//extern uint16_t ucData_Buf_EmpLen(const UCDATA_BUF *ucdata_buf);

extern void Uart_DataPut(USART_TypeDef* USART_X);  //出口发送

#endif

