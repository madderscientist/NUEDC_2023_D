#include "led.h"
#include "delay.h"
#include "key.h"
#include "OLED.h"
#include "rda5820.h"
#include "myiic.h"
#include "stdio.h"
#define trytime 5
/*屏幕显示
1   RDA5820 FM ^w^
2   状态栏
3   TX SPACE %d khz
4   TX Freq %.2f Mhz
*/
// 引脚：PB10 SCL,PB11 SDA,按钮PB12 13 14 15
extern u16 id;
extern u8 space;
extern u16 maxFreq;
extern u16 minFreq;
extern u8 RSSI;
u16 frequency = 0;

char disbuff[100];

void AUTO_SEEK(void);
void SHOW_TX_FREQ(void);
void SHOW_SPACE(void);

int main(void){
    LED_Init();
    Key_Init();

    OLED_Init();        // 初始化OLED
    LED0 = !LED0;       // 亮灯


    int i = 0;
    for (; i < trytime; i++){
        if (!RDA5820_Init()){ // 初始化成功
            OLED_ShowString(0, 0, "RDA5820 FM ^w^");
            RDA5820_Band_Set(0);   //设置频段为87~108Mhz
            RDA5820_Space_Set(2);  //设置步进为50Khz（0为100k，1为200k，2为50k，3为25k）
            RDA5820_TxPGA_Set(3);  //信号增益设置为3
            RDA5820_TxPAG_Set(63); //发射功率为最大.
            break;
        }
        else delay_ms(100);
    }
    if (i == trytime) {             // 初始化失败
        OLED_ShowString(1, 1, "RDA5820 init fail");
        sprintf(disbuff, " id=%x", id);
        OLED_ShowString(1, 1, disbuff);
        OLED_ShowString(2, 2, "Reboot after 3s");
        delay_ms(3000);
        // 软件重启
        __set_FAULTMASK(1);
        __DSB();
        SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos)|(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk)|SCB_AIRCR_SYSRESETREQ_Msk);
        __DSB();
        while(1);
    }

    AUTO_SEEK();
    SHOW_SPACE();

    while (1) {
        delay_ms(10);
        if(KEY_SUB==0){
            delay_ms(10);
            if(KEY_SUB) continue;
            if(frequency-space>=minFreq) {
                RDA5820_Chan_Change(-1);
                frequency = RDA5820_Freq_Get()*2; // 10k->5k
                SHOW_TX_FREQ();
            } else {
                OLED_ShowString(2, 1, " !== Freq Min ==! ");
            }
        }
        if(KEY_ADD==0){
            delay_ms(10);
            if(KEY_ADD) continue;
            if(frequency+space<=maxFreq) {
                RDA5820_Chan_Change(1);
                frequency = RDA5820_Freq_Get()*2;
                SHOW_TX_FREQ();
            } else {
                OLED_ShowString(2, 1, " !== Freq Max ==! ");
            }
        }
        if(KEY_AUTO==0){
            delay_ms(10);
            if(KEY_AUTO) continue;
            AUTO_SEEK();
        }
        if(KEY_ACC==0){
            delay_ms(10);
            if(KEY_ACC) continue;
            // 0,100Khz; 1,200Khz; 2,50Khz; 3,25khz
            switch(space){
                case 5:
                    space = 10;
                    RDA5820_Space_Set(2);
                    break;
                case 10:
                    space = 20;
                    RDA5820_Space_Set(0);
                    break;
                case 20:
                    space = 40;
                    RDA5820_Space_Set(1);
                    break;
                case 40:
                    space = 5;
                    RDA5820_Space_Set(3);
                    break;
            }
            RDA5820_Freq_Set(frequency/2);  // 更新一下chan
            SHOW_SPACE();
        }
        delay_ms(100);
    }
}

void AUTO_SEEK(){
    OLED_ShowString(1, 1, "Searching...");
    frequency = RDA5820_Seek_Channel()*2;
    OLED_ShowString(1, 1, "            ");
    sprintf(disbuff, "RSSI: %d", RSSI);
    OLED_ShowString(1, 1, disbuff);
    SHOW_TX_FREQ();
}
    
void SHOW_TX_FREQ(){
    sprintf(disbuff, "TX f %.2f Mhz", (float)frequency/200);
    OLED_ShowString(4, 1, disbuff);
    OLED_ShowString(2, 1, "                  ");
}
void SHOW_SPACE(){
    sprintf(disbuff, "TX step %d khz", space*5);
    OLED_ShowString(3, 1, disbuff);
}
