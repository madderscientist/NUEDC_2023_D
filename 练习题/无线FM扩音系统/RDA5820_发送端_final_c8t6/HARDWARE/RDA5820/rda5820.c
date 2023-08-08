#include "rda5820.h"
#include "delay.h"
#include "OLED.h"
// �жϵ�ǰ�ǲ���һ���Ϸ��ĵ�̨
# define RDA5820_ValidStop (RDA5820_RD_Reg(0x0B)&0x0100)
# define RDA5820_SF (RDA5820_RD_Reg(0x0B)&0x2000)

u16 id=0;
// ��λ��5kHz
u8 space;
u16 maxFreq;
u16 minFreq;
// �ź�ǿ�� �Զ�����������
u8 RSSI;
//��ʼ��
//0,��ʼ���ɹ�;
//����,��ʼ��ʧ��.

u8 RDA5820_Init(void)
{
	IIC_Init();						//��ʼ��IIC��
	id=RDA5820_RD_Reg(RDA5820_R00);
    OLED_ShowHexNum(3,1,id,4);
	if(id==0X5820)					//��ȡID��ȷ
	{
	 	RDA5820_WR_Reg(RDA5820_R02,0x0002);	//��λ
		delay_ms(50);
	 	RDA5820_WR_Reg(RDA5820_R02,0xC001);	//������,�ϵ�
		delay_ms(600);						//�ȴ�ʱ���ȶ� 
	 	RDA5820_WR_Reg(RDA5820_R05,0X88CF);	//����ǿ��8,LNAN,1.8mA,VOL���
 	 	RDA5820_WR_Reg(0X07,0X7800);		// 
	 	RDA5820_WR_Reg(0X13,0X0008);		// 
	 	RDA5820_WR_Reg(0X15,0x1420);		//VCO����  0x17A0/0x1420 
	 	RDA5820_WR_Reg(0X16,0XC000);		//  
	 	RDA5820_WR_Reg(0X1C,0X3126);		// 
	 	RDA5820_WR_Reg(0X22,0X9C24);		//fm_true 
	 	RDA5820_WR_Reg(0X47,0XF660) ;		//tx rds 
 	}else return 1;//��ʼ��ʧ��
	return 0;
}

//дRDA5820�Ĵ���  
void RDA5820_WR_Reg(u8 addr,u16 val)
{
    IIC_Start();  			   
	IIC_Send_Byte(RDA5820_WRITE);	//����д����
	IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   			//���͵�ַ
	IIC_Wait_Ack();
	IIC_Send_Byte(val>>8);     		//���͸��ֽ�
	IIC_Wait_Ack();  		    	   
 	IIC_Send_Byte(val&0XFF);     	//���͵��ֽ�
 	IIC_Wait_Ack();
    IIC_Stop();						//����һ��ֹͣ����
}
//��RDA5820�Ĵ���	
u16 RDA5820_RD_Reg(u8 addr)
{
	u16 res;
    IIC_Start();
	IIC_Send_Byte(RDA5820_WRITE);	//����д����
	IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   			//���͵�ַ
	IIC_Wait_Ack();
    IIC_Start();
	IIC_Send_Byte(RDA5820_READ);    //���Ͷ�����
	IIC_Wait_Ack();
 	res=IIC_Read_Byte(1);     		//�����ֽ�,����ACK
  	res<<=8;
  	res|=IIC_Read_Byte(0);     		//�����ֽ�,����NACK
  	IIC_Stop();						//����һ��ֹͣ���� 
	return res;						//���ض���������
}


//����RDA5820ΪRXģʽ
void RDA5820_RX_Mode(void) {
	u16 temp=RDA5820_RD_Reg(0X40);		//��ȡ0X40������
	temp&=0xfff0;					//RX ģʽ
	RDA5820_WR_Reg(0X40,temp) ;		//FM RXģʽ 
}

