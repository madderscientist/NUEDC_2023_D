/**********************************************************
                       ��������
���ܣ�stm32f103rct6���ƣ�20MHz����ʱ�ӣ� 20��Ƶ��
�ӿڣ����ƽӿ������AD9954.h
ʱ�䣺2023/06/xx
�汾��4.3
���ߣ���������
������������ֻ��ѧϰʹ�ã�����ؾ���

������������뵽�Ա��꣬�������ӽ߳�Ϊ������ ^_^
https://kvdz.taobao.com/ 
**********************************************************/
#include "AD9954.h"
#include "delay.h"

//ϵͳƵ��fosc���ⲿ����Ƶ�ʣ���ϵͳƵ��=fs
#define fosc  20                        //����Ƶ�� 20Mhz
#define PLL_MULTIPLIER  20              //PLL��Ƶ����4--20��
#define fs  (fosc*PLL_MULTIPLIER)       //ϵͳʱ��Ƶ��

double fH_Num=10.73741824;
//double fH_Num=11.2204;
//double fH_Num=11.3671588397205;//
//double fH_Num = 11.3025455157895;//Ƶ��ת��ϵ����2^32/ϵͳʱ��Ƶ��


/************************************************************
** �������� ��void AD9954_GPIO_Init(void)  
** �������� ����ʼ������AD9954��Ҫ�õ���IO��
** ��ڲ��� ����
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9954_GPIO_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	AD9954_RES=0;
	AD9954_CS =0;
	AD9954_SCLK =0;
	AD9954_SDIO=0;
	AD9954_OSK=0;
	PS0=0;
	PS1=0;
	IOUPDATE=0;
	AD9954_IOSY=0;
	AD9954_PWR=0;
	delay_ms(10);
}

/*********************************************************************************************************
** �������� ��void AD9954_RESET(void)
** �������� ����λAD9954
** ��ڲ��� ����
** ���ڲ��� ����
** ����˵�� ������λҲ����
*********************************************************************************************************/
void AD9954_RESET(void)
{
	AD9954_RES = 1;
	delay_ms(10);
	AD9954_RES = 0;
	AD9954_CS = 0;
	AD9954_SCLK = 0;
	AD9954_CS = 1;
}


/*********************************************************************************************************
** �������� ��void UPDATE(void)
** �������� ������һ�������źţ�����AD9954�ڲ��Ĵ�����
** ��ڲ��� ����
** ���ڲ��� ����
** ����˵�� �����Բ����κ���ʱ
*********************************************************************************************************/
void UPDATE(void)
{ 
	IOUPDATE = 1;
	delay_us(10);
	IOUPDATE = 0;
}

/*********************************************************************************************************
** �������� ��void AD9954_Send_Byte(uint8_t dat)
** �������� ����AD9954����һ���ֽڵ�����
** ��ڲ��� ���������ֽ�
** ���ڲ��� ����
** ����˵�� ��AD9954�Ĵ����ٶ����Ϊ25M�����Բ�����ʱҲ����
*********************************************************************************************************/
void AD9954_Send_Byte(uint8_t dat)
{
	uint8_t i;
	for (i = 0;i<8;i++)
	{
		AD9954_SCLK = 0;
		delay_us(10);
		if (dat & 0x80)
		{
			AD9954_SDIO = 1;
		}
		else
		{
			AD9954_SDIO = 0;
		}
		AD9954_SCLK = 1;
		delay_us(10);
		dat <<= 1;
	}
}

/*********************************************************************************************************
** �������� ��uint8_t AD9954_Read_Byte(void)
** �������� ����AD9954һ���ֽڵ�����
** ��ڲ��� ����
** ���ڲ��� ��������һ���ֽ�����
** ����˵�� ��
*********************************************************************************************************/
uint8_t AD9954_Read_Byte(void)
{
	uint8_t i,dat=0;
	for (i = 0;i<8;i++)
	{
		AD9954_SCLK = 0;
		delay_us(2);
		dat|=AD9954_SDO;
		AD9954_SCLK = 1;
		delay_us(2);
		dat <<= 1;
	}
	return dat;
}

