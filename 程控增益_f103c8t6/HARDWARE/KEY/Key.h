#ifndef __KEY_H
#define __KEY_H

#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)
#define KEY3   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)
#define KEY4   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)

void Key_Init(void);

#endif
