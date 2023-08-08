#include "led.h"
#include "delay.h"
#include "key.h"
#include "OLED.h"
#include "rda5820.h"
#include "myiic.h"
#include "stdio.h"
#define trytime 5
/*��Ļ��ʾ
ʵ��Ƶ��    �ĸ�������
WD: д����
RD: ������
�趨Ƶ��    �ź�ǿ��
*/
/*
��I2C���ȸ�myiic.c��SDA_PIN��SCL_PIN�����޸�main.c��switchMode������ע��ֻ����GPIOB
�İ�ť���ĳ�ʼ���Ͱ�ť�궨���е�����
����Ļ���ý��ƴ��7pinOLED�ļ��滻
*/
extern u8* RADIO_RDDATA;
extern u8* RADIO_WRDATA;
extern u8 radio_wrdata[5];
extern u8 radio_rddata[5];
u8 radio_rddata_2[5];
u8 radio_wrdata_2[5] = {0x2a,0xa6,0x30,0x11,0x00};// FM93.7

extern u8 SCL_PIN;
extern u8 SDA_PIN;
u32 frequency = 89700;
u32 frequency_temp = 93700;
int8_t step = 100;
u16 pll = 0;
char disbuff[20];

void showRD(){
    for(u8 i=0;i<5;i++){
        OLED_ShowHexNum(3,2*i+4,RADIO_RDDATA[i],2);
    }
}
void showWD(){
    for(u8 i=0;i<5;i++){
        OLED_ShowHexNum(2,2*i+4,RADIO_WRDATA[i],2);
    }
}
void showMode(){
    OLED_ShowNum(1,15,SCL_PIN==10,1);
}
void refreshOLED(){
    showMode();
    TEA5767_ReadReg(RADIO_RDDATA);
    showWD();
    showRD();
    OLED_ShowChar(1,10,' ');
    OLED_ShowNum(1,1,TEA5767_Calc_freq(TEA5767_Get_pll(RADIO_RDDATA)),6);              // ��ʾ��ȡ��Ƶ��
    OLED_ShowNum(4,1,frequency,6);      // ���õ�Ƶ��
    OLED_ShowBinNum(4,10,RADIO_RDDATA[3]>>4,4); // �ź�ǿ��
    
}
void change(u32 f){
    LED0 = !LED0;
    TEA5767_ReadReg(RADIO_RDDATA);
    TEA5767_Set_Frequency(frequency);
    OLED_ShowNum(1,1,TEA5767_Calc_freq(TEA5767_Get_pll(RADIO_RDDATA)),6);
    showWD();
    showRD();
    OLED_ShowNum(4,1,frequency,6);
    u8 temp = RADIO_RDDATA[3]>>4;
    OLED_ShowBinNum(4,10,temp,4);
}

void switchMode() {
    {
        u32 temp = frequency_temp;
        frequency_temp = frequency;
        frequency = temp;
    }
    if(SCL_PIN==10){
        SCL_PIN = 0;
        SDA_PIN = 1;
        RADIO_RDDATA = radio_rddata_2;
        RADIO_WRDATA = radio_wrdata_2;
    }else{
        SCL_PIN = 10;
        SDA_PIN = 11;
        RADIO_RDDATA = radio_rddata;
        RADIO_WRDATA = radio_wrdata;
    }
}

int main(void){
    LED_Init();
    Key_Init();
    OLED_Init();
    OLED_ShowString(2,1,"WD:");
    OLED_ShowString(3,1,"RD:");
    
    LED0 = !LED0;
    TEA5767_Init(); // ��ʼ����һ��(PIN10�� PIN11)
    switchMode();   // ��ʼ����һ��(PIN0 �� PIN1)
    TEA5767_Init();
    refreshOLED();
    LED0 = !LED0;
    
    while(1){
        if(KEY_SUB==0){ // ��Ƶ��
            delay_ms(10);
            if(KEY_SUB) continue;
            frequency+=step;
            change(frequency);
        }
        if(KEY_ADD==0){ // ��Ƶ��
            delay_ms(10);
            if(KEY_ADD) continue;
            frequency-=step;
            change(frequency);
        }
        if(KEY_AUTO==0){    // �Զ�����
            delay_ms(10);
            if(KEY_AUTO) continue;
            OLED_ShowString(4,1," AUTO ");
            OLED_ShowNum(1,10,auto_search(frequency<(min_freq+max_freq)/2),1);
            LED0 = !LED0;
            frequency = TEA5767_Calc_freq(TEA5767_Get_pll(RADIO_RDDATA));
            OLED_ShowNum(1,1,frequency,6);
            showWD();
            showRD();
            OLED_ShowNum(4,1,frequency,6);
            u8 temp = RADIO_RDDATA[3]>>4;
            OLED_ShowBinNum(4,10,temp,4);
        }
        if(KEY_ACC==0){     // �л�������
            delay_ms(10);
            if(KEY_ACC) continue;
            LED0 = !LED0;
            switchMode();
            refreshOLED();
            delay_ms(100);  // ����̫���ˣ�����һ���ٶ�
        }
        delay_ms(10);
    }
}