/************************************************************
** �������� ��void AD9954_Write_nByte(uint8_t RegAddr,uint8_t *Data,uint8_t Len)
** �������� ����AD9954ָ���ļĴ���д����
** ��ڲ��� ��RegAddr: �Ĵ�����ַ
						*Data: ������ʼ��ַ
						Len: Ҫд����ֽ���
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9954_Write_nByte(uint8_t RegAddr,uint8_t *Data,uint8_t Len)
{  	
	uint8_t t=0;
	
	AD9954_Send_Byte(RegAddr);
	for(t=0;t<Len;t++)
	{
		AD9954_Send_Byte(Data[t]);
	}												    
}
/*********************************************************************************************************
** �������� ��uint32_t AD9954_Read_nByte(uint8_t ReadAddr,uint8_t Len)
** �������� ����AD9954�Ĵ�������
** ��ڲ��� ��ReadAddr:Ҫ�����ļĴ�����ַ
							Len:Ҫ�������ݵĳ���1-4
** ���ڲ��� ������������
** ����˵�� ��
*********************************************************************************************************/
uint32_t AD9954_Read_nByte(uint8_t ReadAddr,uint8_t Len)
{
	uint8_t t=0;
	uint32_t temp=0;
	AD9954_CS=1;
	AD9954_Send_Byte(ReadAddr);
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AD9954_Read_Byte();
	}
	AD9954_CS=1;
	return temp;
}

/************************************************************
** �������� ��uint32_t Get_FTW(double Real_fH)
** �������� ��Ƶ������ת��
** ��ڲ��� ��Freq����Ҫת����Ƶ�ʣ�0-140000000hz
** ���ڲ��� ��Ƶ������ֵ
** ����˵�� ��
**************************************************************/
uint32_t Get_FTW(double Real_fH)
{
		return (uint32_t)(fH_Num*Real_fH);
}

/*********************************************************************************************************
** �������� ��void AD9954_Init(void))
** �������� ����ʼ��AD9954�Ĺܽź���򵥵��ڲ��Ĵ��������ã�
** ��ڲ��� ����
** ���ڲ��� ����
** ����˵�� �����ϵľ���Ϊ20MHz����������20��Ƶ��Ϊ400M
*********************************************************************************************************/
void AD9954_Init(void)
{
	uint8_t CFR1_data[4]={0,0,0,0};
	uint8_t CFR2_data[3]={0,0,0};

	AD9954_GPIO_Init();
	AD9954_RESET();
	delay_ms(300);
	AD9954_CS = 0;
	
	CFR1_data[0]=0X02;//�˴���0x02->OSKʹ�ܣ�0X00->OSK�رա���OSKģʽʹ�ܵ�ǰ���£����ȼĴ���(0X02)��Ч
										//��Ӣ�������ֲᣬpage 31 ��Amplitude Scale Factor (ASF)����
										//In manual OSK
										//mode, ASF<15:14> has no effect. ASF<13:0> provide the output
										//scale factor directly. If the OSK is disabled using CFR1<25>,
										//this register has no effect on device operation.
	CFR1_data[1]=0X00;
	CFR1_data[2]=0X00;
	CFR1_data[3]=0x00;//�Ƚ�������,�������;0x40,�Ƚ������÷��������
	AD9954_Write_nByte(CFR1,CFR1_data,4);//����д����ƹ��ܼĴ���1

	CFR2_data[0]=0X00;
	CFR2_data[1]=0X00;
	if(fs>400)
		;//ϵͳƵ�ʳ���оƬ���ֵ
	else if(fs>=250)
		CFR2_data[2]=PLL_MULTIPLIER<<3|0x04|0X03;
	else CFR2_data[2]=PLL_MULTIPLIER<<3;
	AD9954_Write_nByte(CFR2,CFR2_data,3);//����д����ƹ��ܼĴ���2
	
	AD9954_CS=1;
}


/*********************************************************************************************************
** �������� ��void AD9954_Set_Fre(double fre)
** �������� ������AD9954��ǰ��Ƶ����������õ��ǵ�һƵ�����
** ��ڲ��� ��fre:�����õ�Ƶ��ֵ 0-140000000hz
** ���ڲ��� ����
** ����˵�� ����Ϊ���õĸ��������м��㣬ת�������л������ͨ���������Ծ�ȷ��0.1Hz����
*********************************************************************************************************/
void AD9954_Set_Fre(double fre)//single tone
{
	uint8_t date[4] ={0x00,0x00,0x00,0x00};	//�м����
	uint32_t Temp=0;   
	
	Temp=Get_FTW(fre);
	date[0] =(uint8_t)(Temp >> 24);
	date[1] =(uint8_t)(Temp >> 16);
	date[2] =(uint8_t)(Temp >> 8);
	date[3] =(uint8_t)Temp;
	
	AD9954_CS = 0;
	AD9954_Write_nByte(FTW0,date,4);//дƵ�ʿ�����
	AD9954_CS=1;
	UPDATE();
}

