#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "24cxx.h"
#include "w25qxx.h"
#include "touch.h"
#include "AD9954.h"
#include "stimuSerial.h"
#include "UI.h"
 
extern char(*getKey)(u16, u16);
extern void(*function)(char);
u8 dds = 6;
// A0 A1 A4 A5 A6 A7
// C0 C1 C2 C3 C4 是连9954的-AD9954.h
// 电阻屏要用usart1，不然屏幕不亮
u8 testserial = 0;
int main(void) {
	delay_init();           //延时函数初始化	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);      //串口初始化为115200 屏幕显示必备
 	LED_Init();             //LED端口初始化
	LCD_Init();	
	KEY_Init();	 	
 	tp_dev.init();

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断分组
	delay_init();	//初始化延时函数
	delay_ms(1000);//延时一会儿，等待上电稳定,确保AD9954比控制板先上电。
	AD9954_Init();				//初始化控制AD9954需要用到的IO口,及寄存器
	AD9954_Set_Amp(16383);//设置幅度，范围0~16383 （对应幅度约：0~500mv)
	AD9954_Set_Phase(0);//设置相位，范围0~16383（对应角度：0°~360°)
//    AD9954_Set_Fre(dds*1000000);
    stimuSerial_Init(300,0,1);

    callUI();
    u8 key;u8 i=0;
    // 按键判断
	while(1){
        if(testserial) {
            stimuSerial_SendByte(0xAC);
        }
        key=KEY_Scan(0);
		tp_dev.scan(0);
		if(tp_dev.sta&TP_PRES_DOWN) {
			function(getKey(tp_dev.x[0],tp_dev.y[0]));
            delay_ms(100);
		}else delay_ms(10);

//        if(key==KEY0_PRES) {    // 减频率
//			AD9954_Set_Fre((--dds)*1000000);
//            char msg[10] = {0};
//            sprintf(msg,"dds:%dMHz",dds);
//            LCD_ShowString(10, 80, 120, 16, 16, (u8*)msg);
//		}
//        if(key==KEY1_PRES) {    // 加
//            AD9954_Set_Fre((++dds)*1000000);
//            char msg[10] = {0};
//            sprintf(msg,"dds:%dMHz",dds);
//            LCD_ShowString(10, 80, 120, 16, 16, (u8*)msg);
//		}
        if(key==WKUP_PRES) {    // 校准
			//TP_Adjust();
            //callUI();
            testserial = !testserial;
            delay_ms(100);
		}
        i++;
		if(i%30==0){
            LED0=!LED0;
            //AD9954_Set_Fre((dds)*1000000);
        }
	}
}

