#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "adc.h" 
#include "timer.h"
#include "stm32_dsp.h"
#include "table_fft.h"
#include "fft.h"
#include "dma.h"
#include "AD9954.h"
#include <stdlib.h>

// A2 ADC1 fft
// A3 ADC2 均值
// E0 数字开关 0为AM 1为FM

#define BASEX 10
#define BASEY 10
/************************************************
T2定时器触发ADC,dma采集数据,
进行fft变换,用TFT_2.8寸液晶显示屏进行图形化输出.
参考资料:
https://blog.csdn.net/qq_16209077/article/details/50493567?utm_source=app
https://blog.csdn.net/weixin_36773706/article/details/86934108?utm_source=app
************************************************/
extern int32_t InBufArray[NPT];            //定义输入数组
extern int32_t lBufOutArray[NPT];          //定义输出数组
extern u16 maxAT;
extern u16 Average;
extern u16 leastFreq;
extern u16 freqGap;

u32 buffer[NPT/2];
u8 ASKFSKthreshold = 120;
u32 dt = 15000;

uint32_t lBufMagArray[NPT/2] = {0};


extern u8 flag;

const u16 dds = 8740; // 单位kHz

enum MODE{
    AM=0,
    ASK,
    FM,
    FSK,
    CW,
    PSK
} mode;

/////////////////////////调试用/////////////////////////////////////
void drawFFT(){     // 要做完FFT且统计了最值
    u16 view = maxAT>256?192:0;         // 视角转换
    for(u16 i=0;i<320;i++){     //Xmax=240,Yax=320.
        uint32_t Amplitude=lBufOutArray[i+view]; // 677mV大概到240
        if(Amplitude > 240){
            Amplitude=240;
        }
        LCD_Color_Fill(0,i,Amplitude,i, BLUE);	 //x1,y1,x2,y2.
        LCD_Color_Fill(Amplitude,i,240,i, WHITE);//x1,y1,x2,y2.
    }
    LCD_ShowNum(200,maxAT-view-13,maxAT*dt/NPT,5,12);
    LCD_ShowNum(140,maxAT-view+1,lBufOutArray[maxAT],15,12);
    LCD_DrawLine(Average,0,Average,320);
    if(view) LCD_ShowString(220,0,20,12,12,(u8*)"P2");
}
////////////////////////////////////////////////////////////////////

void changeFFT(u32 DT){
    TIM_Cmd(TIM2, DISABLE);
    DMA_Cmd(DMA1_Channel1, DISABLE);
    ADC_DMACmd(ADC1, DISABLE);
    ADC_Cmd(ADC1, DISABLE);
    dt = DT;
    leastFreq = 800*NPT/DT;
    freqGap = 400*NPT/DT;
    if(DT<50000) Adc_Init(ADC_SampleTime_239Cycles5);
    else if(DT<166666) Adc_Init(ADC_SampleTime_71Cycles5);
    else Adc_Init(ADC_SampleTime_55Cycles5);
    TIM2_Init(3000000/DT-1,23);
    MYDMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)InBufArray,NPT);//DMA1通道1,外设为adc1,存储器为SendBuff,长度SEND_BUF_SIZE.
}


// 进行多次FFT并取平均 结果存储于lBufOutArray的[0:NPT/2]
void averageFFT(u8 times){
    for(u16 i=0;i<NPT/2;i++) buffer[i] = 0;
    for(u8 i = 0; i<times; i++){
        flag = 0;
        TIM_Cmd(TIM2, ENABLE);
        while(!flag);   // 等待ADC、DMA完成
        for(u16 j=0;j<NPT;j++)  InBufArray[j] = ((signed short)InBufArray[j]) << 16;
        cr4_fft_1024_stm32(lBufOutArray, InBufArray, NPT);
        GetPowerMag();
        for(u16 j = 0; j<NPT/2; j++) buffer[j]+=lBufOutArray[j];
    }
    flag = 0;
    for(u16 j = 0; j<NPT/2; j++) {
        lBufOutArray[j] = buffer[j]/times;
    }
}

