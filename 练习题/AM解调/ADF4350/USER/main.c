#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "24cxx.h"
#include "w25qxx.h"
#include "touch.h"
#include "ADF4351.h"
#include "UI.h"
 
extern char(*getKey)(u16, u16);
extern void(*function)(char);
u8 dds = 0;
u8 testserial = 0;
char msg[50] = {0};

int main(void) {
	delay_init();           //��ʱ������ʼ��
    
    // LCD��ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);      //���ڳ�ʼ��Ϊ115200 ��Ļ��ʾ�ر�
	LCD_Init();	
 	tp_dev.init();

 	LED_Init();             //LED�˿ڳ�ʼ��
    KEY_Init();
	delay_ms(300);

	ADF4351Init();          // �˴���4351�Ĵ������4350
	ADF4351WriteFreq(290);

    u8 key;u8 i=0;
    // �����ж�
	while(1){
//        sprintf(msg,"dds:%dMHz",dds);
//        LCD_ShowString(10, 80, 120, 16, 16, (u8*)msg);
//        dds = (1+dds)%100;
//        delay_ms(500);
//        LED0=!LED0;
	}
}

