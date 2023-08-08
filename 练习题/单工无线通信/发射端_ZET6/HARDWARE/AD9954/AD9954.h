#ifndef __AD9954_H
#define __AD9954_H	 
#include "sys.h"


#define AD9954_CS 	PCout(5)
#define AD9954_SCLK PAout(7)
#define AD9954_SDIO PAout(5)
#define AD9954_OSK 	PAout(1)
#define PS1 				PCout(1)
#define PS0 				PCout(3)
#define IOUPDATE 		PCout(2)

#define AD9954_SDO	PCin(4)
#define AD9954_IOSY PAout(6)
#define AD9954_RES 	PAout(4)
#define AD9954_PWR 	PAout(0)


#define CFR1		0X00	//���ƹ��ܼĴ���1
#define CFR2    0X01	//���ƹ��ܼĴ���2
#define ASF     0X02	//����������ӼĴ���
#define ARR     0X03	//���б�����ʼĴ���

#define FTW0    0X04	//Ƶ�ʵ�г�ּĴ���0
#define POW0    0X05	//��λƫ���ּĴ���
#define FTW1    0X06	//Ƶ�ʵ�г�ּĴ���1

#define NLSCW   0X07	//�½�ɨ������ּĴ���
#define PLSCW   0X08	//����ɨ������ּĴ���

#define RSCW0   0X07	//
#define RSCW1   0X08	//
#define RSCW2   0X09	//
#define RSCW3   0X0A	//RAM�ο����ּĴ���
#define RAM     0X0B	//��ȡָ��д��RAMǩ���Ĵ�������

#define No_Dwell	0x04	//No_Dwell��ͣ�������ɨƵ����ֹƵ�ʻص���ʼƵ�ʡ�
#define Dwell 		0x00	//Dwellͣ�������ɨƵ����ֹƵ�ʺ󱣳�����ֹƵ�ʡ�


void AD9954_GPIO_Init(void);//��ʼ������AD9954��Ҫ�õ���IO��
void AD9954_RESET(void);		//��λAD9954
void UPDATE(void);					//����һ�������źţ�����AD9954�ڲ��Ĵ���

void AD9954_Send_Byte(uint8_t dat);//��AD9954����һ���ֽڵ�����
uint8_t AD9954_Read_Byte(void);			//��AD9954һ���ֽڵ�����
void AD9954_Write_nByte(uint8_t RegAddr,uint8_t *Data,uint8_t Len);//��AD9954ָ���ļĴ���д����
uint32_t AD9954_Read_nByte(uint8_t ReadAddr,uint8_t Len);					//��AD9954�Ĵ�������
uint32_t Get_FTW(double Real_fH);	//Ƶ������ת��


void AD9954_Init(void);//��ʼ��AD9954�Ĺܽź��ڲ��Ĵ��������ã�
void AD9954_Set_Fre(double fre);//����AD9954���Ƶ�ʣ���Ƶ
void AD9954_Set_Amp(uint16_t Ampli);//д����
void AD9954_Set_Phase(uint16_t Phase);//д��λ

void AD9954_SetFSK(double f1,double f2,double f3,double f4,uint16_t Ampli);//AD9954��FSK��������
void AD9954_SetPSK(uint16_t Phase1,uint16_t Phase2,uint16_t Phase3,uint16_t Phase4,double fre,uint16_t Ampli);//AD9954��PSK��������
void AD9954_Set_LinearSweep(double Freq_Low,double Freq_High,double  UpStepFreq, uint8_t UpStepTime,double	DownStepFreq, uint8_t DownStepTime,uint8_t mode);//ɨƵ��������

		

#endif