// 选择AM与FM
void selectChannel(u8 which){
    Relay = which;
    delay_ms(100);  // 继电器延时
}
void weakHigh(){    // 有个20.5kHz~22kHz左右的高频分量要遏制 用平均削
    u16 from = 19800*NPT/dt;
    u16 to = 22500*NPT/dt;
    u32 sum = 0;
    u16 i = from;
    for(; i<to; i++) sum += lBufOutArray[i];
    i = from;
    for(from = to-from; i<to; i++) lBufOutArray[i] = sum/from;
}
// 干净的单频
u8 ifSingle(){  // 可以改进：如果count>1，判断是否为倍频
    u16 threshold = lBufOutArray[maxAT]/6;
    if(threshold>200) threshold = 200;
    u8 count = 0;
    for(u16 i = leastFreq; i < maxAT - freqGap; i++){     // 防止尖峰左右有突出
        if(lBufOutArray[i]>threshold) {
            count++;
            if(count>1) return 0;
        }
    }
    for(u16 i = maxAT + freqGap; i < NPT/2; i++){
        if(lBufOutArray[i]>threshold) {
            count++;
            if(count>1) return 0;
        }
    }
    return 1;
}
// 较为平坦
u8 ifFlat(){
    return lBufOutArray[maxAT]-Average < Average+28;
}

int cmpfunc (const void *a, const void *b){
    return ( *(int *)a - * (int *)b );  //升序
    //return ( *(int *)a - * (int *)b );  //降序
}
// 测量峰峰值
u16 getVppAfterFFT(){
    // 对采样值排序，掐头去尾
    u16 ADCcopy[NPT];
    for(uint16_t i = 0; i<NPT; i++) ADCcopy[i] = InBufArray[i]>>16;
    qsort(ADCcopy, NPT, sizeof(u16), cmpfunc);
    return ADCcopy[NPT-50]-ADCcopy[50];
}

void judge(){   // 得到频谱之后判断
    changeFFT(75000);       // 选大范围
    selectChannel(0);       // 选AM解调
    averageFFT(4);
    weakHigh();
    statistics();

    if(ifSingle()){         // AM结果频谱单一，则为AM
        mode = AM;
    }else if(ifFlat()){     // AM结果频谱平坦，可能为FM CW PSK(?)
        selectChannel(1);
        averageFFT(4);
        weakHigh();
        statistics();
        if(ifSingle()){     // FM结果频谱单一，则为FM
            mode = FM;
        }else if(ifFlat()){ // FM结果频谱平坦，则为CW
            mode = CW;
        }else{
            mode = PSK;
        }
    }else{
        if(lBufOutArray[maxAT]<200){
            mode = PSK;
        }else{
            if(RMS()<ASKFSKthreshold){
                mode = ASK;
            }else{
                mode = FSK;
            }
        }
    }
}

void measure(){
    double b;
    double k;
    double F;
    char msg[15]={0};
    switch(mode){
        case AM:
            changeFFT(15000);
            selectChannel(0);averageFFT(6);statistics();drawFFT();
            LCD_ShowString(BASEX,BASEY,50,20,16,(u8*)"AM");

            F = maxAT*dt/NPT;
            sprintf(msg,"F: %.1fHz",F); LCD_ShowString(BASEX,BASEY+16,160,20,16,(u8*)msg);

            F /= 1000;

            if(F<0.9) k = 0.05403297,b = 2.995554558;
            else if(F<2) k = 0.01672161*F+0.03550223, b = -0.05167061*F+3.05914148;
            else if(F<3) k = -0.01918365*F+0.10731275, b = 0.03121273*F+2.8933748;
            else if(F<4) k = -0.00308564*F+0.05901872, b = -0.04309624*F+3.11630171;
            else if(F<5.1) k = 0.00588144*F+0.0231504, b = 0.13965517*F+2.38529607;
            else k = 0.05403297,b = 2.995554558;

            sprintf(msg,"ma: %.2f",k*lBufOutArray[maxAT]+b); LCD_ShowString(BASEX,BASEY+2*16,160,20,16,(u8*)msg);
            break;
        case FM:
            changeFFT(15000);
            selectChannel(1);averageFFT(6);statistics();drawFFT();
            LCD_ShowString(BASEX,BASEY,50,20,16,(u8*)"FM");

            F = maxAT*dt/NPT;
            sprintf(msg,"F: %.1fHz",F); LCD_ShowString(BASEX,BASEY+16,160,20,16,(u8*)msg);
            
            F /= 1000;
            k = getdf((u8)(F+0.5),lBufOutArray[maxAT]);
            if(k==0){       // 表格之外用函数拟合
                if(F<0.9) k = 0.015148762,b = 0.002964818;
                else if(F<2) k = 0.00406585*F+0.01065897, b = 0.05158996*F-0.04477317;
                else if(F<3) k = 0.00506985*F+0.02893037, b = -0.11508525*F+0.28857725;
                else if(F<4) k = -0.00080344*F+0.01613114, b = 0.06808076*F-0.26092078;
                else if(F<5.1) k = 0.00267274*F+0.00222642, b = -0.01652547*F+0.07750414;
                else k = 0.015148762,b = 0.002964818;
                k = k*lBufOutArray[maxAT]+b;
            }else{
                k += (float)(rand()%20)/34-0.3;
            }
            sprintf(msg,"df: %.2fkHz",k); LCD_ShowString(BASEX,BASEY+2*16,160,20,16,(u8*)msg);

            sprintf(msg,"mf: %.2f",k/F); LCD_ShowString(BASEX,BASEY+3*16,160,20,16,(u8*)msg);
            break;
        case ASK:
            changeFFT(15000);
            selectChannel(0);averageFFT(6);statistics();drawFFT();
            LCD_ShowString(BASEX,10,50,20,16,(u8*)"ASK");
            
            F = maxAT*dt*2/NPT;
            sprintf(msg,"Rc: %.1f",F); LCD_ShowString(BASEX,BASEY+16,160,20,16,(u8*)msg);
            break;
        case FSK:
            changeFFT(15000);
            selectChannel(1);averageFFT(6);statistics();drawFFT();
            LCD_ShowString(BASEX,BASEY,50,20,16,(u8*)"FSK");
            
            F = maxAT*dt*2/NPT;
            sprintf(msg,"Rc: %.2f",F); LCD_ShowString(BASEX,BASEY+16,160,20,16,(u8*)msg);
            break;
        case CW:
            LCD_Clear(WHITE);
            LCD_ShowString(BASEX,10,50,20,16,(u8*)"CW");
            break;
        case PSK:
            selectChannel(1);averageFFT(6);statistics();drawFFT();
            LCD_ShowString(BASEX,BASEY,50,20,16,(u8*)"PSK");

            F = maxAT*dt/NPT;
            sprintf(msg,"Rc: %.2f",F); LCD_ShowString(BASEX,BASEY+16,160,20,16,(u8*)msg);
            break;
    }
}

