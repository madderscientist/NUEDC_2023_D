#include "rda5820.h"
#include "delay.h"
#include "OLED.h"
// 判断当前是不是一个合法的电台
# define RDA5820_ValidStop (RDA5820_RD_Reg(0x0B)&0x0100)
# define RDA5820_SF (RDA5820_RD_Reg(0x0B)&0x2000)

u16 id=0;
// 单位：5kHz
u8 space;
u16 maxFreq;
u16 minFreq;
// 信号强度 自动搜索后会更新
u8 RSSI;
//初始化
//0,初始化成功;
//其他,初始化失败.

u8 RDA5820_Init(void)
{
	IIC_Init();						//初始化IIC口
	id=RDA5820_RD_Reg(RDA5820_R00);
    OLED_ShowHexNum(3,1,id,4);
	if(id==0X5820)					//读取ID正确
	{
	 	RDA5820_WR_Reg(RDA5820_R02,0x0002);	//软复位
		delay_ms(50);
	 	RDA5820_WR_Reg(RDA5820_R02,0xC001);	//立体声,上电
		delay_ms(600);						//等待时钟稳定 
	 	RDA5820_WR_Reg(RDA5820_R05,0X88CF);	//搜索强度8,LNAN,1.8mA,VOL最大
 	 	RDA5820_WR_Reg(0X07,0X7800);		// 
	 	RDA5820_WR_Reg(0X13,0X0008);		// 
	 	RDA5820_WR_Reg(0X15,0x1420);		//VCO设置  0x17A0/0x1420 
	 	RDA5820_WR_Reg(0X16,0XC000);		//  
	 	RDA5820_WR_Reg(0X1C,0X3126);		// 
	 	RDA5820_WR_Reg(0X22,0X9C24);		//fm_true 
	 	RDA5820_WR_Reg(0X47,0XF660) ;		//tx rds 
 	}else return 1;//初始化失败
	return 0;
}

//写RDA5820寄存器  
void RDA5820_WR_Reg(u8 addr,u16 val)
{
    IIC_Start();  			   
	IIC_Send_Byte(RDA5820_WRITE);	//发送写命令
	IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   			//发送地址
	IIC_Wait_Ack();
	IIC_Send_Byte(val>>8);     		//发送高字节
	IIC_Wait_Ack();  		    	   
 	IIC_Send_Byte(val&0XFF);     	//发送低字节
 	IIC_Wait_Ack();
    IIC_Stop();						//产生一个停止条件
}
//读RDA5820寄存器	
u16 RDA5820_RD_Reg(u8 addr)
{
	u16 res;
    IIC_Start();
	IIC_Send_Byte(RDA5820_WRITE);	//发送写命令
	IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   			//发送地址
	IIC_Wait_Ack();
    IIC_Start();
	IIC_Send_Byte(RDA5820_READ);    //发送读命令
	IIC_Wait_Ack();
 	res=IIC_Read_Byte(1);     		//读高字节,发送ACK
  	res<<=8;
  	res|=IIC_Read_Byte(0);     		//读低字节,发送NACK
  	IIC_Stop();						//产生一个停止条件 
	return res;						//返回读到的数据
}


//设置RDA5820为RX模式
void RDA5820_RX_Mode(void) {
	u16 temp=RDA5820_RD_Reg(0X40);		//读取0X40的内容
	temp&=0xfff0;					//RX 模式
	RDA5820_WR_Reg(0X40,temp) ;		//FM RX模式 
}

//设置RDA5820为TX模式
void RDA5820_TX_Mode(void) {
	u16 temp=RDA5820_RD_Reg(0X40);		//读取0X40的内容
	temp&=0xfff0;
	temp|=0x0001;				    //TX 模式
	RDA5820_WR_Reg(0X40,temp) ;		//FM TM 模式 
}
//得到信号强度
//返回值范围:0~127
u8 RDA5820_Rssi_Get(void)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X0B);		//读取0X0B的内容
	return temp>>9;                 //返回信号强度
}
//设置音量ok
//vol:0~15;
void RDA5820_Vol_Set(u8 vol)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X05);		//读取0X05的内容
	temp&=0XFFF0;
	temp|=vol&0X0F;
	RDA5820_WR_Reg(0X05,temp) ;		//设置音量
}
//静音设置
//mute:0,不静音;1,静音
void RDA5820_Mute_Set(u8 mute)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X02);		//读取0X02的内容
	if(mute)temp|=1<<14;
	else temp&=~(1<<14);
	RDA5820_WR_Reg(0X02,temp) ;		//设置MUTE
}

