#include "led.h"
#include "sys.h"
// C8T6的C13接入了LED
void LED_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 // GPIO_Pin_13端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 // IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 // 根据设定参数初始化GPIOC.13

    GPIO_ResetBits(GPIOC,GPIO_Pin_13);						 // PC.13 输出低
}
