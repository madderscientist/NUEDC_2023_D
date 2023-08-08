#include "rda5820.h"
#include "delay.h"
#include "OLED.h"
#include "myiic.h"
#define	TEA5767_WRADD		0xC0
#define	TEA5767_RDADD		0xC1

u8 radio_rddata[5];
u8* RADIO_RDDATA = radio_rddata;
u8 radio_wrdata[5]={0x2c,0x8e,0x30,0x11,0x00};// FM89.7
//{0x30,0x6f,0xc0,0x11,0x00}
u8* RADIO_WRDATA = radio_wrdata;

#define hlsi RADIO_RDDATA[2]&0x10

void TEA5767_Init(void)
{
	IIC_Init();
	TEA5767_WriteReg(RADIO_WRDATA);	//初始化写入初始化RADIO_WRDATA[5]={0x29,0xc2,0x20,0x11,0x00}
	TEA5767_ReadReg(RADIO_RDDATA);	//初始化写入初始化RADIO_WRDATA[5]={0x29,0xc2,0x20,0x11,0x00}
}

//写RDA5820寄存器						 				    
void TEA5767_WriteReg(u8* data)
{
    IIC_Start();  			   
	IIC_Send_Byte(TEA5767_WRADD);	//发送写命令
    if(!IIC_Wait_Ack()){
        for(u8 i=0;i<5;i++){
            IIC_Send_Byte(data[i]);
            IIC_Wait_Ack();
        }
    }
    IIC_Stop();						//产生一个停止条件
}
//读RDA5820寄存器	
void TEA5767_ReadReg(u8* data)
{
    IIC_Start();
	IIC_Send_Byte(TEA5767_RDADD);	//发送写命令
	if(!IIC_Wait_Ack()){
        for(u8 i=0;i<5;i++){
            data[i] = IIC_Read_Byte(1);
        }
    }
    IIC_Stop();
}
u16 TEA5767_Get_pll(u8* data){
    return ((data[0]&0x3f)<<8)+data[1];
}
u16 TEA5767_Calc_pll(u32 frequency){    // 需要先readReg到RADIO_RDDATA
    return (frequency+(hlsi?255:-255))/8.192;
}

u32 TEA5767_Calc_freq(u16 pll){
    return pll*8.192+(hlsi?255:-255);
}

u8 TEA5767_Set_Frequency(u32 fre){    // 需要先readReg到RADIO_RDDATA
    u16 pll = TEA5767_Calc_pll(fre);
    RADIO_WRDATA[0]=(pll>>8);
    RADIO_WRDATA[1]=pll&0x00ff;
    RADIO_WRDATA[2]=0x30;
    RADIO_WRDATA[3]=0x11;
    RADIO_WRDATA[4]=0x00;
    TEA5767_WriteReg(RADIO_WRDATA);
    delay_ms(100);
    TEA5767_ReadReg(RADIO_RDDATA);
    return (RADIO_RDDATA[0]&0x80)!=0;   // return 1:成功
}

u8 auto_search(u8 mode)
{
    OLED_ShowNum(1,13,mode,1);
    TEA5767_ReadReg(RADIO_RDDATA);
    u32 pll = TEA5767_Get_pll(RADIO_RDDATA);
    RADIO_WRDATA[0]=pll/256+0x40;
    RADIO_WRDATA[1]=pll%256;
    RADIO_WRDATA[2]=mode?0xa0:0x20;
    RADIO_WRDATA[3]=0x11;
    RADIO_WRDATA[4]=0x00;

    for(u8 max_trial_time = 50; max_trial_time>0; max_trial_time--){
        delay_ms(100);
        TEA5767_ReadReg(RADIO_RDDATA);
        if((RADIO_RDDATA[0]&0x80)!=0) return 1;
    }
    return 0;
}
