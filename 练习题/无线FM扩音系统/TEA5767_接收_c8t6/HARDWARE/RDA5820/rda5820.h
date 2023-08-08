#ifndef __TEA5767_H
#define __TEA5767_H
#include "stm32f10x.h"
#define max_freq 108000
#define min_freq 87500
void TEA5767_Init(void);
void TEA5767_WriteReg(u8* data);
void TEA5767_ReadReg(u8* data);
void Handle_search(int handle_mode,int step);

void Auto_search(int mode);
u16 TEA5767_Get_pll(u8* data);
u16 TEA5767_Calc_pll(u32 frequency);
u32 TEA5767_Calc_freq(u16 pll);
u8 TEA5767_Set_Frequency(u32 fre);
u8 auto_search(u8 mode);
#endif