/*********************************************************************************************************
** �������� ��void AD9954_Set_Amp(uint16_t Ampli)
** �������� ������AD9954�������
** ��ڲ��� ��Ampli��0-16383�������ֵԼ500mv
** ���ڲ��� ����
** ����˵�� ��
*********************************************************************************************************/
void AD9954_Set_Amp(uint16_t Ampli)
{
	uint8_t date[2] ={0x00,0x00};	
	
	AD9954_CS = 0;
	date[0]=(uint8_t)(Ampli >> 8);
	date[1]=(uint8_t)Ampli;
	AD9954_Write_nByte(ASF,date,2);
	AD9954_CS = 1;
	UPDATE();
}

/************************************************************
** �������� ��void AD9954_Set_Phase(uint8_t Channel,uint16_t Phase)
** �������� ������ͨ���������λ
** ��ڲ��� ��Phase:	�����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
** ���ڲ��� ����
** ����˵�� ����
**************************************************************/
void AD9954_Set_Phase(uint16_t Phase)//д��λ
{
	uint8_t date[2] ={0x00,0x00};	
	
	AD9954_CS = 0;
	date[0]=(uint8_t)(Phase >> 8);
	date[1]=(uint8_t)Phase;
	AD9954_Write_nByte(POW0,date,2);
	AD9954_CS = 1;
	UPDATE();
}


