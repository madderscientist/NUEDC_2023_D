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
// C0 C1 C2 C3 C4 ����9954��-AD9954.h
// ������Ҫ��usart1����Ȼ��Ļ����
u8 testserial = 0;
int main(void) {
	delay_init();           //��ʱ������ʼ��	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);      //���ڳ�ʼ��Ϊ115200 ��Ļ��ʾ�ر�
 	LED_Init();             //LED�˿ڳ�ʼ��
	LCD_Init();	
	KEY_Init();	 	
 	tp_dev.init();

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����жϷ���
	delay_init();	//��ʼ����ʱ����
	delay_ms(1000);//��ʱһ������ȴ��ϵ��ȶ�,ȷ��AD9954�ȿ��ư����ϵ硣
	AD9954_Init();				//��ʼ������AD9954��Ҫ�õ���IO��,���Ĵ���
	AD9954_Set_Amp(16383);//���÷��ȣ���Χ0~16383 ����Ӧ����Լ��0~500mv)
	AD9954_Set_Phase(0);//������λ����Χ0~16383����Ӧ�Ƕȣ�0��~360��)
//    AD9954_Set_Fre(dds*1000000);
    stimuSerial_Init(300,0,1);

    callUI();
    u8 key;u8 i=0;
    // �����ж�
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

//        if(key==KEY0_PRES) {    // ��Ƶ��
//			AD9954_Set_Fre((--dds)*1000000);
//            char msg[10] = {0};
//            sprintf(msg,"dds:%dMHz",dds);
//            LCD_ShowString(10, 80, 120, 16, 16, (u8*)msg);
//		}
//        if(key==KEY1_PRES) {    // ��
//            AD9954_Set_Fre((++dds)*1000000);
//            char msg[10] = {0};
//            sprintf(msg,"dds:%dMHz",dds);
//            LCD_ShowString(10, 80, 120, 16, 16, (u8*)msg);
//		}
        if(key==WKUP_PRES) {    // У׼
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

