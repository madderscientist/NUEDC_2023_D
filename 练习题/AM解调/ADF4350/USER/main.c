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
	delay_init();           //延时函数初始化
    
    // LCD初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);      //串口初始化为115200 屏幕显示必备
	LCD_Init();	
 	tp_dev.init();

 	LED_Init();             //LED端口初始化
    KEY_Init();
	delay_ms(300);

	ADF4351Init();          // 此处用4351的代码控制4350
	ADF4351WriteFreq(290);

    u8 key;u8 i=0;
    // 按键判断
	while(1){
//        sprintf(msg,"dds:%dMHz",dds);
//        LCD_ShowString(10, 80, 120, 16, 16, (u8*)msg);
//        dds = (1+dds)%100;
//        delay_ms(500);
//        LED0=!LED0;
	}
}

