#ifndef __KEY_H
#define __KEY_H

#define KEY_SUB  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)
#define KEY_ADD  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)
#define KEY_AUTO   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)
#define KEY_ACC   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)


void Key_Init(void);
#endif
