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
}
// RL->RW->RH������Ӧ��RL��RH�룬��RW������0��ӦRL->RW����С��ֵ 
void X9C104_Change(int8_t x){
    CS = 0;     // ѡ�У�ʹ��
    if(x<0){    // �жϷ���
        x = -x;
        UD = 0;
    }else{
        UD = 1;
    }
    for(u8 i =0;i<x;i++){   //����
        delay_ms(1);
        INC = 0;
        delay_ms(1);
        INC = 1;
    }
    CS = 1;
}

void AD620_Init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOB, &GPIO_InitStruct);
    ADCS = 1;
    ADINC = 1;
    ADUD = 1;
}

void AD620_Change(int8_t x){
    ADCS = 0;     // ѡ�У�ʹ��
    if(x<0){    // �жϷ��� ��X9C104�෴
        x = -x;
        ADUD = 1;
    }else{
        ADUD = 0;
    }
    for(u8 i =0;i<x;i++){   //����
        delay_ms(1);
        ADINC = 0;
        delay_ms(1);
        ADINC = 1;
    }
    ADCS = 1;
}
