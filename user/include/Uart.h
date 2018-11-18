#ifndef UART_H
#define UART_H
#include "stm32f37x.h"
#include "BaseDef.h"

#define DRV_BUF_SIZE 1000 //串口缓存大小

#define DATA_BUF_SIZE 1000

#define SEARCH_SOH 0
#define RCV_DATA1 1
#define RCV_DATA2 2

struct ucbuf
{
    uint8_t data[DRV_BUF_SIZE];
    uint32_t rd;
    uint32_t wr;
};

typedef struct Uart
{
    USART_TypeDef *handler;
    struct ucbuf *pRsvbuf;
    struct ucbuf *pSndbuf;
} UartDef;

//extern void vUart_Init(void);                                                                  //串口初始化
extern bool blDrv_Buf_IsEmpty(const struct ucbuf *ucdrv_buf);                                  //判断驱动缓存是否为空
extern bool blDrv_Buf_IsFull(const struct ucbuf *ucdrv_buf);                                   //判断驱动缓存是否满
extern bool ucDrv_Buf_GetBytes(struct ucbuf *ucdrv_buf, uint8_t *ucdata_buf, uint16_t length); //从驱动缓存中读取指定数量的字节到数据缓存数组
extern bool ucDrv_Buf_PutBytes(uint8_t *ucdata_buf, struct ucbuf *ucdrv_buf, uint16_t length); //从驱动缓存中读取指定数量的字节到数据缓存数组
extern void Uart_DataPut(USART_TypeDef *USART_X);                                              //出口发送
extern void vUart_EnableBaudrate(UartDef *puart, uint32_t baudrate);
//extern void vUart_ChangeBaudrate(UartDef *puart, uint32_t baudrate);

extern int32_t Uart_OnceWrite(UartDef *puart, const uint8_t *pbuf, uint32_t count, uint32_t delay);
extern void Uart_Write(UartDef *puart, const uint8_t *pbuf, uint32_t count);
extern uint32_t Uart_IdleRead(UartDef *puart, uint8_t *buf, uint32_t count, uint32_t tickCount);
extern uint32_t Uart_OnceRead(UartDef *puart, uint8_t *buf, uint32_t count, uint32_t tickCount);
extern uint32_t Uart_Read(UartDef *puart, uint8_t *buf, uint32_t count);
extern void Uart_idleReadEnable(UartDef *puart);
extern void Uart_idleReadDisable(UartDef *puart);
extern uint32_t uwBuf_EmpLen(const struct ucbuf *buf);
extern uint32_t uwBuf_UnReadLen(const struct ucbuf *buf);
extern void vBuf_Clear(struct ucbuf *buf);
extern bool blBufchr(struct ucbuf *uc_buf, uint8_t chr, uint16_t ln);
extern bool blBufcmp(struct ucbuf *uc_buf, char *ptchr, uint8_t ln);
#endif
