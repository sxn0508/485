#ifndef UART_H
#define UART_H
#include "stm32f37x.h"
#include "BaseDef.h"

   

#define DRV_BUF_SIZE			300	 //���ڻ����С
#define DATA_BUF_SIZE			300	

#define SEARCH_SOH 0
#define RCV_DATA1  1
#define RCV_DATA2  2






/*����ṹ�����Ͷ���*/
typedef struct ucdrv_buf{
	uint8_t ucBuf[DRV_BUF_SIZE];
	uint16_t WR_Index;
	uint16_t RD_Index;

}UCDRV_BUF;

/*������������ṹ������*/
extern  UCDRV_BUF ucDrv_Buf_Uart1_Rcv;//����1������������ṹ��
extern  UCDRV_BUF ucDrv_Buf_Uart1_Snd;//����1������������ṹ��

extern  UCDRV_BUF ucDrv_Buf_Uart2_Rcv;//����2������������ṹ��
extern  UCDRV_BUF ucDrv_Buf_Uart2_Snd;//����2������������ṹ��

extern  UCDRV_BUF ucDrv_Buf_Uart3_Rcv;//����3������������ṹ��
extern  UCDRV_BUF ucDrv_Buf_Uart3_Snd;//����3������������ṹ��


static void vIRQ_UART_Config(void);   //�����жϳ�ʼ��

extern uint16_t	ucDrv_Buf_DataLen(const UCDRV_BUF *ucdrv_buf);				//����������δ��ȡ���ֽ���
extern void			vUart_Init(void);															//���ڳ�ʼ��
extern bool			blDrv_Buf_IsEmpty(const UCDRV_BUF *ucdrv_buf);					//�ж����������Ƿ�Ϊ��
extern bool			blDrv_Buf_IsFull(const UCDRV_BUF *ucdrv_buf);					//�ж����������Ƿ���
extern uint16_t	ucDrv_Buf_EmpLen(const UCDRV_BUF *ucdrv_buf);				//���������е�ʣ��ռ�
extern bool			ucDrv_Buf_GetBytes(UCDRV_BUF *ucdrv_buf,uint8_t *ucdata_buf,uint16_t length);//�����������ж�ȡָ���������ֽڵ����ݻ�������
extern bool			ucDrv_Buf_PutBytes(uint8_t *ucdata_buf,UCDRV_BUF *ucdrv_buf,uint16_t length);//�����������ж�ȡָ���������ֽڵ����ݻ�������

//extern bool blData_Buf_IsEmpty(const UCDATA_BUF *ucdata_buf);
//extern bool blData_Buf_IsFull(const UCDATA_BUF *ucdata_buf);
//extern uint16_t ucData_Buf_DataLen(const UCDATA_BUF *ucdata_buf);
//extern uint16_t ucData_Buf_EmpLen(const UCDATA_BUF *ucdata_buf);

extern void Uart_DataPut(USART_TypeDef* USART_X);  //���ڷ���

#endif

