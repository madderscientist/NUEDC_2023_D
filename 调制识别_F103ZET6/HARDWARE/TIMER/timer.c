#include "timer.h"

void TIM2_Init(u16 arr,u16 psc){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //打开定时器2的时钟
    
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision =0;
	TIM_TimeBaseInitStruct.TIM_CounterMode =TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period =arr;
	TIM_TimeBaseInitStruct.TIM_Prescaler =psc;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);			//设置定时器的装载值和分频系数

	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCInitStruct.TIM_OCMode =TIM_OCMode_PWM1 ;
	TIM_OCInitStruct.TIM_OCPolarity =TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState =TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse =10;
	TIM_OC2Init(TIM2, &TIM_OCInitStruct);			//设置定时器的PWM已经他的占空比 占空比在这里没有实际的意义  只是希望定时器能够提供上升沿信号给adc，然后触发adc的转换

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);	 //打开输出比较通道2的自动重装值
	TIM_Cmd(TIM2, DISABLE);			//使能定时器2
}
