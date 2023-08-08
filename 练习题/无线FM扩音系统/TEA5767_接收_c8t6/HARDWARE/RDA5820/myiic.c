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
//初始化IIC
void IIC_Init(void)
{
    u16 pin = (1<<SCL_PIN)|(1<<SDA_PIN);
 	RCC->APB2ENR|=1<<3;//先使能外设IO PORTB时钟
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIOB->ODR|=pin;     // SCL SDA输出高
}

# define TIMEBASE  10    // 调试用，调为500+可以看I2C波形

//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA(SDA_PIN)=1;
	IIC_SCL(SCL_PIN)=1;
	delay_us(4*TIMEBASE);
 	IIC_SDA(SDA_PIN)=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4*TIMEBASE);
	IIC_SCL(SCL_PIN)=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL(SCL_PIN)=0;
	IIC_SDA(SDA_PIN)=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4*TIMEBASE);
	IIC_SCL(SCL_PIN)=1; 
	IIC_SDA(SDA_PIN)=1;//发送I2C总线结束信号
	delay_us(4*TIMEBASE);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u16 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
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
	IIC_SCL(SCL_PIN)=0;//时钟输出0
	return 0;  
} 
//产生ACK应答
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
//不产生ACK应答
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
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL(SCL_PIN)=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA(SDA_PIN)=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2*TIMEBASE);   //对TEA5767这三个延时都是必须的
		IIC_SCL(SCL_PIN)=1;
		delay_us(2*TIMEBASE);
		IIC_SCL(SCL_PIN)=0;
		delay_us(2*TIMEBASE);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
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
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}