/*********************************************************************************************************
** �������� ��void AD9954_SetFSK(double f1,double f2,double f3,double f4,uint16_t Ampli)   
** �������� ������FSK�ź������������
** ��ڲ��� ��f1:Ƶ��1  0-140000000hz
** 	      		f2:Ƶ��2
** 	      		f3:Ƶ��3
** 	      		f4:Ƶ��4
**					 Ampli���ȣ�0-16383�������ֵԼ500mv
** ���ڲ��� ����
** ��������   			PS0: 0   1	0	 1
** �ܽŲ����� 			PS1: 0   0	1	 1
** ��Ӧ���� RAM�Σ�		 0   1	2	 3
** ����˵�� �����ĸ�RAM����������һ��Ƶ��ֵ��ͨ���ı�PS0��PS1�ĵ�ƽѡ���Ӧ��RAM�������Ӧ��Ƶ��ֵ��ʵ��FSK��Ҳ����ʵ�ֶ����FSK��
**            ͨ�����ÿ���PS0,PS1�ܽŵĵ�ƽ�Ϳ��Խ������Ƶı���ת��ΪFSK�ź����
*********************************************************************************************************/
void AD9954_SetFSK(double f1,double f2,double f3,double f4,uint16_t Ampli)   
{
	uint32_t FTW_Vau=0;
	uint8_t fdata[4]={0,0,0,0};	
	uint8_t CFR1_data[4]={0,0,0,0};
	uint8_t data[5]={0,0,0,0,0};
	
	AD9954_CS = 0;

	data[0]=0x00;	//��8λ б��=0x0000  
	data[1]=0x00;	//��8λ б��=0x0000
	data[2]=0x00;	//��ֹ��ַ:0x0000   ��8λ
	data[3]=0x00;	//0:1��ֹ��ַ:��2λ��2:7��ʼ��ַ��6λ:0x000;   
	data[4]=0x00;	//0:3��ʼ��ַ��4λ  5:7��RAM0������ģʽ0;  4����ͣ��λû�м���  
	AD9954_Write_nByte(RSCW0,data,5);
		
	data[0]=0x00;	//��8λ б��=0x0000  
	data[1]=0x00;	//��8λ б��=0x0000
	data[2]=0x01;	//��ֹ��ַ:0x0001   ��8λ
	data[3]=0x04;	//0:1��ֹ��ַ:��2λ��2:7��ʼ��ַ��6λ:0x0001;   
	data[4]=0x00;	//0:3��ʼ��ַ��4λ  5:7��RAM1������ģʽ0;  4����ͣ��λû�м���  
	AD9954_Write_nByte(RSCW1,data,5);
	
	data[0]=0x00;	//��8λ б��=0x0000  
	data[1]=0x00;	//��8λ б��=0x0000
	data[2]=0x02;	//��ֹ��ַ:0x0002   ��8λ
	data[3]=0x08;	//0:1��ֹ��ַ:��2λ��2:7��ʼ��ַ��6λ:0x0002;   
	data[4]=0x00;	//0:3��ʼ��ַ��4λ  5:7��RAM2������ģʽ0;  4����ͣ��λû�м���  
	AD9954_Write_nByte(RSCW2,data,5);
	
	data[0]=0x00;	//��8λ б��=0x0000  
	data[1]=0x00;	//��8λ б��=0x0000
	data[2]=0x03;	//��ֹ��ַ:0x0003   ��8λ
	data[3]=0x0c;	//0:1��ֹ��ַ:��2λ��2:7��ʼ��ַ��6λ:0x0003;   
	data[4]=0x00;	//0:3��ʼ��ַ��4λ  5:7��RAM3������ģʽ0;  4����ͣ��λû�м���  
	AD9954_Write_nByte(RSCW3,data,5);    	
	
	AD9954_CS = 1;
	UPDATE();
	
	AD9954_CS = 0;
	PS1=0;PS0=0;
	FTW_Vau=Get_FTW(f1); 
  fdata[0]=FTW_Vau>>24;
  fdata[1]=FTW_Vau>>16;
  fdata[2]=FTW_Vau>>8;
  fdata[3]=FTW_Vau;
	AD9954_Write_nByte(RAM,fdata,4);    	

	
	PS1=0;PS0=1;
	FTW_Vau=Get_FTW(f2); 
  fdata[0]=FTW_Vau>>24;
  fdata[1]=FTW_Vau>>16;
  fdata[2]=FTW_Vau>>8;
  fdata[3]=FTW_Vau;
	AD9954_Write_nByte(RAM,fdata,4);  
	
	PS1=1;PS0=0;
	FTW_Vau=Get_FTW(f3); 
  fdata[0]=FTW_Vau>>24;
  fdata[1]=FTW_Vau>>16;
  fdata[2]=FTW_Vau>>8;
  fdata[3]=FTW_Vau;
	AD9954_Write_nByte(RAM,fdata,4);  
	
	PS1=1;PS0=1;
	FTW_Vau=Get_FTW(f4); 
  fdata[0]=FTW_Vau>>24;
  fdata[1]=FTW_Vau>>16;
  fdata[2]=FTW_Vau>>8;
  fdata[3]=FTW_Vau;
	AD9954_Write_nByte(RAM,fdata,4);  
	
	AD9954_CS = 1;
	UPDATE();

	AD9954_CS = 0;
	CFR1_data[0]=0X82;//��RAM����λ����FTW
	CFR1_data[1]=0X00;
	CFR1_data[2]=0X00;
	CFR1_data[3]=0x00;//�Ƚ�������,�������;0x40,�Ƚ������÷��������
	AD9954_Write_nByte(CFR1,CFR1_data,4);//����д����ƹ��ܼĴ���1
	AD9954_CS = 1;
	UPDATE();

	AD9954_Set_Amp(Ampli);//���÷���

}   

