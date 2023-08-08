#include "stm32f10x.h"
#include "AD9954.h"

#define StimuSerialOut PAout(9)
#define StimuSerialOut0 AD9954_Set_Fre(0*1000000)
#define StimuSerialOut1 AD9954_Set_Fre(50*1000000)

void stimuSerial_Init(uint16_t BaudRate, uint8_t parity, uint8_t stopbits);
void stimuSerial_SendByte(uint8_t Byte);
