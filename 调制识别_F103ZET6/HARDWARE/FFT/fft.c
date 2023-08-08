#include "fft.h"
#include "math.h"
#include "table.h"

// 32位
int32_t InBufArray[NPT]={0};         //定义输入数组
int32_t lBufOutArray[NPT];         //定义输出数组

/******************************************************************
函数名称:GetPowerMag()
函数功能:计算各次谐波幅值
参数说明:
备　　注:先将lBufOutArray分解成实部(X)和虚部(Y)，然后计算幅值(sqrt(X*X+Y*Y)
作　　者:博客园 依旧淡然（http://www.cnblogs.com/menlsh/）
*******************************************************************/
void GetPowerMag(){
	signed short lX,lY;
    float X,Y,Mag;
	for(u16 i=0; i<NPT/2; i++){
		lX  = (lBufOutArray[i] << 16) >> 16;
		lY  = (lBufOutArray[i] >> 16);
		X = NPT * ((float)lX) / 32768;
		Y = NPT * ((float)lY) / 32768;
		Mag = sqrt(X * X + Y * Y) / NPT;
		if(i == 0)
			 lBufOutArray[i] = (int32_t)(Mag * 32768);
		else
			 lBufOutArray[i] = (int32_t)(Mag * 65536);
	}
}

u16 leastFreq = 800*NPT/15000;
u16 freqGap = 120*NPT/15000;
uint16_t maxAT = 0;
uint32_t Average = 0;
void statistics(void){  // 统计最大值和平均值 忽略前800Hz
    uint32_t sum = 0;
    maxAT = leastFreq-1;
    for(u16 i=leastFreq;i<NPT/2; i++){  // 排除了直流
        sum += lBufOutArray[i];
        if(lBufOutArray[i]>lBufOutArray[maxAT]) maxAT = i;
    }
    Average = sum/(NPT/2-leastFreq);
}

u8 getdf(u16 kH, u16 FFTvalue){
    kH--;
    const u16* table = FMTable[kH];
    kH = FMTableSize[kH];
    u8 i = 0;
    while(i<kH && table[i]>FFTvalue){
        i++;
    }
    if(i==0) {
        if(FFTvalue - table[0]>80) return 0;    // 超大的就返回0， 表示不在表内，用公式算
        else return 25;                         // 表明已经最大
    }
    if(i==kH && table[i-1]>FFTvalue) return 0;
    else{
        kH = FFTvalue-table[i];
        u16 kH2 = table[i-1] - FFTvalue;
        if(kH<kH2){     // 更靠近下一个
            return 25-i;
        }else{
            return 26-i;
        }
    }
}