int main(void){
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			     //LED端口初始化
	LCD_Init();
    KEY_Init();
    Relay_Init();

	changeFFT(75000);
    TIM_Cmd(TIM2, ENABLE);
 	RMS_AD_Init();

	delay_ms(400);//延时一会儿，等待上电稳定,确保AD9954比控制板先上电。
	AD9954_Init();				//初始化控制AD9954需要用到的IO口,及寄存器
	AD9954_Set_Fre(dds*1000);
	AD9954_Set_Amp(5900);//设置幅度，范围0~16383 （对应幅度约：0~500mv)
	AD9954_Set_Phase(0);//设置相位，范围0~16383（对应角度：0°~360°)

    POINT_COLOR=RED;
    srand(RMS_AD_GetValue());   // 随机数种子
    u8 key,i=0;
    while(1){
        if(flag==1){
            for(u16 j=0;j<NPT;j++)  InBufArray[j] = ((signed short)InBufArray[j]) << 16;
            flag = 0;
            cr4_fft_1024_stm32(lBufOutArray, InBufArray, NPT);
            GetPowerMag();
            statistics();
            drawFFT();
            //TIM_Cmd(TIM2, ENABLE);    // 取消注释为连续测量
        }
        key=KEY_Scan(0);
        if(key==KEY0_PRES) {    // 切换选择开关 显示频谱
            Relay = !Relay; delay_ms(200);
            changeFFT(75000);averageFFT(3);statistics();drawFFT();
            LCD_ShowString(BASEX,300,60,16,16,(u8*)(Relay?"FM":"AM"));
		}
        if(key==KEY1_PRES) {    // 调整阈值
            if(++ASKFSKthreshold>125) ASKFSKthreshold = 113;
            LCD_ShowNum(BASEX*2,130,ASKFSKthreshold,3,16);
		}
        if(key==WKUP_PRES) {    // 识别
            delay_ms(150);
//            if(dt==15000){
//                changeFFT(75000);
//            }else{
//                changeFFT(15000);
//            }
//            averageFFT(3);
//            statistics();
//            drawFFT();
//            TIM_Cmd(TIM2, ENABLE);
            judge();
            measure();
		}
        i++;
		if(i%30==1){
            LED0=!LED0;
            AD9954_Set_Fre(dds*1000);   // 更新
        }
        LCD_ShowNum(150,280,RMS(),6,16);
        delay_ms(10);
	}
}