//����RDA5820ΪTXģʽ
void RDA5820_TX_Mode(void) {
	u16 temp=RDA5820_RD_Reg(0X40);		//��ȡ0X40������
	temp&=0xfff0;
	temp|=0x0001;				    //TX ģʽ
	RDA5820_WR_Reg(0X40,temp) ;		//FM TM ģʽ 
}
//�õ��ź�ǿ��
//����ֵ��Χ:0~127
u8 RDA5820_Rssi_Get(void)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X0B);		//��ȡ0X0B������
	return temp>>9;                 //�����ź�ǿ��
}
//��������ok
//vol:0~15;
void RDA5820_Vol_Set(u8 vol)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X05);		//��ȡ0X05������
	temp&=0XFFF0;
	temp|=vol&0X0F;
	RDA5820_WR_Reg(0X05,temp) ;		//��������
}
//��������
//mute:0,������;1,����
void RDA5820_Mute_Set(u8 mute)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X02);		//��ȡ0X02������
	if(mute)temp|=1<<14;
	else temp&=~(1<<14);
	RDA5820_WR_Reg(0X02,temp) ;		//����MUTE
}

//���������ȣ�Seek SNR threshold value: Noise_th(dB) = 79 �C seek_th��
//rssi:0~127;
void RDA5820_Rssi_Set(u8 rssi)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X05);		//��ȡ0X05������
	temp&=0X80FF;
	temp|=((u16)rssi)<<8;
	RDA5820_WR_Reg(0X05,temp) ;		//����RSSI
}


//����TX���͹���
//gain:0~63
void RDA5820_TxPAG_Set(u8 gain) // TXPA_GAIN
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X41);		//��ȡ0X41������
	temp&=0XFFC0;
	temp|=gain;				   		//GAIN
	RDA5820_WR_Reg(0X41,temp) ;		//����PA�Ĺ���
}

//����TX �����ź����棬gain:0~7
void RDA5820_TxPGA_Set(u8 gain) // FMTX_PGA_GAIN
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X68);
	temp&=0XE3FF;
	temp|=gain<<10;			    	//GAIN
	RDA5820_WR_Reg(0X68,temp) ;		//����PGA
}


//����RDA5820�Ĺ���Ƶ��
//band:0,87~108Mhz;1,76~91Mhz;2,76~108Mhz;3,65-76MHz or 50-65MHz
void RDA5820_Band_Set(u8 band)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X03);		//��ȡ0X03������
	temp&=0XFFF3;
	temp|=band<<2;			     
	RDA5820_WR_Reg(0X03,temp) ;		//����BAND
}

//����RDA5820�Ĳ���Ƶ��
//0,100Khz;1,200Khz;2,50Khz;3,25khz
void RDA5820_Space_Set(u8 spc)
{
	u16 temp;
	temp=RDA5820_RD_Reg(0X03);		//��ȡ0X03������
	temp&=0XFFFC;
	temp|=spc;
	RDA5820_WR_Reg(0X03,temp);		//����BAND
}

//����RDA5820��Ƶ��
//freq:Ƶ��ֵ(��λΪ10Khz),����10805,��ʾ108.05Mhz
u16 RDA5820_Freq_Set(u16 freq) {
	u16 temp=RDA5820_RD_Reg(0X03)&0X001F;

	u8 band=(temp&0x0C)>>2;         // �õ�Ƶ��

	u8 spc=temp&0x03;   // ��10kΪ��λ��25k�޷���ʾ���ȳ�2
	if(spc==0)spc=20;
	else if(spc==1)spc=40;
	else if(spc==2)spc=10;
    else spc=5;
    
    u16 fbtm = 7600;                // �õ�Ƶ������
	if(band==0)fbtm=8700;
	else if(band==3) fbtm=6500;

	if(freq<fbtm) return 0;
	u16 chan=2*(freq-fbtm)/spc;		//�õ�CHANӦ��д���ֵ
	chan&=0X3FF;				//ȡ��10λ
	temp|=chan<<6;
	temp|=1<<4;					//TONE ENABLE
	RDA5820_WR_Reg(0X03,temp);	//����Ƶ��
	delay_ms(5);
	while((RDA5820_RD_Reg(0X0B)&(1<<7))==0){};//�ȴ�FM_READY
    return RDA5820_Freq_Get();
}


//�õ���ǰƵ��
//����ֵ:Ƶ��ֵ(��λ10Khz)
u16 RDA5820_Freq_Get(void) {
	u16 temp=RDA5820_RD_Reg(0X03);
	u16 chan=temp>>6;   
	u8 band=(temp&0x0C)>>2;

	u8 spc=temp&0x03;   // ��10kΪ��λ��25k�޷���ʾ���ȳ�2
	if(spc==0)spc=20;
	else if(spc==1)spc=40;
	else if(spc==2)spc=10;
    else spc=5;

	u16 fbtm = 7600;                // �õ�Ƶ������
	if(band==0)fbtm=8700;
	else if(band==3) fbtm=6500;

	return fbtm+chan*spc/2;//����Ƶ��ֵ
}

