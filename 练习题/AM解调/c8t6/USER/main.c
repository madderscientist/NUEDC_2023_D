#include "sys.h"
#include "delay.h"
#include "led.h"
#include "X9C104.h"
#include "OLED.h"
#include "Key.h"
#include "ADF4351.h"
#include "AD.h"

/*
C8T6
锁相环频率输出
A0->PLL_CE
A1->PLL_LE
A2->PLL-DATA
A3->PLL-CLK
OLED和按钮
B8 B9->OLED
B12 B13 B14 B15->KEY
数字电位器
A12->x9c104-CS
A11->x9c104-U/D
A10->x9c104-INC
AD620程控放大模块
B5->U/D
B6->INC
B7->CS
采样
A4
*/

float PLLfreq = 139.3;

void changePLL(float x){
    PLLfreq += x;
    OLED_ShowFloat(1,5,PLLfreq,3,2);
    ADF4351WriteFreq(PLLfreq);
}

uint8_t Rf = 0;
uint8_t ADRf = 0;

// 每次调整增加输入的3倍左右
u8 changeRf(int8_t x){        // 成功则返回1
    if(x+Rf>=0 && x+Rf<=31){  // 这是实测的可行范围
        X9C104_Change(x);
        Rf+=x;
        return 1;
    }else{
        Rf = x>0?31:0;
        X9C104_Change(x);
        return 0;
    }
}
// 调整AD620
#define AD620MAX 90
u8 changeADRf(int8_t x){      // 成功则返回1
    if(x+ADRf>=0 && x+ADRf<=AD620MAX){  // 这是精度达标的范围
        AD620_Change(x);
        ADRf+=x;
        return 1;
    }else{
        if(x>0){
            AD620_Change(AD620MAX-ADRf);
            ADRf = AD620MAX;
        }else{
            AD620_Change(-ADRf);
            ADRf = 0;
        }
        return 0;
    }
}

#include "math.h"
#define abs(x) x>0?x:-x
#define CENTER 1.16
#define CENTER2 0.99

float_t RMS(){              // 有效值测量
    delay_ms(2);
    float_t temp = 0;
	double_t sum = 0;
	for(uint16_t i = 0; i<1000; i++){
		temp = 3.3 * AD_GetValue() / 4095;
		sum += temp * temp;
	}
	return sqrt(sum/1000);
}

void adjust(){
    double_t rms = RMS();
    if(rms<=0.91||rms>=1.09){   // 范围稍微窄一点，弥补实际和单片机测量的误差
        // AD620调整至24档
        changeADRf(23-ADRf);
        // 调整X9C104至rms离CENTER(1.16)最近
        rms = RMS()-CENTER;
        int8_t direction = rms>0?-1:1;
        double_t before = abs(rms);
        while(changeRf(direction)){     // 没到上下限
            rms = RMS();
            if(rms>2.4||rms<0.2) continue;  // 防止削顶使两次差值相近。（严格控制应在0.417~1.9Vrms间）
            rms = abs(rms-CENTER);    // 差值
            if(rms>before){           // 这次离得远了，用上次的
                changeRf(-direction);
                break;
            }
            before = rms;
        }
        OLED_ShowNum(3,1,Rf,2);
        // 调整AD620至rms在1左右
        rms = RMS()-CENTER2;
        direction = rms>0?-1:1;
        before = abs(rms);
        while(changeADRf(direction)){
            rms = RMS();
            if(rms>1.2||rms<0.8) continue;
            rms = abs(rms-CENTER2);
            if(rms>before){
                changeADRf(-direction);
                break;
            }
            before = rms;
        }
        OLED_ShowNum(3,6,ADRf,2);
        OLED_ShowFloat(2,1,RMS(),1,2);
    }else{
        OLED_ShowFloat(2,1,rms,1,2);
    }
}


int main(void){
    delay_init();
    
    X9C104_Init();
    AD620_Init();
    Key_Init();
    OLED_Init();
    AD_Init();
    delay_ms(200);
    ADF4351Init();          // 此处用4351的代码控制4350

    OLED_ShowString(1,1,"PLL:");
    OLED_ShowFloat(1,5,PLLfreq,3,2);
    delay_ms(100);  // 不延迟似乎不稳，但是延迟了也不稳。所以在while中时不时更新
    ADF4351WriteFreq(PLLfreq);

    X9C104_Change(-100);    // 电位器清零
    AD620_Change(-100);
    OLED_ShowNum(2,1,Rf,3);

    u8 i = 0;
    while(1){
        if(KEY1==0){
            delay_ms(10);
            if(KEY1) continue;
            //changeRf(1);
            changePLL(1);
            
        }
        if(KEY2==0){
            delay_ms(10);
            if(KEY2) continue;
            //changeRf(-1);
            changePLL(0.1);
        }
        if(KEY3==0){
            delay_ms(10);
            if(KEY3) continue;
            //changeADRf(1);
            changePLL(-0.1);
        }
        if(KEY4==0){
            delay_ms(10);
            if(KEY4) continue;
            //changeADRf(-1);
            changePLL(-1);
            //adjust();
        }
        i = (i+1)%50;
        if(i==2) ADF4351WriteFreq(PLLfreq); // 时不时更新PLL防止波形不稳
        adjust();
        delay_ms(100);
    }
}