//设置灵敏度（Seek SNR threshold value: Noise_th(dB) = 79 – seek_th）
//rssi:0~127;
void RDA5820_Rssi_Set(u8 rssi)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X05);		//读取0X05的内容
	temp&=0X80FF;
	temp|=((u16)rssi)<<8;
	RDA5820_WR_Reg(0X05,temp) ;		//设置RSSI
}


//设置TX发送功率
//gain:0~63
void RDA5820_TxPAG_Set(u8 gain) // TXPA_GAIN
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X41);		//读取0X41的内容
	temp&=0XFFC0;
	temp|=gain;				   		//GAIN
	RDA5820_WR_Reg(0X41,temp) ;		//设置PA的功率
}

//设置TX 输入信号增益，gain:0~7
void RDA5820_TxPGA_Set(u8 gain) // FMTX_PGA_GAIN
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X68);
	temp&=0XE3FF;
	temp|=gain<<10;			    	//GAIN
	RDA5820_WR_Reg(0X68,temp) ;		//设置PGA
}


//设置RDA5820的工作频段
//band:0,87~108Mhz;1,76~91Mhz;2,76~108Mhz;3,65-76MHz or 50-65MHz
void RDA5820_Band_Set(u8 band)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X03);		//读取0X03的内容
	temp&=0XFFF3;
	temp|=band<<2;			     
	RDA5820_WR_Reg(0X03,temp) ;		//设置BAND
}

//设置RDA5820的步进频率
//0,100Khz;1,200Khz;2,50Khz;3,25khz
void RDA5820_Space_Set(u8 spc)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X03);		//读取0X03的内容
	temp&=0XFFFC;
	temp|=spc;
	RDA5820_WR_Reg(0X03,temp);		//设置BAND
}

//设置RDA5820的频率
//freq:频率值(单位为10Khz),比如10805,表示108.05Mhz
u16 RDA5820_Freq_Set(u16 freq) {
	u16 temp=RDA5820_RD_Reg(0X03)&0X001F;

	u8 band=(temp&0x0C)>>2;         // 得到频带

	u8 spc=temp&0x03;   // 以10k为单位则25k无法表示。先乘2
	if(spc==0)spc=20;
	else if(spc==1)spc=40;
	else if(spc==2)spc=10;
    else spc=5;
    
    u16 fbtm = 7600;                // 得到频段下限
	if(band==0)fbtm=8700;
	else if(band==3) fbtm=6500;

	if(freq<fbtm) return 0;
	u16 chan=2*(freq-fbtm)/spc;		//得到CHAN应该写入的值
	chan&=0X3FF;				//取低10位
	temp|=chan<<6;
	temp|=1<<4;					//TONE ENABLE
	RDA5820_WR_Reg(0X03,temp);	//设置频率
	delay_ms(5);
	while((RDA5820_RD_Reg(0X0B)&(1<<7))==0){};//等待FM_READY
    return RDA5820_Freq_Get();
}


//得到当前频率
//返回值:频率值(单位10Khz)
u16 RDA5820_Freq_Get(void) {
	u16 temp=RDA5820_RD_Reg(0X03);
	u16 chan=temp>>6;   
	u8 band=(temp&0x0C)>>2;

	u8 spc=temp&0x03;   // 以10k为单位则25k无法表示。先乘2
	if(spc==0)spc=20;
	else if(spc==1)spc=40;
	else if(spc==2)spc=10;
    else spc=5;

	u16 fbtm = 7600;                // 得到频段下限
	if(band==0)fbtm=8700;
	else if(band==3) fbtm=6500;

	return fbtm+chan*spc/2;//返回频率值
}

