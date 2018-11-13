#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string.h>
#include "uart.h"
#include "protocol.h"

typedef enum emprotocol
{
    dlt645,
    dlt698,
    none,
} ProtocolDef;

/*从应用层帧缓存格式化为帧结构*/
extern uint8_t dlt645_CalCS(uint8_t *pframe, uint32_t cspos);
extern void *pGetpFrame(uint8_t *framebuf, uint32_t len);
extern ProtocolDef GetProtocolType(uint8_t *pbuf, uint32_t len);
extern uint32_t dwUartCopy(UartDef *SrcUart, uint8_t *pdata, UartDef *DestUart);
//extern uint8_t dlt645_CalCS(Frame07_645Def *pframe);

#endif
;
