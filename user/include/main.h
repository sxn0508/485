#ifndef _MAIN_H
#define _MAIN_H

#include "stm32f37x.h"

/***************
 *默认功能：修改电压上限值，电压值不回时替代回帧
 *可选功能1：修改电流短路、断路、反向事件，通过屏蔽电表运行状态字相应位实现
 *可选功能2：修改各相电流差值，最小值不低于最大值的80%（赣州要求）
 *可选功能3：修改电流最大值，变压器一次侧电流不越限
 *可选功能4：修改表码值
 *可选功能5：红外设置参数功能
 * ****************/
//#define ENABLE_ESTATUS //电表状态字修改功能
//#define ENABLE_CURRENT_DEVIATION			//电流各相差值修改功能
//#define ENABLE_CURRENT_LIMIT				//电流最大值修改功能
#define ENABLE_ENERGY_MODIFY //修改正向有功
#define ENABLE_INFR          //红外设置参数功能

/*变压器容量，决定三相电流的最大值*/
//#define TRANSFORMER_CAPACITY_50KVA
//#define TRANSFORMER_CAPACITY_100KVA
#define TRANSFORMER_CAPACITY_200KVA
//#define TRANSFORMER_CAPACITY_400KVA

static void VoltageTimeOutHandle(void);
static void RsvFrameHandle(uint8_t *pucBuffer);
static void vNVIC_IRQ_Config(void);
//static void vVoltageHandle(uint8_t * pucBuffer);

#endif
