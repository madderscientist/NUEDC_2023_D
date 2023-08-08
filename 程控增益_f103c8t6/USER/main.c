#include "sys.h"
#include "delay.h"
#include "X9C104.h"
#include "OLED.h"
#include "Key.h"
#include "AD.h"
#include "math.h"

/*
C8T6
B12 B13 B14 B15->KEY
数字电位器
A12->x9c104-CS
A11->x9c104-U/D
A10->x9c104-INC

采样
A4
*/


#define N 1000
float RMS(){              // 有效值测量
    delay_ms(20);
    float temp = 0;
    double rms = 0;
    for(uint16_t i = 0; i<N; i++){
        temp = 3.5 * AD_GetValue() / 4095;
        rms += temp * temp;
    }
    return sqrt(rms/N);
}

int8_t r = 0;
u8 add1(){
    r++;
    if(r>43){
        r = 43;
        return 0;
    }
    X9C104_Change(1);
    return 1;
}

void adjust(){
    float ACrms = RMS();
    if(ACrms>2.2){
        if(r<=1) goto final;    // 放小放大但仍然超范围
    }else{
        if(ACrms>0.4) goto final;   // 范围内
    }
    X9C104_Change(-100); r = 0;
    ACrms = RMS();
    if(ACrms>0.4) goto final;
    while(add1()){
        ACrms = RMS();
        if(ACrms>0.4) break;
        OLED_ShowNum(1,2,r,2);
        OLED_ShowNum(3,2,ACrms*1000,4);
    }
final:
    OLED_ShowNum(3,2,ACrms*1000,4);
    OLED_ShowNum(1,2,r,2);
}

int main(void){
    delay_init();
    
    X9C104_Init();
    Key_Init();
    OLED_Init();
    AD_Init();

    uint8_t i = 0;
    while(1){
        adjust();
        if(KEY1==0){
            delay_ms(10);
            if(KEY1) continue;

        }
        if(KEY2==0){
            delay_ms(10);
            if(KEY2) continue;

        }
        if(KEY3==0){
            delay_ms(10);
            if(KEY3) continue;

        }
        if(KEY4==0){
            delay_ms(10);
            if(KEY4) continue;

        }
        i = (i+1)%50;
        OLED_ShowNum(4,10,i,2);
        delay_ms(100);
    }
}