/*********************************************************************************************************
** �������� ��void AD9954_SetPSK(uint16_t Phase1,uint16_t Phase2,uint16_t Phase3,uint16_t Phase4,double fre,uint16_t Ampli)
** �������� ������PSK�ź������������
** ��ڲ��� ��Phase1:��λ1 ��Χ��0-16383 ��Ӧ0-360��
** 	      		Phase2:��λ2
** 	      		Phase3:��λ3
** 	      		Phase4:��λ4
** 						fre��Ƶ��    0-140000000hz
**					 Ampli���ȣ�0-16383�������ֵԼ500mv
** ���ڲ��� ����
** ��������   			PS0: 0   1	0	 1
** �ܽŲ����� 			PS1: 0   0	1	 1
** ��Ӧ���� RAM�Σ�		 0   1	2	 3
** ����˵�� �����ĸ�RAM����������һ��Ƶ��ֵ��ͨ���ı�PS0��PS1�ĵ�ƽѡ���Ӧ��RAM�������Ӧ��Ƶ��ֵ��ʵ��PSK��Ҳ����ʵ�ֶ����PSK��
**            ͨ�����ÿ���PS0,PS1�ܽŵĵ�ƽ�Ϳ��Խ������Ƶı���ת��ΪPSK�ź����
*********************************************************************************************************/
void AD9954_SetPSK(uint16_t Phase1,uint16_t Phase2,uint16_t Phase3,uint16_t Phase4,double fre,uint16_t Ampli)
{
	uint8_t pdata[4]={0,0,0,0};	
	uint8_t CFR1_data[4]={0,0,0,0};

	uint8_t data[5]={0,0,0,0,0};
	
	AD9954_CS = 0;

	data[0]=0x00;	//��8λ б��=0x0000  
	data[1]=0x00;	//��8λ б��=0x0000
	data[2]=0x00;	//��ֹ��ַ:0x0000   ��8λ
	data[3]=0x00;	//0:1��ֹ��ַ:��2λ��2:7��ʼ��ַ��6λ:0x000;   
	data[4]=0x00;	//0:3��ʼ��ַ��4λ  5:7��RAM0������ģʽ0;  4����ͣ��λû�м���  
	AD9954_Write_nByte(RSCW0,data,5);
		
	data[0]=0x00;	//��8λ б��=0x0000  
	data[1]=0x00;	//��8λ б��=0x0000
	data[2]=0x01;	//��ֹ��ַ:0x0001   ��8λ
	data[3]=0x04;	//0:1��ֹ��ַ:��2λ��2:7��ʼ��ַ��6λ:0x0001;   
	data[4]=0x00;	//0:3��ʼ��ַ��4λ  5:7��RAM1������ģʽ0;  4����ͣ��λû�м���  
	AD9954_Write_nByte(RSCW1,data,5);
	
	data[0]=0x00;	//��8λ б��=0x0000  
	data[1]=0x00;	//��8λ б��=0x0000
	data[2]=0x02;	//��ֹ��ַ:0x0002   ��8λ
	data[3]=0x08;	//0:1��ֹ��ַ:��2λ��2:7��ʼ��ַ��6λ:0x0002;   
	data[4]=0x00;	//0:3��ʼ��ַ��4λ  5:7��RAM2������ģʽ0;  4����ͣ��λû�м���  
	AD9954_Write_nByte(RSCW2,data,5);
	
	data[0]=0x00;	//��8λ б��=0x0000  
	data[1]=0x00;	//��8λ б��=0x0000
	data[2]=0x03;	//��ֹ��ַ:0x0003   ��8λ
	data[3]=0x0c;	//0:1��ֹ��ַ:��2λ��2:7��ʼ��ַ��6λ:0x0003;   
	data[4]=0x00;	//0:3��ʼ��ַ��4λ  5:7��RAM3������ģʽ0;  4����ͣ��λû�м���  
	AD9954_Write_nByte(RSCW3,data,5);    	
	
	AD9954_CS = 1;
	UPDATE();
	
	Phase1<<=2;
	Phase2<<=2;
	Phase3<<=2;
	Phase4<<=2;
	
	AD9954_CS = 0;
	PS1=0;PS0=0;
  pdata[0]=(uint8_t)(Phase1>>8);
  pdata[1]=(uint8_t)Phase1;
	pdata[2]=0x00;
  pdata[3]=0x00;
	AD9954_Write_nByte(RAM,pdata,4);    	

	PS1=0;PS0=1;
  pdata[0]=(uint8_t)(Phase2>>8);
  pdata[1]=(uint8_t)Phase2;
	pdata[2]=0x00;
  pdata[3]=0x00;
	AD9954_Write_nByte(RAM,pdata,4);  
	
	PS1=1;PS0=0;
  pdata[0]=(uint8_t)(Phase3>>8);
  pdata[1]=(uint8_t)Phase3;
	pdata[2]=0x00;
  pdata[3]=0x00;
	AD9954_Write_nByte(RAM,pdata,4);  
	
	PS1=1;PS0=1;
  pdata[0]=(uint8_t)(Phase4>>8);
  pdata[1]=(uint8_t)Phase4;
	pdata[2]=0x00;
  pdata[3]=0x00;
	AD9954_Write_nByte(RAM,pdata,4);  
	
	AD9954_CS = 1;
	UPDATE();

	AD9954_CS = 0;
	CFR1_data[0]=0Xc2;//��RAM����λ����POW��λ
	CFR1_data[1]=0X00;
	CFR1_data[2]=0X00;
	CFR1_data[3]=0x00;//�Ƚ�������,�������;0x40,�Ƚ������÷��������
	AD9954_Write_nByte(CFR1,CFR1_data,4);//����д����ƹ��ܼĴ���1
	AD9954_CS = 1;
	UPDATE();
	AD9954_Set_Amp(Ampli);//���÷���
	AD9954_Set_Fre(fre);
}

