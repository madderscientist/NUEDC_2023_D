#include "fft.h"
#include "math.h"
#include "table.h"

// 32λ
int32_t InBufArray[NPT]={0};         //������������
int32_t lBufOutArray[NPT];         //�����������

/******************************************************************
��������:GetPowerMag()
��������:�������г����ֵ
����˵��:
������ע:�Ƚ�lBufOutArray�ֽ��ʵ��(X)���鲿(Y)��Ȼ������ֵ(sqrt(X*X+Y*Y)
��������:����԰ ���ɵ�Ȼ��http://www.cnblogs.com/menlsh/��
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
void statistics(void){  // ͳ�����ֵ��ƽ��ֵ ����ǰ800Hz
    uint32_t sum = 0;
    maxAT = leastFreq-1;
    for(u16 i=leastFreq;i<NPT/2; i++){  // �ų���ֱ��
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
        if(FFTvalue - table[0]>80) return 0;    // ����ľͷ���0�� ��ʾ���ڱ��ڣ��ù�ʽ��
        else return 25;                         // �����Ѿ����
    }
    if(i==kH && table[i-1]>FFTvalue) return 0;
    else{
        kH = FFTvalue-table[i];
        u16 kH2 = table[i-1] - FFTvalue;
        if(kH<kH2){     // ��������һ��
            return 25-i;
        }else{
            return 26-i;
        }
    }
}

