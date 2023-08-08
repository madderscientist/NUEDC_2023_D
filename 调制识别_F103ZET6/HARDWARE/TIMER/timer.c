#include "timer.h"

void TIM2_Init(u16 arr,u16 psc){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //�򿪶�ʱ��2��ʱ��
    
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision =0;
	TIM_TimeBaseInitStruct.TIM_CounterMode =TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period =arr;
	TIM_TimeBaseInitStruct.TIM_Prescaler =psc;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);			//���ö�ʱ����װ��ֵ�ͷ�Ƶϵ��

	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCInitStruct.TIM_OCMode =TIM_OCMode_PWM1 ;
	TIM_OCInitStruct.TIM_OCPolarity =TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState =TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse =10;
	TIM_OC2Init(TIM2, &TIM_OCInitStruct);			//���ö�ʱ����PWM�Ѿ�����ռ�ձ� ռ�ձ�������û��ʵ�ʵ�����  ֻ��ϣ����ʱ���ܹ��ṩ�������źŸ�adc��Ȼ�󴥷�adc��ת��

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);	 //������Ƚ�ͨ��2���Զ���װֵ
	TIM_Cmd(TIM2, DISABLE);			//ʹ�ܶ�ʱ��2
}