/*********************************************************************************************************
** �������� ��void AD9954_Set_LinearSweep(double Freq_Low,double Freq_High,double  UpStepFreq, 
**																			uint8_t UpStepTime,double	DownStepFreq, uint8_t DownStepTime,uint8_t mode)
** �������� ������ɨ�����ģʽ
** ����˵�� ��ʹƵ�ʰ�Ԥ�õ�ģʽ����ɨ����ȥ����ϸ�μ��ٷ�PDF
** ��ڲ��� ��Freq_Low:��ʼƵ�� 0-140000000hz
** 			  		Freq_High:��ֹƵ��; 0-140000000hz
**						UpStepFreq������ɨƵ������ 0-140000000hz
**						UpStepTime������ɨƵ�Ĳ������ʱ��;1-255
**            DownStepFreq������ɨƵ������ 0-140000000hz
**						DownStepTime������ɨƵ�Ĳ������ʱ��;1-255
**						mode:����ɨ����ͣ������,No_Dwell��ͣ�������ɨƵ����ֹƵ�ʻص���ʼƵ�ʣ�Dwellͣ�������ɨƵ����ֹƵ�ʺ󱣳�����ֹƵ�ʡ�
** ���ڲ��� ����
** ����˵�� ����Ҫ��֤��Freq_Low<Freq_High
**						�������ʱ�� T = StepTime*10 ������UpStepTime=100��������ɨƵ�Ĳ������ʱ��T=1000nS=1us
**						ɨƵ��ʱ��=��ɨ��Ƶ����*T
**						PS0�ſ���ɨƵ����PS0=1����ɨƵ��PS0=0����ɨƵ
*********************************************************************************************************/
void AD9954_Set_LinearSweep(double Freq_Low,double Freq_High,double  UpStepFreq, uint8_t UpStepTime,double	DownStepFreq, uint8_t DownStepTime,uint8_t mode)//linear sweep mode
{	
	uint8_t date[5] ={0x00,0x00,0x00,0x00,0x00};	//�м����
	uint8_t CFR1_data[4] ={0x00,0x00,0x00,0x00};
	uint32_t Temp=0; 
	
	PS1=0;PS0=0;
	AD9954_CS=0;
	
	CFR1_data[0]=0x00;//31-24
	CFR1_data[1]=0x20;//23-16
	CFR1_data[2]=0x00;
	CFR1_data[3]=0x00|mode;//�Ƚ�������,�������;0x40,�Ƚ������÷��������
	AD9954_Write_nByte(CFR1,CFR1_data,4);//����д����ƹ��ܼĴ���1
	
	Temp=Get_FTW(Freq_Low);
	date[0] =(uint8_t)(Temp >> 24);
	date[1] =(uint8_t)(Temp >> 16);
	date[2] =(uint8_t)(Temp >> 8);
	date[3] =(uint8_t)Temp;
	AD9954_Write_nByte(FTW0,date,4);//дƵ�ʿ�����
	
	Temp=Get_FTW(Freq_High);
	date[0] =(uint8_t)(Temp >> 24);
	date[1] =(uint8_t)(Temp >> 16);
	date[2] =(uint8_t)(Temp >> 8);
	date[3] =(uint8_t)Temp;
	AD9954_Write_nByte(FTW1,date,4);//дƵ�ʿ�����
	
	Temp=Get_FTW(DownStepFreq);
	date[0] =DownStepTime;//�½���ɨ��б��
	date[1] =(uint8_t)(Temp >> 24);
	date[2] =(uint8_t)(Temp >> 16);
	date[3] =(uint8_t)(Temp >> 8);
	date[4] =(uint8_t)Temp;//�½���Ƶ������
	AD9954_Write_nByte(NLSCW,date,5);
	
	Temp=Get_FTW(UpStepFreq);
	date[0] =UpStepTime;//������ɨ��б��
	date[1] =(uint8_t)(Temp >> 24);
	date[2] =(uint8_t)(Temp >> 16);
	date[3] =(uint8_t)(Temp >> 8);
	date[4] =(uint8_t)Temp;//������Ƶ������
	AD9954_Write_nByte(PLSCW,date,5);
	
	AD9954_CS=1;
	UPDATE();
}