// �ڱ�����CHANNEL�ϼ�x ���������
u16 RDA5820_Chan_Change(int8_t x){
    u16 temp=RDA5820_RD_Reg(0X03);
    u16 chan=temp>>6;
    chan+=x;
    RDA5820_WR_Reg(0X03,(temp&0x003F)|(chan<<6)|16);	//����Ƶ��
	delay_ms(5);
	while((RDA5820_RD_Reg(0X0B)&(1<<7))==0){};//�ȴ�FM_READY
    return chan;
}

void RDA5820_Information(void){
    u16 temp=RDA5820_RD_Reg(0X03);
	
    space=temp&0x03;
	if(space==0)space=20;  // ��λ5kHz
	else if(space==1)space=40;
	else if(space==2)space=10;
    else space=5;
    
    u16 band = temp&0x000C;
    maxFreq = 15200, minFreq = 13000; // ��λ��5kHz
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

// ������Ƶ��������ϻ�ı�CHAN��0A[13]��0A[9:0]����Ҫ��ǰ���úõ�ǰƵ�� ��Ҫ��ǰ�ĳɽ���ģʽ
// ��Ƶ�ɹ�������������ȴﵽRDA5820_Rssi_Set�趨����ֵ
u8 RDA5820_FMSeek(void) // ʹSEEKUP SEEK SKMODE��Ϊ1
{
	u16 temp = RDA5820_RD_Reg(0x02) & ~0x0381;
	temp |= 0x0381;
	RDA5820_WR_Reg(0x02,temp);
    delay_ms(10);
    while(RDA5820_RD_Reg(0X02)&1<<8);      // �ж�SEEK�Ƿ����
    return (RDA5820_RD_Reg(0X0A)&1<<13)!=0;// ���������ɹ����
}


//SEEKTH:0~127 ����������޶�����seekth Ҫ��ǰ���úò���
u8 RDA5820_Seek_Station(u16 FreqStart,u16 SEEKTH){
	RDA5820_RX_Mode();//����Ϊ����ģʽ
    RDA5820_Freq_Set(FreqStart);//������ʼƵ��
	//����seek����
    if(SEEKTH<128) RDA5820_Rssi_Set(SEEKTH);
	while(1) {
		RDA5820_WR_Reg(0x02,0xD381);
        while((RDA5820_RD_Reg(0x0A) & 0x4000)==0);  //STC=1
        if(RDA5820_SF==0) return 0;
		if(RDA5820_ValidStop==1) return 1;
	}
}

//�Զ���������Ƶ�β�����
//��Ƶ�ɹ��������ź�ǿ����С
//�����ѵ���Ƶ�ʣ���λ10kHz
u16 RDA5820_Seek_Channel(void){
    u16 temp=RDA5820_RD_Reg(0X03);
	
    u8 spc=temp&0x03;
	if(spc==0)spc=20;  // ��λ5kHz
	else if(spc==1)spc=40;
	else if(spc==2)spc=10;
    else spc=5;
    space = spc;    // ˳�����ȫ����Ϣ
    
    u16 band = temp&0x000C;
    u16 max = 15200, min = 13000; // ��λ��5kHz
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
    // ��ѯ�ź�������ͨ��
    RDA5820_RX_Mode();
    for(band=0;min<=max-spc;min+=spc,band++){
        // ����
        RDA5820_WR_Reg(0X03,(temp&0x003F)|(band<<6)|(1<<4));
        delay_ms(5);
        while((RDA5820_RD_Reg(0X0A)&(1<<14))==0);
        // �Ƚ��ź�ǿ��
        u8 current = RDA5820_Rssi_Get();
        if(current<RSSI){
            minStep = band;
            RSSI = current;
        }
    }
    // ����
    RDA5820_TX_Mode();
    RDA5820_WR_Reg(0X03,(temp&0x003F)|(minStep<<6)|(1<<4));
    delay_ms(5);
    while((RDA5820_RD_Reg(0X0B)&(1<<7))==0);//�ȴ�FM_READY
    return (minFreq+minStep*space)/2;
}
