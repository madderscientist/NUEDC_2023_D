#include "led.h"
#include "sys.h"
// C8T6��C13������LED
void LED_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 // GPIO_Pin_13�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 // IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 // �����趨������ʼ��GPIOC.13

    GPIO_ResetBits(GPIOC,GPIO_Pin_13);						 // PC.13 �����
}
