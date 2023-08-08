#include "X9C104.h"
#include "delay.h"
#include "sys.h"

void X9C104_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStruct);
    CS = 1;
    INC = 1;
    UD = 1;
    X9C104_Change(-100);
}

// RL->RW->RH，电流应从RL或RH入，从RW出，且0对应RL->RW的最小阻值 
void X9C104_Change(int8_t x){
    CS = 0;     // 选中，使能
    if(x<0){    // 判断方向
        x = -x;
        UD = 0;
    }else{
        UD = 1;
    }
    for(u8 i =0;i<x;i++){   //更改
        delay_ms(1);
        INC = 0;
        delay_ms(1);
        INC = 1;
    }
    CS = 1;
}