// 在本来的CHANNEL上加x 不检测问题
u16 RDA5820_Chan_Change(int8_t x){
    u16 temp=RDA5820_RD_Reg(0X03);
    u16 chan=temp>>6;
    chan+=x;
    RDA5820_WR_Reg(0X03,(temp&0x003F)|(chan<<6)|16);	//设置频率
	delay_ms(5);
	while((RDA5820_RD_Reg(0X0B)&(1<<7))==0){};//等待FM_READY
    return chan;
}

void RDA5820_Information(void){
    u16 temp=RDA5820_RD_Reg(0X03);
	
    space=temp&0x03;
	if(space==0)space=20;  // 单位5kHz
	else if(space==1)space=40;
	else if(space==2)space=10;
    else space=5;
    
    u16 band = temp&0x000C;
    maxFreq = 15200, minFreq = 13000; // 单位：5kHz
    switch(band){
        case 0:
            maxFreq = 21600;
            minFreq = 17400;
            break;
        case 4:
            maxFreq = 18200;
            minFreq = 15200;
            break;
        case 8:
            maxFreq = 21600;
            minFreq = 15200;
            break;
    }
}

// 开启搜频（搜索完毕会改变CHAN和0A[13]和0A[9:0]）需要提前设置好当前频率 需要提前改成接收模式
// 搜频成功的条件：信噪比达到RDA5820_Rssi_Set设定的阈值
u8 RDA5820_FMSeek(void) // 使SEEKUP SEEK SKMODE都为1
{
	u16 temp = RDA5820_RD_Reg(0x02) & ~0x0381;
	temp |= 0x0381;
	RDA5820_WR_Reg(0x02,temp);
    delay_ms(10);
    while(RDA5820_RD_Reg(0X02)&1<<8);      // 判断SEEK是否完成
    return (RDA5820_RD_Reg(0X0A)&1<<13)!=0;// 返回搜索成功与否
}


//SEEKTH:0~127 如果超过此限度则保留seekth 要提前设置好步距
u8 RDA5820_Seek_Station(u16 FreqStart,u16 SEEKTH){
	RDA5820_RX_Mode();//设置为接收模式
    RDA5820_Freq_Set(FreqStart);//设置起始频率
	//设置seek门限
    if(SEEKTH<128) RDA5820_Rssi_Set(SEEKTH);
	while(1) {
		RDA5820_WR_Reg(0x02,0xD381);
        while((RDA5820_RD_Reg(0x0A) & 0x4000)==0);  //STC=1
        if(RDA5820_SF==0) return 0;
		if(RDA5820_ValidStop==1) return 1;
	}
}

//自动搜索空闲频段并设置
//搜频成功条件：信号强度最小
//返回搜到的频率，单位10kHz
u16 RDA5820_Seek_Channel(void){
    u16 temp=RDA5820_RD_Reg(0X03);
	
    u8 spc=temp&0x03;
	if(spc==0)spc=20;  // 单位5kHz
	else if(spc==1)spc=40;
	else if(spc==2)spc=10;
    else spc=5;
    space = spc;    // 顺便更新全局信息
    
    u16 band = temp&0x000C;
    u16 max = 15200, min = 13000; // 单位：5kHz
    switch(band){
        case 0:
            max = 21600;
            min = 17400;
            break;
        case 4:
            max = 18200;
            min = 15200;
            break;
        case 8:
            max = 21600;
            min = 15200;
            break;
    }
    maxFreq = max; minFreq = min;

    RSSI = 0xFF;
    u16 minStep = 0;
    // 查询信号最弱的通道
    RDA5820_RX_Mode();
    for(band=0;min<=max-spc;min+=spc,band++){
        // 接收
        RDA5820_WR_Reg(0X03,(temp&0x003F)|(band<<6)|(1<<4));
        delay_ms(5);
        while((RDA5820_RD_Reg(0X0A)&(1<<14))==0);
        // 比较信号强度
        u8 current = RDA5820_Rssi_Get();
        if(current<RSSI){
            minStep = band;
            RSSI = current;
        }
    }
    // 设置
    RDA5820_TX_Mode();
    RDA5820_WR_Reg(0X03,(temp&0x003F)|(minStep<<6)|(1<<4));
    delay_ms(5);
    while((RDA5820_RD_Reg(0X0B)&(1<<7))==0);//等待FM_READY
    return (minFreq+minStep*space)/2;
}
