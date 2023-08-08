#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Key.h"
// PA10接信号输入
// PA6 PA7按键改ID
uint8_t RxData;
u8 receive = 0;
u8 address = 41;
// OLED
u8 cnt = 2;
void showID(){
    OLED_ShowString(4,2,"ID");
    OLED_ShowNum(4,4,address,3);
}
int main(void)
{
	OLED_Init();
	Serial_Init();
    Key_Init();
	showID();
	while (1)
	{
		if (Serial_GetRxFlag() == 1) {
			RxData = Serial_GetRxData();
			if(RxData&0x80){
                if((RxData&0x7F)==address||RxData==0x80) {
                    receive = 1;
                    OLED_Clear();
                    cnt= 2;
                } else {        //127|0x80终止，126|0x80语音开始，0|0x80群呼
                    if(RxData==(126|0x80)&&receive)
                        OLED_ShowString(1,2,"Voice message!");
                    receive = 0;
                }
            } else {
                if(receive){
                    OLED_ShowChar(cnt/15+1,cnt%15+1,RxData);
                    cnt++;
                }
            }
		}
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0) {
            Delay_ms(20);
            if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)) continue;
            if(address<125) address++;
            showID();
            Delay_ms(50);
        }
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0) {
            Delay_ms(20);
            if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)) continue;
            if(address>1) address--;
            showID();
            Delay_ms(50);
        }
	}
}
