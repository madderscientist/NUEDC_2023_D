#include "myiic.h"
#include "delay.h"

u8 SCL_PIN = 10;
u8 SDA_PIN = 11;

void SDA_IN(void){
    if(SDA_PIN>7){
        u8 loc = (SDA_PIN - 8)<<2;
        GPIOB->CRH &= ~(0xF<<loc);
        GPIOB->CRH |= 8<<loc;
    }else{
        u8 loc = SDA_PIN<<2;
        GPIOB->CRL &= ~(0xF<<loc);
        GPIOB->CRL |= 8<<loc;
    }
}
void SDA_OUT(void){
    if(SDA_PIN>7){
        u8 loc = (SDA_PIN - 8)<<2;
        GPIOB->CRH &= ~(0xF<<loc);
        GPIOB->CRH |= 3<<loc;
    }else{
        u8 loc = SDA_PIN<<2;
        GPIOB->CRL &= ~(0xF<<loc);
        GPIOB->CRL |= 3<<loc;
    }
}
//��ʼ��IIC
void IIC_Init(void)
{
    u16 pin = (1<<SCL_PIN)|(1<<SDA_PIN);
 	RCC->APB2ENR|=1<<3;//��ʹ������IO PORTBʱ��
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIOB->ODR|=pin;     // SCL SDA�����
}

# define TIMEBASE  10    // �����ã���Ϊ500+���Կ�I2C����

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA(SDA_PIN)=1;
	IIC_SCL(SCL_PIN)=1;
	delay_us(4*TIMEBASE);
 	IIC_SDA(SDA_PIN)=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4*TIMEBASE);
	IIC_SCL(SCL_PIN)=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL(SCL_PIN)=0;
	IIC_SDA(SDA_PIN)=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4*TIMEBASE);
	IIC_SCL(SCL_PIN)=1; 
	IIC_SDA(SDA_PIN)=1;//����I2C���߽����ź�
	delay_us(4*TIMEBASE);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u16 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA(SDA_PIN)=1;delay_us(1*TIMEBASE);
	IIC_SCL(SCL_PIN)=1;delay_us(1*TIMEBASE);
	while(READ_SDA(SDA_PIN))
	{
		ucErrTime++;
		if(ucErrTime>2500)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(SCL_PIN)=0;//ʱ�����0
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL(SCL_PIN)=0;
	SDA_OUT();
	IIC_SDA(SDA_PIN)=0;
	delay_us(2*TIMEBASE);
	IIC_SCL(SCL_PIN)=1;
	delay_us(2*TIMEBASE);
	IIC_SCL(SCL_PIN)=0;
}
//������ACKӦ��
void IIC_NAck(void)
{
	IIC_SCL(SCL_PIN)=0;
	SDA_OUT();
	IIC_SDA(SDA_PIN)=1;
	delay_us(2*TIMEBASE);
	IIC_SCL(SCL_PIN)=1;
	delay_us(2*TIMEBASE);
	IIC_SCL(SCL_PIN)=0;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL(SCL_PIN)=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA(SDA_PIN)=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2*TIMEBASE);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL(SCL_PIN)=1;
		delay_us(2*TIMEBASE);
		IIC_SCL(SCL_PIN)=0;
		delay_us(2*TIMEBASE);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL(SCL_PIN)=0;
        delay_us(2*TIMEBASE);
		IIC_SCL(SCL_PIN)=1;
        receive<<=1;
        if(READ_SDA(SDA_PIN))receive++;
		delay_us(1*TIMEBASE);
    }
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}
