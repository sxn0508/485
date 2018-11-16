#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string.h>
#include "uart.h"
#include "DLT698.h"

#define TICKS_500MS 6
#define TICKS_1000MS 11
typedef enum emprotocol
{
    dlt645,
    dlt698,
    none,
} ProtocolDef;

/*从应用层帧缓存格式化为帧结构*/
extern uint8_t dlt645_CalCS(uint8_t *pframe, uint32_t cspos);
extern uint8_t *pGetpFrame(uint8_t *pframe, uint32_t dwLen, DLT698_FRAME *p698Frame);
extern ProtocolDef GetProtocolType(uint8_t *pbuf, uint32_t len);
extern uint32_t dwUartCopy(UartDef *SrcUart, uint8_t *pdata, UartDef *DestUart);

#endif
;
