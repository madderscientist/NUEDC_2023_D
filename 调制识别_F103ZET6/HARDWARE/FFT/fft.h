#ifndef FFT_H_
#define FFT_H_
#include "sys.h"

#define NPT 1024
#define PI2 6.28318530717959
# define abs(x) x>0?x:-x

void GetPowerMag(void);
void statistics(void);
u8 getdf(u16 kH, u16 FFTvalue);

#endif









