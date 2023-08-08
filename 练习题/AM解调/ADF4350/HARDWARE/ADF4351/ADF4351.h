#ifndef _ADF4351_H_
#define _ADF4351_H_
#include "sys.h"

#define ADF4351_CE PCout(12)
#define ADF4351_LE PCout(9)
#define ADF4351_OUTPUT_DATA PCout(10)
#define ADF4351_CLK		PCout(11)

//										无用的管脚
#define ADF4351_INPUT_DATA PCin(10)

#define ADF4351_RF_OFF	((u32)0XEC801C)

void ADF4351Init(void);
void ReadToADF4351(u8 count, u8 *buf);
void WriteToADF4351(u8 count, u8 *buf);
void WriteOneRegToADF4351(u32 Regster);
void ADF4351_Init_some(void);
void ADF4351WriteFreq(float Fre);		//	(xx.x) M Hz

#endif

