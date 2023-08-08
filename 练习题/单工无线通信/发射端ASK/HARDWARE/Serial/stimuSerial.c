# include "stimuSerial.h"
# include "delay.h"
# include "lcd.h"
# include "timer.h"

uint8_t Parity = 0;
uint8_t bitlist[11]={0};
int8_t bitlen = 10;

void stimuSerial_Init(uint16_t BaudRate, uint8_t parity, uint8_t stopbits){
    TIM3_Int_Init((3000000/BaudRate)-1,23);     // 波特率不能比1100再高，原因未知，传入2400时间单位还是那么多，小不下去。300波特率效果理想
    StimuSerialOut1;
    Parity = parity;
}

void stimuSerial_SendByte(uint8_t Byte){    // 只支持1个暂停位
    //{1, 0,0,0,0,0,0,0,0,0, 0}; max为[10]
    bitlist[0] = 1;
    bitlist[9] = 0;
    bitlen = Parity?9:8;
    uint8_t bit;
    uint8_t one_num = 0;
    for(uint8_t i = 0; i < 8; i++, bitlen--){
        bit = Byte & 1;
        bitlist[bitlen] = bit;
        Byte>>=1;
        one_num += bit;
    }
    switch(Parity){
        case 1:     // 1是奇校验
            bitlist[1] = 1 - one_num%2;
            bitlen = 10;
            break;
        case 2:     // 2是偶校验
            bitlist[1] = one_num%2;
            bitlen = 10;
            break;
        default:
            bitlen = 9;
            break;
    }
//    for(int8_t i = bitlen; i >= 0; i--){
//        LCD_ShowNum(10+16*(bitlen-i),10,bitlist[i],1,16);
//    }
    TIM_Cmd(TIM3, ENABLE);
    while(bitlen>=0);
}
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志
        if(bitlen<0) {
            TIM_Cmd(TIM3, DISABLE);return;
        }
        if(bitlist[bitlen]) StimuSerialOut1;
        else StimuSerialOut0;
        bitlen--;
	}
}
